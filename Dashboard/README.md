# Local monitoring dashboard

Edit `index.html`, `styles.css`, and `app.js`, then run:

```powershell
python Tools/package_dashboard.py
```

The generated `Firmware/ESP32_Logger/dashboard_generated.h` is the embedded artifact. Build it with the firmware; the source page is not separately hosted on the device. `python Tools/package_dashboard.py --check` verifies consistency.


The live page polls `/data`; samples, energy integration, and recording are controlled by firmware. Start, stop, and reset use POST requests. Stop before CSV export. A full buffer stops recording and shows its state. Export and reset before starting another session through the interface.

Access the device using its displayed IP address. State-changing requests require `X-Solar-Action: 1`; browser cross-origin requests are denied. The bodyless API rejects uploads and request bodies, limits request headers to 2,047 bytes, and closes incomplete requests after one second. `/start` refuses to replace an existing recording until it is reset. These checks do not authenticate other clients on the local network; use a trusted Wi-Fi network.

Readings become unavailable when invalid or stale. The device has no controller telemetry bridge, so the dashboard does not claim measured panel angle or servo position.
