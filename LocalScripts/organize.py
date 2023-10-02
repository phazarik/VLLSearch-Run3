import os, sys, argparse

parser=argparse.ArgumentParser()
parser.add_argument('--jobname',type=str, required=True, help='AnalysisName: Such as VLL2018_Mar1_v0')
parser.add_argument('--date',type=str, required=True, help='In the format : YYYY-MM-DD')
parser.add_argument('--dryrun' ,type=bool,required=False,help='Check If everything is correct before submitting')
args=parser.parse_args()

indir = args.jobname
date = args.date #'2023-10-02'
dryrun = args.dryrun

indict = {
    "VLLS":{
        "ele":{
            "M100":{},
            "M125":{},
            "M150":{},
            "M200":{},
            "M250":{},
            "M300":{},
            "M350":{},
            "M400":{},
            "M450":{},
            "M500":{},
            "M750":{},
            "M1000":{}
        },
        "mu":{
            "M100":{},
            "M125":{},
            "M150":{},
            "M200":{},
            "M250":{},
            "M300":{}, #350 is missing
            "M400":{},
            "M450":{},
            "M500":{},
            "M750":{},
            "M1000":{}
        }
    },
    "VLLD":{
        "ele":{
            "M100":{},
            "M200":{},
            "M300":{},
            "M400":{},
            "M600":{},
            "M800":{},
            "M1000":{},
            "M1200":{},
            "M1400":{},
            "M1600":{},
            "M1800":{},
            "M2000":{},
            "M2200":{}
        },
        "mu":{
            "M100":{},
            "M200":{},
            "M300":{},
            "M400":{},
            "M600":{},
            "M800":{}, #1000 is missing
            "M1200":{},
            "M1400":{},
            "M1600":{},
            "M1800":{},
            "M2000":{},
            "M2200":{}
        }
    }
}

plots = ['allvll_decay']

for model, flavors in indict.items():
    for flav, masses in flavors.items():
        for mass, parameters in masses.items():
            foldername = f'hists/{indir}/{model}_{flav}_{mass}_{date}'
            files = os.listdir(foldername)
            if len(files) == 0 : print(f'Files not found : {model}_{flav}_{mass}')
            else :
                infile = f'{foldername}/hst_{model}_{flav}_{mass}.root'
                if len(files) > 1:
                    #Add hst files together into one file.
                    process = f'hadd {infile} {foldername}/*.root'
                    cleanup = f'rm -rf {foldername}/*_data.root'
                    if dryrun == True:
                        print(process)
                        print(cleanup)
                    else:
                        os.system(process)
                        os.system(cleanup)
                else :
                    if files[0].endswith('_data.root'):
                        process = f'mv {foldername}/*_data.root {infile}'
                        if dryrun == True:
                            print(process)
                        else:
                            os.system(process)

                if dryrun == True: break
                mkdir = 'mkdir -p input_files'
                if os.path.exists(infile):
                    main = f'cp {infile} input_files/hst_{model}_{flav}_{mass}.root'
                    if dryrun == True:
                        print(main)
                    else:
                        os.system(mkdir)
                        os.system(main)
                else:
                    print(f'Not found : {infile}')
        #break
    #break
    
print('Done!')
