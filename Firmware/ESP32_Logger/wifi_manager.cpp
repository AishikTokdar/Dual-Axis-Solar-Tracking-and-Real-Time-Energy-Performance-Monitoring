#include "wifi_manager.h"
#include "config.h"
#include <WiFi.h>
#if __has_include("secrets.h")
#include "secrets.h"
#else
#include "secrets.example.h"
#endif
static uint32_t last_attempt = 0;
void Network_Begin() {
    WiFi.mode(WIFI_STA); WiFi.setAutoReconnect(true);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); last_attempt = millis();
}
void Network_Update() {
    static bool printed = false;
    if (WiFi.status() == WL_CONNECTED && !printed) {
        Serial.print("Dashboard: http://"); Serial.println(WiFi.localIP());
        printed = true;
    } else if (WiFi.status() != WL_CONNECTED && millis() - last_attempt >= WIFI_RETRY_MS) {
        last_attempt = millis(); WiFi.reconnect();
    }
}
