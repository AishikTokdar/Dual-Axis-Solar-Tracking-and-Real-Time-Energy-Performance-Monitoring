import argparse
import csv
import json
from pathlib import Path


def load_session(path, max_gap_s=2.5):
    with Path(path).open(newline="", encoding="utf-8-sig") as f:
        rows = list(csv.DictReader(f))
    total = covered = 0.0
    excluded = 0
    for a, b in zip(rows, rows[1:]):
        dt = float(b["Time_s"]) - float(a["Time_s"])
        if a["Valid"] == "1" and b["Valid"] == "1" and dt <= max_gap_s:
            total += (float(a["Power_mW"]) + float(b["Power_mW"])) * 0.5 * dt / 3600000
            covered += dt
        else:
            excluded += 1
    valid_power = [float(r["Power_mW"]) for r in rows if r["Valid"] == "1"]
    return dict(
        samples=len(rows),
        valid_samples=len(valid_power),
        elapsed_s=float(rows[-1]["Time_s"]) - float(rows[0]["Time_s"]),
        covered_s=covered,
        excluded_intervals=excluded,
        energy_wh=total,
        peak_power_mw=max(valid_power),
        time_weighted_mean_power_mw=total * 3600000 / covered,
        rows=rows,
    )


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
    result = {"tracker": {k: v for k, v in t.items() if k != "rows"}, "fixed": {k: v for k, v in f.items() if k != "rows"}, "gross_energy_gain_pct": gain}
    (args.output / "summary.json").write_text(json.dumps(result, indent=2) + "
")


if __name__ == "__main__":
    main()
