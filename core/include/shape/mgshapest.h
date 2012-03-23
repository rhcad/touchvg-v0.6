//! \file mgshapest.h
//! \brief 定义图形列表模板类 MgShapesT
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_MGSHAPES_TEMPL_H_
#define __GEOMETRY_MGSHAPES_TEMPL_H_

#include <mgshapes.h>
#include <gigraph.h>

//! 图形列表模板类
/*! \ingroup _GEOM_SHAPE_
    \param Container 包含(MgShape*)的vector、list等容器类型
*/
template <typename Container /*=std::vector<MgShape*>*/ >
class MgShapesT : public MgShapes
{
    typedef MgShapesT<Container> ThisClass;
    typedef typename Container::const_iterator const_iterator;
public:
    MgShapesT()
    {
    }

    ~MgShapesT()
    {
        clear();
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
        ThisClass *p = new ThisClass;
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

    MgShape* hitTest(const Box2d& limits, Point2d& ptNear, Int32& segment) const
    {
        MgShape* retshape = NULL;
        double distMin = limits.width();

        for (const_iterator it = _shapes.begin(); it != _shapes.end(); ++it)
        {
            const MgBaseShape* shape = (*it)->shape();

            if (shape->getExtent().isIntersect(limits))
            {
                Point2d tmpNear;
                Int32   tmpSegment;
                double  dist = shape->hitTest(limits.center(), 
                    limits.width() / 2, tmpNear, tmpSegment);

                if (distMin > dist) {
                    distMin = dist;
                    segment = tmpSegment;
                    ptNear = tmpNear;
                    retshape = *it;
                }
            }
        }

        return retshape;
    }

    void draw(GiGraphics& gs, const GiContext *ctx = NULL) const
    {
        Box2d clip(gs.getClipModel());
        
        for (const_iterator it = _shapes.begin(); it != _shapes.end(); ++it)
        {
            if ((*it)->shape()->getExtent().isIntersect(clip))
                (*it)->draw(gs, ctx);
        }
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
};

#endif // __GEOMETRY_MGSHAPES_TEMPL_H_
