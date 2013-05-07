//! \file mgshapes.h
//! \brief 定义图形列表接口 MgShapes
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGSHAPES_H_
#define __GEOMETRY_MGSHAPES_H_

#include <mgbox.h>
#include "mgobject.h"

class MgShape;
struct MgStorage;
class MgShapes;
class GiGraphics;
class GiContext;

//! 创建图形列表
MgShapes* mgCreateShapes(MgObject* owner = NULL, int index = -1);

//! 图形列表接口
/*! \ingroup GEOM_SHAPE
    \see MgShapeIterator, mgCreateShapes
*/
class MgShapes : public MgObject
{
public:
    //! 返回本对象的类型
    static int Type() { return 1; }
    
    //! 复制出一个新图形列表对象
    MgShapes* cloneShapes() const { return (MgShapes*)clone(); }

#ifndef SWIG
    virtual MgShape* getFirstShape(void*& it) const = 0;
    virtual MgShape* getNextShape(void*& it) const = 0;
    virtual void freeIterator(void*& it) = 0;
    typedef bool (*Filter)(const MgShape*);
#endif

    virtual int getShapeCount() const = 0;
    virtual MgShape* getHeadShape() const = 0;
    virtual MgShape* getLastShape() const = 0;
    virtual MgShape* findShape(int sid) const = 0;
    virtual MgShape* findShapeByTag(int tag) const = 0;
    virtual MgShape* findShapeByType(int type) const = 0;
    virtual Box2d getExtent() const = 0;
    
    virtual MgShape* hitTest(const Box2d& limits, Point2d& nearpt
#ifndef SWIG
        , int* segment = NULL, Filter filter = NULL) const = 0;
#else
        ) const = 0;
#endif
    
    virtual int draw(GiGraphics& gs, const GiContext *ctx = NULL) const = 0;
    virtual int dyndraw(int mode, GiGraphics& gs, const GiContext *ctx, int segment) const = 0;

    virtual bool save(MgStorage* s, int startIndex = 0) const = 0;
    virtual bool load(MgStorage* s, bool addOnly = false) = 0;
    
    //! 删除所有图形
    virtual void clear() = 0;

    //! 复制(深拷贝)每一个图形
    virtual void copyShapes(const MgShapes* src) = 0;
    
    //! 复制出新图形并添加到图形列表中
    virtual MgShape* addShape(const MgShape& src) = 0;

    //! 添加一个指定类型的新图形
    virtual MgShape* addShapeByType(int type) = 0;
    
    //! 移除一个图形，由调用者删除图形对象
    virtual MgShape* removeShape(int sid) = 0;

    //! 将一个图形移到另一个图形列表
    virtual MgShape* moveTo(int sid, MgShapes* dest) = 0;

    //! 将所有图形移到另一个图形列表
    virtual void moveAllShapesTo(MgShapes* dest) = 0;
    
    //! 移动图形到最后，以便显示在最前面
    virtual bool bringToFront(int sid) = 0;

    //! 返回拥有者对象
    virtual MgObject* getOwner() const = 0;

protected:
    virtual ~MgShapes() {}
};

//! 遍历图形的辅助类
/*! \ingroup GEOM_SHAPE
*/
class MgShapeIterator
{
public:
    MgShapeIterator(const MgShapes* shapes) : _s(shapes), _it(NULL) {}
    ~MgShapeIterator() { freeIterator(); }

    MgShape* getFirstShape() { return _s->getFirstShape(_it); }
    MgShape* getNextShape() { return _s->getNextShape(_it); }
    void freeIterator() { _s->getNextShape(_it); }

private:
    MgShapeIterator();
    const MgShapes* _s;
    void* _it;
};

#endif // __GEOMETRY_MGSHAPES_H_
