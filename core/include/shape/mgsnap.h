//! \file mgsnap.h
//! \brief 定义图形特征点捕捉器接口 MgSnap
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGSNAP_H_
#define __GEOMETRY_MGSNAP_H_

#include <mgshape.h>

struct MgMotion;

//! 图形特征点捕捉器接口
/*! \ingroup GEOM_SHAPE
    \interface MgSnap
    \see MgCommandManager
*/
struct MgSnap {
    //! 显示捕捉提示线
    virtual bool draw(const MgMotion* sender, GiGraphics* gs) = 0;
    
    //! 根据当前点捕捉新的坐标
    virtual Point2d snapPoint(const MgMotion* sender, MgShape* hotShape, int hotHandle) = 0;
    
    //! 返回捕捉到的特征点类型
    virtual int getSnappedType() = 0;
};

#endif // __GEOMETRY_MGSNAP_H_
