#include <Wire.h>
#include <INA226_WE.h>

static INA226_WE ina(0x40);
static uint32_t last_ms = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22);
    if (ina.init()) {
        ina.setResistorRange(0.1f, 1.5f);
        ina.setAverage(AVERAGE_16);
    }
    Serial.println("Time_s,Voltage_V,Current_mA,Power_mW");
}

void loop() {
    uint32_t now = millis();
    if (now - last_ms >= 1000) {
        last_ms = now;
        float v = ina.getBusVoltage_V();
        float i = ina.getCurrent_mA();
        Serial.printf("%.3f,%.4f,%.4f,%.4f
", now / 1000.0, v, i, v * i);
    }
    delay(1);
}
