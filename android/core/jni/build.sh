#!/bin/sh
#
if [ ! -f touchvg_java_wrap.cpp ]; then # Make JNI classes
    mkdir -p ../src/touchvg/core
    rm -rf ../src/touchvg/core/*.*
    
    swig -c++ -java -package touchvg.core -D__ANDROID__ \
        -outdir ../src/touchvg/core \
        -o touchvg_java_wrap.cpp \
        -I../../../core/pub_inc \
        -I../../../core/pub_inc/canvas \
        -I../../../core/pub_inc/graph \
        -I../../../core/pub_inc/cmd \
        -I../../../core/mgr_inc/test \
        -I../../../core/mgr_inc/view \
        ../../../core/mgr_src/view/touchvg.swig
fi
ndk-build # Make libtouchvg.so
