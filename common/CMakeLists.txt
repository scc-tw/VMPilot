add_subdirectory(crypto)

find_package(Threads REQUIRED)

set (INCLUDE_DIRS ${INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/vm
    ${CRYPTO_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/third_party/expected/include
)
include_directories (${INCLUDE_DIRS})

# setup library source name
set(LIB_NAME "opcode_table")

# Add the source files
set (SRC_FILES ${SRC_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/opcode_table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/instruction_t.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/file_type_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utilities.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/macho_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/thread_pool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/diagnostic_collector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vm/vm_crypto.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vm/vm_encoding.cpp
)

# Add the library dependencies dir BLAKE3
set (LIBS ${LIBS}
    crypto
    Threads::Threads
)

add_library(${LIB_NAME} STATIC ${SRC_FILES})
target_link_libraries(${LIB_NAME} ${LIBS})