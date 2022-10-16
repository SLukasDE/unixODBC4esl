cmake_minimum_required(VERSION 3.23)

project(unixODBC4esl VERSION 1.5.0
            LANGUAGES CXX)
            
#include(FetchContent)
#FetchContent_Declare(
#  catch
#  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#  GIT_TAG        v2.13.6
#)
#FetchContent_MakeAvailable(catch)


# The compiled library code is here
#add_subdirectory(src/main)
# STATIC, SHARED, MODULE, INTERFACE
add_library(unixODBC4esl STATIC
      ./src/main/unixODBC4esl/Plugin.cpp
      ./src/main/unixODBC4esl/database/ResultSetBinding.cpp
      ./src/main/unixODBC4esl/database/Diagnostics.cpp
      ./src/main/unixODBC4esl/database/ResultSetBinding.h
      ./src/main/unixODBC4esl/database/ConnectionFactory.h
      ./src/main/unixODBC4esl/database/PreparedStatementBinding.h
      ./src/main/unixODBC4esl/database/PreparedBulkStatementBinding.cpp
      ./src/main/unixODBC4esl/database/Driver.cpp
      ./src/main/unixODBC4esl/database/BindVariable.h
      ./src/main/unixODBC4esl/database/StatementHandle.h
      ./src/main/unixODBC4esl/database/PreparedBulkStatementBinding.h
      ./src/main/unixODBC4esl/database/StatementHandle.cpp
      ./src/main/unixODBC4esl/database/Connection.h
      ./src/main/unixODBC4esl/database/Connection.cpp
      ./src/main/unixODBC4esl/database/ConnectionFactory.cpp
      ./src/main/unixODBC4esl/database/BindVariable.cpp
      ./src/main/unixODBC4esl/database/PreparedStatementBinding.cpp
      ./src/main/unixODBC4esl/database/Diagnostics.h
      ./src/main/unixODBC4esl/database/BindResult.cpp
      ./src/main/unixODBC4esl/database/Driver.h
      ./src/main/unixODBC4esl/database/BindResult.h
      ./src/main/unixODBC4esl/Plugin.h
      ./src/main/unixODBC4esl/Logger.h
)

target_include_directories(unixODBC4esl PUBLIC ./src/main)
target_compile_features(unixODBC4esl PUBLIC cxx_std_11)
set_property(TARGET unixODBC4esl PROPERTY POSITION_INDEPENDENT_CODE ON)

find_package(esl REQUIRED)
find_package(ODBC REQUIRED)

target_link_libraries(unixODBC4esl PRIVATE esl::esl)
target_link_libraries(unixODBC4esl PRIVATE odbc)

install(TARGETS unixODBC4esl
        EXPORT unixODBC4esl-export
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
)

install(EXPORT      unixODBC4esl-export
        FILE        unixODBC4eslTargets.cmake
        NAMESPACE   unixODBC4esl::
        DESTINATION lib/cmake
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/unixODBC4eslConfig.cmake
        DESTINATION "lib/cmake"
)




# The compiled test code is here
#add_subdirectory(src/test)
