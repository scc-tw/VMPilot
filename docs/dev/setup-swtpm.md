# Setting up swtpm for Stage B development

Stage B (TPM-backed persistent state) lands incrementally from PR-B2
onward. On Linux you can exercise the real code path without attaching
a physical TPM by running `swtpm` as a local emulator.

This note covers the minimum dev loop. CI follows the same steps.

## 1. Install tpm2-tss + swtpm

Ubuntu 22.04 / Debian 12 and later:

```bash
sudo apt update
sudo apt install \
    libtss2-dev \
    tpm2-tools \
    swtpm \
    swtpm-tools \
    pkg-config
```

Fedora / RHEL:

```bash
sudo dnf install tpm2-tss-devel tpm2-tools swtpm swtpm-tools pkgconf-pkg-config
```

`libtss2-dev` provides the `tss2-esys` and `tss2-tctildr` pkg-config
files that `cmake/Tpm2Tss.cmake` looks for.

## 2. Start swtpm

A minimal swtpm instance listening on localhost:

```bash
mkdir -p /tmp/mytpm
swtpm socket \
    --tpm2 \
    --tpmstate dir=/tmp/mytpm \
    --server type=tcp,port=2321 \
    --ctrl  type=tcp,port=2322 \
    --flags not-need-init,startup-clear \
    --daemon
```

Point the TCTI at it for the test binary:

```bash
export VMPILOT_TPM_TCTI="swtpm:host=localhost,port=2321"
```

*(The env var is honoured from PR-B3 onward. In PR-B2 the code uses the
system-default TCTI — `Tss2_TctiLdr_Initialize(nullptr, ...)` — which
picks `tabrmd` if it's running, otherwise `/dev/tpm0`. For PR-B2
testing against swtpm, set the `TCTI` env var that tpm2-tss itself
honours.)*

## 3. Configure + build with TPM support

```bash
cmake -S . -B build/dev-linux -DVMPILOT_ENABLE_TPM=ON
cmake --build build/dev-linux --parallel
```

The `VMPILOT_ENABLE_TPM=ON` option flips `VMPILOT_HAS_TPM2_TSS` on
through `cmake/Tpm2Tss.cmake` and links the runtime against tss2-esys
+ tss2-tctildr.

## 4. Run the smoke test

```bash
cd build/dev-linux
ctest -R TpmSession --output-on-failure
```

Expected output with swtpm running:

```
    Start: TpmSessionSmoke.GetManufacturerReturnsNonZero       ... Passed
    Start: TpmSessionSmoke.ManufacturerIsStableAcrossCalls     ... Passed
    Start: TpmSessionLifecycle.OpenAndCloseDoesNotLeak         ... Passed
```

If swtpm isn't running:

```
    Start: TpmSessionSmoke.GetManufacturerReturnsNonZero       ... Skipped
```

Skips are fine — they mean no TPM was reachable, which is the
honest-unavailable posture Stage B wants.

## 5. Stopping swtpm

```bash
pkill -f 'swtpm socket'
rm -rf /tmp/mytpm
```

## Windows / macOS

Stage B is Linux-first (see doc 17a §2 platform policy):

- **Windows** — TBS integration is a later PR. Leave
  `VMPILOT_ENABLE_TPM=OFF` (the default).
- **macOS** — no public TPM2 NV monotonic counter API. Apple providers
  belong in Stage C; Stage B will not be enabled on macOS.

Attempting `VMPILOT_ENABLE_TPM=ON` on either platform causes a CMake
`FATAL_ERROR` with a pointer to the relevant doc.
