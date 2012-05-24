#!/bin/sh

curDir=${PWD}
cd "${curDir}"

curDate=`date   +%Y%m%d`
ipaDir="/Users/zhangyg/Desktop/touchvg_${curDate}"
mkdir "${ipaDir}"

#the directory your xcode project is
distDir="/Users/zhangyg/Desktop/touchvg/iPad/Graph2d"

cd "${distDir}"

###################################################
#directory where the ipa is
version="1_0_0"
releaseDir="build/Release-iphoneos"

TARGET="Graph2d"
CONFIGURATION="Release"
SDK="iphoneos"
IDENTITY="iPhone Distribution: Beijing Founder Electronics Co Ltd"     

xcodebuild clean
xcodebuild -target "${TARGET}" -configuration ${CONFIGURATION} -sdk iphoneos CODE_SIGN_IDENTITY="${IDENTITY}"

ipapath="${ipaDir}/${TARGET}_${version}_${curTime}.ipa"
ipa="${TARGET}.ipa"

#echo "$ipapath"
appfile="${releaseDir}/${TARGET}.app"
#echo "$appfile"

xcrun -sdk iphoneos PackageApplication -v "$appfile" -o "$ipapath" --sign "${IDENTITY}"

#echo "=========Success========="
echo "Waiting for next runtime......"

cd "${curDir}"

#done
