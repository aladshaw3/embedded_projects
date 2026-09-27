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
  SIL-StaticCodeAnalysis
  LANGUAGES CXX
            C
            ASM
  VERSION 0.1)

option(
  BUILD_UT
  "Builds the unit tests"
  OFF)
option(
  RUN_STATIC_CODE_ANALYSIS
  "Run static code analysis"
  ON)

set(APPLICATION_NAME StaticCodeAnalysis)

# Defines
set(APPLICATION_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SIL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(LIBRARIES_DIR ${SIL_DIR}/../libraries)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
    ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})

include(${SIL_DIR}/FindClangTidy.cmake)

add_compile_options("-fkeep-static-consts")

if(CLANG_TIDY_ERROR_FIXING
   MATCHES
   ON)
  # Adding this flag will apply suggested fixes even if compilation errors were
  # found. If compiler errors have attached fix-its, clang-tidy will apply them
  # as well.
  set(FIX_ERRORS_FLAG --fix-errors)
endif()

set(CONFIG_FILE ${SIL_DIR}/.clang-tidy)
set(CMAKE_CXX_CLANG_TIDY
    ${CLANG_TIDY};--config-file=${CONFIG_FILE};${FIX_ERRORS_FLAG})

include_directories(${APPLICATION_DIR}/Include)

# including these directories as SYSTEM silences any warnings in these
# dependencies while building
include_directories(
  SYSTEM
  ${LIBRARIES_DIR}/etl/include
  ${LIBRARIES_DIR}/eigen
  ${LIBRARIES_DIR}/csvstream)

# Includes
include(${SIL_DIR}/include.cmake)

# Declare executable
add_executable(${APPLICATION_NAME} main.cpp)

# Add Sources
include(${SIL_DIR}/sources.cmake)

# Link
target_link_libraries(${APPLICATION_NAME} PRIVATE pthread)
