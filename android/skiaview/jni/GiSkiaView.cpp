// GiSkiaView.cpp
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "GiSkiaView.h"
#include "GiCmdAndr.h"
#include <mgshapest.h>
#include <list>
#include <mgstoragebs.h>

GiSkiaView::GiSkiaView(GiCanvasBase* canvas) : _canvas(canvas)
{
    _shapes = new MgShapesT<std::list<MgShape*> >;
    _cmdc = new GiCmdController;
}

GiSkiaView::~GiSkiaView()
{
    delete _cmdc;
    if (_shapes)
        _shapes->release();
}

bool GiSkiaView::saveShapes(MgStorageBase* s)
{
    return s && _shapes && _shapes->save(s);
}

bool GiSkiaView::loadShapes(MgStorageBase* s)
{
    if (_shapes && !s) {
        _shapes->clear();
        return true;
    }
    return _shapes && s && _shapes->load(s);
}

int GiSkiaView::getWidth() const
{
    return _canvas->xf().getWidth();
}

int GiSkiaView::getHeight() const
{
    return _canvas->xf().getHeight();
}

void GiSkiaView::onSize(int width, int height)
{
    _canvas->xf().setWndSize(width, height);
}

bool GiSkiaView::onDraw(GiCanvasBase& canvas)
{
	return _shapes && _shapes->draw(canvas.gs()) > 0;
}

bool GiSkiaView::onDynDraw(GiCanvasBase& canvas)
{
	return false;
}

#include <testgraph/RandomShape.cpp>

int GiSkiaView::addTestingShapes()
{
	RandomParam param;
    UInt32 n = _shapes->getShapeCount();

	RandomParam::init();
	param.initShapes(_shapes);

	if (n == 0) {
		_canvas->xf().zoomTo(_shapes->getExtent() * _canvas->xf().modelToWorld());
		_canvas->xf().zoomByFactor(1.5f);
	}

	return _shapes->getShapeCount() - n;
}
