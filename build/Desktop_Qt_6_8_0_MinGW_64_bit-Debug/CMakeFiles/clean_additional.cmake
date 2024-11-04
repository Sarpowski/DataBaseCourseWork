# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\CourseWorkDB_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\CourseWorkDB_autogen.dir\\ParseCache.txt"
  "CourseWorkDB_autogen"
  )
endif()
