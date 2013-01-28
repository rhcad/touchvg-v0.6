# Type 'sh toapp.sh' to make skiaview.jar for Android.
# Need to copy $ANDROID_SDK_HOME/platforms/android-?/android.jar to libs/ .
#
if [ -f libs/android.jar ]
then
    python utf8togbk.py # the javac application don't support UTF8 files.
    cd skiaview/src/touchvg/jni; javac *.java;
    cd ../..; jar -cf vgjni.jar touchvg/jni/*.class;
    cd touchvg/view; javac -cp ../../vgjni.jar:../../../../libs/android.jar *.java;
    cd ../..; jar -cfv skiaview.jar touchvg/view/*.class touchvg/jni/*.class;
    cd ..; mv -v src/skiaview.jar ../libs;
    rm -rf src/touchvg/jni/*.class;
    rm -rf src/touchvg/view/*.class;
    rm -rf src/*.jar;
    cp -v libs/armeabi/libskiaview.so ../libs/armeabi
    cd ..

    if [ -n "$TOUCHVG_ANDROID_APP" ]
    	then
    	cp -v libs/skiaview.jar $TOUCHVG_ANDROID_APP/libs;
    	cp -v libs/armeabi/libskiaview.so $TOUCHVG_ANDROID_APP/libs/armeabi;
    	else
    	echo "you have not the environment variable TOUCHVG_ANDROID_APP"
    fi
    python restore_utf8.py
else
    echo "Warning: Need to copy android.jar from ANDROID_SDK_HOME/platforms to android/libs/ ."
    if [ -n "$TOUCHVG_ANDROID_APP" ]
        then
    	cp -v libs/armeabi/libskiaview.so $TOUCHVG_ANDROID_APP/libs/armeabi
    fi
fi