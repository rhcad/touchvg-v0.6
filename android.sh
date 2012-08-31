#!/bin/sh
make skiaview
ndk-build -C android/skiaview/jni
cd android/skiaview
sh toapp.sh