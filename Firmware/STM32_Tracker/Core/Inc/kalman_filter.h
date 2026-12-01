#ifndef KALMAN_FILTER_H
#define KALMAN_FILTER_H
#include <stdint.h>
typedef struct { float x, p; uint8_t initialized; } Kalman1D;
float Kalman_Update(Kalman1D *filter, float measurement);
#endif
