#include "giandroid.h"

_GEOM_BEGIN

class GiAndroidImpl
{
public:
    void*           convas;
    GiColor         bkColor;

    GiAndroidImpl() : convas(NULL), bkColor(GiColor::White())
    {
    }
};

GiAndroid::GiAndroid(GiTransform& xform)
    : GiGraphics(xform)
{
    m_draw = new GiAndroidImpl();
}

GiAndroid::GiAndroid(const GiAndroid& src)
    : GiGraphics(src)
{
    m_draw = new GiAndroidImpl();
}

GiAndroid::~GiAndroid()
{
    delete m_draw;
}

GiAndroid& GiAndroid::operator=(const GiAndroid& src)
{
    GiGraphics::operator=(src);
    return *this;
}

bool GiAndroid::beginPaint(void* convas, bool buffered, bool overlay)
{
    if (isDrawing() || convas == NULL)
        return false;

    GiGraphics::beginPaint();
    m_draw->convas = convas;

    return true;
}

void GiAndroid::endPaint(bool draw)
{
    if (isDrawing())
    {
        m_draw->convas = NULL;
        GiGraphics::endPaint();
    }
}

void GiAndroid::clearWnd()
{
}

bool GiAndroid::drawCachedBitmap(int x, int y, bool secondBmp)
{
    return false;
}

bool GiAndroid::drawCachedBitmap2(const GiGraphics* p, bool secondBmp)
{
    return false;
}

void GiAndroid::saveCachedBitmap(bool secondBmp)
{
}

bool GiAndroid::hasCachedBitmap(bool secondBmp) const
{
    return false;
}

void GiAndroid::clearCachedBitmap()
{
}

bool GiAndroid::isBufferedDrawing() const
{
    return false;
}

bool GiAndroid::setClipBox(const RECT* prc)
{
    return false;
}

bool GiAndroid::setClipWorld(const Box2d& rectWorld)
{
    return false;
}

GiColor GiAndroid::getBkColor() const
{
    return m_draw->bkColor;
}

GiColor GiAndroid::setBkColor(const GiColor& color)
{
    GiColor old(m_draw->bkColor);
    m_draw->bkColor = color;
    return old;
}

GiColor GiAndroid::getNearestColor(const GiColor& color) const
{
    return color;
}

const GiContext* GiAndroid::getCurrentContext() const
{
    return NULL;
}

bool GiAndroid::rawLine(const GiContext* ctx, 
                         int x1, int y1, int x2, int y2)
{
    return false;
}

bool GiAndroid::rawPolyline(const GiContext* ctx, 
                             const POINT* lppt, int count)
{
    return false;
}

bool GiAndroid::rawPolyBezier(const GiContext* ctx, 
                               const POINT* lppt, int count)
{
    return false;
}

bool GiAndroid::rawPolygon(const GiContext* ctx, 
                            const POINT* lppt, int count)
{
    return false;
}

bool GiAndroid::rawRect(const GiContext* ctx, 
                         int x, int y, int w, int h)
{
    return false;
}

bool GiAndroid::rawEllipse(const GiContext* ctx, 
                            int x, int y, int w, int h)
{
    return false;
}

bool GiAndroid::rawPolyDraw(const GiContext* ctx, 
                             int count, const POINT* lppt, const UInt8* types)
{
    return false;
}

bool GiAndroid::rawBeginPath()
{
    return false;
}

bool GiAndroid::rawEndPath(const GiContext* ctx, bool fill)
{
    return false;
}

bool GiAndroid::rawMoveTo(int x, int y)
{
    return false;
}

bool GiAndroid::rawLineTo(int x, int y)
{
    return false;
}

bool GiAndroid::rawPolyBezierTo(const POINT* lppt, int count)
{
    return false;
}

bool GiAndroid::rawCloseFigure()
{
    return false;
}

_GEOM_END