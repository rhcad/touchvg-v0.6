// GiSkiaView.cpp
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "GiSkiaView.h"
#include "GiCmdAndr.h"
#include <mgshapest.h>
#include <list>
#include <mgstoragebs.h>
#include <mgcmd.h>

class MgViewProxy : public MgView
{
public:
	bool			_needRedraw;
	GiCanvasBase*	_canvas;
	MgShapes*		_shapes;
	MgMotion		_motion;

	MgViewProxy(GiCanvasBase* canvas) : _needRedraw(true), _canvas(canvas) {
		_shapes = new MgShapesT<std::list<MgShape*> >;
		_motion.view = this;
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
    	_needRedraw = true;
    }
    virtual void redraw(bool) {
    	_needRedraw = true;
    }
};

GiSkiaView::GiSkiaView(GiCanvasBase* canvas)
{
	_view = new MgViewProxy(canvas);
    _cmdc = new GiCmdController;
}

GiSkiaView::~GiSkiaView()
{
    delete _cmdc;
    delete _view;
}

bool GiSkiaView::saveShapes(MgStorageBase* s)
{
    return s && _view->_shapes && _view->_shapes->save(s);
}

bool GiSkiaView::loadShapes(MgStorageBase* s)
{
    if (_view->_shapes && !s) {
        _view->_shapes->clear();
        return true;
    }
    return _view->_shapes && s && _view->_shapes->load(s);
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
	_view->_needRedraw = false;
	return _view->_shapes && _view->_shapes->draw(canvas.gs()) > 0;
}

bool GiSkiaView::onDynDraw(GiCanvasBase& canvas)
{
	_view->_needRedraw = false;
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

bool GiSkiaView::isNeedRedraw() const
{
	return _view->_needRedraw;
}

bool GiSkiaView::onGesture(int gestureType, int gestureState, int fingerCount,
		                   float x1, float y1, float x2, float y2)
{
	bool ret = false;
	MgCommand* cmd = mgGetCommandManager()->getCommand();

	if (!cmd) {
		return false;
	}
	if (gestureState < 1 || gestureState > 3) {
		return cmd->cancel(&_view->_motion);
	}

	_view->_motion.point.set(x1, y1);
	_view->_motion.pointM = _view->_motion.point * _view->_canvas->xf().displayToModel();
	if (1 == gestureState) {
		_view->_motion.startPoint = _view->_motion.point;
		_view->_motion.startPointM = _view->_motion.pointM;
	}

	switch (gestureType) {
		case 1:	// 单指滑动
			if (1 == gestureState) {
				ret = cmd->touchBegan(&_view->_motion);
			}
			else if (2 == gestureState) {
				ret = cmd->touchMoved(&_view->_motion);
			}
			else if (3 == gestureState) {
				ret = cmd->touchEnded(&_view->_motion);
			}
			_view->_motion.lastPoint = _view->_motion.point;
			_view->_motion.lastPointM = _view->_motion.pointM;
			break;
		case 2:	// 单指单击
			ret = cmd->click(&_view->_motion);
			break;
		case 3:	// 单指双击
			ret = cmd->doubleClick(&_view->_motion);
			break;
		case 4:	// 长按
			ret = cmd->longPress(&_view->_motion);
			break;
		case 5:	// 双指滑动
			break;
		case 6:	// 双指放缩
			break;
		case 7:	// 双指双击
			break;
	}

	return ret;
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
