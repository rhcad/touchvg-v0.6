//! \file mgshapedoc.cpp
//! \brief 实现图形文档 MgShapeDoc
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include <mgshapedoc.h>
#include <mgstorage.h>
#include <gidef.h>

MgShapeDoc::MgShapeDoc() : _viewScale(0), _changeCount(0)
{
    _shapes = mgCreateShapes(this, 0);
}

MgShapeDoc::~MgShapeDoc()
{
    _shapes->release();
}

MgShapeDoc* MgShapeDoc::create()
{
    return new MgShapeDoc();
}

void MgShapeDoc::release()
{
    delete this;
}

MgObject* MgShapeDoc::clone() const
{
    MgObject* p = new MgShapeDoc();
    p->copy(*this);
    return p;
}

void MgShapeDoc::copy(const MgObject& src)
{
    if (src.isKindOf(Type())) {
        const MgShapeDoc& doc = (const MgShapeDoc&)src;
        _xf = doc._xf;
        _rectW = doc._rectW;
        _viewScale = doc._viewScale;
    }
}

bool MgShapeDoc::equals(const MgObject& src) const
{
    if (src.isKindOf(Type())) {
        const MgShapeDoc& doc = (const MgShapeDoc&)src;
        return _xf == doc._xf && _shapes->equals(*(doc._shapes));
    }

    return false;
}

void MgShapeDoc::setPageRectW(const Box2d& rectW, float viewScale)
{
    _rectW = rectW;
    _viewScale = viewScale;
}

void MgShapeDoc::clear()
{
    _shapes->clear();
}

Box2d MgShapeDoc::getExtent() const
{
    return _shapes->getExtent();
}

int MgShapeDoc::getShapeCount() const
{
    return _shapes->getShapeCount();
}

MgShapes* MgShapeDoc::getCurrentShapes() const
{
    return _shapes;
}

int MgShapeDoc::draw(GiGraphics& gs) const
{
    return _shapes->draw(gs);
}

void MgShapeDoc::afterChanged()
{
    giInterlockedIncrement(&_changeCount);
}

bool MgShapeDoc::save(MgStorage* s, int startIndex) const
{
    bool ret = false;
    Box2d rect;

    if (!s || !s->writeNode("shapedoc", -1, false))
        return false;

    s->writeFloatArray("transform", &_xf.m11, 6);
    s->writeFloatArray("zoomExtent", &_rectW.xmin, 4);
    s->writeFloat("viewScale", _viewScale);
    rect = getExtent();
    s->writeFloatArray("extent", &rect.xmin, 4);
    s->writeUInt32("count", 1);

    ret = _shapes->save(s, startIndex);

    s->writeNode("shapedoc", -1, true);

    return ret;
}

bool MgShapeDoc::load(MgStorage* s, bool addOnly)
{
    bool ret = false;
    Box2d rect;

    if (!s || !s->readNode("shapedoc", -1, false))
        return s->setError("No root node.");

    s->readFloatArray("transform", &_xf.m11, 6);
    s->readFloatArray("zoomExtent", &_rectW.xmin, 4);
    _viewScale = s->readFloat("viewScale", _viewScale);
    s->readFloatArray("extent", &rect.xmin, 4);
    s->readUInt32("count", 0);

    ret = _shapes->load(s, addOnly);

    s->readNode("shapedoc", -1, true);

    return ret;
}
