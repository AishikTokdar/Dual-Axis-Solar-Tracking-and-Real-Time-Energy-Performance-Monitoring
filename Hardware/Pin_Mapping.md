# Pin mapping

## STM32 tracker

| Signal | MCU pin | Peripheral | Connection |
|---|---|---|---|
| LDR top-left | PA4 | ADC1_IN4 | Divider midpoint |
| LDR top-right | PA1 | ADC1_IN1 | Divider midpoint |
| LDR bottom-left | PA5 | ADC1_IN5 | Divider midpoint |
| LDR bottom-right | PA0 | ADC1_IN0 | Divider midpoint |
| Pan servo | PA8 | TIM1_CH1 | Servo signal |
| Tilt servo | PA10 | TIM1_CH3 | Servo signal |
| Debug transmit | PA2 | USART2_TX | Board virtual COM interface |
| Debug receive | PA3 | USART2_RX | Board virtual COM interface |

LDR dividers use the 3.3 V domain. Servo supply comes from the external 5 V rail. Connect servo signal ground to tracker ground. Match labels to physical sensor quadrants before testing direction. ADC sample order in the application is TL, TR, BL, BR.

## ESP32 logger

| Signal | Pin | Connection |
|---|---|---|
| INA226 SDA | GPIO21 | I2C data |
| INA226 SCL | GPIO22 | I2C clock |
| INA226 VCC | 3V3 | Sensor supply and compatible bus levels |
| INA226 GND | GND | Measurement reference |
| INA226 address | 0x40 | Configured address; verify module straps |

Wire panel positive through the shunt input to the load according to the breakout pin labels. Connect VBUS to the appropriate monitored positive node. Keep the load return and INA226 reference consistent with the schematic. Check monitor polarity against a reference meter before collecting data.

The two MCUs have independent roles and no telemetry bridge. Electrical isolation depends on the physical supplies and wiring; it is not implied by using two boards.

Source: STM32 `.ioc`, HAL MSP initialization, logger configuration, and [existing schematic](Schematics/hardware_schematic.pdf).
