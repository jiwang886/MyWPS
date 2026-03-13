# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\MyWPS_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\MyWPS_autogen.dir\\ParseCache.txt"
  "MyWPS_autogen"
  )
endif()
