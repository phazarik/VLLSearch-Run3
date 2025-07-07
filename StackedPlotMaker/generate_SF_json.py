#!/usr/bin/env python3
import time
from datetime import timedelta
import subprocess, json, os, re, argparse
parser = argparse.ArgumentParser()
parser.add_argument('-d', '--debug', action='store_true', help='show ROOT/stdout in debug mode')
args = parser.parse_args()

# Custom JSON encoder:
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
                    if e.get('scale') == [0, 0]: continue #filtering zero bins
                    items = []
                    for k, v in e.items():
                        if k == 'scale' and isinstance(v, list): items.append(f'{json.dumps(k)}: [' + ', '.join(fmt_scale(x) for x in v) + ']')
                        else: items.append(f'{json.dumps(k)}: {json.dumps(v)}')
                    body.append('\n' + pad + '{' + ', '.join(items) + '}')
                return '[' + ','.join(body) + f'\n{" " * lvl}' + ']'
            body = ','.join(f'\n{pad}{_dump(e, lvl + indent)}' for e in o)
            return '[' + body + f'\n{" " * lvl}' + ']'
        return json.dumps(o)
    fp.write(_dump(obj, 0))

# ----------------- parameters ----------------
campaigns = ["2016preVFP_UL","2016postVFP_UL","2017_UL","2018_UL",
             "Run3Summer22","Run3Summer22EE","Run3Summer23","Run3Summer23BPix"]
channels  = ["mm","me","em","ee"]
channel_idx = {c:str(i) for i,c in enumerate(channels)}

jobdir = "2025-07-07_topcr_unscaled"
tag    = "topcr"
text   = ""

# ----------------- DON'T TOUCH BELOW  -----------------
macro  = "makeStackedPlot.C"
outdir = "auto_generated_jsons"
os.makedirs(outdir, exist_ok=True)
scale_json, obs_exp_json, obs_exp_table, ssqrtb_table = {}, {}, [], []

count = 0
start_time = time.time()

for camp in campaigns:
    scale_json[camp] = {}
    obs_exp_json[camp] = {}
    for ch in channels:
        count+=1
        
        jobname = os.path.join(jobdir, f"hist_{camp}_{tag}_{ch}")
        cmd = ["root","-l","-b","-q", f'{macro}("HT","HT (GeV)","{jobname}","{camp}","{ch}","{tag}","{text}")']
        print(f'{count}/{len(channels)*len(campaigns)} Running ==> {" ".join(cmd)}  ... ', end="")
        if args.debug: print()

        run = subprocess.run(cmd, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        out = run.stdout
        if args.debug:
            print("\n\033[93m=================== DEBUG MODE ===================\033[0m")
            print(out)
            print("\n\033[93m==================================================\033[0m")

        # ----------- JSON block -------------
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
                if args.debug: print(f"\033[93m[Debug] ==> JSON start detected:\033[0;3m{line}\033[0m")
            elif inside:
                json_lines.append(line)
                brace_count += line.count('{') - line.count('}')
                if args.debug: print(f"\033[33;3m[Debug] Line: \033[0;3m{line}\033[0m, Brace count: {brace_count}")
                if brace_count <= 0:
                    json_lines[-1] = json_lines[-1].rstrip(',')  # Remove trailing comma
                    break

        if not json_lines:
            print(f'\n\033[91m[Warning] No JSON in {camp}_{ch}\033[0m')
            continue

        # Parse JSON safely
        json_str = "\n".join(json_lines).replace('"high": "inf"', '"high": 1e6')
        if args.debug: print(f"\033[93m==> [Debug] JSON string to parse:\n{json_str}")
        try:
            js = json.loads(json_str)
        except json.JSONDecodeError as e:
            print(f'\033[91m[Error] Failed to parse JSON for {camp}_{ch}: {e}\033[0m')
            continue

        for b in js["bins"]:  # Restore "inf"
            if b["high"] == 1e6:
                b["high"] = "inf"

        scale_json[camp][channel_idx[ch]] = js["bins"]
        if args.debug: print(f"\033[93m[Debug] Successfully parsed JSON for {camp}_{ch}:\033[0m\n{js}")

        # -------- Obs/Exp and S/sqrt(B) -----
        if m := re.search(r'Obs/Exp\s*=\s*([\d.]+)\s*±\s*([\d.]+)', out):
            val, err = map(float, m.groups())
            obs_exp_json[camp][channel_idx[ch]] = [round(val, 3), round(err, 3)]
            obs_exp_table.append((camp, ch, val, err))
            
        if m := re.search(r'S/sqrt\(B\)\s*=\s*([\d.]+)\s*±\s*([\d.]+)', out):
            val, err = map(float, m.groups())
            ssqrtb_table.append((camp, ch, val, err))

        print("Done.")

        if args.debug: break
    if args.debug: break

# ----------------- dump -----------------
with open(f'{outdir}/scale_factors_{tag}.json', 'w') as f:  dump_compact(scale_json, f)
with open(f'{outdir}/obs_exp_{tag}.json', 'w') as f:  dump_compact(obs_exp_json, f)

def show(title, rows):
    print(f'\n\033[93m{"Campaign":20} {"Ch":2} {title:>10}\033[0m')
    for c, ch, v, e in rows: print(f'{c:20} {ch:2} {v:>6.3f} ± {e:<6.3f}')
    print()
    

show("Obs/Exp", obs_exp_table)
show("S/sqrt(B)", ssqrtb_table)

end_time = time.time()
time_taken = end_time - start_time
print(f"Time taken = {str(timedelta(seconds=int(time_taken)))}\n")
