/// @file vm_runner.cpp
/// @brief VmRunner + StepController template method implementations + explicit instantiations.
///
/// Extracted from vm_runner.hpp.  Template methods are defined here and
/// explicitly instantiated for all known (Policy, Oram) combinations.

#include "vm_runner.hpp"

namespace VMPilot::Runtime {

// ═════════════════════════════════════════════════════════════════════════════
// StepController methods
// ═════════════════════════════════════════════════════════════════════════════

template<typename Policy, typename Oram>
tl::expected<VmResult, DiagnosticCode> StepController<Policy, Oram>::step() {
    if (!engine_) return tl::make_unexpected(DiagnosticCode::BlobHeaderInvalid);
    if (halted_) return VmResult::Halted;

    auto r = engine_->step();
    if (!r) return r;
    if (*r == VmResult::Halted) halted_ = true;
    return r;
}

template<typename Policy, typename Oram>
tl::expected<VmExecResult, DiagnosticCode>
StepController<Policy, Oram>::run_to_completion() {
    if (!engine_) return tl::make_unexpected(DiagnosticCode::BlobHeaderInvalid);
    return engine_->execute();
}

template<typename Policy, typename Oram>
uint64_t StepController<Policy, Oram>::return_value() const {
    if (!engine_) return 0;
    return Common::VM::Crypto::FPE_Decode(
        engine_->execution().insn_fpe_key, 0,
        engine_->execution().regs[0].bits);
}

template<typename Policy, typename Oram>
void StepController<Policy, Oram>::set_engine(
    tl::expected<VmEngine<Policy, Oram>, DiagnosticCode>&& eng) {
    if (!eng) throw std::runtime_error("VmRunner: engine creation failed");
    engine_holder_ = std::make_unique<VmEngine<Policy, Oram>>(std::move(*eng));
    engine_ = engine_holder_.get();
}

// ═════════════════════════════════════════════════════════════════════════════
// VmRunner methods
// ═════════════════════════════════════════════════════════════════════════════

template<typename Policy, typename Oram>
tl::expected<VmExecResult, DiagnosticCode> VmRunner<Policy, Oram>::run() {
    auto blob = build_blob_internal();

    VMPILOT_TRY_ASSIGN(engine, VmEngine<Policy, Oram>::create(
        blob.data(), blob.size(), seed_,
        load_base_delta_, init_regs_, num_init_regs_));

    return engine.execute();
}

template<typename Policy, typename Oram>
StepController<Policy, Oram> VmRunner<Policy, Oram>::run_interactive() {
    StepController<Policy, Oram> ctrl;
    ctrl.blob_storage_ = build_blob_internal();

    auto engine = VmEngine<Policy, Oram>::create(
        ctrl.blob_storage_.data(), ctrl.blob_storage_.size(), seed_,
        load_base_delta_, init_regs_, num_init_regs_);
    ctrl.set_engine(std::move(engine));

    return ctrl;
}

template<typename Policy, typename Oram>
std::vector<uint8_t> VmRunner<Policy, Oram>::build_blob_internal() {
    if (!build_fn_)
        throw std::runtime_error("VmRunner: no program defined (call .program())");

    // 1. Assemble the program via ProgramBuilder
    ProgramBuilder pb(&registry_);
    build_fn_(pb);

    const auto& builder_bbs = pb.basic_blocks();
    const auto& builder_pool = pb.pool_entries();

    // 2. Scan all instructions for NATIVE_CALL and auto-compute call_site_ip.
    //    Build the BlobNativeCall vector from the registry.
    std::vector<BlobNativeCall> blob_natives = compute_native_calls(builder_bbs);

    // 3. Build the blob
    return build_blob(seed_, builder_bbs, builder_pool, blob_natives, debug_mode_);
}

template<typename Policy, typename Oram>
std::vector<BlobNativeCall> VmRunner<Policy, Oram>::compute_native_calls(
    const std::vector<BuilderBB>& bbs) const
{
    if (registry_.size() == 0) return {};

    // Pre-allocate: one entry per registered native
    std::vector<BlobNativeCall> result(registry_.size());
    std::vector<bool> seen(registry_.size(), false);

    uint32_t global_ip = 0;
    for (const auto& bb : bbs) {
        for (const auto& insn : bb.instructions) {
            if (insn.opcode == Common::VM::VmOpcode::NATIVE_CALL) {
                uint32_t idx = insn.aux;  // transition entry index
                if (idx < registry_.size()) {
                    const auto& desc = registry_.at(idx);
                    result[idx].call_site_ip = global_ip;
                    result[idx].arg_count = NativeRegistry::pack_arg_count(
                        desc.arg_count, desc.fp_mask, desc.is_variadic);
                    result[idx].target_addr = desc.fn_addr;
                    seen[idx] = true;
                }
            }
            ++global_ip;
        }
    }

    // Fill any natives that weren't referenced in the program
    // (they still need valid transition entries).
    for (uint32_t i = 0; i < registry_.size(); ++i) {
        if (!seen[i]) {
            const auto& desc = registry_.at(i);
            result[i].call_site_ip = 0;  // unused
            result[i].arg_count = NativeRegistry::pack_arg_count(
                desc.arg_count, desc.fp_mask, desc.is_variadic);
            result[i].target_addr = desc.fn_addr;
        }
    }

    return result;
}

// ═════════════════════════════════════════════════════════════════════════════
// Explicit template instantiations
//
// WHY: Same rationale as vm_engine.cpp — template method bodies moved out of
// header for debuggability; explicit instantiation ensures link-time symbols.
// ═════════════════════════════════════════════════════════════════════════════

template class StepController<DebugPolicy,    DirectOram>;
template class StepController<DebugPolicy,    RollingKeyOram>;
template class StepController<StandardPolicy, DirectOram>;
template class StepController<StandardPolicy, RollingKeyOram>;
template class StepController<HighSecPolicy,  DirectOram>;
template class StepController<HighSecPolicy,  RollingKeyOram>;

template class VmRunner<DebugPolicy,    DirectOram>;
template class VmRunner<DebugPolicy,    RollingKeyOram>;
template class VmRunner<StandardPolicy, DirectOram>;
template class VmRunner<StandardPolicy, RollingKeyOram>;
template class VmRunner<HighSecPolicy,  DirectOram>;
template class VmRunner<HighSecPolicy,  RollingKeyOram>;

}  // namespace VMPilot::Runtime
