#include "power_monitor.h"
#include <Wire.h>
#include <INA226_WE.h>
class CheckedINA226 : public INA226_WE {
public:
    CheckedINA226() : INA226_WE(::INA226_ADDRESS) {}
    bool readChecked(uint8_t reg, uint16_t &value) {
        Wire.beginTransmission(::INA226_ADDRESS);
        Wire.write(reg);
        if (Wire.endTransmission(false)!=0) return false;
        if (Wire.requestFrom(::INA226_ADDRESS,static_cast<uint8_t>(2))!=2) {
            while (Wire.available()) Wire.read();
            return false;
        }
        value=static_cast<uint16_t>(Wire.read())<<8;
        value|=static_cast<uint16_t>(Wire.read());
        return true;
    }
    bool configured() {
        uint16_t calibration,configuration;
        return readChecked(INA226_CAL_REG,calibration) && calibration==calVal &&
            readChecked(INA226_CONF_REG,configuration) &&
            (configuration&0x0E07)==(static_cast<uint16_t>(AVERAGE_16)|0x0007);
    }
    bool output(float &voltage, float &current) {
        uint16_t flags,bus,raw;
        if (!configured() || !readChecked(INA226_MASK_EN_REG,flags) || (flags&0x0004) ||
            !readChecked(INA226_BUS_REG,bus) || !readChecked(INA226_CURRENT_REG,raw)) return false;
        voltage=bus*0.00125f;
        current=static_cast<int16_t>(raw)/currentDivider_mA;
        return true;
    }
};
static CheckedINA226 monitor;
static bool ready=false;
static uint32_t sequence=0, last_retry=0;
void PowerMonitor_Begin() {
    Wire.begin(I2C_SDA,I2C_SCL);
    Wire.setTimeOut(20);
    ready=monitor.init();
    if (ready) {
        monitor.setResistorRange(SHUNT_OHMS,MAX_CURRENT_A);
        monitor.setAverage(AVERAGE_16);
        ready=monitor.configured();
    }
}
PowerSample PowerMonitor_Read(uint32_t now) {
    PowerSample s; s.id=++sequence; s.uptime_ms=now;
    if (!ready) {
        if (now-last_retry>=5000) { last_retry=now; PowerMonitor_Begin(); }
        return s;
    }
    if (!monitor.output(s.voltage_v,s.current_ma)) {
        ready=false; s.voltage_v=s.current_ma=NAN; return s;
    }
    s.power_mw=s.voltage_v*s.current_ma;
    s.valid=isfinite(s.voltage_v) && isfinite(s.current_ma) &&
        s.voltage_v>=0 && s.current_ma>=0 && s.current_ma<=MAX_CURRENT_A*1000;
    if (!s.valid) s.voltage_v=s.current_ma=s.power_mw=NAN;
    return s;
}
