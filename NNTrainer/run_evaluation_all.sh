#!/usr/bin/env bash

start_time=$(date +%s)

campaigns=(
    "2016preVFP_UL"
    "2016postVFP_UL"
    "2017_UL"
    "2018_UL"
    "Run3Summer22"
    "Run3Summer22EE"
    "Run3Summer23"
    "Run3Summer23BPix"
)

for c in "${campaigns[@]}"; do
    echo "=== Running campaign: $c ==="
    python3 Evaluate.py --campaign "$c"
    echo
done

end_time=$(date +%s)
elapsed=$((end_time - start_time))

printf "\nTotal time: %02d:%02d:%02d (hh:mm:ss)\n" \
    $((elapsed/3600)) $(( (elapsed%3600)/60 )) $((elapsed%60))
