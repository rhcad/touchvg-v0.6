// GiViewController.mm
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/touchvg

#import "GiViewController.h"
#import "GiCmdController.h"
#import "GiGraphView.h"
#include <string.h>
#include <iosgraph.h>
#include <mgbasicsp.h>
#include <mgstorage.h>
#include <list>

@interface GiViewController(GestureRecognizer)

- (id<GiView>)gview;
- (id<GiMotionHandler>)motionView:(SEL)aSelector;
- (id<GiMotionHandler>)getCommand:(SEL)aSelector;
- (void)updateMagnifierCenter:(UIGestureRecognizer *)sender;

- (void)addGestureRecognizers:(int)t view:(UIView*)view;
- (BOOL)gestureCheck:(UIGestureRecognizer*)sender;

- (BOOL)twoFingersPinch:(UIPinchGestureRecognizer *)sender;
- (BOOL)twoFingersRotate:(UIRotationGestureRecognizer *)sender;
- (BOOL)twoFingersPan:(UIPanGestureRecognizer *)sender;
- (BOOL)oneFingerPan:(UIPanGestureRecognizer *)sender;
- (void)twoFingersTwoTaps:(UITapGestureRecognizer *)sender;
- (void)oneFingerTwoTaps:(UITapGestureRecognizer *)sender;
- (void)oneFingerOneTap:(UITapGestureRecognizer *)sender;

@end

#define MAXPATH 512
typedef struct {
    char filename[MAXPATH];
    char name[64];
    UIImage* image;
} ImageItem;
typedef std::list<ImageItem>::iterator ImageIt;

static std::list<CGPoint>   s_ptsPending;

@interface GiViewController(Image)
- (void)clearImages;
- (void)clearUnusedImages;
- (UIImage *)getImageWithID:(const char*)name;
- (std::list<ImageItem>*)images;
- (BOOL)addImageShape_:(UIImage*)image filename:(NSString*)filename name:(NSString*)name;
@end

void mgGetBoundingViewBox(Box2d& box, const MgMotion* sender);
static GiViewController* s_instance = nil;

@implementation GiViewController

@synthesize editDelegate;
@synthesize gestureRecognizerUsed = _gestureRecognizerUsed;
@synthesize magnifierView;
@synthesize activeView = _activeView;
@synthesize currentShapeFixedLength;
@synthesize lineWidth, strokeWidth, lineColor, fillColor;
@synthesize lineAlpha, fillAlpha, lineStyle;
@synthesize commandName;
@synthesize doc, currentShapes;

- (id)init
{
    self = [super init];
    if (self) {
        for (int iv = 0; iv < 3; iv++)                      // 末尾的nil用于结束占位
            _magViews[iv] = Nil;
        _cmdctl = [[GiCommandController alloc]initWithViews:_magViews];
        _docCreated = NULL;
        _images = new std::list<ImageItem>;
        
        for (int t = 0; t < 2; t++) {
            for (int i = 0; i < RECOGNIZER_COUNT; i++)
                _recognizers[t][i] = Nil;
        }
        _gestureRecognizerUsed = YES;
        
        BOOL iPad = (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad);
        GiCanvasIos::setScreenDpi(iPad ? 132 : 163, [UIScreen mainScreen].scale);
    }
    return self;
}

- (void)viewDidLoad
{
    if ([self gview]) {
        _activeView = self.view;
        
        if (!s_instance)
            s_instance = self;
        
        [[self gview] setDrawingDelegate:self];
        [self addGestureRecognizers:0 view:self.view];
        
        GiCommandController* cmd = (GiCommandController*)_cmdctl;
        [cmd touchesBegan:CGPointZero view:_activeView count:0];    // 传入View
        if (cmd.lineWidth == 0) {
            cmd.lineWidth = 20;                                 // 默认画笔0.2mm
            cmd.lineColor = GiColor(0, 0, 0, 128);              // 默认黑色画笔，50%透明
            cmd.autoFillColor = YES;
        }
    }
}

- (void)dealloc
{
    [_cmdctl release];
    _cmdctl = nil;
    
    if (_docCreated) {
        ((MgShapeDoc*)_docCreated)->release();
        _docCreated = NULL;
    }
    for (int t = 0; t < 2; t++) {
        for (int i = 0; i < RECOGNIZER_COUNT; i++) {
            [_recognizers[t][i] release];
            _recognizers[t][i] = Nil;
        }
    }
    if (s_instance == self) {
        s_instance = nil;
    }
    if (_images) {
        [self clearImages];
        std::list<ImageItem>* imgs = [self images];
        imgs->clear();
        delete imgs;
        _images = NULL;
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
    
    if (_docCreated) {
        aview.doc = (MgShapeDoc*)_docCreated;
    }
    else {
        aview.doc = MgShapeDoc::create();
        _docCreated = aview.doc;
    }
    
    [self viewDidLoad];
    
    [aview release];
    return aview;
}

- (UIView*)createSubGraphView:(UIView*)parentView frame:(CGRect)frame
                          doc:(void*)d backgroundColor:(UIColor*)bkColor
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
    
    aview.doc = (MgShapeDoc*)(d ? d : _docCreated);
    if (!aview.doc)
    {
        aview.doc = MgShapeDoc::create();
        _docCreated = aview.doc;
    }
    
    [self viewDidLoad];
    _recognizers[0][kTwoFingersTwoTaps].enabled = NO;
    
    [aview release];
    return aview;
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

- (void)zoomTo:(CGRect)modelRect
{
    GiGraphView* gview = (GiGraphView*)self.view;
    Box2d rect(modelRect.origin.x, modelRect.origin.y,
               modelRect.origin.x + modelRect.size.width,
               modelRect.origin.y + modelRect.size.height);
    
    [gview xform]->zoomTo(rect * [gview xform]->modelToWorld());
    if ([gview xform]->getViewScale() > 1.5f) {
        [gview graph]->setMaxPenWidth(10);
    }
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
    [self clearImages];
}

- (CGImageRef)cachedBitmap:(BOOL)invert
{
    GiGraphView *aview = (GiGraphView *)self.view;
    return [aview cachedBitmap:invert];
}

- (UIImage *)createThumbnail:(CGSize)size shapes:(void*)mgstorage invert:(BOOL)invert
{
    MgShapeDoc *sp = NULL;
    GiGraphIos graph;
    UIImage *image = nil;
    Box2d rectW;
    
    if (mgstorage) {
        sp = MgShapeDoc::create();
        if (!sp->load((MgStorage*)mgstorage)) {
            return image;
        }
        graph.xf.setModelTransform(sp->modelTransform());
        rectW = sp->getPageRectW();
        rectW = rectW.isEmpty() ? sp->getExtent() * sp->modelTransform() : rectW;
    }
    else {
        GiGraphView *aview = (GiGraphView *)self.view;
        sp = [aview doc];
        graph.xf.setModelTransform(sp->modelTransform());
        rectW = sp->getPageRectW();
        if (rectW.isEmpty()) {
            graph.xf.setModelTransform([aview xform]->modelToWorld());
            rectW = [aview xform]->getWndRectW();
        }
    }
    
    if (rectW.isEmpty()) {
        if (mgstorage && sp) {
            sp->release();
        }
        return image;
    }
    
    if (size.width * rectW.height() > size.height * rectW.width())
        size.width = rectW.width() / rectW.height() * size.height;
    else
        size.height = rectW.height() / rectW.width() * size.width;
    
    int w = mgRound(size.width);
    int h = mgRound(size.height);
    while (w > 1024 || h > 1024) {
        w = w * 4 / 5;
        h = h * 4 / 5;
    }
    graph.xf.setWndSize(w, h);
    graph.xf.zoomTo(rectW);
    
    if (graph.canvas->beginPaintBuffered(false, false)) {
        sp->draw(graph.gs);
        
        graph.canvas->saveCachedBitmap();
        graph.canvas->endPaint();
    }
    
    CGImageRef cgimage = graph.canvas->cachedBitmap(invert);
    image = [[UIImage alloc]initWithCGImage:cgimage
                                      scale:1 //[UIScreen mainScreen].scale
                                orientation:UIImageOrientationUp];
    if (invert) {
        CGImageRelease(cgimage);
    }
    
    if (mgstorage && sp) {
        sp->release();
    }
    
    return image;
}

- (UIView*)magnifierView {
    return _magViews[0];
}

- (NSUInteger)getShapeCount
{
    return [[self gview] doc]->getShapeCount();
}

- (void)removeShapes
{
    [self setCommandName:""];   // 取消当前命令
    
    MgShapesLock locker([[self gview] doc], MgShapesLock::Remove);
    [[self gview] doc]->clear();
    [self regen];
    
    [self clearImages];
}

- (BOOL)loadShapes:(void*)mgstorage
{
    const char* oldcmd = [self commandName];
    [self setCommandName:""];
    
    MgShapeDoc* sp = [[self gview] doc];
    MgShapesLock locker(sp, MgShapesLock::Load);
    BOOL ret = (locker.locked() && mgstorage
                && sp->load((MgStorage*)mgstorage));
    [self regen];
    [self setCommandName:oldcmd];
    
    return ret;
}

- (BOOL)saveShapes:(void*)mgstorage
{
    MgShapesLock locker([[self gview] doc], MgShapesLock::ReadOnly);
    bool ret = locker.locked() && mgstorage;
    
    if (ret) {
        ret = locker.doc->save((MgStorage*)mgstorage);
        locker.resetEditFlags();
        [self clearUnusedImages];
    }
    
    return ret;
}

- (void)afterShapeChanged
{
}

- (id)dynDraw:(id)sender
{
    GiGraphics* gs = NULL;
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    
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
        [cmd dynDraw: gs];
        
        if ((_activeView != self.view || !cmd.dragging)
            && sender == self.view && _magViews[0]
            && !_magViews[0].hidden && !_magViews[0].superview.hidden) {
            GiMagnifierView *aview = (GiMagnifierView *)_magViews[0];
            Box2d rect(aview.bounds.size.width, aview.bounds.size.height);
            bool fromMag = (_activeView == _magViews[0]);
            GiContext ctx(fromMag ? -2 : 0, 
                          GiColor(0, 200, 200, fromMag ? 160 : 64), kGiLineDot, 
                          GiColor(0, 200, 0, fromMag ? 32 : 16));
            gs->drawRect(&ctx, rect * [aview xform]->displayToModel());
        }
    }
    
    return sender;
}

- (void*)doc {
    return [[self gview] doc];
}

- (void*)currentShapes {
    return [[self gview] doc]->getCurrentShapes();
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

- (NSObject*)editDelegate
{
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    return cmd.editDelegate;
}

- (void)setEditDelegate:(NSObject*)d
{
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    cmd.editDelegate = d;
}

- (BOOL)currentShapeFixedLength
{
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    return cmd.currentShapeFixedLength;
}

- (void)setCurrentShapeFixedLength:(BOOL)fixed
{
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    [cmd setCurrentShapeFixedLength:fixed];
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
    GiGraphView *gview = (GiGraphView *)self.view;
    GiGraphics* gs = [gview graph];
    
    return gs->calcPenWidth(cmd.lineWidth, false) * gview.initialScale;
}

- (void)setStrokeWidth:(float)w {
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiGraphView *gview = (GiGraphView *)self.view;
    
    [cmd setLineWidth:-w / gview.initialScale];
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
    [cmd setAutoFillColor:c == nil];
    if (c) {
        GiColor color(giFromUIColor(c));
        if (color.a > 0.01f && cmd.fillColor.a > 0)     // 只要不是完全透明
            color.a = cmd.fillColor.a;                  // 就不改变透明度
        [cmd setFillColor:color];
    }
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

- (CGRect)getBoundingViewBox
{
    Box2d box;
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    mgGetBoundingViewBox(box, cmd.motion);
    return CGRectMake(box.xmin, box.ymin, box.width(), box.height());
}

- (BOOL)addImageShape:(UIImage*)image filename:(NSString*)filename name:(NSString*)name
{
    if (s_instance != self)
        s_instance = self;
    return [self addImageShape_:image filename:filename name:name];
}

- (NSString*)getImageShapePath:(NSString*)name
{
    NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, 
                                                          NSUserDomainMask, YES) objectAtIndex:0];
    NSString *filename = [NSString stringWithFormat:@"%@/%@", path, name];
    
    return filename;
}

- (void)imageShapeDeleted:(NSString*)name {}

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

//- (BOOL)canBecomeFirstResponder {
//    return YES;
//}

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

- (void)doContextAction:(int)action
{
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    [cmd doContextAction:action];
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

/*- (void)setGestureRecognizerEnabled:(BOOL)enabled
{
    for (int i = 0; i < RECOGNIZER_COUNT; i++) {
        if (_recognizers[0][i]) {
            _recognizers[0][i].enabled = enabled;
        }
        if (_recognizers[1][i]) {
            _recognizers[1][i].enabled = enabled;
        }
    }
}*/

- (UIGestureRecognizer*) getGestureRecognizer:(int)index
{
    return index >= 0 && index < RECOGNIZER_COUNT ? _recognizers[0][index] : NULL;
}

// 手势即将开始，在 touchesBegan 后发生，即将调用本类的相应手势响应函数
- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)gestureRecognizer
{
    BOOL allow = YES;
    
    if (gestureRecognizer == _recognizers[0][kPinchGesture]) {
        allow = [self twoFingersPinch:(UIPinchGestureRecognizer *)gestureRecognizer];
    }
    else if (gestureRecognizer == _recognizers[0][kRotateGesture]) {
        allow = [self twoFingersRotate:(UIRotationGestureRecognizer *)gestureRecognizer];
    }
    else if (gestureRecognizer == _recognizers[0][kTwoFingersPan]) {
        allow = [self twoFingersPan:(UIPanGestureRecognizer *)gestureRecognizer];
    }
    else if (gestureRecognizer == _recognizers[0][kPanGesture]) {
        allow = [self oneFingerPan:(UIPanGestureRecognizer *)gestureRecognizer];
    }
    
    if (!allow) {
        return allow;
    }
    
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
    if (_ignorepoint.x < 0 && _ignorepoint.y < 0 && point.x > 0) {
        _ignorepoint = [sender convertPoint:point toView:[sender window]];
    }
    [GiCommandController hideContextActions];
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
    
    // 双指旋转手势
    UIRotationGestureRecognizer *twoFingersRotate =
    [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersRotate:)];
    twoFingersRotate.delegate = self;                               // 用于检测长按
    _recognizers[t][n++] = twoFingersRotate;
    
    // 双指滑动手势
    UIPanGestureRecognizer *twoFingersPan =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(twoFingersPan:)];
    twoFingersPan.maximumNumberOfTouches = 2;
    twoFingersPan.minimumNumberOfTouches = 2;
    twoFingersPan.delegate = self;
    //[twoFingersPinch requireGestureRecognizerToFail:twoFingersPan]; // 捏合优先
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
    [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(longPressGesture:)];
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
    s_ptsPending.clear();
    
    if (touch.view == self.view) {
        [super touchesBegan:touches withEvent:event];
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (_gestureState != UIGestureRecognizerStateChanged && [touches count] == 1) {
        UITouch *touch = [touches anyObject];
        CGPoint point = [touch locationInView:touch.view];
        s_ptsPending.push_back(point);
    }
    else {
        s_ptsPending.clear();
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    
    _timeBegan = 0;
    
    if (!s_ptsPending.empty() && touch) {
        MgDynShapeLock locker;
        GiCommandController* cmd = (GiCommandController*)_cmdctl;
        std::list<CGPoint>::iterator it = s_ptsPending.begin();
        CGPoint pt;
        
        for (; it != s_ptsPending.end(); ++it) {
            pt = *it;
            [cmd touchesMoved:pt view:touch.view count:1];
        }
        [cmd touchesEnded:pt view:touch.view count:1];
    }
    s_ptsPending.clear();
    
    if (touch.view == self.view) {
        [super touchesEnded:touches withEvent:event];
    }
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    UITouch *touch = [touches anyObject];
    
    if (!s_ptsPending.empty() && touch
        && _gestureState > UIGestureRecognizerStateChanged) {
        MgDynShapeLock locker;
        std::list<CGPoint>::iterator it = s_ptsPending.begin();
        CGPoint pt;
        
        for (; it != s_ptsPending.end(); ++it) {
            pt = *it;
            [cmd touchesMoved:pt view:touch.view count:1];
        }
        [cmd touchesEnded:pt view:touch.view count:1];
        s_ptsPending.clear();
    }
    else if (touch) {           // oneFingerOneTap不直接处理是为了检测是点击还是短划动
        CGPoint point = [touch locationInView:touch.view];
        if ([cmd delayTap:point view:touch.view]) { // 看是否有点击待处理
            [self updateMagnifierCenter:nil];
        }
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
    _gestureState = sender.state;
    if (MgShapesLock::lockedForRead([[self gview] doc])      // 如果正在录屏复制
        || MgDynShapeLock::lockedForRead()) {
        if (sender.state == UIGestureRecognizerStateChanged)    // 触摸移动则忽略本次
            return NO;
        MgShapesLock([[self gview] doc], MgShapesLock::Unknown);     // 等待
        MgDynShapeLock();                                       // 等待录屏复制完成
    }
    if (sender.state == UIGestureRecognizerStateBegan
        || sender.state > UIGestureRecognizerStateChanged) {
        [self gestureStateChanged:sender];
    }
    
    return YES;
}

- (BOOL)twoFingersPinch:(UIPinchGestureRecognizer *)sender
{
    if (![self gestureCheck:sender])
        return NO;
    
    if (sender.view == _magViews[0]) {
        GiMagnifierView *zview = (GiMagnifierView *)_magViews[0];
        return [zview twoFingersPinch:sender];
    }
    
    BOOL ret = [[self getCommand:@selector(twoFingersPinch:)] twoFingersPinch:sender];
    if (!ret && sender.view == self.view) {
        ret = [[self motionView:@selector(twoFingersPinch:)] twoFingersPinch:sender];
    }
    [self updateMagnifierCenter:sender];
    
    return ret;
}

- (BOOL)twoFingersRotate:(UIRotationGestureRecognizer *)sender
{
    if (![self gestureCheck:sender])
        return NO;
    
    BOOL ret = [[self getCommand:@selector(twoFingersRotate:)] twoFingersRotate:sender];
    if (!ret && sender.view == self.view) {
        ret = [[self motionView:@selector(twoFingersRotate:)] twoFingersRotate:sender];
    }
    [self updateMagnifierCenter:sender];
    
    return ret;
}

- (BOOL)twoFingersPan:(UIPanGestureRecognizer *)sender
{
    if (![self gestureCheck:sender])
        return NO;
    
    BOOL ret = NO;
    if (sender.state <= UIGestureRecognizerStateBegan) {
        _touchCount = [sender numberOfTouches];
    }
    if (1 == _touchCount) {
        ret = YES;
        [self oneFingerPan:sender];
    }
    else if (sender.view == _magViews[0]) {
        GiMagnifierView *zview = (GiMagnifierView *)_magViews[0];
        ret = [zview twoFingersPan:sender];
    }
    else if (sender.view == _magViews[1]) {
        GiMagnifierView *zview = (GiMagnifierView *)_magViews[1];
        ret = [zview twoFingersPan:sender];
    }
    else if (_touchCount > 1) {
        ret = ([[self getCommand:@selector(twoFingersPan:)] twoFingersPan:sender]
               || [[self motionView:@selector(twoFingersPan:)] twoFingersPan:sender]);
        [self updateMagnifierCenter:sender];
    }
    
    return ret;
}

- (BOOL)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    if (![self gestureCheck:sender])
        return NO;
    
    if (!s_ptsPending.empty() && _gestureState == UIGestureRecognizerStateChanged) {
        MgDynShapeLock locker;
        GiCommandController* cmd = (GiCommandController*)_cmdctl;
        std::list<CGPoint>::iterator it = s_ptsPending.begin();
        
        for (; it != s_ptsPending.end(); ++it) {
            [cmd touchesMoved:*it view:sender.view count:1];
        }
        s_ptsPending.clear();
    }
    
    BOOL ret = [[self getCommand:@selector(oneFingerPan:)] oneFingerPan:sender];
    if (!ret && sender.view == self.view) {
        ret = [[self motionView:@selector(oneFingerPan:)] oneFingerPan:sender];
    }
    [self updateMagnifierCenter:sender];
    
    return ret;
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

- (void)longPressGesture:(UIGestureRecognizer *)sender
{
    [[self getCommand:@selector(longPressGesture:)] longPressGesture:sender];
}

- (void)updateMagnifierCenter:(UIGestureRecognizer *)sender
{
    UIView* senderView = sender ? sender.view : _activeView;
    if (!_magViews[0] || senderView != self.view
        || sender.state < UIGestureRecognizerStateBegan) {
        return;
    }
    
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    GiMagnifierView *zview = (GiMagnifierView *)_magViews[0];
    
    if (sender && [sender numberOfTouches] > 0) {
        [zview automoveSuperview:[sender locationInView:senderView] fromView:sender.view];
    }
    
    if (!sender) {
        [zview setPointWandRedraw:[cmd getPointW] : ![self isCommand:"select"]];
    }
    else if (sender.state == UIGestureRecognizerStateBegan
        || sender.state == UIGestureRecognizerStateEnded) {
        [zview setPointWandRedraw:[cmd getPointW] :NO];
    }
    else {
        [zview setPointW:[cmd getPointW]];
    }
}

@end

MgShape* mgAddImageShape(const MgMotion* sender, const char* name, float width, float height);

CGImageRef getImageInShape(const char* name)
{
    UIImage *image = [s_instance getImageWithID:name];
    return image ? [image CGImage] : NULL;
}

@implementation GiViewController(Image)

- (std::list<ImageItem>*)images
{
    return (std::list<ImageItem>*)_images;
}

- (void)clearImages
{
    std::list<ImageItem>* imgs = [self images];
    for (ImageIt it = imgs->begin(); it != imgs->end(); ++it) {
        if (it->image) {
            [it->image release];
            it->image = nil;
            if (_cmdctl) {
                [self imageShapeDeleted:[NSString stringWithUTF8String:it->name]];
            }
        }
    }
}

- (void)clearUnusedImages
{
    MgShapes* sp = [[self gview] doc]->getCurrentShapes();
    std::list<ImageItem>* imgs = [self images];
    
    for (ImageIt it = imgs->begin(); it != imgs->end(); ++it) {
        if (it->image && !MgImageShape::findShapeByImageID(sp, it->name)) {
            [it->image release];
            it->image = nil;
            [self imageShapeDeleted:[NSString stringWithUTF8String:it->name]];
        }
    }
}

- (UIImage *)getImageWithID:(const char*)name
{
    int len1 = strlen(name);
    UIImage *image = nil;
    
    for (ImageIt it = [self images]->begin(); it != [self images]->end(); ++it) {
        int len2 = strlen(it->filename);
        int len = std::min(len1, len2);
        
        if (strcmp(name, it->filename + len2 - len) == 0) {
            if (!it->image) {
                NSString* filename = nil;
                if (s_instance) {
                    filename = [NSString stringWithUTF8String:name];
                    filename = [s_instance getImageShapePath:filename];
                }
                if (filename && [filename length] > len1) {
                    len = std::min((int)[filename length], MAXPATH-1);
                    strncpy(it->filename, [filename UTF8String] + [filename length] - len, MAXPATH);
                    it->filename[MAXPATH - 1] = 0;
                }
                filename = [NSString stringWithUTF8String:it->filename];
                it->image = [[UIImage alloc]initWithContentsOfFile:filename];
            }
            image = it->image;
            break;
        }
    }
    if (!image && s_instance) {
        NSString* filename = [NSString stringWithUTF8String:name];
        filename = [s_instance getImageShapePath:filename];
        
        if (filename && [filename length] > len1) {
            ImageItem item;
            
            item.image = [[UIImage alloc]initWithContentsOfFile:filename];
            if (item.image == nil || item.image.size.width < 1) {
                [item.image release];
                return nil;
            }
            
            strncpy(item.name, name, sizeof(item.name));
            item.name[sizeof(item.name) - 1] = 0;
            
            strncpy(item.filename, [filename UTF8String], sizeof(item.filename));
            item.filename[sizeof(item.filename) - 1] = 0;
            
            [self images]->push_back(item);
            image = item.image;
        }
    }
    
    return image;
}

- (BOOL)addImageShape_:(UIImage*)image filename:(NSString*)filename name:(NSString*)name
{
    if (!filename)
        filename = [NSString stringWithFormat:@"%ld",[self images]->size()];
    if (!name)
        name = filename;
    
    const char* sname = strrchr([name UTF8String], '/');
    sname = sname ? sname + 1 : [name UTF8String];
    
    ImageItem item;
    UIImage* imageOld = [self getImageWithID:sname];
    GiCommandController* cmd = (GiCommandController*)_cmdctl;
    
    if (image || !imageOld) {
        if (image) {
            item.image = image;
            [image retain];
        }
        else {
            item.image = [[UIImage alloc]initWithContentsOfFile:filename];
            if (item.image && item.image.size.width < 1) {
                [item.image release];
                return nil;
            }
            image = item.image;
        }
        
        strncpy(item.name, sname, sizeof(item.name));
        item.name[sizeof(item.name) - 1] = 0;
        
        strncpy(item.filename, [filename UTF8String], sizeof(item.filename));
        item.filename[sizeof(item.filename) - 1] = 0;
        
        [self images]->push_back(item);
    }
    
    image = image ? image : imageOld;
    return image && mgAddImageShape(cmd.motion, sname, image.size.width, image.size.height);
}

@end
