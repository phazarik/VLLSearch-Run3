#-----------------------------------------------------------------------------------------------------
# input file: ../../AnalysisScripts/Corrections/SFfromPOG/electron_id_sf.txt
# columns: campaign >> etalow >> etahigh >> ptlow >> pthigh >> philow >> phihigh >> down >> sf >> up
#-----------------------------------------------------------------------------------------------------

import os, sys
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

def load_df(infile):
    colnames = ['campaign', 'etalow', 'etahigh', 'ptlow', 'pthigh', 'philow', 'phihigh', 'sfdown', 'sf', 'sfup']
    df = pd.read_csv(infile, sep='\s+', comment='#', names=colnames)
    df = df[~((df['sfdown'] == 0.0) & (df['sf'] == 1.0) & (df['sfup'] == 2.0))]
    return df

def make_electronSF_plot(df, correction_name, campaign_dict, run, yrange=(0.99, 1.01)):
    selected_campaigns = list(campaign_dict.keys())
    df = df[df['campaign'].isin(selected_campaigns)]
    if df.empty:
        print("\033[31mWarning: none of the requested campaigns found in input.\033[0m")
        return

    outdir = f'plots/{correction_name}'
    os.makedirs(outdir, exist_ok=True)

    eta_phi_bins = df[['etalow', 'etahigh', 'philow', 'phihigh']].drop_duplicates().values

    count = 0
    for bin_index, (etalow, etahigh, philow, phihigh) in enumerate(eta_phi_bins):

        prefix = f"{bin_index:03d}"
        ymin, ymax = yrange
        ymax_dev = 0
        
        fig, ax = plt.subplots(figsize=(5, 4))
        ax.axhline(1.0, color='black', linestyle='--', linewidth=1)

        for campaign, props in campaign_dict.items():
            subdf = df[(df['campaign'] == campaign) &
                       (df['etalow'] == etalow) & (df['etahigh'] == etahigh) &
                       (df['philow'] == philow) & (df['phihigh'] == phihigh)]
            if subdf.empty: continue

            pt_centers = (subdf['ptlow'] + subdf['pthigh']) / 2
            pt_errors = (subdf['pthigh'] - subdf['ptlow']) / 2
            sf = subdf['sf']
            sf_err = [sf - subdf['sfdown'], subdf['sfup'] - sf]

            ax.errorbar(pt_centers, sf, xerr=pt_errors, yerr=sf_err,
                        fmt='o', capsize=3, color=props["color"], label=props["name"])

            y_low = (sf - sf_err[0]).min()
            y_high = (sf + sf_err[1]).max()
            dev = max(abs(y_low - 1), abs(y_high - 1))
            ymax_dev = max(ymax_dev, dev)

        if not ax.lines:
            plt.close(fig)
            continue

        ### x-axis
        ax.set_xscale('symlog', linthresh=50)
        ax.set_xlim(5, 1600)
        xticks = [10, 20, 35, 50, 100, 200, 1500]
        ax.set_xticks(xticks)
        ax.get_xaxis().set_major_formatter(plt.ScalarFormatter())
        ax.set_xlabel("pT [GeV]", fontsize=12)

        ### y-axis
        ax.set_ylim(1 - ymax_dev * 1.7, 1 + ymax_dev * 1.7)
        ax.set_ylabel(correction_name.replace('_', ' '), fontsize=12)
        ax.yaxis.set_major_formatter(plt.FormatStrFormatter('%.3f'))
        ax.legend(fontsize=10, frameon=False, ncol=2)
        ax.tick_params(axis='both', which='both', top=True, right=True)

        ax.text(0.03, 0.89, 'CMS', transform=ax.transAxes, fontsize=22, fontweight='bold', family='sans-serif')
        ax.text(0.97, 0.09, fr"${etalow: >5.1f} < \eta < {etahigh: >5.1f}$", transform=ax.transAxes,
                fontsize=10, family='sans-serif', ha='right')
        ax.text(0.97, 0.03, fr"${philow: >6.2f} < \phi < {phihigh: >6.2f}$", transform=ax.transAxes,
                fontsize=10, family='sans-serif', ha='right')
        
        fname = f"{prefix}_{run}_{correction_name}_phi{philow:.1f}-{phihigh:.1f}_eta{etalow:.1f}-{etahigh:.1f}.png"
        fullname = os.path.join(outdir, fname)
        plt.tight_layout()
        plt.savefig(fullname, dpi=150)
        print(f'Created: {fullname}')
        plt.close(fig)
        count += 1

    print(f'Created {count} images.\n')

def main():

    campaign_dict_run2 = {
        "2016preVFP_UL":  {"name": "2016-preVFP",  "color": "xkcd:royal blue"},
        "2016postVFP_UL": {"name": "2016-postVFP", "color": "xkcd:deep sky blue"},
        "2017_UL":        {"name": "2017",         "color": "xkcd:medium green"},
        "2018_UL":        {"name": "2018",         "color": "xkcd:tomato"},
    }

    campaign_dict_run3 = {
        "Run3Summer22":     {"name": "2022",     "color": "xkcd:purple"},
        "Run3Summer22EE":   {"name": "2022EE",   "color": "xkcd:magenta"},
        "Run3Summer23":     {"name": "2023",     "color": "xkcd:burnt orange"},
        "Run3Summer23BPix": {"name": "2023BPix", "color": "xkcd:teal"},
    }
    
    infile = '../../AnalysisScripts/Corrections/SFfromPOG/electron_id_sf.txt'
    df = load_df(infile)
    make_electronSF_plot(df, "Electron_ID_SF", campaign_dict_run2, "Run2")
    make_electronSF_plot(df, "Electron_ID_SF", campaign_dict_run3, "Run3")

if __name__ == "__main__": main()
