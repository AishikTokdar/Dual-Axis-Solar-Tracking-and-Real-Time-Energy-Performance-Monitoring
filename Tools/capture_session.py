import argparse
import csv
import json
import math
import time
import urllib.request
import hashlib
import sys
from datetime import datetime, timezone
from http.client import HTTPException
from pathlib import Path

def validate_snapshot(d):
    if not isinstance(d,dict): raise ValueError("Expected a device snapshot object")
    for field in ("boot_id","sample_id","uptime_ms","sample_age_ms"):
        value=d.get(field)
        if type(value) is not int or not 0<=value<=0xffffffff:
            raise ValueError("Invalid snapshot field: "+field)
    if type(d.get("valid")) is not bool: raise ValueError("Invalid validity flag")
    fields=["device_energy_wh"]
    if d["valid"]: fields += ["voltage_v","current_ma","power_mw"]
    for field in fields:
        value=d.get(field)
        if type(value) not in (int,float) or value<0 or value>1e308 or not math.isfinite(value):
            raise ValueError("Invalid snapshot field: "+field)
    if d["valid"]:
        expected=float(d["voltage_v"])*float(d["current_ma"])
        if not math.isfinite(expected) or abs(d["power_mw"]-expected)>max(0.1,0.001*d["power_mw"]):
            raise ValueError("Inconsistent electrical measurement")
    return d

def main():
    parser=argparse.ArgumentParser(description="Capture device snapshots to a host CSV without RAM duration limits")
    parser.add_argument("--url",required=True);parser.add_argument("--output",type=Path,required=True)
    parser.add_argument("--metadata-output",type=Path,help="Save capture provenance to a new JSON sidecar")
    parser.add_argument("--context",type=Path,help="Existing JSON acquisition notes to include in the sidecar")
    parser.add_argument("--duration",type=float,default=10800);args=parser.parse_args()
    if not math.isfinite(args.duration) or args.duration<=0: parser.error("duration must be finite and positive")
    context={}
    if args.context:
        if not args.metadata_output: parser.error("--context requires --metadata-output")
        context=json.loads(args.context.read_text(encoding="utf-8"))
        if not isinstance(context,dict): parser.error("context must be a JSON object")
        json.dumps(context,allow_nan=False)
    if args.metadata_output:
        if args.metadata_output.resolve()==args.output.resolve(): parser.error("CSV and metadata output must differ")
        if args.metadata_output.exists(): parser.error("metadata output already exists")
        args.metadata_output.parent.mkdir(parents=True,exist_ok=True)
    args.output.parent.mkdir(parents=True,exist_ok=True)
    provenance={"capture_started_utc":datetime.now(timezone.utc).isoformat(),"first_sample_received_utc":None,
                "capture_method":"host_capture","requested_duration_s":args.duration,"operator_context":context,
                "sample_time_basis":"Time_s=0 is the first accepted snapshot. Host receipt times do not establish synchronized sensor acquisition UTC.",
                "samples":0,"status":"completed"}
    last_id=None;previous_ms=None;previous_boot=None;previous_energy=None;elapsed_ms=0;deadline=time.monotonic()+args.duration
    with args.output.open("x",newline="",encoding="utf-8") as f:
        writer=csv.writer(f);writer.writerow(["Time_s","Voltage_V","Current_mA","Power_mW","Energy_Wh","Valid"])
        try:
            while time.monotonic()<deadline:
                try:
                    with urllib.request.urlopen(args.url.rstrip('/')+'/data',timeout=3) as response:
                        payload=response.read(65537)
                        if len(payload)>65536: raise ValueError("Device snapshot is too large")
                        d=validate_snapshot(json.loads(payload))
                except (OSError,ValueError,HTTPException) as e:
                    print("Capture gap:",e)
                    time.sleep(0.4)
                    continue
                boot=d.get("boot_id")
                if previous_boot is not None and boot!=previous_boot:
                    raise RuntimeError("Device restarted; preserve this file and start a separate capture")
                if boot is not None: previous_boot=boot
                if last_id is not None and d["sample_id"]<last_id:
                    raise RuntimeError("Device sample sequence regressed; start a separate capture")
                if d["sample_id"]>0 and d["sample_id"]!=last_id and d["sample_age_ms"]<=2500:
                    now=d["uptime_ms"]
                    energy=d["device_energy_wh"]
                    if previous_energy is not None and energy<previous_energy:
                        raise RuntimeError("Device energy counter reset; start a separate capture")
                    if previous_ms is not None:
                        delta=(now-previous_ms)&0xffffffff
                        if delta==0 or delta>3600000: raise RuntimeError("Device restarted or capture clock is inconsistent; start a separate file")
                        elapsed_ms+=delta
                    if provenance["first_sample_received_utc"] is None:
                        provenance["first_sample_received_utc"]=datetime.now(timezone.utc).isoformat()
                        provenance["boot_id"]=boot
                        provenance["first_sample_uptime_ms"]=now
                    previous_ms=now;last_id=d["sample_id"];previous_energy=energy
                    writer.writerow([f"{elapsed_ms/1000:.3f}",d["voltage_v"] if d["valid"] else '',d["current_ma"] if d["valid"] else '',d["power_mw"] if d["valid"] else '',d["device_energy_wh"],int(d["valid"])])
                    f.flush()
                    provenance["samples"]+=1
                time.sleep(0.4)
            if not provenance["samples"]: raise RuntimeError("No fresh device samples were captured")
        except KeyboardInterrupt: provenance["status"]="interrupted"
        finally:
            if args.metadata_output:
                f.flush()
                if sys.exc_info()[0] is not None: provenance["status"]="failed"
                provenance["capture_finished_utc"]=datetime.now(timezone.utc).isoformat()
                provenance["elapsed_s"]=elapsed_ms/1000
                provenance["csv_file"]=args.output.name
                provenance["csv_sha256"]=hashlib.sha256(args.output.read_bytes()).hexdigest()
                with args.metadata_output.open("x",encoding="utf-8") as record:
                    json.dump(provenance,record,indent=2,allow_nan=False);record.write("\n")
    print("Saved",args.output)
if __name__=="__main__":main()
