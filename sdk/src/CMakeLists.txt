CPMAddPackage(
    NAME ELFIO
    GITHUB_REPOSITORY "serge1/ELFIO"
    GIT_TAG "Release_3.12"
)

CPMAddPackage(
    NAME COFFI
    GITHUB_REPOSITORY "scc-tw/COFFI"
    GIT_TAG "v1.1.3"
)

CPMAddPackage("gh:gabime/spdlog@1.17.0")

CPMAddPackage(
    NAME tomlplusplus
    GITHUB_REPOSITORY "marzer/tomlplusplus"
    GIT_TAG "v3.4.0"
)

# Build abseil-cpp + protobuf from submodules.
# Save and strip warnings-as-errors so third-party C code compiles cleanly.
set(_saved_c_flags "${CMAKE_C_FLAGS}")
set(_saved_cxx_flags "${CMAKE_CXX_FLAGS}")
string(REPLACE "/WX" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REPLACE "/WX" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE "-Werror" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REPLACE "-Werror" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Abseil — pinned submodule (LTS 2026-01-07), built before protobuf
# so that absl::strings target already exists and protobuf skips FetchContent.
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/abseil-cpp
                 ${CMAKE_BINARY_DIR}/third_party/abseil-cpp
                 EXCLUDE_FROM_ALL)

set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(protobuf_INSTALL OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_PROTOC_BINARIES ON CACHE BOOL "" FORCE)
set(protobuf_WITH_ZLIB OFF CACHE BOOL "" FORCE)
set(protobuf_MSVC_STATIC_RUNTIME OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/protobuf
                 ${CMAKE_BINARY_DIR}/third_party/protobuf
                 EXCLUDE_FROM_ALL)

# Restore original flags
set(CMAKE_C_FLAGS "${_saved_c_flags}")
set(CMAKE_CXX_FLAGS "${_saved_cxx_flags}")

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
    ${CMAKE_BINARY_DIR}/_deps/elfio-src/
    ${CMAKE_BINARY_DIR}/_deps/coffi-src/
    ${spdlog_SOURCE_DIR}/include
)

set (TARGET_NAME VMPilot_SDK_LIB)

include_directories(${INCLUDE_DIRS})

add_subdirectory(bytecode_compiler)
add_subdirectory(segmentator)
add_subdirectory(region_refiner)
add_subdirectory(serializer)

# VMPilot_SDK_LIB is an interface library that aggregates all SDK sub-libraries.
add_library(${TARGET_NAME} INTERFACE)
target_link_libraries(${TARGET_NAME} INTERFACE
    VMPilot_SDK_Bytecode_Compiler
    VMPilot_SDK_RegionRefiner
    VMPilot_SDK_Serializer
    capstone_wrapper
)
