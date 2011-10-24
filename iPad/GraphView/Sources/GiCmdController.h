// GiCmdController.h
// Copyright (c) 2012, Zhang Yungui <rhcad@hotmail.com>
// License: LGPL, https://github.com/rhcad/graph2d

#import <Graph2d/GiMotionHandler.h>

struct MgMotion;
class MgViewProxy;
class GiContext;

@interface GiCommandController : NSObject<GiMotionHandler> {
    MgMotion    *_motion;
    MgViewProxy *_mgview;
    GiContext   *_context;
    BOOL        undoFired;
}

@property (nonatomic,readonly)  GiContext*      context;
@property (nonatomic)           const char*     commandName;

@end
