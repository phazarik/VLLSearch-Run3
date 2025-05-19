#  Search for Vector-like Leptons
This repository contains tools for searching for vector-like leptons (VLLs) using the CMS Run-2 and Run-3 datasets in NanoAOD format. The workflow involves converting NanoAOD files to custom trees with event-level information and MC corrections, training and evaluating DNNs on these trees, filtering the events, and producing histograms. These histograms are then used for stacking backgrounds and overlaying signal/data. Finally, the observed results are interpreted using tools like CMS-combine.

## 🛠️ Prerequisites

![24.9.1](https://img.shields.io/badge/Conda-24.9.1-green)<br>
![3.10.9](https://img.shields.io/badge/Python-3.10.9-blue)<br>
![6.26/10](https://img.shields.io/badge/ROOT-6.26%2F10-yellow) (Built on: Apr 07, 2023)<br>
**[nlohmann/json](https://github.com/nlohmann/json)** (Accessed: Oct 15, 2024)

The following are some additional Python packages installed using conda-forge.

![awkward 2.7.0](https://img.shields.io/badge/awkward-2.7.0-lightgrey)  ![correctionlib 2.6.4](https://img.shields.io/badge/correctionlib-2.6.4-lightgrey)  ![ipython 8.29.0](https://img.shields.io/badge/ipython-8.29.0-lightgrey)  ![jupyterlab 4.2.5](https://img.shields.io/badge/jupyterlab-4.2.5-lightgrey)  ![keras 3.6.0](https://img.shields.io/badge/keras-3.6.0-lightgrey)  ![notebook 7.2.2](https://img.shields.io/badge/notebook-7.2.2-lightgrey)  ![numpy 1.26.4](https://img.shields.io/badge/numpy-1.26.4-lightgrey)  ![pandas 2.2.2](https://img.shields.io/badge/pandas-2.2.2-lightgrey)  ![scikit-learn 1.5.2](https://img.shields.io/badge/scikit--learn-1.5.2-lightgrey)  ![scipy 1.14.1](https://img.shields.io/badge/scipy-1.14.1-lightgrey)  ![seaborn 0.13.2](https://img.shields.io/badge/seaborn-0.13.2-lightgrey)  ![tensorflow 2.17.0](https://img.shields.io/badge/tensorflow-2.17.0-lightgrey)  ![uproot 5.4.1](https://img.shields.io/badge/uproot-5.4.1-lightgrey)


## ▶️ How to Run

### Preparing nanoAOD files
This setup turns nanoAOD files into custom trees containing event-level branches. The nanoAOD files should be organized in directories in the form `samplename/sub-samplename` so that the TreeMaker tool can sequentially run on all files. The following is how I organize my files.
```
├── DYtoLL
│   ├── 10to50/*root
│   └── 50toInf/*root
├── WJets
│   └── Inclusive/*root
└── Muon
    ├── C/*root
    └── D/*root
```
I use [this tool](https://github.com/phazarik/crabSkimSetuphttps://github.com/phazarik/MakeSelector-CRAB-setup) to skim the nanoAOD files from DAS and bring only the fraction of events that I want to optimize storage space and computational bandwidth.

### Making flattened trees from nanoAOD (MakeSelector-based)
The basic MakeSelector-based setup is organized as follows:
```
├── AnalysisScripts
│   ├── Corrections        → Manages pre-defined MC corrections for physics objects.
│   ├── TreeMaker          → Source code for NanoAOD to custom tree maker (MakeSelector). 
|   ├── HistMaker          → Source code for NanoAOD to histogram maker (MakeSelector).
│   ├── Setup              → Contains object selections and utility functions.
│   ├── compile_and_run.C  → Driver script that runs TreeMaker/HistMaker.
│   └── includeHeaders.h   → Connects all the necessary header files.
├── LumiJsons              → Sample/subsample information are kept here.
└── processAllSamples.py   → Runs the MakeSelector based setup for all samples sequentially.
```
The `MakeSelector` classes are located in the `TreeMaker` and `HistMaker` directories, which both rely on common header files stored in other directories. This setup uses relative paths for all input files and headers, making it portable across different systems.

To compile and run the setup for testing purposes:
1. Put desired parameters in `compile_and_run.C`.
2. Go to ROOT prompt.
3. Run `compile_and_run.C`.
```cpp
[].x compile_and_run.C
```
For running over all the samples, run `processAllSamples.py` as follows.
```bash
python3 processAllSamples.py --jobname testjob --dryrun --test
```
For this analysis, the `TreeMaker` setup is used to produce flattened trees containing event-level variables as branches, along with additional branches carrying event-level MC corrections (such as lepton scale factors, trigger scale factors, etc.). Once the baseline events from NanoAOD are converted into these flattened trees, a Python-based setup is used for further analysis. This approach is convenient for working with neural networks (TensorFlow, NumPy, Pandas) due to the flattened, dataframe-like structure of the event-level information.

The output trees are written by `processAllSamples.py` in the following structure.
```
└── ROOT_FILES
    └── trees
        └── testjob
            ├── tree_sample1_subsample1.root
            └── tree_sample2_subsample2.root
```

>**Note :** NanoAODv11- and NanoAODv12+ have different branch structure and types. Some type definitions and branch names in the MakeSelector class need to be added/commented-out before running the codes. It's always recommended to run the setup for one file before submitting sequential jobs for all.

### Python based analysis (event filtering and DNN)
Once ROOT files are produced containing the custom tree (baseline events), the following setup is used for further analysis.
```
├── LumiJsons                       → Required for luminosity scaling
└── TreeToHist
    ├── corrections                 → Contains MC corrections in JSON files.
    ├── extractHistsFromTrees.C     → Converts tree-files into hist-files.
    ├── eventProcessor.h            → The event loop is here.
    ├── setBranchesAndHistograms.h  → Managing branches and creating histograms.
    ├── runForAllJobs.py            → Produces histograms from trees in bulk.  
    └── filterTrees.py              → Creates a copy of the tree files after putting event selections. 
```
On top of the baseline events, additional event selections are applied using `filterTrees.py`. The events passing these selections are written back to disk. This process is iterative, resulting in multiple sets of ROOT files with different event selections, such as baseline, control regions, validation regions, and signal regions.
```
└── NNTrainer
    ├── Train.ipynb     → Training and testing is done using flattened trees.
    ├── Evaluate.py     → Writes a copy of the input trees after adding DNN branches.
    └── trained_models  → DNN models, scaling parameters etc. are organised here.
                          (these are automatically picked up by Evaluate.py)
```
DNN models are trained on these flattened trees, with the Keras models, input-output information, and scaling parameters saved on disk. The `filterTrees.py` script is used to apply further event selections, and `extractHistsFromTrees.C` is employed to produce histograms.
- **Luminosity Scaling**: Applied as a weight to each histogram. The luminosity of each sample is read from the JSON files located in the `LumiJsons` directory.
- **Event Weights**: Additional branches in the input trees hold event weights, which are applied to each histogram.
- **MC Sample Corrections**: Corrections specific to certain MC samples are extracted from the JSON files kept in `TreeToHist/corrections` and applied as event weights for those samples.
- Histograms are produced with luminosity scaling and all necessary corrections. No further scaling is required downstream in the pipeline. These histograms are directly used for stacking and overlaying.

### Plot-maker
Once histograms with corrections and scaling applied are produced, the following plot-maker setup is used to stack background histograms, overlay signal and data, and generate the final figures.
```
├── LumiJsons               → Required for finding samples/subsamples.
└── StackedPlotMaker
    ├── makeStackedPlot.C   → Main ROOT macro.
    ├── utilities           → File management, decoration and other utilities.
    ├── include_headers.h   → Connects all the header files.
    ├── runOnAllPlots.py    → Driver script for producing images in bulk.
    └── writeYields.C       → Writing out yields for CMS-combine datacard preparation. (Limit calculation)
```

### Limits
The procedure for computing limits is described in detail in the [LimitCalculation](LimitCalculation/) section. Limits are computed at 95% using the Asymptotic approximation from CMS Combine.

## 🔗 Relevant Links & Tools
- Sample database:  
  [Google Spreadsheet](https://docs.google.com/spreadsheets/d/10LLDj0NRvGVtQPhoFLRR2A7NRblJRvnX_U7wvfeYhvw/edit?pli=1&gid=0#gid=0)
- Skimming NanoAOD files from DAS to T2/T3 area:  
  [phazarik/MakeSelector-CRAB-setup](https://github.com/phazarik/MakeSelector-CRAB-setup)

- Simpler analysis template for NanoAOD files:  
  [phazarik/nanoAOD_analyzer](https://github.com/phazarik/nanoAOD_analyzer)

- Extracting POG scale factors in text format:  
  [phazarik/correctionlib-to-text](https://github.com/phazarik/correctionlib-to-text)