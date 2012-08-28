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
- (BOOL)gestureCheck:(UIGestureRecognizer*)sender;

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
        _shapesDynamic = NULL;
        _dynChangeCount[0] = 0;
        _dynChangeCount[1] = 0;
        _recordIndex = 0;
        
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
        [cmd touchesBegan:CGPointZero view:_activeView count:0];    // 传入View
        cmd.lineWidth = 50;                                 // 默认画笔0.5mm
        cmd.lineColor = GiColor(0, 0, 0, 128);              // 默认黑色画笔，50%透明
    }
}

- (void)dealloc
{
    [_cmdctl release];
    
    if (_shapesDynamic) {
        ((MgShapes*)_shapesDynamic)->release();
        _shapesDynamic = NULL;
    }
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

- (UIView*)createGraphView:(UIView*)parentView frame:(CGRect)frame
           backgroundColor:(UIColor*)bkColor
{
    GiGraphView *aview = [[GiGraphView alloc] initWithFrame:frame];
    
    if (self.view)
        [self.view removeFromSuperview];
    self.view = aview;
    
    if (!bkColor) {
        bkColor = parentView.backgroundColor;
        if (!bkColor)
            bkColor = parentView.superview.backgroundColor;
    }
    [aview graph]->setBkColor(giFromUIColor(bkColor ? bkColor : [UIColor whiteColor]));
    
    BOOL hasColor = parentView.backgroundColor || parentView.superview.backgroundColor;
    aview.backgroundColor = hasColor ? [UIColor clearColor] : [UIColor whiteColor];
    
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
    
    if (!bkColor) {
        bkColor = parentView.backgroundColor;
        if (!bkColor)
            bkColor = parentView.superview.backgroundColor;
    }
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
    _recognizers[0][kPinchGesture].enabled = NO;
    _recognizers[0][kTwoFingersPan].enabled = NO;
    _recognizers[0][kTwoFingersTwoTaps].enabled = NO;
    
    [aview release];
    return self.view;
}

- (UIView*)createMagnifierView:(UIView*)parentView
                         frame:(CGRect)frame scale:(CGFloat)scale
{
    if (_magViews[_magViews[0] ? 1 : 0])                        // 最多2个
        return Nil;
    
    GiMagnifierView *aview = [[GiMagnifierView alloc]
                              initWithFrame:frame graphView:[self gview]];
    _magViews[_magViews[0] ? 1 : 0] = aview;
    aview.backgroundColor = [UIColor clearColor];
    aview.scale = scale;
    
    UIColor* bkColor = parentView.backgroundColor;
    if (!bkColor)
        bkColor = parentView.superview.backgroundColor;
    [aview graph]->setBkColor(giFromUIColor(bkColor ? bkColor : [UIColor whiteColor]));
    
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
    MgShapesLock locker([[self gview] shapes], MgShapesLock::Edit);
    [[self gview] shapes]->clear();
    [self regen];
    
    // 清除播放图形列表
    GiGraphView *gview = (GiGraphView *)self.view;
    [gview getPlayShapes:YES];
    [self setDynamicShapes:NULL];
}

- (BOOL)loadShapes:(void*)mgstorage
{
    MgShapes* sp = [[self gview] shapes];
    MgShapesLock locker(sp, MgShapesLock::Edit);
    BOOL ret = (locker.locked() && mgstorage
                && sp->load((MgStorage*)mgstorage));
    [self regen];
    
    return ret;
}

- (BOOL)saveShapes:(void*)mgstorage
{
    MgShapesLock locker([[self gview] shapes], MgShapesLock::ReadOnly);
    bool ret = locker.locked() && mgstorage;
    
    if (ret) {
        ret = locker.shapes->save((MgStorage*)mgstorage);
        locker.resetEditFlags();
    }
    
    return ret;
}

- (BOOL)record:(void*)mgstorage
{
    MgStorage* s = (MgStorage*)mgstorage;
    MgShapesLock locker([[self gview] shapes], MgShapesLock::ReadOnly);
    bool ret = locker.locked() && mgstorage;
    
    if (ret) {
        GiGraphView *gview = (GiGraphView *)self.view;
        [gview getPlayShapes:YES];                      // 清除播放图形列表
        
        int mode = locker.getEditFlags();               // 修改标志
        bool addOnly = (MgShapesLock::Add == mode);
        
        s->writeNode("record", -1, false);
        s->writeInt32("mode", mode);
        
        ret = locker.shapes->save(s, addOnly ? _recordIndex : 0);   // 增量保存
        s->writeNode("record", -1, true);
        
        if (!addOnly) {
            _recordIndex = locker.shapes->getShapeCount();  // 下次新加图形时的序号
        }
        locker.resetEditFlags();                            // 清除修改标志，表示已录屏
    }
    
    return ret;
}

- (BOOL)playback:(void*)mgstorage
{
    MgStorage* s = (MgStorage*)mgstorage;
    GiGraphView *gview = (GiGraphView *)self.view;
    MgShapes* sp = [gview getPlayShapes:!s];            // 播放图形列表，自动创建或删除
    MgShapesLock locker(sp, MgShapesLock::Edit);        // 锁定改写播放图形列表
    BOOL ret = !sp || locker.locked();                  // 删除或锁定成功
    
    if (locker.locked() && s->readNode("record", -1, false))
    {            
        int mode = s->readInt32("mode");                // 录制的标记
        bool addOnly = (MgShapesLock::Add == mode);
        UInt32 oldCount = sp->getShapeCount();          // 原来的图形数
        
        ret = sp->load(s, addOnly);                     // 增量播放
        s->readNode("record", -1, true);
        
        if (!addOnly || oldCount + 1 != sp->getShapeCount()) {
            [self regen];
        }
        else {                                              // 是增量录制最后一个图形
            [[self gview] shapeAdded:sp->getLastShape()];   // 仅添加显示一个图形
        }
    }
    
    return ret;
}

- (NSUInteger)getChangeCount
{
    return _dynChangeCount[0];
}

- (NSUInteger)getRedrawCount
{
    return _dynChangeCount[1];
}

- (BOOL)getDynamicShapes:(void*)mgstorage
{
    MgDynShapeLock locker(false);       // 锁定以便读取, 临界区是动态图形
    bool ret = locker.locked();
    
    if (ret) {
        GiCommandController* cmd = (GiCommandController*)_cmdctl;
        GiGraphView *gview = (GiGraphView *)self.view;
        MgShapesT<std::list<MgShape*> > sp(false);
        
        [cmd getDynamicShapes:&sp];         // 从当前命令取动态图形
        
        if (sp.getShapeCount() == 0) {
            MgShapesLock locker2([[self gview] shapes], MgShapesLock::ReadOnly);
            if (gview.shapeAdded && locker2.locked()
                && locker2.getEditFlags() == MgShapesLock::Add) {
                sp.addShape(*gview.shapeAdded);
            }
            else {
                return NO;
            }
        }
        ret = sp.save((MgStorage*)mgstorage);  // 存到mgstorage
        
        [self setDynamicShapes:NULL];   // 录时取消动态播放
    }
    
    return ret;
}

- (BOOL)setDynamicShapes:(void*)mgstorage
{
    BOOL ret = YES;
    
    if (!mgstorage) {
        if (_shapesDynamic) {
            MgShapesLock locker((MgShapes*)_shapesDynamic, MgShapesLock::Edit);
            if (locker.shapes) {
                locker.shapes->release();
                locker.shapes = NULL;
            }
            _shapesDynamic = NULL;
        }
    }
    else {
        if (!_shapesDynamic) {
            _shapesDynamic = new MgShapesT<std::list<MgShape*> >(false);
        }
        
        // 优先写到最旧的临时图形列表，不能锁定则换另一个临时图形列表
        MgShapesLock locker((MgShapes*)_shapesDynamic, MgShapesLock::Edit);
        
        ret = locker.locked();
        if (ret) {
            MgShapes* sp = (MgShapes*)_shapesDynamic;
            ret = sp->load((MgStorage*)mgstorage);          // 写到临时图形列表
        }
    }
    
    return ret;
}

- (void)afterShapeChanged
{
    while (_dynChangeCount[0] < [[self gview] shapes]->getChangeCount()) {
        giInterlockedIncrement(_dynChangeCount);
    }
}

- (id)dynDraw:(id)sender
{
    GiGraphics* gs = NULL;
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiGraphView *gview = (GiGraphView *)self.view;
    
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
        // 优先取最新的临时图形列表，不能锁定则换另一个临时图形列表
        MgShapesLock locker((MgShapes*)_shapesDynamic, MgShapesLock::ReadOnly);
        
        if (locker.locked()) {
            if (locker.shapes->getShapeCount() != 1) {
                locker.shapes->draw(*gs);
            }
            else {
                void *it;
                MgShape *tmpShape = locker.shapes->getFirstShape(it);
                MgShape *added = [gview shapeAdded];
                
                if (tmpShape && added
                    && tmpShape->shape()->getExtent() == added->shape()->getExtent()
                    && tmpShape->shape()->getPointCount() == added->shape()->getPointCount()) {
                    locker.shapes->clear();
                }
                else {
                    locker.shapes->draw(*gs);
                }
            }
        }
        else if (_shapesDynamic) {                      // 冲突则下次再显示
            sender = nil;
        }
    }
    if (gs && gs->isDrawing()) {
        // 延迟到显示完成才检查动态图形是否已改变，自增一让外界知道已改变
        if ([cmd isDynamicChanged:YES]) {
            giInterlockedIncrement(_dynChangeCount + 1);
        }
        
        [cmd dynDraw: gs];
    }
    
    return sender;
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
    
    // 清除播放图形列表
    GiGraphView *gview = (GiGraphView *)self.view;
    [gview getPlayShapes:YES];
    [self setDynamicShapes:NULL];
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

#pragma mark - GiEditAction implement

- (IBAction)menuClickDraw:(id)sender
{
    [_cmdctl menuClickDraw:sender];
}

- (IBAction)menuClickSelAll:(id)sender
{
    [_cmdctl menuClickSelAll:sender];
}

- (IBAction)menuClickReset:(id)sender
{
    [_cmdctl menuClickReset:sender];
}

- (IBAction)menuClickDelete:(id)sender
{
    [_cmdctl menuClickDelete:sender];
}

- (IBAction)menuClickClone:(id)sender
{
    [_cmdctl menuClickClone:sender];
}

- (IBAction)menuClickClosed:(id)sender
{
    [_cmdctl menuClickClosed:sender];
}

- (IBAction)menuClickAddNode:(id)sender
{
    [_cmdctl menuClickAddNode:sender];
}

- (IBAction)menuClickDelNode:(id)sender
{
    [_cmdctl menuClickDelNode:sender];
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
    
    // 长按手势: 当前命令响应长按操作时手势才生效
    if (gestureRecognizer == _recognizers[0][kLongPressGesture]
        || gestureRecognizer == _recognizers[1][kLongPressGesture]) {
        allow = [[self getCommand:@selector(longPressGesture:)] longPressGesture:gestureRecognizer];
    }
    // 从按下到开始移动经过的秒数超过阀值，就向当前命令触发长按动作，当前命令响应长按操作时手势取消
    else if (seconds > 0.8) {
        allow = ![[self getCommand:@selector(longPressGesture:)] longPressGesture:gestureRecognizer];
    }
    // 单指平移手势开始时只能有一个触点，只在移动过程中识别第二个触点
    else if (gestureRecognizer == _recognizers[0][kPanGesture]
             || gestureRecognizer == _recognizers[1][kPanGesture]) {
        allow = [gestureRecognizer numberOfTouches] == 1;
    }
    
    return allow;
}

static CGPoint _ignorepoint = CGPointMake(-1000, -1000);    // 全局屏幕坐标

+ (void)ignoreTouchesBegan:(CGPoint)point view:(UIView*)sender {
    _ignorepoint = [sender convertPoint:point toView:[sender window]];
}

- (void)gestureStateChanged:(UIGestureRecognizer*)sender {
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
    return [_cmdctl respondsToSelector:aSelector] ? (id<GiMotionHandler>)_cmdctl : nil;
}

- (void)addGestureRecognizers:(int)t view:(UIView*)view
{
    if (_recognizers[t][2])
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
    oneFingerPan.maximumNumberOfTouches = 2;                        // 滑动中允许双指
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
    
    // 单指长按手势
    UILongPressGestureRecognizer *longPressGesture =
    [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longPressGesture)];
    longPressGesture.minimumPressDuration = 0.8;
    longPressGesture.delegate = self;                               // 用于检测长按
    _recognizers[t][n++] = longPressGesture;
    
    _touchCount = 0;
    if (_gestureRecognizerUsed) {
        for (int i = 0; i < RECOGNIZER_COUNT; i++) {
            if (_recognizers[t][i]) {
                [view addGestureRecognizer:_recognizers[t][i]];
            }
        }
    }
}

- (void)setGestureRecognizerUsed:(BOOL)used
{
    if (_gestureRecognizerUsed) {
        for (int i = 0; i < RECOGNIZER_COUNT; i++) {
            if (_recognizers[0][i]) {
                [self.view removeGestureRecognizer:_recognizers[0][i]];
            }
            if (_recognizers[1][i]) {
                [_magViews[0] removeGestureRecognizer:_recognizers[1][i]];
            }
        }
    }
    _gestureRecognizerUsed = used;
    if (_gestureRecognizerUsed) {
        for (int i = 0; i < RECOGNIZER_COUNT; i++) {
            if (_recognizers[0][i]) {
                [self.view addGestureRecognizer:_recognizers[0][i]];
            }
            if (_recognizers[1][i]) {
                [_magViews[0] addGestureRecognizer:_recognizers[1][i]];
            }
        }
    }
}

- (void)setGestureRecognizerEnabled:(BOOL)enabled
{
    for (int i = 0; i < RECOGNIZER_COUNT; i++) {
        if (_recognizers[0][i]) {
            _recognizers[0][i].enabled = enabled;
        }
        if (_recognizers[1][i]) {
            _recognizers[1][i].enabled = enabled;
        }
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
    int count = _timeBegan < 0.1 ? 1 : 2;
    if (_timeBegan < 0.1) {                             // 是第一个触点
        _timeBegan = touch.timestamp;                   // 第一个触点的时刻
    }
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    [cmd touchesBegan:point view:touch.view count:count];
    
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
    
    if (touch) {            // oneFingerOneTap不直接处理是为了检测是点击还是短划动
        CGPoint point = [touch locationInView:touch.view];
        GiCommandController* cmd = (GiCommandController*)_cmdctl;
        [cmd delayTap:point view:touch.view];           // 看是否有点击待处理
    }
    _timeBegan = 0;
    
    if (!touch || touch.view == self.view) {
        [super touchesCancelled:touches withEvent:event];
    }
}

- (BOOL)gestureCheck:(UIGestureRecognizer*)sender
{
    if (_ignoreTouches) {
        sender.cancelsTouchesInView = YES;
        return NO;
    }
    if (MgShapesLock::lockedForRead([[self gview] shapes])      // 如果正在录屏复制
        || MgDynShapeLock::lockedForRead()) {
        if (sender.state == UIGestureRecognizerStateChanged)    // 触摸移动则忽略本次
            return NO;
        MgShapesLock([[self gview] shapes], MgShapesLock::Unknown);     // 等待
        MgDynShapeLock();                                       // 等待录屏复制完成
    }
    if (sender.state == UIGestureRecognizerStateBegan
        || sender.state != UIGestureRecognizerStateChanged) {
        [self gestureStateChanged:sender];
    }
    
    return YES;
}

- (void)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    if (![self gestureCheck:sender])
        return;
    
    if (![[self getCommand:@selector(twoFingersPinch:)] twoFingersPinch:sender]
        && sender.view == self.view) {
        [[self motionView:@selector(twoFingersPinch:)] twoFingersPinch:sender];
    }
    [self updateMagnifierCenter:sender];
}

- (void)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    if (![self gestureCheck:sender])
        return;
    
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
    if (![self gestureCheck:sender])
        return;
    
    if (![[self getCommand:@selector(oneFingerPan:)] oneFingerPan:sender]
        && sender.view == self.view) {
        [[self motionView:@selector(oneFingerPan:)] oneFingerPan:sender];
    }
    [self updateMagnifierCenter:sender];
}

- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender
{
    if (![self gestureCheck:sender])
        return;
    
    if (![[self getCommand:@selector(oneFingerOneTap:)] oneFingerOneTap:sender]
        && sender.view == self.view) {
        [[self motionView:@selector(oneFingerOneTap:)] oneFingerOneTap:sender];
    }
    [self updateMagnifierCenter:sender];
}

- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender
{
    if (![self gestureCheck:sender])
        return;
    
    if (![[self getCommand:@selector(oneFingerTwoTaps:)] oneFingerTwoTaps:sender]
        && sender.view == self.view) {
        [[self motionView:@selector(oneFingerTwoTaps:)] oneFingerTwoTaps:sender];
    }
    [self updateMagnifierCenter:sender];
}

- (void)twoFingersTwoTaps:(UITapGestureRecognizer *)sender
{
    if (![self gestureCheck:sender])
        return;
    
    if (![[self getCommand:@selector(twoFingersTwoTaps:)] twoFingersTwoTaps:sender]
        && sender.view == self.view) {
        [[self motionView:@selector(twoFingersTwoTaps:)] twoFingersTwoTaps:sender];
    }
    [self updateMagnifierCenter:sender];
}

- (void)longPressGesture
{
    // gestureRecognizerShouldBegin 已经处理了
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
