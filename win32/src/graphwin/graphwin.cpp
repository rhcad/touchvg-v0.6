#include "graphwin.h"
#include <_gigraph.h>

GiGraphWin::GiGraphWin(GiTransform* xform)
    : GiGraphics(xform), m_attribDC(NULL)
{
}

GiGraphWin::GiGraphWin(const GiGraphWin& src)
    : GiGraphics(src), m_attribDC(src.m_attribDC)
{
}

GiGraphWin& GiGraphWin::operator=(const GiGraphWin& src)
{
    if (this != &src)
    {
        GiGraphics::operator=(src);
        m_attribDC = src.m_attribDC;
    }
    return *this;
}

bool giPrintSetup(GiTransform& xf, HDC hdc, const Box2d& rectShow, bool bWorldRect,
                  const RECT* margin, double scale,
                  double offsetX, double offsetY)
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
    xf.setResolution(dpix, dpiy);

    Box2d rectShowW = rectShow;
    if (!bWorldRect)
        rectShowW *= xf.modelToWorld();

    Box2d rectOld = xf.setWorldLimits(Box2d());
    bool ret = xf.zoomTo(rectShowW, &rc, true);
    if (scale >= xf.getMinViewScale() && scale <= xf.getMaxViewScale())
    {
        ret = ret && xf.zoomScale(scale, NULL, true);
        xf.zoom(xf.getCenterW() - Vector2d(offsetX, offsetY), xf.getViewScale());
    }
    xf.setWorldLimits(rectOld);

    return ret;
}

int GiGraphWin::getScreenDpi() const
{
    if (0 == m_impl->screenDPI())
    {
        HDC hdc = ::GetDC(NULL);
        if (hdc != NULL)
        {
            InterlockedExchange(&m_impl->screenDPI(), GetDeviceCaps(hdc, LOGPIXELSY));
            ::ReleaseDC(NULL, hdc);
        }
    }
    return m_impl->screenDPI();
}

bool GiGraphWin::beginPaint(HDC hdc, HDC attribDC, bool buffered, bool)
{
    if (isDrawing() || hdc == NULL)
        return false;

    if (attribDC == hdc)
        attribDC = NULL;
    HDC prtDC = (attribDC != NULL) ? attribDC : hdc;
    if (::GetMapMode(prtDC) != MM_TEXT)
        return false;

    m_attribDC = attribDC;
    m_impl->drawColors = GetDeviceCaps(prtDC, NUMCOLORS);
    m_impl->xform->setResolution(GetDeviceCaps(prtDC, LOGPIXELSX), 
        GetDeviceCaps(prtDC, LOGPIXELSY));
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

    if (m_impl->isPrint
        || (buffered && !hasCachedBitmap())
        || ERROR == ::GetClipBox(hdc, &clipBox)
        || ::IsRectEmpty(&clipBox))
    {
        ::SetRect(&clipBox, 0, 0, xf().getWidth(), xf().getHeight());
    }

    GiGraphics::beginPaint(&clipBox);

    return true;
}

void GiGraphWin::endPaint(bool /*draw*/)
{
    if (isDrawing())
    {
        m_attribDC = NULL;
        GiGraphics::endPaint();
    }
}

bool GiGraphWin::rawTextOut(HDC hdc, int x, int y, const char* str, int len)
{
    return ::TextOutA(hdc, x, y, str, len) != 0;
}

bool GiGraphWin::rawTextOut(HDC hdc, int x, int y, const wchar_t* str, int len)
{
    return ::TextOutW(hdc, x, y, str, len) != 0;
}

bool GiGraphWin::rawTextOut(HDC hdc, int x, int y, 
                            UInt32 options, const RECT* prc, 
                            const char* str, int len, const Int32* pDx)
{
    return ::ExtTextOutA(hdc, x, y, options, prc, 
        str, len, reinterpret_cast<const INT *>(pDx)) != 0;
}

bool GiGraphWin::rawTextOut(HDC hdc, int x, int y, 
                            UInt32 options, const RECT* prc, 
                            const wchar_t* str, int len, const Int32* pDx)
{
    return ::ExtTextOutW(hdc, x, y, options, prc, 
        str, len, reinterpret_cast<const INT *>(pDx)) != 0;
}
