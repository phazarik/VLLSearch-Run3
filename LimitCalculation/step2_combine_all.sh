#!/usr/bin/env bash

## Clean existing combined shapes
rm -rf "shapes/*combined"
rm -rf "shapes/FullDataset*"
rm -rf "shapes/Run2*"
rm -rf "shapes/Run3*"

## Start global timer
start_time=$(date +%s)

## Run all combine_shapes.py commands
echo -e "\n\033[1;33mCombining by channels ...\033[0m\n"
python3 step2_combine_shapes.py --by channels

echo -e "\n\033[1;33mCombining by campaigns ...\033[0m\n"
python3 step2_combine_shapes.py --by campaigns

echo -e "\n\033[1;33mCombining by Run2 (all channels) ...\033[0m\n"
python3 step2_combine_shapes.py --by run2

echo -e "\n\033[1;33mCombining by Run3 (all channels) ...\033[0m\n"
python3 step2_combine_shapes.py --by run3

## New: Run2 combinations by individual channels
for ch in mm ee me em; do
    echo -e "\n\033[1;33mCombining Run2 for channel: $ch ...\033[0m\n"
    python3 step2_combine_shapes.py --by run2_$ch
done

## New: Run3 combinations by individual channels
for ch in mm ee me em; do
    echo -e "\n\033[1;33mCombining Run3 for channel: $ch ...\033[0m\n"
    python3 step2_combine_shapes.py --by run3_$ch
done

echo -e "\n\033[1;33mCombining by FullDataset ...\033[0m\n"
python3 step2_combine_shapes.py --by all

## Calculate and display total elapsed time
end_time=$(date +%s)
elapsed=$((end_time - start_time))
printf "\nTotal time for the shell script: \033[1;33m%02d:%02d:%02d\033[0m\n" $((elapsed/3600)) $(((elapsed%3600)/60)) $((elapsed%60))
