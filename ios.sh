#!/bin/sh
# Replace destlibs as your app's location.

cd ios/Graph2d
destlibs=../Graph2d-Build

xcodebuild -sdk iphoneos5.1 -configuration Release
cp ../Build/ios/Graph2d/*.h $destlibs/include/
cp -v ../Build/Products/Release-iphoneos/libGraph2d.a $destlibs/libs/iphoneos

xcodebuild -sdk iphonesimulator5.1 -configuration Release
cp -v ../Build/Products/Release-iphonesimulator/libGraph2d.a $destlibs/libs/iphonesimulator