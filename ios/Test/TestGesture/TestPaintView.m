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

- (BOOL)gesturePan:(UIPanGestureRecognizer *)gesture
{
    if (gesture.state != UIGestureRecognizerStatePossible) {
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
    }
    
    return YES;
}

- (BOOL)gestureTap:(UIGestureRecognizer *)gesture
{
    if (gesture.state != UIGestureRecognizerStatePossible) {
        if (_count >= 198) {
            for (int i = 20; i < _count; i++)
                _points[i - 20] = _points[i];
            _count -= 20;
        }
        _points[_count++] = CGPointMake(-999, -999);
        _points[_count++] = [gesture locationInView:self];
        [self setNeedsDisplay];
    }
    
    return YES;
}

- (BOOL)gestureTwoDblTaps:(UIGestureRecognizer *)gesture
{
    _count = 0;
    [self setNeedsDisplay];
    
    return YES;
}

@end

@implementation TestDragView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        _count = 4;
        _index[0] = -1;
        _index[1] = -1;
        for (int i = 0; i < _count; i++) {
            _points[i] = CGPointMake(30 + i * 40, 50 + i * 10);
        }
    }
    return self;
}

- (void)drawRect:(CGRect)rect
{
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    CGContextSetFillColorWithColor(context, [UIColor redColor].CGColor);
    for (int i = 0; i < _count; i++) {
        float r = (i == _index[0] || i == _index[1]) ? 30 : 20;
        CGRect rect = CGRectMake(_points[i].x - r, _points[i].y - r, 2 * r, 2 * r);
        CGContextFillEllipseInRect(context, rect);
    }
}

- (BOOL)gestureFreeDrag:(UIGestureRecognizer *)gesture
{
    if (gesture.state == UIGestureRecognizerStatePossible) {
        _index[0] = -1;
        _index[1] = -1;
        _touchCount = [gesture numberOfTouches];
        
        if (_touchCount > 0) {
            CGPoint pt = [gesture locationOfTouch:0 inView:self];
            for (int i = 0; i < _count; i++) {
                if (hypotf(pt.x - _points[i].x, pt.y - _points[i].y) < 30) {
                    _index[0] = i;
                    break;
                }
            }
        }
        if (_touchCount > 1) {
            CGPoint pt = [gesture locationOfTouch:1 inView:self];
            for (int i = 0; i < _count; i++) {
                if (hypotf(pt.x - _points[i].x, pt.y - _points[i].y) < 30) {
                    _index[1] = i;
                    break;
                }
            }
        }
    }
    else if ([gesture numberOfTouches] == _touchCount) {
        CGPoint pt = [gesture locationOfTouch:0 inView:self];
        if (_index[0] >= 0 && CGRectContainsPoint(self.bounds, pt)) {
            _points[_index[0]] = pt;
            [self setNeedsDisplay];
        }
        pt = _touchCount > 1 ? [gesture locationOfTouch:1 inView:self] : pt;
        if (_touchCount > 1 && _index[1] >= 0 && CGRectContainsPoint(self.bounds, pt)) {
            _points[_index[1]] = pt;
            [self setNeedsDisplay];
        }
    }
    if (gesture.state >= UIGestureRecognizerStateEnded) {
        _index[0] = -1;
        _index[1] = -1;
        [self setNeedsDisplay];
    }
    
    return _index[0] >= 0 || _index[1] >= 0;
}

- (BOOL)gesturePan:(UIPanGestureRecognizer *)gesture
{
    return [self gestureFreeDrag:gesture];
}

- (BOOL)gesturePinch:(UIPinchGestureRecognizer *)gesture
{
    return [self gestureFreeDrag:gesture];
}

- (BOOL)gestureRotate:(UIRotationGestureRecognizer *)gesture
{
    return [self gestureFreeDrag:gesture];
}

- (BOOL)gestureTwoPan:(UIPanGestureRecognizer *)gesture
{
    return [self gestureFreeDrag:gesture];
}

@end
