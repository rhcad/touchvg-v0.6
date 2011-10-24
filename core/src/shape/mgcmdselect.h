// mgcmdselect.h: 定义选择命令类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_MGCOMMAND_SELECT_H_
#define __GEOMETRY_MGCOMMAND_SELECT_H_

#include <mgcmd.h>
#include <vector>

class MgCommandSelect : public MgCommand
{
public:
    MgCommandSelect();
    virtual ~MgCommandSelect();

    static const char* Name() { return "select"; }
    static MgCommand* Create() { return new MgCommandSelect; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }

    virtual bool cancel(const MgMotion* sender);
    virtual bool initialize(const MgMotion* sender);
    virtual bool undo(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool click(const MgMotion* sender);
    virtual bool doubleClick(const MgMotion* sender);
    virtual bool longPress(const MgMotion* sender);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);

private:
    MgShape* hitTestAll(const MgMotion* sender, Point2d &ptNear, Int32 &segment);
    MgShape* getSelectedShape(const MgMotion* sender);
    bool canSelect(MgShape* shape, const MgMotion* sender);
    Int32 hitTestHandles(MgShape* shape, const MgMotion* sender);
    
    UInt32                  m_id;               // 选中图形的ID
    Point2d                 m_ptNear;           // 图形上的最近点
    Int32                   m_segment;          // 点中的是图行上的哪部分
    UInt32                  m_handleIndex;      // 点中的是哪个控制点
    MgShape                 *m_clonesp;         // 选中图形的复制对象
};

#endif // __GEOMETRY_MGCOMMAND_SELECT_H_
