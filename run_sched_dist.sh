#!/bin/bash
set -euxo pipefail

ompflags='OMP_MAX_ACTIVE_LEVELS=8 OMP_WAIT_POLICY=active KMP_BLOCKTIME=infinite KMP_AFFINITY="granularity=core,compact" LIBOMP_NUM_HIDDEN_HELPER_THREADS=0'
prefix_path="cmake-build-release/scheduling_dist"
cpu_speed=1995

mkdir -p raw_results/scheduling_dist


for x in $(ls -1 ${prefix_path}/scheduling_dist_* | xargs -n 1 basename | grep -v OMP_RUNTIME | sort); do
   sh -c "$ompflags $prefix_path/$x > raw_results/scheduling_dist/$x.json";
done


lb4ompmodes=("fsc" "fac" "fac2" "wf2" "tap" "mfsc" "tfss" "fiss" "awf" "af")


for x in $(ls -1 ${prefix_path}/scheduling_dist_* | xargs -n 1 basename | grep OMP_RUNTIME); do
    for schedule in ${lb4ompmodes[@]}; do
        sh -c "$ompflags KMP_CPU_SPEED=$cpu_speed OMP_SCHEDULE=$schedule $prefix_path/$x > raw_results/scheduling_dist/${x}_${schedule}.json";
    done
done
