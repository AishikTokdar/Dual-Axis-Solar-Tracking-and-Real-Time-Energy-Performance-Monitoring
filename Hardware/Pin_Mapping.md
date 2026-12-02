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

## ESP32 logger

| Signal | Pin | Connection |
|---|---|---|
| INA226 SDA | GPIO21 | I2C data |
| INA226 SCL | GPIO22 | I2C clock |
| INA226 address | 0x40 | Breakout default |
