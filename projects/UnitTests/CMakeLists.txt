# cmake declarations
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_COMPILER_LAUNCHER ccache)
set(CMAKE_C_COMPILER_LAUNCHER ccache)

set(COMPILE_WARNING_AS_ERROR TRUE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

project(
  SIL-UnitTests
  LANGUAGES CXX
            C
            ASM
  VERSION 0.1)

option(
  BUILD_UT
  "Builds the unit tests"
  ON)
option(
  ENABLE_TEST_COVERAGE
  "Enable test coverage"
  OFF)
option(
  ON_UBUNTU
  "Run build on Ubuntu"
  FALSE)
option(
  RUN_STATIC_CODE_ANALYSIS
  "Run static code analysis"
  OFF)

set(APPLICATION_NAME UnitTests)

# Defines
set(APPLICATION_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SIL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(LIBRARIES_DIR ${SIL_DIR}/../libraries)

list(
  APPEND
  CMAKE_MODULE_PATH
  "${LIBRARIES_DIR}/cmake-modules")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
    ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

add_compile_options(
  "-DGTEST"
  "-fkeep-static-consts")

include(CTest)
include(GoogleTest)
enable_testing()

include_directories(${APPLICATION_DIR}/Include)

# including these directories as SYSTEM silences any warnings in these
# dependencies while building
include_directories(
  SYSTEM
  ${LIBRARIES_DIR}/etl/include
  ${LIBRARIES_DIR}/googletest/googletest/include
  ${LIBRARIES_DIR}/eigen
  ${LIBRARIES_DIR}/csvstream)

# Includes
include(${SIL_DIR}/SanitizerOptions.cmake)

include(${SIL_DIR}/include.cmake)

# Declare executable
add_executable(${APPLICATION_NAME})
add_dependencies(
  ${APPLICATION_NAME}
  gtest
  gtest_main)

# Add Sources
include(${SIL_DIR}/sources.cmake)

# due to the googletest directory not being a real subdirectory, have to also
# set its binary distory to CMAKE_CURRENT_BINARY_DIR/googletest
add_subdirectory(
  ${LIBRARIES_DIR}/googletest
  googletest)

configure_file(
  gdbinit
  gdbinit
  COPYONLY)
configure_file(
  lldbinit
  lldbinit
  COPYONLY)

# Link
target_link_directories(
  ${APPLICATION_NAME}
  PRIVATE
  ${CMAKE_BINARY_DIR}/lib)
target_link_libraries(
  ${APPLICATION_NAME}
  PRIVATE libgtest.a
          libgtest_main.a
          pthread)

# TIMEOUT: A best practice for unit tests is that all unit tests for the entire
# project execute within a few seconds.
gtest_discover_tests(
  ${APPLICATION_NAME}
  PROPERTIES
  TIMEOUT 10)

# Test coverage
if(ENABLE_TEST_COVERAGE
   MATCHES
   ON)
  include(CodeCoverage)

  append_coverage_compiler_flags()

  if(ON_UBUNTU
     MATCHES
     TRUE)
    # cmake-format: off
    # cmake-lint: disable=C0301
    setup_target_for_coverage_lcov(
      NAME ${APPLICATION_NAME}_coverage
      EXECUTABLE ${APPLICATION_NAME} -j ${PROCESSOR_COUNT}
      DEPENDENCIES ${APPLICATION_NAME}
      EXCLUDE "${LIBRARIES_DIR}/*" "/usr/include/c++/*"
      "${LIBRARIES_DIR}/*"
      LCOV_ARGS --ignore-errors inconsistent,inconsistent --ignore-errors unused
      GENHTML_ARGS --ignore-errors inconsistent,inconsistent --ignore-errors category)
    # cmake-format: on
  else()
    # cmake-format: off
    # cmake-lint: disable=C0301
    setup_target_for_coverage_lcov(
      NAME ${APPLICATION_NAME}_coverage
      EXECUTABLE ${APPLICATION_NAME} -j ${PROCESSOR_COUNT}
      DEPENDENCIES ${APPLICATION_NAME}
      EXCLUDE "/Library/*" "/opt/*" "/Applications/*" "${LIBRARIES_DIR}/*"
      LCOV_ARGS --ignore-errors inconsistent,inconsistent --ignore-errors unused
      GENHTML_ARGS --ignore-errors inconsistent,inconsistent --ignore-errors category)
    # cmake-format: on
  endif()
endif()
