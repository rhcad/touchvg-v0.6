#include "graphwin.h"
#include "../graph/_gigraph.h"

_GEOM_BEGIN

GiGraphWin::GiGraphWin(GiTransform& xform)
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

    int dpix   = GetDeviceCaps(hdc, LOGPIXELSX);        // ÿӢ���������
    int dpiy   = GetDeviceCaps(hdc, LOGPIXELSY);        // ÿӢ���������
    int width  = GetDeviceCaps(hdc, PHYSICALWIDTH);     // ҳ���������
    int height = GetDeviceCaps(hdc, PHYSICALHEIGHT);    // ҳ��ߣ�����
    int cx     = GetDeviceCaps(hdc, HORZRES);           // �ɴ�ӡ���ȣ�����
    int cy     = GetDeviceCaps(hdc, VERTRES);           // �ɴ�ӡ�߶ȣ�����
    int ml     = GetDeviceCaps(hdc, PHYSICALOFFSETX);   // ����С�߾࣬����
    int mt     = GetDeviceCaps(hdc, PHYSICALOFFSETY);   // ����С�߾࣬����
    int mr     = width - cx - ml;                       // ����С�߾࣬����
    int mb     = height - cy - mt;                      // ����С�߾࣬����
    RECT rc    = { 0, 0, cx, cy };                      // ֽ�ϴ�ӡ��Χ������

    if (margin != NULL)     // ָ����ҳ�߾�
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
    ::InflateRect(&rc, -1, -1);     // ��Сֽ�ϴ�ӡ��Χ���Ա�����󲻶�ʧ����

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

    GiGraphics::beginPaint();

    m_attribDC = attribDC;
    m_impl->drawColors = GetDeviceCaps(prtDC, NUMCOLORS);
    m_impl->xform.setResolution(GetDeviceCaps(prtDC, LOGPIXELSX), 
        GetDeviceCaps(prtDC, LOGPIXELSY));
    m_impl->isPrint = (DT_RASDISPLAY != GetDeviceCaps(prtDC, TECHNOLOGY));
    if (m_impl->isPrint)
    {
        m_impl->xform.setWndSize(GetDeviceCaps(prtDC, HORZRES),
            GetDeviceCaps(prtDC, VERTRES));
    }
    else
    {
        m_attribDC = NULL;
    }

    if (m_impl->isPrint
        || (buffered && !hasCachedBitmap())
        || ERROR == ::GetClipBox(hdc, &m_impl->clipBox0)
        || ::IsRectEmpty(&m_impl->clipBox0))
    {
        ::SetRect(&m_impl->clipBox0, 0, 0, xf().getWidth(), xf().getHeight());
    }
    ::CopyRect(&m_impl->clipBox, &m_impl->clipBox0);
    m_impl->rectDraw.set(m_impl->clipBox.left, m_impl->clipBox.top,
        m_impl->clipBox.right, m_impl->clipBox.bottom);
    m_impl->rectDraw.inflate(GiGraphicsImpl::CLIP_INFLATE);
    m_impl->rectDrawM = m_impl->rectDraw * xf().displayToModel();
    m_impl->rectDrawMaxM = Box2d(0, 0, xf().getWidth(), xf().getHeight()) * xf().displayToModel();
    m_impl->rectDrawW = m_impl->rectDrawM * xf().modelToWorld();
    m_impl->rectDrawMaxW = m_impl->rectDrawMaxM * xf().modelToWorld();

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

_GEOM_END