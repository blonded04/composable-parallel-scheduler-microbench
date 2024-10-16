#!/bin/bash -x

set -e

git submodule update --init

cd onetbb
git checkout v2021.13.0
cd ../taskflow
git checkout v3.8.0
cd ../googletest
git checkout release-1.12.0
cd ../googlebenchmark
git checkout v1.7.0
cd ../

