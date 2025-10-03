# CMS-Combine workflow

## Setting up CMS-combine
`Combine` is part of the CMS software ecosystem and requires a CMSSW environment. On `lxplus9`, follow these steps:
```bash
cmsrel CMSSW_14_1_0_pre4
cd CMSSW_14_1_0_pre4/src
cmsenv
git -c advice.detachedHead=false clone --depth 1 --branch v10.2.1 https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
cd HiggsAnalysis/CombinedLimit
scramv1 b clean; scramv1 b # always make a clean build
```

## Dummy example with only one bin

Consider a single bin (`bin5`) with the following yields:
```
bin 5: data = 72 (dummy data)
VV = 23.356 ± 0.848
ttbar+x = 29.191 ± 1.006
WWss = 8.485 ± 0.292
multi-top = 3.122 ± 0.102
VVV = 2.837 ± 0.368
Single t = 2.493 ± 0.687
W+jets/γ = 1.590 ± 0.530
Higgs = 0.794 ± 0.037
QCD = 0.133 ± 0.133
Zγ = 0.096 ± 0.096
DY = 0.000 ± 0.000
Signal = 59.457
```

First, write down some bin numbers/process names and IDs with rates.
```
imax 1 # number of channels
jmax 3 # number of backgrounds
kmax 5 # number of nuisance parameters (sources of systematic uncertainty)

bin bin5
observation 72

bin          bin5    bin5    bin5   bin5   bin5   bin5   bin5   bin5   bin5   bin5   bin5   bin5
process      sig     VV      ttX    WWss   multitop VVV   singlet Wjets Higgs  QCD   Zgamma DY
process      0       1       2      3      4       5     6       7     8      9     10     11
rate         59.457  23.356  29.191 8.485  3.122   2.837 2.493   1.590 0.794  0.133 0.096  0.000
```
-   `bin` indicates which histogram bin this row corresponds to.
-   `process` lists the physics processes.
-   `process ID` is required by `Combine` for bookkeeping.
-   `rate` is the expected yield of that process in the bin.

Each process has a statistical uncertainty implemented as a **log-normal (`lnN`) nuisance**:
```
stat_bin5_VV       lnN   -   1.036  -   -   -   -   -   -   -   -   -   -
stat_bin5_ttX      lnN   -   -   1.035 -   -   -   -   -   -   -   -   -
stat_bin5_WWss     lnN   -   -   -   1.034 -   -   -   -   -   -   -   -
stat_bin5_multitop lnN   -   -   -   -   1.033 -   -   -   -   -   -   -
stat_bin5_VVV      lnN   -   -   -   -   -   1.130 -   -   -   -   -   -
stat_bin5_singlet  lnN   -   -   -   -   -   -   1.276 -   -   -   -   -
stat_bin5_Wjets    lnN   -   -   -   -   -   -   -   1.333 -   -   -   -
stat_bin5_Higgs    lnN   -   -   -   -   -   -   -   -   1.047 -   -   -
stat_bin5_QCD      lnN   -   -   -   -   -   -   -   -   -   2.000 -   -
stat_bin5_Zgamma   lnN   -   -   -   -   -   -   -   -   -   -   2.000 -
```
-   The name `stat_bin5_<process>` identifies the uncertainty.
-   `lnN` specifies it’s log-normal.
-   A value >1 represents a fractional uncertainty; e.g., `1.036` corresponds to ±3.6%.

Other sources of uncertainty can be included, such as lepton efficiency or trigger efficiency as follows:
```
lep_bin5   lnN   1.017296/1.017061   1.017296/1.017061  ... (same for all MC)
trig_bin5  lnN   1.000069/1.000069   1.000069/1.000069  ... (same for all MC)
```
At the end, add the global nuisance:
```
lumi       lnN   1.025 1.025 1.025 1.025 1.025 1.025 1.025 1.025 1.025 1.025 1.025 1.025
```


## Building a datacard for shape analysis

For more complex analyses, we fit **binned distributions** rather than a single count.

As with the one-bin card, the total nominal rate of a given process must be specified in the `rate` line of the datacard. This should agree with the value returned by `TH1::Integral`. However, we can also put a value of `-1` and the Integral value will be substituted automatically.

Histogram naming scheme in the shape files:
```bash
{VARIABLE}_{channel}_{process}_{syst,if any} # For MC
{VARIABLE}_{channel}_data_obs # For Data
```

Example keys in the ROOT file:
```cpp
  KEY: TH1F     LTplusMET_sr18mm_data_obs;1     data_obs
  KEY: TH1F     LTplusMET_sr18mm_VV;1   VV
  KEY: TH1F     LTplusMET_sr18mm_VV_lepUp;1     VV
  KEY: TH1F     LTplusMET_sr18mm_VV_lepDown;1   VV
...
  KEY: TH1F     LTplusMET_sr18mm_VV_jerUp;1     VV
  KEY: TH1F     LTplusMET_sr18mm_VV_jerDown;1   VV
...
```
The datacard looks like the following.
```
imax 1
jmax 11
kmax *

shapes * * shapes_2018_UL_mm_VLLD_mu_100.root LTplusMET_$CHANNEL_$PROCESS LTplusMET_$CHANNEL_$PROCESS_$SYSTEMATIC

bin      sr18mm
observation  190 #(dummy data)

bin      sr18mm  sr18mm  sr18mm  sr18mm  sr18mm  sr18mm  sr18mm  sr18mm  sr18mm  sr18mm  sr18mm  sr18mm
process  VLLDmu100  DY  Higgs  MultiTop  QCD  ST  TTX  VV  VVV  WJetsGamma  WWss  ZGamma
process  0  1  2  3  4  5  6  7  8  9  10  11
rate     152.169  0.000  1.779  8.650  0.133  4.879  66.279  68.112  8.330  3.551  28.353  0.096
--------------------------------------------------------------------------------
lep shape - 1 1 1 1 1 1 1 1 1 1 1
trig shape - 1 1 1 1 1 1 1 1 1 1 1
bjet shape - 1 1 1 1 1 1 1 1 1 1 1
pileup shape - 1 1 1 1 1 1 1 1 1 1 1
dy shape - 1 - - - - - - - - - -
qcd shape - - - - 1 - - - - - - -
ttbar shape - - - - - - 1 - - - - -

* autoMCStats 0 0 1
```
-   `shapes` lines map datacard entries to ROOT histogram names.    
-   Each nuisance source can be applied per bin and per process.

Once datacards and shapes are ready,
```bash
text2workspace.py shapes/2018_UL_mm/datacard_VLLDmu300_LTplusMET.txt -o workspace_mytest.root
combine -M AsymptoticLimits workspace_mytest.root
```

or more directly,
```bash
combine -M AsymptoticLimits shapes/2018_UL_mm/datacard_2018_UL_mm_VLLDmu300_LTplusMET.txt   --setParameters r=10 --freezeParameters r
```
-   `-M AsymptoticLimits` tells Combine to compute expected and observed limits using asymptotic formulas.
-   `--setParameters` and `--freezeParameters` allow fixing a parameter (e.g., signal strength `r`) during the fit.