set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

add_compile_options(
  -g3
  -c
  -Os
  -Wall
  -Wextra
  -Werror
  -Wno-psabi # shuts up ABI warnings, only on x86 I think so that's why this
             # isn't present on the Gen3 ARM cross compile
  $<$<COMPILE_LANGUAGE:CXX>:-Wno-volatile> # remove when compiler supports C++23
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
