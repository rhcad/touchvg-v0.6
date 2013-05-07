//! \file mgshapedoc.h
//! \brief 定义图形文档 MgShapeDoc
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_SHAPEDOC_H_
#define __GEOMETRY_SHAPEDOC_H_

#include <mgshapes.h>
#include <gicontxt.h>
#include <mgmat.h>

//! 读写锁定数据类
/*! \ingroup GEOM_SHAPE
 */
class MgLockRW
{
public:
    MgLockRW();
    bool lock(bool forWrite, int timeout = 200);
    long unlock(bool forWrite);
    
    bool firstLocked();
    bool lockedForRead();
    bool lockedForWrite();
    
    int getEditFlags() { return _editFlags; }
    void setEditFlags(int flags) {
        _editFlags = flags ? (_editFlags | flags) : 0;
    }
    
private:
    volatile long _counts[3];
    int     _editFlags;
};

//! 图形文档
/*! \ingroup GEOM_SHAPE
 */
class MgShapeDoc : public MgObject
{
public:
    //! 返回本对象的类型
    static int Type() { return 8; }
    
    //! 复制出一个新图形文档对象
    MgShapeDoc* cloneDoc() const { return (MgShapeDoc*)clone(); }
    
    //! 创建图形文档对象
    static MgShapeDoc* create();
    
    //! 保存图形
    bool save(MgStorage* s, int startIndex = 0) const;
    
    //! 加载图形
    bool load(MgStorage* s, bool addOnly = false);
    
    //! 删除所有图形
    void clear();
    
    //! 显示所有图形
    int draw(GiGraphics& gs) const;
    
    //! 返回图形范围
    Box2d getExtent() const;
    
    //! 返回图形总数
    int getShapeCount() const;
    
    //! 返回当前图形列表
    MgShapes* getCurrentShapes() const;
    
    //! 返回新图形的图形属性
    GiContext* context() { return &_context; }
    
    //! 模型变换矩阵
    Matrix2d& modelTransform() { return _xf; }
    
    //! 得到页面范围的世界坐标
    Box2d getPageRectW() const { return _rectW; }
    
    //! 得到显示比例
    float getViewScale() const { return _viewScale; }
    
    //! 设置页面范围的世界坐标
    void setPageRectW(const Box2d& rectW, float viewScale);
    
    //! 返回改变计数
    int getChangeCount() const { return _changeCount; }
    
    //! 改变完成后更新改变计数
    void afterChanged();
    
    //! 得到锁定数据对象以便读写锁定
    virtual MgLockRW* getLockData() { return &_lock; }
    
public:
    virtual MgObject* clone() const;
    virtual void copy(const MgObject& src);
    virtual void release();
    virtual bool equals(const MgObject& src) const;
    virtual int getType() const { return Type(); }
    virtual bool isKindOf(int type) const { return type == Type(); }
    
protected:
    MgShapeDoc();
    virtual ~MgShapeDoc();
    
protected:
    MgShapes*   _shapes;
    GiContext   _context;
    Matrix2d    _xf;
    Box2d       _rectW;
    float       _viewScale;
    long        _changeCount;
    MgLockRW    _lock;
};

//! 图形列表锁定辅助类
/*! \ingroup GEOM_SHAPE
 */
class MgShapesLock
{
    int     _mode;
public:
    MgShapeDoc*   doc;
    
    enum { Unknown = 0x100, ReadOnly = 0,
        Add = 0x1, Remove = 0x2, Edit = 0x4, Load = 0x8 };
    MgShapesLock(MgShapeDoc* doc, int flags, int timeout = 200);
    ~MgShapesLock();
    
    bool locked();
    static bool lockedForRead(MgShapeDoc* doc);
    static bool lockedForWrite(MgShapeDoc* doc);
    
    int getEditFlags() { return doc->getLockData()->getEditFlags(); }
    void resetEditFlags() { doc->getLockData()->setEditFlags(0); }
    
#ifndef SWIG
    typedef void (*ShapesLocked)(MgShapeDoc* doc, void* obj, bool locked);
    static void registerObserver(ShapesLocked func, void* obj);
    static void unregisterObserver(ShapesLocked func, void* obj);
#endif
};

//! 动态图形锁定辅助类
/*! \ingroup GEOM_SHAPE
 */
class MgDynShapeLock
{
    int     _mode;
public:
    MgDynShapeLock(bool forWrite = true, int timeout = 200);
    ~MgDynShapeLock();
    
    bool locked();
    static bool lockedForRead();
    static bool lockedForWrite();
};

#endif // __GEOMETRY_SHAPEDOC_H_
