//! \file mgshapest.h
//! \brief 定义图形列表模板类 MgShapesT
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGSHAPES_TEMPL_H_
#define __GEOMETRY_MGSHAPES_TEMPL_H_

#include <mgshapes.h>
#include <mgshape.h>
#include <mgstorage.h>
#include <gigraph.h>

MgShape* mgCreateShape(int type);

//! 图形列表模板类
/*! \ingroup GEOM_SHAPE
    \param Container 包含(MgShape*)的vector、list等容器类型
    \param ContextT 图形属性的类，为 GiContext 或其子类
*/
template <typename Container, typename ContextT = GiContext>
class MgShapesT : public MgShapes
{
    typedef MgShapesT<Container, ContextT> ThisClass;
    typedef typename Container::const_iterator const_iterator;
    typedef typename Container::iterator iterator;
public:
    MgShapesT(bool hasContext = true) : _context(hasContext ? new ContextT() : NULL)
        , _viewScale(0), _changeCount(0)
    {
    }

    virtual ~MgShapesT()
    {
        clear();
        delete _context;
    }

    static int Type() { return 0x10000 | MgShapes::Type(); }
    int getType() const { return Type(); }

    bool isKindOf(int type) const
    {
        return type == Type() || type == MgShapes::Type();
    }

    void release()
    {
        delete this;
    }

    MgObject* clone() const
    {
        ThisClass *p = new ThisClass(_context != NULL);
        p->copy(*this);
        return p;
    }

    void copy(const MgObject& src)
    {
        if (src.isKindOf(Type())) {
            const ThisClass& _src = (const ThisClass&)src;
            if (&_src != this) {
                *_context = *_src._context;
                _xf = _src._xf;
                _rectW = _src._rectW;
                _viewScale = _src._viewScale;
            }
        }
    }
    
    bool equals(const MgObject& src) const
    {
        bool ret = false;

        if (src.isKindOf(Type())) {
            const ThisClass& _src = (const ThisClass&)src;
            ret = (_shapes == _src._shapes);
        }

        return ret;
    }

    void clear()
    {
        typename Container::iterator it = _shapes.begin();
        for (; it != _shapes.end(); ++it)
            (*it)->release();
        _shapes.clear();
    }

    MgShape* addShape(const MgShape& src)
    {
        MgShape* p = (MgShape*)src.clone();
        if (p) {
            p->setParent(this, getNewID(src.getID()));
            p->shape()->setFlag(kMgShapeLocked, false);
            _shapes.push_back(p);
        }
        return p;
    }
    
    MgShape* removeShape(int sid)
    {
        for (iterator it = _shapes.begin(); it != _shapes.end(); ++it) {
            MgShape* shape = *it;
            if (shape->getID() == sid) {
                if (shape->shapec()->getFlag(kMgShapeLocked)) {
                    return NULL;
                }
                _shapes.erase(it);
                return shape;
            }
        }
        return NULL;
    }
    
    bool bringToFront(int sid)
    {
        for (iterator it = _shapes.begin(); it != _shapes.end(); ++it) {
            MgShape* shape = *it;
            if (shape->getID() == sid) {
                _shapes.erase(it);
                _shapes.push_back(shape);
                return true;
            }
        }
        return false;
    }

    int getShapeCount() const
    {
        return _shapes.size();
    }

    void freeIterator(void*& it)
    {
        delete (const_iterator*)it;
        it = NULL;
    }

    MgShape* getFirstShape(void*& it) const
    {
        it = (void*)(new const_iterator(_shapes.begin()));
        return _shapes.empty() ? NULL : _shapes.front();
    }
    
    MgShape* getNextShape(void*& it) const
    {
        const_iterator* pit = (const_iterator*)it;
        if (pit && *pit != _shapes.end()) {
            ++(*pit);
            if (*pit != _shapes.end())
                return *(*pit);
        }
        return NULL;
    }
    
    MgShape* getLastShape() const
    {
        return _shapes.empty() ? NULL : _shapes.back();
    }

    MgShape* findShape(int sid) const
    {
        for (const_iterator it = _shapes.begin(); it != _shapes.end(); ++it) {
            if ((*it)->getID() == sid)
                return *it;
        }
        return NULL;
    }

    MgShape* findShapeByTag(int tag) const
    {
        for (const_iterator it = _shapes.begin(); it != _shapes.end(); ++it) {
            if ((*it)->getTag() == tag)
                return *it;
        }
        return NULL;
    }
    
    MgShape* findShapeByType(int type) const
    {
        for (const_iterator it = _shapes.begin(); it != _shapes.end(); ++it) {
            if ((*it)->shapec()->getType() == type)
                return *it;
        }
        return NULL;
    }

    Box2d getExtent() const
    {
        Box2d extent;
        for (const_iterator it = _shapes.begin(); it != _shapes.end(); ++it) {
            extent.unionWith((*it)->shape()->getExtent());
        }

        return extent;
    }
    
    MgShape* hitTest(const Box2d& limits, Point2d& nearpt, 
                     int* segment = NULL, Filter filter = NULL) const
    {
        MgShape* retshape = NULL;
        float distMin = _FLT_MAX;

        for (const_iterator it = _shapes.begin(); it != _shapes.end(); ++it) {
            const MgBaseShape* shape = (*it)->shape();
            Box2d extent(shape->getExtent());

            if (extent.isIntersect(limits) && (!filter || filter(*it))) {
                Point2d tmpNear;
                int   tmpSegment;
                float  tol = (!hasFillColor(*it) ? limits.width() / 2
                              : mgMax(extent.width(), extent.height()));
                float  dist = shape->hitTest(limits.center(), tol, tmpNear, tmpSegment);

                if (distMin > dist - _MGZERO) {     // 让末尾图形优先选中
                    distMin = dist;
                    if (segment) {
                        *segment = tmpSegment;
                    }
                    nearpt = tmpNear;
                    retshape = *it;
                }
            }
        }
        if (retshape && distMin > limits.width() && !hasFillColor(retshape)) {
            retshape = NULL;
        }

        return retshape;
    }

    int draw(GiGraphics& gs, const GiContext *ctx = NULL) const
    {
        Box2d clip(gs.getClipModel());
        int count = 0;
        
        for (const_iterator it = _shapes.begin(); it != _shapes.end(); ++it) {
            if ((*it)->shape()->getExtent().isIntersect(clip)) {
                if ((*it)->draw(0, gs, ctx))
                    count++;
            }
        }
        
        return count;
    }
    
    int getChangeCount()
    {
        return (int)_changeCount;
    }
    
    void afterChanged()
    {
        giInterlockedIncrement(&_changeCount);
    }
    
    bool save(MgStorage* s, int startIndex = 0) const
    {
        bool ret = false;
        Box2d rect;
        int index = 0;
        
        if (_context) {
            if (!s->writeNode("shapedoc", -1, false))
                return false;
            
            s->writeFloatArray("transform", &_xf.m11, 6);
            s->writeFloatArray("zoomExtent", &_rectW.xmin, 4);
            s->writeFloat("viewScale", _viewScale);
            rect = getExtent();
            s->writeFloatArray("extent", &rect.xmin, 4);
            s->writeUInt32("count", 1);
        }
        
        if (s->writeNode("shapes", _context ? 0 : -1, false)) {
            ret = true;
            rect = getExtent();
            s->writeFloatArray("extent", &rect.xmin, 4);
            
            s->writeUInt32("count", _shapes.size() - (int)startIndex);
            for (const_iterator it = _shapes.begin(); ret && it != _shapes.end(); ++it, ++index)
            {
                if (index < startIndex)
                    continue;
                ret = s->writeNode("shape", index - startIndex, false);
                if (ret) {
                    s->writeUInt32("type", (*it)->getType() & 0xFFFF);
                    s->writeUInt32("id", (*it)->getID());
                    
                    rect = (*it)->shape()->getExtent();
                    s->writeFloatArray("extent", &rect.xmin, 4);
                    
                    ret = (*it)->save(s);
                    s->writeNode("shape", index - startIndex, true);
                }
            }
            s->writeNode("shapes", _context ? 0 : -1, true);
        }
        
        if (_context) {
            s->writeNode("shapedoc", -1, true);
        }
        
        return ret;
    }
    
    bool load(MgStorage* s, bool addOnly = false)
    {
        bool ret = false;
        Box2d rect;
        int index = 0;
        
        if (_context) {
            if (!s->readNode("shapedoc", -1, false))
                return false;
            
            s->readFloatArray("transform", &_xf.m11, 6);
            s->readFloatArray("zoomExtent", &_rectW.xmin, 4);
            _viewScale = s->readFloat("viewScale", _viewScale);
            s->readFloatArray("extent", &rect.xmin, 4);
            s->readUInt32("count", 0);
        }
        
        if (s->readNode("shapes", _context ? 0 : -1, false)) {
            ret = true;
            s->readFloatArray("extent", &rect.xmin, 4);
            s->readUInt32("count", 0);
            
            if (!addOnly)
                clear();
            
            while (ret && s->readNode("shape", index, false)) {
                int type = s->readUInt32("type", 0);
                int id = s->readUInt32("id", 0);
                MgShape* shape = mgCreateShape(type);
                
                s->readFloatArray("extent", &rect.xmin, 4);
                if (shape) {
                    shape->setParent(this, id);
                    ret = shape->load(s);
                    if (ret) {
                        _shapes.push_back(shape);
                    }
                    else {
                        shape->release();
                    }
                }
                s->readNode("shape", index++, true);
            }
            s->readNode("shapes", _context ? 0 : -1, true);
        }
        
        if (_context) {
            s->readNode("shapedoc", -1, true);
        }
        
        return ret;
    }

    GiContext* context()
    {
        return _context;
    }
    
    Matrix2d& modelTransform()
    {
        return _xf;
    }
    
    Box2d getZoomRectW() const
    {
        return _rectW;
    }

    float getViewScale() const
    {
        return _viewScale;
    }
    
    void setZoomRectW(const Box2d& rectW, float viewScale)
    {
        _rectW = rectW;
        _viewScale = viewScale;
    }
    
    virtual MgLockRW* getLockData()
    {
        return &_lock;
    }

private:
    int getNewID(int sid)
    {
        if (0 == sid || findShape(sid)) {
            sid = 1;
            if (!_shapes.empty())
                sid = _shapes.back()->getID() + 1;
            while (findShape(sid))
                sid++;
        }
        return sid;
    }
    
    bool hasFillColor(const MgShape* shape) const
    {
        return shape->contextc()->hasFillColor() && shape->shapec()->isClosed();
    }

protected:
    Container               _shapes;
    ContextT*               _context;
    Matrix2d                _xf;
    Box2d                   _rectW;
    float                   _viewScale;
    long                    _changeCount;
    MgLockRW                _lock;
};

#endif // __GEOMETRY_MGSHAPES_TEMPL_H_
