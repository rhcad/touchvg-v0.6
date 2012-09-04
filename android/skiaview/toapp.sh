cd src/touchvg/view; javac -cp ../../../libs/skiaview.jar:$ANDROID_SDK_HOME/platforms/android-8/android.jar *.java;
cd ../..; jar -cfv paintview.jar touchvg/view/*.class;
cd ..; mv -v src/*.jar libs;
rm -rf src/touchvg/view/*.class;
if [ -n "$TOUCHVG_ANDROID_APP" ]
	then
	cd libs;
	cp -v skiaview.jar $TOUCHVG_ANDROID_APP/libs;
	cp -v paintview.jar $TOUCHVG_ANDROID_APP/libs;
	cp -v armeabi/libskiaview.so $TOUCHVG_ANDROID_APP/libs/armeabi;
	cd ..
	else
	echo "you have not the environment variable TOUCHVG_ANDROID_APP"
fi
cd