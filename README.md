# Fast work distribution for composable task scheduling engines

## Install dependencies
Using conda:
```bash
conda env create -f environment.yml
conda activate benchmarks
```

## Build & Run
```bash
make bench # build, runs benchmarks and saves results to ./bench_results
```

Depenping on runtime, the approtiate way to determine max number of threads will be used.
You can limit the number of threads by setting the environment variable `BENCH_NUM_THREADS`.

Also [LB4OMP](https://github.com/unibas-dmi-hpc/LB4OMP) runtime was supported, can be executed using `make bench_lb4omp`.

## Plot results
```bash
conda activate benchmarks
python3 benchplot.py # plots benchmark results and saves images to ./bench_results/images
```
