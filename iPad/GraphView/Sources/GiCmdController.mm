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
    GiContext*      _context;
    id<GiView>      view;
    MgViewProxy(GiContext* ctx) : _context(ctx), view(Nil) {}
private:
    MgShapes* shapes() { return [view getShapes]; }
    GiTransform* xform() { return [view getXform]; }
    GiGraphics* graph() { return [view getGraph]; }
    void regen() { [view regen]; }
    void redraw() { [view redraw]; }
    const GiContext* context() { return _context; }
    bool shapeWillAdded(MgShape* shape) { return true; }
    bool shapeWillDeleted(MgShape* shape) { return true; }
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

@synthesize context = _context;
@synthesize commandName;

- (id)init
{
    self = [super init];
    if (self) {
        _context = new GiContext(-3);
        _mgview = new MgViewProxy(_context);
        _motion = new MgMotion;
        _motion->view = _mgview;
    }
    return self;
}

- (void)dealloc
{
    delete _motion;
    delete _mgview;
    delete _context;
    [super dealloc];
}

- (const char*)getCommandName {
    return mgGetCommandManager()->getCommandName();
}

- (void)setCommandName:(const char*)name {
    mgGetCommandManager()->setCommand(_motion, name);
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
            
            ret = cmd->touchBegan(_motion);
        }
        else if (sender.state == UIGestureRecognizerStateChanged) {
            ret = cmd->touchMoved(_motion);
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
        _mgview->view = [self toView:sender];
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
        _mgview->view = [self toView:sender];
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
