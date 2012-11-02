#include "mgcmdbreak.h"
#include <mglnrel.h>
#include <mgshapet.h>
#include <mgbasicsp.h>

float mgDisplayMmToModel(float mm, GiGraphics* gs);

bool MgCommandBreak::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (sender->dragging) {
        GiContext ctx(0, GiColor(0, 0, 255, 128), kGiLineDash);
        gs->drawLine(&ctx, sender->startPointM, sender->pointM);
        
        if (_target) {
            GiContext ctxdot(0, GiColor(0, 0, 255, 128), kGiLineSolid, GiColor(255, 0, 255, 64));
            float radius = mgDisplayMmToModel(1, gs);
            
            if (_edges[0] >= 0) {
                gs->drawEllipse(&ctxdot, _crosspt[0], radius);
            }
            if (_edges[1] >= 0) {
                gs->drawEllipse(&ctxdot, _crosspt[1], radius);
            }
            if (_edges[1] == (_edges[0] + 2) % 4) {
                ctx.setLineStyle(kGiLineSolid);
                gs->drawLine(&ctx, _crosspt[0], _crosspt[1]);
                
                GiContext ctxsel(-2, GiColor(0, 0, 255, 128), kGiLineSolid, GiColor(0, 0, 255, 32));
                _target->shapec()->draw(*gs, ctxsel);
            }
            else if (_target->shapec()->getPointCount() == 2) {
                GiContext ctxsel(-2, GiColor(0, 0, 255, 128));
                _target->shapec()->draw(*gs, ctxsel);
            }
        }
    }
    return true;
}

void MgCommandBreak::gatherShapes(const MgMotion* sender, MgShapes* shapes)
{
    if (sender->dragging) {
        MgShapeT<MgLine> shape(GiContext(0, GiColor(0, 0, 255, 128), kGiLineDash));
        shape.shape()->setPoint(0, sender->startPointM);
        shape.shape()->setPoint(1, sender->pointM);
        shapes->addShape(shape);
    }
}

bool MgCommandBreak::touchBegan(const MgMotion*)
{
    _target = NULL;
    return true;
}

static bool canBreak(MgBaseShape* sp)
{
    return (sp->isKindOf(MgLine::Type())
            || sp->isKindOf(MgRect::Type())
            || sp->isKindOf(MgDiamond::Type())
            || sp->isKindOf(MgLines::Type())
            || sp->isKindOf(MgParallelogram::Type()));
}

bool MgCommandBreak::touchMoved(const MgMotion* sender)
{
    Box2d snapbox(sender->startPointM, sender->pointM);
    void* it = NULL;
    Point2d crosspt;
    
    _target = NULL;
    _edges[0] = _edges[1] = -1;
    
    for (MgShape* sp = sender->view->shapes()->getFirstShape(it);
         sp; sp = sender->view->shapes()->getNextShape(it)) {
        if (!sp->shape()->getExtent().isIntersect(snapbox)
            || !canBreak(sp->shape())) {
            continue;
        }
        
        UInt32 n = sp->shapec()->getPointCount();
        if (n == 4) {
            for (UInt32 i = 0; i < n; i++) {
                Point2d pt1(sp->shapec()->getHandlePoint(i));
                Point2d pt2(sp->shapec()->getHandlePoint((i + 1) % n));
                
                if (mgCross2Line(pt1, pt2, sender->startPointM, sender->pointM, crosspt))
                {
                    if (_target == sp) {
                        _edges[1] = i;
                        _crosspt[1] = crosspt;
                    }
                    else if (!_target || _edges[1] != (_edges[0] + 2) % 4) {
                        _target = sp;
                        _edges[0] = i;
                        _crosspt[0] = crosspt;
                        _edges[1] = -1;
                    }
                }
            }
        }
        else if (n == 2 && _edges[1] < 0) {
            if (mgCross2Line(sp->shapec()->getPoint(0), 
                             sp->shapec()->getPoint(1), 
                             sender->startPointM, sender->pointM, crosspt)) {
                _target = sp;
                _crosspt[0] = crosspt;
                _edges[0] = 0;
                _edges[1] = -1;
            }
        }
    }
        
    sender->view->redraw(true);
    return true;
}

bool MgCommandBreak::touchEnded(const MgMotion* sender)
{
    UInt32 n = _target ? _target->shapec()->getPointCount() : 0;
    
    if (n == 2 || _edges[1] == (_edges[0] + 2) % 4) {
        Vector2d vec(Vector2d(1, 1) * sender->view->xform()->worldToModel());
        const MgBaseShape* sp = _target->shapec();
        
        if (n == 2) {
            MgShapesLock lockerAdd(sender->view->shapes(), MgShapesLock::Add);
            MgShapeT<MgLine> shape(*_target->contextc());
            
            shape.shape()->copy(*_target->shapec());
            
            shape.shape()->setPoint(0, sp->getPoint(0));
            shape.shape()->setPoint(1, _crosspt[0]);
            shape.shape()->offset(_crosspt[0].rulerPoint(sp->getPoint(0), fabsf(vec.x), 0) - _crosspt[0], -1);
            
            if (sender->view->shapeWillAdded(&shape)) {
                MgShape* newsp = sender->view->shapes()->addShape(shape);
                sender->view->shapeAdded(newsp);
            }
            
            shape.shape()->setPoint(0, sp->getPoint(1));
            shape.shape()->setPoint(1, _crosspt[0]);
            shape.shape()->offset(_crosspt[0].rulerPoint(sp->getPoint(1), fabsf(vec.y), 0) - _crosspt[0], -1);
            
            if (sender->view->shapeWillAdded(&shape)) {
                MgShape* newsp = sender->view->shapes()->addShape(shape);
                sender->view->shapeAdded(newsp);
            }
        }
        else {
            MgShapesLock lockerAdd(sender->view->shapes(), MgShapesLock::Add);
            MgShapeT<MgLines> shape(*_target->contextc());
            
            shape.shape()->copy(*_target->shapec());
            ((MgLines*)shape.shape())->resize(4);
            ((MgLines*)shape.shape())->setClosed(true);
            
            shape.shape()->setPoint(0, _crosspt[0]);
            shape.shape()->setPoint(1, _crosspt[1]);
            shape.shape()->setPoint(2, sp->getHandlePoint((_edges[1] + 1) % 4));
            shape.shape()->setPoint(3, sp->getHandlePoint((_edges[1] + 2) % 4));
            shape.shape()->offset(_crosspt[0].rulerPoint(sp->getHandlePoint(_edges[0]), fabsf(vec.x), 0) - _crosspt[0], -1);
            
            if (sender->view->shapeWillAdded(&shape)) {
                MgShape* newsp = sender->view->shapes()->addShape(shape);
                sender->view->shapeAdded(newsp);
            }
            
            shape.shape()->setPoint(0, _crosspt[1]);
            shape.shape()->setPoint(1, _crosspt[0]);
            shape.shape()->setPoint(2, sp->getHandlePoint((_edges[0] + 1) % 4));
            shape.shape()->setPoint(3, sp->getHandlePoint((_edges[0] + 2) % 4));
            shape.shape()->offset(_crosspt[1].rulerPoint(sp->getHandlePoint(_edges[1]), fabsf(vec.y), 0) - _crosspt[1], -1);
            
            if (sender->view->shapeWillAdded(&shape)) {
                MgShape* newsp = sender->view->shapes()->addShape(shape);
                sender->view->shapeAdded(newsp);
            }
        }
        
        if (sender->view->shapeWillDeleted(_target)) {
            MgShapesLock locker(sender->view->shapes(), MgShapesLock::Remove);
            if (sender->view->removeShape(_target)) {
                _target->release();
                sender->view->regen();
            }
        }
    }
    
    _target = NULL;
    sender->view->redraw(true);
    
    return true;
}
