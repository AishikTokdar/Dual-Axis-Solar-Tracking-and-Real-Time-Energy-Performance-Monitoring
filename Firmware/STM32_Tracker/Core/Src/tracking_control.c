#include "tracking_control.h"
#include "tracker_config.h"
#include <math.h>
#include <string.h>

static float clamp(float x, float lo, float hi) { return x < lo ? lo : (x > hi ? hi : x); }

static float axis_step(AxisState *a, float measurement, float dt) {
    float error = -measurement;
    if (fabsf(error) < DEADBAND_ADC) {
        a->active = 0;
        a->integral = 0.0f;
        a->previous_error = error;
        return 0.0f;
    }
    a->active = 1;
    a->integral = clamp(a->integral + 0.5f * (error + a->previous_error) * dt, -20.0f, 20.0f);
    float raw = -(measurement - a->previous_measurement) / dt;
    a->previous_measurement = measurement;
    a->previous_error = error;
    return -(0.0045f * error + 0.002f * a->integral + 0.0005f * raw);
}

void Tracker_Init(TrackerState *s) {
    memset(s, 0, sizeof(*s));
    s->target_pan = s->pan_us = PAN_HOME;
    s->target_tilt = s->tilt_us = TILT_HOME;
}

void Tracker_Update(TrackerState *s, const uint16_t v[4], uint8_t valid, float dt) {
    for (unsigned i = 0; i < 4; ++i) if (v[i] > 4095U) valid = 0;
    s->valid = valid && isfinite(dt) && dt > 0.0f;
    if (!s->valid) return;
    float f[4];
    for (unsigned i = 0; i < 4; ++i) f[i] = Kalman_Update(&s->filters[i], (float)v[i]);
    s->ambient = (f[0] + f[1] + f[2] + f[3]) * 0.25f;
    s->pan_error = (f[1] + f[3] - f[0] - f[2]) * 0.5f;
    s->tilt_error = (f[2] + f[3] - f[0] - f[1]) * 0.5f;
    s->target_pan = clamp(s->target_pan + clamp(axis_step(&s->pan, s->pan_error, dt), -MAX_DELTA_PER_LOOP, MAX_DELTA_PER_LOOP), PAN_MIN, PAN_MAX);
    s->target_tilt = clamp(s->target_tilt + clamp(axis_step(&s->tilt, s->tilt_error, dt), -MAX_DELTA_PER_LOOP, MAX_DELTA_PER_LOOP), TILT_MIN, TILT_MAX);
    s->pan_us += SMOOTH_ALPHA * (s->target_pan - s->pan_us);
    s->tilt_us += SMOOTH_ALPHA * (s->target_tilt - s->tilt_us);
}
