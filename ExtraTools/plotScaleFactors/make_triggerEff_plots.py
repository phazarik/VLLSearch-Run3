import os, sys
import numpy as np
import matplotlib.pyplot as plt
from math import erf, fabs

### Trigger dictionaries in the format: function, HLT trigger-path, HLT threshold, offline threshold 

### 1) For Muons:
muon_trigger = {
    "2016preVFP_UL": {
        "campaign": "2016-preVFP",
        "trigger": "HLT_IsoMu24",
        "trigger_threshold": 24,
        "offline_threshold": 26,
        "fit_nominal": {
            "barrel": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.952501 * (1.0 + erf((pt - 23.9491) / (2.0 * 0.405232))),
            "endcap": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else (0.5 * 0.951936 * (1.0 + erf((pt - 23.9399) / (2.0 * 0.533316))) if pt <= 100 else 0.774)
        }
    },
    "2016postVFP_UL": {
        "campaign": "2016-postVFP",
        "trigger": "HLT_IsoMu24",
        "trigger_threshold": 24,
        "offline_threshold": 26,
        "fit_nominal": {
            "barrel": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.963135 * (1.0 + erf((pt - 23.9546) / (2.0 * 0.363316))),
            "endcap": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.96043 * (1.0 + erf((pt - 23.9677) / (2.0 * 0.463984)))
        }
    },
    "2017_UL": {
        "campaign": "2017",
        "trigger": "HLT_IsoMu27",
        "trigger_threshold": 27,
        "offline_threshold": 29,
        "fit_nominal": {
            "barrel": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else (0.5 * 0.929318 * (1.0 + erf((pt - 26.9424) / (2.0 * 0.612448))) if pt <= 100 else 0.958305),
            "endcap": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.94163 * (1.0 + erf((pt - 26.9092) / (2.0 * 0.844543)))
        }
    },
    "2018_UL": {
        "campaign": "2018",
        "trigger": "HLT_IsoMu24",
        "trigger_threshold": 24,
        "offline_threshold": 26,
        "fit_nominal": {
            "barrel": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.950463 * (1.0 + erf((pt - 23.9593) / (2.0 * 0.375996))),
            "endcap": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.953162 * (1.0 + erf((pt - 23.9459) / (2.0 * 0.457351)))
        }
    },
    "Run3Summer22": {
        "campaign": "2022-preEE",
        "trigger": "HLT_IsoMu24",
        "trigger_threshold": 24,
        "offline_threshold": 26,
        "fit_nominal": {
            "barrel": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.955504 * (1.0 + erf((pt - 24.7165) / (2.0 * 0.371029))),
            "endcap": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.931413 * (1.0 + erf((pt - 23.4266) / (2.0 * 0.200128)))
        }
    },
    "Run3Summer22EE": {
        "campaign": "2022-postEE",
        "trigger": "HLT_IsoMu24",
        "trigger_threshold": 24,
        "offline_threshold": 26,
        "fit_nominal": {
            "barrel": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.950136 * (1.0 + erf((pt - 24.2686) / (2.0 * 0.00194796))),
            "endcap": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.940207 * (1.0 + erf((pt - 23.8785) / (2.0 * 0.378177)))
        }
    },
    "Run3Summer23": {
        "campaign": "2023-preBPix",
        "trigger": "HLT_IsoMu24",
        "trigger_threshold": 24,
        "offline_threshold": 26,
        "fit_nominal": {
            "barrel": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.952283 * (1.0 + erf((pt - 25.3956) / (2.0 * 0.167536))),
            "endcap": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.955237 * (1.0 + erf((pt - 24.1104) / (2.0 * 0.0992034)))
        }
    },
    "Run3Summer23BPix": {
        "campaign": "2023-postBPix",
        "trigger": "HLT_IsoMu24",
        "trigger_threshold": 24,
        "offline_threshold": 26,
        "fit_nominal": {
            "barrel": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.949014 * (1.0 + erf((pt - 24.9446) / (2.0 * 0.0456965))),
            "endcap": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.955304 * (1.0 + erf((pt - 25.1877) / (2.0 * 0.0328055)))
        }
    }
}

### For Electrons:
electron_trigger = {
    "2016preVFP_UL": {
        "campaign": "2016-preVFP",
        "trigger": "HLT_Ele27_WPTight_Gsf",
        "trigger_threshold": 27,
        "offline_threshold": 30,
        "fit_nominal": {
            "barrel": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.980431 * (1.0 + erf((pt - 26.6311) / (2.0 * 0.977291))),
            "endcap": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.984774 * (1.0 + erf((pt - 27.5187) / (2.0 * 1.33533)))
        }
    },
    "2016postVFP_UL": {
        "campaign": "2016-postVFP",
        "trigger": "HLT_Ele27_WPTight_Gsf",
        "trigger_threshold": 27,
        "offline_threshold": 30,
        "fit_nominal": {
            "barrel": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.975241 * (1.0 + erf((pt - 26.6834) / (2.0 * 1.08336))),
            "endcap": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.981091 * (1.0 + erf((pt - 27.7339) / (2.0 * 1.44615)))
        }
    },
    "2017_UL": {
        "campaign": "2017",
        "trigger": "HLT_Ele35_WPTight_Gsf",
        "trigger_threshold": 35,
        "offline_threshold": 37,
        "fit_nominal": {
            "barrel": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.955945 * (1.0 + erf((pt - 33.0345) / (2.0 * 0.885676))),
            "endcap": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.962208 * (1.0 + erf((pt - 33.9927) / (2.0 * 1.55814)))
        }
    },
    "2018_UL": {
        "campaign": "2018",
        "trigger": "HLT_Ele32_WPTight_Gsf",
        "trigger_threshold": 32,
        "offline_threshold": 35,
        "fit_nominal": {
            "barrel": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.962897 * (1.0 + erf((pt - 33.1188) / (2.0 * 0.844886))),
            "endcap": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.975043 * (1.0 + erf((pt - 32.9805) / (2.0 * 1.18094)))
        }
    },
    "Run3Summer22": {
        "campaign": "2022-preEE",
        "trigger": "HLT_Ele32_WPTight_Gsf",
        "trigger_threshold": 32,
        "offline_threshold": 35,
        "fit_nominal": {
            "barrel": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.976349 * (1.0 + erf((pt - 31.1233) / (2.0 * 1.68838))),
            "endcap": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.971387 * (1.0 + erf((pt - 29.2013) / (2.0 * 2.10407)))
        }
    },
    "Run3Summer22EE": {
        "campaign": "2022-postEE",
        "trigger": "HLT_Ele32_WPTight_Gsf",
        "trigger_threshold": 32,
        "offline_threshold": 35,
        "fit_nominal": {
            "barrel": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.979149 * (1.0 + erf((pt - 33.3691) / (2.0 * 1.28607))),
            "endcap": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.975062 * (1.0 + erf((pt - 32.9815) / (2.0 * 1.47289)))
        }
    },
    "Run3Summer23": {
        "campaign": "2023-preBPix",
        "trigger": "HLT_Ele32_WPTight_Gsf",
        "trigger_threshold": 32,
        "offline_threshold": 35,
        "fit_nominal": {
            "barrel": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.974765 * (1.0 + erf((pt - 33.0725) / (2.0 * 1.43905))),
            "endcap": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.975377 * (1.0 + erf((pt - 32.6495) / (2.0 * 1.69333)))
        }
    },
    "Run3Summer23BPix": {
        "campaign": "2023-postBPix",
        "trigger": "HLT_Ele32_WPTight_Gsf",
        "trigger_threshold": 32,
        "offline_threshold": 35,
        "fit_nominal": {
            "barrel": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.97538 * (1.0 + erf((pt - 33.2668) / (2.0 * 1.39703))),
            "endcap": lambda pt, eta: 0.0 if pt < 10 or fabs(eta) > 2.4 else 0.5 * 0.970987 * (1.0 + erf((pt - 32.7315) / (2.0 * 1.64687)))
        }
    }
}

def plot_efficiency(trigger, campaign, xlim, index=None):
    pt_values = np.linspace(0, 50, 1000)

    if campaign not in trigger:
        print(f"\033[31mCampaign '{campaign}' not found in trigger dictionary.\033[0m")
        return

    config = trigger[campaign]
    triggername   = config["trigger"]
    triggerline   = config["trigger_threshold"]
    offline_cut   = config["offline_threshold"]
    eff_barrel    = config["fit_nominal"]["barrel"]
    eff_endcap    = config["fit_nominal"]["endcap"]

    fig, ax = plt.subplots(figsize=(5, 4))

    barrel_eff = [eff_barrel(pt, 1.2) for pt in pt_values]
    endcap_eff = [eff_endcap(pt, 1.8) for pt in pt_values]

    ax.axvline(x=triggerline, label=triggername, color='k', linestyle='-', linewidth=1)
    ax.axvline(x=offline_cut, label='offline pT-cut', color='k', linestyle='--', linewidth=1)

    ax.plot(pt_values, barrel_eff, label=r"$|\eta| \leq 1.479$", color="red")
    ax.plot(pt_values, endcap_eff, label=r"$|\eta| > 1.479$", color="blue")

    ax.set_xlabel("pT [GeV]", fontsize=12)
    ax.set_xlim(xlim[0], xlim[1])
    ax.set_ylabel("Trigger efficiency", fontsize=12)
    ax.set_ylim(0, 1)
    ax.legend(loc='center left', fontsize=10, frameon=False)
    ax.tick_params(axis='both', which='both', top=True, right=True)

    ax.text(0.03, 0.89, 'CMS', transform=ax.transAxes, fontsize=22, fontweight='bold', family='sans-serif')
    ax.text(0.03, 0.83, config["campaign"], transform=ax.transAxes, fontsize=10, family='sans-serif')

    outdir = os.path.join('plots', 'TriggerEfficiency')
    os.makedirs(outdir, exist_ok=True)
    figname = f"{triggername}_{campaign}.png"
    if index is not None: figname = f"{index:03d}_{figname}"
    fullfigname = os.path.join(outdir, figname)

    plt.tight_layout()
    plt.savefig(fullfigname, dpi=150, bbox_inches='tight')
    print('File saved: ' + fullfigname)
        
def main():
    plot_efficiency(muon_trigger,     '2016preVFP_UL',  xlim=(15, 35), index=1)
    plot_efficiency(muon_trigger,     '2016postVFP_UL', xlim=(15, 35), index=2)
    plot_efficiency(muon_trigger,     '2017_UL',        xlim=(15, 35), index=3)
    plot_efficiency(muon_trigger,     '2018_UL',        xlim=(15, 35), index=4)
    plot_efficiency(muon_trigger,     'Run3Summer22',     xlim=(15, 35), index=5)
    plot_efficiency(muon_trigger,     'Run3Summer22EE',   xlim=(15, 35), index=6)
    plot_efficiency(muon_trigger,     'Run3Summer23',     xlim=(15, 35), index=7)
    plot_efficiency(muon_trigger,     'Run3Summer23BPix', xlim=(15, 35), index=8)
    plot_efficiency(electron_trigger, '2016preVFP_UL',  xlim=(10, 38), index=9)
    plot_efficiency(electron_trigger, '2016postVFP_UL', xlim=(10, 38), index=10)
    plot_efficiency(electron_trigger, '2017_UL',        xlim=(20, 42), index=11)
    plot_efficiency(electron_trigger, '2018_UL',        xlim=(20, 42), index=12)
    plot_efficiency(electron_trigger, 'Run3Summer22',     xlim=(12, 42), index=13)
    plot_efficiency(electron_trigger, 'Run3Summer22EE',   xlim=(12, 42), index=14)
    plot_efficiency(electron_trigger, 'Run3Summer23',     xlim=(12, 42), index=15)
    plot_efficiency(electron_trigger, 'Run3Summer23BPix', xlim=(12, 42), index=16)
    #plt.show()

if __name__ == "__main__": main()
