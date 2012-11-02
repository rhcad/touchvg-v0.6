#ifndef __GEOMETRY_MGCOMMAND_BREAK_H_
#define __GEOMETRY_MGCOMMAND_BREAK_H_

#include <mgcmd.h>

class MgCommandBreak : MgBaseCommand
{
protected:
    MgCommandBreak() {}
    virtual ~MgCommandBreak() {}

public:
    static const char* Name() { return "break"; }
    static MgCommand* Create() { return new MgCommandBreak; }

private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual void gatherShapes(const MgMotion* sender, MgShapes* shapes);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
    virtual bool isFloatingCommand() { return true; }
    
private:
    MgShape*    _target;
    int         _edges[2];
    Point2d     _crosspt[2];
};

#endif // __GEOMETRY_MGCOMMAND_BREAK_H_
