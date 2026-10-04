#include "DSPProcessor.h"
#include <cmath>

float DSPProcessor::process_gain(float input, float gain_db) {
    // Pure function - same inputs always produce same outputs
    double gain = std::pow(10.0, gain_db / 20.0);
    return static_cast<float>(input * gain);
}

float DSPProcessor::apply_lowpass(float input, float cutoff, float sample_rate) {
    // Simplified lowpass filter example
    // In a real plugin this would be more complex and properly implemented
    if (cutoff >= sample_rate / 2.0f) {
        return input; // No filtering if cutoff is at Nyquist frequency or above
    }
    
    // Return input unchanged for simple example
    return input;
}

std::function<float(float)> DSPProcessor::make_gain_processor(float gain_db) {
    // Higher-order function - returns a callable function 
    return [gain_db](float input) -> float {
        return process_gain(input, gain_db);
    };
}