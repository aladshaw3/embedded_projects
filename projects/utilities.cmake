# include sources.cmake for the subdirectory
function(
  include_sources
  directory)
  include(${CMAKE_CURRENT_LIST_DIR}/${directory}/sources.cmake)
endfunction()

# add source files at an absolute directory
function(
  add_sources_at
  directory)
  list(
    TRANSFORM
    ARGN
    PREPEND ${directory}/)
  target_sources(${APPLICATION_NAME} PRIVATE ${ARGN})
endfunction()

# add source files in the current directory
function(add_sources)
  add_sources_at(
    ${CMAKE_CURRENT_LIST_DIR}
    ${ARGN})
endfunction()

# supress a compiler warning for a set of files in absolute directory
function(
  suppress_warning_at
  directory
  warning)
  list(
    TRANSFORM
    ARGN
    PREPEND ${directory}/)
  set_source_files_properties(
    ${ARGN}
    PROPERTIES COMPILE_OPTIONS
               -Wno-${warning})
endfunction()

# suppress a compiler warning for a set of files in current directory
function(
  suppress_warning
  warning)
  suppress_warning_at(
    ${CMAKE_CURRENT_LIST_DIR}
    ${warning}
    ${ARGN})
endfunction()
