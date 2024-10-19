include(CMakeFindDependencyMacro)

find_dependency(esa)
find_dependency(esl)
find_dependency(ODBC)

include("${CMAKE_CURRENT_LIST_DIR}/odbc4eslTargets.cmake")
