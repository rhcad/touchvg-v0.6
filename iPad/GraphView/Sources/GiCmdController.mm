// GiCmdController.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import "GiCmdController.h"
#include <Graph2d/mgcmd.h>

@interface GiCommandController(Internal)

- (id<GiView>)toView:(UIGestureRecognizer*)sender;
- (void)convertPoint:(CGPoint)pt;

@end

class MgViewProxy : public MgView
{
public:
    id<GiView>      view;
    MgViewProxy() : view(Nil) {}
private:
    MgShapes* shapes() { return [view getShapes]; }
    GiTransform* xform() { return [view getXform]; }
    GiGraphics* graph() { return [view getGraph]; }
    void regen() { [view regen]; }
    void redraw() { [view redraw]; }
};

@implementation GiCommandController(Internal)

- (id<GiView>)toView:(UIGestureRecognizer*)sender
{
    assert([sender.view conformsToProtocol:@protocol(GiView)]);
    return (id<GiView>)sender.view;
}

- (void)convertPoint:(CGPoint)pt
{
    _motion->point.x = mgRound(pt.x);
    _motion->point.y = mgRound(pt.y);
    _motion->pointM = Point2d(pt.x, pt.y) * _motion->view->xform()->displayToModel();
}

@end

@implementation GiCommandController

- (id)init
{
    self = [super init];
    if (self) {
        _mgview = new MgViewProxy;
        _motion = new MgMotion;
        _motion->view = _mgview;
    }
    return self;
}

- (void)dealloc
{
    delete _motion;
    delete _mgview;
    [super dealloc];
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
    return cmd && _mgview->view && cmd->undo(_motion);
}

- (BOOL)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    BOOL ret = NO;
    
    if (cmd)
    {
        _mgview->view = [self toView:sender];
        [self convertPoint:[sender locationInView:sender.view]];
        
        if (sender.state == UIGestureRecognizerStateBegan) {
            _motion->startPoint = _motion->point;
            _motion->startPointM = _motion->pointM;
            _motion->lastPoint = _motion->point;
            _motion->lastPointM = _motion->pointM;
            
            ret = cmd->touchesBegan(_motion);
        }
        else if (sender.state == UIGestureRecognizerStateChanged) {
            ret = cmd->touchesMoved(_motion);
        }
        else if (sender.state == UIGestureRecognizerStateEnded) {
            ret = cmd->touchesEnded(_motion);
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
        _mgview->view = [self toView:sender];
        [self convertPoint:[sender locationInView:sender.view]];
        
        ret = cmd->doubleClick(_motion);
        
        _motion->lastPoint = _motion->point;
        _motion->lastPointM = _motion->pointM;
    }
    
    return ret;
}

- (BOOL)oneFingerOneTap:(UITapGestureRecognizer *)sender
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    BOOL ret = NO;
    
    if (cmd) {
        _mgview->view = [self toView:sender];
        [self convertPoint:[sender locationInView:sender.view]];
        
        ret = cmd->click(_motion);
        
        _motion->lastPoint = _motion->point;
        _motion->lastPointM = _motion->pointM;
    }
    
    return ret;
}

@end
