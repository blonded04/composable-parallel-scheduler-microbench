#!/usr/bin/bash

conda env create -f environment.yml
conda activate benchmarks

bash another-run.sh $1
