# My CLAP Plugin

A simple CLAP plugin implementation using C++ with functional programming principles.

## Project Structure

```
.
├── CMakeLists.txt          # Main CMake build file
├── README.md               # This file
├── include/                # Header files
│   ├── MyPlugin.h          # Main plugin class definition
│   ├── MyPluginData.h      # Plugin data structure
│   └── DSPProcessor.h      # DSP processing functions
├── src/                    # Source files
│   ├── plugin.cpp          # C entry point and wrapper
│   ├── MyPlugin.cpp        # Main plugin implementation  
│   └── DSPProcessor.cpp    # DSP processor implementation
├── test/                   # Unit tests
│   ├── CMakeLists.txt      # Test build configuration
│   ├── test_main.cpp       # Test harness
│   └── test_DSPProcessor.cpp # DSP processor tests
└── build/                  # Build output directory (created by cmake)
```

## Implementation Approach

This plugin uses a hybrid approach that combines:
1. **C ABI compatibility** for the main plugin interface 
2. **C++ features** for internal implementation (RAII, functional patterns)
3. **Functional programming principles** in DSP processing
4. **TDD testing** via unit tests

### Key Features

- **C ABI Wrapper Layer**: All plugin callbacks are implemented as C functions that delegate to C++ methods
- **Functional DSP Processing**: Pure functions for gain processing and filtering  
- **Testability**: DSP functions can be easily unit tested
- **RAII Resource Management**: Automatic memory management for internal state
- **Extensible Design**: Functional composition patterns support future enhancements

## Building

### Prerequisites
- CMake 3.16 or higher
- A C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CLAP development headers installed

### Build Steps

```bash
# Create build directory and configure
mkdir build && cd build
cmake ..

# Build the plugin
make

# The resulting .clap plugin file will be in the build directory
```

## Testing

Unit tests are written with Check framework and can be built and run as:

```bash
cd build
make
cd test
./plugin_tests
```

## Next Steps

1. Implement more complex DSP processing logic
2. Add additional parameters to the plugin  
3. Integrate with CLAP host extensions
4. Add more comprehensive unit tests
5. Implement plugin state saving/loading (if needed)
6. Add GUI support through CLAP's GUI extension

## Design Philosophy

This implementation demonstrates:
- **Functional-first approach** for DSP logic where possible
- **Test-driven development** with pure functions that are easy to test
- **Clean separation** between C ABI interface and C++ internals 
- **C++ benefits** like RAII, const correctness, and higher-order functions without breaking compatibility