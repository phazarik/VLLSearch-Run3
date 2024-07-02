###############################################################
# This python script uses ROOT to lumi-scaled histograms      #
# ROOT only works in the same environment where it was built. #
###############################################################

import os, sys, json, argparse
import ROOT #Run it with default (base) environment
import time
start_time = time.time()

#---------------------------------------------------------------------------------------------
campaign = '2018_UL_fixed'
parser=argparse.ArgumentParser()
parser.add_argument('--test'   ,type=bool,required=False,help='Check one job')
args=parser.parse_args()
test     = args.test
INDIR    = 'inputs/'+campaign
OUTDIR   = 'inputs/'+campaign+'_lumiscaled'
if not test : os.system(f'mkdir -p {OUTDIR}')
test     = args.test
#with open('../../../InputJsons/lumidata_2018.json', 'r') as file: json_data = json.load(file)
with open('../../../InputJsons/ratiodata_2018.json', 'r') as file: json_data = json.load(file)
indict   = json_data
#---------------------------------------------------------------------------------------------

for sample, subdict in indict.items():
    for subsample, lumi in subdict.items():
        print("\033[33m",sample, subsample,"\033[0m",lumi)

        file_before = f'{INDIR}/hst_{sample}_{subsample}.root'
        file_after = f'{OUTDIR}/hst_{sample}_{subsample}.root'

        if not os.path.exists(file_before):
            print(f'\033[31mWarning: file not found: {file_before}\033[0m')
            continue

        #Loading ROOT
        if not test:
        
            # Open the input ROOT file
            input_file = ROOT.TFile.Open(file_before, "READ")
            if not input_file:
                print(f"Error: Cannot open file: {file_before}")
                continue

            # Create the output ROOT file
            output_file = ROOT.TFile.Open(file_after, "RECREATE")
            if not output_file:
                print(f"Error: Cannot create file: {file_after}")
                continue

            # Loop over all keys in the input file
            for key in input_file.GetListOfKeys():
                obj = key.ReadObj()
                if isinstance(obj, (ROOT.TH1F, ROOT.TH2F)):
                    #if campaign=='2018_UL' : obj.Scale(59800/lumi)
                    obj.Scale(lumi)
                    output_file.cd()
                    obj.Write()
        
            input_file.Close()
            output_file.Close()
            print(f'Created file: {file_after}')
