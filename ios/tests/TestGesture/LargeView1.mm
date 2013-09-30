// LargeView1.mm
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "LargeView1.h"
#import "GraphView1.h"

@implementation LargeView1

- (id)initWithFrame:(CGRect)frame withFlags:(int)t
{
    self = [super initWithFrame:frame];
    if (self) {
        _subview = [[GraphView1 alloc]initWithFrame:CGRectMake(0, 0, 2048, 2048)
                                          withFlags:t];
        [self addSubview:_subview];
        [_subview release];
        
        self.delegate = self;
        self.contentSize = _subview.frame.size;
        self.minimumZoomScale = 0.25f;
        self.maximumZoomScale = 5.f;
        
#ifdef __IPHONE_5_0
        if (_subview.panGestureRecognizer) {
            [self.panGestureRecognizer requireGestureRecognizerToFail:_subview.panGestureRecognizer];
        }
#endif
    }
    
    return self;
}

- (void)didMoveToSuperview
{
    [super didMoveToSuperview];
    self.backgroundColor = [UIColor scrollViewTexturedBackgroundColor];
    if (!_subview.backgroundColor) {
        _subview.backgroundColor = self.superview.backgroundColor;
    }
}

- (UIView *)viewForZoomingInScrollView:(UIScrollView *)scrollView
{
    return _subview;
}

@end
