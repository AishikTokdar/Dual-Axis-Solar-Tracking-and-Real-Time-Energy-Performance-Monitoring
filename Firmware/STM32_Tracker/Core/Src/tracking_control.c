#include "tracking_control.h"
#include "tracker_config.h"
#include <math.h>
#include <string.h>
static float clamp(float x, float lo, float hi) { return x < lo ? lo : (x > hi ? hi : x); }
static void idle(AxisState *a, float measurement) {
    memset(a, 0, sizeof(*a)); a->previous_measurement = measurement;
}
static float axis_step(AxisState *a, float measurement, float dt, float tolerance) {
    float error = -measurement;
    if (a->active && fabsf(error) <= tolerance * EXIT_RATIO) {
        idle(a, measurement); return 0.0f;
    }
    if (!a->active) {
        a->previous_measurement = measurement;
        if (fabsf(error) >= tolerance) {
            if (++a->confirm < CONFIRM_THRESH) return 0.0f;
            a->active = 1; a->previous_error = error;
        } else { a->confirm = 0; return 0.0f; }
    }
    a->integral = clamp(a->integral + 0.5f * (error + a->previous_error) * dt, -20.0f, 20.0f);
    float raw = -(measurement - a->previous_measurement) / dt;
    float k = 0.075f;
    a->derivative = (1.0f-k)/(1.0f+k)*a->derivative + k/(1.0f+k)*(raw+a->previous_derivative);
    a->previous_derivative = raw;
    a->previous_measurement = measurement;
    a->previous_error = error;
    return -(0.0045f*error + 0.002f*a->integral + 0.0005f*a->derivative);
}
static float move(AxisState *a, float target, float delta, float lo, float hi) {
    float next = target + clamp(delta, -MAX_DELTA_PER_LOOP, MAX_DELTA_PER_LOOP);
    if ((next > hi && a->integral < 0) || (next < lo && a->integral > 0)) a->integral = 0;
    return clamp(next, lo, hi);
}
void Tracker_Init(TrackerState *s) {
    memset(s, 0, sizeof(*s));
    s->target_pan = s->pan_us = PAN_HOME;
    s->target_tilt = s->tilt_us = TILT_HOME;
}
void Tracker_Update(TrackerState *s, const uint16_t v[4], uint8_t valid, float dt) {
    for (unsigned i=0; i<4; ++i) if (v[i] > 4095U) valid = 0;
    s->valid = valid && isfinite(dt) && dt > 0.0f && dt <= 0.1f;
    if (!s->valid) {
        idle(&s->pan,0); idle(&s->tilt,0);
        s->target_pan=s->pan_us; s->target_tilt=s->tilt_us;
        for (unsigned i=0;i<4;++i) s->filters[i].initialized=0;
        return;
    }
    float f[4]; for (unsigned i=0;i<4;++i) f[i] = Kalman_Update(&s->filters[i],v[i]);
    s->ambient = (f[0]+f[1]+f[2]+f[3])*0.25f;
    s->pan_error = (f[1]+f[3]-f[0]-f[2])*0.5f;
    s->tilt_error = (f[2]+f[3]-f[0]-f[1])*0.5f;
    s->low_light = s->ambient < LOW_LIGHT_ADC;
    if (s->low_light) {
        idle(&s->pan,s->pan_error); idle(&s->tilt,s->tilt_error);
        s->target_pan=s->pan_us; s->target_tilt=s->tilt_us; return;
    }
    float tolerance = TOL_MIN + clamp(s->ambient/4095.0f,0,1)*(TOL_MAX-TOL_MIN);
    s->target_pan = move(&s->pan,s->target_pan,axis_step(&s->pan,s->pan_error,dt,tolerance),PAN_MIN,PAN_MAX);
    s->target_tilt = move(&s->tilt,s->target_tilt,axis_step(&s->tilt,s->tilt_error,dt,tolerance),TILT_MIN,TILT_MAX);
    s->pan_us += SMOOTH_ALPHA*(s->target_pan-s->pan_us);
    s->tilt_us += SMOOTH_ALPHA*(s->target_tilt-s->tilt_us);
}
