#include <WiFi.h>
#include "power_monitor.h"
#include "wifi_manager.h"
#include "web_server.h"
static uint32_t last_sample=0;
void setup() {
    Serial.begin(115200);
    PowerMonitor_Begin(); Network_Begin(); Web_Begin();
    Serial.println("Time_s,Voltage_V,Current_mA,Power_mW,Energy_Wh,Valid");
}
void loop() {
    uint32_t now=millis();
    if (now-last_sample>=SAMPLE_INTERVAL_MS) {
        last_sample=now;
        PowerSample sample=PowerMonitor_Read(now);
        logger.ingest(sample);
        if(sample.valid) Serial.printf("%.3f,%.5f,%.5f,%.5f,%.7f,1\n",now/1000.0,sample.voltage_v,sample.current_ma,sample.power_mw,logger.device_energy.watt_hours);
        else Serial.printf("%.3f,,,,%.7f,0\n",now/1000.0,logger.device_energy.watt_hours);
    }
    Network_Update(); Web_Update(); delay(1);
}
