
# Check if examples are enabled
if(NOT OPTION_BUILD_EXAMPLES)
    return()
endif()

# Example applications
add_subdirectory(cppfs-ls)
add_subdirectory(cppfs-cat)
add_subdirectory(cppfs-cp)
add_subdirectory(cppfs-ln)
add_subdirectory(cppfs-tree)
add_subdirectory(cppfs-sync)
add_subdirectory(cppfs-watch)
