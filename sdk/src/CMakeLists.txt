CPMAddPackage(
    NAME ELFIO
    GITHUB_REPOSITORY "serge1/ELFIO"
    GIT_TAG "Release_3.12"
)

CPMAddPackage("gh:gabime/spdlog@1.14.1")

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
    ${spdlog_SOURCE_DIR}/include
)

set (TARGET_NAME VMPilot_SDK_LIB)
set (SRC_FILE ${SRC_FILE}
    ${CMAKE_CURRENT_SOURCE_DIR}/BytecodeCompileRecipe.cpp
)

include_directories(${INCLUDE_DIRS})
add_library(${TARGET_NAME} STATIC ${SRC_FILE})

add_subdirectory(bytecode_compiler)
add_subdirectory(segmentator)
add_subdirectory(region_refiner)
set (LIBS ${LIBS}
    VMPilot_SDK_Bytecode_Compiler
    VMPilot_SDK_RegionRefiner
    capstone_wrapper
)

target_link_libraries(${TARGET_NAME} PUBLIC ${LIBS})
