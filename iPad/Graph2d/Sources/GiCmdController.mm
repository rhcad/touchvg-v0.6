// GiCmdController.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchvg

#import "GiCmdController.h"
#include <vector>

@interface GiCommandController(Internal)

- (BOOL)setView:(UIView*)view;
- (void)convertPoint:(CGPoint)pt;
- (GiContext*)currentContext;

@end

class MgViewProxy : public MgView
{
private:
    id<GiView>      _curview;
    id<GiView>      _mainview;
    UIView**        _auxviews;
public:
    MgViewProxy(UIView** views) : _curview(Nil), _mainview(Nil), _auxviews(views) {}
    GiContext* context() { return shapes() ? shapes()->context() : NULL; }
    id<GiView> getView() { return _curview; }
    
    void setView(id<GiView> gv) {
        _curview = gv;
        if (!_mainview)
            _mainview = gv;
    }
    
private:
    MgShapes* shapes() { return [_curview shapes]; }
    GiTransform* xform() { return [_curview xform]; }
    GiGraphics* graph() { return [_curview graph]; }
    
    void regen() {
        [_mainview regen];
        for (int i = 0; _auxviews[i]; i++) {
            if ([_auxviews[i] respondsToSelector:@selector(regen)] && !_auxviews[i].hidden) {
                [_auxviews[i] performSelector:@selector(regen)];
            }
        }
    }
    
    void redraw(bool fast) {
        [_curview redraw:fast];
    }
    
    void shapeAdded(MgShape* shape) {
        [_mainview shapeAdded:shape];
        for (int i = 0; _auxviews[i]; i++) {
            if ([_auxviews[i] conformsToProtocol:@protocol(GiView)] && !_auxviews[i].hidden) {
                id<GiView> gv = (id<GiView>)_auxviews[i];
                [gv shapeAdded:shape];
            }
        }
    }
    
    bool shapeWillAdded(MgShape* shape) { return true; }
    bool shapeWillDeleted(MgShape* shape) { return true; }
};

@implementation GiCommandController(Internal)

- (BOOL)setView:(UIView*)view
{
    if ([view conformsToProtocol:@protocol(GiView)])
        _mgview->setView((id<GiView>)view);
    return !!_mgview->getView();
}

- (void)convertPoint:(CGPoint)pt
{
    _motion->point = Point2d(pt.x, pt.y);
    _motion->pointM = Point2d(pt.x, pt.y) * _motion->view->xform()->displayToModel();
}

- (GiContext*)currentContext
{
    MgShape* shape = NULL;
    mgGetCommandManager()->getSelection(_mgview, 1, &shape, false);
    return shape ? shape->context() : _mgview->context();
}

@end

@implementation GiCommandController

@synthesize commandName;
@synthesize lineWidth;
@synthesize lineStyle;
@synthesize lineColor;
@synthesize fillColor;

- (id)initWithViews:(UIView**)auxviews
{
    self = [super init];
    if (self) {
        _mgview = new MgViewProxy(auxviews);
        _motion = new MgMotion;
        _motion->view = _mgview;
    }
    return self;
}

- (void)dealloc
{
    mgGetCommandManager()->unloadCommands();
    delete _motion;
    delete _mgview;
    [super dealloc];
}

- (const char*)commandName {
    return mgGetCommandManager()->getCommandName();
}

- (void)setCommandName:(const char*)name {
    mgGetCommandManager()->setCommand(_motion, name);
}

- (float)lineWidth {
    return [self currentContext]->getLineWidth();
}

- (void)setLineWidth:(float)w {
    UInt32 n = mgGetCommandManager()->getSelection(_mgview, 0, NULL, true);
    std::vector<MgShape*> shapes(n, NULL);
    
    if (n > 0 && mgGetCommandManager()->getSelection(_mgview, n, (MgShape**)&shapes.front(), true) == n) {
        for (UInt32 i = 0; i < n; i++) {
            shapes[i]->context()->setLineWidth(w);
        }
        _motion->view->redraw(false);
    }
    else {
        _mgview->context()->setLineWidth(w);
    }
}

- (GiColor)lineColor {
    return [self currentContext]->getLineColor();
}

- (void)setLineColor:(GiColor)c {
    UInt32 n = mgGetCommandManager()->getSelection(_mgview, 0, NULL, true);
    std::vector<MgShape*> shapes(n, NULL);
    
    if (n > 0 && mgGetCommandManager()->getSelection(_mgview, n, (MgShape**)&shapes.front(), true) == n) {
        for (UInt32 i = 0; i < n; i++) {
            shapes[i]->context()->setLineColor(c);
        }
        _motion->view->redraw(false);
    }
    else {
        _mgview->context()->setLineColor(c);
    }
}

- (GiColor)fillColor {
    return [self currentContext]->getFillColor();
}

- (void)setFillColor:(GiColor)c {
    UInt32 n = mgGetCommandManager()->getSelection(_mgview, 0, NULL, true);
    std::vector<MgShape*> shapes(n, NULL);
    
    if (n > 0 && mgGetCommandManager()->getSelection(_mgview, n, (MgShape**)&shapes.front(), true) == n) {
        for (UInt32 i = 0; i < n; i++) {
            shapes[i]->context()->setFillColor(c);
        }
        _motion->view->redraw(false);
    }
    else {
        _mgview->context()->setFillColor(c);
    }
}

- (int)lineStyle {
    return [self currentContext]->getLineStyle();
}

- (void)setLineStyle:(int)style {
    UInt32 n = mgGetCommandManager()->getSelection(_mgview, 0, NULL, true);
    std::vector<MgShape*> shapes(n, NULL);
    
    if (n > 0 && mgGetCommandManager()->getSelection(_mgview, n, (MgShape**)&shapes.front(), true) == n) {
        for (UInt32 i = 0; i < n; i++) {
            shapes[i]->context()->setLineStyle((kLineStyle)style);
        }
        _motion->view->redraw(false);
    }
    else {
        _mgview->context()->setLineStyle((kLineStyle)style);
    }
}

- (BOOL)dynamicChangeEnded:(BOOL)apply
{
    return mgGetCommandManager()->dynamicChangeEnded(_mgview, apply);
}

- (CGPoint)getPointModel {
    return CGPointMake(_motion->pointM.x, _motion->pointM.y);
}

- (void)dynDraw:(GiGraphics*)gs
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    if (cmd && _mgview->getView()) {
        cmd->draw(_motion, gs);
    }
}

- (BOOL)cancel
{
    return _mgview->getView() && mgGetCommandManager()->cancel(_motion);
}

- (BOOL)undoMotion
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    bool recall;
    return cmd && _mgview->getView() && cmd->undo(recall, _motion);
}

- (void)touchesBegan:(CGPoint)point view:(UIView*)view
{
    if ([self setView:view]) {
        [self convertPoint:point];
        _motion->startPoint = _motion->point;
        _motion->startPointM = _motion->pointM;
        _motion->lastPoint = _motion->point;
        _motion->lastPointM = _motion->pointM;
    }
    _moved = NO;
    _clicked = NO;
}

- (BOOL)touchesMoved:(CGPoint)point view:(UIView*)view count:(int)count
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    BOOL ret = NO;
    
    if (!_moved && cmd) {
        _undoFired = NO;
        _moved = YES;
        ret = cmd->touchBegan(_motion);
    }
    if (cmd) {
        [self setView:view];
        [self convertPoint:point];
        
        if (_motion->lastPoint.distanceTo(_motion->point) <= 2) {
            ret = YES;
        }
        else if (count > 1) {                   // 滑动时有两个手指
            bool recall = false;
            float dist = mgHypot(_motion->point.x - _motion->lastPoint.x, 
                                 _motion->point.y - _motion->lastPoint.y);
            if (!_undoFired && dist > 10) {     // 双指滑动超过10像素可再触发Undo操作
                if (cmd->undo(recall, _motion) && !recall)  // 触发一次Undo操作
                    _undoFired = YES;           // 另一个手指不松开也不再触发Undo操作
                _motion->lastPoint = _motion->point;    // 用于检测下次双指滑动距离
                _motion->lastPointM = _motion->pointM;
            }
            return YES;                         // 直接返回，不记录lastPoint
        }
        else {                                  // 单指滑动
            _undoFired = NO;                    // 允许再触发Undo操作
            ret = cmd->touchMoved(_motion);
        }

        _motion->lastPoint = _motion->point;
        _motion->lastPointM = _motion->pointM;
    }
    
    return ret;
}

- (BOOL)touchesEnded:(CGPoint)point view:(UIView*)view count:(int)count
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    BOOL ret = NO;
    
    if (cmd) {
        [self setView:view];
        [self convertPoint:point];
        
        ret = cmd->touchEnded(_motion);
    }
    
    return ret;
}

- (BOOL)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    BOOL ret = NO;
    
    if (cmd)
    {
        if (sender.state == UIGestureRecognizerStateChanged) {
            ret = [self touchesMoved:[sender locationInView:sender.view]
                                view:sender.view count:sender.numberOfTouches];
        }
        else if (sender.state == UIGestureRecognizerStateEnded) {
            if ([sender numberOfTouches]) {
                [self convertPoint:[sender locationInView:sender.view]];
            }
            ret = cmd->touchEnded(_motion);
        }
        else if (sender.state != UIGestureRecognizerStateBegan) {
            ret = cmd->cancel(_motion);
        }
        ret = YES;
    }
    
    return ret;
}

- (BOOL)oneFingerTwoTaps:(UITapGestureRecognizer *)sender
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    BOOL ret = NO;
    
    if (cmd) {
        [self setView:sender.view];
        [self convertPoint:[sender locationInView:sender.view]];
        _motion->startPoint = _motion->point;
        _motion->startPointM = _motion->pointM;
        _motion->lastPoint = _motion->point;
        _motion->lastPointM = _motion->pointM;
        
        ret = cmd->doubleClick(_motion);
    }
    
    return ret;
}

- (BOOL)oneFingerOneTap:(UITapGestureRecognizer *)sender
{
    _clicked = YES;
    return mgGetCommandManager()->getCommand() != NULL;
}

- (BOOL)delayTap:(CGPoint)point view:(UIView*)view
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    BOOL ret = NO;
    
    if (cmd && _clicked) {
        [self setView:view];
        [self convertPoint:point];
        
        ret = cmd->click(_motion);
    }
    _clicked = NO;
    
    return ret;
}

@end
