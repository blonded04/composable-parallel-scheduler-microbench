#!/bin/bash
set -euxo pipefail

benchname=$1

ompflags='OMP_MAX_ACTIVE_LEVELS=8 OMP_WAIT_POLICY=active KMP_BLOCKTIME=infinite KMP_AFFINITY="granularity=core,compact" LIBOMP_NUM_HIDDEN_HELPER_THREADS=0'
prefix_path="cmake-build-release/benchmarks"
cpu_speed=1995

mkdir -p raw_results/$benchname


for x in $(ls -1 ${prefix_path}/bench_${benchname}_* | xargs -n 1 basename | grep -v OMP_RUNTIME | sort); do
    sh -c "$ompflags $prefix_path/$x --benchmark_out_format=json --benchmark_out=raw_results/$benchname/$x.json";
done

lb4ompmodes=("fsc" "fac" "fac2" "wf2" "tap" "mfsc" "tfss" "fiss" "awf" "af")

for x in $(ls -1 ${prefix_path}/bench_${benchname}_* | xargs -n 1 basename | grep OMP_RUNTIME); do
    for schedule in ${lb4ompmodes[@]}; do
        sh -c "$ompflags KMP_CPU_SPEED=$cpu_speed OMP_SCHEDULE=$schedule $prefix_path/$x --benchmark_out_format=json --benchmark_out=raw_results/$benchname/${x}_${schedule}.json";
    done
done

