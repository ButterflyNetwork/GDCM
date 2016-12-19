#!/usr/bin/env bash

set -euo pipefail

LINUX_BUILD_DIR=build-linux
LINUX_INSTALL_DIR=install

mkdir -p $LINUX_BUILD_DIR
cd $LINUX_BUILD_DIR
FLAGS="-fPIC"
cmake .. -DGDCM_BUILD_DOCBOOK_MANPAGES:BOOL=No -DCMAKE_INSTALL_PREFIX:String=$LINUX_INSTALL_DIR -DCMAKE_C_FLAGS:String="$FLAGS" -DCMAKE_CXX_FLAGS:String="$FLAGS"
make -j8
make install
cd ..
