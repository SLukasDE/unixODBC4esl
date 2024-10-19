include(FetchContent)
#include(FindPkgConfig)

if(ESL_DEPENDENCIES_USE_CONAN)
    message(STATUS "Using Conan")
    include(${CMAKE_BINARY_DIR}/conan/conan_toolchain.cmake)
endif()

if(ESL_DEPENDENCIES_USE_VCPKG)
    message(STATUS "Using VCPKG")
    if(WIN32)
        set(USER_HOME_DIRECTORY $ENV{USERPROFILE})
    else()
        set(USER_HOME_DIRECTORY $ENV{HOME})
    endif()
    message(STATUS "User Home Directory: ${USER_HOME_DIRECTORY}")
    include(${USER_HOME_DIRECTORY}/opt/vcpkg/scripts/buildsystems/vcpkg.cmake)
endif()

if(ESL_DEPENDENCIES_USE_PKGCONFIG)
    find_package(PkgConfig QUIET)
endif()

function(find_custom_package NAME URL TAG)
    # Default, try 'find_package'. VCPKG or Conan may be used, if enabled
    if(NOT ${NAME}_FOUND)
        message(STATUS "Try to find ${NAME} by find_package")
        find_package(${NAME} QUIET)
        if(${NAME}_FOUND)
            message(STATUS "${NAME} has been found by using find_package")
        endif()
    endif()

    if(NOT ${NAME}_FOUND)
        message(STATUS "Try to find ${NAME} by FetchContent")
        set(COMPILE_UNITTESTS OFF)
        FetchContent_Declare(
            ${NAME}
            GIT_REPOSITORY ${URL}
            GIT_TAG ${TAG}
            GIT_SHALLOW TRUE
            OVERRIDE_FIND_PACKAGE # 'find_package(...)' will call 'FetchContent_MakeAvailable(...)'
        )
        find_package(${NAME} QUIET)
        if(${NAME}_FOUND)
            message(STATUS "${NAME} has been found by using FetchContent")
        endif()
    endif()

    if(NOT ${NAME}_FOUND)
        message(FATAL_ERROR "${NAME} NOT found")
    endif()
endfunction()

function(find_package_esa)
    find_custom_package(esa https://github.com/SLukasDE/esa master)
endfunction()

function(find_package_esl)
    find_custom_package(esl https://github.com/SLukasDE/esl master)
endfunction()


function(find_package_ODBC) # ODBC::ODBC
    # Default, try 'find_package'. VCPKG or Conan may be used, if enabled
    if(NOT ODBC_FOUND)
        message(STATUS "Try to find ODBC by find_package")
        find_package(ODBC QUIET)
        if(ODBC_FOUND)
            message(STATUS "ODBC has been found by using find_package")
        endif()
    endif()


    # If still not found, try to download the sourcecode and compile it or use the prebuild binaries from the original website
    if(NOT ODBC_FOUND)
        if(UNIX)
            # https://www.unixodbc.org/
            message(STATUS "Try to fetch source code of unixODBC from original website")
            if(NOT EXISTS "${CMAKE_BINARY_DIR}/external/unixODBC-2.3.12.tar.gz")
                file(
                    DOWNLOAD "https://www.unixodbc.org/unixODBC-2.3.12.tar.gz" "${CMAKE_BINARY_DIR}/external/unixODBC-2.3.12.tar.gz"
                    EXPECTED_HASH MD5=d62167d85bcb459c200c0e4b5a63ee48
                    #SHOW_PROGRESS
                    )
            endif()
            if(EXISTS "${CMAKE_BINARY_DIR}/external/unixODBC-2.3.12.tar.gz")
                file(ARCHIVE_EXTRACT INPUT "${CMAKE_BINARY_DIR}/external/unixODBC-2.3.12.tar.gz" DESTINATION "${CMAKE_BINARY_DIR}/external")
            endif()
            if(EXISTS "${CMAKE_BINARY_DIR}/external/unixODBC-2.3.12")
                execute_process(
                    COMMAND "${CMAKE_BINARY_DIR}/external/unixODBC-2.3.12/configure" --prefix=${CMAKE_BINARY_DIR}/external/unixODBC
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/external/unixODBC-2.3.12"
                )
                execute_process(
                    COMMAND make
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/external/unixODBC-2.3.12"
                )
                execute_process(
                    COMMAND make install
                    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/external/unixODBC-2.3.12"
                )
            endif()
            if(EXISTS "${CMAKE_BINARY_DIR}/external/unixODBC")
                add_library(ODBC::ODBC UNKNOWN IMPORTED)
                set_target_properties(ODBC::ODBC PROPERTIES
                    IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/external/unixODBC/lib/libodbc.so.2.0.0"
                    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_BINARY_DIR}/external/unixODBC/include")
                set(ODBC_FOUND TRUE)
            endif()

            if(ODBC_FOUND)
                message(STATUS "unixODBC has been found by fetiching the source code from original website")
            endif()
        endif(UNIX)

        if(APPLE)
            # https://github.com/openlink/iODBC
            message(STATUS "Try to fetch source code of iODBC from original website")

            # ...

            if(ODBC_FOUND)
                message(STATUS "iODBC has been found by fetiching the source code from original website")
            endif()
        endif(APPLE)
    endif()

    if(NOT ODBC_FOUND)
        message(FATAL_ERROR "ODBC NOT found")
    endif()
endfunction()
