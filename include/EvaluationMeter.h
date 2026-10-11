#pragma once

#include <algorithm>
#include <cmath>

inline constexpr double kEvaluationMeterMaximum = 11.0;
inline constexpr double kEvaluationMeterStep = 0.025;
inline constexpr double kEvaluationMeterHalfTurnRadians = 3.14159265358979323846;

inline double clamp_evaluation_meter(double meter) {
    if (!std::isfinite(meter))
        return 0.0;
    return std::clamp(meter, 0.0, kEvaluationMeterMaximum);
}

inline bool evaluation_is_complete(double meter) {
    return clamp_evaluation_meter(meter) >= kEvaluationMeterMaximum;
}

inline double advance_evaluation_meter(double meter, bool is_playing) {
    if (!is_playing)
        return 0.0;
    return std::min(kEvaluationMeterMaximum,
                    clamp_evaluation_meter(meter) + kEvaluationMeterStep);
}

inline double evaluation_meter_angle(double meter) {
    const double normalized =
        clamp_evaluation_meter(meter) / kEvaluationMeterMaximum;
    return kEvaluationMeterHalfTurnRadians * (1.0 - normalized);
}
