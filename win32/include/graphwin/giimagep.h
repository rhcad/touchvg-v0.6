//! \file giimagep.h
//! \brief 定义GDI+图像文件的辅助类 GiGdipImage
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_GDIPIMAGE_H_
#define __GEOMETRY_GDIPIMAGE_H_

#include "canvaswin.h"
#include <ocidl.h>

struct GiGdipImageImpl;

//! GDI+图像文件的辅助类
/*!
    支持 BMP/JPEG/GIF/TIFF/PNG/WMF/EMF/ICON 文件
    \ingroup GRAPH_WIN
*/
class GiGdipImage
{
public:
    //! 构造函数，可打开指定的文件
    GiGdipImage(const wchar_t* filename = L"");

    //! 从给定的位图构造
    GiGdipImage(HBITMAP hbm);

    //! 析构函数
    ~GiGdipImage();

    //! 释放内存
    void clear();

    //! 打开指定的图像文件
    bool open(const wchar_t* filename);

    //! 设置为Gdiplus::Bitmap对象，不释放也不加引用计数
    void setBitmap(void* bitmap);

    //! 返回X分辨率
    long getDpiX() const;

    //! 返回Y分辨率
    long getDpiY() const;

    //! 返回图像像素宽度
    long getWidth() const;

    //! 返回图像像素高度
    long getHeight() const;

    //! 返回图像原始宽度，单位为HIMETRIC(0.01mm)
    long getHmWidth() const;

    //! 返回图像原始高度，单位为HIMETRIC(0.01mm)
    long getHmHeight() const;

    //! 创建GDI图像资源
    HBITMAP createBitmap(GiColor bkColor = GiColor::White()) const;

    //! 显示图像（旋转角度为90度的整数倍）
    /*!
        \param graph 图形系统，如果不是 GiCanvasGdip 类型则自动取HBITMAP来显示
        \param rectW 整个图像对应的世界坐标区域
        \param fast 显示方式. true: 快速显示, false: 高精度显示
        \return 是否显示成功
    */
    bool draw(GiCanvasWin& graph, const Box2d& rectW, bool fast = false) const;

    //! 创建新的缩略图
    /*!
        \param maxWidth 新图像的最大像素宽度，0表示不缩小
        \param maxHeight 新图像的最大像素高度，0表示不缩小
        \return 新的缩略图，需要用 delete 释放
    */
    GiGdipImage* thumbnailImage(int maxWidth = 0, int maxHeight = 0);

    //! 保存新图像到文件
    /*!
        \param filename 文件全名
        \param quality JPEG质量, 0到100
        \return 是否执行成功
    */
    bool save(const wchar_t* filename, ULONG quality = 100);

    //! 得到图片后缀名对应的图像编码格式串
    /*!
        \param[in] filename 文件名，只要包含有后缀名就行，例如“1.jpg”
        \param[out] format 填充图像编码格式名称，例如“image/jpeg”
        \param[out] clsidEncoder 填充图像编码器GUID
        \return 是否查询成功
    */
    static bool getEncoder(const wchar_t* filename, WCHAR format[20], CLSID& clsidEncoder);

private:
    GiGdipImageImpl*   m_impl;
};

#endif // __GEOMETRY_GDIPIMAGE_H_
