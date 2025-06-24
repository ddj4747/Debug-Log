# 🔗 Fetch Content Example
```cmake
cmake_minimum_required(VERSION 3.20)
project(ExampleProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch the Debug-Log library
FetchContent_Declare(
Debug-Log
GIT_REPOSITORY https://github.com/ddj4747/Debug-Log.git
GIT_TAG main  # Use a specific tag or commit hash for reproducibility
)
FetchContent_MakeAvailable(Debug-Log)

# Add your executable
add_executable(example main.cpp)

# Link the Debug-Log target
target_link_libraries(example PRIVATE Debug-Log)
```