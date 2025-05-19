# Estimation of limits using CMS-combine
Limits on BSM models are calculated using the [CMS Combine](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/latest/) tool. This framework performs statistical inference based on likelihood functions. This setup uses the **Asymptotic** method to estimate 95% confidence level (CL) limits on the signal cross section.

The tools are organized as follows.
```
LimitCalculation/
├── lxplus_tools
│   ├── find_limits_allJobs.py   → Put this in lxplus working directory.
│   └── find_limits_oneJob.py    → Put this in lxplus working directory.
├── prepare_datacards.py         → Prepares datacards based on yields in SR
├── lxplus_pull_limits.py        → Uploads all datacards into lxplus.
├── lxplus_push_datacards.py     → Uploads all limts (r-values) from lxplus.
├── convertToSigmaB.py           → Converts r-values into sigma-B.
├── makeBrazilianPlots.C         → Plotmaker tool
├── CMSdecorations.h             → Plotmaker tool
└── runOnAllLimits.py            → Plotmaker tool
```
### Datacard preparation
Datacards are prepared for each signal mass point. Each datacard includes multiple bins from a discriminating variable. For each bin, the following inputs are provided:
-   Signal yield ($S$)
-   Observed events ($D$)
-   Background prediction ($B$)
-   Error in background prediction ($\delta B$)
-   $S/\sqrt{B}$  (used for sorting the bins)
-   $1 + \delta B / B$

These numbers are produced as text files by [`writeYields.C`](../StackedPlotMaker/writeYields.C) in the following format.
```
bin-index >> nsig >> nobs >> nexp >> nexperr >> s-over-rootB >> 1+experr/nexp
```
More information on datacard preparation is available [here](https://cms-analysis.github.io/HiggsAnalysis-CombinedLimit/latest/part2/settinguptheanalysis/).

![Work pending](https://img.shields.io/badge/Work%20pending-red)  
Working on including per-bin systematic uncertainties in datacard preparation.

### Running the CMS Combine tool in lxplus
The Combine tool is set up in an lxplus working area as follows.
```
cmsrel CMSSW_14_1_0_pre4
cd CMSSW_14_1_0_pre4/src
cmsenv
git clone https://github.com/cms-analysis/HiggsAnalysis-CombinedLimit.git HiggsAnalysis/CombinedLimit
cd HiggsAnalysis/CombinedLimit
```
The datacards are transferred into lxplus working are, into a directory named `datacards` using `lxplus_push_datacards.py`.  By design, `find_limits_allJobs.py` automatically picks up the datacards from here, calculates r-values using the asymptotic method, and writes them down in text files. These are brought back locally using `lxplus_pull_limits.py` for making Brazilian limit plots.

### Key Points
-  The CMS Combine tool performs best when the expected r-value is close to 1, due to the approximations underlying the asymptotic method.
- To achieve this, signal strengths ($\sigma B$) for some mass points are **artificially scaled** when preparing the datacards, ensuring reasonable r-values from Combine.
- When converting the resulting r-values into $\sigma B$ limits, the **same scaling factor is applied** to maintain consistency.