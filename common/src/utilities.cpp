#include <utilities.hpp>

#include <fstream>

// All known name-mangling variants for VMPilot_Begin/End across compilers
const std::vector<std::string> VMPilot::Common::BEGIN_VMPILOT_SIGNATURES = {
    "_Z13VMPilot_BeginPKc",   // gcc / clang (Itanium ABI)
    "__Z13VMPilot_BeginPKc",  // MinGW 32-bit (Itanium ABI + COFF underscore)
    "?VMPilot_Begin@@YAXPEBD@Z",  // MSVC / clang-cl x64
    "?VMPilot_Begin@@YAXPBD@Z",   // MSVC / clang-cl x86
};

const std::vector<std::string> VMPilot::Common::END_VMPILOT_SIGNATURES = {
    "_Z11VMPilot_EndPKc",       // gcc / clang (Itanium ABI)
    "__Z11VMPilot_EndPKc",      // MinGW 32-bit (Itanium ABI + COFF underscore)
    "?VMPilot_End@@YAXPEBD@Z",  // MSVC / clang-cl x64
    "?VMPilot_End@@YAXPBD@Z",   // MSVC / clang-cl x86
};

// Preferred signature for the current host platform
//
// Order matters: __MINGW64__ implies __MINGW32__, so test it first.
// MinGW (both GCC and Clang) uses Itanium ABI on Windows, while
// MSVC and clang-cl use Microsoft ABI.  32-bit COFF prepends an
// extra underscore to every symbol, giving Itanium's _Z… a __Z… form.
const std::string VMPilot::Common::BEGIN_VMPILOT_SIGNATURE =
#if defined(__MINGW64__)
    "_Z13VMPilot_BeginPKc";  // MinGW 64-bit (Itanium ABI)
#elif defined(__MINGW32__)
    "__Z13VMPilot_BeginPKc";  // MinGW 32-bit (Itanium ABI + COFF underscore)
#elif defined(_WIN64)
    "?VMPilot_Begin@@YAXPEBD@Z";  // MSVC / clang-cl x64
#elif defined(_WIN32)
    "?VMPilot_Begin@@YAXPBD@Z";  // MSVC / clang-cl x86
#else
    "_Z13VMPilot_BeginPKc";  // gcc / clang (Linux, macOS, etc.)
#endif

const std::string VMPilot::Common::END_VMPILOT_SIGNATURE =
#if defined(__MINGW64__)
    "_Z11VMPilot_EndPKc";  // MinGW 64-bit (Itanium ABI)
#elif defined(__MINGW32__)
    "__Z11VMPilot_EndPKc";  // MinGW 32-bit (Itanium ABI + COFF underscore)
#elif defined(_WIN64)
    "?VMPilot_End@@YAXPEBD@Z";  // MSVC / clang-cl x64
#elif defined(_WIN32)
    "?VMPilot_End@@YAXPBD@Z";  // MSVC / clang-cl x86
#else
    "_Z11VMPilot_EndPKc";  // gcc / clang (Linux, macOS, etc.)
#endif

std::vector<uint8_t> VMPilot::Common::read_file(
    const std::string& filename) noexcept {
    auto f = std::ifstream(filename, std::ios::binary);
    if (!f.is_open())
        return {};

    f.seekg(0, std::ios::end);
    auto size = f.tellg();
    f.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer = std::vector<uint8_t>(size);
    f.read(reinterpret_cast<char*>(buffer.data()), size);
    f.close();

    return buffer;
}