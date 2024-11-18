import subprocess
import re
import argparse
import os
import time

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
    if test: print(f"Running subprocess: \033[93m{' '.join(command)}\033[0m")
    
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
    inputpath = os.path.join('datacards', jobname)
    limits_dict = {}
    # If test is True, only process the first datacard and print the full output
    if test:
        for filename in os.listdir(inputpath):
            if filename.endswith(".txt"):
                datacard_path = os.path.join(inputpath, filename)
                limits, output = extract_limits(datacard_path, method, test)
                print(f"Full output for {filename}:")
                print(output)
                return
    
    # Loop through all files in the input directory
    for filename in os.listdir(inputpath):
        if filename.endswith(".txt"):
            # Extract the mass from the filename (e.g., M100, M200)
            mass = re.search(r'(\d+)', filename)
            if mass:
                mass = int(mass.group(1))
                datacard_path = os.path.join(inputpath, filename)
                limits, _ = extract_limits(datacard_path, method)
                limits_dict[mass] = limits
    return limits_dict

def print_limits(limits_dict, jobname, test=False):
    # Print the header
    print(f"\n\033[93m{'Mass':<10}", end="")
    for key in patterns_to_look_for.keys(): print(f"{key:<15}", end="")
    print('\033[0m')
    
    # Print the limits for each mass point
    for mass, limits in sorted(limits_dict.items()):
        print(f"{mass:<10}", end="")
        for key in patterns_to_look_for.keys():
            value = limits.get(key, 'N/A')
            print(f"{value:<15}", end="")
        print()

    if not test:
        outdir = 'limits'
        os.makedirs(outdir, exist_ok=True)
        output_file = f"limits/{jobname}.txt"
        output_file = output_file.replace('datacards', 'limits')
        with open(output_file, 'w') as file:
            file.write(f"{'Mass':<10}")
            for key in patterns_to_look_for.keys():
                file.write(f"{key:<15}")
            file.write("\n")

            # Write the limits for each mass point to the file
            for mass, limits in sorted(limits_dict.items()):
                file.write(f"{mass:<10}")
                for key in patterns_to_look_for.keys():
                    value = limits.get(key, 'N/A')
                    file.write(f"{value:<15}")
                file.write("\n")

        print(f"\nOutput written to \033[93m{output_file}\033[0m")
        
# Main execution
if __name__ == "__main__":
    # Parse arguments
    parser = argparse.ArgumentParser(description="Run combine command and extract limits.")
    parser.add_argument("--jobname", type=str, help="Jobname containing datacards.")
    parser.add_argument("--method", type=str, default='AsymptoticLimits', help="Method for combine.")
    parser.add_argument("--test", action='store_true', help="Run the subprocess once for testing and display the full output.")
    args = parser.parse_args()

    start_time = time.time()
        
    limits_dict = process_datacards(args.jobname, args.method, test=args.test)
    print_limits(limits_dict, args.jobname, test=args.test)

    end_time = time.time()
    print(f"Processing completed in {end_time - start_time:.2f} seconds.\n")
