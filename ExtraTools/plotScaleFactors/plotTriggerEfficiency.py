import os, sys
import numpy as np
import matplotlib.pyplot as plt
from math import erf, fabs

### Trigger dictionaries in the format: function, HLT trigger-path, HLT threshold, offline threshold 

### 1) For Muons:
muon_trigger = {
    "2016preVFP_UL": (
        lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else (
            0.5 * 0.952501 * (1.0 + erf((pt - 23.9491) / (2.0 * 0.405232))) if fabs(eta) <= 1.479
            else (0.5 * 0.951936 * (1.0 + erf((pt - 23.9399) / (2.0 * 0.533316))) if pt <= 100 else 0.774)
        ),
        "HLT_IsoMu24", 24, 26
    ),
    "2016postVFP_UL": (
        lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else (
            0.5 * 0.963135 * (1.0 + erf((pt - 23.9546) / (2.0 * 0.363316))) if fabs(eta) <= 1.479
            else 0.5 * 0.96043 * (1.0 + erf((pt - 23.9677) / (2.0 * 0.463984)))
        ),
        "HLT_IsoMu24", 24, 26
    ),
    "2017_UL": (
        lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else (
            (0.5 * 0.929318 * (1.0 + erf((pt - 26.9424) / (2.0 * 0.612448))) if pt <= 100 else 0.958305)
            if fabs(eta) <= 1.479
            else 0.5 * 0.94163 * (1.0 + erf((pt - 26.9092) / (2.0 * 0.844543)))
        ),
        "HLT_IsoMu27", 27, 29
    ),
    "2018_UL": (
        lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else (
            0.5 * 0.950463 * (1.0 + erf((pt - 23.9593) / (2.0 * 0.375996))) if fabs(eta) <= 1.479
            else 0.5 * 0.953162 * (1.0 + erf((pt - 23.9459) / (2.0 * 0.457351)))
        ),
        "HLT_IsoMu24", 24, 26
    ),
    "Run3Summer22": (
        lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else (
            0.5 * 0.955504 * (1.0 + erf((pt - 24.7165) / (2.0 * 0.371029))) if fabs(eta) <= 1.479
            else 0.5 * 0.931413 * (1.0 + erf((pt - 23.4266) / (2.0 * 0.200128)))
        ),
        "HLT_IsoMu24", 24, 26
    ),
}

### For Electrons:
electron_trigger = {
    "2016preVFP_UL": (
        lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else (
            0.5 * 0.980431 * (1.0 + erf((pt - 26.6311) / (2.0 * 0.977291))) if fabs(eta) <= 1.479
            else 0.5 * 0.984774 * (1.0 + erf((pt - 27.5187) / (2.0 * 1.33533)))
        ),
        "HLT_Ele27_WPTight_Gsf", 27, 30
    ),
    "2016postVFP_UL": (
        lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else (
            0.5 * 0.975241 * (1.0 + erf((pt - 26.6834) / (2.0 * 1.08336))) if fabs(eta) <= 1.479
            else 0.5 * 0.981091 * (1.0 + erf((pt - 27.7339) / (2.0 * 1.44615)))
        ),
        "HLT_Ele27_WPTight_Gsf", 27, 30
    ),
    "2017_UL": (
        lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else (
            0.5 * 0.955945 * (1.0 + erf((pt - 33.0345) / (2.0 * 0.885676))) if fabs(eta) <= 1.479
            else 0.5 * 0.962208 * (1.0 + erf((pt - 33.9927) / (2.0 * 1.55814)))
        ),
        "HLT_Ele35_WPTight_Gsf", 35, 37
    ),
    "2018_UL": (
        lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else (
            0.5 * 0.962897 * (1.0 + erf((pt - 33.1188) / (2.0 * 0.844886))) if fabs(eta) <= 1.479
            else 0.5 * 0.975043 * (1.0 + erf((pt - 32.9805) / (2.0 * 1.18094)))
        ),
        "HLT_Ele32_WPTight_Gsf", 32, 35
    ),
    "Run3Summer22": (
        lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else (
            0.5 * 0.976349 * (1.0 + erf((pt - 31.1233) / (2.0 * 1.68838))) if fabs(eta) <= 1.479
            else 0.5 * 0.971387 * (1.0 + erf((pt - 29.2013) / (2.0 * 2.10407)))
        ),
        "HLT_Ele32_WPTight_Gsf", 32, 35
    ),
}
def plot_efficiency(trigger, campaign, xlim):
    pt_values = np.linspace(0, 50, 1000) ### This determined the resolution

    if campaign not in trigger:
        print(f"Campaign '{campaign}' not found in trigger dictionary.")
        return

    eff_func, triggername, triggerline, offline_cut = trigger[campaign]

    fig, ax = plt.subplots(figsize=(5, 4))

    barrel_eff = [eff_func(pt, 1.2) for pt in pt_values]
    endcap_eff = [eff_func(pt, 1.8) for pt in pt_values]

    ax.axvline(x=triggerline, label=triggername, color='k', linestyle='-', linewidth=1)
    ax.axvline(x=offline_cut, label='offline pT-cut', color='k', linestyle='--', linewidth=1)

    ax.plot(pt_values, barrel_eff, label=r"$|\eta| \leq 1.479$", color="red")
    ax.plot(pt_values, endcap_eff, label=r"$|\eta| > 1.479$",    color="blue")

    ax.set_xlabel("pT [GeV]", fontsize=12)
    ax.set_xlim(xlim[0], xlim[1])
    ax.set_ylabel("Trigger efficiency", fontsize=12)
    ax.set_ylim(0, 1)
    ax.legend(loc='center left', fontsize=10, frameon=False)
    ax.tick_params(axis='both', which='both', top=True, right=True)

    ax.text(0.03, 0.89, 'CMS', transform=ax.transAxes, fontsize=22, fontweight='bold', family='sans-serif')
    ax.text(0.03, 0.83, campaign, transform=ax.transAxes, fontsize=10, family='sans-serif')

    outdir = os.path.join('plots', 'TriggerEfficiency')
    os.makedirs(outdir, exist_ok=True)
    figname = f"{triggername}_{campaign}.png"
    fullfigname = os.path.join(outdir, figname)
    plt.tight_layout()
    plt.savefig(fullfigname, dpi=150, bbox_inches='tight')
    print('File saved: ' + fullfigname)
        
def main():
    plot_efficiency(muon_trigger,     '2016postVFP_UL', xlim=(15, 35))
    plot_efficiency(muon_trigger,     '2016preVFP_UL',  xlim=(15, 35))
    plot_efficiency(muon_trigger,     '2017_UL',        xlim=(15, 35))
    plot_efficiency(muon_trigger,     '2018_UL',        xlim=(15, 35))
    plot_efficiency(muon_trigger,     'Run3Summer22',   xlim=(15, 35))
    plot_efficiency(electron_trigger, '2016postVFP_UL', xlim=(10, 38))
    plot_efficiency(electron_trigger, '2016preVFP_UL',  xlim=(10, 38))
    plot_efficiency(electron_trigger, '2017_UL',        xlim=(20, 42))
    plot_efficiency(electron_trigger, '2018_UL',        xlim=(20, 42))
    plot_efficiency(electron_trigger, 'Run3Summer22',   xlim=(12, 42))
    #plt.show()

if __name__ == "__main__": main()
