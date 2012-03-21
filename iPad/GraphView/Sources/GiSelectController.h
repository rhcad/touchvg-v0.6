// GiSelectController.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import <Graph2d/GiMotionHandler.h>

class MgShape;

@interface GiSelectController : NSObject<GiMotionHandler> {
    id<GiView>      _view;
    MgShape*        _selection[100];
    int             _count;
}

- (id)initWithView:(id<GiView>)view;

@end
