//! \file mgshapest.h
//! \brief 定义图形列表模板类 MgShapesT
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGSHAPES_TEMPL_H_
#define __GEOMETRY_MGSHAPES_TEMPL_H_

#include <mgshapes.h>
#include <mgstorage.h>
#include <gigraph.h>

MgShape* mgCreateShape(UInt32 type);

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
        , _scale(1), _changeCount(0)
    {
    }

    ~MgShapesT()
    {
        clear();
        delete _context;
    }

    static UInt32 Type() { return 8; }
    UInt32 getType() const { return Type(); }

    bool isKindOf(UInt32 type) const
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
        *p->_context = *_context;
        return p;
    }

    void copy(const MgObject& src)
    {
        if (src.isKindOf(Type())) {
            const ThisClass& _src = (const ThisClass&)src;
            if (&_src != this) {
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
        if (p)
        {
            p->setParent(this, getNewID(src.getID()));
            _shapes.push_back(p);
        }
        return p;
    }
    
    MgShape* removeShape(UInt32 nID)
    {
        for (iterator it = _shapes.begin(); it != _shapes.end(); ++it)
        {
            MgShape* shape = *it;
            if (shape->getID() == nID) {
                _shapes.erase(it);
                return shape;
            }
        }
        return NULL;
    }

    UInt32 getShapeCount() const
    {
        return _shapes.size();
    }

    MgShape* getFirstShape(void*& it) const
    {
        it = (void*)0;
        _it = _shapes.begin();
        return _shapes.empty() ? NULL : _shapes.front();
    }
    
    MgShape* getNextShape(void*& it) const
    {
        int i = (int)it;
        if (0 == i && _it != _shapes.end()) {
            _it++;
            if (_it != _shapes.end())
                return *_it;
        }
        return NULL;
    }
    
    MgShape* getLastShape() const
    {
        return _shapes.empty() ? NULL : _shapes.back();
    }

    MgShape* findShape(UInt32 nID) const
    {
        for (const_iterator it = _shapes.begin(); it != _shapes.end(); ++it)
        {
            if ((*it)->getID() == nID)
                return *it;
        }
        return NULL;
    }

    Box2d getExtent() const
    {
        Box2d extent;
        for (const_iterator it = _shapes.begin(); it != _shapes.end(); ++it)
        {
            extent.unionWith((*it)->shape()->getExtent());
        }

        return extent;
    }

    MgShape* hitTest(const Box2d& limits, Point2d& nearpt, Int32& segment) const
    {
        MgShape* retshape = NULL;
        float distMin = limits.width();

        for (const_iterator it = _shapes.begin(); it != _shapes.end(); ++it)
        {
            const MgBaseShape* shape = (*it)->shape();

            if (shape->getExtent().isIntersect(limits))
            {
                Point2d tmpNear;
                Int32   tmpSegment;
                float  dist = shape->hitTest(limits.center(), 
                    limits.width() / 2, tmpNear, tmpSegment);

                if (distMin > dist) {
                    distMin = dist;
                    segment = tmpSegment;
                    nearpt = tmpNear;
                    retshape = *it;
                }
            }
        }

        return retshape;
    }

    int draw(GiGraphics& gs, const GiContext *ctx = NULL) const
    {
        Box2d clip(gs.getClipModel());
        int count = 0;
        
        for (const_iterator it = _shapes.begin(); it != _shapes.end(); ++it)
        {
            if ((*it)->shape()->getExtent().isIntersect(clip)) {
                if ((*it)->draw(gs, ctx))
                    count++;
            }
        }
        
        return count;
    }
    
    UInt32 getChangeCount()
    {
        return (UInt32)_changeCount;
    }
    
    void afterChanged()
    {
        giInterlockedIncrement(&_changeCount);
    }
    
    bool save(MgStorage* s, UInt32 startIndex = 0) const
    {
        bool ret = false;
        Box2d rect;
        UInt32 index = 0;
        
        if (_context) {
            if (!s->writeNode("shapedoc", -1, false))
                return false;
            
            s->writeFloatArray("transform", &_xf.m11, 6);
            s->writeFloat("scale", _scale);
            s->writeFloatArray("center", &_centerW.x, 2);
            rect = getExtent();
            s->writeFloatArray("extent", &rect.xmin, 4);
            s->writeUInt32("count", 1);
        }
        
        if (s->writeNode("shapes", _context ? 0 : -1, false)) {
            ret = true;
            rect = getExtent();
            s->writeFloatArray("extent", &rect.xmin, 4);
            
            s->writeUInt32("count", _shapes.size() - (UInt32)startIndex);
            for (const_iterator it = _shapes.begin(); ret && it != _shapes.end(); ++it, ++index)
            {
                if (index < startIndex)
                    continue;
                ret = s->writeNode("shape", index - startIndex, false);
                if (ret) {
                    s->writeUInt32("type", (*it)->getType() % 10000);
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
            _scale = s->readFloat("scale", _scale);
            s->readFloatArray("center", &_centerW.x, 2);
            s->readFloatArray("extent", &rect.xmin, 4);
            s->readUInt32("count");
        }
        
        if (s->readNode("shapes", _context ? 0 : -1, false)) {
            ret = true;
            s->readFloatArray("extent", &rect.xmin, 4);
            s->readUInt32("count");
            
            if (!addOnly)
                clear();
            
            while (ret && s->readNode("shape", index, false)) {
                UInt32 type = s->readUInt32("type");
                UInt32 id = s->readUInt32("id");
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
    
    float getViewScale() const
    {
        return _scale;
    }
    
    Point2d getViewCenterW() const
    {
        return _centerW;
    }
    
    void setZoomState(float scale, const Point2d& centerW)
    {
        _scale = scale;
        _centerW = centerW;
    }
    
    virtual MgLockRW* getLockData()
    {
        return &_lock;
    }

private:
    UInt32 getNewID(UInt32 nID)
    {
        if (0 == nID || findShape(nID)) {
            nID = 1;
            if (!_shapes.empty())
                nID = _shapes.back()->getID() + 1;
            while (findShape(nID))
                nID++;
        }
        return nID;
    }

protected:
    Container               _shapes;
    mutable const_iterator  _it;
    ContextT*               _context;
    Matrix2d                _xf;
    float                   _scale;
    Point2d                 _centerW;
    long                    _changeCount;
    MgLockRW                _lock;
};

#endif // __GEOMETRY_MGSHAPES_TEMPL_H_
