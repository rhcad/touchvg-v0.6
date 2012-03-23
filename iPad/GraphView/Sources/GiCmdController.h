// GiCmdController.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import <Graph2d/GiMotionHandler.h>

struct MgMotion;
class MgViewProxy;

@interface GiCommandController : NSObject<GiMotionHandler> {
    MgMotion    *_motion;
    MgViewProxy *_mgview;
}

@end
