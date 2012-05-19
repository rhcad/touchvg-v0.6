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
            p->setParent(this, getNewID());
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
    
    bool save(MgStorage* s) const
    {
        bool ret = false;
        Box2d rect;
        
        if (s->writeNode("shapes", false)) {
            ret = true;
            s->writeUInt32("count", _shapes.size());
            
            rect = getExtent();
            s->writeFloatArray("extent", &rect.xmin, 4);
            
            for (const_iterator it = _shapes.begin(); ret && it != _shapes.end(); ++it)
            {
                ret = s->writeNode("shape", false);
                if (ret) {
                    s->writeUInt32("type", (*it)->getType() % 10000);
                    s->writeUInt32("id", (*it)->getID());
                    
                    rect = (*it)->shape()->getExtent();
                    s->writeFloatArray("extent", &rect.xmin, 4);
                    
                    ret = (*it)->save(s) && s->writeNode("shape", true);
                }
            }
            s->writeNode("shapes", true);
        }
        
        return ret;
    }
    
    bool load(MgStorage* s)
    {
        MgShapesLock locker(this);
        bool ret = false;
        Box2d rect;
        
        if (s->readNode("shapes", false)) {
            ret = true;
            s->readUInt32("count");
            s->readFloatArray("extent", &rect.xmin, 4);
            
            clear();
            while (ret && s->readNode("shape", false)) {
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
                s->readNode("shape", true);
            }
            s->readNode("shapes", true);
        }
        
        return ret;
    }

    GiContext* context()
    {
        return _context;
    }

private:
    UInt32 getNewID()
    {
        UInt32 nID = 1;

        if (!_shapes.empty())
            nID = _shapes.back()->getID() + 1;
        while (findShape(nID))
            nID++;

        return nID;
    }

protected:
    Container               _shapes;
    mutable const_iterator  _it;
    ContextT*               _context;
};

#endif // __GEOMETRY_MGSHAPES_TEMPL_H_
