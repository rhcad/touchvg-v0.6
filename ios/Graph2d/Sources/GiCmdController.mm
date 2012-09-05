// GiCmdController.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchvg

#import "GiCmdController.h"
#include <mgselect.h>
#include <vector>
#include <ioscanvas.h>

@interface GiCommandController(Internal)

- (BOOL)setView:(UIView*)view;
- (void)convertPoint:(CGPoint)pt;
- (BOOL)getPointForPressDrag:(UIGestureRecognizer *)sender :(CGPoint*)point;
- (GiContext*)currentContext;
- (bool)longPressSelection:(int)selState;

@end

class MgViewProxy : public MgView
{
private:
    id              _owner;
    id<GiView>      _curview;
    id<GiView>      _mainview;
    UIView**        _auxviews;
    UIImage*        _pointImages[2];
    
public:
    BOOL            _dynChanged;
    BOOL            _lockVertex;
    int             _snappedType;
    
    MgViewProxy(id owner, UIView** views) : _owner(owner)
        , _curview(Nil), _mainview(Nil), _auxviews(views)
        , _dynChanged(NO), _lockVertex(false), _snappedType(-1)
    {
        _pointImages[0] = nil;
        _pointImages[1] = nil;
    }
    
    ~MgViewProxy() {
        [_pointImages[0] release];
        [_pointImages[1] release];
    }
    
    GiContext* context() { return shapes() ? shapes()->context() : NULL; }
    id<GiView> getView() { return _curview; }
    
    void setView(id<GiView> gv) {
        _curview = gv;
        if (!_mainview) {
            _mainview = gv;
        }
    }
    
private:
    MgShapes* shapes() { return [_curview shapes]; }
    GiTransform* xform() { return [_curview xform]; }
    GiGraphics* graph() { return [_curview graph]; }
    bool shapeWillAdded(MgShape* shape) {
        NSObject* obj = _mainview.ownerView.nextResponder;
        return (![obj respondsToSelector:@selector(shapeWillAdded)]
                || [obj performSelector:@selector(shapeWillAdded)]);
    }
    bool shapeWillDeleted(MgShape* shape) {
        NSObject* obj = _mainview.ownerView.nextResponder;
        return (![obj respondsToSelector:@selector(shapeWillDeleted)]
                || [obj performSelector:@selector(shapeWillDeleted)]);
    }
    bool shapeCanRotated(MgShape*) {
        NSObject* obj = _mainview.ownerView.nextResponder;
        return (![obj respondsToSelector:@selector(shapeCanRotated)]
                || [obj performSelector:@selector(shapeCanRotated)]);
    }
    bool shapeCanTransform(MgShape*) {
        NSObject* obj = _mainview.ownerView.nextResponder;
        return (![obj respondsToSelector:@selector(shapeCanTransform)]
                || [obj performSelector:@selector(shapeCanTransform)]);
    }
    
    void regen() {
        [_mainview regen];
        for (int i = 0; _auxviews[i]; i++) {
            if ([_auxviews[i] respondsToSelector:@selector(regen)]
                && !_auxviews[i].hidden) {
                [_auxviews[i] performSelector:@selector(regen)];
            }
        }
        if (MgDynShapeLock::lockedForWrite()) {
            _dynChanged = YES;
        }
    }
    
    void redraw(bool fast) {
        [_curview redraw:fast];
        
        if (MgDynShapeLock::lockedForWrite()) {
            _dynChanged = YES;
        }
    }
    
    void shapeAdded(MgShape* shape) {
        [_mainview shapeAdded:shape];
        for (int i = 0; _auxviews[i]; i++) {
            if ([_auxviews[i] conformsToProtocol:@protocol(GiView)]
                && !_auxviews[i].hidden) {
                id<GiView> gv = (id<GiView>)_auxviews[i];
                [gv shapeAdded:shape];
            }
        }
        NSObject* obj = _mainview.ownerView.nextResponder;
        if ([obj respondsToSelector:@selector(shapeAdded)]) {
            [obj performSelector:@selector(shapeAdded)];
        }
    }
    
    bool longPressSelection(int selState) {
        return [_owner longPressSelection:selState];
    }
    
    bool drawHandle(GiGraphics* gs, const Point2d& pnt, bool hotdot) {
        int index = hotdot ? 1 : 0;
        GiCanvasIos* canvas = (GiCanvasIos*)gs->getCanvas();
        
        if (!_pointImages[index]) {
            _pointImages[index] = [UIImage imageNamed:hotdot ? @"vgdot2.png" : @"vgdot1.png"];
            [_pointImages[index] retain];
        }
        if (_pointImages[index]) {
            canvas->drawImage([_pointImages[index] CGImage], pnt);
        }
        
        return _pointImages[index];
    }
};

static long s_cmdRef = 0;


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
    _mgview->_snappedType = mgGetCommandManager()->snapHandlePoint(_motion, 5.f);
}

- (BOOL)getPointForPressDrag:(UIGestureRecognizer *)sender :(CGPoint*)point
{
    BOOL valid = ([sender numberOfTouches] >= _touchCount);
    
    if (valid) {
        *point = [sender locationOfTouch:0 inView:sender.view];
        float dist = mgHypot(point->x - _motion->startPoint.x, point->y - _motion->startPoint.y);
    
        if ([sender numberOfTouches] > 1) {
            CGPoint pt = [sender locationOfTouch:1 inView:sender.view];
            float dist2 = mgHypot(pt.x - _motion->startPoint.x, pt.y - _motion->startPoint.y);
            if (dist2 > dist)
                *point = pt;
        }
    }
    
    return valid;
}

- (GiContext*)currentContext
{
    MgShape* shape = NULL;
    mgGetCommandManager()->getSelection(_mgview, 1, &shape, false);
    return shape ? shape->context() : _mgview->context();
}

- (bool)longPressSelection:(int)selState
{
    UIView *view = [_mgview->getView() ownerView];
    UIMenuController *menuController = [UIMenuController sharedMenuController];
    UIMenuItem *items[5] = { nil, nil, nil, nil, nil };
    
    if (menuController.menuVisible)
        return false;
    
    switch (selState) {
        case kMgSelNone:
            items[0] = [[UIMenuItem alloc] initWithTitle:@"全选" action:@selector(menuClickSelAll:)];
            items[1] = [[UIMenuItem alloc] initWithTitle:@"绘图" action:@selector(menuClickDraw:)];
            break;
            
        case kMgSelOneShape:
        case kMgSelMultiShapes:
            items[0] = [[UIMenuItem alloc] initWithTitle:@"删除" action:@selector(menuClickDelete:)];
            items[1] = [[UIMenuItem alloc] initWithTitle:@"复制" action:@selector(menuClickClone:)];
            items[2] = [[UIMenuItem alloc] initWithTitle:@"重选" action:@selector(menuClickReset:)];
            break;
            
        case kMgSelVertexes:
            items[0] = [[UIMenuItem alloc] initWithTitle:@"闭合" action:@selector(menuClickClosed:)];
            items[1] = [[UIMenuItem alloc] initWithTitle:@"加点" action:@selector(menuClickAddNode:)];
            items[2] = [[UIMenuItem alloc] initWithTitle:@"边长固定" action:@selector(menuClickFixedLength:)];
            items[3] = [[UIMenuItem alloc] initWithTitle:@"重选" action:@selector(menuClickReset:)];
            break;
            
        case kMgSelVertex:
            items[0] = [[UIMenuItem alloc] initWithTitle:@"闭合" action:@selector(menuClickClosed:)];
            items[1] = [[UIMenuItem alloc] initWithTitle:@"删点" action:@selector(menuClickDelNode:)];
            items[2] = [[UIMenuItem alloc] initWithTitle:@"边长固定" action:@selector(menuClickFixedLength:)];
            break;
            
        default:
            return false;
            break;
    }
    
    menuController.menuItems = [NSArray arrayWithObjects: items[0], items[1], items[2], items[3], items[4], nil];
    [menuController setTargetRect:CGRectMake(_motion->point.x - 25, _motion->point.y - 25, 50, 50) inView:view];
    [menuController setMenuVisible:YES animated:YES];
    
    for (NSUInteger i = 0; i < [menuController.menuItems count]; i++) {
        [[menuController.menuItems objectAtIndex:i] release];
    }
    
    return false;
}

@end

@implementation GiCommandController

@synthesize commandName;
@synthesize currentShapeFixedLength;
@synthesize lineWidth;
@synthesize lineStyle;
@synthesize lineColor;
@synthesize fillColor;

- (id)initWithViews:(UIView**)auxviews
{
    self = [super init];
    if (self) {
        _mgview = new MgViewProxy(self, auxviews);
        _motion = new MgMotion;
        _motion->view = _mgview;
        s_cmdRef++;
    }
    return self;
}

- (void)dealloc
{
    if (--s_cmdRef == 0) {
        mgGetCommandManager()->unloadCommands();
    }
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
            shapes[i]->context()->setLineStyle((GiLineStyle)style);
        }
        _motion->view->redraw(false);
    }
    else {
        _mgview->context()->setLineStyle((GiLineStyle)style);
    }
}

- (BOOL)dynamicChangeEnded:(BOOL)apply
{
    return mgGetCommandManager()->dynamicChangeEnded(_mgview, apply);
}

- (BOOL)currentShapeFixedLength
{
    MgSelection *sel = mgGetCommandManager()->getSelection(_mgview);
    return sel && sel->isFixedLength(_mgview);
}

- (void)setCurrentShapeFixedLength:(BOOL)fixed
{
    MgSelection *sel = mgGetCommandManager()->getSelection(_mgview);
    if (sel && sel->setFixedLength(_mgview, !!fixed)) {}
}

- (CGPoint)getPointModel {
    return CGPointMake(_motion->pointM.x, _motion->pointM.y);
}

- (BOOL)dynDraw:(GiGraphics*)gs
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    if (cmd && _mgview->getView()) {
        cmd->draw(_motion, gs);
    }
    if (_mgview->_snappedType >= 0) {
        GiContext ctx(0, GiColor(128, 128, 64, 200), kGiLineDot);
        gs->drawEllipse(&ctx, _motion->pointM, gs->xf().displayToModel(5.f, true));
    }
    return YES;
}

- (void)getDynamicShapes:(MgShapes*)shapes
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    if (cmd && _mgview->getView()) {
        cmd->gatherShapes(_motion, shapes);
    }
}

- (BOOL)isDynamicChanged:(BOOL)reset
{
    BOOL ret = _mgview->_dynChanged;
    if (reset)
        _mgview->_dynChanged = NO;
    return ret;
}

- (BOOL)cancel
{
    _motion->pressDrag = false;
    return _mgview->getView() && mgGetCommandManager()->cancel(_motion);
}

- (BOOL)undoMotion
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    bool recall;
    return cmd && _mgview->getView() && cmd->undo(recall, _motion);
}

- (void)touchesBegan:(CGPoint)point view:(UIView*)view count:(int)count
{
    if (_touchCount <= count) {
        _touchCount = count;
        
        if ([self setView:view] && count == 1) {
            [self convertPoint:point];
            _motion->startPoint = _motion->point;
            _motion->startPointM = _motion->pointM;
            _motion->lastPoint = _motion->point;
            _motion->lastPointM = _motion->pointM;
            _motion->velocity = 0;
            _motion->pressDrag = false;
            _moved = NO;
            _clickFingers = 0;
            _undoFired = NO;
        }
    }
}

- (BOOL)touchesMoved:(CGPoint)point view:(UIView*)view count:(int)count
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    BOOL ret = NO;
    
    if (!_moved && cmd) {
        _moved = YES;
        ret = cmd->touchBegan(_motion);
    }
    if (cmd) {
        [self setView:view];
        
        if (!_motion->pressDrag && count > 1) {             // 变为双指滑动
            bool recall = false;
            if (!_undoFired) {                              // 双指滑动后可再触发Undo操作
                if (cmd->undo(recall, _motion) && !recall)  // 触发一次Undo操作
                    _undoFired = true;                      // 另一个手指不松开也不再触发Undo操作
            }
        }
        else {
            [self convertPoint:point];
            
            ret = cmd->touchMoved(_motion);
            _undoFired = false;                             // 允许再触发Undo操作
            _motion->lastPoint = _motion->point;
            _motion->lastPointM = _motion->pointM;
        }
    }
    
    return !!cmd;
}

- (BOOL)touchesEnded:(CGPoint)point view:(UIView*)view count:(int)count
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    BOOL ret = NO;
    
    if (cmd) {
        [self setView:view];
        [self convertPoint:point];
        
        ret = cmd->touchEnded(_motion);
        _motion->pressDrag = false;
        _touchCount = 0;
    }
    
    return ret;
}

- (BOOL)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    MgDynShapeLock locker;
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    BOOL ret = NO;
    CGPoint point;
    
    if (cmd && locker.locked())
    {
        if (sender.state == UIGestureRecognizerStateBegan) {
            if (_touchCount > [sender numberOfTouches]) {
                _touchCount = [sender numberOfTouches];
                point = [sender locationInView:sender.view];
                [self touchesBegan:point view:sender.view count:_touchCount];
            }
        }
        else if (sender.state == UIGestureRecognizerStateChanged) {
            CGPoint velocity = [sender velocityInView:sender.view];
            _motion->velocity = hypotf(velocity.x, velocity.y);
            
            ret = ([self getPointForPressDrag:sender :&point]
                   && [self touchesMoved:point view:sender.view
                                   count:sender.numberOfTouches]);
        }
        else if (sender.state == UIGestureRecognizerStateEnded) {
            if ([sender numberOfTouches] && [self getPointForPressDrag:sender :&point]) {
                [self convertPoint:point];
            }
            ret = cmd->touchEnded(_motion);
            _touchCount = 0;
        }
        else {
            ret = cmd->cancel(_motion);
            _touchCount = 0;
        }
        ret = YES;
    }
    
    return ret;
}

- (BOOL)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    MgDynShapeLock locker;
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    BOOL ret = NO;
    CGPoint point;
    
    if (_motion->pressDrag && cmd && locker.locked()) {
        if (sender.state == UIGestureRecognizerStateBegan) {
            if (_touchCount > [sender numberOfTouches]) {
                _touchCount = [sender numberOfTouches];
                point = [sender locationInView:sender.view];
                [self touchesBegan:point view:sender.view count:_touchCount];
            }
        }
        else if (sender.state == UIGestureRecognizerStateChanged) {
            _motion->velocity = 0;
            ret = ([self getPointForPressDrag:sender :&point]
                   && [self touchesMoved:point view:sender.view
                                   count:sender.numberOfTouches]);
        }
        else if (sender.state == UIGestureRecognizerStateEnded) {
            if ([sender numberOfTouches]
                && [self getPointForPressDrag:sender :&point]) {
                [self convertPoint:point];
            }
            ret = cmd->touchEnded(_motion);
            _touchCount = 0;
        }
        else {
            ret = cmd->cancel(_motion);
            _touchCount = 0;
        }
        ret = YES;
    }
    
    return ret;
}

- (BOOL)oneFingerTwoTaps:(UITapGestureRecognizer *)sender
{
    _clickFingers = 2;
    return mgGetCommandManager()->getCommand() != NULL;
}

- (BOOL)longPressGesture:(UIGestureRecognizer *)sender
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
        _motion->pressDrag = YES;       // 设置长按标记
        
        ret = cmd->longPress(_motion);
    }
    
    return ret;
}

- (BOOL)oneFingerOneTap:(UITapGestureRecognizer *)sender
{
    _clickFingers = 1;
    return mgGetCommandManager()->getCommand() != NULL;
}

- (BOOL)delayTap:(CGPoint)point view:(UIView*)view
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    BOOL ret = NO;
    
    if (cmd && _clickFingers > 0) {
        [self setView:view];
        [self convertPoint:point];
        
        if (1 == _clickFingers) {
            ret = cmd->click(_motion);
        }
        else if (2 == _clickFingers) {
            ret = cmd->doubleClick(_motion);
        }
        _motion->pressDrag = false;
        _touchCount = 0;
    }
    _clickFingers = 0;
    
    return ret;
}

- (IBAction)menuClickDraw:(id)sender
{
    self.commandName = "splines";
}

- (IBAction)menuClickSelAll:(id)sender
{
    MgSelection *sel = mgGetCommandManager()->getSelection(_mgview);
    if (sel) {
        sel->selectAll(_mgview);
    }
}

- (IBAction)menuClickReset:(id)sender
{
    MgSelection *sel = mgGetCommandManager()->getSelection(_mgview);
    if (sel) {
        sel->resetSelection(_mgview);
    }
}

- (IBAction)menuClickDelete:(id)sender
{
    MgSelection *sel = mgGetCommandManager()->getSelection(_mgview);
    if (sel) {
        sel->deleteSelection(_mgview);
    }
}

- (IBAction)menuClickClone:(id)sender
{
    MgSelection *sel = mgGetCommandManager()->getSelection(_mgview);
    if (sel) {
        sel->cloneSelection(_mgview);
    }
}

- (IBAction)menuClickClosed:(id)sender
{
    MgSelection *sel = mgGetCommandManager()->getSelection(_mgview);
    if (sel) {
        sel->switchClosed(_mgview);
    }
}

- (IBAction)menuClickAddNode:(id)sender
{
    MgSelection *sel = mgGetCommandManager()->getSelection(_mgview);
    if (sel) {
        sel->insertVertext(_motion);
    }
}

- (IBAction)menuClickDelNode:(id)sender
{
    MgSelection *sel = mgGetCommandManager()->getSelection(_mgview);
    if (sel) {
        sel->deleteVertext(_motion);
    }
}

- (IBAction)menuClickFixedLength:(id)sender
{
    MgSelection *sel = mgGetCommandManager()->getSelection(_mgview);
    if (sel) {
        sel->setFixedLength(_mgview, !sel->isFixedLength(_mgview));
    }
}

@end
