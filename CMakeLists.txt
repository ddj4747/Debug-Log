cmake_minimum_required(VERSION 3.1)
project(DebugLog)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE INC inc/*)

add_subdirectory(ext)
add_library(DebugLog STATIC ${SRC} ${INC})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC inc/)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ext)