#!/usr/bin/env bash

set -euo pipefail

MACOS_BUILD_DIR=build-macos
MACOS_INSTALL_DIR=install

mkdir -p $MACOS_BUILD_DIR
cd $MACOS_BUILD_DIR
FLAGS="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk -mmacosx-version-min=10.8 -stdlib=libc++"
cmake .. -DGDCM_BUILD_DOCBOOK_MANPAGES:BOOL=No -DCMAKE_INSTALL_PREFIX:String=$MACOS_INSTALL_DIR -DCMAKE_C_FLAGS:String="$FLAGS" -DCMAKE_CXX_FLAGS:String="$FLAGS"
make -j8
make install
cd ..
