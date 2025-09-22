#!/usr/bin/env bash

# Start global timer
start_time=$(date +%s)

# Run all combine_shapes.py commands
python3 combine_shapes.py --by channels
python3 combine_shapes.py --by campaigns
python3 combine_shapes.py --by all
python3 combine_shapes.py --by run2
python3 combine_shapes.py --by run3

# Calculate and display total elapsed time
end_time=$(date +%s)
elapsed=$((end_time - start_time))
printf "Total elapsed time: %02d:%02d:%02d\n" $((elapsed/3600)) $(( (elapsed%3600)/60 )) $((elapsed%60))
