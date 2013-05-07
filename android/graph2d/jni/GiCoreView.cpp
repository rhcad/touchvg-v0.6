// GiCoreView.cpp
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "GiCoreView.h"
#include <mgstoragebs.h>
#include <mgcmd.h>
#include <vector>
#include <mgselect.h>
#include <mgbasicsp.h>

//! 绘图视图代理类
/*! \ingroup GRAPH_SKIA
    \see GiCoreView
 */
class MgViewProxyAndr : public MgView
{
public:
    GiCanvasBase*   _canvas;        //!< 由Android等画布适配器继承的画布对象
    MgShapeDoc*     _doc;           //!< 图形文档
    MgMotion        _motion;        //!< 当前触摸参数
    bool            _moved;         //!< 是否开始移动
    GiContext       _tmpContext;    //!< 临时绘图参数，用于避免applyContext引用参数问题
    MgShapeDoc*     _playShapes;    //!< 临时播放的图形列表
    MgShape*        _shapeAdded;    //!< 待添加显示的图形


    MgViewProxyAndr(GiCanvasBase* canvas) : _canvas(canvas), _moved(false) {
        _doc = MgShapeDoc::create();
        _motion.view = this;
        _doc->context()->setLineAlpha(140);  // 默认55%透明度
        _doc->context()->setAutoFillColor(true);
        _playShapes = NULL;
        _shapeAdded = NULL;
    }

    virtual ~MgViewProxyAndr() {
        mgGetCommandManager()->unloadCommands();
        _doc->release();
    }

    void release() {
        delete this;
    }

    virtual MgShapeDoc* doc() {
        return _doc;
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
    virtual bool drawHandle(GiGraphics* gs, const Point2d& pnt, bool hotdot) {
        Point2d ptd(pnt * gs->xf().modelToDisplay());
        return _canvas->drawHandle(ptd.x, ptd.y, hotdot ? 1 : 0);
    }
    virtual bool isContextActionsVisible() {
        return _canvas->isContextActionsVisible();
    }
    virtual bool showContextActions(int, const int* actions, const Box2d& selbox, const MgShape*) {
        if (_motion.pressDrag && isContextActionsVisible())
            return false;
        int n = 0;
        while (actions && actions[n] > 0) n++;
        mgvector<int> arr(actions, n);
        return _canvas->showContextActions(arr, selbox.xmin, selbox.ymin, selbox.width(), selbox.height());
    }
};

MgShape* mgAddImageShape(const MgMotion* sender, const char* name, float width, float height);

GiCoreView::GiCoreView(GiCanvasBase* canvas) : _zoomMask(7)
{
    _view = new MgViewProxyAndr(canvas);
}

GiCoreView::~GiCoreView()
{
    if (_view) {
        _view->release();
        _view = NULL;
    }
}

bool GiCoreView::findShapeByImageId(const char* name)
{
    return !!MgImageShape::findShapeByImageID(_view->_doc->getCurrentShapes(), name);
}

bool GiCoreView::addImageShapes(const char* name, int width, int height)
{
    return !!mgAddImageShape(&_view->_motion, name, (float)width, (float)height);
}

bool GiCoreView::saveShapes(MgStorage* s)
{
    return s && _view->_doc && _view->_doc->save(s);
}

bool GiCoreView::loadShapes(MgStorage* s)
{
    bool ret = false;
    const char* oldcmd = getCommandName();

    setCommandName("");

    if (_view->_doc && !s) {
        MgShapesLock locker(_view->_doc, MgShapesLock::Remove);
        _view->_doc->clear();
        ret = true;
    }
    else if (_view->_doc) {
        MgShapesLock locker(_view->_doc, MgShapesLock::Load);
        ret = locker.locked() && _view->_doc->load(s);
    }
    _view->regen();
    setCommandName(oldcmd);

    return ret;
}

int GiCoreView::getShapeCount()
{
    return _view->_doc ? _view->_doc->getChangeCount() : 0;
}

int GiCoreView::getChangeCount()
{
    return _view->_doc ? _view->_doc->getChangeCount() : 0;
}

int GiCoreView::getWidth() const
{
    return _view->_canvas->xf().getWidth();
}

int GiCoreView::getHeight() const
{
    return _view->_canvas->xf().getHeight();
}

bool GiCoreView::onSize(int width, int height)
{
    return _view->_canvas->xf().setWndSize(width, height);
}

bool GiCoreView::onDraw(GiCanvasBase& canvas)
{
    if (_view->_playShapes) {
        return _view->_playShapes->draw(canvas.gs()) > 0;
    }
    return _view->_doc && _view->_doc->draw(canvas.gs()) > 0;
}

bool GiCoreView::onDynDraw(GiCanvasBase& canvas)
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    return cmd && cmd->draw(&_view->_motion, &canvas.gs());
}

const char* GiCoreView::getCommandName() const
{
    return mgGetCommandManager()->getCommandName();
}

bool GiCoreView::setCommandName(const char* name)
{
    return mgGetCommandManager()->setCommand(&_view->_motion, name);
}

void GiCoreView::doContextAction(int action)
{
    mgGetCommandManager()->doContextAction(&_view->_motion, action);
}

bool GiCoreView::onGesture(GiGestureType gestureType, GiGestureState gestureState, int,
                           float x1, float y1, float x2, float y2)
{
    bool ret = false;
    MgCommand* cmd = mgGetCommandManager()->getCommand();

    if (!cmd) {
        return false;
    }
    _view->_motion.dragging = ((kSinglePan == gestureType || kZoomRotatePan == gestureType)
            && gestureState == kGestureMoved);
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
            ret = handleSelectionTwoFingers(_view->_motion.point, Point2d(x2, y2), gestureState);
            if (!ret && (_zoomMask & 3)) {
                ret = dynZoom(_view->_motion.point, Point2d(x2, y2), gestureState);
            }
            break;
        case kTwoFingersDblClick:
            if (_zoomMask & 4) {
                ret = switchZoom(_view->_motion.point);
            }
            break;
        default:
            break;
    }

    return ret;
}

GiContext& GiCoreView::getCurrentContext(bool forChange)
{
    MgShape* shape = NULL;
    mgGetCommandManager()->getSelection(_view, 1, &shape, forChange);
    _view->_tmpContext = shape ? *shape->context() : *_view->context();
    return _view->_tmpContext;
}

void GiCoreView::applyContext(const GiContext& ctx, int mask, int apply)
{
    if (mask != 0) {
        int n = mgGetCommandManager()->getSelection(_view, 0, NULL, true);
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

void GiCoreView::setZoomFeature(int mask)
{
    _zoomMask = mask;
}

bool GiCoreView::dynZoom(const Point2d& pt1, const Point2d& pt2, int gestureState)
{
    if (kGestureBegan == gestureState) {
        _lastViewScale = _view->_canvas->xf().getZoomValue(_lastCenterW);
        _firstDist = pt1.distanceTo(pt2);
        _firstPt = (pt1 + pt2) / 2;
    }
    else if (kGestureMoved == gestureState && _firstDist > 1) {
        float scale = pt1.distanceTo(pt2) / _firstDist;
        Point2d pt = (pt1 + pt2) / 2;

        _view->_canvas->xf().zoom(_lastCenterW, _lastViewScale);        // 先恢复
        _view->_canvas->xf().zoomByFactor(scale - 1, &_firstPt);        // 以起始点为中心放缩显示
        _view->_canvas->xf().zoomPan(pt.x - _firstPt.x, pt.y - _firstPt.y); // 平移到当前点

        _view->regen();
    }

    return true;
}

bool GiCoreView::handleSelectionTwoFingers(const Point2d& pt1, const Point2d& pt2, int gestureState)
{
    MgDynShapeLock locker;
    MgSelection *sel = mgGetCommandManager()->getSelection(_view);
    Point2d pnt1 = pt1 * _view->xform()->displayToModel();
    Point2d pnt2 = pt2 * _view->xform()->displayToModel();

    _view->_motion.dragging = (gestureState == kGestureMoved);

    return sel && sel->handleTwoFingers(&_view->_motion, gestureState, pnt1, pnt2);
}

bool GiCoreView::switchZoom(const Point2d&)
{
    return false;
}

#include <testgraph/RandomShape.cpp>

int GiCoreView::addTestingShapes()
{
    RandomParam param;
    int n = _view->_doc->getShapeCount();

    MgShapesLock locker(_view->_doc, MgShapesLock::Load);
    RandomParam::init();
    param.initShapes(_view->_doc->getCurrentShapes());

    if (n == 0) {
        _view->_canvas->xf().zoomTo(_view->_doc->getExtent() * _view->_canvas->xf().modelToWorld());
        _view->_canvas->xf().zoomByFactor(1.5f);
    }

    return _view->_doc->getShapeCount() - n;
}
