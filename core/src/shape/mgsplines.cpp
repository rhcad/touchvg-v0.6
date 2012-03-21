// mgsplines.cpp: 实现三次参数样条曲线类 MgSplines
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#include "mgbasicsp.h"
#include <_mgshape.h>
#include <mgnear.h>
#include <mgcurv.h>

MG_IMPLEMENT_CREATE(MgSplines)

MgSplines::MgSplines() : _knotVectors(NULL), _bzcount(0)
{
}

MgSplines::~MgSplines()
{
    if (_knotVectors)
        delete[] _knotVectors;
}

void MgSplines::_update()
{
    __super::_update();

    if (_bzcount < _count)
    {
        if (_knotVectors)
            delete[] _knotVectors;
        _bzcount = _maxCount;
        _knotVectors = new Vector2d[_bzcount];
    }

    mgCubicSplines(_count, _points, _knotVectors);
    mgCubicSplinesBox(_extent, _count, _points, _knotVectors);
}

double MgSplines::_hitTest(const Point2d& pt, double tol, 
                           Point2d& ptNear, Int32& segment) const
{
    return mgCubicSplinesHit(_count, _points, _knotVectors, _closed, 
        pt, tol, ptNear, segment);
}

bool MgSplines::_draw(GiGraphics& gs, const GiContext& ctx) const
{
    bool ret = false;

    if (_closed)
    {
        ret = gs.drawClosedSplines(&ctx, _count, _points, _knotVectors);
    }
    else
    {
        ret = gs.drawSplines(&ctx, _count, _points, _knotVectors);
    }

    return __super::_draw(gs, ctx) || ret;
}
