set(TARGET_NAME capstone_wrapper)

set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/capstone.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/x86.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/arm64.cpp
)

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/capstone_wrapper
    ${CMAKE_SOURCE_DIR}/third_party/capstone/include
    ${CMAKE_SOURCE_DIR}/common/include
)

add_library(${TARGET_NAME} STATIC ${SRC_FILES})
target_include_directories(${TARGET_NAME} PUBLIC ${INCLUDE_DIRS})
target_link_libraries(${TARGET_NAME} PUBLIC capstone)
if(MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /utf-8)
endif()
