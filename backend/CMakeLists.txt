add_subdirectory(src/bytecode_compiler)
add_subdirectory(tools)

# Backend aggregate library
add_library(VMPilot_Backend INTERFACE)
target_link_libraries(VMPilot_Backend INTERFACE
    VMPilot_SDK_Bytecode_Compiler
)

# Main executable
add_executable(VMPilot_SDK main.cpp)
target_link_libraries(VMPilot_SDK PRIVATE VMPilot_Backend VMPilot_Frontend VMPilot_SDK_Serializer)

if (ENABLE_TESTS)
    add_subdirectory(tests)
endif()
