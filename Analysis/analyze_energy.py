import argparse
import csv
import json
from pathlib import Path


def load_session(path):
    with Path(path).open(newline="", encoding="utf-8-sig") as f:
        rows = list(csv.DictReader(f))
    total = covered = 0.0
    for a, b in zip(rows, rows[1:]):
        dt = float(b["Time_s"]) - float(a["Time_s"])
        if a["Valid"] == "1" and b["Valid"] == "1":
            total += (float(a["Power_mW"]) + float(b["Power_mW"])) * 0.5 * dt / 3600000
            covered += dt
    return dict(samples=len(rows), energy_wh=total, covered_s=covered)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--tracker", type=Path, required=True)
    parser.add_argument("--fixed", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    t = load_session(args.tracker)
    f = load_session(args.fixed)
    gain = 100 * (t["energy_wh"] - f["energy_wh"]) / f["energy_wh"]
    args.output.mkdir(parents=True, exist_ok=True)
    (args.output / "summary.json").write_text(json.dumps({"gross_energy_gain_pct": gain}, indent=2))


if __name__ == "__main__":
    main()
