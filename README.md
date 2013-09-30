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
* Build for Android:
  * Import the android/demo project into eclipse, then run the demo application.
  * Or type 'sh build.sh' in the android folder to make libtouchvg.so and touchvg.jar for Android applications.
* Build for iOS:
  * Open ios/TestVG.xcworkspace in Xcode, then run a demo application.
  * Or type 'sh build.sh' in the ios folder to make libtouchvg.a for iOS applications.
* Build for Windows:
  * Open win/Test_vc9.sln in VC++ 2008, then run a demo application.
  * Open win/Test_cs9.sln (or Test_cs10.sln for VS2010) to run the C# demo application.