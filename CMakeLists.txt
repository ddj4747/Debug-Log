cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0167 NEW)
project(Debug-Log)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE INC inc/*)

find_package(Boost)
include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG        11.2.0
)
FetchContent_MakeAvailable(fmt)

add_library(Debug-Log STATIC ${SRC} ${INC})
target_include_directories(Debug-Log PUBLIC inc/)

if (Boost_FOUND AND NOT DEBUG_LOG_DISABLE_STACKTRACE)
    target_link_libraries(Debug-Log PRIVATE Boost::boost fmt::fmt)
else()
    add_compile_definitions(DISABLE_LOGGING_STACKTRACE)
    target_link_libraries(Debug-Log PRIVATE fmt::fmt)
endif()

if (DEBUG_LOG_DISABLE_LOGGING OR (DEBUG_LOG_DISABLE_CONSOLE_LOGGING AND DEBUG_LOG_DISABLE_FILE_LOGGING))
    add_compile_definitions(
            DISABLE_LOGGING
            DISABLE_CONSOLE_LOGGING
            DISABLE_FILE_LOGGING
    )
else ()
    if (DEBUG_LOG_DISABLE_CONSOLE_LOGGING)
        add_compile_definitions(DISABLE_CONSOLE_LOGGING)
    endif()

    if (DEBUG_LOG_DISABLE_FILE_LOGGING)
        add_compile_definitions(DISABLE_FILE_LOGGING)
    endif ()
endif ()