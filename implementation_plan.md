# Simple CLAP Plugin Implementation Plan (C++ with Functional Approach)

## Overview

In this plan, we'll implement a simple CLAP plugin using C++, but maintain strict C ABI compatibility for the plugin interface. This approach allows us to:

1. Leverage C++ features like RAII, templates, and functional programming
2. Apply functional programming principles where appropriate
3. Enable unit testing through TDD
4. Maintain maximum host compatibility

## Core Architecture Strategy

### 1. C ABI Wrapper Layer (plugin.cpp)
The main plugin interface will be implemented in C with extern "C" functions that serve as the bridge to our C++ implementation.

### 2. C++ Core Implementation (MyPlugin.h/.cpp)  
The core DSP and logic will be implemented using C++ features:
- RAII for resource management
- Functional programming patterns 
- Testable components

## Implementation Details

### Plugin Data Structure (MyPluginData.h)
```cpp
#pragma once

#include <cstdint>
#include <array>
#include <functional>

struct MyPluginData {
    double sample_rate;
    
    // Parameter values - these will be managed by the host
    std::array<float, 2> parameters;  // Example: gain, filter_cutoff
    
    // Plugin state
    float delay_line[1024];
    int delay_ptr;
    
    // Functional processing components (can be tested independently)
    std::function<float(float, float)> process_function;
};
```

### Core DSP Logic (DSPProcessor.h/.cpp)
```cpp
#pragma once

#include <functional>
#include <vector>

// Pure functions that can be unit tested
class DSPProcessor {
public:
    // Pure function - no side effects, easy to test
    static float process_gain(float input, float gain_db);
    
    // Pure function for filtering
    static float apply_lowpass(float input, float cutoff, float sample_rate);
    
    // Higher order functional patterns (for extensibility)
    static std::function<float(float)> make_gain_processor(float gain_db);
};
```

### Plugin Implementation (MyPlugin.h/.cpp)
```cpp
#pragma once

#include "MyPluginData.h"
#include <clap/clap.h>

class MyPlugin {
private:
    MyPluginData data_;
    
public:
    MyPlugin(const clap_host_t *host);
    ~MyPlugin();
    
    // C ABI compliant interface functions
    static bool init(const struct clap_plugin *plugin);
    static void destroy(const struct clap_plugin *plugin);
    static bool activate(const struct clap_plugin *plugin, 
                        double sample_rate, 
                        uint32_t min_frames_count, 
                        uint32_t max_frames_count);
    static void deactivate(const struct clap_plugin *plugin);
    static bool start_processing(const struct clap_plugin *plugin);
    static void stop_processing(const struct clap_plugin *plugin);
    static void reset(const struct clap_plugin *plugin);
    static clap_process_status process(const struct clap_plugin *plugin,
                                       const clap_process_t *process);
    
    // Access the CLAP interface
    const clap_plugin_t* get_clap_interface() const;
    
private:
    void process_audio(const clap_process_t *process);
    float process_sample(float input, uint32_t param_index);
};
```

### C Wrapper Layer (plugin.cpp)
```cpp
#include "MyPlugin.h"

// Static storage for plugin instance - necessary for C ABI compatibility
static MyPlugin* g_plugin = nullptr;

extern "C" {
    const clap_plugin_t *clap_plugin_create(const clap_host_t *host) {
        g_plugin = new MyPlugin(host);
        return g_plugin->get_clap_interface();
    }
    
    // All other C ABI compatible functions that delegate to MyPlugin methods
}

// Plugin entry point
extern "C" const clap_plugin_entry_t clap_entry = {
    .clap_version = CLAP_VERSION,
    .create = clap_plugin_create,
};
```

## Functional Programming Elements

### 1. Pure Functions for DSP Processing (DSPProcessor.cpp)
```cpp
float DSPProcessor::process_gain(float input, float gain_db) {
    // Pure function - same inputs always produce same outputs
    double gain = pow(10.0, gain_db / 20.0);
    return static_cast<float>(input * gain);
}

std::function<float(float)> DSPProcessor::make_gain_processor(float gain_db) {
    // Higher-order function - returns a callable function 
    return [gain_db](float input) -> float {
        return process_gain(input, gain_db);
    };
}
```

### 2. Functional Composition
```cpp
// Compose multiple processing functions
class ProcessingChain {
public:
    std::function<float(float)> compose(
        const std::vector<std::function<float(float)>>& processors) {
        
        return [processors](float input) -> float {
            float result = input;
            for (const auto& processor : processors) {
                result = processor(result);
            }
            return result;
        };
    }
};
```

### 3. Immutability and Const Correctness
```cpp
class FilterParameters {
public:
    const float cutoff;  
    const float resonance;
    
    // Constructor with initialization list for immutable values
    FilterParameters(float cutoff, float resonance) 
        : cutoff(cutoff), resonance(resonance) {}
        
    // Pure functions that don't modify object state
    float apply_filter(float input) const {
        // Filtering logic that doesn't change parameters
        return input * 0.5f;  // simplified example
    }
};
```

## Testing Strategy (TDD)

### Unit Tests for Pure Functions (using CMock or similar)
```cpp
// Test that gain processing is correct
TEST_CASE("Gain processing with 0 dB") {
    float result = DSPProcessor::process_gain(1.0f, 0.0f);
    CHECK(result == 1.0f);
}

TEST_CASE("Gain processing with 6 dB") {
    float result = DSPProcessor::process_gain(1.0f, 6.0f);
    // Verify result is approximately 1.995 (2^0.5)
    CHECK(abs(result - 1.995f) < 0.01f);
}
```

### Mock Host Interface
```cpp
class MockHost : public clap_host {
public:
    // Implement all expected host callbacks for testing
    void request_callback() override { /* mock implementation */ }
    
    // Override specific methods to verify plugin behavior
};
```

## Benefits of This Approach

1. **C ABI Compatibility**: The plugin interface remains C-compatible, ensuring host compatibility
2. **Functional Programming Patterns**: Pure functions, higher-order functions, immutability 
3. **Testability**: Easy to unit test pure functions with TDD
4. **RAII Integration**: Automatic resource management for plugin state
5. **Extensibility**: Functional composition makes the system modular
6. **Maintainability**: Clear separation of concerns between interface and implementation

## Next Steps

1. Create initial project structure with header files
2. Implement core plugin interface (C wrapper)
3. Build DSP logic using C++ features while keeping ABI clean  
4. Add unit tests for pure functions
5. Integrate with CLAP host discovery system
6. Test in actual audio hosts

This approach gives you the flexibility to use modern C++ features where they add value while maintaining compatibility and enabling thorough testing.