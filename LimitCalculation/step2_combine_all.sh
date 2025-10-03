#!/usr/bin/env bash

## Start global timer
start_time=$(date +%s)

## Run all combine_shapes.py commands
echo -e "\n\033[1;33mCombining by channels ...\033[0m\n"
python3 step2_combine_shapes.py --by channels

echo -e "\n\033[1;33mCombining by campaigns ...\033[0m\n"
python3 step2_combine_shapes.py --by campaigns

echo -e "\n\033[1;33mCombining by run2 ...\033[0m\n"
python3 step2_combine_shapes.py --by run2

echo -e "\n\033[1;33mCombining by run3 ...\033[0m\n"
python3 step2_combine_shapes.py --by run3

echo -e "\n\033[1;33mCombining by all ...\033[0m\n"
python3 step2_combine_shapes.py --by all

## Calculate and display total elapsed time
end_time=$(date +%s)
elapsed=$((end_time - start_time))
printf "Total time for the shell script: \033[1;33m%02d:%02d:%02d\033[0m\n" $((elapsed/3600)) $(( (elapsed%3600)/60 )) $((elapsed%60))
