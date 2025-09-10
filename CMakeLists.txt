cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0167 NEW)
project(Debug-Log)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE INC inc/*)

find_package(Boost COMPONENTS stacktrace_basic QUIET)

include(FetchContent)

FetchContent_Declare(
        google-test
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main
)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG        11.2.0
)

FetchContent_Declare(
        google-benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG main
)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(fmt)
FetchContent_MakeAvailable(google-test)
FetchContent_MakeAvailable(google-benchmark)

add_library(Debug-Log STATIC ${SRC} ${INC})
target_include_directories(Debug-Log PUBLIC inc/)

if (Boost_FOUND AND NOT DEBUG_LOG_DISABLE_STACKTRACE)
    if(APPLE)
        add_compile_definitions(BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_compile_definitions(_GNU_SOURCE)
    endif()

    target_link_libraries(Debug-Log PUBLIC Boost::boost fmt::fmt)
else()
    add_compile_definitions(DISABLE_LOGGING_STACKTRACE)
    target_link_libraries(Debug-Log PUBLIC fmt::fmt)
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

set(DEBUG_LOG_TESTS_ENABLED ON)

if (DEBUG_LOG_TESTS_ENABLED)
    include(GoogleTest)

    FILE(GLOB_RECURSE TEST_FILES tests/*.cpp)

    add_executable(DebugLogTests ${TEST_FILES})
    target_link_libraries(DebugLogTests PRIVATE Debug-Log gtest_main)
    gtest_discover_tests(DebugLogTests)


    add_executable(DebugLogBenchmark
            benchmark/log_benchmark.cpp
    )

    target_link_libraries(DebugLogBenchmark PRIVATE Debug-Log benchmark::benchmark)
endif()
