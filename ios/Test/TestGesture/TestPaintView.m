// TestPaintView.m
// Created by Zhang Yungui on 2012-8-27.
//

#import "TestPaintView.h"

@implementation TestPaintView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        _count = 0;
        self.contentMode = UIViewContentModeRedraw;
    }
    return self;
}

- (void)drawRect:(CGRect)rect
{
    CGContextRef context = UIGraphicsGetCurrentContext();
        
    if (_count < 2) {
        CGRect rect = CGRectMake(_points[0].x - 5, _points[0].y - 5, 5, 5);
        CGContextStrokeEllipseInRect(context, rect);
    }
    else {
        for (int i = 0; i < _count; i++) {
            while (i < _count && _points[i].x <= -999) i++;
            CGContextBeginPath(context);
            CGContextMoveToPoint(context, _points[i].x, _points[i].y);
            int j = i++;
            for (; i < _count && _points[i].x > -999; i++) {
                CGContextAddLineToPoint(context, _points[i].x, _points[i].y);
            }
            if (i - j > 1) {
                CGContextStrokePath(context);
            }
            else {
                CGRect rect = CGRectMake(_points[j].x - 5, _points[j].y - 5, 5, 5);
                CGContextStrokeEllipseInRect(context, rect);
            }
        }
    }
}

- (id)oneFingerPan:(UIPanGestureRecognizer *)gesture
{
    if (_count >= 198) {
        for (int i = 20; i < _count; i++)
            _points[i - 20] = _points[i];
        _count -= 20;
    }
    if (gesture.state == UIGestureRecognizerStateBegan) {
        _points[_count++] = CGPointMake(-999, -999);
    }
    _points[_count++] = [gesture locationInView:self];
    [self setNeedsDisplay];
    
    return self;
}

- (id)oneFingerOneTap:(UIGestureRecognizer *)gesture
{
    if (_count >= 198) {
        for (int i = 20; i < _count; i++)
            _points[i - 20] = _points[i];
        _count -= 20;
    }
    _points[_count++] = CGPointMake(-999, -999);
    _points[_count++] = [gesture locationInView:self];
    [self setNeedsDisplay];
    
    return self;
}

- (id)twoFingersTwoTaps:(UIGestureRecognizer *)gesture
{
    _count = 0;
    [self setNeedsDisplay];
    
    return self;
}

@end
