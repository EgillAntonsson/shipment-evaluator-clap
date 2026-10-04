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