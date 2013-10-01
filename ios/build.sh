#!/bin/sh
# Type 'sh build.sh' to make TouchVG's library and bundle.

iphoneos61=`xcodebuild -showsdks | grep -i iphoneos6.1`
iphoneos51=`xcodebuild -showsdks | grep -i iphoneos5.1`
iphoneos43=`xcodebuild -showsdks | grep -i iphoneos4.3`

if [ -n "$iphoneos61" ]; then
    xcodebuild -project lib/TouchVG/TouchVG.xcodeproj -sdk iphoneos6.1 -configuration Release -arch armv7
    xcodebuild -project lib/DemoCmds/DemoCmds.xcodeproj -sdk iphoneos6.1 -configuration Release -arch armv7
else
if [ -n "$iphoneos51" ]; then
    xcodebuild -project lib/TouchVG/TouchVG.xcodeproj -sdk iphoneos5.1 -configuration Release
    xcodebuild -project lib/DemoCmds/DemoCmds.xcodeproj -sdk iphoneos5.1 -configuration Release
else
if [ -n "$iphoneos43" ]; then
    xcodebuild -project lib/TouchVG/TouchVG.xcodeproj -sdk iphoneos4.3 -configuration Release
    xcodebuild -project lib/DemoCmds/DemoCmds.xcodeproj -sdk iphoneos4.3 -configuration Release
fi
fi
fi

mkdir -p output/include
cp -R lib/TouchVG/build/Release-universal/libTouchVG.a output
cp -R lib/TouchVG/build/Release-universal/usr/local/include/*.h output/include
cp -R lib/DemoCmds/build/Release-universal/libDemoCmds.a output
cp -R lib/DemoCmds/build/Release-universal/usr/local/include/*.h output/include