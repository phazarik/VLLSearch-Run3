import os
import numpy as np
from datetime import datetime

today = datetime.now().strftime("%Y-%m-%d")

sigdict = {
    'VLLD_ele': {
        'M100': {'mass': 100, 'xsec': 16.9, 'ngen': 110871, 'scale': 10},
        'M200': {'mass': 200, 'xsec': 1.36, 'ngen': 73730, 'scale': 1},
        'M300': {'mass': 300, 'xsec': 0.291, 'ngen': 24753, 'scale': 1},
        'M400': {'mass': 400, 'xsec': 0.0907, 'ngen': 24491, 'scale': 1},
        'M600': {'mass': 600, 'xsec': 0.0149, 'ngen': 24611, 'scale': 1},
        'M800': {'mass': 800, 'xsec': 0.00347, 'ngen': 23680, 'scale': 1},
    },
    'VLLD_mu': {
        'M100': {'mass': 100, 'xsec': 16.9, 'ngen': 111926, 'scale': 10},
        'M200': {'mass': 200, 'xsec': 1.36, 'ngen': 73908, 'scale': 1},
        'M300': {'mass': 300, 'xsec': 0.291, 'ngen': 25022, 'scale': 1},
        'M400': {'mass': 400, 'xsec': 0.0907, 'ngen': 24299, 'scale': 1},
        'M600': {'mass': 600, 'xsec': 0.0149, 'ngen': 24890, 'scale': 1},
        'M800': {'mass': 800, 'xsec': 0.00347, 'ngen': 24763, 'scale': 1}
    }
}

combine_output_path = 'fromCMScombine/2025-04-21'
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

    subdict = sigdict.get(model_name)
    if subdict is None:
        print(f"Model {model_name} not in sigdict. Skipping {f}.")
        continue

    print(f'\nCalculating sigma*B for {model_name} using {f}:')
    try:
        with open(infile, 'r') as inf, open(outfile, 'w') as outf:
            outf.write("Mass\tObserved\tExp_2down\tExp_1down\tExp\tExp_1up\tExp_2up\ttheory\n")
            header = inf.readline().strip().split()

            for line in inf:
                values = line.strip().split()
                if len(values) < 7:
                    print(f"Skipping malformed line in {f}: {line.strip()}")
                    continue

                try:
                    mass = int(values[0])
                except ValueError:
                    print(f"Invalid mass value in {f}: {values[0]}. Skipping line.")
                    continue

                if f"M{mass}" not in subdict:
                    print(f"Mass {mass} not found in sigdict for {model_name}. Skipping.")
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

                xsec_limit = {
                    'obs': r_values['obs'] * sigmaB_theory,
                    'exp_2down': r_values['exp_2down'] * sigmaB_theory,
                    'exp_1down': r_values['exp_1down'] * sigmaB_theory,
                    'exp': r_values['exp'] * sigmaB_theory,
                    'exp_1up': r_values['exp_1up'] * sigmaB_theory,
                    'exp_2up': r_values['exp_2up'] * sigmaB_theory
                }

                outf.write(f"{mass}\t"
                           f"{xsec_limit['obs']:.4f}\t"
                           f"{xsec_limit['exp_2down']:.4f}\t"
                           f"{xsec_limit['exp_1down']:.4f}\t"
                           f"{xsec_limit['exp']:.4f}\t"
                           f"{xsec_limit['exp_1up']:.4f}\t"
                           f"{xsec_limit['exp_2up']:.4f}\t"
                           f"{sigmaB_theory:.4f}\n")

                print(f"Mass: {mass}, Sigma*B Limits: {xsec_limit}")

        print(f'File created: {outfile}')
    except Exception as e: print(f"Error processing {f}: {e}")
    print('-' * 100)

print('\nAll CMS-combine files have been converted to sigma*B limits!\n')
