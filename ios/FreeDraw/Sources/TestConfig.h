//  TestConfig.h
//  FreeDraw
//  Created by Zhang Yungui on 2012-3-2.
//

// 定义TESTMODE_SIMPLEVIEW时使用简单视图控制器，注释掉时使用带按钮面板的复杂视图
//#define TESTMODE_SIMPLEVIEW

// 是否使用 RandomShape.cpp
#define USE_RANDOMSHAPE

// 定义USE_MAGNIFIER时使用放大镜视图，未定义TESTMODE_SIMPLEVIEW时有效
#define USE_MAGNIFIER

// 定义MAG_AT_BOTTOM时放大镜视图在底部，注释掉时放大镜浮动显示，定义了USE_MAGNIFIER时有效
//#define MAG_AT_BOTTOM

// 定义USE_STROKEWIDTH时使用像素单位的线宽，注释掉时使用0.01毫米单位线宽，未定义TESTMODE_SIMPLEVIEW时有效
//#define USE_STROKEWIDTH

// 定义AUTO_HIDE_CMDBAR时在触摸时自动隐藏命令按钮
#define AUTO_HIDE_CMDBAR
