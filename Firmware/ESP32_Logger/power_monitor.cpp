#include "power_monitor.h"
#include <Wire.h>
#include <INA226_WE.h>
static INA226_WE monitor(INA226_ADDRESS);
static bool ready = false;
static uint32_t sequence = 0;
void PowerMonitor_Begin() {
    Wire.begin(I2C_SDA, I2C_SCL);
    ready = monitor.init();
    if (ready) {
        monitor.setResistorRange(SHUNT_OHMS, MAX_CURRENT_A);
        monitor.setAverage(AVERAGE_16);
    }
}
PowerSample PowerMonitor_Read(uint32_t now) {
    PowerSample s; s.id = ++sequence; s.uptime_ms = now;
    if (!ready) return s;
    s.voltage_v = monitor.getBusVoltage_V();
    s.current_ma = monitor.getCurrent_mA();
    s.power_mw = s.voltage_v * s.current_ma;
    s.valid = isfinite(s.voltage_v) && isfinite(s.current_ma) &&
        s.voltage_v >= 0 && s.current_ma >= 0 && s.current_ma <= MAX_CURRENT_A * 1000;
    if (!s.valid) s.voltage_v = s.current_ma = s.power_mw = NAN;
    return s;
}
