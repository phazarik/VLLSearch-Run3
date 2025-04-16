import os
import numpy as np
from datetime import datetime
today = datetime.now().strftime("%Y-%m-%d")

# Define your cross-section limits and sample details
sigdict = {
    'VLLD_ele': {
        'M100': {'mass': 100, 'xsec': 16.9, 'ngen': 110871, 'scale': 10},
        'M200': {'mass': 200, 'xsec': 1.36, 'ngen': 73730, 'scale': 1},
        'M300': {'mass': 300, 'xsec': 0.291, 'ngen': 24753, 'scale': 1},
        'M400': {'mass': 400, 'xsec': 0.0907, 'ngen': 24491, 'scale': 1},
        'M600': {'mass': 600, 'xsec': 0.0149, 'ngen': 24611, 'scale': 1},
        'M800': {'mass': 800, 'xsec': 0.00347, 'ngen': 23680, 'scale': 1},
        #'M1000': {'mass': 1000, 'xsec': 0.000971, 'ngen': 24286, 'scale': 1}
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

# Path for CMS-combine output files and the output directory for the sigma limits
combine_output_path = 'fromCMScombine/2025-04-16'
files = os.listdir(combine_output_path)
outdir = 'sigmaBlimits/'+today
os.makedirs(outdir, exist_ok=True)

# Process each file in the directory
for f in files:
    if not f.endswith('.txt'): continue
    
    infile = os.path.join(combine_output_path, f)
    outfile = os.path.join(outdir, 'sigmaB_' + f)

    # Iterate over each sample in the dictionary
    for sample, subdict in sigdict.items():
        if sample not in f: continue  # Skip if the sample is not in the filename

        print(f'\nCalculating sigma*B for {sample} using {f}:')
        with open(infile, 'r') as inf, open(outfile, 'w') as outf:
            # Write the header for the output file
            outf.write("Mass\tObserved\tExp_2down\tExp_1down\tExp\tExp_1up\tExp_2up\ttheory\n")

            # Skip the header line in the input file
            header = inf.readline().strip().split()

            # Read each line from the input file and calculate sigmaB limits
            for line in inf:
                values = line.strip().split()
                mass = int(values[0])  # Get mass from the first column
                r_values = {
                    'obs': float(values[1]),
                    'exp_2down': float(values[2]),
                    'exp_1down': float(values[3]),
                    'exp': float(values[4]),
                    'exp_1up': float(values[5]),
                    'exp_2up': float(values[6])
                }

                print(f"Mass: {mass}, R-values: {r_values}")

                # Calculate sigma*B values for the given mass and sample
                for subsample, val in subdict.items():
                    if val['mass'] == mass:
                        sigmaB_theory = val['xsec']  # Cross-section for the given mass

                        # Calculate sigma*B for each limit
                        xsec_limit = {
                            'obs': r_values['obs'] * sigmaB_theory,
                            'exp_2down': r_values['exp_2down'] * sigmaB_theory,
                            'exp_1down': r_values['exp_1down'] * sigmaB_theory,
                            'exp': r_values['exp'] * sigmaB_theory,
                            'exp_1up': r_values['exp_1up'] * sigmaB_theory,
                            'exp_2up': r_values['exp_2up'] * sigmaB_theory
                        }

                        # Write the values to the output file in the required format
                        outf.write(f"{mass}\t"
                                   f"{xsec_limit['obs']:.2f}\t"
                                   f"{xsec_limit['exp_2down']:.2f}\t"
                                   f"{xsec_limit['exp_1down']:.2f}\t"
                                   f"{xsec_limit['exp']:.2f}\t"
                                   f"{xsec_limit['exp_1up']:.2f}\t"
                                   f"{xsec_limit['exp_2up']:.2f}\t"
                                   f"{sigmaB_theory:.2f}\n")

                        print(f"Mass: {mass}, Sigma*B Limits: {xsec_limit}")

        print(f'File created: {outfile}')
    print('-' * 100)

print('\nAll CMS-combine files have been converted to sigma*B limits!\n')
