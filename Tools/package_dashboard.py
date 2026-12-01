from pathlib import Path
import argparse
root = Path(__file__).resolve().parents[1]
def content():
    html = (root / "Dashboard/index.html").read_text(encoding="utf-8")
    html = html.replace('<link rel="stylesheet" href="styles.css">', '<style>' + (root / "Dashboard/styles.css").read_text(encoding="utf-8") + '</style>')
    html = html.replace('<script src="app.js"></script>', '<script>' + (root / "Dashboard/app.js").read_text(encoding="utf-8") + '</script>')
    return '#pragma once\n#include <pgmspace.h>\nconst char DASHBOARD_HTML[] PROGMEM = R"SOLARHTML(' + html + ')SOLARHTML";\n'
if __name__ == "__main__":
    parser=argparse.ArgumentParser();parser.add_argument("--check",action="store_true");args=parser.parse_args()
    target=root/"Firmware/ESP32_Logger/dashboard_generated.h"
    if args.check:
        if not target.exists() or target.read_text(encoding="utf-8") != content(): raise SystemExit("Embedded dashboard is out of date")
        print("Embedded dashboard matches source")
    else:
        target.write_text(content(),encoding="utf-8");print("Embedded dashboard updated")
