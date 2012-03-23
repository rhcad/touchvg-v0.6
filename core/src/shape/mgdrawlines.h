// mgdrawlines.h: 定义折线绘图命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_MGCOMMAND_DRAW_LINES_H_
#define __GEOMETRY_MGCOMMAND_DRAW_LINES_H_

#include "mgcmddraw.h"

class MgCmdDrawLines : public MgCmdBaseLines
{
public:
    MgCmdDrawLines();
    virtual ~MgCmdDrawLines();

    static const char* Name() { return "lines"; }
    static MgCommand* Create() { return new MgCmdDrawLines; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender);
};

#endif // __GEOMETRY_MGCOMMAND_DRAW_LINES_H_
