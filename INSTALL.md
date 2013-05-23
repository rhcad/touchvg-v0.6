# TouchVG 编译说明

## 一、iPhone/iPad 程序

* 编译条件： Mac OS X 10.5 以上，Xcode 3.2 以上

* 在 Xcode 中打开 ios 目录下的 demos.xcworkspace 工作区文件
  * 选择 FreeDraw 工程编译到 iPad / iPhone / iPod Touch 真机或模拟器上。
  * 在真机上调试时需要修改主程序的Bundle Identifier，例如“com.yourcompany.FreeDraw”，并选择自己的开发证书（Code Signing）

* ios/FreeDraw 是测试用的主程序工程
  * 使用了 Graph2d-Build 目录下的头文件和静态库文件，运行 ios/build.sh 可重新生成这些文件
  * 修改 FreeDraw 下的 [TestConfig.h](https://github.com/rhcad/touchvg/blob/master/ios/FreeDraw/Sources/TestConfig.h) 中的测试条件宏可以看到不同的效果

* ios/Graph2d 是底层图形库、矢量图形、图形视图的静态库工程
  * 在 Xcode 中打开可查看代码和编译
  * 或进入ios目录，运行“sh build.sh”将重新编译到 Graph2d-Build 目录下，需要下载Xcode命令行工具


## 二、Android 程序

* 演示程序编译 （Mac/Linux/Win)

  * 在eclipse中导入 android/graph2d 工程，在AVD模拟器或真机上运行看测试界面。
     * 可能需要根据实际安装情况修改工程的SDK版本号(target)。
  
  * 如需重新编译本地库 libgraph2d.so，则
     * 在命令行窗口中进入android目录，输入“sh ndk.sh” 自动使用ndk-build编译出libgraph2d.so。
     * 使用NDK-r8c以后的版本如果出现“build/gmsl/__gmsl:512: *** non-numeric second argument to wordlist function”错误，则打开NDK安装目录下的build/gmsl/__gmsl文件，将512行改为：
        `int_encode = $(__gmsl_tr1)$(wordlist 1,$(words $1),$(__gmsl_input_int))`
        
  * 如需在其他程序中使用绘图平台，则
     * 配置 TOUCHVG_ANDROID_APP 环境变量，指向程序的代码目录，其中有libs和src等子目录。
     * 复制 Android SDK 下的 android.jar 到 android/vglibs 下。
     * 在命令行窗口中进入android目录，输入“sh toapp.sh”生成graph2d.jar，并自动复制 graph2d.jar 和 libgraph2d.so 到 TOUCHVG_ANDROID_APP。
  
  * 如需修改内核接口、重新生成JNI类，则
     * 删除 android/demo/jni/touchvg_java_wrap.cpp ，然后按上面方法输入“sh swig.sh”重新编译。
     * 或输入“sh swig.sh”编译。

* 安装Android开发环境
  * 安装 JDK，并将其bin路径加到PATH (Mac和Linux下省略)
  * 解压安装 Android Bundle r21 到本地，含有 Android SDK r21、eclipse、ADT。
    
  * 如果需要编译本地库 libgraph2d.so，则
     * 解压安装 Android NDK (例如 r8e)，将NDK目录、Android SDK的 platform-tools 目录加到 PATH，对Mac则在 .bash_profile 文件中设置环境变量
     * 对于Windows推荐安装 MSYS (Mac下省略)，以便使用UNIX环境，或者安装cygwin
      
  * 如需修改内核接口、重新生成JNI类，则再安装：
     * 安装 ActivePython 2.7 (Mac下省略)，3.0或更高版本可能会出现encoding问题，将python目录加到 PATH
     * 安装 swigwin-2.0.9，对于Mac则编译安装 swig-2.0.9，将swigwin目录加到 PATH

## 三、Windows 程序

使用 VC++ 2008 打开 win32/projects 目录下的 graph_vc90.sln 文件，然后编译即可。如需使用 VC++ 2005 编译则打开 graph_vc80.sln 文件。
* 如果是直接下载代码zipball包，编译时可能遇到行结束符及中文编码错误，解决方法：运行 win32\utf8togbk.py 脚本，自动改为Windows平台的行结束符、将utf8编码的中文注释替换为gbk编码的文字。
