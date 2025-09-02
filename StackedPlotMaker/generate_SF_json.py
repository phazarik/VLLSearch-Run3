#!/usr/bin/env python3
import time
from datetime import timedelta
import subprocess, json, os, re

# ================================ GLOBAL PARAMETERS =========================================
jobdir = "2025-08-14_baseline" ## hist folder
tag = "baseline" ## event tag
var = "HT" ## bins
lookatdata = False

debug = False
test_camp = "Run3Summer22"
test_chan = "mm"
# ============================================================================================

## internal parameters
campaigns = ["2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL",
             "Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix"]
campaigns.extend(["Run2", "Run3", "FullDataset"])
channels = ["mm", "me", "em", "ee", "combined"] ## order is important for index
channel_idx = {c: str(i) for i, c in enumerate(channels)}

## Custom JSON encoder
def dump_compact(obj, fp, indent=2):
    def fmt_scale(x): return f'{x:.6f}' if isinstance(x, float) else json.dumps(x)
    def _dump(o, lvl):
        pad = ' ' * (lvl + indent)
        if isinstance(o, dict):
            if not o: return '{}'
            body = ','.join(f'\n{pad}{json.dumps(k)}: {_dump(v, lvl + indent)}' for k, v in o.items())
            return '{' + body + f'\n{" " * lvl}' + '}'
        if isinstance(o, list):
            if not o: return '[]'
            if all(not isinstance(e, (dict, list)) for e in o):
                return '[' + ', '.join(fmt_scale(e) for e in o) + ']'
            if all(isinstance(e, dict) for e in o):
                body = []
                for e in o:
                    if e.get('scale') == [0, 0]: continue  # Filter zero bins
                    items = []
                    for k, v in e.items():
                        if k == 'scale' and isinstance(v, list):
                            items.append(f'{json.dumps(k)}: [' + ', '.join(fmt_scale(x) for x in v) + ']')
                        else:
                            items.append(f'{json.dumps(k)}: {json.dumps(v)}')
                    body.append('\n' + pad + '{' + ', '.join(items) + '}')
                return '[' + ','.join(body) + f'\n{" " * lvl}' + ']'
            body = ','.join(f'\n{pad}{_dump(e, lvl + indent)}' for e in o)
            return '[' + body + f'\n{" " * lvl}' + ']'
        return json.dumps(o)
    fp.write(_dump(obj, 0))

## Initialize JSONs and tables
macro = "makeStackedPlot.C"
outdir = "auto_generated_jsons"
os.makedirs(outdir, exist_ok=True)

scale_json, obs_exp_json, ssqrtb_json, global_sf_json = {}, {}, {}, {}
obs_exp_table, ssqrtb_table, global_sf_table = [], [], []

## Exception handling:
def is_valid(camp, ch):
    if camp in ["Run2", "Run3"] and ch != "combined": return False
    return True

valid_pairs = [(camp, ch) for camp in campaigns for ch in channels if is_valid(camp, ch)]
total_runs  = len(valid_pairs)
count = 0
start_time = time.time()

## Main loop
if debug: print("\033[31m[debug mode]\033[0m")
for camp in campaigns:
    scale_json[camp] = {}
    obs_exp_json[camp] = {}
    ssqrtb_json[camp] = {}
    global_sf_json[camp] = {}

    for ch in channels:
        
        if debug:
            if camp != test_camp: continue
            if ch   != test_chan: continue

        ## Exception:
        if not is_valid(camp, ch): continue
        
        count += 1
        jobname = os.path.join(jobdir, f"hist_{tag}_{camp}_{ch}")
        cmd = ["root", "-l", "-b", "-q",
               f'{macro}("{var}","name","{jobname}","{camp}","{ch}","{tag}","text", {str(lookatdata).lower()}, false)']
        print(f'({count}/{total_runs}) Running ==> {" ".join(cmd)}...', flush=True)

        ## Run ROOT command
        run = subprocess.run(cmd, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        out = run.stdout

        if debug:
            print("\n\033[33m=================== DEBUG MODE ===================\033[0m")
            print(out)
            print("\033[33m==================================================\033[0m")

        ## Parse JSON block (Scale Factors)
        json_lines = []
        inside = False
        brace_count = 0
        for line in out.splitlines():
            line = line.strip()
            if not line: continue
            if not inside and line.lstrip().startswith('{'):
                inside = True
                brace_count = line.count('{')
                json_lines.append(line)
                if debug: print(f"\033[93m[Debug] JSON start detected: {line}\033[0m")
            elif inside:
                json_lines.append(line)
                brace_count += line.count('{') - line.count('}')
                if debug: print(f"\033[33m[Debug] Line: {line}, Brace count: {brace_count}\033[0m")
                if brace_count <= 0:
                    json_lines[-1] = json_lines[-1].rstrip(',')  # Remove trailing comma
                    break

        if json_lines:
            json_str = "\n".join(json_lines).replace('"high": "inf"', '"high": 1e6')
            if debug: print(f"\033[93m[Debug] JSON string to parse:\n{json_str}\033[0m")
            try:
                js = json.loads(json_str)
                for b in js.get("bins", []):  # Restore "inf"
                    if b.get("high") == 1e6:
                        b["high"] = "inf"
                scale_json[camp][channel_idx[ch]] = js.get("bins", [])
                if debug: print(f"\033[33m[Debug] Parsed JSON for {camp}_{ch}:\033[0m\n{js}")
            except json.JSONDecodeError as e: print(f'\033[91m[Error] Failed to parse JSON for {camp}_{ch}: {e}\033[0m')
        else: print(f'\033[91m[Warning] No JSON in {camp}_{ch}\033[0m')

        ## Parse Obs/Exp
        if m := re.search(r'Obs/Exp\s*=\s*([\d.]+)\s*±\s*([\d.]+)', out):
            val, err = map(float, m.groups())
            obs_exp_json[camp][channel_idx[ch]] = [round(val, 3), round(err, 3)]
            obs_exp_table.append((camp, ch, val, err))

        ## Parse S/sqrt(B) with flexible decimal places
        if m := re.search(r'S/sqrt\(B\)\s*=\s*([\d.]+)\s*±\s*([\d.]+)', out):
            val, err = map(float, m.groups())
            ssqrtb_json[camp][channel_idx[ch]] = [round(val, 3), round(err, 3)]
            ssqrtb_table.append((camp, ch, val, err))

        ## Parse Global SF
        if m := re.search(r'Global SF\s*=\s*([\d.]+)\s*±\s*([\d.]+)', out):
            val, err = map(float, m.groups())
            global_sf_json[camp][channel_idx[ch]] = [round(val, 6), round(err, 6)]
            global_sf_table.append((camp, ch, val, err))

## Display tables
def show(title, rows):
    print(f'\n\033[93m{"Campaign":20} {"Ch":2} {title:>10}\033[0m')
    for c, ch, v, e in rows: print(f'{c:20} {ch:2} {v:>6.3f} ± {e:<6.3f}')
    print()

show("Obs/Exp", obs_exp_table)
show("S/sqrt(B)", ssqrtb_table)
show("Global SF", global_sf_table)

## Save JSON files
def savejson(json, filename):
    if json:
        with open(filename, 'w') as f:
            dump_compact(json, f)
            print(f"File created: \033[93m{filename}\033[0m")

if not debug:
    savejson(scale_json,     f'{outdir}/{tag}_SF_inBins.json')
    savejson(obs_exp_json,   f'{outdir}/{tag}_obsbyexp.json')
    savejson(global_sf_json, f'{outdir}/{tag}_SF_global.json')
    savejson(ssqrtb_json,    f'{outdir}/{tag}_sbysqrtb.json' )

## Print execution time
time_taken = time.time() - start_time
print(f"Time taken = {str(timedelta(seconds=int(time_taken)))}\n")
