# Repository tools

Run these scripts from the repository root.

| Script | Purpose | Requirements |
|---|---|---|
| `capture_session.py` | Capture fresh ESP32 `/data` snapshots to a host CSV beyond the 3,600-sample RAM limit | Python 3 and ESP32 on the local network |
| `package_dashboard.py` | Embed editable `Dashboard/` sources into `Firmware/ESP32_Logger/dashboard_generated.h`; `--check` verifies parity | Python 3 |

## Long-duration host capture

```powershell
python Tools/capture_session.py --url http://<device-ip> --duration 10800 --output Data/Raw/local/tracker.csv --metadata-output Data/Raw/local/tracker.capture.json
```

`capture_session.py` uses exclusive file creation so existing session logs are never overwritten, validates incoming JSON snapshots, detects device reboots or counter resets, and writes a companion `.capture.json` provenance sidecar.
