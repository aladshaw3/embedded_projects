set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}clang)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}clang++)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_C_COMPILER_ID Clang)
set(CMAKE_CXX_COMPILER_ID Clang)
add_compile_options(
  -c
  -Os
  -Wall
  -Wextra
  -Werror
  -pedantic
  -pedantic-errors
  -Wdouble-promotion
  -fstack-usage
  -ffunction-sections
  -fdata-sections
  -fno-rtti
  -fno-use-cxa-atexit
  -fno-exceptions
  -MP
  -Wno-delete-non-abstract-non-virtual-dtor
  -Wno-gnu-zero-variadic-macro-arguments
  -Wno-newline-eof
  -Wno-extra-semi)

add_compile_definitions(
  ETL_NO_SMALL_CHAR_SUPPORT=0
  ETL_CHECK_PUSH_POP)

execute_process(COMMAND ${CMAKE_ASM_COMPILER} --version
                OUTPUT_VARIABLE clang_full_version_string)
string(
  REGEX
  REPLACE ".*clang version ([0-9]+\\.[0-9]+).*"
          "\\1"
          CMAKE_ASM_COMPILER_VERSION
          ${clang_full_version_string})
