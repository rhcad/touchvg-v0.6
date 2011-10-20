//! \file mgshapes.h
//! \brief 定义图形列表接口 MgShapes
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_MGSHAPES_H_
#define __GEOMETRY_MGSHAPES_H_

#include <mgshape.h>

//! 图形列表接口
/*! \ingroup _GEOM_SHAPE_
*/
struct MgShapes
{
    virtual UInt32 getShapeCount() const = 0;
    virtual GiShape* getFirstShape(void*& it) const = 0;
    virtual GiShape* getNextShape(void*& it) const = 0;
    virtual GiShape* findShape(UInt32 id) const = 0;
    virtual Box2d getExtent() const = 0;

    virtual GiShape* hitTest(const Box2d& limits, Point2d& ptNear, Int32& segment) const = 0;
    virtual void draw(GiGraphics& gs, const GiContext *ctx = NULL) const = 0;

    virtual void release() = 0;
    virtual void clear() = 0;
    virtual GiShape* addShape(const GiShape& src) = 0;
};

#endif // __GEOMETRY_MGSHAPES_H_
