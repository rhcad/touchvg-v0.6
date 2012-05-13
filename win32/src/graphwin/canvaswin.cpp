// canvaswin.cpp: 实现图形显示接口类 GiCanvasWin
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "canvaswin.h"
#include <_gigraph.h>

GiCanvasWin::GiCanvasWin(GiGraphics* gs) : m_attribDC(NULL)
{
    gs->_setCanvas(this);
}

const GiTransform& GiCanvasWin::xf() const
{
    return m_owner->xf();
}

void GiCanvasWin::copy(const GiCanvasWin& src)
{
    if (this != &src)
    {
        m_owner->copy(*src.m_owner);
        m_attribDC = src.m_attribDC;
    }
}

bool giPrintSetup(GiTransform& xf, HDC hdc, const Box2d& rectShow, bool bWorldRect,
                  const RECT* margin, float scale,
                  float offsetX, float offsetY)
{
    if (hdc == NULL || rectShow.isEmpty())
        return false;

    int dpix   = GetDeviceCaps(hdc, LOGPIXELSX);        // 每英寸的像素数
    int dpiy   = GetDeviceCaps(hdc, LOGPIXELSY);        // 每英寸的像素数
    int width  = GetDeviceCaps(hdc, PHYSICALWIDTH);     // 页面宽，像素
    int height = GetDeviceCaps(hdc, PHYSICALHEIGHT);    // 页面高，像素
    int cx     = GetDeviceCaps(hdc, HORZRES);           // 可打印宽度，像素
    int cy     = GetDeviceCaps(hdc, VERTRES);           // 可打印高度，像素
    int ml     = GetDeviceCaps(hdc, PHYSICALOFFSETX);   // 左最小边距，像素
    int mt     = GetDeviceCaps(hdc, PHYSICALOFFSETY);   // 上最小边距，像素
    int mr     = width - cx - ml;                       // 右最小边距，像素
    int mb     = height - cy - mt;                      // 下最小边距，像素
    RECT rc    = { 0, 0, cx, cy };                      // 纸上打印范围，像素

    if (margin != NULL)     // 指定了页边距
    {
        int n;
        if (ml < (n = MulDiv(margin->left, dpix, 2540)))
            rc.left += n - ml;
        if (mt < (n = MulDiv(margin->top, dpiy, 2540)))
            rc.top += n - mt;
        if (mr < (n = MulDiv(margin->right, dpix, 2540)))
            rc.right -= n - mr;
        if (mb < (n = MulDiv(margin->bottom, dpiy, 2540)))
            rc.bottom -= n - mb;
        if (rc.right - rc.left < 10)
        {
            rc.left = 0;
            rc.right = cx;
        }
        if (rc.bottom - rc.top < 10)
        {
            rc.top = 0;
            rc.bottom = cy;
        }
    }
    ::InflateRect(&rc, -1, -1);     // 缩小纸上打印范围，以便放缩后不丢失像素

    xf.setWndSize(cx, cy);
    xf.setResolution((float)dpix, (float)dpiy);

    Box2d rectShowW = rectShow;
    if (!bWorldRect)
        rectShowW *= xf.modelToWorld();

    Box2d rectOld = xf.setWorldLimits(Box2d());
    RECT2D rc2d(giConvertRect(rc));
    bool ret = xf.zoomTo(rectShowW, &rc2d, true);
    if (scale >= xf.getMinViewScale() && scale <= xf.getMaxViewScale())
    {
        ret = ret && xf.zoomScale(scale, NULL, true);
        xf.zoom(xf.getCenterW() - Vector2d(offsetX, offsetY), xf.getViewScale());
    }
    xf.setWorldLimits(rectOld);

    return ret;
}

float GiCanvasWin::getScreenDpi() const
{
    static long s_dpi = 0;

    if (0 == s_dpi)
    {
        HDC hdc = ::GetDC(NULL);
        if (hdc != NULL)
        {
            InterlockedExchange(&s_dpi, GetDeviceCaps(hdc, LOGPIXELSY));
            ::ReleaseDC(NULL, hdc);
        }
    }

    return (float)s_dpi;
}

bool GiCanvasWin::beginPaint(HDC hdc, HDC attribDC, bool buffered, bool)
{
    if (m_owner->isDrawing() || hdc == NULL)
        return false;

    if (attribDC == hdc)
        attribDC = NULL;
    HDC prtDC = (attribDC != NULL) ? attribDC : hdc;
    if (::GetMapMode(prtDC) != MM_TEXT)
        return false;

    m_attribDC = attribDC;
    m_impl->drawColors = GetDeviceCaps(prtDC, NUMCOLORS);
    m_impl->xform->setResolution((float)GetDeviceCaps(prtDC, LOGPIXELSX), 
        (float)GetDeviceCaps(prtDC, LOGPIXELSY));
    m_impl->isPrint = (DT_RASDISPLAY != GetDeviceCaps(prtDC, TECHNOLOGY));
    if (m_impl->isPrint)
    {
        m_impl->xform->setWndSize(GetDeviceCaps(prtDC, HORZRES),
            GetDeviceCaps(prtDC, VERTRES));
    }
    else
    {
        m_attribDC = NULL;
    }

    RECT clipBox;

    if (m_owner->isPrint()
        || (buffered && !hasCachedBitmap())
        || ERROR == ::GetClipBox(hdc, &clipBox)
        || ::IsRectEmpty(&clipBox))
    {
        ::SetRect(&clipBox, 0, 0, xf().getWidth(), xf().getHeight());
    }

    m_owner->_beginPaint(giConvertRect(clipBox));

    return true;
}

void GiCanvasWin::endPaint(bool /*draw*/)
{
    if (m_owner->isDrawing())
    {
        m_attribDC = NULL;
        m_owner->_endPaint();
    }
}

bool GiCanvasWin::rawTextOut(HDC hdc, float x, float y, const char* str, int len)
{
    return ::TextOutA(hdc, mgRound(x), mgRound(y), str, len) != 0;
}

bool GiCanvasWin::rawTextOut(HDC hdc, float x, float y, const wchar_t* str, int len)
{
    return ::TextOutW(hdc, mgRound(x), mgRound(y), str, len) != 0;
}

bool GiCanvasWin::rawTextOut(HDC hdc, float x, float y, 
                            UInt32 options, const RECT2D& rc, 
                            const char* str, int len, const Int32* pDx)
{
    RECT rect = { mgRound(rc.left), mgRound(rc.top), mgRound(rc.right), mgRound(rc.bottom) };
    return ::ExtTextOutA(hdc, mgRound(x), mgRound(y), options, &rect, 
        str, len, reinterpret_cast<const INT *>(pDx)) != 0;
}

bool GiCanvasWin::rawTextOut(HDC hdc, float x, float y, 
                            UInt32 options, const RECT2D& rc, 
                            const wchar_t* str, int len, const Int32* pDx)
{
    RECT rect = { mgRound(rc.left), mgRound(rc.top), mgRound(rc.right), mgRound(rc.bottom) };
    return ::ExtTextOutW(hdc, mgRound(x), mgRound(y), options, &rect, 
        str, len, reinterpret_cast<const INT *>(pDx)) != 0;
}

RECT2D giConvertRect(const RECT& rc)
{
    RECT2D rect;
    rect.left = (float)rc.left;
    rect.top = (float)rc.top;
    rect.right = (float)rc.right;
    rect.bottom = (float)rc.bottom;
    return rect;
}
