# 编译说明

## 一、iPhone/iPad 程序

* 编译条件： Mac OS X 10.5 以上，Xcode 3.2 以上

* 在 Xcode 中打开 ios 目录下的 TestVG.xcworkspace 工作区文件
  * 选择 TestCanvas、TestView 或其他工程编译到 iPad / iPhone / iPod Touch 真机或模拟器上。
  * 在真机上调试时需要修改主程序的Bundle Identifier，例如“com.yourcompany.TestCanvas”，并选择自己的开发证书（Code Signing）。

* 命令行编译方式
  * 进入 ios 目录，执行“sh build.sh”命令编译出 libtouchvg.a 和 touchvg.bundle

## 二、Android 程序

* 演示程序编译 （Mac/Linux/Win)

  * 在eclipse中导入 android/demo 工程，在AVD模拟器或真机上运行看测试界面。
     * 可能需要根据实际安装情况修改工程的SDK版本号(target)。
     * 需要在eclipse的Android选项页中设置NDK目录。
  
  * 如需重新编译本地库 libtouchvg.so，则
     * 在命令行窗口中进入android目录，输入“sh build.sh” 自动使用ndk-build编译出libtouchvg.so。
     * 使用NDK-r8c以后的版本如果出现“build/gmsl/__gmsl:512: *** non-numeric second argument to wordlist function”错误，则打开NDK安装目录下的build/gmsl/__gmsl文件，将512行改为：
        `int_encode = $(__gmsl_tr1)$(wordlist 1,$(words $1),$(__gmsl_input_int))`
     * 复制Android安装目录下的 android.jar 到本项目的 android 下的 output 目录，则运行“sh build.sh”将编译出 touchvg.jar 。
  
  * 如需修改内核接口、重新生成JNI类，则
     * 删除 android/demo/jni/touchvg_java_wrap.cpp ，然后按上面方法输入“sh swig.sh”重新编译。
     * 或进入android下的 mk 目录，输入“sh swig.sh”编译。

* 安装Android开发环境
  * 安装 JDK，并将其bin路径加到PATH (Mac和Linux下省略)
  * 解压安装 Android Bundle r21 或更新版本到本地，含有 Android SDK r21、eclipse、ADT。
  * 解压安装 Android NDK (例如 r8e)，在eclipse的Android选项页中设置NDK目录
    
  * 如果需要编译本地库 libtouchvg.so，则
     * 将NDK目录、Android SDK的 platform-tools 目录加到 PATH，对Mac则在 .bash_profile 文件中设置环境变量
     * 对于Windows推荐安装 MSYS (Mac下省略)，以便使用UNIX环境，或者安装cygwin
     * 复制Android安装目录下的 android.jar 到本项目的 android 下的 output 目录，则运行“sh build.sh”将编译出 touchvg.jar
      
  * 如需修改内核接口、重新生成JNI类，则再安装：
     * 安装 ActivePython 2.7 (Mac下省略)，3.0或更高版本可能会出现encoding问题，将python目录加到 PATH
     * 安装 swigwin-2.0.10，对于Mac则编译安装 swig-2.0.10，将swigwin目录加到 PATH

## 三、Windows 程序

* VC++演示程序编译
  * 在 Visual Studio 2008（或更新版本）中打开 win 目录下的 Test_vc9.sln ，选择VC++演示程序运行。
    打开 Test_cs9.sln 则运行C#演示程序，如果是 Visual Studio 2010 则打开 Test_cs10.sln 。
  * 如需在 Visual Studio 2005 中编译，则先运行 win\projects\cpvc9to8.py 脚本生成 Test_vc8.sln，然后打开 Test_vc8.sln。
  * 如果是直接下载代码zipball包，编译时可能遇到行结束符及中文编码错误。
    解决方法：运行 win\utf8togbk.py 脚本。运行 win\restore_utf8.py 脚本将还原这些文件。

* 在 MinGW 或 Linux 下，可进入 core 目录输入“make”编译C++模块。
  输入“make java”或“make python”可为Java或Python应用编译。