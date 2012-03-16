// GiSelectController.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import "GiGraphView.h"

@class GiGraphView;

@interface GiSelectController : NSObject<GiMotionHandler> {
    GiGraphView*    _view;
    void*           _section[100];
    int             _count;
}

- (id)initWithView:(GiGraphView*)view;

@end
