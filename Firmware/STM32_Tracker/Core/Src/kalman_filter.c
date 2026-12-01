#include "kalman_filter.h"
float Kalman_Update(Kalman1D *f, float z) {
    if (!f->initialized) { f->x = z; f->p = 1.0f; f->initialized = 1; }
    f->p += 2.0f;
    float gain = f->p / (f->p + 200.0f);
    f->x += gain * (z - f->x);
    f->p *= 1.0f - gain;
    return f->x;
}
