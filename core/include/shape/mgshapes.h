//! \file mgshapes.h
//! \brief 定义图形列表接口 MgShapes
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#ifndef __GEOMETRY_MGSHAPES_H_
#define __GEOMETRY_MGSHAPES_H_

#include <mgshape.h>

//! 图形列表接口
/*! \ingroup GEOM_SHAPE
    \interface MgShapes
*/
struct MgShapes : public MgObject
{
    static UInt32 Type() { return 1; }

    virtual UInt32 getShapeCount() const = 0;
    virtual MgShape* getFirstShape(void*& it) const = 0;
    virtual MgShape* getNextShape(void*& it) const = 0;
    virtual MgShape* findShape(UInt32 nID) const = 0;
    virtual Box2d getExtent() const = 0;

    virtual MgShape* hitTest(const Box2d& limits, Point2d& nearpt, Int32& segment) const = 0;
    virtual bool draw(GiGraphics& gs, const GiContext *ctx = NULL) const = 0;
    virtual bool save(MgStorage* s) const = 0;
    virtual bool load(MgStorage* s) = 0;

    //! 删除所有图形
    virtual void clear() = 0;
    
    //! 复制出新图形并添加到图形列表中
    virtual MgShape* addShape(const MgShape& src) = 0;
    
    //! 移除一个图形，由调用者删除图形对象
    virtual MgShape* removeShape(UInt32 nID) = 0;

    //! 返回新图形的图形属性
    virtual GiContext* context() = 0;
};

#endif // __GEOMETRY_MGSHAPES_H_
