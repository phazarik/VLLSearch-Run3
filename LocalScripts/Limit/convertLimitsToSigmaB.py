import os, sys
import numpy as np

models = ['VLLD_mu', 'VLLD_ele']

sigdict = {
    'VLLD_ele': {
        'M100': {'mass': 100, 'xsec': 16.9, 'ngen': 110871},
        'M200': {'mass': 200, 'xsec': 1.36, 'ngen': 73730},
        'M300': {'mass': 300, 'xsec': 0.291, 'ngen': 24753},
        'M400': {'mass': 400, 'xsec': 0.0907, 'ngen': 24491},
        'M600': {'mass': 600, 'xsec': 0.0149, 'ngen': 24611},
        'M800': {'mass': 800, 'xsec': 0.00347, 'ngen': 23680},
        'M1000': {'mass': 1000, 'xsec': 0.000971, 'ngen': 24286}
    },
    'VLLD_mu': {
        'M100': {'mass': 100, 'xsec': 16.9, 'ngen': 111926},
        'M200': {'mass': 200, 'xsec': 1.36, 'ngen': 73908},
        'M300': {'mass': 300, 'xsec': 0.291, 'ngen': 25022},
        'M400': {'mass': 400, 'xsec': 0.0907, 'ngen': 24299},
        'M600': {'mass': 600, 'xsec': 0.0149, 'ngen': 24890},
        'M800': {'mass': 800, 'xsec': 0.00347, 'ngen': 24763}
    }
}

for model in models:
    for sample, subs in sigdict.items():
        if model not in sample: continue

        input_file = f'combinelimits_r/limits_{model}.txt'
        output_file = f'combinelimits_sigmaB/limits_sigmaB_{model}.txt'

        os.makedirs('combinelimits_sigmaB', exist_ok=True)
        
        print(f'\nCalculating sigma*B for {model}:')
        with open(input_file, 'r') as f, open(output_file, 'w') as out_file:

            out_file.write("Mass\tObserved\tExp_2down\tExp_1down\tExp\tExp_1up\tExp_2up\tthoery\n")

            # Skip the header in the input file:
            header = f.readline().strip().split()
            
            # Read each line from the file
            for line in f:
                values = line.strip().split()
                mass = int(values[0])
                r_values = {
                    'obs':       float(values[1]),
                    'exp_2down': float(values[2]),
                    'exp_1down': float(values[3]),
                    'exp':       float(values[4]),
                    'exp_1up':   float(values[5]),
                    'exp_2up':   float(values[6])
                }

                print(mass, r_values)

                xsec_limit = {}
                 # Iterate through the subsamples and update the cross-section values
                for subsample, val in subs.items():
                    if val['mass'] == mass:
                        #########################################
                        # Observed limit = r-value * sigmaBtheory
                        #########################################
                        sigmaB_theory = val['xsec']

                        # Convert expected r-values to cross-section
                        xsec_limit = {
                            'obs':       r_values['obs'] * sigmaB_theory,
                            'exp_2down': r_values['exp_2down'] * sigmaB_theory,
                            'exp_1down': r_values['exp_1down'] * sigmaB_theory,
                            'exp':       r_values['exp'] * sigmaB_theory,
                            'exp_1up':   r_values['exp_1up'] * sigmaB_theory,
                            'exp_2up':   r_values['exp_2up'] * sigmaB_theory
                        }
                        #sigdict[sample][subsample]['limit'] = xsec_limit 

                        # Write to the output file with formatting (2 decimal places)
                        out_file.write(f"{mass}\t{xsec_limit['obs']:.2f}\t{xsec_limit['exp_2down']:.2f}\t"
                                       f"{xsec_limit['exp_1down']:.2f}\t{xsec_limit['exp']:.2f}\t"
                                       f"{xsec_limit['exp_1up']:.2f}\t{xsec_limit['exp_2up']:.2f}\t{sigmaB_theory:.2f}\n")

                print(mass, xsec_limit)
                #break#subsample
        print(f'File created: {output_file}')
        #break#sample
    print('-'*100)
    #break #model
print('\nReady to plot!')
