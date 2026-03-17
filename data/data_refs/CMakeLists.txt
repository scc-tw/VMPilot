cmake_minimum_required(VERSION 3.20)
project(data_refs)
enable_language(CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /utf-8")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
    if("${CMAKE_CXX_FLAGS}" MATCHES "-m32")
        set(CMAKE_SYSTEM_PROCESSOR "x86")
    endif()
endif()

add_executable(data_refs main.cpp)
add_library(mock_VMPilot SHARED mock_VMPilot.cpp)
set_target_properties(mock_VMPilot PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
target_link_libraries(data_refs mock_VMPilot)

if(MSVC)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(PROC_NAME "x86_64")
    else()
        set(PROC_NAME "x86")
    endif()
else()
    set(PROC_NAME ${CMAKE_SYSTEM_PROCESSOR})
endif()

set(OUTPUT_SUBFIX ${CMAKE_SYSTEM_NAME}.${PROC_NAME})
set_target_properties(data_refs PROPERTIES OUTPUT_NAME data_refs.${OUTPUT_SUBFIX})
set_target_properties(mock_VMPilot PROPERTIES OUTPUT_NAME mock_VMPilot.${OUTPUT_SUBFIX})

set_target_properties(data_refs PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)
set_target_properties(mock_VMPilot PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)
