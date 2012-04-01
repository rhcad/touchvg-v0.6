//! \file mgcmdselect.h
//! \brief 定义选择命令类 MgCommandSelect
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_MGCOMMAND_SELECT_H_
#define __GEOMETRY_MGCOMMAND_SELECT_H_

#include <mgcmd.h>
#include <vector>

//! 选择命令类
/*! \ingroup _GEOM_SHAPE_
*/
class MgCommandSelect : public MgCommand
{
public:
    MgCommandSelect();
    virtual ~MgCommandSelect();

    static const char* Name() { return "select"; }
    static MgCommand* Create() { return new MgCommandSelect; }
    UInt32 getSelection(MgView* view, UInt32 count, MgShape** shapes);
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }

    virtual bool cancel(const MgMotion* sender);
    virtual bool initialize(const MgMotion* sender);
    virtual bool undo(bool &enableRecall, const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool click(const MgMotion* sender);
    virtual bool doubleClick(const MgMotion* sender);
    virtual bool longPress(const MgMotion* sender);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);

private:
    UInt32 getStep() { return 0; }
    MgShape* hitTestAll(const MgMotion* sender, Point2d &ptNear, Int32 &segment);
    MgShape* getSelectedShape(const MgMotion* sender);
    bool canSelect(MgShape* shape, const MgMotion* sender);
    Int32 hitTestHandles(MgShape* shape, const Point2d& pointM);
    
    typedef std::vector<MgShape*>::iterator sel_iterator;
    sel_iterator getSelectedPostion(MgShape* shape);
    bool isSelected(MgShape* shape);
    
private:
    std::vector<MgShape*>   m_selection;        // 选中的图形
    std::vector<MgShape*>   m_cloneShapes;      // 选中图形的复制对象
    UInt32                  m_id;               // 选中图形的ID
    Point2d                 m_ptNear;           // 图形上的最近点
    Int32                   m_segment;          // 点中的是图行上的哪部分
    UInt32                  m_handleIndex;      // 点中的是哪个控制点
    bool                    m_insertPoint;      // 是否可插入新点
    bool                    m_showSel;          // 是否亮显选中的图形
};

#endif // __GEOMETRY_MGCOMMAND_SELECT_H_
