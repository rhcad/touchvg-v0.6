#!/bin/sh
python android/utf8togbk.py
make skiaview
python android/skiaview/replacejstr.py
ndk-build -C android/skiaview/jni
cd android/skiaview
sh toapp.sh
cd ../..
python android/restore_utf8.py