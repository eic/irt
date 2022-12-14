#!/bin/bash
# set env vars, for convenience

# add ./bin and ./bin/install to PATH
if [ -z "${BASH_SOURCE[0]}" ]; then wd=$(dirname $(realpath $0))
else wd=$(dirname $(realpath ${BASH_SOURCE[0]})); fi
PATH=$PATH:$wd/bin
#PATH=$PATH:$wd/install/bin # (instead using prefix $ATHENA_PREFIX)
export PATH

# source detector and juggler environment
source /opt/detector/setup.sh
