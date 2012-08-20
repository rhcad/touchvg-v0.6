// GiSkiaView.cpp
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "GiSkiaView.h"
#include <mgshapest.h>
#include <list>
#include <mgstoragebs.h>
#include <mgcmd.h>
#include <vector>

//! 绘图视图代理类
class MgViewProxy : public MgView
{
public:
    GiCanvasBase*   _canvas;		//!< 由Android等画布适配器继承的画布对象
    MgShapes*       _shapes;		//!< 矢量图形列表
    MgMotion        _motion;		//!< 当前触摸参数
    bool            _moved;			//!< 是否开始移动
    GiContext       _tmpContext;	//!< 临时绘图参数，用于避免applyContext引用参数问题

    MgViewProxy(GiCanvasBase* canvas) : _canvas(canvas), _moved(false) {
        _shapes = new MgShapesT<std::list<MgShape*> >;
        _motion.view = this;
        _shapes->context()->setLineAlpha(140);	// 默认55%透明度
    }
    virtual ~MgViewProxy() {
    	mgGetCommandManager()->unloadCommands();
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

bool GiSkiaView::onGesture(kGestureType gestureType, kGestureState gestureState, int,
                           float x1, float y1, float x2, float y2)
{
    bool ret = false;
    MgCommand* cmd = mgGetCommandManager()->getCommand();

    if (!cmd) {
        return false;
    }
    if (gestureState == kGestureCancel
    	&& (kSinglePan == gestureType || kZoomRotatePan == gestureType)) {
        return cmd->cancel(&_view->_motion);
    }

    _view->_motion.point.set(x1, y1);
    _view->_motion.pointM = _view->_motion.point * _view->_canvas->xf().displayToModel();
    if (1 == gestureState || (gestureType != kSinglePan && gestureType != kZoomRotatePan)) {
        _view->_motion.startPoint = _view->_motion.point;
        _view->_motion.startPointM = _view->_motion.pointM;
        _view->_motion.lastPoint = _view->_motion.point;
        _view->_motion.lastPointM = _view->_motion.pointM;
        _view->_moved = false;
    }

    switch (gestureType) {
        case kSinglePan:
            if (kGestureBegan == gestureState) {
                ret = cmd->touchBegan(&_view->_motion);
            }
            else if (kGestureMoved == gestureState) {
                ret = cmd->touchMoved(&_view->_motion);
                _view->_moved = _view->_moved || _view->_motion.startPoint.distanceTo(_view->_motion.point) > 2;
            }
            else if (kGestureEnded == gestureState) {
                ret = cmd->touchEnded(&_view->_motion);
                if (!_view->_moved) {
                    ret = cmd->click(&_view->_motion);
                }
            }
            _view->_motion.lastPoint = _view->_motion.point;
            _view->_motion.lastPointM = _view->_motion.pointM;
            break;

        case kSingleTap:
            ret = cmd->click(&_view->_motion);
            break;
        case kDoubleTap:
            ret = cmd->doubleClick(&_view->_motion);
            break;
        case kLongPress:
            ret = cmd->longPress(&_view->_motion);
            break;
        case kZoomRotatePan:
            if (_zoomMask & 3) {
            	ret = dynZoom(_view->_motion.point, Point2d(x2, y2), gestureState);
            }
            break;
        case kTwoFingersDblClick:
            if (_zoomMask & 4) {
            	ret = switchZoom(_view->_motion.point);
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
            for (UInt32 i = 0; i < n; i++) {
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

bool GiSkiaView::dynZoom(const Point2d& pt1, const Point2d& pt2, int gestureState)
{
    if (kGestureBegan == gestureState) {
    	_view->_canvas->xf().getZoomValue(_lastCenterW, _lastViewScale);
    	_firstDist = pt1.distanceTo(pt2);
    	_firstPt = (pt1 + pt2) / 2;
    }
    else if (kGestureMoved == gestureState && _firstDist > 1) {
    	float scale = pt1.distanceTo(pt2) / _firstDist;
    	Point2d pt = (pt1 + pt2) / 2;

    	_view->_canvas->xf().zoom(_lastCenterW, _lastViewScale);   		// 先恢复
    	_view->_canvas->xf().zoomByFactor(scale - 1, &_firstPt);		// 以起始点为中心放缩显示
    	_view->_canvas->xf().zoomPan(pt.x - _firstPt.x, pt.y - _firstPt.y);	// 平移到当前点

        _view->regen();
    }

    return true;
}

bool GiSkiaView::switchZoom(const Point2d&)
{
	return false;
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
