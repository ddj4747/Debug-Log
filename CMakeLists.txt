cmake_minimum_required(VERSION 3.10)
project(Debug-Log)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SRC src/*)
file(GLOB_RECURSE INC inc/*)

add_subdirectory(ext)
add_library(Debug-Log STATIC ${SRC} ${INC})
target_include_directories(Debug-Log PUBLIC inc/)
target_link_libraries(Debug-Log PRIVATE ext)