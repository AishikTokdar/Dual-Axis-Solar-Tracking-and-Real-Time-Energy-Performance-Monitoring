# Measurement schema

| Column | Unit | Meaning |
|---|---|---|
| `Time_s` | s | Monotonic elapsed time |
| `Voltage_V` | V | INA226 bus voltage |
| `Current_mA` | mA | Shunt-derived output current |
| `Power_mW` | mW | Voltage multiplied by current in mA |
| `Energy_Wh` | Wh | Integrated valid energy |
| `Valid` | 0 or 1 | Whether electrical values can be used |

Invalid readings leave electrical fields blank and set `Valid=0`.
