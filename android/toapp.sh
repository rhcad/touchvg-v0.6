# Type 'sh toapp.sh' to make graph2d.jar for Android.
# Need to copy $ANDROID_SDK_HOME/platforms/android-?/android.jar to vglibs/ .
#
if [ -f vglibs/android.jar ]
then
    python utf8togbk.py # the javac application don't support UTF8 files.
    cd graph2d/jni; ndk-build; cd ../..;
    cd graph2d/src/touchvg/jni; javac *.java;
    cd ../..; jar -cf vgjni.jar touchvg/jni/*.class;
    cd touchvg/view; javac -cp ../../vgjni.jar:../../../../vglibs/android.jar *.java;
    cd ../..; jar -cfv graph2d.jar touchvg/view/*.class touchvg/jni/*.class;
    cd ..; mv -v src/graph2d.jar ../vglibs;
    rm -rf src/touchvg/jni/*.class;
    rm -rf src/touchvg/view/*.class;
    rm -rf src/*.jar;
    cp -v libs/armeabi/libgraph2d.so ../vglibs/armeabi
    cd ..

    if [ -n "$TOUCHVG_ANDROID_APP" ]
    	then
    	cp -v vglibs/graph2d.jar $TOUCHVG_ANDROID_APP/libs;
    	cp -v vglibs/armeabi/libgraph2d.so $TOUCHVG_ANDROID_APP/libs/armeabi;
    	else
    	echo "you have not the environment variable TOUCHVG_ANDROID_APP"
    fi
    python restore_utf8.py
else
    echo "Warning: Need to copy android.jar from ANDROID_SDK_HOME/platforms to android/vglibs/ ."
    if [ -n "$TOUCHVG_ANDROID_APP" ]
        then
    	cp -v vglibs/armeabi/libgraph2d.so $TOUCHVG_ANDROID_APP/libs/armeabi
    fi
fi