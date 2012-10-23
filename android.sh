#!/bin/sh
make skiaview
python android/skiaview/replacejstr.py
ndk-build -C android/skiaview/jni
cd android/skiaview
sh toapp.sh