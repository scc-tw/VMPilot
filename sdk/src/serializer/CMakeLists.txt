set(TARGET_NAME VMPilot_SDK_Serializer)

# ---- Proto generation ----
set(PROTO_FILE ${CMAKE_SOURCE_DIR}/sdk/proto/vmpilot.proto)
set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})

set(PROTO_SRC ${PROTO_GEN_DIR}/vmpilot.pb.cc)
set(PROTO_HDR ${PROTO_GEN_DIR}/vmpilot.pb.h)

add_custom_command(
    OUTPUT ${PROTO_SRC} ${PROTO_HDR}
    COMMAND protobuf::protoc
        --proto_path=${CMAKE_SOURCE_DIR}/sdk/proto
        --cpp_out=${PROTO_GEN_DIR}
        ${PROTO_FILE}
    DEPENDS ${PROTO_FILE} protobuf::protoc
    COMMENT "Generating protobuf sources from vmpilot.proto"
)

# ---- Serializer library ----
set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Serializer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SerializationTraits.cpp
    ${PROTO_SRC}
)

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/serializer
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/segmentator
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/region_refiner
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/core
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/crypto
    ${CMAKE_SOURCE_DIR}/third_party/expected/include
    ${PROTO_GEN_DIR}
    ${spdlog_SOURCE_DIR}/include
)

add_library(${TARGET_NAME} STATIC ${SRC_FILES})

# Generated protobuf code and protobuf headers emit warnings under strict settings.
# Suppress third-party warnings with per-compiler flags.
include(${CMAKE_SOURCE_DIR}/cmake/SuppressThirdPartyWarnings.cmake)
suppress_thirdparty_warnings(${TARGET_NAME})

target_include_directories(${TARGET_NAME} PUBLIC
    ${INCLUDE_DIRS}
    ${PROTO_GEN_DIR}
)

target_link_libraries(${TARGET_NAME}
    PUBLIC
        protobuf::libprotobuf
        tomlplusplus::tomlplusplus
        crypto
)
