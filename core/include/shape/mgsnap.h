//! \file mgsnap.h
//! \brief 定义图形特征点捕捉器接口 MgSnap
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGSNAP_H_
#define __GEOMETRY_MGSNAP_H_

#include <mgshape.h>

struct MgMotion;

//! 特征点类型
typedef enum {
    kSnapNone,
    kSnapSameX,
    kSnapSameY,
    kSnapGridX,
    kSnapGridY,
    kSnapPoint,
} MgSnapType;

//! 图形特征点捕捉器接口
/*! \ingroup CORE_COMMAND
    \interface MgSnap
    \see MgCommandManager
*/
struct MgSnap {
    //! 显示捕捉提示线
    virtual bool drawSnap(const MgMotion* sender, GiGraphics* gs) = 0;
    
    //! 根据当前点捕捉新的坐标
    virtual Point2d snapPoint(const MgMotion* sender, const MgShape* shape,
                              int hotHandle, int ignoreHandle = -1,
                              const int* ignoreids = NULL) = 0;
    
    //! 返回捕捉到的特征点类型, >=kSnapPoint
    virtual int getSnappedType() = 0;
    
    //! 得到捕捉到的特征点坐标和原始参考坐标、捕捉坐标
    virtual int getSnappedPoint(Point2d& fromPt, Point2d& toPt) = 0;
    
    //! 得到捕捉到的图形、控制点序号、源图形上匹配的控制点序号
    virtual bool getSnappedHandle(int& shapeid, int& handleIndex, int& handleIndexSrc) = 0;
    
    //! 清除捕捉结果
    virtual void clearSnap() = 0;
};

#endif // __GEOMETRY_MGSNAP_H_
