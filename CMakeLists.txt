cmake_minimum_required(VERSION 3.16)
project(MyClapPlugin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find CLAP
find_package(PkgConfig REQUIRED)
pkg_check_modules(CLAP REQUIRED clap)

# Create the plugin library
add_library(my_clap_plugin SHARED
    src/plugin.cpp
    src/MyPlugin.cpp
    src/DSPProcessor.cpp
)

# Set include directories
target_include_directories(my_clap_plugin PRIVATE
    include
    ${CLAP_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(my_clap_plugin 
    ${CLAP_LIBRARIES}
)

# Define plugin-specific variables
set_target_properties(my_clap_plugin PROPERTIES
    PREFIX ""
    SUFFIX ".clap"
)