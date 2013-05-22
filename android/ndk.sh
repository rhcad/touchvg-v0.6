#!/bin/sh
# Type 'sh build.sh' to make libgraph2d.so for Android.
#
cd graph2d/jni
ndk-build
cd ../..
if [ -f vglibs/android.jar ]; then
sh toapp.sh
fi