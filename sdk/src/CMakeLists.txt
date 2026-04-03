# CPM dependencies (ELFIO, COFFI, spdlog, toml++) are declared centrally
# in cmake/Dependencies.cmake — not here.

# Build capstone from submodule
set(CAPSTONE_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(CAPSTONE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CAPSTONE_BUILD_CSTOOL OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/capstone
                 ${CMAKE_BINARY_DIR}/third_party/capstone)

# Build our capstone C++ wrapper
add_subdirectory(capstone_wrapper)

set (INCLUDE_DIRS
    ${INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/common/include
)

# Third-party header-only libs — mark SYSTEM to suppress warnings
include_directories(SYSTEM
    ${spdlog_SOURCE_DIR}/include
)

set (TARGET_NAME VMPilot_SDK_LIB)

include_directories(${INCLUDE_DIRS})

add_subdirectory(bytecode_compiler)
add_subdirectory(segmentator)
add_subdirectory(region_refiner)
add_subdirectory(reference_analyzer)
add_subdirectory(serializer)

# VMPilot_SDK_LIB is an interface library that aggregates all SDK sub-libraries.
add_library(${TARGET_NAME} INTERFACE)
target_link_libraries(${TARGET_NAME} INTERFACE
    VMPilot_SDK_Bytecode_Compiler
    VMPilot_SDK_RegionRefiner
    VMPilot_SDK_ReferenceAnalyzer
    VMPilot_SDK_Serializer
    capstone_wrapper
)
