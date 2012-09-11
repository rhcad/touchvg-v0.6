//! \file mggrid.h
//! \brief 定义网格图形类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_GRIDSHAPE_H_
#define __GEOMETRY_GRIDSHAPE_H_

#include "mgbasicsp.h"

//! 网格图形类
/*! \ingroup GEOM_SHAPE
*/
class MgGrid : public MgBaseRect
{
    MG_INHERIT_CREATE(MgGrid, MgBaseRect, 20)
public:
    virtual int snap(Point2d& pnt, float& distx, float& disty);
    virtual void setRotateDisnable(bool disnable);

protected:
    void _copy(const MgGrid& src);
    bool _equals(const MgGrid& src) const;
    void _clear();
    bool _save(MgStorage* s) const;
    bool _load(MgStorage* s);
    UInt32 _getHandleCount() const { return 4; }

protected:
};

#endif // __GEOMETRY_GRIDSHAPE_H_
