# Build capstone from submodule (frontend owns disassembly)
set(CAPSTONE_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(CAPSTONE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CAPSTONE_BUILD_CSTOOL OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third_party/capstone
                 ${CMAKE_BINARY_DIR}/third_party/capstone)

add_subdirectory(src/capstone_wrapper)
add_subdirectory(src/segmentator)
add_subdirectory(src/region_refiner)
add_subdirectory(src/reference_analyzer)

# Frontend aggregate library
add_library(VMPilot_Frontend INTERFACE)
target_link_libraries(VMPilot_Frontend INTERFACE
    VMPilot_SDK_Segmentator
    VMPilot_SDK_RegionRefiner
    VMPilot_SDK_ReferenceAnalyzer
    capstone_wrapper
)

if (ENABLE_TESTS)
    add_subdirectory(tests)
endif()
