#!/bin/sh
make skiaview
python android/skiaview/replacejstr.py
ndk-build NDK_DEBUG=1 -C android/skiaview/jni
cd android/skiaview
sh toapp.sh