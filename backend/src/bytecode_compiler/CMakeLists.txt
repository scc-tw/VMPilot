set (INCLUDE_DIRS
    ${INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/backend/include/
    ${CMAKE_SOURCE_DIR}/backend/include/bytecode_compiler/
    ${CMAKE_SOURCE_DIR}/common/include/core/
    ${CMAKE_SOURCE_DIR}/frontend/include/segmentator/
    ${CMAKE_SOURCE_DIR}/frontend/include/region_refiner/
    ${CMAKE_SOURCE_DIR}/frontend/include/reference_analyzer/
    ${CMAKE_SOURCE_DIR}/frontend/include/capstone_wrapper/
    ${CMAKE_SOURCE_DIR}/serializer/include/serializer/
    ${CMAKE_SOURCE_DIR}/common/include/
    ${CMAKE_SOURCE_DIR}/third_party/expected/include/
    ${spdlog_SOURCE_DIR}/include
)

set (TARGET_NAME VMPilot_SDK_Bytecode_Compiler)

# Add the source files
set (SRC_FILES ${SRC_FILES}
    ${CMAKE_CURRENT_SOURCE_DIR}/bytecode_compiler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SimpleBackend.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/CompilationOrchestrator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/compile_pipeline.cpp
)

set (LIBS ${LIBS} VMPilot_Common VMPilot_SDK_Segmentator VMPilot_SDK_ReferenceAnalyzer VMPilot_SDK_Serializer)

include_directories(${INCLUDE_DIRS})

add_library(${TARGET_NAME} STATIC ${SRC_FILES})

if (MSVC)
    target_compile_options(${TARGET_NAME} PRIVATE /WX-)
endif()

target_link_libraries(${TARGET_NAME} ${LIBS})
