# Dual-Axis Solar Tracking and Real-Time Energy Performance Monitoring

A dual-microcontroller prototype combining STM32 LDR pan-tilt tracking with ESP32 INA226 electrical monitoring, live web dashboard, and analysis scripts.

**Author:** Aishik Tokdar  
**Technologies:** Embedded C, C++, STM32G031K8, ESP32, INA226, PID control, Kalman filtering, MATLAB, Python, HTML/CSS/JavaScript.

## Structure

- `Firmware/STM32_Tracker/` - `ldr_sensors.c`, `kalman_filter.c`, `tracking_control.c`
- `Firmware/ESP32_Logger/` - INA226 power logger, RAM session recorder, and Host/CSRF-hardened `/data` `/csv` HTTP server
- `Dashboard/` - Local live UI packaged via `Tools/package_dashboard.py`
- `Data/` - [Measurement schema](Data/Schema.md) and long-duration sessions via `Tools/capture_session.py`
- `Hardware/` - [Pin mapping](Hardware/Pin_Mapping.md), [BOM](Hardware/BOM.csv), [schematic](Hardware/Schematics/hardware_schematic.pdf)
- `Analysis/` - [Python & MATLAB energy evaluation](Analysis/README.md)

![Tracker wiring](Hardware/Schematics/tracker_wiring.png)

![Logger wiring](Hardware/Schematics/logger_wiring.png)

Copy `Firmware/ESP32_Logger/secrets.example.h` to `Firmware/ESP32_Logger/secrets.h` (git-ignored) before flashing the ESP32.

## Assembly and datasets

![Physical assembly](Images/system_photo.jpeg)

[Project report](Docs/Project_Report.pdf) | [Demonstration video](Videos/system_demonstration.mp4)

Paired 3-hour exports (`10,801` samples) are stored in [Data](Data/README.md). See [Calibration](Hardware/Calibration.md) and the [static project website](Website/dist/index.html) (deploys via [GitHub Actions](.github/workflows/deploy-website.yml)).

## Quick build

```powershell
python Tools/package_dashboard.py
python -m platformio run -d Firmware/STM32_Tracker
python -m platformio run -e esp32_logger
```

**Tracked: 1.4125 Wh vs Fixed: 0.5707 Wh (+147.50% gross energy gain).**

![System architecture](Images/System_Architecture.png)

[Architecture details](Docs/Architecture.md) | [Control math](Docs/Control_Algorithm.md)

![Experimental performance](Analysis/Results/reported_comparison.png)

Run `python Analysis/analyze_energy.py` or `Analysis/MATLAB/analyze_energy_performance.m` (pass `--require-complete-metadata` to enforce resolved metadata and SHA-256 checksums).
