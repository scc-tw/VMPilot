#include <utilities.hpp>

#include <fstream>

// All known name-mangling variants for VMPilot_Begin/End across compilers
const std::vector<std::string> VMPilot::Common::BEGIN_VMPILOT_SIGNATURES = {
    "_Z13VMPilot_BeginPKc",        // gcc / clang (Itanium ABI)
    "?VMPilot_Begin@@YAXPEBD@Z",   // MSVC x64
    "?VMPilot_Begin@@YAXPBD@Z",    // MSVC x86
};

const std::vector<std::string> VMPilot::Common::END_VMPILOT_SIGNATURES = {
    "_Z11VMPilot_EndPKc",           // gcc / clang (Itanium ABI)
    "?VMPilot_End@@YAXPEBD@Z",     // MSVC x64
    "?VMPilot_End@@YAXPBD@Z",      // MSVC x86
};

// Preferred signature for the current host platform
const std::string VMPilot::Common::BEGIN_VMPILOT_SIGNATURE =
#ifdef _WIN64
    "?VMPilot_Begin@@YAXPEBD@Z";
#elif _WIN32
    "?VMPilot_Begin@@YAXPBD@Z";
#else
    "_Z13VMPilot_BeginPKc";
#endif

const std::string VMPilot::Common::END_VMPILOT_SIGNATURE =
#ifdef _WIN64
    "?VMPilot_End@@YAXPEBD@Z";
#elif _WIN32
    "?VMPilot_End@@YAXPBD@Z";
#else
    "_Z11VMPilot_EndPKc";
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