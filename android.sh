#!/bin/sh
make skiaview
$ANDROID_NDK_ROOT/ndk-build -C android/skiaview/jni
