import argparse
import csv
import json
import math
import hashlib
from datetime import datetime, timedelta, timezone
from pathlib import Path
REQUIRED = {"Time_s", "Voltage_V", "Current_mA", "Power_mW", "Energy_Wh", "Valid"}
def load_session(path, max_gap_s=2.5):
    if not math.isfinite(max_gap_s) or max_gap_s<=0: raise ValueError("Maximum gap must be finite and positive")
    with Path(path).open(newline="", encoding="utf-8-sig") as f:
        reader=csv.DictReader(f)
        headers=reader.fieldnames or []
        if not REQUIRED.issubset(headers) or len(headers)!=len(set(headers)): raise ValueError("Missing or duplicate required columns")
        rows=[]
        for source in reader:
            if None in source or any(value is None for value in source.values()): raise ValueError("Malformed CSV row")
            t=float(source["Time_s"])
            if not math.isfinite(t) or t<0 or (rows and t<=rows[-1]["time"]): raise ValueError("Timestamps must be finite, nonnegative, and strictly increasing")
            if source["Valid"] not in ("0","1"): raise ValueError("Valid must be 0 or 1")
            valid=source["Valid"]=="1"
            p=float(source["Power_mW"]) if valid else math.nan
            v=float(source["Voltage_V"]) if valid else math.nan
            current=float(source["Current_mA"]) if valid else math.nan
            energy=float(source["Energy_Wh"])
            if not math.isfinite(energy) or energy<0: raise ValueError("Invalid energy accumulator")
            if valid and (not all(math.isfinite(x) and x>=0 for x in [p,v,current]) or abs(p-v*current)>max(0.1,0.001*p)): raise ValueError("Invalid or inconsistent electrical measurement")
            if rows and energy<rows[-1]["energy"]: raise ValueError("Energy accumulator decreased; split restarted/reset sessions")
            rows.append(dict(time=t,power=p,voltage=v,current=current,valid=valid,energy=energy))
    if len(rows)<2: raise ValueError("At least two samples are required")
    total=covered=0.0; excluded=0
    for a,b in zip(rows,rows[1:]):
        dt=b["time"]-a["time"]
        if a["valid"] and b["valid"] and dt<=max_gap_s:
            total+=(a["power"]+b["power"])*0.5*dt/3600000
            covered+=dt
        else: excluded+=1
    valid_power=[x["power"] for x in rows if x["valid"]]
    if not covered or not valid_power: raise ValueError("No integrable sample intervals")
    if not math.isfinite(total) or not math.isfinite(covered) or not math.isfinite(total*3600000/covered):
        raise ValueError("Integrated values exceed the supported numeric range")
    return dict(samples=len(rows),valid_samples=len(valid_power),elapsed_s=rows[-1]["time"]-rows[0]["time"],covered_s=covered,excluded_intervals=excluded,energy_wh=total,peak_power_mw=max(valid_power),time_weighted_mean_power_mw=total*3600000/covered,logged_energy_change_wh=rows[-1]["energy"]-rows[0]["energy"],rows=rows)
def compare(tracker,fixed):
    if fixed["energy_wh"]<=0: raise ValueError("Fixed panel energy must be positive")
    if not math.isclose(tracker["elapsed_s"],fixed["elapsed_s"],rel_tol=0,abs_tol=0.01): raise ValueError("Use equal observation durations before comparing energy")
    if tracker["excluded_intervals"] or fixed["excluded_intervals"]: raise ValueError("Select complete valid windows before computing comparative gain")
    gain=100*(tracker["energy_wh"]-fixed["energy_wh"])/fixed["energy_wh"]
    if not math.isfinite(gain): raise ValueError("Energy gain exceeds the supported numeric range")
    return gain
def validate_protocol(meta, tracker, fixed):
    if not isinstance(meta,dict): raise ValueError("Metadata must be a JSON object")
    if meta.get("protocol") not in ["sequential","simultaneous"]:
        raise ValueError("Metadata must declare the acquisition protocol")
    simultaneous=meta["protocol"]=="simultaneous"
    declared=[meta.get("tracker_start_utc"),meta.get("fixed_start_utc")]
    if simultaneous or all(isinstance(value,str) and value.strip() for value in declared):
        starts=[]
        for field,session in [("tracker_start_utc",tracker),("fixed_start_utc",fixed)]:
            value=meta.get(field)
            if not isinstance(value,str): raise ValueError("Observation windows require valid start timestamps")
            try: stamp=datetime.fromisoformat(value.replace("Z","+00:00"))
            except ValueError as e: raise ValueError("Invalid observation start timestamp") from e
            if stamp.utcoffset() is None: raise ValueError("Observation timestamps must include a timezone")
            starts.append(stamp.astimezone(timezone.utc)+timedelta(seconds=session["rows"][0]["time"]))
        if simultaneous and abs((starts[0]-starts[1]).total_seconds())>0.01:
            raise ValueError("Trim both CSVs to the same absolute observation window")
        if not simultaneous:
            ends=[start+timedelta(seconds=session["elapsed_s"]) for start,session in zip(starts,[tracker,fixed])]
            if (min(ends)-max(starts)).total_seconds()>0.01:
                raise ValueError("Sequential observation windows must not overlap")

def audit_metadata(meta, sessions, directory=None):
    """Check recorded facts without inventing missing acquisition conditions."""
    if not isinstance(meta,dict): raise ValueError("Metadata must be a JSON object")
    paired=set(sessions)=={"tracker","fixed"}
    if paired: validate_protocol(meta,sessions["tracker"],sessions["fixed"])
    missing=[];warnings=[];diagnostics={}
    fields=["firmware_revision","calibration_record","processing_history","acquisition_method","duration_s","load_ohms"]
    if paired: fields += ["weather_and_shading","fixed_azimuth_deg","fixed_tilt_deg"]
    else: fields += ["test_source","reference_instrument"]
    for key in fields:
        if meta.get(key) is None or (isinstance(meta.get(key),str) and not meta[key].strip()): missing.append(key)
    for key in ["data_kind","firmware_revision","calibration_record","processing_history","acquisition_method","weather_and_shading"]:
        if meta.get(key) is not None and not isinstance(meta[key],str): raise ValueError("Invalid metadata field: "+key)
    for role,session in sessions.items():
        key=role+"_start_utc";stamp=meta.get(key)
        if stamp is None or (isinstance(stamp,str) and not stamp.strip()): missing.append(key)
        else:
            if not isinstance(stamp,str): raise ValueError("Invalid start timestamp: "+key)
            parsed=datetime.fromisoformat(stamp.replace("Z","+00:00"))
            if parsed.utcoffset() is None: raise ValueError("Start timestamp requires a timezone: "+key)
        duration=meta.get("duration_s")
        if duration is not None and (type(duration) not in (int,float) or not math.isfinite(duration) or
                                     not math.isclose(duration,session["elapsed_s"],rel_tol=0,abs_tol=0.01)):
            raise ValueError("Metadata duration does not match "+role)
        if paired:
            identifiers=meta.get("panel_identifiers")
            if not isinstance(identifiers,dict): raise ValueError("panel_identifiers must be an object")
            if not identifiers.get(role) or (isinstance(identifiers[role],str) and not identifiers[role].strip()): missing.append("panel_identifiers."+role)
            elif not isinstance(identifiers[role],str): raise ValueError("Panel identifier must be a string")
        valid=[r for r in session["rows"] if r["valid"]]
        loads=[r["voltage"]*1000/r["current"] for r in valid if r["current"]>0]
        if not all(math.isfinite(value) for value in loads): raise ValueError("Inferred load exceeds numeric range")
        off_grid=sum(abs(r["voltage"]-round(r["voltage"]/0.00125)*0.00125)>0.000006 for r in valid)
        diagnostics[role]={"valid_samples":len(valid),"voltage_samples_off_current_firmware_grid":off_grid,
                           "inferred_load_ohms_min":min(loads) if loads else None,
                           "inferred_load_ohms_max":max(loads) if loads else None}
        if off_grid:
            warnings.append(f"{role}: {off_grid}/{len(valid)} voltages do not match the current firmware's 0.00125 V bus step, allowing CSV rounding; acquisition firmware/processing remains unverified")
        if session["samples"]>3600 and meta.get("acquisition_method")!="host_capture":
            warnings.append(f"{role}: exceeds current 3600-record RAM export capacity; document the host capture or other acquisition path")
        if not session["excluded_intervals"] and abs(session["logged_energy_change_wh"]-session["energy_wh"])>max(1e-6,session["energy_wh"]*0.01):
            warnings.append(f"{role}: logged energy change differs from power integration by more than rounding/1%; check capture, counter resets, and processing history")
        load=meta.get("load_ohms")
        if load is not None:
            if type(load) not in (int,float) or not math.isfinite(load) or load<=0: raise ValueError("Invalid load_ohms")
            if any(abs(value-load)>max(0.01,load*0.02) for value in loads):
                raise ValueError("Recorded load does not agree with V/I for "+role)
        inferred=meta.get("inferred_load_ohms")
        if inferred is not None and (type(inferred) not in (int,float) or not math.isfinite(inferred) or inferred<=0 or
                                     any(abs(value-inferred)>max(0.01,inferred*0.02) for value in loads)):
            raise ValueError("Inferred load metadata does not agree with V/I for "+role)
        files=meta.get("files",{})
        if not isinstance(files,dict): raise ValueError("files must be an object")
        if directory is not None:
            entry=files.get(role)
            if not isinstance(entry,dict) or not isinstance(entry.get("path"),str) or not isinstance(entry.get("sha256"),str):
                raise ValueError("Missing file identity for "+role)
            base=Path(directory).resolve();path=(base/entry["path"]).resolve()
            if not path.is_relative_to(base): raise ValueError("Measurement path escapes session directory")
            if hashlib.sha256(path.read_bytes()).hexdigest()!=entry["sha256"]: raise ValueError("Measurement checksum mismatch: "+role)
    for key in ["panel_rated_w","shunt_ohms"]:
        value=meta.get(key)
        if value is not None and (type(value) not in (int,float) or not math.isfinite(value) or value<=0):
            raise ValueError("Invalid nominal parameter: "+key)
    for key,low,high in [("fixed_azimuth_deg",0,360),("fixed_tilt_deg",0,90)]:
        value=meta.get(key)
        if value is not None and (type(value) not in (int,float) or not math.isfinite(value) or not low<=value<=high):
            raise ValueError("Invalid orientation: "+key)
    if missing: warnings.insert(0,"Acquisition details not recorded: "+", ".join(missing))
    return {"acquisition_details_complete":not missing,"missing_fields":missing,"warnings":warnings,"diagnostics":diagnostics}

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument("--tracker",type=Path,required=True);parser.add_argument("--fixed",type=Path,required=True)
    parser.add_argument("--metadata",type=Path,required=True);parser.add_argument("--output",type=Path,required=True)
    parser.add_argument("--require-complete-metadata",action="store_true",help="Reject unresolved acquisition details or compatibility warnings")
    args=parser.parse_args();meta=json.loads(args.metadata.read_text(encoding="utf-8"))
    t=load_session(args.tracker);f=load_session(args.fixed);gain=compare(t,f)
    quality=audit_metadata(meta,{"tracker":t,"fixed":f})
    files=meta.get("files",{})
    for role,path in [("tracker",args.tracker),("fixed",args.fixed)]:
        if role in files and hashlib.sha256(path.read_bytes()).hexdigest()!=files[role].get("sha256"):
            raise ValueError("Input CSV does not match metadata checksum: "+role)
    for warning in quality["warnings"]: print("Data warning:",warning)
    if args.require_complete_metadata and quality["warnings"]: raise ValueError("Acquisition metadata requires review")
    args.output.mkdir(parents=True,exist_ok=True)
    data_kind=meta.get("data_kind") or "unspecified"
    result={"protocol":meta["protocol"],"data_kind":data_kind,"acquisition_metadata":meta,"data_quality":quality,"tracker":{k:v for k,v in t.items() if k!="rows"},"fixed":{k:v for k,v in f.items() if k!="rows"},"gross_energy_gain_pct":gain}
    (args.output/"summary.json").write_text(json.dumps(result,indent=2)+"\n")
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    fig,axes=plt.subplots(1,2,figsize=(11,4))
    for s,label,c in [(t,"Tracked","#c77b25"),(f,"Fixed","#2d718e")]:
        axes[0].plot([x["time"]-s["rows"][0]["time"] for x in s["rows"]],[x["power"] for x in s["rows"]],label=label,color=c)
    axes[0].set(xlabel="Elapsed session time (s)",ylabel="Power (mW)",title=meta["protocol"].capitalize()+" observation windows")
    axes[0].legend();axes[1].bar(["Tracked","Fixed"],[t["energy_wh"],f["energy_wh"]],color=["#c77b25","#2d718e"])
    axes[1].set(ylabel="Integrated energy (Wh)",title=f"Gross energy difference: {gain:.2f}%")
    fig.suptitle("Energy performance / "+data_kind);fig.tight_layout()
    fig.savefig(args.output/"energy_performance.png",dpi=180);fig.savefig(args.output/"energy_performance.svg");plt.close(fig)
    print(json.dumps({k:v for k,v in result.items() if k not in ["tracker","fixed"]},indent=2))
if __name__=="__main__": main()
