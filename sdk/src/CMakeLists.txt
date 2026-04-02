# CPM dependencies (ELFIO, COFFI, spdlog, toml++) are declared centrally
# in cmake/Dependencies.cmake — not here.

# Build abseil-cpp + protobuf from submodules.
# Save and strip warnings-as-errors so third-party C code compiles cleanly.
set(_saved_c_flags "${CMAKE_C_FLAGS}")
set(_saved_cxx_flags "${CMAKE_CXX_FLAGS}")
string(REPLACE "/WX" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REPLACE "/WX" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE "-Werror" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REPLACE "-Werror" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Suppress all warnings from third-party code (abseil/protobuf).
# On MSVC we only strip /WX (above) — adding /w would conflict with abseil's
# /W3 and protobuf's /W1 target_compile_options, causing D9025 spam.
# Both libraries are clean at their own warning levels so no extra noise appears.
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
endif()

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

# Mark Google's include directories as SYSTEM so warnings from protobuf/abseil
# headers don't fire when included transitively by our code under -Werror.
# Note: protobuf::libprotobuf and absl::base are ALIAS targets — must resolve
# to the real target before setting properties.
foreach(_google_tgt libprotobuf absl_base)
    if (TARGET ${_google_tgt})
        get_target_property(_inc ${_google_tgt} INTERFACE_INCLUDE_DIRECTORIES)
        if (_inc)
            set_target_properties(${_google_tgt} PROPERTIES
                INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${_inc}")
        endif()
    endif()
endforeach()

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
