//! \file mgtype.h
//! \brief 定义数学几何库的简单数据类型 VECTOR_2D,POINT_2D,BOX_2D,MATRIX_2D
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGTYPE_H_
#define __GEOMETRY_MGTYPE_H_

//! 二维矢量结构
typedef struct tagVECTOR2D
{
    float   x;      //!< X坐标分量
    float   y;      //!< Y坐标分量
} VECTOR_2D;

//! 二维点结构
typedef struct tagPOINT2D
{
    float   x;      //!< X坐标分量
    float   y;      //!< Y坐标分量
} POINT_2D;

//! 浮点矩形框结构，用于模型坐标系或世界坐标系
typedef struct tagBOX2D
{
    float   xmin;   //!< 矩形左下角X坐标
    float   ymin;   //!< 矩形左下角Y坐标
    float   xmax;   //!< 矩形右上角X坐标
    float   ymax;   //!< 矩形右上角Y坐标
} BOX_2D;

//! 显示坐标矩形框结构，用于显示坐标系
typedef struct tagRECT2D
{
    float   left;   //!< 矩形左上角X坐标
    float   top;    //!< 矩形左上角Y坐标
    float   right;  //!< 矩形右下角X坐标
    float   bottom; //!< 矩形右下角Y坐标
} RECT_2D;

//! 二维齐次变换矩阵结构
typedef struct tagMATRIX2D
{
    float   m11;    //!< 矩阵元素，代表X方向线性变换分量
    float   m12;    //!< 矩阵元素，代表X方向线性变换分量
    float   m21;    //!< 矩阵元素，代表Y方向线性变换分量
    float   m22;    //!< 矩阵元素，代表Y方向线性变换分量
    float   dx;     //!< 矩阵元素，代表X方向平移量
    float   dy;     //!< 矩阵元素，代表Y方向平移量
} MATRIX_2D;

// 定义准确字节长度的基本数据类型
#ifndef Int32
typedef signed   char  Int8;    //!< 单字节有符号整数类型
typedef unsigned char  UInt8;   //!< 单字节无符号整数类型
typedef signed   short Int16;   //!< 双字节有符号整数类型
typedef unsigned short UInt16;  //!< 双字节无符号整数类型
typedef signed   long  Int32;   //!< 四字节有符号整数类型
typedef unsigned long  UInt32;  //!< 四字节无符号整数类型
#endif

#endif // __GEOMETRY_MGTYPE_H_
