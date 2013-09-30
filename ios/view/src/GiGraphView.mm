//! \file GiGraphView.mm
//! \brief 实现iOS绘图视图类 GiGraphView
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GiGraphViewImpl.h"

@implementation IosTempView

- (id)initView:(CGRect)frame :(GiViewAdapter *)adapter {
    self = [super initWithFrame:frame];
    if (self) {
        _adapter = adapter;
        self.opaque = NO;                           // 透明背景
        self.userInteractionEnabled = NO;           // 禁止交互，避免影响主视图显示
    }
    return self;
}

- (void)drawRect:(CGRect)rect {
    GiCanvasAdapter canvas;
    
    if (canvas.beginPaint(UIGraphicsGetCurrentContext())) {
        _adapter->coreView()->dynDraw(_adapter, &canvas);
        canvas.endPaint();
    }
}

@end

static GiGraphView* _activeGraphView = nil;
GiColor CGColorToGiColor(CGColorRef color);

@implementation GiGraphView

@synthesize panRecognizer = _panRecognizer;
@synthesize tapRecognizer = _tapRecognizer;
@synthesize twoTapsRecognizer = _twoTapsRecognizer;
@synthesize pressRecognizer = _pressRecognizer;
@synthesize pinchRecognizer = _pinchRecognizer;
@synthesize rotationRecognizer = _rotationRecognizer;
@synthesize gestureEnabled;

- (void)dealloc {
    if (_activeGraphView == self)
        _activeGraphView = nil;
    delete _adapter;
    [super dealloc];
}

- (void)initView:(GiView*)mainView :(GiCoreView*)coreView {
    self.opaque = NO;                               // 透明背景
    self.multipleTouchEnabled = YES;                // 检测多个触点
    
    GiCoreView::setScreenDpi(GiCanvasAdapter::getScreenDpi());
    [self setupGestureRecognizers];
    
    if (mainView && coreView) {
        _adapter = new GiViewAdapter(self, coreView);
        coreView->createMagnifierView(_adapter, mainView);
    }
    else {
        _adapter = new GiViewAdapter(self, NULL);
        _adapter->coreView()->createView(_adapter);
    }
}

- (id)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.autoresizingMask = 0xFF;               // 自动适应大小
        _activeGraphView = self;                    // 设置为当前绘图视图
        [self initView:NULL :NULL];
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame :(GiGraphView *)refView {
    self = [super initWithFrame:frame];
    if (self) {
        [self initView:[refView viewAdapter] :[refView coreView]];
    }
    return self;
}

+ (GiGraphView *)createGraphView:(CGRect)frame :(UIView *)parentView {
    GiGraphView *v = [[GiGraphView alloc]initWithFrame:frame];
    [parentView addSubview:v];
    [v release];
    return v;
}

+ (GiGraphView *)createMagnifierView:(CGRect)frame
                              refView:(GiGraphView *)refView
                           parentView:(UIView *)parentView
{
    refView = refView ? refView : [GiGraphView activeView];
    if (!refView)
        return nil;
    
    GiGraphView *v = [[GiGraphView alloc]initWithFrame:frame :refView];
    [parentView addSubview:v];
    [v release];
    
    return v;
}

+ (GiGraphView *)activeView {
    return _activeGraphView;
}

- (GiView *)viewAdapter {
    return _adapter;
}

- (GiCoreView *)coreView {
    return _adapter->coreView();
}

- (int)coreViewHandle {
    return _adapter->coreView()->viewAdapter();
}

- (UIImage *)snapshot {
    return _adapter->snapshot(true);
}

- (BOOL)savePng:(NSString *)filename {
    BOOL ret = NO;
    NSData* imageData = UIImagePNGRepresentation([self snapshot]);
    
    if (imageData) {
        ret = [imageData writeToFile:filename atomically:NO];
    }
    
    return ret;
}

- (void)setBackgroundColor:(UIColor *)color {
    [super setBackgroundColor:color];
    
    if (!color && self.superview) {
        color = self.superview.backgroundColor;
        if (!color && self.superview.superview) {
            color = self.superview.superview.backgroundColor;
        }
    }
    if (color) {
        [self coreView]->setBkColor(_adapter, CGColorToGiColor(color.CGColor).getARGB());
    }
}

- (void)drawRect:(CGRect)rect {
    GiCoreView *coreView = _adapter->coreView();
    GiCanvasAdapter canvas;
    
    coreView->onSize(_adapter, self.bounds.size.width, self.bounds.size.height);
    
    if (canvas.beginPaint(UIGraphicsGetCurrentContext())) {
        if (!_adapter->drawAppend(&canvas)) {
            coreView->drawAll(_adapter, &canvas);
        }
        canvas.endPaint();
    }
}

- (void)redraw {
    _adapter->redraw();
}

- (void)clearCachedData {
    _adapter->clearCachedData();
}

- (BOOL)gestureEnabled {
    return self.userInteractionEnabled;
}

- (void)setGestureEnabled:(BOOL)enabled {
    UIGestureRecognizer *recognizers[] = {
        _pinchRecognizer, _rotationRecognizer, _panRecognizer, 
        _tapRecognizer, _twoTapsRecognizer, _pressRecognizer, nil
    };
    for (int i = 0; recognizers[i]; i++) {
        recognizers[i].enabled = enabled;
    }
    self.userInteractionEnabled = enabled;
}

- (void)activiteView {
    if (_activeGraphView != self) {
        _activeGraphView = self;
    }
}

- (void)addDelegate:(id<GiGraphViewDelegate>)d {
    if (d) {
        [self removeDelegate:d];
        _adapter->delegates.push_back(d);
        _adapter->respondsTo.didCommandChanged |= [d respondsToSelector:@selector(onCommandChanged:)];
        _adapter->respondsTo.didSelectionChanged |= [d respondsToSelector:@selector(onSelectionChanged:)];
        _adapter->respondsTo.didContentChanged |= [d respondsToSelector:@selector(onContentChanged:)];
    }
}

- (void)removeDelegate:(id<GiGraphViewDelegate>)d {
    for (size_t i = 0; i < _adapter->delegates.size(); i++) {
        if (_adapter->delegates[i] == d) {
            _adapter->delegates.erase(_adapter->delegates.begin() + i);
            break;
        }
    }
}

- (void)hideContextActions {
    _adapter->hideContextActions();
}

- (IBAction)onContextAction:(id)sender
{
    UIView *btn = (UIView *)sender;
    int action = btn ? btn.tag : 0;
    
    [NSObject cancelPreviousPerformRequestsWithTarget:self
                                             selector:@selector(hideContextActions) object:nil];
    _adapter->hideContextActions();
    [self coreView]->doContextAction(action);
}

@end
