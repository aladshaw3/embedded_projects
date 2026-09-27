set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}clang)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}clang++)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_C_COMPILER_ID Clang)
set(CMAKE_CXX_COMPILER_ID Clang)

add_compile_options(
  -g3
  -c
  -O0
  -Wall
  -Wextra
  -Werror
  -Wno-psabi
  -Wno-newline-eof
  -pedantic
  -pedantic-errors
  -fstack-usage
  -ffunction-sections
  -fdata-sections
  $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
  $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>)

add_compile_definitions(
  ETL_NO_SMALL_CHAR_SUPPORT=0
  ETL_CHECK_PUSH_POP)
