#!/usr/bin/env bash

# Start global timer
start_time=$(date +%s)

# Run all combine_hists.py commands
python3 combine_hists.py --by channels
python3 combine_hists.py --by campaigns
python3 combine_hists.py --by run2
python3 combine_hists.py --by run3
python3 combine_hists.py --by all

# Calculate and display total elapsed time
end_time=$(date +%s)
elapsed=$((end_time - start_time))
printf "Total time for the shell script: %02d:%02d:%02d\n" $((elapsed/3600)) $(( (elapsed%3600)/60 )) $((elapsed%60))
