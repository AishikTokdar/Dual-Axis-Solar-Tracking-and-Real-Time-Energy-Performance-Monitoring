# Dual-Axis Solar Tracking and Real-Time Energy Performance Monitoring

A dual-microcontroller prototype combining STM32 LDR pan-tilt tracking with ESP32 INA226 electrical monitoring.

**Author:** Aishik Tokdar

## Structure

- `Firmware/STM32_Tracker/` - NUCLEO-G031K8 ADC1 LDR feedback and TIM1 servo PWM control
- `Firmware/ESP32_Logger/` - ESP32 INA226 power monitoring
- `Hardware/` - Schematic PDF and BOM
- `CAD/` - Third-party pan-tilt STL mount parts

## Quick build

```powershell
python -m platformio run -d Firmware/STM32_Tracker
python -m platformio run -e esp32_logger
```
