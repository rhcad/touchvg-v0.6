// GiViewController.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchvg

#import "GiViewController.h"
#include <mgshapest.h>
#include <list>
#import "GiCmdController.h"
#import "GiGraphView.h"
#include <string.h>

@interface GiViewController(GestureRecognizer)

- (id<GiView>)gview;
- (id<GiMotionHandler>)motionView:(SEL)aSelector;
- (id<GiMotionHandler>)getCommand:(SEL)aSelector;
- (void)updateMagnifierCenter:(UIGestureRecognizer *)sender;

- (void)addGestureRecognizers:(int)t view:(UIView*)view;
- (void)setGestureRecognizerEnabled:(BOOL)enabled;

- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender;
- (void)twoFingersPan:(UIPanGestureRecognizer *)sender;
- (void)oneFingerPan:(UIPanGestureRecognizer *)sender;
- (void)twoFingersTwoTaps:(UITapGestureRecognizer *)sender;
- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender;
- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender;

@end

@implementation GiViewController

@synthesize gestureRecognizerUsed = _gestureRecognizerUsed;
@synthesize magnifierView;
@synthesize activeView = _activeView;
@synthesize lineWidth;
@synthesize strokeWidth;
@synthesize lineColor;
@synthesize fillColor;
@synthesize lineAlpha;
@synthesize fillAlpha;
@synthesize lineStyle;
@synthesize commandName;
@synthesize shapes;

- (id)init
{
    self = [super init];
    if (self) {
        for (int iv = 0; iv < 3; iv++)                      // 末尾的nil用于结束占位
            _magViews[iv] = Nil;
        _cmdctl = [[GiCommandController alloc]initWithViews:_magViews];
        _shapesCreated = NULL;
        for (int t = 0; t < 2; t++) {
            for (int i = 0; i < RECOGNIZER_COUNT; i++)
                _recognizers[t][i] = Nil;
        }
        _gestureRecognizerUsed = YES;
    }
    return self;
}

- (void)viewDidLoad
{
    if ([self gview]) {
        _activeView = self.view;
        
        [[self gview] setDrawingDelegate:self];
        [self addGestureRecognizers:0 view:self.view];
        
        GiCommandController* cmd = (GiCommandController*)_cmdctl;
        [cmd touchesBegan:CGPointZero view:_activeView];    // 传入View
        cmd.lineWidth = 50;                                 // 默认画笔0.5mm
        cmd.lineColor = GiColor(0, 0, 0, 128);              // 默认黑色画笔，50%透明
    }
}

- (void)dealloc
{
    [_cmdctl release];
    
    if (_shapesCreated) {
        ((MgShapes*)_shapesCreated)->release();
        _shapesCreated = NULL;
    }
    for (int t = 0; t < 2; t++) {
        for (int i = 0; i < RECOGNIZER_COUNT; i++) {
            [_recognizers[t][i] release];
            _recognizers[t][i] = Nil;
        }
    }
    [super dealloc];
}

- (UIView*)createGraphView:(UIView*)parentView frame:(CGRect)frame backgroundColor:(UIColor*)bkColor
{
    GiGraphView *aview = [[GiGraphView alloc] initWithFrame:frame];
    
    if (self.view)
        [self.view removeFromSuperview];
    self.view = aview;
    
    if (!bkColor)
        bkColor = parentView.superview.backgroundColor;
    [aview graph]->setBkColor(giFromUIColor(bkColor ? bkColor : [UIColor whiteColor]));
    aview.backgroundColor = [UIColor clearColor];
    
    [aview setDrawingDelegate:self];
    [parentView addSubview:aview];
    
    if (_shapesCreated) {
        aview.shapes = (MgShapes*)_shapesCreated;
    }
    else {
        aview.shapes = new MgShapesT<std::list<MgShape*> >;
        _shapesCreated = aview.shapes;
    }
    
    [self viewDidLoad];
    
    [aview release];
    return self.view;
}

- (UIView*)createSubGraphView:(UIView*)parentView frame:(CGRect)frame
                       shapes:(void*)sp backgroundColor:(UIColor*)bkColor
{
    GiGraphView *aview = [[GiGraphView alloc] initWithFrame:frame];
    
    if (self.view)
        [self.view removeFromSuperview];
    self.view = aview;
    
    if (!bkColor)
        bkColor = parentView.superview.backgroundColor;
    [aview graph]->setBkColor(giFromUIColor(bkColor ? bkColor : [UIColor whiteColor]));
    aview.backgroundColor = [UIColor clearColor];
    aview.enableZoom = NO;
    
    [aview setDrawingDelegate:self];
    [parentView addSubview:aview];
    
    aview.shapes = (MgShapes*)(sp ? sp : _shapesCreated);
    if (!aview.shapes)
    {
        aview.shapes = new MgShapesT<std::list<MgShape*> >;
        _shapesCreated = aview.shapes;
    }
    
    [self viewDidLoad];
    
    [aview release];
    return self.view;
}

- (UIView*)createMagnifierView:(UIView*)parentView frame:(CGRect)frame scale:(CGFloat)scale
{
    if (_magViews[_magViews[0] ? 1 : 0])                        // 最多2个
        return Nil;
    
    GiMagnifierView *aview = [[GiMagnifierView alloc] initWithFrame:frame graphView:[self gview]];
    _magViews[_magViews[0] ? 1 : 0] = aview;
    aview.backgroundColor = [UIColor clearColor];
    aview.scale = scale;
    
    [aview setDrawingDelegate:self];
    [parentView addSubview:aview];
    [self addGestureRecognizers:1 view:aview];
    
    [aview release];
    return aview;
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
    [self clearCachedData];
}

- (void)clearCachedData
{
    [[self gview] graph]->clearCachedBitmap();
}

- (CGImageRef)cachedBitmap:(BOOL)invert
{
    GiGraphView *aview = (GiGraphView *)self.view;
    return [aview cachedBitmap:invert];
}

- (UIView*)magnifierView {
    return _magViews[0];
}

- (void)removeShapes
{
    MgShapesLock locker([[self gview] shapes]);
    [[self gview] shapes]->clear();
    [self regen];
}

- (BOOL)loadShapes:(void*)mgstorage
{
    bool ret = [[self gview] shapes]->load((MgStorage*)mgstorage);
    [self regen];
    return ret;
}

- (BOOL)saveShapes:(void*)mgstorage
{
    return [[self gview] shapes]->save((MgStorage*)mgstorage);
}

- (void*)shapes {
    return [[self gview] shapes];
}

- (BOOL)isCommand:(const char*)cmdname
{
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    return strcmp(cmd.commandName, cmdname) == 0;
}

- (const char*)commandName {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    return cmd.commandName;
}

- (void)setCommandName:(const char*)name {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    cmd.commandName = name;
}

- (float)lineWidth {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    return cmd.lineWidth;
}

- (void)setLineWidth:(float)w {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    [cmd setLineWidth:w];
}

- (float)strokeWidth {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    float w = cmd.lineWidth;
    if (w < 0)                      // 表示单位为像素
        w = -w;
    else if (w > 0) {               // 单位为0.01mm
        w = [[self gview]graph]->calcPenWidth(w);   // 转为当前显示比例下的像素宽
    }
    return w;
}

- (void)setStrokeWidth:(float)w {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    [cmd setLineWidth:-w];          // 传入正数像素宽，内部图形系统负数表示像素单位
}

- (UIColor*)lineColor {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiColor c = cmd.lineColor;
    return [UIColor colorWithRed:c.r green:c.g blue:c.b alpha:1];
}

- (void)setLineColor:(UIColor*)c {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiColor color(giFromUIColor(c));
    if (color.a > 0.01f && cmd.lineColor.a > 0)     // 只要不是完全透明
        color.a = cmd.lineColor.a;                  // 就不改变透明度
    [cmd setLineColor:color];
}

- (UIColor*)fillColor {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiColor c = cmd.fillColor;
    return [UIColor colorWithRed:c.r green:c.g blue:c.b alpha:1];
}

- (void)setFillColor:(UIColor*)c {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiColor color(giFromUIColor(c));
    if (color.a > 0.01f && cmd.fillColor.a > 0)     // 只要不是完全透明
        color.a = cmd.fillColor.a;                  // 就不改变透明度
    [cmd setFillColor:color];
}

- (float)lineAlpha {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    return cmd.lineColor.a / 255.0f;
}

- (void)setLineAlpha:(float)a {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiColor color(cmd.lineColor);
    color.a = mgRound(a * 255);
    [cmd setLineColor:color];
}

- (float)fillAlpha {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    return cmd.fillColor.a / 255.0f;
}

- (void)setFillAlpha:(float)a {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiColor color(cmd.fillColor);
    color.a = mgRound(a * 255);
    [cmd setFillColor:color];
}

- (int)lineStyle {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    return cmd.lineStyle;
}

- (void)setLineStyle:(int)style {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    [cmd setLineStyle:style];
}

- (BOOL)dynamicChangeEnded:(BOOL)apply
{
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    return [cmd dynamicChangeEnded:apply];
}

#pragma mark - View motion

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return YES;     // supported orientations
}

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)orientation duration:(NSTimeInterval)duration
{
    [[self gview] setAnimating:YES];
    [super willRotateToInterfaceOrientation:orientation duration:duration];
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
    [[self gview] setAnimating:NO];
    [super didRotateFromInterfaceOrientation:fromInterfaceOrientation];
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    [self becomeFirstResponder];
}

- (void)viewDidDisappear:(BOOL)animated {
    [self resignFirstResponder];
    [super viewDidDisappear:animated];
}

- (BOOL)canBecomeFirstResponder {
    return YES;
}

- (void)motionEnded:(UIEventSubtype)motion withEvent:(UIEvent *)event
{
    if (motion == UIEventSubtypeMotionShake) {
        [self undoMotion];
    }
}

- (void)undoMotion
{
    if (![[self getCommand:@selector(undoMotion)] undoMotion])
        [[self motionView:@selector(undoMotion)] undoMotion];
}

- (void)dynDraw:(id)sender
{
    GiGraphics* gs = NULL;
    
    if ([sender conformsToProtocol:@protocol(GiView)]) {
        id<GiView> aview = (id<GiView>)sender;
        gs = [aview graph];
    }
    else if (sender == _magViews[0]) {
        GiMagnifierView *aview = (GiMagnifierView *)_magViews[0];
        gs = [aview graph];
    }
    else if (sender == _magViews[1]) {
        GiMagnifierView *aview = (GiMagnifierView *)_magViews[1];
        gs = [aview graph];
    }
    
    if (gs && gs->isDrawing()) {
        [[self getCommand:@selector(dynDraw:)] dynDraw: gs];
    }
}

- (void)regen
{
    [[self gview] regen];
    
    GiMagnifierView *magview = (GiMagnifierView *)_magViews[0];
    [magview regen];
    magview = (GiMagnifierView *)_magViews[1];
    [magview regen];
}

- (void)afterZoomed:(id)sender
{
    [self.view setNeedsDisplay];
    [_magViews[0] setNeedsDisplay];
    [_magViews[1] setNeedsDisplay];
}

- (UIGestureRecognizer*) getGestureRecognizer:(int)index
{
    return index >= 0 && index < RECOGNIZER_COUNT ? _recognizers[0][index] : NULL;
}

// 手势即将开始，在 touchesBegan 后发生，即将调用本类的相应手势响应函数
- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
    BOOL allow = YES;
    NSTimeInterval seconds = [[NSProcessInfo processInfo]systemUptime] - _timeBegan;
    
    if (seconds > 0.8) {    // 从按下到开始移动经过的秒数超过阀值，就向当前命令触发长按动作
        allow = ![[self getCommand:@selector(longPressGesture:)] longPressGesture:gestureRecognizer];
    }
    
    return allow;           // 响应了长按消息时则取消当前手势
}

static CGPoint _ignorepoint = CGPointMake(-1000, -1000);    // 全局屏幕坐标

+ (void)ignoreTouchesBegan:(CGPoint)point view:(UIView*)sender {
    _ignorepoint = [sender convertPoint:point toView:[sender window]];
}

@end

@implementation GiViewController(GestureRecognizer)

- (id<GiView>)gview
{
    if ([self.view conformsToProtocol:@protocol(GiView)])
        return (id<GiView>)self.view;
    return Nil;
}

- (id<GiMotionHandler>)motionView:(SEL)aSelector
{
    if ([self.view conformsToProtocol:@protocol(GiMotionHandler)]
        && [self.view respondsToSelector:aSelector]) {
        return (id<GiMotionHandler>)self.view;
    }
    return Nil;
}

- (id<GiMotionHandler>)getCommand:(SEL)aSelector
{
    return [_cmdctl respondsToSelector:aSelector] ?
        (id<GiMotionHandler>)_cmdctl : Nil;
}

- (void)addGestureRecognizers:(int)t view:(UIView*)view
{
    if (_recognizers[t][0])
        return;
    
    int n = 0;
    
    // 双指捏合手势
    UIPinchGestureRecognizer *twoFingersPinch =
    [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPinch:)];
    twoFingersPinch.delegate = self;                                // 用于检测长按
    _recognizers[t][n++] = twoFingersPinch;
    
    // 双指滑动手势
    UIPanGestureRecognizer *twoFingersPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPan:)];
    twoFingersPan.maximumNumberOfTouches = 2;
    twoFingersPan.minimumNumberOfTouches = 2;
    [twoFingersPan requireGestureRecognizerToFail:twoFingersPinch]; // 捏合优先
    _recognizers[t][n++] = twoFingersPan;
    
    // 单指滑动手势
    UIPanGestureRecognizer *oneFingerPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerPan:)];
    oneFingerPan.maximumNumberOfTouches = 2;                        // 同时识别双指滑动
    oneFingerPan.delegate = self;                                   // 用于检测长按
    _recognizers[t][n++] = oneFingerPan;
    
    // 单指点击手势
    UITapGestureRecognizer *oneFingerOneTap =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerOneTap:)];
    [oneFingerOneTap requireGestureRecognizerToFail:oneFingerPan];  // 不是滑动才算点击
    oneFingerOneTap.delegate = self;                                // 用于检测长按
    _recognizers[t][n++] = oneFingerOneTap;
    
    // 单指双击手势
    UITapGestureRecognizer *oneFingerTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerTwoTaps:)];
    oneFingerTwoTaps.numberOfTapsRequired = 2;
    _recognizers[t][n++] = oneFingerTwoTaps;
    
    // 双指双击手势
    UITapGestureRecognizer *twoFingersTwoTaps =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersTwoTaps:)];
    twoFingersTwoTaps.numberOfTapsRequired = 2;
    twoFingersTwoTaps.numberOfTouchesRequired = 2;
    _recognizers[t][n++] = twoFingersTwoTaps;
    
    _touchCount = 0;
    if (_gestureRecognizerUsed) {
        for (int i = 0; i < RECOGNIZER_COUNT; i++) {
            [view addGestureRecognizer:_recognizers[t][i]];
        }
    }
}

- (void)setGestureRecognizerUsed:(BOOL)used
{
    if (_gestureRecognizerUsed) {
        for (int i = 0; i < RECOGNIZER_COUNT; i++) {
            [self.view removeGestureRecognizer:_recognizers[0][i]];
            [_magViews[0] removeGestureRecognizer:_recognizers[1][i]];
        }
    }
    _gestureRecognizerUsed = used;
    if (_gestureRecognizerUsed) {
        for (int i = 0; i < RECOGNIZER_COUNT; i++) {
            [self.view addGestureRecognizer:_recognizers[0][i]];
            [_magViews[0] addGestureRecognizer:_recognizers[1][i]];
        }
    }
}

- (void)setGestureRecognizerEnabled:(BOOL)enabled
{
    for (int i = 0; i < RECOGNIZER_COUNT; i++) {
        _recognizers[0][i].enabled = enabled;
        _recognizers[1][i].enabled = enabled;
    }
}

// 某个手指接触到屏幕，先于 gestureRecognizerShouldBegin 发生
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    CGPoint point = [touch locationInView:touch.view];
    CGPoint ignorept = [touch.view convertPoint:_ignorepoint fromView:[touch.view window]];
    
    _ignorepoint = CGPointMake(-1000, -1000);
    _ignoreTouches = CGPointEqualToPoint(point, ignorept);
    
    _activeView = touch.view;
    if (_timeBegan < 0.1) {                             // 是第一个触点
        GiCommandController* cmd = (GiCommandController*)_cmdctl;
        [cmd touchesBegan:point view:touch.view];
        _timeBegan = touch.timestamp;                   // 第一个触点的时刻
    }
    
    if (touch.view == self.view) {
        [super touchesBegan:touches withEvent:event];
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    
    _timeBegan = 0;
    
    if (touch.view == self.view) {
        [super touchesEnded:touches withEvent:event];
    }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    
    if (touch) {
        CGPoint point = [touch locationInView:touch.view];
        GiCommandController* cmd = (GiCommandController*)_cmdctl;
        [cmd delayTap:point view:touch.view];           // 看是否有点击待处理
    }
    _timeBegan = 0;
    
    if (!touch || touch.view == self.view) {
        [super touchesCancelled:touches withEvent:event];
    }
}

- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    if (_ignoreTouches) {
        sender.cancelsTouchesInView = YES;
        return;
    }
    if (sender.state == UIGestureRecognizerStateBegan) {
        GiCommandController* cmd = (GiCommandController*)_cmdctl;
        NSTimeInterval seconds = [[NSProcessInfo processInfo]systemUptime] - _timeBegan;
        [cmd setTapDragMode:seconds > 0.8];
    }
    if (![[self getCommand:@selector(twoFingersPinch:)] twoFingersPinch:sender]
        && sender.view == self.view) {
        [[self motionView:@selector(twoFingersPinch:)] twoFingersPinch:sender];
    }
    [self updateMagnifierCenter:sender];
}

- (void)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    if (_ignoreTouches) {
        sender.cancelsTouchesInView = YES;
        return;
    }
    if (sender.state == UIGestureRecognizerStateBegan) {
        _touchCount = [sender numberOfTouches];
    }
    if (1 == _touchCount) {
        [self oneFingerPan:sender];
    }
    else if (sender.view == _magViews[0]) {
        GiMagnifierView *zview = (GiMagnifierView *)_magViews[0];
        [zview twoFingersPan:sender];
    }
    else if (sender.view == _magViews[1]) {
        GiMagnifierView *zview = (GiMagnifierView *)_magViews[1];
        [zview twoFingersPan:sender];
    }
    else if (_touchCount > 1) {
        if (![[self getCommand:@selector(twoFingersPan:)] twoFingersPan:sender]) {
            [[self motionView:@selector(twoFingersPan:)] twoFingersPan:sender];
        }
        [self updateMagnifierCenter:sender];
    }
}

- (void)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    if (_ignoreTouches) {
        sender.cancelsTouchesInView = YES;
        return;
    }
    if (sender.state == UIGestureRecognizerStateBegan) {
        GiCommandController* cmd = (GiCommandController*)_cmdctl;
        NSTimeInterval seconds = [[NSProcessInfo processInfo]systemUptime] - _timeBegan;
        bool tapdrag = seconds > 0.8 && [sender numberOfTouches] == 2;
        
        [cmd setTapDragMode:tapdrag];
        _touchCount = tapdrag ? 1 : [sender numberOfTouches]; // Tap+Drag 视为单指滑动
    }
    if (2 == _touchCount) {
        [self twoFingersPan:sender];
    }
    else if (1 == _touchCount) {
        if (![[self getCommand:@selector(oneFingerPan:)] oneFingerPan:sender]
            && sender.view == self.view) {
            [[self motionView:@selector(oneFingerPan:)] oneFingerPan:sender];
        }
        [self updateMagnifierCenter:sender];
    }
}

- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender
{
    if (_ignoreTouches) {
        sender.cancelsTouchesInView = YES;
        return;
    }
    if (![[self getCommand:@selector(oneFingerOneTap:)] oneFingerOneTap:sender]
        && sender.view == self.view) {
        [[self motionView:@selector(oneFingerOneTap:)] oneFingerOneTap:sender];
    }
    [self updateMagnifierCenter:sender];
}

- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender
{
    if (_ignoreTouches) {
        sender.cancelsTouchesInView = YES;
        return;
    }
    if (![[self getCommand:@selector(oneFingerTwoTaps:)] oneFingerTwoTaps:sender]
        && sender.view == self.view) {
        [[self motionView:@selector(oneFingerTwoTaps:)] oneFingerTwoTaps:sender];
    }
    [self updateMagnifierCenter:sender];
}

- (void)twoFingersTwoTaps:(UITapGestureRecognizer *)sender
{
    if (_ignoreTouches) {
        sender.cancelsTouchesInView = YES;
        return;
    }
    if (![[self getCommand:@selector(twoFingersTwoTaps:)] twoFingersTwoTaps:sender]
        && sender.view == self.view) {
        [[self motionView:@selector(twoFingersTwoTaps:)] twoFingersTwoTaps:sender];
    }
    [self updateMagnifierCenter:sender];
}

- (void)updateMagnifierCenter:(UIGestureRecognizer *)sender
{
    if (!_magViews[0] || sender.view != self.view)
        return;
    
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiMagnifierView *zview = (GiMagnifierView *)_magViews[0];
    
    if ([sender numberOfTouches] > 0) {
        [zview automoveSuperview:[sender locationInView:sender.view] fromView:self.view];
    }
    
    [zview setPointW:[cmd getPointModel]];
    //if (sender.state == UIGestureRecognizerStateEnded) [zview setPointWandRedraw
}

@end
