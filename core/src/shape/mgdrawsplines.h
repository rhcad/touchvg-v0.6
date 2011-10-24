// mgdrawsplines.h: 定义曲线绘图命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_MGCOMMAND_DRAW_SPLINES_H_
#define __GEOMETRY_MGCOMMAND_DRAW_SPLINES_H_

#include "mgcmddraw.h"

class MgCmdDrawSplines : public MgCmdBaseLines
{
public:
    MgCmdDrawSplines();
    virtual ~MgCmdDrawSplines();

    static const char* Name() { return "splines"; }
    static MgCommand* Create() { return new MgCmdDrawSplines; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender);

    virtual bool canAddPoint(const MgMotion* sender, bool ended);
};

#endif // __GEOMETRY_MGCOMMAND_DRAW_SPLINES_H_
