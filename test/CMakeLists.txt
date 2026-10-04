# Test CMakeLists.txt for MyClapPlugin

# Add a test executable that uses Check framework
add_executable(plugin_tests
    test_main.cpp
    test_DSPProcessor.cpp
)

# Link against our plugin library 
target_link_libraries(plugin_tests 
    my_clap_plugin
    check
)

# Set include directories for tests
target_include_directories(plugin_tests PRIVATE
    include
    ../include
)

# Add test target
add_test(NAME plugin_tests COMMAND plugin_tests)