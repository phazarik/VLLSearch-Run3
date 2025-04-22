#!/usr/bin/env python3
import subprocess
import re
import argparse
import os
import time
from tabulate import tabulate
from rich.console import Console
from rich.progress import Progress, BarColumn, TimeRemainingColumn, TaskProgressColumn

console = Console(highlight=False)
print = console.print

# Patterns to extract the limits
patterns_to_look_for = {
    "Observed Limit": re.compile(r"Observed Limit: r < (\d+\.\d+)"),
    "Expected  2.5%": re.compile(r"Expected  2\.5%: r < (\d+\.\d+)"),
    "Expected 16.0%": re.compile(r"Expected 16\.0%: r < (\d+\.\d+)"),
    "Expected 50.0%": re.compile(r"Expected 50\.0%: r < (\d+\.\d+)"),
    "Expected 84.0%": re.compile(r"Expected 84\.0%: r < (\d+\.\d+)"),
    "Expected 97.5%": re.compile(r"Expected 97\.5%: r < (\d+\.\d+)"),
}

# Function to run the combine command and extract the limits
def extract_limits(datacard_path, method, test=False):
    command = ['combine', '-M', method, datacard_path]
    if test: print(f"{' '.join(command)}", style="italic dim")
    
    result = subprocess.run(
        command,
        capture_output=True,
        text=True
    )

    limits = {}
    for key, pattern in patterns_to_look_for.items():
        match = pattern.search(result.stdout)
        if match: limits[key] = float(match.group(1))
        else:     limits[key] = None
    return limits, result.stdout

# Main function to process all files and store the results in a dictionary
def process_datacards(jobname, method, test=False):

    inputpath = jobname
    if not inputpath.startswith('datacards'): inputpath = os.path.join('datacards', jobname)
    limits_by_model = {}

    if test:
        for filename in os.listdir(inputpath):
            if filename.endswith(".txt"):
                datacard_path = os.path.join(inputpath, filename)
                limits, output = extract_limits(datacard_path, method, test)
                print(f"Full output for {filename}:\n\n")
                print(output, style="white")
                model = 'VLLD_ele' if 'ele' in filename else 'VLLD_mu'
                mass = int(re.search(r'(\d+)', filename).group(1))
                return {model: {mass: limits}}

    with Progress("[progress.description]{task.description}", BarColumn(), TaskProgressColumn(), transient=True) as progress:
        datacards = [f for f in os.listdir(inputpath) if f.endswith(".txt")]
        total = len(datacards)
        task_id = progress.add_task("Processing datacards", total=total)

        for filename in datacards:
            match = re.search(r'(\d+)', filename)
            if not match: continue
            mass = int(match.group(1))
            model = 'VLLD_ele' if 'ele' in filename else 'VLLD_mu'
            if model not in limits_by_model:
                limits_by_model[model] = {}

            datacard_path = os.path.join(inputpath, filename)
            limits, _ = extract_limits(datacard_path, method)
            limits_by_model[model][mass] = limits
            progress.update(task_id, advance=1)

    return limits_by_model

def write_limits(limits_by_model, jobname, test=False):
    for model, limits_dict in limits_by_model.items():
        table_data = []
        for mass in sorted(limits_dict.keys()):
            row = {'Mass': mass}
            row.update(limits_dict[mass])
            table_data.append(row)

        print(f"\nResults for model: {model}")
        print(tabulate(table_data, headers='keys', tablefmt='psql'))

        if not test:
            os.makedirs('limits', exist_ok=True)
            campaign = os.path.basename(jobname).replace("yields_", "")
            output_file = f"limits/limits_{model}_{campaign}.txt"
            with open(output_file, 'w') as file:
                file.write(f"{'Mass':<10}")
                for key in patterns_to_look_for.keys():
                    file.write(f"{key:<15}")
                file.write("\n")
                for mass, limits in sorted(limits_dict.items()):
                    file.write(f"{mass:<10}")
                    for key in patterns_to_look_for.keys():
                        value = limits.get(key)
                        file.write(f"{value:<15}" if value is not None else f"{'N/A':<15}")
                    file.write("\n")
            print(f"Output written to {output_file}")

# Main execution
if __name__ == "__main__":
    # Parse arguments
    parser = argparse.ArgumentParser(description="Run combine command and extract limits.")
    parser.add_argument("--jobname", type=str, required=True, help="Jobname containing datacards.")
    parser.add_argument("--method",  type=str, required=False, default='AsymptoticLimits', help="Method for combine.")
    parser.add_argument("--test", action='store_true', required=False, help="Run the subprocess once for testing and display the full output.")
    args = parser.parse_args()

    start_time = time.time()
    limits_dict = process_datacards(args.jobname, args.method, test=args.test)
    write_limits(limits_dict, args.jobname, test=args.test)
    end_time = time.time()

    print(f"Processing completed in {end_time - start_time:.2f} seconds.\n")
