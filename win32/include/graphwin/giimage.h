//! \file giimage.h
//! \brief 定义图像文件打开和显示的辅助类 GiImage
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_IMAGE_H_
#define __GEOMETRY_IMAGE_H_

#include "graphwin.h"
#include <ocidl.h>
#include <objidl.h>

//! 打开文件错误类型
enum kOpenImageError
{
    kOpenImage_OK,              //!< 成功
    kOpenImage_NullString,      //!< 文件名为空
    kOpenImage_FileNotExist,    //!< 文件不存在
    kOpenImage_NullFile,        //!< 空文件
    kOpenImage_ReadFail,        //!< 读取内容失败
    kOpenImage_TooLarge,        //!< 文件太大, 超过16M
    kOpenImage_NoMemory,        //!< 内存不足
    kOpenImage_NotPicture,      //!< 不能识别图像格式
};

//! 图像文件打开和显示的辅助类
/*!
    本类采用 OleLoadPicture 解析图像格式，文件大小不超过16M，
    如果使用GDI+图形系统 GiGraphGdip ,则建议改用 GiGdipImage 类来打开和显示图形
    \ingroup GRAPH_INTERFACE
*/
class GiImage
{
public:
    //! 默认构造函数
    GiImage();

    //! 构造函数，打开指定的文件
    GiImage(const char* filename);

    //! 构造函数，打开指定的文件
    GiImage(const wchar_t* filename);

    //! 析构函数
    ~GiImage();

    //! 释放内存
    void clear();

    //! 打开指定的图像文件
    bool open(const char* filename, kOpenImageError* perr = NULL);

    //! 打开指定的图像文件
    bool open(const wchar_t* filename, kOpenImageError* perr = NULL);

    //! 返回图像原始宽度，单位为HIMETRIC(0.01mm)
    long getHmWidth() const;

    //! 返回图像原始高度，单位为HIMETRIC(0.01mm)
    long getHmHeight() const;

    //! 返回图像资源句柄
    HBITMAP getBitmap() const;

    //! 显示图像（旋转角度为90度的整数倍）
    /*!
        \param graph 图形系统
        \param rectW 整个图像对应的世界坐标区域
        \param fast 显示方式. true: 快速显示, false: 高精度显示
        \return 是否显示成功
    */
    bool draw(GiGraphWin& graph, const Box2d& rectW, bool fast = false) const;

private:
    IPicture*   m_picture;
    IStream*    m_stream;
};

#endif // __GEOMETRY_IMAGE_H_
