# Calibration

## Light sensing and actuator commands

1. Verify each physical quadrant by shading one LDR at a time and observing its ADC value in the debugger.
2. Illuminate all four sensors uniformly. Record mean and spread; correct wiring or resistor mismatch before tuning.
3. Confirm the expected pan and tilt directions using a small light offset. Reverse the relevant command direction only if the physical assembly requires it.
4. Determine safe pulse limits with the mechanism unloaded. The configured ranges are 500-2000 us for pan and 500-1300 us for tilt; they are command limits inherited from the project, not universally safe servo specifications.
5. Tune PID and low-light thresholds on the assembled mechanism. Record the firmware revision and resulting constants.

## Voltage and current

1. Confirm the fitted shunt value and I2C address. The configured values are 0.1 ohm and 0x40.
2. Compare voltage against a reference meter at several operating points relevant to the panel.
3. Compare current through a known load against the reference meter. Verify polarity and check the near-zero offset.
4. Record device reading, reference reading, absolute error, and relative error where the reference is nonzero.
5. Retain raw calibration observations before reporting accuracy. A library range setting alone does not constitute a measured calibration result.

Use [calibration template](calibration_template.csv). No physical calibration readings were supplied with the source files.
