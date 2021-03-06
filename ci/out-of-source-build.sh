#!/usr/bin/env bash

# this script does something close to the work flow end users may follow when
# building Graphviz

set -x
set -e
set -o pipefail

# output some info for debugging
uname -rms
cat /etc/os-release

GV_VERSION=$(cat GRAPHVIZ_VERSION)

# unpack the portable source tarball
tar xfz graphviz-${GV_VERSION}.tar.gz

# setup a directory for building in
mkdir build
cd build
../graphviz-${GV_VERSION}/configure --enable-lefty

# build Graphviz
make

# install Graphviz
make install
