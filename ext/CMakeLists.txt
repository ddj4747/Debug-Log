add_subdirectory(fmt)
add_library(Debug-Log-EXT-Interface INTERFACE)
target_link_libraries(Debug-Log-EXT-Interface INTERFACE fmt::fmt)