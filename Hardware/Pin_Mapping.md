# Pin mapping

## STM32 tracker

| Signal | MCU pin | Peripheral | Connection |
|---|---|---|---|
| LDR top-left | PA0 | ADC1_IN0 | Divider midpoint |
| LDR top-right | PA1 | ADC1_IN1 | Divider midpoint |
| LDR bottom-left | PA4 | ADC1_IN4 | Divider midpoint |
| LDR bottom-right | PA5 | ADC1_IN5 | Divider midpoint |
| Pan servo | PA8 | TIM1_CH1 | Servo signal |
| Tilt servo | PA10 | TIM1_CH3 | Servo signal |

## ESP32 logger

| Signal | Pin | Connection |
|---|---|---|
| INA226 SDA | GPIO21 | I2C data |
| INA226 SCL | GPIO22 | I2C clock |
| INA226 address | 0x40 | Breakout default |
