# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\LibraryGui_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\LibraryGui_autogen.dir\\ParseCache.txt"
  "LibraryGui_autogen"
  )
endif()
