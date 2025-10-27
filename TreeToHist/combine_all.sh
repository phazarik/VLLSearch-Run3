#!/usr/bin/env bash

start_time=$(date +%s)

# List of --by options to run
by_list=(
  channels  ## Outputs = 8. Combines all channels for each campaigns.
  campaigns ## Outputs = nchannel. Combined the full dataset separately for each channel.
  run2      ## Outputs = 1. Combines everything for Run-2.
  run3      ## Outputs = 1. Combines everything for Run-3.
  all       ## Outputs = 1. Combines everything.
  run2_ee   ## Outputs = 1. Combines ee channels for the 4 Run-2 campaigns.
  run2_mm   ## Same for the rest. Mention the channel after the underscore.
  run3_ee
  run3_mm
)

# Loop over all options
for by in "${by_list[@]}"; do
    python3 combine_hists.py --by "$by" #--dryrun
done

end_time=$(date +%s)
elapsed=$((end_time - start_time))
printf "Total time: %02d:%02d:%02d\n" $((elapsed/3600)) $(( (elapsed%3600)/60 )) $((elapsed%60))
