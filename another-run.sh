#!/usr/bin/bash

conda activate benchmarks

rm -rf raw_results
rm -rf bench_results
OMP_NUM_THREADS=$1 numactl --cpunodebind 0 make bench
cp -r raw_results raw_results_base
cp -r bench_results bench_results_base

rm -rf raw_results
rm -rf bench_results
OMP_NUM_THREADS=$1 numactl --cpunodebind 0 make bench_tf
cp -r raw_results raw_results_tf
cp -r bench_results bench_results_tf

rm -rf raw_results
rm -rf bench_results
OMP_NUM_THREADS=$1 numactl --cpunodebind 0 make bench_lb4omp
cp -r raw_results raw_results_lb4omp
cp -r bench_results bench_results_lb4omp

rm -rf raw_results
rm -rf microresults
mkdir microresults
cp -r raw_results* microresults/

conda deactivate
