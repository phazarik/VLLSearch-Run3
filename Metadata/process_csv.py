# =============================================================================
# Script: process_csv.py
#
# How to run:
#     python3 process_csv.py --campaign 2018_UL
#
# Features:
# - Reads a CSV file located at CSVfromSpreadsheet/{campaign}.csv
# - Extracts sample, subsample, lumi, and dataset name from the spreadsheet
# - Filters out irrelevant or empty rows
# - Writes a JSON file: extracted_jsons/lumidata_{campaign}.json
#   containing a nested dict: {sample: {subsample: lumi}}
# - Writes a text file: CRAB_samplelist/{campaign}.txt
#   containing CRAB job info lines like:
#     (sample_subsample, dataset_name, 'mc')
# =============================================================================

import os, argparse, csv, json
import pandas as pd

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

        parts = namecell.split('_', 1)
        sample = parts[0]
        subsample = parts[1] if len(parts) > 1 else ''
        dataset = row[2].strip()
        lumi = row[8].strip() if len(row) > 8 else ''

        try: lumi_val = float(lumi)
        except: lumi_val = ''
        rows.append((sample, subsample, lumi_val, dataset))

    df_lumi = pd.DataFrame(rows, columns=['sample', 'subsample', 'lumi', 'dataset'])
    return df_lumi

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

    lumi_dict = {}
    df = df[df["lumi"].astype(bool) & df["lumi"].notna()][["sample", "subsample", "lumi"]].reset_index()
    print(df[~df["sample"].isin(["Muon", "EGamma"])].to_string(index=False))

    for _, row in df.iterrows():
        sample = row['sample']
        subsample = row['subsample']
        lumi = row['lumi']
        if sample in ["Muon", "EGamma"] and campaign in data_lumi: lumi = data_lumi[campaign]
        if sample not in lumi_dict: lumi_dict[sample] = {}
        lumi_dict[sample][subsample] = lumi

    # For Run3 campaigns, add VLLD/VLL if missing
    if campaign.startswith("Run3") and not any(s for s in lumi_dict if s.startswith("VLL")):
        lumi_dict["VLLD-ele"] = {
            "100": 6575.278,
            "200": 707537.143,
            "300": 1096506.55,
            "400": 3475000.0,
            "600": 21134453.782,
            "800": 88805776.173,
            "1000": 324871794.872
        }
        lumi_dict["VLLD-mu"] = {
            "100": 6636.095,
            "200": 706118.095,
            "300": 1097816.594,
            "400": 3397625.0,
            "600": 21126050.42,
            "800": 90794223.827,
            "1000": 321794871.795
        }

    os.makedirs('extracted_jsons', exist_ok=True)
    outfile = f'extracted_jsons/lumidata_{campaign}.json'
    with open(outfile, 'w') as f:
        json.dump(lumi_dict, f, indent=4)
    print('File written:', outfile)
    
def write_crab_samplelist_from_lumi_df(df, campaign):
    os.makedirs('CRAB_samplelist', exist_ok=True)
    outfile = f'CRAB_samplelist/{campaign}.txt'
    with open(outfile, 'w') as f:
        f.write('( #wrapper\n')
        valid_rows = [row for _, row in df.iterrows() if row['dataset']]
        for i, row in enumerate(valid_rows):
            sample_subsample = f"{row['sample']}_{row['subsample']}"
            dataset_name = row['dataset']
            if 'SingleMuon' in dataset_name or 'Muon' in dataset_name: dtype = 'muon'
            elif 'SingleElectron' in dataset_name or 'EGamma' in dataset_name: dtype = 'egamma'
            else: dtype = 'mc'
            comma = ',' if i < len(valid_rows) - 1 else ''
            f.write(f'("{sample_subsample}", "{dataset_name}", "{dtype}"){comma}\n')
        f.write(')\n')
    print('File written:', outfile)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--campaign', required=True)
    args = parser.parse_args()
    df_lumi = read_file_into_df(args.campaign)
    write_lumidata_into_json(df_lumi, args.campaign)
    write_crab_samplelist_from_lumi_df(df_lumi, args.campaign)

if __name__ == "__main__": main()
