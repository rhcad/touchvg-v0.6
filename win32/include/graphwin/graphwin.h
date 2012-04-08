//! \file graphwin.h
//! \brief 定义图形系统类 GiGraphWin
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#ifndef __GEOMETRY_GRAPHWINSYS_WIN_H_
#define __GEOMETRY_GRAPHWINSYS_WIN_H_

#include "gigraph.h"

#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

//! Windows平台的图形系统类
/*! 本类用于在Windows平台显示各种图形，图元显示原语由派生类实现
    \ingroup GRAPH_INTERFACE
*/
class GiGraphWin : public GiGraphics
{
public:
    //! 默认构造函数
    GiGraphWin(GiTransform* xform);

    //! 拷贝构造函数
    GiGraphWin(const GiGraphWin& src);

    //! 赋值操作符函数
    GiGraphWin& operator=(const GiGraphWin& src);

public:
    //! 准备开始绘图
    /*!
        本函数有选择性的创建绘图缓冲，初始化绘图环境。
        在调用绘图函数前，必须调用本函数。\n
        当参数hdc为NULL或先前绘图还未结束，则调用失败，此时不要调用绘图函数和endPaint()。
        如果调用成功，则在绘图完成或停止时，必须调用endPaint()。\n
        绘图时，映射模式必须为MM_TEXT，否则失败。
        打印预览时hdc的映射模式可以不是MM_TEXT，但attribDC的映射模式则必须为MM_TEXT。\n
        本函数调用了 setResolution 函数，打印和打印预览时还调用了 setWndSize 函数。

        \param hdc 显示输出的设备描述表句柄
        \param attribDC 打印预览时为打印机、绘图仪的设备描述表，其余时候为NULL
        \param buffered 是否使用绘图缓冲，打印或打印预览时忽略该参数
        \param overlay 当使用绘图缓冲时, 该参数表示是否在原hdc上的显示内容
            上进行叠加显示；如果不叠加显示则通常还要调用 clearWnd()
        \return 是否初始化成功，失败原因可能是先前绘图还未结束，或参数错误
        \see endPaint
    */
    virtual bool beginPaint(HDC hdc, HDC attribDC = NULL, 
        bool buffered = true, bool overlay = false);

    //! 结束绘图
    /*! 本函数销毁绘图缓冲和绘图环境的临时数据，如果使用了绘图缓冲，就将缓冲中的
        图形显示到原绘图输出设备描述表上。调用 beginPaint 成功后，必须调用本函数。
        \param draw 是否将绘图缓冲内容显示出来，在beginPaint中指定不使用绘图缓冲时无效。
            在仅需要生成绘图缓冲时传入false，这样可调用 drawCachedBitmap2 来显示缓冲内容
        \see beginPaint
        \see drawCachedBitmap2
    */
    virtual void endPaint(bool draw = true);

    //! 获取绘图DC
    /*! 如果在函数 beginPaint 中指定使用绘图缓冲，则会返回绘图缓冲DC。\n
        调用本函数后，只能使用GDI的函数，一般不能调用本类的Drawxxx绘图函数。\n
        必须在 endPaint 之前调用 releaseDC 释放资源。
        \return 设备描述表句柄
        \see releaseDC
    */
    virtual HDC acquireDC() = 0;

    //! 释放绘图DC
    /*! 调用了 acquireDC 后必须调用本函数释放
        \param hdc 由 acquireDC() 返回的设备描述表句柄
        \see acquireDC
    */
    virtual void releaseDC(HDC hdc) = 0;

    //! 返回屏幕分辨率DPI, 常量
    virtual int getScreenDpi() const;

    //! 显示文字的原语函数(类似于TextOutA)
    virtual bool rawTextOut(HDC hdc, int x, int y, const char* str, int len);
    
    //! 显示文字的原语函数(类似于TextOutW)
    virtual bool rawTextOut(HDC hdc, int x, int y, const wchar_t* str, int len);

    //! 显示文字的原语函数(类似于ExtTextOutA)
    virtual bool rawTextOut(HDC hdc, int x, int y, UInt32 options, 
        const RECT* prc, const char* str, int len, const Int32* pDx);

    //! 显示文字的原语函数(类似于ExtTextOutW)
    virtual bool rawTextOut(HDC hdc, int x, int y, UInt32 options, 
        const RECT* prc, const wchar_t* str, int len, const Int32* pDx);

    //! 显示图像（旋转角度为90度的整数倍）
    /*!
        \param hmWidth 图像原始宽度，单位为HIMETRIC(0.01mm)
        \param hmHeight 图像原始高度，单位为HIMETRIC(0.01mm)
        \param hbitmap 图像资源句柄
        \param rectW 整个图像对应的世界坐标区域
        \param fast 显示方式. true: 快速显示, false: 高精度显示
        \return 是否显示成功
    */
    virtual bool drawImage(long hmWidth, long hmHeight, HBITMAP hbitmap, 
        const Box2d& rectW, bool fast = false) = 0;

protected:
    HDC         m_attribDC;          //!< 属性DC，打印预览时为打印机DC
};

//! 打印页面设置
/*! 该函数设置显示分辨率、显示窗口大小，并根据条件适当放缩显示。\n
    调用了该函数后不需要再调用 setResolution 和 setWndSize 函数。\n
    本函数必须先于 beginPaint 函数被调用。

    \param xf 待设置的坐标系对象
    \param prtDC 打印机或绘图仪的设备描述表句柄
    \param rectShow 要打印的图形范围，是否为世界坐标由bWorldRect决定。\n
        如果是模型坐标矩形，应直接传入模型坐标，由本函数采用缺省模型变化矩阵计算。
    \param bWorldRect true表示给定图形范围是世界坐标，false表示是模型坐标
    \param margin 四个方向的页边距，单位为0.01mm，为NULL则取打印纸最小页边距
    \param scale 打印比例，为1则无放缩打印，小于1则缩小打印；为0或负数则充满打印
    \param offsetX 图形在打印纸上向右额外偏移量，毫米，
        当指定了有效的打印比例时该参数有效
    \param offsetY 图形在打印纸上向上额外偏移量，毫米，
        当指定了有效的打印比例时该参数有效
    \return 是否设置和放缩成功
*/
bool giPrintSetup(GiTransform& xf, HDC prtDC, const Box2d& rectShow,
    bool bWorldRect = true, const RECT* margin = NULL,
    double scale = 0, double offsetX = 0, double offsetY = 0);

//! 自动创建和释放兼容DC的辅助类
class GiCompatibleDC
{
    HDC m_hdc;
public:
    //! 构造函数, 自动创建兼容DC
    GiCompatibleDC(HDC hdc)
    {
        m_hdc = ::CreateCompatibleDC(hdc);
    }
    //! 析构函数, 自动释放兼容DC
    ~GiCompatibleDC()
    {
        if (m_hdc != NULL)
            ::DeleteDC(m_hdc);
    }
    //! 返回兼容DC的操作符函数
    operator HDC() const
    {
        return m_hdc;
    }
};

#endif // __GEOMETRY_GRAPHWINSYS_WIN_H_
