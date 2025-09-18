import os, sys
import ROOT
from rich.console import Console
console = Console(highlight=False)
print = console.print

# ------------------- config ---------------------
basedir = os.path.abspath("shapes")   # location of all ROOT shape files
dump    = os.path.abspath("datacards")

campaigns = [
    "2016preVFP_UL", "2016postVFP_UL", "2017_UL", "2018_UL",
    "Run3Summer22", "Run3Summer22EE", "Run3Summer23", "Run3Summer23BPix",
    "Run2", "Run3", "FullDataset"
]
channels  = ["mm", "me", "em", "ee", "combined"]

sigdict = {
    "VLLD": {
        "ele": [100, 200, 300, 400, 600, 800, 1000],
        "mu": [100, 200, 300, 400, 600, 800, 1000],
        "tau": []
    },
    "VLLS":{
    }
}

backgrounds = ["TTX", "VV", "WWss", "VVV", "MultiTop", "DY", "ST", "WJetsGamma", "Higgs", "ZGamma", "QCD"]
data        = "Data"
variable    = "LTplusMET"

#-------------------------------------------------

def open_root_file(fname):
    print(f"  -> Opening ROOT file: [blue]{fname}[/blue]")
    if not os.path.exists(fname):
        print(f"  [red]File does not exist:[/red] {fname}")
        return None
    rfile = ROOT.TFile.Open(fname)
    if not rfile or rfile.IsZombie():
        print(f"  [red]Error opening ROOT file:[/red] {fname}")
        return None
    return rfile

def fetch_histograms(rfile, variable, campaign, channel, signal, backgrounds, data):
    processlist = [signal] + backgrounds + [data]
    histodict = {}
    print(f"  -> Fetching histograms with prefix '{variable}'")
    for proc in processlist:
        keyname = f"{variable}_{campaign}{channel}_{proc.replace('_','')}" ## remove any extra '_' histnames
        h = rfile.Get(keyname)
        if not h:
            print(f"  [red]Missing histogram:[/red] {keyname}")
            return None
        print(f"     found histogram: {keyname} (integral={h.Integral():.2f})")
        histodict[proc] = h
    return histodict

MIN_RATE = 1e-6  # combine-safe floor

def safe_integral(h):
    val = h.Integral()
    return val if val > 0 else MIN_RATE

def write_datacard(outdir, rfile, channel, campaign, variable, signal, backgrounds, data, histodict):
    os.makedirs(outdir, exist_ok=True)
    dcfilename = os.path.join(outdir, f"datacard_{signal}_{campaign}_{channel}_{variable}.txt")
    rfilename  = os.path.abspath(rfile.GetName())
    bundles    = [signal] + backgrounds

    print(f"  -> Writing datacard: [yellow]{dcfilename}[/yellow]")

    with open(dcfilename, "w") as dc:
        dc.write("imax * number of channels\n")
        dc.write("jmax * number of backgrounds\n")
        dc.write("kmax * number of nuisance parameters (sources of systematical uncertainties)\n\n")
        dc.write(f"shapes * * {os.path.basename(rfilename)} $VARIABLE_$CHANNEL_$PROCESS $VARIABLE_$CHANNEL_$PROCESS_$SYSTEMATIC\n\n")

        dc.write(f"bin          {variable}\n")
        dc.write(f"observation  {safe_integral(histodict[data]):.3f}\n\n")

        dc.write("bin      " + "  ".join([variable]*len(bundles)) + "\n")
        dc.write("process  " + "  ".join(bundles) + "\n")
        dc.write("process  " + "  ".join(str(i) for i in range(len(bundles))) + "\n")
        dc.write("rate     " + "  ".join(f"{safe_integral(histodict[p]):.3f}" for p in bundles) + "\n")
        dc.write("--------------------------------------------------------------------------------\n")

        dc.write("lumi lnN " + " ".join(["1.025"]*len(bundles)) + "\n\n")

        syst_map = {
            "bjet_sys": bundles,
            "dy_sys": ["DY"],
            "lep_sys": bundles,
            "pileup_sys": bundles,
            "qcd_sys": ["QCD"],
            "trig_sys": bundles,
            "ttbar_sys": ["TTX"]
        }

        for syst_base, procs in syst_map.items():
            for updown in ["up", "down"]:
                syst_name = f"{syst_base}{updown}"
                line = [syst_name, "shape"]
                for p in bundles:
                    line.append("1" if p in procs else "-")
                dc.write(" ".join(line) + "\n")

        dc.write("\n* autoMCStats 0 0 1\n")

    out_root = os.path.join(outdir, os.path.basename(rfile.GetName()))
    os.system(f"cp {rfile.GetName()} {outdir}/")
    print(f"  -> Copied Shape ROOT file to: {out_root}")

def make_datacard(campaign, channel, signal_name):
    indir     = f"{basedir}/{campaign}_{channel}"
    infile    = os.path.join(indir, f"shapes_{campaign}_{channel}_{signal_name}.root")
    rfile     = open_root_file(infile)
    if not rfile: 
        print("  [red]Skipping due to file issue[/red]")
        return
    histodict = fetch_histograms(rfile, variable, campaign, channel, signal_name, backgrounds, data)
    if not histodict: 
        print("  [red]Skipping due to missing histograms[/red]")
        return
    outdir    = f"{dump}/{campaign}_{channel}"
    write_datacard(outdir, rfile, channel, campaign, variable, signal_name, backgrounds, data, histodict)
    rfile.Close()

# ------------------- main -----------------------

def main():
    count = 0
    for camp in campaigns:
        for ch in channels:
            if (camp in ["Run2","Run3"]) and ch != "combined": 
                continue
            for sigtype, flavs in sigdict.items():
                for flav, masses in flavs.items():
                    for m in masses:
                        signal_name = f"{sigtype}_{flav}_{m}"
                        count += 1
                        print(f"\n[yellow bold][{count}] {camp}, {ch}, {signal_name}[/yellow bold]")
                        make_datacard(camp, ch, signal_name)

if __name__ == "__main__":
    main()
