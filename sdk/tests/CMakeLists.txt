find_library(GMOCK_LIB NAMES gmock libgmock.a HINTS ${CMAKE_BINARY_DIR}/lib)
if(NOT GMOCK_LIB)
  message(FATAL_ERROR "libgmock.a not found in ${CMAKE_BINARY_DIR}/lib")
endif()

file(GLOB test_srcs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

# Include directories (no longer needed, since they are propagated by target_link_libraries)
# include_directories(${INCLUDE_DIRS})

set(TARGET_CATEGORY VMPilot_SDK_LIB)

foreach(test_src ${test_srcs})
  get_filename_component(test_name ${test_src} NAME_WE)
  add_executable(${test_name} ${test_src})

  # Link libraries, ensuring that ${TARGET_CATEGORY} is included
  target_link_libraries(${test_name} PRIVATE ${LIBS} ${TARGET_CATEGORY})

  add_test(NAME ${test_name} COMMAND ${test_name})
  # Set common properties for all tests might in the future
  # set_tests_properties(${test_name} PROPERTIES TIMEOUT 10)
  message(STATUS "Added test: ${test_name}")
endforeach()
