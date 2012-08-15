// GiSkiaView.cpp
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "GiSkiaView.h"
#include <mgshapest.h>
#include <list>
#include <mgstoragebs.h>
#include <mgcmd.h>
#include <vector>

class MgViewProxy : public MgView
{
public:
	GiCanvasBase*	_canvas;
	MgShapes*		_shapes;
	MgMotion		_motion;
	bool			_moved;
	GiContext		_tmpContext;

	MgViewProxy(GiCanvasBase* canvas) : _canvas(canvas), _moved(false) {
		_shapes = new MgShapesT<std::list<MgShape*> >;
		_motion.view = this;
		_shapes->context()->setLineAlpha(140);
	}
	virtual ~MgViewProxy() {
		_shapes->release();
	}

    virtual MgShapes* shapes() {
    	return _shapes;
    }
    virtual GiTransform* xform() {
    	return &_canvas->xf();
    }
    virtual GiGraphics* graph() {
    	return &_canvas->gs();
    }
    virtual void regen() {
    	_canvas->clearCachedBitmap();
    	_canvas->setNeedRedraw();
    }
    virtual void redraw(bool) {
    	_canvas->setNeedRedraw();
    }
};

GiSkiaView::GiSkiaView(GiCanvasBase* canvas) : _zoomMask(7)
{
	_view = new MgViewProxy(canvas);
}

GiSkiaView::~GiSkiaView()
{
    delete _view;
}

bool GiSkiaView::saveShapes(MgStorageBase* s)
{
    return s && _view->_shapes && _view->_shapes->save(s);
}

bool GiSkiaView::loadShapes(MgStorageBase* s)
{
	bool ret = true;
	
    if (_view->_shapes && !s) {
        _view->_shapes->clear();
    }
    else {
    	ret = _view->_shapes && s && _view->_shapes->load(s);
    }
    _view->regen();
    
    return ret;
}


int GiSkiaView::getWidth() const
{
    return _view->_canvas->xf().getWidth();
}

int GiSkiaView::getHeight() const
{
    return _view->_canvas->xf().getHeight();
}

void GiSkiaView::onSize(int width, int height)
{
    _view->_canvas->xf().setWndSize(width, height);
}

bool GiSkiaView::onDraw(GiCanvasBase& canvas)
{
	return _view->_shapes && _view->_shapes->draw(canvas.gs()) > 0;
}

bool GiSkiaView::onDynDraw(GiCanvasBase& canvas)
{
	MgCommand* cmd = mgGetCommandManager()->getCommand();
	return cmd && cmd->draw(&_view->_motion, &canvas.gs());
}

const char* GiSkiaView::getCommandName() const
{
	return mgGetCommandManager()->getCommandName();
}

bool GiSkiaView::setCommandName(const char* name)
{
	return mgGetCommandManager()->setCommand(&_view->_motion, name);
}

bool GiSkiaView::onGesture(int gestureType, int gestureState, int fingerCount,
		                   float x1, float y1, float x2, float y2)
{
	bool ret = false;
	MgCommand* cmd = mgGetCommandManager()->getCommand();

	if (!cmd) {
		return false;
	}
	if ((gestureState < 1 || gestureState > 3) && (1 == gestureType || 5 == gestureType)) {
		return cmd->cancel(&_view->_motion);
	}

	_view->_motion.point.set(x1, y1);
	_view->_motion.pointM = _view->_motion.point * _view->_canvas->xf().displayToModel();
	if (1 == gestureState || (gestureType != 1 && gestureType != 5)) {
		_view->_motion.startPoint = _view->_motion.point;
		_view->_motion.startPointM = _view->_motion.pointM;
		_view->_motion.lastPoint = _view->_motion.point;
		_view->_motion.lastPointM = _view->_motion.pointM;
		_view->_moved = false;
	}

	switch (gestureType) {
		case 1:	// pan
			if (1 == gestureState) {
				ret = cmd->touchBegan(&_view->_motion);
			}
			else if (2 == gestureState) {
				ret = cmd->touchMoved(&_view->_motion);
				_view->_moved = _view->_moved || _view->_motion.startPoint.distanceTo(_view->_motion.point) > 2;
			}
			else if (3 == gestureState) {
				ret = cmd->touchEnded(&_view->_motion);
				if (!_view->_moved) {
					ret = cmd->click(&_view->_motion);
				}
			}
			_view->_motion.lastPoint = _view->_motion.point;
			_view->_motion.lastPointM = _view->_motion.pointM;
			break;

		case 2:	// click
			ret = cmd->click(&_view->_motion);
			break;
		case 3:
			ret = cmd->doubleClick(&_view->_motion);
			break;
		case 4:
			ret = cmd->longPress(&_view->_motion);
			break;
		case 5:	// two fingers pan
			if (_zoomMask & 4) {
				dynZoom(_view->_motion.point, Point2d(x2, y2), gestureState);
			}
			break;
		case 6:	// two fingers double click
			if (_zoomMask & 4) {
				switchZoom(_view->_motion.point);
			}
			break;
	}

	return ret;
}

GiContext& GiSkiaView::getCurrentContext(bool forChange)
{
	MgShape* shape = NULL;
	mgGetCommandManager()->getSelection(_view, 1, &shape, forChange);
	_view->_tmpContext = shape ? *shape->context() : *_view->context();
	return _view->_tmpContext;
}

void GiSkiaView::applyContext(const GiContext& ctx, int mask, int apply)
{
	if (mask != 0) {
		UInt32 n = mgGetCommandManager()->getSelection(_view, 0, NULL, true);
		std::vector<MgShape*> shapes(n, NULL);

		if (n > 0 && mgGetCommandManager()->getSelection(_view, n, (MgShape**)&shapes.front(), true) > 0) {
			for (int i = 0; i < n; i++) {
				if (shapes[i]) {
					shapes[i]->context()->copy(ctx, mask);
				}
			}
			_view->redraw(false);
		}
		else {
			_view->context()->copy(ctx, mask);
		}
	}

	if (apply != 0) {
		mgGetCommandManager()->dynamicChangeEnded(_view, apply > 0);
	}
}

void GiSkiaView::setZoomFeature(int mask)
{
	_zoomMask = mask;
}

void GiSkiaView::dynZoom(const Point2d& pt1, const Point2d& pt2, int gestureState)
{
	Point2d ptw1 = pt1 * _view->_canvas->xf().displayToWorld();
	Point2d ptw2 = pt2 * _view->_canvas->xf().displayToWorld();

	if (1 == gestureState) {
		_lastPtW[0] = ptw1;
		_lastPtW[1] = ptw2;
	}
	else if (2 == gestureState) {
		float scale = (float)(ptw1.distanceTo(ptw2) / _lastPtW[0].distanceTo(_lastPtW[1]));
		Vector2d offset = (ptw1 - _lastPtW[0]) * _view->_canvas->xf().worldToDisplay();
		Point2d ptAt = _lastPtW[0] * _view->_canvas->xf().worldToDisplay();

		_view->_canvas->xf().zoomScale(_view->_canvas->xf().getViewScale() * scale, &ptAt);
		_view->_canvas->xf().zoomPan(offset.x, offset.y);
		_view->regen();

		_lastPtW[0] = ptw1;
		_lastPtW[1] = ptw2;
	}
}

void GiSkiaView::switchZoom(const Point2d&)
{
}

#include <testgraph/RandomShape.cpp>

int GiSkiaView::addTestingShapes()
{
	RandomParam param;
    UInt32 n = _view->_shapes->getShapeCount();

	RandomParam::init();
	param.initShapes(_view->_shapes);

	if (n == 0) {
		_view->_canvas->xf().zoomTo(_view->_shapes->getExtent() * _view->_canvas->xf().modelToWorld());
		_view->_canvas->xf().zoomByFactor(1.5f);
	}

	return _view->_shapes->getShapeCount() - n;
}
