# Calibration

## Light sensing and actuator commands

1. Verify each quadrant by shading one LDR at a time in the debugger.
2. Illuminate all four sensors uniformly and verify divider balance before tuning.
3. Confirm safe servo pulse limits unloaded (`PAN_MIN`/`PAN_MAX`, `TILT_MIN`/`TILT_MAX`).

## Voltage and current

1. Confirm fitted shunt resistance (`0.1 ohm`) and I2C address (`0x40`).
2. Compare voltage and current against a calibrated reference meter using `calibration_template.csv`.
