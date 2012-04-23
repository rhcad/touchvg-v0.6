// GraphViewController.mm
// Created by Zhang Yungui on 2012-3-2.

#import "GraphViewController.h"

#ifdef TEST_SIMPLE_VIEW
#import "TestGraphView.h"

@implementation GraphViewController

- (void)loadView
{
    CGRect rect = [[UIScreen mainScreen] applicationFrame];
    rect.origin.y = 0;
    UIView *view = [[TestGraphView alloc]initWithFrame:rect];
    self.view = view;
    [view release];
}

@end

#else

#import "SCCalloutView.h"
#import <GiGraphView.h>

static const NSUInteger kRedTag         = 0;
static const NSUInteger kBlueTag        = 1;
static const NSUInteger kYellowTag      = 2;
static const NSUInteger kLineTag        = 3;
static const NSUInteger kDashLineTag    = 4;

@implementation GraphViewController

- (void)dealloc
{
    [_graphc release];
    [super dealloc];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

- (void)clearCachedData
{
    [_graphc clearCachedData];
}

#define MAG_AT_BOTTOM

- (void)loadView
{
    CGRect rect = [[UIScreen mainScreen] applicationFrame];
    CGFloat BAR_HEIGHT = rect.size.height > 1000 ? 50 : 40;
    CGFloat BTN_XDIFF  = rect.size.height > 1000 ? 10 : 0;
    
    UIView *mainview = [[UIView alloc]initWithFrame:rect];
    self.view = mainview;
    self.view.backgroundColor = [UIColor clearColor];
    [mainview release];
    rect.origin.y = 0;
    
    CGRect viewFrame = rect;
    CGRect magFrame = CGRectMake(10, 10, 200, 200);
    CGRect barFrame = rect;
    
    viewFrame.size.height -= BAR_HEIGHT;
#ifdef MAG_AT_BOTTOM
    viewFrame.size.height -= magFrame.size.height;
    barFrame.origin.y += magFrame.size.height;
    magFrame = CGRectMake(0, viewFrame.size.height, rect.size.width, magFrame.size.height);
#endif
    
    _graphc = [[GiViewController alloc]init];
    [_graphc createGraphView:viewFrame backgroundColor:[UIColor grayColor] shapes:NULL];
    [self.view addSubview:_graphc.view];
    
    UIView *magnifierView = [[UIView alloc]initWithFrame:magFrame];
    magnifierView.backgroundColor = [UIColor colorWithRed:0.6f green:0.7f blue:0.6f alpha:0.9f];
    [self.view addSubview:magnifierView];
    
    CGFloat magbtnw = 32;
    CGRect magbarRect = CGRectMake(0, 0, magnifierView.bounds.size.width, magbtnw);
    UIButton *magbarView = [[UIButton alloc]initWithFrame:magbarRect];
    [magbarView setImage:[UIImage imageNamed:@"downview.png"] forState: UIControlStateNormal];
	magbarView.alpha = 0.6;
    magbarView.autoresizingMask = UIViewAutoresizingFlexibleWidth;
	[magnifierView addSubview:magbarView];
    [magbarView release];
    
    CGRect maggraphRect = magnifierView.bounds;
    maggraphRect.origin.y = magbtnw;
    maggraphRect.size.height -= magbtnw;
    
    CGRect mag1rect = maggraphRect;
    mag1rect.size.width = mag1rect.size.height < mag1rect.size.width / 2 ? mag1rect.size.height : mag1rect.size.width / 2;
    CGRect mag2rect = maggraphRect;
    mag2rect.origin.x = mag1rect.size.width;
    mag2rect.size.width -= mag2rect.origin.x;
    
    UIView *magView = [_graphc createMagnifierView:magnifierView frame:mag2rect scale:4];
    magView.autoresizingMask = (UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight
                                | UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleBottomMargin);
    
    magView = [_graphc createMagnifierView:magnifierView frame:mag1rect scale:0.1];
    magView.autoresizingMask = (UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleRightMargin
                                | UIViewAutoresizingFlexibleBottomMargin);
    magView.backgroundColor = [UIColor colorWithRed:1 green:1 blue:1 alpha:0.1f];
    
    CGFloat magbtnx = 4;
    UIButton *magbtn = [self addButton:Nil action:@selector(lockMagnifier:)
                                   bar:magbarView x:&magbtnx size:magbtnw diffx:4];
    [magbtn setTitle:@"L" forState: UIControlStateNormal];
    [magbtn setTitle:@"l" forState: UIControlStateHighlighted];
    
#ifndef MAG_AT_BOTTOM
    magbtn = [self addButton:Nil action:@selector(resizeMagnifier:)
                         bar:magbarView x:&magbtnx size:magbtnw diffx:4];
    [magbtn setTitle:@"S" forState: UIControlStateNormal];
    [magbtn setTitle:@"s" forState: UIControlStateHighlighted];
#endif
    magbtn = [self addButton:Nil action:@selector(fireUndo:)
                         bar:magbarView x:&magbtnx size:magbtnw diffx:4];
    [magbtn setTitle:@"U" forState: UIControlStateNormal];
    [magbtn setTitle:@"u" forState: UIControlStateHighlighted];
    
    magbtn = [self addButton:Nil action:@selector(hideMagnifier:)
                         bar:magbarView x:&magbtnx size:magbtnw diffx:4];
    [magbtn setTitle:@"H" forState: UIControlStateNormal];
    [magbtn setTitle:@"h" forState: UIControlStateHighlighted];
    
    barFrame.size.height = BAR_HEIGHT;
    barFrame.origin.y += viewFrame.size.height;
    
    _downview = [[UIButton alloc]initWithFrame:barFrame];
    [_downview setImage:[UIImage imageNamed:@"downview.png"] forState: UIControlStateNormal];
	_downview.alpha = 0.6;
	[self.view addSubview:_downview];
    [_downview release];
    _downview.autoresizingMask = (UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleTopMargin);
    
    CGFloat btnx = barFrame.size.width / 2 - (BTN_XDIFF + BAR_HEIGHT) * 4;
    redBtn = [self addButton:@"redbrush.png" action:@selector(colorBtnPress:)
                         bar:_downview x:&btnx size:BAR_HEIGHT diffx:BTN_XDIFF];
    redBtn.tag = kRedTag;
    
    blueBtn = [self addButton:@"bluebrush.png" action:@selector(colorBtnPress:)
                          bar:_downview x:&btnx size:BAR_HEIGHT diffx:BTN_XDIFF];
    blueBtn.tag = kBlueTag;
    
    yellowbtn = [self addButton:@"yellowbrush.png" action:@selector(colorBtnPress:)
                            bar:_downview x:&btnx size:BAR_HEIGHT diffx:BTN_XDIFF];
    yellowbtn.tag = kYellowTag;
    
    colorbtn = [self addButton:@"colormix.png" action:@selector(showPaletee:)
                           bar:_downview x:&btnx size:BAR_HEIGHT diffx:BTN_XDIFF];
    
    brushbtn = [self addButton:@"brush.png" action:@selector(showPenView:)
                           bar:_downview x:&btnx size:BAR_HEIGHT diffx:BTN_XDIFF];	
    
    erasebtn = [self addButton:@"erase.png" action:@selector(eraseColor:)
                           bar:_downview x:&btnx size:BAR_HEIGHT diffx:BTN_XDIFF];
    
    clearbtn = [self addButton:@"clearview.png" action:@selector(clearView:)
                           bar:_downview x:&btnx size:BAR_HEIGHT diffx:BTN_XDIFF];
    
    backbtn  = [self addButton:@"back.png" action:@selector(backToView:)
                           bar:_downview x:&btnx size:BAR_HEIGHT diffx:BTN_XDIFF];
}

- (UIButton *)addButton:(NSString *)imgname action:(SEL)action bar:(UIView*)bar
                      x:(CGFloat*)x size:(CGFloat)size diffx:(CGFloat)diffx
{
    UIButton *btn = [[UIButton alloc]initWithFrame:CGRectMake(*x, 0, size, size)];
    if (imgname) {
        [btn setImage:[UIImage imageNamed:imgname] forState: UIControlStateNormal];
    }
    [btn addTarget:self action:action forControlEvents:UIControlEventTouchUpInside];
    [bar addSubview:btn];
    [btn release];
    *x += size + diffx;
    return btn;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    [self colorBtnPress:yellowbtn];
}

- (IBAction)lockMagnifier:(id)sender
{
    GiMagnifierView *magnifierView = (GiMagnifierView*)_graphc.magnifierView;
    magnifierView.lockRedraw = !magnifierView.lockRedraw;
}

- (IBAction)resizeMagnifier:(id)sender
{
    UIView *mview = _graphc.magnifierView.superview;
    CGSize totalsize = _graphc.view.bounds.size;
    CGSize size = mview.frame.size;
    
    if (size.width > totalsize.width - 20 || size.height > totalsize.height - 20) {
        mview.frame = CGRectMake(10, 10, 100, 130);
    }
    else if (size.width > totalsize.width * 0.75f || size.height > totalsize.height * 0.75f) {
        mview.frame = _graphc.view.bounds;
    }
    else {
        size = CGSizeMake(size.width * 2, size.height * 2);
        if (size.width > totalsize.width - 20 || size.height > totalsize.height - 20)
            mview.frame = _graphc.view.bounds;
        else
            mview.frame = CGRectMake(10, 10, size.width, size.height);
    }
}

- (IBAction)fireUndo:(id)sender
{
    [_graphc undoMotion];
}

- (IBAction)showPenView:(id)sender
{
	UIButton* btnsrc = (UIButton*)sender;
    
	[self showUnlightButtons];
	[brushbtn setImage:[UIImage imageNamed:@"brush1.png"] forState:UIControlStateNormal]; // brush 图标加亮
    
    CGRect btnrect = [btnsrc convertRect:btnsrc.bounds toView:self.view];
    CGRect viewrect = CGRectMake(0, 0, 239, 177);
    viewrect.origin.x = btnrect.origin.x + btnrect.size.width / 2 - viewrect.size.width / 2;
    viewrect.origin.y = btnrect.origin.y - viewrect.size.height;
    
	SCCalloutView *calloutView = [[SCCalloutView alloc]initWithFrame:viewrect];
    [calloutView setImage:[UIImage imageNamed:@"brushoption.png"] forState: UIControlStateNormal];
    [self.view addSubview:calloutView];
    [calloutView release];
    
    UISlider *sliderWidth = [[UISlider alloc] initWithFrame:CGRectMake(14,20,211, 40)];
	[sliderWidth addTarget:self action:@selector(lineWidthChange:) forControlEvents:UIControlEventValueChanged];
    sliderWidth.value = _graphc.lineWidth / 500.0f;    
	[calloutView addSubview:sliderWidth];
	[sliderWidth release];
	
	UISlider *sliderAlpha = [[UISlider alloc]initWithFrame:CGRectMake(14, 60, 211, 40)];
	[sliderAlpha addTarget:self action:@selector(alphaChange:) forControlEvents:UIControlEventValueChanged];
    sliderAlpha.value = _graphc.lineAlpha;
	[calloutView addSubview:sliderAlpha];
	[sliderAlpha release];
	
	UIButton *linebtn = [[UIButton alloc]initWithFrame:CGRectMake(14, 105, 63, 31)];
	[linebtn setImage:[UIImage imageNamed:@"line.png"] forState:UIControlStateNormal];
	[linebtn addTarget:self action:@selector(colorBtnPress:) forControlEvents:UIControlEventTouchUpInside];
	linebtn.tag = kLineTag;
	[calloutView addSubview:linebtn];
	[linebtn release];
	
	UIButton *dashlinebtn = [[UIButton alloc]initWithFrame:CGRectMake(84, 105, 63, 31)];
	[dashlinebtn setImage:[UIImage imageNamed:@"dashline.png"] forState:UIControlStateNormal];
	[dashlinebtn addTarget:self action:@selector(colorBtnPress:) forControlEvents:UIControlEventTouchUpInside];
	dashlinebtn.tag = kDashLineTag;
	[calloutView addSubview:dashlinebtn];
	[dashlinebtn release];
	
	UIButton *freebrush = [[UIButton alloc]initWithFrame:CGRectMake(154, 92, 63, 44)];
	[freebrush setImage:[UIImage imageNamed:@"freedraw.png"] forState:UIControlStateNormal];
	[freebrush addTarget:self action:@selector(hideMagnifier:) forControlEvents:UIControlEventTouchUpInside];
	[calloutView addSubview:freebrush];
	[freebrush release];
}

- (IBAction)colorBtnPress:(id)sender
{
	UIButton* btn = (UIButton*)sender;
    
	switch (btn.tag)
	{
		case kRedTag:
			[self showUnlightButtons];
            [redBtn setImage:[UIImage imageNamed:@"redbrush1.png"] forState: UIControlStateNormal]; // 切换至红色画笔
            _graphc.lineColor = [UIColor redColor];
            _graphc.commandName = "splines";
			break;
		case kBlueTag:
		    [self showUnlightButtons];
            [blueBtn setImage:[UIImage imageNamed:@"bluebrush1.png"] forState: UIControlStateNormal]; // 切换至蓝色画笔
            _graphc.lineColor = [UIColor blueColor];
            _graphc.commandName = "splines";
			break;
        case kYellowTag:
		    [self showUnlightButtons];
            [yellowbtn setImage:[UIImage imageNamed:@"yellowbrush1.png"] forState: UIControlStateNormal]; // 切换至黄色画笔
            _graphc.lineColor = [UIColor yellowColor];
            _graphc.commandName = "splines";
			break;
		case kLineTag:              // 画直线
            _graphc.lineStyle = 0;
			break;
		case kDashLineTag:          // 画虚线
            _graphc.lineStyle = 1;
			break;
		default:
			break;
	}
}

- (IBAction)lineWidthChange:(id)sender // 线条宽度调整
{
    UISlider *slider = (UISlider *)sender;
    _graphc.lineWidth = 500.0f * slider.value;
}

- (IBAction)alphaChange:(id)sender  // 透明度调整
{
    UISlider *slider = (UISlider *)sender;
    _graphc.lineAlpha = slider.value;
}

- (IBAction)showPaletee:(id)sender  // 显示调色板
{
    [self showUnlightButtons];
	[colorbtn setImage:[UIImage imageNamed:@"colormix1"] forState:UIControlStateNormal]; // 图标加亮
    
    CGRect viewrect = CGRectMake(0, 0, 298+40, 257+60);
    viewrect.origin.x = (self.view.bounds.size.width - viewrect.size.width) / 2;
    viewrect.origin.y = self.view.bounds.size.height - viewrect.size.height - _downview.frame.size.height - 20;
    
	SCCalloutGraphView *calloutView = [[SCCalloutGraphView alloc]initWithFrame:viewrect];
    [self.view addSubview:calloutView];
    [calloutView release];
    
    UIButton *wrapview = [[UIButton alloc]initWithFrame:calloutView.bounds];
    wrapview.backgroundColor = [UIColor darkGrayColor];
    [wrapview addTarget:self action:@selector(colorMapPress:) forControlEvents:UIControlEventTouchDragInside];
    [calloutView addSubview:wrapview];
    [wrapview release];
    
    UIButton *mapbtn = [[UIButton alloc]initWithFrame:CGRectMake(20, 20, 298, 257)];
	[mapbtn setImage:[UIImage imageNamed:@"colormap.png"] forState:UIControlStateNormal];
	[wrapview addSubview:mapbtn];
	[mapbtn release];
    
    [calloutView.graphc createSubGraphView:mapbtn frame:mapbtn.bounds shapes:_graphc.shapes];
    calloutView.graphc.commandName = "splines";
}

- (IBAction)colorMapPress:(id)sender
{
    //UIControl* btn = (UIControl*)sender;
}

- (IBAction)hideMagnifier:(id)sender   // 切换放大镜视图的可见性
{
    _graphc.magnifierView.superview.hidden = !_graphc.magnifierView.superview.hidden;
    
#ifdef MAG_AT_BOTTOM
    CGRect rect = _graphc.view.frame;
    if (_graphc.magnifierView.superview.hidden) {
        rect.size.height += _graphc.magnifierView.superview.frame.size.height;
        _graphc.view.frame = rect;
    }
    else {
        rect.size.height -= _graphc.magnifierView.superview.frame.size.height;
        _graphc.view.frame = rect;
        rect = CGRectMake(0, rect.size.height, rect.size.width, 
                          _graphc.magnifierView.superview.frame.size.height);
        _graphc.magnifierView.superview.frame = rect;
    }
#endif
}

- (IBAction)eraseColor:(id)sender   // 切换至橡皮擦
{
	[self showUnlightButtons];
	[erasebtn setImage:[UIImage imageNamed:@"erase1.png"] forState:UIControlStateNormal];
    
    _graphc.commandName = "erase";
}

- (IBAction)clearView:(id)sender    // 清屏
{
    _graphc.commandName = "lines";
}

- (IBAction)backToView:(id)sender   // 退出自由绘图
{
    [self showUnlightButtons];
    [backbtn setImage:[UIImage imageNamed:@"back1.png"] forState:UIControlStateNormal];
    _graphc.commandName = "select";
}

- (void)showUnlightButtons          // 显示全部非高亮钮
{
	[redBtn    setImage:[UIImage imageNamed:@"redbrush.png"] forState: UIControlStateNormal];
	[blueBtn   setImage:[UIImage imageNamed:@"bluebrush.png"] forState: UIControlStateNormal];
	[yellowbtn setImage:[UIImage imageNamed:@"yellowbrush.png"] forState: UIControlStateNormal];
	[colorbtn  setImage:[UIImage imageNamed:@"colormix.png"] forState:UIControlStateNormal];
	[brushbtn  setImage:[UIImage imageNamed:@"brush.png"] forState:UIControlStateNormal];
	[erasebtn  setImage:[UIImage imageNamed:@"erase.png"] forState:UIControlStateNormal];
    [backbtn   setImage:[UIImage imageNamed:@"back.png"] forState:UIControlStateNormal];
}

@end

#endif // TEST_SIMPLE_VIEW
