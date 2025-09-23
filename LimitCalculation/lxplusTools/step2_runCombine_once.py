import os, argparse, subprocess
from rich.progress import Progress, BarColumn, TimeElapsedColumn, TextColumn, TimeRemainingColumn
from tabulate import tabulate
import json

sigdict = {
    "VLLD": {
        "ele": {
            "100": {"scale":1},
            "200": {"scale":1},
            "300": {"scale":10},
            "400": {"scale":10},
            "600": {"scale":10},
            "800": {"scale":10},
            "1000":{"scale":10}
        },
        "mu": {
            "100": {"scale":1},
            "200": {"scale":1},
            "300": {"scale":10},
            "400": {"scale":10},
            "600": {"scale":10},
            "800": {"scale":10},
            "1000":{"scale":10}
        },
        "tau": {}
    },
    "VLLS": {}
}
variable = "LTplusMET"

# ------------ main ------------
def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--campaign", default="2018_UL")
    parser.add_argument("--channel",  default="mm")
    parser.add_argument("--method",   default="AsymptoticLimits")
    parser.add_argument("--test",     action="store_true")
    parser.add_argument("--dryrun",   action="store_true")
    args = parser.parse_args()

    datacards = collect_datacards(args.campaign, args.channel)

    if args.test:
        for model, flav, mass, scale, datacard in datacards:
            output = run_combine(datacard, scale, args.method, args.dryrun)
            if not output: continue
            limits = parse_limits(output)
            if limits: sigdict[model][flav][mass]["limit"] = limits
            print(f"\n=== Result for {datacard} ===\n")
            print(output.strip())
            print("\nParsed limits:", limits)
            break

    else:
        with Progress(
                TextColumn("[green]Processing {task.fields[campaign]} {task.fields[channel]}[/green]"),
                BarColumn(bar_width=40, complete_style="yellow"),
                TextColumn("[yellow]{task.percentage:>3.0f}%[/yellow]"),
                TimeElapsedColumn(),
                TimeRemainingColumn(),
        ) as progress:
            task = progress.add_task(
                "combine",
                total=len(datacards),
                campaign=args.campaign,
                channel=args.channel,
            )
            for model, flav, mass, scale, datacard in datacards:
                output = run_combine(datacard, scale, args.method, args.dryrun)
                if output:
                    limits = parse_limits(output)
                    if limits: sigdict[model][flav][mass]["limit"] = limits
                progress.advance(task)
        

    print("===== RESULT =====")
    print(f"campaign={args.campaign}")
    print(f"channel={args.channel}")
    print_limits_tables(sigdict)

    ##Save as JSON
    if not args.dryrun: os.makedirs("limits", exist_ok=True)
    jsonfile = f"limits_{args.campaign}_{args.channel}.json"
    jsonpath = os.path.join("limits", jsonfile)
    formatted_sigdict = format_sigdict_for_json(sigdict)
    with open(jsonpath, "w") as f: json.dump(formatted_sigdict, f, indent=2)
    print(f"\nFile created: \033[93m{jsonpath}\033[0m\n")
            
# ------------ colllect all datacards ------------
def collect_datacards(campaign, channel):
    indir = f"shapes/{campaign}_{channel}"
    datacards = []
    for model, flavors in sigdict.items():
        for flav, masses in flavors.items():
            for mass, info in masses.items():
                scale = info["scale"]
                datacard = f"{indir}/datacard_{campaign}_{channel}_{model}{flav}{mass}_LTplusMET.txt"
                if os.path.exists(datacard):
                    datacards.append((model, flav, mass, scale, datacard))
    return datacards

# ------------ running on one datacard ------------
def run_combine(datacard, scale, method, dryrun=False):
    cmd = [
        "combine","-M", method,
        datacard,
        "--setParameters", f"r={scale}", "--freezeParameters", "r"
    ]
    if dryrun:
        print("\033[31m[DRYRUN]\033[0m", " ".join(cmd))
        return ""

    result = subprocess.run(cmd, capture_output=True, text=True)
    return result.stdout

# ------------ capture output ------------
def parse_limits(output):
    limits = {}
    for line in output.splitlines():
        line = line.strip()
        if line.startswith("Observed Limit:"):
            limits["Observed Limit"] = float(line.split("<")[-1].strip())
        elif line.startswith("Expected"):
            key, val = line.split(":")
            limits[key.strip()] = float(val.split("<")[-1].strip())
    return limits

# ------------ print table ------------
def fmt(val): return f"{val:.3f}" if isinstance(val, (int, float)) else "-"

def print_limits_tables(sigdict):
    table_id = 1
    for model, flavors in sigdict.items():
        for flav, masses in flavors.items():
            if not masses: continue
            rows = []
            for mass, info in sorted(masses.items(), key=lambda x: int(x[0])):
                limits = info.get("limit")
                if not limits: continue
                rows.append([
                    f"{int(mass)}",
                    fmt(limits.get('Observed Limit')),
                    fmt(limits.get('Expected  2.5%')),
                    fmt(limits.get('Expected 16.0%')),
                    fmt(limits.get('Expected 50.0%')),
                    fmt(limits.get('Expected 84.0%')),
                    fmt(limits.get('Expected 97.5%')),
                ])
            if not rows: continue
            headers = ["mass", "Observed", "Expected2.5%", "Expected16%", "Expected50%", "Expected84%", "Expected97.5%"]
            print(f"\nTable {table_id}: Limits for {model}_{flav}\n"+"-"*30)
            print(tabulate(rows, headers=headers, tablefmt="github", stralign="right", numalign="right"))
            table_id += 1

# ------------ JSON formatting ------------
def format_sigdict_for_json(sigdict):
    formatted = {}
    for model, flavors in sigdict.items():
        formatted[model] = {}
        for flav, masses in flavors.items():
            formatted[model][flav] = {}
            for mass, info in masses.items():
                new_info = info.copy()
                limits = new_info.get("limit")
                if limits:
                    # convert to single-line string
                    new_info["limit"] = ", ".join(f"{k}:{v}" for k,v in limits.items())
                formatted[model][flav][mass] = new_info
    return formatted

# ------------ EXECUTION ------------
if __name__ == "__main__": results = main()
