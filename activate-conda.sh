#!/bin/bash -x

DIR=${CONDA_PREFIX:-$HOME/miniconda3}
CONDA_PROFILE=$DIR/etc/profile.d/conda.sh
[ -f $CONDA_PROFILE ] || { # install it
    if [ `uname -s` == "Darwin" ]; then
      INST="Miniconda3-latest-MacOSX-aarch64.sh"
    elif [ `uname -s` == "Linux" ]; then
      INST="Miniconda3-latest-Linux-aarch64.sh"
    fi
    mkdir -p $DIR; cd $DIR/..
    [ -f $INST ] || curl -O https://repo.continuum.io/miniconda/$INST
    bash $INST -b -p $DIR -f
    unset INST; cd -
    [ -x $CONDA ] || exit 1
}
[ x$DIR/bin/conda == x`which conda` ] || { # initialize
    . $CONDA_PROFILE
}
source activate base
conda activate base
[ x$DIR/bin/python == x`which python` ] || exit 1 # check
