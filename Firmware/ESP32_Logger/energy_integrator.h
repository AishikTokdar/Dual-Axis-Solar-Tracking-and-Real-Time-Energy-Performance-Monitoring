#pragma once
#include <stdint.h>
#include <math.h>
struct EnergyIntegrator {
    double watt_hours=0;
    uint32_t previous_ms=0, covered_ms=0, gaps=0;
    float previous_w=0;
    bool previous_valid=false, has_previous=false;
    void reset() { watt_hours=0; previous_ms=covered_ms=gaps=0; previous_w=0; previous_valid=false; has_previous=false; }
    void update(uint32_t now, float watts, bool valid, uint32_t max_gap_ms) {
        valid=valid && isfinite(watts) && watts>=0;
        if (has_previous) {
            uint32_t dt=now-previous_ms;
            if (dt>0) {
                if (valid && previous_valid && dt<=max_gap_ms) {
                    watt_hours += ((double)previous_w+watts)*0.5*(double)dt/3600000.0;
                    covered_ms += dt;
                } else { ++gaps; }
            }
        }
        previous_ms=now; previous_w=valid ? watts : 0;
        previous_valid=valid; has_previous=true;
    }
};
