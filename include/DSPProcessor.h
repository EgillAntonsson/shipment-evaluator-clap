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