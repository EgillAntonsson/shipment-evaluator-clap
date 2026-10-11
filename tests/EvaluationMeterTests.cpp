#include "EvaluationMeter.h"

#include <cassert>
#include <cmath>
#include <limits>

int main() {
    assert(clamp_evaluation_meter(-1.0) == 0.0);
    assert(clamp_evaluation_meter(5.5) == 5.5);
    assert(clamp_evaluation_meter(12.0) == kEvaluationMeterMaximum);
    assert(clamp_evaluation_meter(std::numeric_limits<double>::quiet_NaN()) == 0.0);

    assert(advance_evaluation_meter(0.0, true) == kEvaluationMeterStep);
    assert(advance_evaluation_meter(10.99, true) == kEvaluationMeterMaximum);
    assert(advance_evaluation_meter(5.0, false) == 0.0);
    assert(advance_evaluation_meter(11.0, false) == 0.0);
    assert(advance_evaluation_meter(11.0, true) == kEvaluationMeterMaximum);

    assert(std::abs(evaluation_meter_angle(0.0) -
                    kEvaluationMeterHalfTurnRadians) < 1e-12);
    assert(std::abs(evaluation_meter_angle(5.5) -
                    kEvaluationMeterHalfTurnRadians * 0.5) < 1e-12);
    assert(std::abs(evaluation_meter_angle(11.0)) < 1e-12);
    assert(evaluation_is_complete(11.0));
    assert(!evaluation_is_complete(10.99));
}
