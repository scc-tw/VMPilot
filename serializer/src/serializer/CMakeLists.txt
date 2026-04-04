set(TARGET_NAME VMPilot_SDK_Serializer)

# ---- Serializer library ----
set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Serializer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SerializationTraits.cpp
)

set(INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/serializer/include/serializer
    ${CMAKE_SOURCE_DIR}/frontend/include/segmentator
    ${CMAKE_SOURCE_DIR}/frontend/include/region_refiner
    ${CMAKE_SOURCE_DIR}/common/include/core
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/crypto
    ${CMAKE_SOURCE_DIR}/third_party/expected/include
    ${spdlog_SOURCE_DIR}/include
)

add_library(${TARGET_NAME} STATIC ${SRC_FILES})

target_include_directories(${TARGET_NAME} PUBLIC
    ${INCLUDE_DIRS}
)

target_link_libraries(${TARGET_NAME}
    PUBLIC
        tomlplusplus::tomlplusplus
        crypto
)
