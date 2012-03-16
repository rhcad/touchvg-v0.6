// GiCmdController.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import "GiGraphView.h"

@class GiGraphView;

@interface GiCommandController : NSObject<GiMotionHandler> {
    GiGraphView*    _view;
}

- (id)initWithView:(GiGraphView*)view;

@end
