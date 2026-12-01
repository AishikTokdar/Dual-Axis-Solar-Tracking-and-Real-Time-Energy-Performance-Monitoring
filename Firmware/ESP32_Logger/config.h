#pragma once
#include <stdint.h>
constexpr uint8_t I2C_SDA = 21, I2C_SCL = 22, INA226_ADDRESS = 0x40;
constexpr uint32_t SAMPLE_INTERVAL_MS = 1000;
constexpr uint32_t WIFI_RETRY_MS = 15000;
constexpr unsigned LOG_CAPACITY = 3600, GRAPH_CAPACITY = 60;
constexpr float PANEL_RATED_W = 0.9f, SHUNT_OHMS = 0.1f, MAX_CURRENT_A = 1.0f;
