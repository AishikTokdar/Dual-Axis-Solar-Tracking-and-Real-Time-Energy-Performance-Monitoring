import argparse
import csv
import json
import time
import urllib.request
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description="Capture /data snapshots to host CSV")
    parser.add_argument("--url", required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--duration", type=float, default=10800)
    args = parser.parse_args()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    last_id = None
    start_ms = None
    deadline = time.monotonic() + args.duration
    with args.output.open("x", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["Time_s", "Voltage_V", "Current_mA", "Power_mW", "Energy_Wh", "Valid"])
        while time.monotonic() < deadline:
            with urllib.request.urlopen(args.url.rstrip("/") + "/data", timeout=3) as resp:
                d = json.loads(resp.read())
            if d.get("sample_id", 0) > 0 and d["sample_id"] != last_id:
                last_id = d["sample_id"]
                if start_ms is None:
                    start_ms = d["uptime_ms"]
                t_s = (d["uptime_ms"] - start_ms) / 1000.0
                writer.writerow([
                    f"{t_s:.3f}",
                    d["voltage_v"] if d["valid"] else "",
                    d["current_ma"] if d["valid"] else "",
                    d["power_mw"] if d["valid"] else "",
                    d["device_energy_wh"],
                    int(d["valid"]),
                ])
                f.flush()
            time.sleep(0.4)


if __name__ == "__main__":
    main()
