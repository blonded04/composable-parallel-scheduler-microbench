# Fast work distribution for composable task scheduling engines

## Install dependencies
Using conda:
```bash
conda env create -f environment.yml
conda activate benchmarks
```

## Build & Run
```bash
make clean
make release_scheduling
make release_benchmarks
make bench # runs benchmarks and saves results to ./bench_results
```

TODO: python dependencies
```bash
python3 benchplot.py # plots benchmark results and saves images to ./bench_results/images
```
