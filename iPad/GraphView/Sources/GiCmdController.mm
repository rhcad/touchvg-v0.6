// GiCmdController.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import "GiCmdController.h"
#include <Graph2d/mgcmd.h>
#include <vector>

@interface GiCommandController(Internal)

- (BOOL)setView:(UIView*)view;
- (void)convertPoint:(CGPoint)pt;

@end

class MgViewProxy : public MgView
{
public:
    GiContext*      _context;
    id<GiView>      view;
    UIView**        _auxview;
    MgViewProxy(GiContext* ctx, UIView** auxview) : _context(ctx), view(Nil), _auxview(auxview) {}
private:
    MgShapes* shapes() { return [view getShapes]; }
    GiTransform* xform() { return [view getXform]; }
    GiGraphics* graph() { return [view getGraph]; }
    void regen() {
        [view regen];
        for (int i = 0; _auxview[i]; i++) {
            if ([_auxview[i] respondsToSelector:@selector(regen)])
                [_auxview[i] performSelector:@selector(regen)];
        }
    }
    void redraw() {
        [view redraw];
        for (int i = 0; _auxview[i]; i++) {
            [_auxview[i] setNeedsDisplay];
        }
    }
    const GiContext* context() { return _context; }
    bool shapeWillAdded(MgShape* shape) { return true; }
    bool shapeWillDeleted(MgShape* shape) { return true; }
};

@implementation GiCommandController(Internal)

- (BOOL)setView:(UIView*)view
{
    if ([view conformsToProtocol:@protocol(GiView)])
        _mgview->view = (id<GiView>)view;
    return !!_mgview->view;
}

- (void)convertPoint:(CGPoint)pt
{
    _motion->point.x = mgRound(pt.x);
    _motion->point.y = mgRound(pt.y);
    _motion->pointM = Point2d(pt.x, pt.y) * _motion->view->xform()->displayToModel();
}

@end

@implementation GiCommandController

@synthesize commandName;
@synthesize lineWidth;
@synthesize lineStyle;
@synthesize lineColor;
@synthesize fillColor;

- (id)initWithViews:(UIView**)auxview
{
    self = [super init];
    if (self) {
        _context = new GiContext(50, GiColor(0, 0, 0, 128));
        _mgview = new MgViewProxy(_context, auxview);
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
    delete _context;
    [super dealloc];
}

- (const char*)commandName {
    return mgGetCommandManager()->getCommandName();
}

- (void)setCommandName:(const char*)name {
    mgGetCommandManager()->setCommand(_motion, name);
}

- (int)lineWidth {
    MgShape* shape = NULL;
    if (mgGetCommandManager()->getSelection(_mgview, 1, &shape) > 0) {
        return shape->context()->getLineWidth();
    }
    return _context->getLineWidth();
}

- (void)setLineWidth:(int)w {
    UInt32 n = mgGetCommandManager()->getSelection(_mgview, 0, NULL);
    std::vector<MgShape*> shapes(n, NULL);
    
    if (n > 0 && mgGetCommandManager()->getSelection(_mgview, n, (MgShape**)&shapes.front()) == n) {
        for (UInt32 i = 0; i < n; i++) {
            shapes[i]->context()->setLineWidth(w);
        }
        _motion->view->redraw();
    }
    else {
        _context->setLineWidth(w);
    }
}

- (GiColor)lineColor {
    MgShape* shape = NULL;
    if (mgGetCommandManager()->getSelection(_mgview, 1, &shape) > 0) {
        return shape->context()->getLineColor();
    }
    return _context->getLineColor();
}

- (void)setLineColor:(GiColor)c {
    UInt32 n = mgGetCommandManager()->getSelection(_mgview, 0, NULL);
    std::vector<MgShape*> shapes(n, NULL);
    
    if (n > 0 && mgGetCommandManager()->getSelection(_mgview, n, (MgShape**)&shapes.front()) == n) {
        for (UInt32 i = 0; i < n; i++) {
            shapes[i]->context()->setLineColor(c);
        }
        _motion->view->redraw();
    }
    else {
        _context->setLineColor(c);
    }
}

- (GiColor)fillColor {
    MgShape* shape = NULL;
    if (mgGetCommandManager()->getSelection(_mgview, 1, &shape) > 0) {
        return shape->context()->getFillColor();
    }
    return _context->getFillColor();
}

- (void)setFillColor:(GiColor)c {
    UInt32 n = mgGetCommandManager()->getSelection(_mgview, 0, NULL);
    std::vector<MgShape*> shapes(n, NULL);
    
    if (n > 0 && mgGetCommandManager()->getSelection(_mgview, n, (MgShape**)&shapes.front()) == n) {
        for (UInt32 i = 0; i < n; i++) {
            shapes[i]->context()->setFillColor(c);
        }
        _motion->view->redraw();
    }
    else {
        _context->setFillColor(c);
    }
}

- (int)lineStyle {
    MgShape* shape = NULL;
    if (mgGetCommandManager()->getSelection(_mgview, 1, &shape) > 0) {
        return shape->context()->getLineStyle();
    }
    return _context->getLineStyle();
}

- (void)setLineStyle:(int)style {
    UInt32 n = mgGetCommandManager()->getSelection(_mgview, 0, NULL);
    std::vector<MgShape*> shapes(n, NULL);
    
    if (n > 0 && mgGetCommandManager()->getSelection(_mgview, n, (MgShape**)&shapes.front()) == n) {
        for (UInt32 i = 0; i < n; i++) {
            shapes[i]->context()->setLineStyle((kLineStyle)style);
        }
        _motion->view->redraw();
    }
    else {
        _context->setLineStyle((kLineStyle)style);
    }
}

- (CGPoint)getPointModel {
    return CGPointMake(_motion->pointM.x, _motion->pointM.y);
}

- (void)dynDraw:(GiGraphics*)gs
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    if (cmd && _mgview->view) {
        cmd->draw(_motion, gs);
    }
}

- (BOOL)cancel
{
    return _mgview->view && mgGetCommandManager()->cancel(_motion);
}

- (BOOL)undoMotion
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    bool recall;
    return cmd && _mgview->view && cmd->undo(recall, _motion);
}

- (void)touchesBegan:(CGPoint)point view:(UIView*)sender
{
    if ([self setView:sender]) {
        [self convertPoint:point];
        _motion->startPoint = _motion->point;
        _motion->startPointM = _motion->pointM;
        _motion->lastPoint = _motion->point;
        _motion->lastPointM = _motion->pointM;
    }
}

- (BOOL)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    BOOL ret = NO;
    
    if (cmd)
    {
        [self setView:sender.view];
        if (sender.state != UIGestureRecognizerStateBegan && [sender numberOfTouches]) {
            [self convertPoint:[sender locationInView:sender.view]];
        }
        
        if (sender.state == UIGestureRecognizerStateBegan) {
            _undoFired = NO;
            ret = cmd->touchBegan(_motion);
        }
        else if (sender.state == UIGestureRecognizerStateChanged) {
            if (sender.numberOfTouches > 1) {   // 滑动时有两个手指
                bool recall = false;
                double dist = mgHypot(_motion->point.x - _motion->lastPoint.x, 
                                      _motion->point.y - _motion->lastPoint.y);
                if (!_undoFired && dist > 10) { // 双指滑动超过10像素可再触发Undo操作
                    if (cmd->undo(recall, _motion) && !recall)  // 触发一次Undo操作
                        _undoFired = YES;       // 另一个手指不松开也不再触发Undo操作
                    _motion->lastPoint = _motion->point;    // 用于检测下次双指滑动距离
                    _motion->lastPointM = _motion->pointM;
                }
                return YES;                     // 直接返回，不记录lastPoint
            }
            else {                              // 单指滑动
                _undoFired = NO;                // 允许再触发Undo操作
                ret = cmd->touchMoved(_motion);
            }
        }
        else if (sender.state == UIGestureRecognizerStateEnded) {
            ret = cmd->touchEnded(_motion);
        }
        else {
            ret = cmd->cancel(_motion);
        }
        
        _motion->lastPoint = _motion->point;
        _motion->lastPointM = _motion->pointM;
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
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    BOOL ret = NO;
    
    if (cmd) {
        [self setView:sender.view];
        [self convertPoint:[sender locationInView:sender.view]];
        _motion->startPoint = _motion->point;
        _motion->startPointM = _motion->pointM;
        _motion->lastPoint = _motion->point;
        _motion->lastPointM = _motion->pointM;
        
        ret = cmd->click(_motion);
    }
    
    return ret;
}

@end
