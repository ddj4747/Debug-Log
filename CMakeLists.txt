cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0167 NEW)
project(Debug-Log)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE INC inc/*)

find_package(Boost)

add_subdirectory(ext)
add_library(Debug-Log STATIC ${SRC} ${INC})
target_include_directories(Debug-Log PUBLIC inc/)

if (Boost_FOUND AND NOT DEBUG_LOG_DISABLE_STACKTRACE)
    target_link_libraries(Debug-Log PRIVATE Boost::boost Debug-Log-EXT-Interface)
else()
    add_compile_definitions(DISABLE_LOGGING_STACKTRACE)
    target_link_libraries(Debug-Log PRIVATE Debug-Log-EXT-Interface)
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