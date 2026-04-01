# cmake/CompilerWarnings.cmake — vmpilot_options INTERFACE target
#
# Carries C++17, warning flags, per-config optimisation, and colour
# diagnostics.  Every first-party target links this; third_party does NOT.

add_library(vmpilot_options INTERFACE)

target_compile_features(vmpilot_options INTERFACE cxx_std_17)

target_compile_options(vmpilot_options INTERFACE
    # Warnings
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Werror -pedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /utf-8>
    # Colour diagnostics
    $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>
    $<$<CXX_COMPILER_ID:Clang,AppleClang>:-fcolor-diagnostics>
    # Debug
    $<$<AND:$<CONFIG:Debug>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-g -fno-omit-frame-pointer -O0>
    $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/Zi /Od>
    # Release
    $<$<AND:$<CONFIG:Release>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-O3 -fdata-sections -ffunction-sections>
    $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/O2 /Gy>
    # MinSizeRel
    $<$<AND:$<CONFIG:MinSizeRel>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-Os -fdata-sections -ffunction-sections>
    $<$<AND:$<CONFIG:MinSizeRel>,$<CXX_COMPILER_ID:MSVC>>:/O1 /Gy>
    # RelWithDebInfo
    $<$<AND:$<CONFIG:RelWithDebInfo>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-O2 -g -fno-omit-frame-pointer>
    $<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:MSVC>>:/O2 /Zi>
)

target_compile_definitions(vmpilot_options INTERFACE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:NDEBUG>
)
