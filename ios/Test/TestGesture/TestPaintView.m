// TestPaintView.m
// Created by Zhang Yungui on 2012-8-27.
//

#import "TestPaintView.h"

@implementation TestPaintView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        self.contentMode = UIViewContentModeRedraw;
    }
    return self;
}

- (void)drawRect:(CGRect)rect
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    if (CGPointEqualToPoint(_points[0], _points[1])) {
        CGContextStrokeEllipseInRect(context, CGRectMake(_points[0].x - 5, _points[0].y - 5, 5, 5));
    }
    else {
        CGContextStrokeLineSegments(context, _points, 2);
    }
}

- (id)oneFingerPan:(UIPanGestureRecognizer *)gesture
{
    _points[1] = [gesture locationInView:self];
    
    if (gesture.state == UIGestureRecognizerStateBegan) {
        _points[0] = _points[1];
    }
    [self setNeedsDisplay];
    
    return self;
}

- (id)oneFingerOneTap:(UIGestureRecognizer *)gesture
{
    _points[0] = [gesture locationInView:self];
    _points[1] = _points[0];
    [self setNeedsDisplay];
    
    return self;
}

@end
