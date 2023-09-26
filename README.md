# VLL Search in Run3 dataset
>Note : The condor submission scripts need to be customized later depending on how we wish to keep the input files. Also, right now the C script has relative paths to the header files. Change those to absolute paths before submitting condor jobs. Also, the skimmer keeps all the branches, just does not fill them. This is done to avoid compatibility issues with the header file.

### Organizational structure:
```.
├── AnaCodes
│   └── username
│       └── Analysis Codes folder
│           ├── AnaScript.C -> The main C script
│           ├── AnaScript.h -> The associated header file
│           ├── anaCond.C   -> The input-output handler (variable inputs)
│           ├── ana_test.C  -> For testing codes locally
│           └── cleanup.py  -> Cleaning up the directory (including the .so file)
│
├── CondorSetup
│   ├── createCondorJob.py          -> Submits job for only one sample (folder)
│   ├── runJobsCondor_simulation.sh ->The auto-generated shell script that runs in condor
│   ├── runana.C                    -> It picks the anaCond.C and feeds it the necessary inputs.
│   └── submitCondorJobsInBulk.py   -> Submits condor job for all samples.
│
├── InputJsons -> Contains information about input files.
└── Setup      -> Contains all the extra header files used by AnaScript.C
```

### Suggestions to contributors:
- If you are a contributor, make your own branch. I will merge them later when the analysis takes shape.
-  Keep your codes inside `./AnaCodes/username/`
- Please consult me before changing anything in the `./Setup` folder. (except for `BookHistograms()`)
- Feel free to change anything inside the `./CondorSetup` folder depending on what you want to do and how you are organizing the files.
-  Right now the codes are designed to work with nanoAOD version 9 (while developing the analysis framework), which are produced using  `CMSSW_10_3_1`. You are recommended to work in this environment to avoid compatibility issues. We will shift to a newer version later.