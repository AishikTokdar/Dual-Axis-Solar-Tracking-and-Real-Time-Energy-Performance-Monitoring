# Measurement schema

| Column | Unit | Meaning |
|---|---|---|
| `Time_s` | s | Monotonic elapsed time; device session exports start at the first recorded sample |
| `Voltage_V` | V | INA226 bus voltage |
| `Current_mA` | mA | Shunt-derived output current |
| `Power_mW` | mW | Voltage multiplied by current in mA |
| `Energy_Wh` | Wh | Integrated valid energy; session total for device exports, device total for host captures |
| `Valid` | 0 or 1 | Whether electrical values can be used |

Invalid electrical readings use blank CSV cells and `Valid=0`. Their energy accumulator is retained, while integration across invalid samples is excluded. Gaps longer than 2.5 s are excluded from integration.

Device `/data` additionally exposes a per-boot identifier (`boot_id`), sample sequence, uptime, sample age, recording state, capacity, rolling graph array, covered time, and gap count. Host captures (`Tools/capture_session.py`) unwrap uptime rollover, normalize time to the first received sample, and abort on boot-identifier changes, sequence regressions, or energy resets.

## Acquisition metadata

Use `Data/session_metadata.example.json` to document each experiment run (`protocol`, UTC start times with timezone, panel identifiers, load and shunt values, fixed-panel tilt/azimuth, weather/shading notes, firmware build, and SHA-256 checksums). Leave unknown fields as `null`.
