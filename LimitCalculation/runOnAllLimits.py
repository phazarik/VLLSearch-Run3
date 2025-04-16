import os, re

indir = 'sigmaBlimits/2025-01-08/'
outdir = 'plots'
os.makedirs(outdir, exist_ok=True)

for filename in os.listdir(indir):
    if not filename.endswith('.txt'): continue

    infilepath = os.path.join(indir, filename)
    filename = filename.split('.')[0]

    # Determine modelname
    modelname = ''
    if   'VLLD_mu'  in filename: modelname = 'VLLD-#mu'
    elif 'VLLD_ele' in filename: modelname = 'VLLD-e'

    # Determine campaign
    campaign = ''
    if '2017-18' in filename: campaign = '2017+2018'
    elif '2017'  in filename: campaign = '2017_UL'
    elif '2018'  in filename: campaign = '2018_UL'

    # Determine channel
    channel = ''
    if   filename.endswith('mm'): channel = '#mu#mu channel'
    elif filename.endswith('me'): channel = '#mue channel'
    elif filename.endswith('em'): channel = 'e#mu channel'
    elif filename.endswith('ee'): channel = 'ee channel'
    else:                         channel = 'combined'

    outfilename = (
        "limit_"
        + re.sub(r"[+# ]", "_", modelname.replace("#", ""))
        + "_"
        + re.sub(r"[+# ]", "_", campaign)
        + "_"
        + re.sub(r"[+# ]", "_", channel)
        + ".png"
    )

    arguments   = f'"{infilepath}", "{modelname}", "{campaign}", "{channel}", "{os.path.join(outdir, outfilename)}"'
    processline = f"root -q -b -l 'makeLimitPlot.C({arguments})'"
    #print(processline)
    os.system(processline)
