// ViewFactory.mm
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GraphView1.h"
#import "LargeView1.h"

static UIViewController *_tmpController = nil;

static void addView(NSMutableArray *arr, NSString* title, UIView* view)
{
    if (arr) {
        [arr addObject:title];
    }
    else if (view) {
        _tmpController = [[UIViewController alloc] init];
        _tmpController.title = title;
        _tmpController.view = view;
    }
}

static void addView1(NSMutableArray *arr, NSUInteger &i, NSUInteger index, 
                     NSString* title, int flags, CGRect frame)
{
    GraphView1 *view = nil;
    
    if (!arr && index == i++) {
        view = [[GraphView1 alloc]initWithFrame:frame withFlags:flags];
    }
    addView(arr, title, view);
    [view release];
}

static void addLargeView1(NSMutableArray *arr, NSUInteger &i, NSUInteger index, 
                          NSString* title, int flags, CGRect frame)
{
    LargeView1 *view = nil;
    
    if (!arr && index == i++) {
        view = [[LargeView1 alloc]initWithFrame:frame withFlags:flags];
    }
    addView(arr, title, view);
    [view release];
}

static void gatherTestView(NSMutableArray *arr, NSUInteger index, CGRect frame)
{
    NSUInteger i = 0;
    
    addView1(arr, i, index, @"testTouch", 0, frame);
    addView1(arr, i, index, @"testGesture", 1, frame);
    addView1(arr, i, index, @"testGestureTouches", 2, frame);
    
    addLargeView1(arr, i, index, @"testTouch in large view", 0, frame);
    addLargeView1(arr, i, index, @"testGesture in large view", 1, frame);
    addLargeView1(arr, i, index, @"testGestureTouches in large view", 2, frame);
}

void getTestViewTitles(NSMutableArray *arr)
{
    gatherTestView(arr, 0, CGRectNull);
}

UIViewController *createTestView(NSUInteger index, CGRect frame)
{
    _tmpController = nil;
    gatherTestView(nil, index, frame);
    return _tmpController;
}
