import os, sys
import time
from datetime import datetime
today = datetime.today().strftime('%Y-%m-%d')
start_time = time.time()

limit_files = os.listdir('sigmaBlimits')

for filename in limit_files:
    
    print(f'\nMaking plots from sigmaBlimits/{filename}')

    ### Setting modelname
    modelname = None 
    if 'VLLD_mu'    in filename: modelname="VLLD-#mu"
    elif 'VLLD_ele' in filename: modelname="VLLD-e"
    if modelname == None: print('Error: please set modelname.')
    
    ### Setting campaign
    campaign = None
    if '2018'   in filename:                           campaign = "2018_UL"
    if '2017'   in filename:                           campaign = "2017_UL"
    elif '2016' in filename and 'preVFP'  in filename: campaign = "2016preVFP_UL"
    elif '2016' in filename and 'postVFP' in filename: campaign = "2016postVFP_UL"
    if campaign == None: print('Error: please set campagin.')
    
    ### Setting channel
    channel = None
    if 'ee'   in filename: channel = 'ee channel'
    elif 'em' in filename: channel = 'e#mu channel'
    elif 'me' in filename: channel = '#mue channel'
    elif 'mm' in filename: channel = '#mu#mu channel'
    else : channel = filename.split('_')[-1].split('.')[0]
    if channel == None: print('Error: please set channel.')

    ### Setting outfile:
    outdir = "plots/"+today
    os.makedirs(outdir, exist_ok=True)
    outfile = filename.replace('.txt', '.png')
    outfile = os.path.join(outdir, outfile)

    ### Running ROOT on bash
    parameters = f'"{filename}","{modelname}","{campaign}","{channel}","{outfile}"'
    processline = f"root -q -b -l 'makeLimitPlot.C({parameters})'"
    print(processline)
    os.system(processline)

end_time = time.time()
time_taken = int(end_time - start_time)
print('\nDone!')
print('Time taken = {time_taken} seconds.')

