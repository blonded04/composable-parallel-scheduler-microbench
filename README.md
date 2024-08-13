# Hybrid work distribution for parallel programs

## Summary
In modern computing systems, the increasing number of cores in processors emphasizes the need for efficient utilization of resources. To achieve this goal, it is important to distribute the work generated by a parallel algorithm optimally among the cores.

Typically, there are two paradigms to achieve that: static and dynamic. The standard static way, used in OpenMP, is just the fork barrier that splits the whole work into the fixed number of presumably "equal" parts, e.g., split into equally-sized ranges for a parallel_for. The classic dynamic approaches are: 1) work-stealing task schedulers like in OneTBB or BOLT, or 2) work-sharing. As one can guess, the static approach has a very low overhead on the work distribution itself, but it does not work well in terms of the distribution optimality of parallel programs for general-purpose tasks, e.g., nested complex parallelism or uneven iterations of parallel_for. Here, we present a task scheduler that unites two discussed dynamic work distribution paradigms at the same time and achieves a low overhead with reasonably good task distribution.

## Install dependencies
Using conda:
```bash
conda env create -f environment.yml
conda activate benchmarks
```

## Build & Run
```bash
numactl --cpunodebind 0 make bench # build, runs benchmarks and saves results to ./raw_results
```

Depenping on runtime, the approtiate way to determine max number of threads will be used.
You can limit the number of threads by setting the environment variable `BENCH_NUM_THREADS`.

Also [LB4OMP](https://github.com/unibas-dmi-hpc/LB4OMP) runtime was supported, can be executed using `make bench_lb4omp`.
You can run it via following command:
```bash
numactl --cpunodebind 0 make bench_lb4omp
```

## Changing number of threads

To change number of threads just modify a `GetNumThreads` function body from `./include/num_threads.h` file.

## Plot results
You should modify `filtered_modes` list in `./benchplot.py` script to control which modes are about to be plotted

```bash
conda activate benchmarks
python3 benchplot.py # plots benchmark results from ./raw_results and saves images to ./bench_results
```
