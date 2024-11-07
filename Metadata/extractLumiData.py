#-------------------------------------------------------------------------------------------------------------------
# This python script produces lumidata_X.json files for capturing the luminosity information for each MC sample.
# First, download the spreadsheet containing samplename and lumi information as csv file for each sheet.
# Link: https://docs.google.com/spreadsheets/d/10LLDj0NRvGVtQPhoFLRR2A7NRblJRvnX_U7wvfeYhvw/edit?pli=1&gid=0#gid=0
# Owner: prachurjya.hazarika@students.iiserpune.ac.in
#-------------------------------------------------------------------------------------------------------------------

import os
import csv
import json

def process_csv_to_json(csv_file_path, json_output_file, datalumi=59800):

    csv_file_path = 'dataFromSpreadsheet/' + csv_file_path
    json_output_file = 'jsondata/' + json_output_file

    #---------------------------------------------
    # Reading the CSV file and store rows in dict
    #---------------------------------------------
    result_dict = {}

    with open(csv_file_path, newline='') as csvfile:
        reader = csv.reader(csvfile)

        # Locating the header row
        header_row_idx = None
        data_rows = []
        for idx, row in enumerate(reader):
            if "Sample" in row:
                header_row_idx = idx
                # Capture remaining rows after the header
                data_rows = list(reader)
                break

        if header_row_idx is None: raise ValueError("Header row with 'Sample' not found in the file.")

        #---------------------------------
        # Processing rows containing data
        #---------------------------------
        current_sample = None
        for row in data_rows:
            if "Missing samples (found in Angira's AN)" in row: break

            sample = row[1].strip() or current_sample  # Get new sample, or use current if empty
            subsample = row[2].strip()
            lumi_str = row[7].strip()

            if sample and subsample and lumi_str:
                current_sample = sample
                try: lumi = float(lumi_str)
                except ValueError: continue
                if sample not in result_dict: result_dict[sample] = {}
                
                result_dict[sample][subsample] = lumi

        ### File reading done.

    # ----------------------------------
    # Organizing data and DY separately
    # ----------------------------------
    dyjets_samples = {}
    data_samples = ["SingleMuon", "EGamma", "SingleElectron"]

    # Move DYJetsToLL samples to the front
    for key in list(result_dict.keys()):
        if "DYJetsToLL" in key: dyjets_samples[key] = result_dict.pop(key)

    # Replace lumi values for data samples with datalumi
    data_samples_dict = {}
    for key in data_samples:
        if key in result_dict:
            data_samples_dict[key] = {subsample: datalumi for subsample in result_dict[key]}

    # Temporarily removing data from the dict (to add at the end)
    result_dict = {key: result_dict[key] for key in result_dict if key not in data_samples}

    # Combining the dictionaries in the right order
    ordered_result_dict = {**dyjets_samples, **result_dict, **data_samples_dict}

    #----------------------------
    # Writing into the json file
    #----------------------------
    with open(json_output_file, "w") as f:
        json.dump(ordered_result_dict, f, indent=2)

    print(f"Dictionary successfully written to {json_output_file}.")

print('\nFunction loaded.\n')

os.makedirs('jsondata', exist_ok=True)

# Process the CSV files with the appropriate luminosity values
process_csv_to_json("Data and MC Sample Database - 2018_UL.csv", "lumidata_2018_UL.json", 59830)
process_csv_to_json("Data and MC Sample Database - 2017_UL.csv", "lumidata_2017_UL.json", 41480)
process_csv_to_json("Data and MC Sample Database - 2016preVFP_UL.csv", "lumidata_2016preVFP_UL.json", 19692)
process_csv_to_json("Data and MC Sample Database - 2016postVFP_UL.csv", "lumidata_2016postVFP_UL.json", 16227)

print('Done!\n')
