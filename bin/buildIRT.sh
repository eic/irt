#!/bin/bash

# arguments and options (TODO: extend)
[[ $# -gt 0 ]] && clean=1 || clean=0 # clean build if any args

# ensure proper pwd
pushd $(dirname $(realpath $0))/..

# clean
if [ $clean -eq 1 ]; then
  echo "clean build -----------"
  mkdir -p build
  rm -r build
fi

# build
# - NOTE: hopefully using `$ATHENA_PREFIX` doesn't bother those of us
#   who also develop the `EIC/detectors/athena` repo
cmake -B build -S . \
  -DCMAKE_INSTALL_PREFIX=$ATHENA_PREFIX \
  && \
cmake --build build -j$((`nproc`/4+1)) -- install && \
popd && \
exit 0

# exit with error, upon build failure
popd
exit 1
