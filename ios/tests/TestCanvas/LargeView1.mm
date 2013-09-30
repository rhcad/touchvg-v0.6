// LargeView1.mm
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "LargeView1.h"
#import "GraphView1.h"

@implementation LargeView1

- (id)initWithFrame:(CGRect)frame withFlags:(int)t
{
    self = [super initWithFrame:frame];
    if (self) {
        float w = (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) ? 1024 : 2048;
        if (t & 0x400) {    // testDynCurves
            _subview = [[GraphView2 alloc]initWithFrame:CGRectMake(0, 0, w, w) withFlags:t];
        }
        else {
            _subview = [[GraphView1 alloc]initWithFrame:CGRectMake(0, 0, w, w) withFlags:t];
        }
        [self addSubview:_subview];
        [_subview release];
        
        self.delegate = self;
        self.contentSize = _subview.frame.size;
        self.minimumZoomScale = 0.25f;
        self.maximumZoomScale = 5.f;
    }
    
    return self;
}

/*
- (void)didMoveToSuperview
{
    [super didMoveToSuperview];
    self.backgroundColor = [UIColor scrollViewTexturedBackgroundColor];
    if (!_subview.backgroundColor) {
        _subview.backgroundColor = self.superview.backgroundColor;
    }
}*/

- (UIView *)viewForZoomingInScrollView:(UIScrollView *)scrollView
{
    return _subview;
}

- (void)save
{
    [_subview save];
}

- (void)edit
{
    [_subview edit];
}

@end
