#!/bin/sh
export PATH=$PATH:$ANDROID_NDK_ROOT
make skiaview
ndk-build -C android/skiaview/jni
