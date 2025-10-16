# =============================================================================
# Script: process_csv.py
#
# How to run:
#     python3 process_csv.py --campaign 2018_UL
#
# Features:
# - Reads a CSV file located at CSVfromSpreadsheet/{campaign}.csv
# - Extracts sample, subsample, lumi, avgGenWt, expEvt, and dataset name
# - Filters out irrelevant or empty rows
# - Writes JSON files:
#       extracted_lumi_jsons/lumidata_{campaign}.json
#       extracted_avgGenWt_jsons/genwtdata_{campaign}.json
#   each containing nested dicts {sample: {subsample: value}}
# - Writes a text file: CRAB_samplelist/{campaign}.txt
#   containing CRAB job info lines like:
#     (sample_subsample, dataset_name, 'mc', expected_events)
# =============================================================================

import os, argparse, csv, json
import pandas as pd

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--campaign', required=True)
    args = parser.parse_args()

    df = read_file_into_df(args.campaign)

    write_lumidata_into_json(df, args.campaign)
    write_genwtdata_into_json(df, args.campaign)
    write_crab_samplelist(df, args.campaign)

#--------------------------------------------------------
def read_file_into_df(campaign):
    csv_file = f"CSVfromSpreadsheet/{campaign}.csv"
    with open(csv_file, newline='') as csvfile:
        reader = csv.reader(csvfile)
        lumilist = list(reader)

    df_all = pd.DataFrame(lumilist)
    rows = []
    for row in df_all.itertuples(index=False):
        row = list(row)
        if len(row) < 3: continue
        namecell = row[1].strip()
        if not namecell or namecell.lower().startswith("sample_subsample"): continue

        parts     = namecell.split('_', 1)
        sample    = parts[0]
        subsample = parts[1] if len(parts) > 1 else ''
        dataset   = row[2].strip()
        lumi      = row[8].strip() if len(row) > 8 else ''
        avgGenWt  = row[10].strip() if len(row) > 10 else ''

        try: lumi_val = float(lumi)
        except: lumi_val = ''
        try: genwt_val = float(avgGenWt)
        except: genwt_val = ''

        rows.append((sample, subsample, lumi_val, genwt_val, dataset))

    df = pd.DataFrame(rows, columns=['sample', 'subsample', 'lumi', 'avgGenWt', 'dataset'])
    df = df[df.apply(lambda row: any(str(x).replace('.', '', 1).isdigit() for x in row), axis=1)].reset_index(drop=True) ## Dropping rows with no number
    df = df[(df['lumi'] != 0) | (df['sample'].isin(['Muon','EGamma']))].reset_index(drop=True) ## Dropping non-data rows with zero lumi
    print(df)
    return df

def write_lumidata_into_json(df, campaign):

    data_lumi = {
        "2016preVFP_UL": 19692,
        "2016postVFP_UL": 16227,
        "2017_UL": 41480,
        "2018_UL": 59830,
        "Run3Summer22": 7980.4,
        "Run3Summer22EE": 26671.7,
        "Run3Summer23": 17794.0,
        "Run3Summer23BPix": 9451.0
    }

    os.makedirs('extracted_lumi_jsons', exist_ok=True)
    df_valid = df[df["lumi"].astype(bool) & df["lumi"].notna()][["sample", "subsample", "lumi"]].reset_index(drop=True)

    lumi_dict = {}
    for _, row in df_valid.iterrows():
        sample = row['sample']
        subsample = row['subsample']
        lumi = row['lumi']
        if (sample.startswith("Muon") or sample.startswith("EGamma")) and campaign in data_lumi:
            lumi = data_lumi[campaign]
        if sample not in lumi_dict: lumi_dict[sample] = {}
        lumi_dict[sample][subsample] = lumi

    outfile = f"extracted_lumi_jsons/lumidata_{campaign}.json"
    with open(outfile, "w") as f: json.dump(lumi_dict, f, indent=4)
    print("File written:", outfile)

def write_genwtdata_into_json(df, campaign):
    os.makedirs('extracted_avgGenWt_jsons', exist_ok=True)
    df["avgGenWt"] = df["avgGenWt"].apply(lambda x: x if pd.notna(x) and x != 0 else 1)
    df = df[~df["sample"].isin(["Muon", "EGamma"])]
    df = df[["sample", "subsample", "avgGenWt"]].reset_index(drop=True)

    genwt_dict = {}
    for _, row in df.iterrows():
        sample, subsample, value = row["sample"], row["subsample"], row["avgGenWt"]
        if sample not in genwt_dict: genwt_dict[sample] = {}
        genwt_dict[sample][subsample] = value

    outfile = f"extracted_avgGenWt_jsons/genwtdata_{campaign}.json"
    with open(outfile, "w") as f: json.dump(genwt_dict, f, indent=4)
    print("File written:", outfile)

def write_crab_samplelist(df, campaign):
    #Filtering
    if "Run3" in campaign: df = df[~df['sample'].str.contains("VLLD")].reset_index(drop=True)
    
    os.makedirs('CRAB_samplelist', exist_ok=True)
    outfile = f'CRAB_samplelist/{campaign}.txt'
    with open(outfile, 'w') as f:
        f.write('( #wrapper\n')
        valid_rows = [row for _, row in df.iterrows() if row['dataset']]
        for i, row in enumerate(valid_rows):
            sample_subsample = f"{row['sample']}_{row['subsample']}"
            dataset_name = row['dataset']
            if 'SingleMuon' in dataset_name or 'Muon' in dataset_name:         dtype = 'muon'
            elif 'SingleElectron' in dataset_name or 'EGamma' in dataset_name: dtype = 'egamma'
            elif 'QCD' in dataset_name and any(x in dataset_name for x in ('MuEnriched', 'EMEnriched')): dtype = 'qcd'
            elif 'VLLD' in dataset_name: dtype = 'doublet'
            else: dtype = 'mc'
            comma = ',' if i < len(valid_rows) - 1 else ''
            f.write(f'("{sample_subsample}", "{dataset_name}", "{dtype}"){comma}\n')
        f.write(')\n')
    print('File written:', outfile)

if __name__ == "__main__": main()
