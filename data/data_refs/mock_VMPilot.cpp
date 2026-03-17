/// Mock VMPilot library with exported data for cross-DSO testing.
///
/// The main binary accesses g_shared_counter and g_shared_config[]
/// through GOT-indirect loads (ELF) or IAT (PE).

void VMPilot_Begin(char const*) {}
void VMPilot_End(char const*) {}

/// Exported data symbols — live in the shared library's .data section.
/// From the main binary's perspective these are cross-DSO references
/// that must go through a GOT/IAT indirection.
int g_shared_counter = 42;
int g_shared_config[4] = {10, 20, 30, 40};
