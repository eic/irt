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
# - use `-DATHENA=YES` if you want to build `athena` with preprocessor macro
#   `IRTGEO` defined (to be deprecated)
cmake -B build -S . \
  -DCMAKE_INSTALL_PREFIX=$ATHENA_PREFIX \
  -DREADER=YES \
  && \
cmake --build build -j$(grep -c processor /proc/cpuinfo) -- install && \
popd && \
exit 0

# exit with error, upon build failure
popd
exit 1
