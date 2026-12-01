#ifndef TRACKING_CONTROL_H
#define TRACKING_CONTROL_H
#include <stdint.h>
#include "kalman_filter.h"
typedef struct {
    float integral, previous_error, previous_measurement, derivative, previous_derivative;
    uint8_t active, confirm;
} AxisState;
typedef struct {
    Kalman1D filters[4];
    AxisState pan, tilt;
    float target_pan, target_tilt, pan_us, tilt_us;
    float ambient, pan_error, tilt_error;
    uint8_t valid, low_light;
} TrackerState;
void Tracker_Init(TrackerState *state);
void Tracker_Update(TrackerState *state, const uint16_t readings[4], uint8_t valid, float dt);
#endif
