# Definitions

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(${SIL_DIR}/utilities.cmake)

# ################ Add other project directories here #####################
include_directories(${SIL_DIR}/ExampleProject)

# including these directories as SYSTEM silences any warnings in these
# dependencies while building
include_directories(
  SYSTEM
  ${SIL_DIR}/../libraries)

add_compile_definitions(
  LFS_NO_MALLOC
  LFS_THREADSAFE
  DISABLEFLOAT16)
