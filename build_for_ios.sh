#!/usr/bin/env bash

set -euo pipefail

IPHONEOS_BUILD_DIR=build-iphoneos
IPHONESIMULATOR_BUILD_DIR=build-iphonesimulator
IOS_UNIVERSAL_BUILD_DIR=build-ios-universal

mkdir -p $IPHONEOS_BUILD_DIR
cd $IPHONEOS_BUILD_DIR
FLAGS="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk -miphoneos-version-min=8.0 -arch arm64 -stdlib=libc++ -fvisibility=hidden"
cmake .. -DGDCM_BUILD_DOCBOOK_MANPAGES:BOOL=No -DCMAKE_C_FLAGS:String="$FLAGS" -DCMAKE_CXX_FLAGS:String="$FLAGS"
make -j8
cd ..

mkdir -p $IPHONESIMULATOR_BUILD_DIR
cd $IPHONESIMULATOR_BUILD_DIR
FLAGS="-isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk -miphoneos-version-min=8.0 -arch x86_64 -stdlib=libc++ -fvisibility=hidden"
cmake .. -DGDCM_BUILD_DOCBOOK_MANPAGES:BOOL=No -DCMAKE_C_FLAGS:String="$FLAGS" -DCMAKE_CXX_FLAGS:String="$FLAGS"
make -j8
cd ..

mkdir -p $IOS_UNIVERSAL_BUILD_DIR/bin
FILES=$IPHONEOS_BUILD_DIR/bin/*.a
for f in $FILES
do
    LIB=$(basename "$f")
    echo "Creating $IOS_UNIVERSAL_BUILD_DIR/bin/$LIB"
    lipo -create -output "$IOS_UNIVERSAL_BUILD_DIR/bin/$LIB" "$IPHONEOS_BUILD_DIR/bin/$LIB" "$IPHONESIMULATOR_BUILD_DIR/bin/$LIB"
done
