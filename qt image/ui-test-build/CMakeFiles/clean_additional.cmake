# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\LibraryUiSmoke_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\LibraryUiSmoke_autogen.dir\\ParseCache.txt"
  "LibraryUiSmoke_autogen"
  )
endif()
