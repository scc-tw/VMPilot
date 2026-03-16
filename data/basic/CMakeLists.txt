cmake_minimum_required(VERSION 3.20)

project(basic_binary)

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

add_executable(basic_binary main.cpp)
add_library(mock_VMPilot SHARED mock_VMPilot.cpp)

# Export all symbols from the DLL on Windows so they appear in the PE export/import tables
set_target_properties(mock_VMPilot PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_link_libraries(basic_binary mock_VMPilot)

# Normalize processor name for consistent output naming across platforms
# On MSVC, CMAKE_SYSTEM_PROCESSOR reports host arch; use pointer size to distinguish x86/x86_64
if(MSVC)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(PROC_NAME "x86_64")
    else()
        set(PROC_NAME "x86")
    endif()
else()
    set(PROC_NAME ${CMAKE_SYSTEM_PROCESSOR})
endif()

# set output filename, mangle with platform and architecture
set(OUTPUT_SUBFIX ${CMAKE_SYSTEM_NAME}.${PROC_NAME})
set_target_properties(basic_binary PROPERTIES OUTPUT_NAME basic_binary.${OUTPUT_SUBFIX})
set_target_properties(mock_VMPilot PROPERTIES OUTPUT_NAME mock_VMPilot.${OUTPUT_SUBFIX})

# copy output files to bin directory
# On Windows, DLLs are runtime outputs; on Unix, shared libs use LIBRARY_OUTPUT_DIRECTORY
set_target_properties(basic_binary PROPERTIES
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

