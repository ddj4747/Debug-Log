add_subdirectory(fmt)
add_library(ext INTERFACE)
target_link_libraries(ext INTERFACE fmt::fmt)