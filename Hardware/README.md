# Hardware

The assembly has a tracker board and an electrical measurement board. The STM32 reads four light dividers and drives two servos. The ESP32 reads one INA226 channel across a fixed resistive load.

- [Bill of materials](BOM.csv)
- [Pin mapping and voltage domains](Pin_Mapping.md)
- [Calibration procedure](Calibration.md)
- [Calibration record template](calibration_template.csv)
- [Original two-page schematic](Schematics/hardware_schematic.pdf)
- [CAD attribution and assembly information](../CAD/README.md)

One configured power-monitor channel supports sequential panel measurements. Concurrent tracked/fixed measurement requires a second channel and matched load. Servo/control consumption is outside the existing panel-output measurement boundary.
