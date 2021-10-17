#!/bin/bash

# arguments and options (TODO: extend)
[[ $# -gt 0 ]] && clean=1 || clean=0 # clean build if any args

# ensure proper pwd
pushd $(dirname $(realpath $0))/..

# clean
if [ $clean -eq 1 ]; then
  echo "clean build -----------"
  mkdir -p build install
  rm -r build install
fi

# build
# NOTE: it may be better to use `$ATHENA_PREFIX` for the install dir,
# but this may clobber installations for those of us who also develop
# the `EIC/detectors/athena` repo
cmake -B build -S . \
  -DCMAKE_INSTALL_PREFIX=install \
  -DREADER=YES \
  -DATHENA=YES \
  && \
cmake --build build -j$(grep -c processor /proc/cpuinfo) -- install && \
popd && \
exit 0

# exit with error, upon build failure
popd
exit 1
