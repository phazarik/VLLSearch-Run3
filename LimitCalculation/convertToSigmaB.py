import os
import numpy as np
from datetime import datetime
from tabulate import tabulate
from rich.console import Console
console = Console(highlight=False)
print = console.print

today = datetime.now().strftime("%Y-%m-%d")

sigdict = {
    'VLLD_ele': {
        'M100': {'mass': 100, 'xsec': 16.9, 'ngen': 110871, 'scale': 1},
        'M200': {'mass': 200, 'xsec': 1.36, 'ngen': 73730, 'scale': 1},
        'M300': {'mass': 300, 'xsec': 0.291, 'ngen': 24753, 'scale': 1},
        'M400': {'mass': 400, 'xsec': 0.0907, 'ngen': 24491, 'scale': 10},
        'M600': {'mass': 600, 'xsec': 0.0149, 'ngen': 24611, 'scale': 10},
        'M800': {'mass': 800, 'xsec': 0.00347, 'ngen': 23680, 'scale': 50},
    },
    'VLLD_mu': {
        'M100': {'mass': 100, 'xsec': 16.9, 'ngen': 111926, 'scale': 1},
        'M200': {'mass': 200, 'xsec': 1.36, 'ngen': 73908, 'scale': 1},
        'M300': {'mass': 300, 'xsec': 0.291, 'ngen': 25022, 'scale': 1},
        'M400': {'mass': 400, 'xsec': 0.0907, 'ngen': 24299, 'scale': 10},
        'M600': {'mass': 600, 'xsec': 0.0149, 'ngen': 24890, 'scale': 10},
        'M800': {'mass': 800, 'xsec': 0.00347, 'ngen': 24763, 'scale': 50}
    }
}

combine_output_path = 'fromCMScombine/2025-04-22'
files = os.listdir(combine_output_path)
outdir = 'sigmaBlimits/' + today
os.makedirs(outdir, exist_ok=True)

for f in files:
    if not f.endswith('.txt'):
        continue

    infile = os.path.join(combine_output_path, f)
    outfile = os.path.join(outdir, f.replace('limits_', 'sigmaB_'))

    model_name = None
    if 'VLLD_ele' in f:  model_name = 'VLLD_ele'
    elif 'VLLD_mu' in f: model_name = 'VLLD_mu'
    else:
        print(f"Model not found in file name: {f}. Skipping.")
        continue

    print(f"\n{model_name}", style='yellow bold')
    subdict = sigdict.get(model_name)
    if subdict is None:
        print(f"Model {model_name} not in sigdict. Skipping {f}.")
        continue

    print(f'{f}', style='yellow bold')
    try:
        with open(infile, 'r') as inf, open(outfile, 'w') as outf:
            outf.write("Mass\tObserved\tExp_2down\tExp_1down\tExp\tExp_1up\tExp_2up\ttheory\n")
            header = inf.readline().strip().split()
            table_rows = []

            for line in inf:
                values = line.strip().split()
                if 'N/A' in values:
                    print(f"Skipping line with N/A in {f}: {line.strip()}", style='red')
                    continue
                
                if len(values) < 7:
                    print(f"Skipping malformed line in {f}: {line.strip()}", style='red')
                    continue

                try:
                    mass = int(values[0])
                except ValueError:
                    print(f"Invalid mass value in {f}: {values[0]}. Skipping line.", style='red')
                    continue

                if f"M{mass}" not in subdict:
                    print(f"Mass {mass} not found in sigdict for {model_name}. Skipping.", style='red')
                    continue

                r_values = {
                    'obs': float(values[1]),
                    'exp_2down': float(values[2]),
                    'exp_1down': float(values[3]),
                    'exp': float(values[4]),
                    'exp_1up': float(values[5]),
                    'exp_2up': float(values[6])
                }

                val = subdict[f"M{mass}"]
                sigmaB_theory = val['xsec']

                #multiply r-values by same proportaion as in prepare_datacards.py
                scale = val.get('scale', 1)
                xsec_limit = {
                    'obs': r_values['obs'] * sigmaB_theory * scale,
                    'exp_2down': r_values['exp_2down'] * sigmaB_theory * scale,
                    'exp_1down': r_values['exp_1down'] * sigmaB_theory * scale,
                    'exp': r_values['exp'] * sigmaB_theory * scale,
                    'exp_1up': r_values['exp_1up'] * sigmaB_theory * scale,
                    'exp_2up': r_values['exp_2up'] * sigmaB_theory * scale
                }

                outf.write(f"{mass}\t"
                           f"{xsec_limit['obs']:.4f}\t"
                           f"{xsec_limit['exp_2down']:.4f}\t"
                           f"{xsec_limit['exp_1down']:.4f}\t"
                           f"{xsec_limit['exp']:.4f}\t"
                           f"{xsec_limit['exp_1up']:.4f}\t"
                           f"{xsec_limit['exp_2up']:.4f}\t"
                           f"{sigmaB_theory:.4f}\n")

                table_rows.append(
                    [mass] + 
                    [f"{xsec_limit[key]:.4f}" for key in xsec_limit] + 
                    [f"{sigmaB_theory:.4f}"] +
                    ['\033[1;33m✓\033[0m' if xsec_limit['exp'] < sigmaB_theory else '\033[1;31m✗\033[0m']
                )
                #line loop
            #with file open
        #try
            
        print(tabulate(
        table_rows,
            headers=["Mass", "Obs", "Exp -2σ", "Exp -1σ", "Exp", "Exp +1σ", "Exp +2σ", "Theory", "?"],
            tablefmt="psql"
        ))
        print(f'File created: {outfile}', style='yellow')
    except Exception as e: print(f"Error processing {f}: {e}", style='red')

print('\nAll CMS-combine files have been converted to sigma*B limits!\n', style='yellow bold')
