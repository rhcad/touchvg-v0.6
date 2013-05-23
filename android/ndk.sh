#!/bin/sh
# Type 'sh ndk.sh' to make libgraph2d.so for Android.
#

if [ -f graph2d/jni/graph2d_java_wrap.cpp ]; then
cd graph2d/jni
ndk-build
cd ../..
if [ -f vglibs/android.jar ]; then
sh toapp.sh
fi
else
sh swig.sh
fi
