#include "state/persistent_state_provider.hpp"

// Interface-only translation unit — ensures the header compiles as
// part of VMPilot_Runtime. Implementations live in sibling TUs that
// land in Stage A PR-2 (FileBackedStateProvider), Stage B
// (TpmBackedStateProvider) and Stage C (CloudAttestedStateProvider).

static_assert(sizeof(VMPilot::Runtime::State::PersistenceCapability) >= 8,
              "PersistenceCapability must expose all eight honest claims");
