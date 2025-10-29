#!/usr/bin/env bash
rm -rf limits_rvalue/*
python3 step2_runCombine_once.py --campaign FullDataset --channel combined --fs 2LSS
python3 step2_runCombine_once.py --campaign FullDataset --channel combined --fs 2LOS
python3 step2_runCombine_once.py --campaign FullDataset --channel combined --fs 2L
