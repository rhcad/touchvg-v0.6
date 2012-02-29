// GiGraphIos.cpp
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#include "GiGraphIos.h"

class GiGraphIosImpl
{
public:
    CGContextRef    context;
    GiColor         bkColor;

    GiGraphIosImpl() : context(NULL), bkColor(GiColor::White())
    {
    }
};

GiGraphIos::GiGraphIos(GiTransform& xform)
    : GiGraphics(xform)
{
    m_draw = new GiGraphIosImpl();
}

GiGraphIos::GiGraphIos(const GiGraphIos& src)
    : GiGraphics(src)
{
    m_draw = new GiGraphIosImpl();
}

GiGraphIos::~GiGraphIos()
{
    delete m_draw;
}

GiGraphIos& GiGraphIos::operator=(const GiGraphIos& src)
{
    GiGraphics::operator=(src);
    return *this;
}

bool GiGraphIos::beginPaint(CGContextRef context, bool buffered, bool overlay)
{
    if (isDrawing() || context == NULL)
        return false;

    GiGraphics::beginPaint();
    m_draw->context = context;

    return true;
}

void GiGraphIos::endPaint(bool draw)
{
    if (isDrawing())
    {
        m_draw->context = NULL;
        GiGraphics::endPaint();
    }
}

void GiGraphIos::clearWnd()
{
}

bool GiGraphIos::drawCachedBitmap(int x, int y, bool secondBmp)
{
    return false;
}

bool GiGraphIos::drawCachedBitmap2(const GiGraphics* p, bool secondBmp)
{
    return false;
}

void GiGraphIos::saveCachedBitmap(bool secondBmp)
{
}

bool GiGraphIos::hasCachedBitmap(bool secondBmp) const
{
    return false;
}

void GiGraphIos::clearCachedBitmap()
{
}

bool GiGraphIos::isBufferedDrawing() const
{
    return false;
}

bool GiGraphIos::setClipBox(const RECT* prc)
{
    return false;
}

bool GiGraphIos::setClipWorld(const Box2d& rectWorld)
{
    return false;
}

GiColor GiGraphIos::getBkColor() const
{
    return m_draw->bkColor;
}

GiColor GiGraphIos::setBkColor(const GiColor& color)
{
    GiColor old(m_draw->bkColor);
    m_draw->bkColor = color;
    return old;
}

GiColor GiGraphIos::getNearestColor(const GiColor& color) const
{
    return color;
}

const GiContext* GiGraphIos::getCurrentContext() const
{
    return NULL;
}

bool GiGraphIos::rawLine(const GiContext* ctx, 
                         int x1, int y1, int x2, int y2)
{
    return false;
}

bool GiGraphIos::rawPolyline(const GiContext* ctx, 
                             const POINT* lppt, int count)
{
    return false;
}

bool GiGraphIos::rawPolyBezier(const GiContext* ctx, 
                               const POINT* lppt, int count)
{
    return false;
}

bool GiGraphIos::rawPolygon(const GiContext* ctx, 
                            const POINT* lppt, int count)
{
    return false;
}

bool GiGraphIos::rawRect(const GiContext* ctx, 
                         int x, int y, int w, int h)
{
    return false;
}

bool GiGraphIos::rawEllipse(const GiContext* ctx, 
                            int x, int y, int w, int h)
{
    return false;
}

bool GiGraphIos::rawPolyDraw(const GiContext* ctx, 
                             int count, const POINT* lppt, const UInt8* types)
{
    return false;
}

bool GiGraphIos::rawBeginPath()
{
    return false;
}

bool GiGraphIos::rawEndPath(const GiContext* ctx, bool fill)
{
    return false;
}

bool GiGraphIos::rawMoveTo(int x, int y)
{
    return false;
}

bool GiGraphIos::rawLineTo(int x, int y)
{
    return false;
}

bool GiGraphIos::rawPolyBezierTo(const POINT* lppt, int count)
{
    return false;
}

bool GiGraphIos::rawCloseFigure()
{
    return false;
}
