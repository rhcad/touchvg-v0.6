TouchVG
======
A lightweight 2D vector drawing framework using C++ for iOS, Android and Windows.

Installation notes in chinese, see INSTALL.md .
License: GNU GPL v3. Author: Zhang Yungui <rhcad@hotmail.com>

FAQ and other hints may be found on the Wiki:
        https://github.com/rhcad/touchvg/wiki

Discussion and suggestions are welcome. You may commit issues on the site:
        https://github.com/rhcad/touchvg/issues

Email to the current maintainers may be sent to <rhcad@hotmail.com>.
Some documents in chinese may be found at http://www.cnblogs.com/rhcad .

Build
------
Installation notes in chinese, see INSTALL.md .
* Build for Android applications:
  * Import the android/test project into eclipse, then run the demo application.
  * Or type 'sh build.sh' in the android folder to make libtouchvg.so and touchvg.jar for Android applications.
* Build for iOS applications:
  * Open ios/TestVG.xcworkspace in Xcode, then run a demo application.
  * Or type 'sh build.sh' in the ios folder to make libtouchvg.a for iOS applications.
* Build for WPF applications:
  * Open wpf/Test_cs9.sln in VS2008(or Test_cs10.sln in VS2010), then run a demo application.