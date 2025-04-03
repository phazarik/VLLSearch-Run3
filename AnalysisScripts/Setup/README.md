## Setup
### Object selections
| Selection type | Muon | Electron | Jet | 
|--|--|--|--|
|pT, abs(eta)     | >10, <2.4 | >10, <2.4 | >30, <2.4 |
| prompt (dxy, dz)| 0.05, 0.1 | 0.05 (0.1), 0.1 (0.2) in barrel (endcap) | -- |
| ID              | `Muon_mediumId` | `Electron_cutBased`>2 (medium) | `Jet_jetId` >= 1 (2 in 2016) |
| Isolation       | `Muon_pfRelIso03_all`<0.15 | `Electron_pfRelIso03_all`<0.15 | -- |
|Cleaning         |--- |Cleaned from loose-mu (0.4)| Cleaned from loose-L (0.4)

### b-tagging thresholds
|Campaign|b-tagger| Threshold|
|--|--|--|
|2016preVFP_UL |`Jet_btagDeepFlavB`|0.2783|
|2016postVFP_UL|`Jet_btagDeepFlavB`|0.3040|
|2017_UL       |`Jet_btagDeepFlavB`|0.2598|
|2018_UL       |`Jet_btagDeepFlavB`|0.2489|

***Source: https://btv-wiki.docs.cern.ch/ScaleFactors/***

### HLT paths used in different campaigns
|Campaign|Electron trigger|Muon trigger|Offline pT cuts (e, mu)|
|--|--|--|--|
|2016preVFP_UL |`HLT_Ele27_WPTight_Gsf`|`HLT_IsoMu24`|30, 26|
|2016postVFP_UL|`HLT_Ele27_WPTight_Gsf`|`HLT_IsoMu24`|30, 26|
|2017_UL       |`HLT_Ele35_WPTight_Gsf`|`HLT_IsoMu27`|37, 29|
|2018_UL       |`HLT_Ele32_WPTight_Gsf`|`HLT_IsoMu24`|35, 26|

The trigger efficiencies are calculated by [Kumar Yash](https://github.com/ykumar05) using tag-and-probe methods, and the details can be found in [GitHub:ykumar05/TriggerEfficiency](https://github.com/ykumar05/TriggerEfficiency). The corrections are expressed as a function of pT of the triggering object, which is extracted from a fit.

### Baseline event selections
- Exactly two L with same sign (L = electron, muon)
- At least one lepton passes the offline trigger threshold.
- Dilepton invariant mass > 15 GeV
- In case of ee events, Z window (76-106 GeV) is vetoed.

### MC corrections
The correction factors are provided by POGs. The correctionlib library is used to read the files and dump them into text files using a [correctionlib-to-text](https://github.com/phazarik/correctionlib-to-text) tool that I made.

***Source: [GitLab: cms-nanoAOD/jsonPOG-integration](https://gitlab.cern.ch/cms-nanoAOD/jsonpog-integration)*** 


## Important references
- Documentation of the JSON files containing the corrections: https://cms-nanoaod-integration.web.cern.ch/commonJSONSFs/
- Instructions on how to correct Jet 4-vectors: https://twiki.cern.ch/twiki/bin/view/CMS/JetEnergyScale
- b-tagging correction methods: https://twiki.cern.ch/twiki/bin/view/CMS/BTagSFMethods

### Relevant TWiki pages
-   [Egamma POG in Twiki](https://twiki.cern.ch/twiki/bin/view/CMS/EgammaPOG)
-   [Jets/Missing Et in Twiki](https://twiki.cern.ch/twiki/bin/view/CMS/JetMET)
-   [Muon POG in Twiki](https://twiki.cern.ch/twiki/bin/view/CMS/MuonPOG)
-   [b tag & vertexing POG in Twiki](https://twiki.cern.ch/twiki/bin/view/CMS/BtagPOG)
-   [Luminosity POG in Twiki](https://twiki.cern.ch/twiki/bin/view/CMS/TWikiLUM)

