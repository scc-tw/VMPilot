#!/usr/bin/env python3
"""py2vmp — Python subset → VMPilot VM compiler.

Compiles a restricted Python subset into a C++ header file containing
VMPilot VM basic-block definitions (TestBB structs) ready for
build_test_blob().

Usage:
    python py2vmp.py snake_logic.py -o snake_vm.gen.hpp

The input file must define these module-level dicts (read via AST, not
executed):
    REGISTER_MAP  — {name: register_number}
    CONST_REGS    — {name: (register_number, value)}
    TEMP_REGS     — [register_number, ...]
    NATIVE_MAP    — {function_name: slot_index}
"""

from __future__ import annotations

import argparse
import ast
import sys
import textwrap
from dataclasses import dataclass, field
from typing import Dict, List, Optional, Set, Tuple

# ─── VM constants ────────────────────────────────────────────────────────

VM_OPERAND_NONE = 0
VM_OPERAND_REG  = 1
VM_OPERAND_POOL = 2
VM_OPERAND_MEM  = 3

# Condition codes (must match handler_impls.hpp evaluate_condition)
CC_EQ = 0   # ZF
CC_NE = 1   # !ZF
CC_LT = 2   # SF!=OF   (signed <)
CC_GE = 3   # SF==OF   (signed >=)
CC_LE = 4   # ZF||(SF!=OF)  (signed <=)
CC_GT = 5   # !ZF&&(SF==OF) (signed >)

NEGATE_CC = {CC_EQ: CC_NE, CC_NE: CC_EQ,
             CC_LT: CC_GE, CC_GE: CC_LT,
             CC_LE: CC_GT, CC_GT: CC_LE}

_COMPARE_TO_CC = {
    ast.Eq:    CC_EQ, ast.NotEq: CC_NE,
    ast.Lt:    CC_LT, ast.GtE:   CC_GE,
    ast.LtE:  CC_LE, ast.Gt:    CC_GT,
}


def _flags(op_a: int, op_b: int, cond: int = 0) -> int:
    return (op_a << 6) | (op_b << 4) | (cond & 0xF)

F_NONE = _flags(VM_OPERAND_NONE, VM_OPERAND_NONE)
F_RR   = _flags(VM_OPERAND_REG,  VM_OPERAND_REG)
F_POOL = _flags(VM_OPERAND_POOL, VM_OPERAND_NONE)
F_R    = _flags(VM_OPERAND_REG,  VM_OPERAND_NONE)

# ─── IR ──────────────────────────────────────────────────────────────────

@dataclass
class Insn:
    opcode:  str
    flags:   int  = 0
    reg_a:   int  = 0
    reg_b:   int  = 0
    aux:     int  = 0
    comment: str  = ""

@dataclass
class BB:
    bb_id: int
    insns: List[Insn] = field(default_factory=list)

@dataclass
class NativeSite:
    global_ip: int        # filled in during finalize()
    arg_count: int
    slot:      int        # NATIVE_MAP slot

# ─── Config ──────────────────────────────────────────────────────────────

@dataclass
class Config:
    register_map: Dict[str, int]        = field(default_factory=dict)
    const_regs:   Dict[str, Tuple[int, int]] = field(default_factory=dict)
    temp_regs:    List[int]             = field(default_factory=list)
    native_map:   Dict[str, int]        = field(default_factory=dict)

def _parse_config(tree: ast.Module) -> Config:
    cfg = Config()
    for node in tree.body:
        if not isinstance(node, ast.Assign) or len(node.targets) != 1:
            continue
        tgt = node.targets[0]
        if not isinstance(tgt, ast.Name):
            continue
        name = tgt.id
        try:
            val = ast.literal_eval(node.value)
        except (ValueError, TypeError):
            continue
        if   name == "REGISTER_MAP": cfg.register_map = val
        elif name == "CONST_REGS":   cfg.const_regs   = val
        elif name == "TEMP_REGS":    cfg.temp_regs    = val
        elif name == "NATIVE_MAP":   cfg.native_map   = val
    return cfg

# ─── Constant resolver ───────────────────────────────────────────────────

_CONFIG_NAMES = frozenset(["REGISTER_MAP", "CONST_REGS", "TEMP_REGS", "NATIVE_MAP"])

def _eval_const(node: ast.expr, consts: Dict[str, int]) -> int:
    if isinstance(node, ast.Constant) and isinstance(node.value, int):
        return node.value
    if isinstance(node, ast.Name) and node.id in consts:
        return consts[node.id]
    if isinstance(node, ast.UnaryOp) and isinstance(node.op, ast.USub):
        return -_eval_const(node.operand, consts)
    if isinstance(node, ast.BinOp):
        l = _eval_const(node.left, consts)
        r = _eval_const(node.right, consts)
        op = type(node.op)
        if op is ast.Add:      return l + r
        if op is ast.Sub:      return l - r
        if op is ast.Mult:     return l * r
        if op is ast.FloorDiv: return l // r
        if op is ast.Mod:      return l % r
    raise ValueError

def _resolve_constants(tree: ast.Module, cfg: Config) -> Dict[str, int]:
    """Resolve module-level UPPER_CASE assignments as compile-time constants.

    Only names that are fully uppercase (e.g. GW, GH) qualify — lowercase
    names like ``status`` are runtime variables even if assigned at the
    top level.
    """
    consts: Dict[str, int] = {}
    for node in tree.body:
        if not isinstance(node, ast.Assign) or len(node.targets) != 1:
            continue
        tgt = node.targets[0]
        if not isinstance(tgt, ast.Name):
            continue
        name = tgt.id
        if not name.isupper():
            continue   # only UPPER_CASE → constant
        if name in _CONFIG_NAMES or name in cfg.register_map:
            continue
        try:
            consts[name] = _eval_const(node.value, consts)
        except (ValueError, TypeError):
            pass
    return consts

# ─── Register allocator ─────────────────────────────────────────────────

class RegAlloc:
    """Simple register allocator.

    *   r0 is **reserved** for native-call argument setup / return value.
        It never enters the local or temp pool.
    *   Temp pool contains registers from TEMP_REGS only.  Locals are
        drawn from this pool and returned when freed.
    *   Global registers (REGISTER_MAP) and constant registers (CONST_REGS)
        are immutable and never in the pool.
    """

    # Registers that must never be in the temp pool.
    _RESERVED = frozenset({0})

    def __init__(self, cfg: Config):
        self.globals   = dict(cfg.register_map)
        self.const_reg = {n: r for n, (r, _) in cfg.const_regs.items()}
        self.const_val = {n: v for n, (_, v) in cfg.const_regs.items()}
        self._val_to_const: Dict[int, int] = {v: r for _, (r, v) in cfg.const_regs.items()}
        # Pool never contains reserved, global, or const registers.
        self.pool: List[int] = [r for r in cfg.temp_regs
                                if r not in self._RESERVED]
        self.locals: Dict[str, int] = {}

    def resolve(self, name: str) -> Optional[int]:
        if name in self.globals:   return self.globals[name]
        if name in self.const_reg: return self.const_reg[name]
        return self.locals.get(name)

    def const_for_value(self, v: int) -> Optional[int]:
        return self._val_to_const.get(v)

    def alloc_temp(self) -> int:
        if not self.pool:
            raise RuntimeError("out of temp registers")
        return self.pool.pop(0)

    def alloc_temp_avoiding(self, avoid: Set[int]) -> int:
        """Allocate a temp that is NOT in *avoid*."""
        for i, r in enumerate(self.pool):
            if r not in avoid:
                return self.pool.pop(i)
        raise RuntimeError(f"no temp register outside {avoid}")

    def free_temp(self, r: int) -> None:
        if r in self._RESERVED:
            return                     # never re-pool reserved regs
        if r in self.pool:
            return                     # already free
        if r in self.globals.values() or r in self.const_reg.values():
            return                     # immutable
        self.pool.insert(0, r)

    def alloc_local(self, name: str) -> int:
        if name in self.locals:
            return self.locals[name]
        r = self.alloc_temp()
        self.locals[name] = r
        return r

    def free_local(self, name: str) -> None:
        r = self.locals.pop(name, None)
        if r is not None:
            self.free_temp(r)

    def snapshot(self) -> Dict[str, int]:
        return dict(self.locals)

    def free_since(self, snap: Dict[str, int]) -> None:
        for name in list(self.locals):
            if name not in snap:
                self.free_local(name)

# ─── Compiler ────────────────────────────────────────────────────────────

class Compiler:
    def __init__(self, cfg: Config, consts: Dict[str, int]):
        self.cfg     = cfg
        self.consts  = consts
        self.ra      = RegAlloc(cfg)
        self.bbs:    List[BB] = []
        self.nsites: List[NativeSite] = []
        self.pool:   List[int] = []
        self._pool_map: Dict[int, int] = {}
        self._next_bb = 1
        self._break_targets:    List[int] = []
        self._continue_targets: List[int] = []
        self._native_idx = 0

    # ── BB management ────────────────────────────────────────────────

    def _new_bb(self) -> BB:
        bb = BB(bb_id=self._next_bb)
        self._next_bb += 1
        self.bbs.append(bb)
        return bb

    def _reserve_id(self) -> int:
        n = self._next_bb; self._next_bb += 1; return n

    def _make_bb(self, bb_id: int) -> BB:
        bb = BB(bb_id=bb_id); self.bbs.append(bb); return bb

    # ── Emitters ─────────────────────────────────────────────────────

    def _emit(self, bb: BB, insn: Insn) -> None:
        bb.insns.append(insn)

    def _emit_move(self, bb: BB, dst: int, src: int) -> None:
        if dst != src:
            self._emit(bb, Insn("MOVE", F_RR, dst, src))

    def _emit_jmp(self, bb: BB, target: int) -> None:
        self._emit(bb, Insn("JMP", F_NONE, 0, 0, target))

    def _emit_jcc(self, bb: BB, cc: int, target: int) -> None:
        self._emit(bb, Insn("JCC", _flags(VM_OPERAND_NONE, VM_OPERAND_NONE, cc), 0, 0, target))

    def _emit_cmp(self, bb: BB, a: int, b: int) -> None:
        self._emit(bb, Insn("CMP", F_RR, a, b))

    def _emit_halt(self, bb: BB) -> None:
        self._emit(bb, Insn("HALT"))

    def _pool_idx(self, value: int) -> int:
        if value not in self._pool_map:
            idx = len(self.pool)
            self.pool.append(value)
            self._pool_map[value] = idx
        return self._pool_map[value]

    def _emit_load_const(self, bb: BB, reg: int, value: int) -> None:
        idx = self._pool_idx(value)
        self._emit(bb, Insn("LOAD_CONST", F_POOL, reg, 0, idx, f"= {value}"))

    def _emit_native(self, bb: BB, name: str, argc: int) -> None:
        slot = self.cfg.native_map[name]
        idx  = self._native_idx
        self._native_idx += 1
        self._emit(bb, Insn("NATIVE_CALL", F_NONE, 0, 0, idx, name))
        self.nsites.append(NativeSite(global_ip=-1, arg_count=argc, slot=slot))

    # ── Expression compilation ───────────────────────────────────────
    # Returns (register, is_temp).  Caller frees temps.

    def _expr(self, node: ast.expr, bb: BB) -> Tuple[int, bool]:
        # Integer literal
        if isinstance(node, ast.Constant) and isinstance(node.value, int):
            return self._load_int(node.value, bb)

        # Variable reference
        if isinstance(node, ast.Name):
            return self._load_name(node.id, bb)

        # Binary operation
        if isinstance(node, ast.BinOp):
            return self._binop(node, bb)

        # Unary minus
        if isinstance(node, ast.UnaryOp) and isinstance(node.op, ast.USub):
            r, t = self._expr(node.operand, bb)
            if not t:
                tmp = self.ra.alloc_temp()
                self._emit_move(bb, tmp, r)
                r, t = tmp, True
            self._emit(bb, Insn("NEG", F_R, r))
            return r, True

        # Function call
        if isinstance(node, ast.Call):
            return self._call(node, bb)

        raise NotImplementedError(f"unsupported expr: {ast.dump(node)}")

    def _load_int(self, v: int, bb: BB) -> Tuple[int, bool]:
        cr = self.ra.const_for_value(v)
        if cr is not None:
            return cr, False
        tmp = self.ra.alloc_temp()
        self._emit_load_const(bb, tmp, v)
        return tmp, True

    def _load_name(self, name: str, bb: BB) -> Tuple[int, bool]:
        if name in self.consts:
            return self._load_int(self.consts[name], bb)
        r = self.ra.resolve(name)
        if r is not None:
            return r, False
        raise NameError(f"undefined variable: {name}")

    _OP_MAP = {
        ast.Add: "ADD", ast.Sub: "SUB", ast.Mult: "MUL",
        ast.FloorDiv: "DIV", ast.Mod: "MOD",
    }

    def _binop(self, node: ast.BinOp, bb: BB) -> Tuple[int, bool]:
        lr, lt = self._expr(node.left, bb)
        rr, rt = self._expr(node.right, bb)
        if not lt:
            tmp = self.ra.alloc_temp()
            self._emit_move(bb, tmp, lr)
            lr, lt = tmp, True
        opc = self._OP_MAP.get(type(node.op))
        if opc is None:
            raise NotImplementedError(type(node.op).__name__)
        self._emit(bb, Insn(opc, F_RR, lr, rr))
        if rt:
            self.ra.free_temp(rr)
        return lr, True

    def _call(self, node: ast.Call, bb: BB) -> Tuple[int, bool]:
        """Compile a native function call.

        Uses a clean save → place → call → restore pattern:
          1. Evaluate argument expressions.
          2. Save locals whose registers will be overwritten (arg range).
          3. Move arg values into r0 … r(argc-1).
          4. Emit NATIVE_CALL.
          5. Restore saved locals.
        """
        assert isinstance(node.func, ast.Name)
        name = node.func.id
        if name not in self.cfg.native_map:
            raise ValueError(f"unknown native: {name}")
        argc = len(node.args)
        arg_range = set(range(argc))     # registers that will be overwritten

        # ── 1. Evaluate all argument expressions ────────────────────
        arg_vals: List[Tuple[int, bool]] = []
        for a in node.args:
            r, t = self._expr(a, bb)
            arg_vals.append((r, t))

        # ── 2. Save locals in the arg range to safe registers ───────
        saved: List[Tuple[str, int, int]] = []     # (name, orig, safe)
        for lname, lr in list(self.ra.locals.items()):
            if lr in arg_range:
                safe = self.ra.alloc_temp_avoiding(arg_range)
                self._emit_move(bb, safe, lr)
                saved.append((lname, lr, safe))
                self.ra.locals[lname] = safe

        # Update any arg_val that pointed at a register we just evacuated.
        relocated = {orig: safe for _, orig, safe in saved}
        for i, (r, t) in enumerate(arg_vals):
            if not t and r in relocated:
                arg_vals[i] = (relocated[r], False)

        # ── 3. Place args into r0 … r(argc-1) ──────────────────────
        # After step 2 no local sits inside arg_range, so the only
        # conflict left is an arg value that sits in a *different*
        # arg's target register.  Evacuate those first.
        for i, (r, t) in enumerate(arg_vals):
            if r in arg_range and r != i:
                safe = self.ra.alloc_temp_avoiding(arg_range)
                self._emit_move(bb, safe, r)
                if t:
                    self.ra.free_temp(r)
                arg_vals[i] = (safe, True)

        # Now every source is either outside arg_range or already in
        # its correct position — plain sequential MOVEs are safe.
        for i, (r, t) in enumerate(arg_vals):
            self._emit_move(bb, i, r)
            if t and r != i:
                self.ra.free_temp(r)

        # ── 4. Emit the native call ─────────────────────────────────
        self._emit_native(bb, name, argc)

        # ── 5. Restore saved locals ─────────────────────────────────
        for lname, orig, safe in saved:
            self._emit_move(bb, orig, safe)
            self.ra.free_temp(safe)
            self.ra.locals[lname] = orig

        return 0, True   # return value in r0

    # ── Condition compilation ────────────────────────────────────────
    # Emit CMP+JCC chain; jumps to true_id / false_id.

    def _cond_jump(self, node: ast.expr, bb: BB, true_id: int, false_id: int) -> BB:
        if isinstance(node, ast.Compare):
            return self._compare_jump(node, bb, true_id, false_id)
        if isinstance(node, ast.BoolOp):
            if isinstance(node.op, ast.And):
                return self._and_jump(node, bb, true_id, false_id)
            return self._or_jump(node, bb, true_id, false_id)
        if isinstance(node, ast.Constant):
            self._emit_jmp(bb, true_id if node.value else false_id)
            return bb
        # General expr: compare to zero
        r, t = self._expr(node, bb)
        zr = self.ra.const_for_value(0)
        assert zr is not None
        self._emit_cmp(bb, r, zr)
        self._emit_jcc(bb, CC_NE, true_id)
        self._emit_jmp(bb, false_id)
        if t: self.ra.free_temp(r)
        return bb

    def _compare_jump(self, node: ast.Compare, bb: BB,
                      true_id: int, false_id: int) -> BB:
        assert len(node.ops) == 1
        lr, lt = self._expr(node.left, bb)
        rr, rt = self._expr(node.comparators[0], bb)
        cc = _COMPARE_TO_CC[type(node.ops[0])]
        self._emit_cmp(bb, lr, rr)
        self._emit_jcc(bb, cc, true_id)
        self._emit_jmp(bb, false_id)
        if lt: self.ra.free_temp(lr)
        if rt: self.ra.free_temp(rr)
        return bb

    def _and_jump(self, node: ast.BoolOp, bb: BB, true_id: int, false_id: int) -> BB:
        for i, val in enumerate(node.values):
            if i < len(node.values) - 1:
                nxt = self._new_bb()
                self._cond_jump(val, bb, nxt.bb_id, false_id)
                bb = nxt
            else:
                self._cond_jump(val, bb, true_id, false_id)
        return bb

    def _or_jump(self, node: ast.BoolOp, bb: BB, true_id: int, false_id: int) -> BB:
        for i, val in enumerate(node.values):
            if i < len(node.values) - 1:
                nxt = self._new_bb()
                self._cond_jump(val, bb, true_id, nxt.bb_id)
                bb = nxt
            else:
                self._cond_jump(val, bb, true_id, false_id)
        return bb

    # ── Statement compilation ────────────────────────────────────────

    def _body(self, stmts: List[ast.stmt], bb: BB) -> BB:
        for s in stmts:
            bb = self._stmt(s, bb)
        return bb

    def _stmt(self, node: ast.stmt, bb: BB) -> BB:
        if isinstance(node, ast.Assign):   return self._assign(node, bb)
        if isinstance(node, ast.AugAssign):return self._aug_assign(node, bb)
        if isinstance(node, ast.If):       return self._if(node, bb)
        if isinstance(node, ast.While):    return self._while(node, bb)
        if isinstance(node, ast.Expr):
            if isinstance(node.value, ast.Call):
                return self._call_stmt(node.value, bb)
            if isinstance(node.value, ast.Constant) and isinstance(node.value.value, str):
                return bb  # docstring / string expression — skip
            raise NotImplementedError(f"unsupported expr stmt: {ast.dump(node.value)}")
        if isinstance(node, ast.Break):    return self._break(bb)
        if isinstance(node, ast.Delete):   return self._del(node, bb)
        if isinstance(node, ast.Pass):     return bb
        raise NotImplementedError(f"unsupported stmt: {type(node).__name__}")

    def _assign(self, node: ast.Assign, bb: BB) -> BB:
        assert len(node.targets) == 1
        tgt = node.targets[0]
        assert isinstance(tgt, ast.Name)
        name = tgt.id
        if name in _CONFIG_NAMES or name in self.consts:
            return bb  # compile-time only

        vr, vt = self._expr(node.value, bb)
        tr = self.ra.resolve(name)
        if tr is not None:
            self._emit_move(bb, tr, vr)
            if vt and vr != tr:
                self.ra.free_temp(vr)
        else:
            # New local — reuse the temp register if it is a safe one
            # (i.e. not r0, which is reserved for native-call plumbing).
            if vt and vr not in RegAlloc._RESERVED:
                self.ra.locals[name] = vr
                if vr in self.ra.pool:
                    self.ra.pool.remove(vr)
            else:
                lr = self.ra.alloc_local(name)
                self._emit_move(bb, lr, vr)
                if vt:
                    self.ra.free_temp(vr)
        return bb

    def _aug_assign(self, node: ast.AugAssign, bb: BB) -> BB:
        assert isinstance(node.target, ast.Name)
        name = node.target.id
        tr = self.ra.resolve(name)
        if tr is None:
            raise NameError(f"undefined: {name}")
        rr, rt = self._expr(node.value, bb)
        opc = self._OP_MAP.get(type(node.op))
        if opc is None:
            raise NotImplementedError(type(node.op).__name__)
        # If target is a const reg, we must not modify it
        if tr in self.ra.const_reg.values():
            raise RuntimeError(f"cannot augassign to const register r{tr}")
        self._emit(bb, Insn(opc, F_RR, tr, rr))
        if rt: self.ra.free_temp(rr)
        return bb

    def _if(self, node: ast.If, bb: BB) -> BB:
        snap = self.ra.snapshot()
        merge_id = self._reserve_id()
        then_bb  = self._new_bb()

        if node.orelse:
            else_bb = self._new_bb()
            self._cond_jump(node.test, bb, then_bb.bb_id, else_bb.bb_id)
            then_exit = self._body(node.body, then_bb)
            self._emit_jmp(then_exit, merge_id)
            if len(node.orelse) == 1 and isinstance(node.orelse[0], ast.If):
                elif_exit = self._if(node.orelse[0], else_bb)
                self._emit_jmp(elif_exit, merge_id)
            else:
                else_exit = self._body(node.orelse, else_bb)
                self._emit_jmp(else_exit, merge_id)
        else:
            self._cond_jump(node.test, bb, then_bb.bb_id, merge_id)
            then_exit = self._body(node.body, then_bb)
            self._emit_jmp(then_exit, merge_id)

        self.ra.free_since(snap)
        return self._make_bb(merge_id)

    def _while(self, node: ast.While, bb: BB) -> BB:
        snap      = self.ra.snapshot()
        header_bb = self._new_bb()
        body_bb   = self._new_bb()
        exit_id   = self._reserve_id()

        self._break_targets.append(exit_id)
        self._continue_targets.append(header_bb.bb_id)
        self._emit_jmp(bb, header_bb.bb_id)

        is_true = isinstance(node.test, ast.Constant) and node.test.value is True
        if is_true:
            self._emit_jmp(header_bb, body_bb.bb_id)
        else:
            self._cond_jump(node.test, header_bb, body_bb.bb_id, exit_id)

        body_exit = self._body(node.body, body_bb)

        # Free body-scoped locals
        self.ra.free_since(snap)

        self._emit_jmp(body_exit, header_bb.bb_id)

        self._break_targets.pop()
        self._continue_targets.pop()
        return self._make_bb(exit_id)

    def _call_stmt(self, node: ast.Call, bb: BB) -> BB:
        r, t = self._call(node, bb)
        if t: self.ra.free_temp(r)
        return bb

    def _break(self, bb: BB) -> BB:
        if not self._break_targets:
            raise SyntaxError("break outside loop")
        self._emit_jmp(bb, self._break_targets[-1])
        return self._new_bb()  # unreachable continuation

    def _del(self, node: ast.Delete, bb: BB) -> BB:
        for tgt in node.targets:
            if isinstance(tgt, ast.Name):
                self.ra.free_local(tgt.id)
        return bb

    # ── Top-level ────────────────────────────────────────────────────

    def compile(self, tree: ast.Module) -> None:
        stmts = [n for n in tree.body
                 if not (isinstance(n, ast.Assign)
                         and len(n.targets) == 1
                         and isinstance(n.targets[0], ast.Name)
                         and (n.targets[0].id in _CONFIG_NAMES
                              or n.targets[0].id in self.consts))]
        entry = self._new_bb()
        exit_bb = self._body(stmts, entry)
        self._emit_halt(exit_bb)
        self._finalize()

    def _finalize(self) -> None:
        idx = 0
        gip = 0
        for bb in self.bbs:
            for insn in bb.insns:
                if insn.opcode == "NATIVE_CALL":
                    self.nsites[idx].global_ip = gip
                    idx += 1
                gip += 1

    # ── Stats ────────────────────────────────────────────────────────

    def stats(self) -> str:
        ti = sum(len(b.insns) for b in self.bbs)
        return (f"{len(self.bbs)} BBs, {ti} instructions, "
                f"{len(self.pool)} pool entries, {len(self.nsites)} native sites")

# ─── C++ code generator ─────────────────────────────────────────────────

_OPC = {
    "MOVE": "VmOpcode::MOVE", "LOAD": "VmOpcode::LOAD",
    "STORE": "VmOpcode::STORE", "PUSH": "VmOpcode::PUSH",
    "POP": "VmOpcode::POP", "LOAD_CONST": "VmOpcode::LOAD_CONST",
    "ADD": "VmOpcode::ADD", "SUB": "VmOpcode::SUB",
    "MUL": "VmOpcode::MUL", "DIV": "VmOpcode::DIV",
    "MOD": "VmOpcode::MOD", "NEG": "VmOpcode::NEG",
    "AND": "VmOpcode::AND", "OR": "VmOpcode::OR",
    "XOR": "VmOpcode::XOR", "NOT": "VmOpcode::NOT",
    "SHL": "VmOpcode::SHL", "SHR": "VmOpcode::SHR",
    "CMP": "VmOpcode::CMP", "TEST": "VmOpcode::TEST",
    "JMP": "VmOpcode::JMP", "JCC": "VmOpcode::JCC",
    "NATIVE_CALL": "VmOpcode::NATIVE_CALL",
    "HALT": "VmOpcode::HALT", "NOP": "VmOpcode::NOP",
}


def generate_hpp(comp: Compiler, cfg: Config) -> str:
    o: List[str] = []
    W = o.append

    W("// snake_vm.gen.hpp — auto-generated by py2vmp.py from snake_logic.py")
    W("// DO NOT EDIT — regenerate with:  python py2vmp.py snake_logic.py -o snake_vm.gen.hpp")
    W("//")
    W(f"// {comp.stats()}")
    W("#pragma once")
    W("")
    W('#include "test_blob_builder.hpp"')
    W("#include <vm/vm_opcode.hpp>")
    W("#include <vm/vm_insn.hpp>")
    W("#include <cstdint>")
    W("#include <vector>")
    W("")
    W("namespace SnakeVM {")
    W("")
    W("using VMPilot::Common::VM::VmOpcode;")
    W("using VMPilot::Test::TestBB;")
    W("using VMPilot::Test::TestNativeCall;")
    W("")

    # Register map comment
    W("// ── Register map ──")
    for name, reg in sorted(cfg.register_map.items(), key=lambda x: x[1]):
        W(f"//   r{reg:2d} = {name}")
    for name, (reg, val) in sorted(cfg.const_regs.items(), key=lambda x: x[1][0]):
        W(f"//   r{reg:2d} = {name} ({val})")
    W(f"//   temps: {cfg.temp_regs}")
    W("")

    # Native slot enum
    W("enum NativeSlot : uint32_t {")
    for name, slot in sorted(cfg.native_map.items(), key=lambda x: x[1]):
        enum_name = name.upper()
        W(f"    {enum_name} = {slot},")
    W(f"    NATIVE_COUNT = {len(cfg.native_map)},")
    W("};")
    W("")

    # Constant pool
    W("inline const std::vector<uint64_t>& pool() {")
    W("    static const std::vector<uint64_t> p = {")
    for i, v in enumerate(comp.pool):
        W(f"        {v}ULL,  // [{i}]")
    W("    };")
    W("    return p;")
    W("}")
    W("")

    # NativeSite
    W("struct NativeSite {")
    W("    uint32_t global_ip;")
    W("    uint32_t arg_count;")
    W("    uint32_t slot;")
    W("};")
    W("")

    # Program struct
    W("struct Program {")
    W("    std::vector<TestBB> bbs;")
    W("    std::vector<NativeSite> native_sites;")
    W("};")
    W("")

    # build() function
    W("inline Program build() {")
    W("    Program prog;")
    W("")

    for bb in comp.bbs:
        W(f"    // ── BB {bb.bb_id}  ({len(bb.insns)} insns) ──")
        W( "    {")
        W( "        TestBB bb{};")
        W(f"        bb.bb_id = {bb.bb_id}; bb.epoch = 0;")
        W( "        bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;")
        W(f"        for (int i = 0; i < 32; ++i)")
        W(f"            bb.epoch_seed[i] = static_cast<uint8_t>({bb.bb_id} * 37 + i);")
        W( "        bb.instructions = {")
        for insn in bb.insns:
            cop = _OPC.get(insn.opcode, f"/* {insn.opcode} */")
            cmt = f"  // {insn.comment}" if insn.comment else ""
            W(f"            {{{cop}, {insn.flags}, {insn.reg_a}, {insn.reg_b}, "
              f"{insn.aux}}},{cmt}")
        W( "        };")
        W( "        prog.bbs.push_back(std::move(bb));")
        W( "    }")
        W("")

    # native sites
    W("    prog.native_sites = {")
    for ns in comp.nsites:
        W(f"        {{{ns.global_ip}, {ns.arg_count}, {ns.slot}}},")
    W("    };")
    W("")
    W("    return prog;")
    W("}")
    W("")
    W("} // namespace SnakeVM")
    return "\n".join(o) + "\n"

# ─── Main ────────────────────────────────────────────────────────────────

def main() -> None:
    ap = argparse.ArgumentParser(description="Python subset → VMPilot VM compiler")
    ap.add_argument("input", help="input .py file")
    ap.add_argument("-o", "--output", default="snake_vm.gen.hpp",
                    help="output C++ header")
    args = ap.parse_args()

    with open(args.input, encoding="utf-8") as f:
        source = f.read()

    tree = ast.parse(source, filename=args.input)
    cfg  = _parse_config(tree)
    csts = _resolve_constants(tree, cfg)

    comp = Compiler(cfg, csts)
    comp.compile(tree)

    hpp = generate_hpp(comp, cfg)
    with open(args.output, "w", encoding="utf-8", newline="\n") as f:
        f.write(hpp)

    print(f"py2vmp: {comp.stats()}")
    print(f"py2vmp: wrote {args.output}")


if __name__ == "__main__":
    main()
