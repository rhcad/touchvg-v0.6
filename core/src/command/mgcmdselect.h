//! \file mgcmdselect.h
//! \brief 定义选择命令类 MgCmdSelect
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGCOMMAND_SELECT_H_
#define __GEOMETRY_MGCOMMAND_SELECT_H_

#include <mgcmd.h>
#include <mgselect.h>
#include <vector>

//! 选择命令类
/*! \ingroup CORE_COMMAND
*/
class MgCmdSelect : public MgCommand, public MgSelection
{
protected:
    MgCmdSelect();
    virtual ~MgCmdSelect();

public:
    static const char* Name() { return "select"; }
    static MgCommand* Create() { return new MgCmdSelect; }
    bool dynamicChangeEnded(MgView* view, bool apply);
    
public:
    virtual int getSelection(MgView* view, int count, MgShape** shapes, bool forChange = false);
    virtual MgSelState getSelectState(MgView* view);
    virtual bool selectAll(MgView* view);
    virtual bool deleteSelection(MgView* view);
    virtual bool cloneSelection(MgView* view);
    virtual bool groupSelection(MgView* view);
    virtual bool ungroupSelection(MgView* view);
    virtual void resetSelection(MgView* view);
    virtual bool addSelection(MgView* view, int shapeID);
    virtual bool deleteVertext(const MgMotion* sender);
    virtual bool insertVertext(const MgMotion* sender);
    virtual bool switchClosed(MgView* view);
    virtual bool isFixedLength(MgView* view);
    virtual bool setFixedLength(MgView* view, bool fixed);
    virtual bool isLocked(MgView* view);
    virtual bool setLocked(MgView* view, bool locked);
    virtual bool isVertexMode(MgView* view);
    virtual bool setVertexMode(MgView* view, bool vertexMode);
    virtual bool overturnPolygon(const MgMotion* sender);
    virtual bool handleTwoFingers(const MgMotion* sender, int state,
                                  const Point2d& pt1, const Point2d& pt2);
    virtual Box2d getBoundingBox(const MgMotion* sender);
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }

    virtual bool cancel(const MgMotion* sender);
    virtual bool initialize(const MgMotion* sender);
    virtual bool undo(bool &enableRecall, const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual void gatherShapes(const MgMotion* sender, MgShapes* shapes);
    virtual bool click(const MgMotion* sender);
    virtual bool doubleClick(const MgMotion* sender);
    virtual bool longPress(const MgMotion* sender);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
    virtual int getDimensions(MgView* view, float* vars, char* types, int count);

private:
    MgShape* getCurrentShape(const MgMotion* sender) { return getShape(m_id, sender); }
    int getStep() { return 0; }
    MgShape* hitTestAll(const MgMotion* sender, Point2d &nearpt, int &segment);
    MgShape* getSelectedShape(const MgMotion* sender);
    bool canSelect(MgShape* shape, const MgMotion* sender);
    int hitTestHandles(MgShape* shape, const Point2d& pointM, 
                         const MgMotion* sender, float tolmm = 10.f);
    bool isIntersectMode(const MgMotion* sender);
    Point2d snapPoint(const MgMotion* sender, const MgShape* shape);
    
    typedef std::vector<int>::iterator sel_iterator;
    sel_iterator getSelectedPostion(MgShape* shape);
    bool isSelected(MgShape* shape);
    MgShape* getShape(int id, const MgMotion* sender) const;
    bool isDragRectCorner(const MgMotion* sender, Matrix2d& mat);
    bool isCloneDrag(const MgMotion* sender);
    void cloneShapes(MgView* view);
    bool applyCloneShapes(MgView* view, bool apply, bool addNewShapes = false);
    bool canTransform(MgShape* shape, const MgMotion* sender);
    bool canRotate(MgShape* shape, const MgMotion* sender);
    
private:
    std::vector<int>        m_selIds;           // 选中的图形的ID
    std::vector<MgShape*>   m_clones;           // 选中图形的复制对象
    int                     m_id;               // 选中图形的ID
    Point2d                 m_ptNear;           // 图形上的最近点
    Point2d                 m_ptSnap;           // 捕捉点
    Point2d                 m_ptStart;          // touchBegan捕捉点
    int                     m_segment;          // 点中的是图行上的哪部分
    int                     m_handleIndex;      // 点中的是哪个控制点
    int                     m_rotateHandle;     // 旋转中心控制点
    int                     m_boxHandle;        // 选中框的活动控制点序号
    bool                    m_handleMode;       // 控制点修改模式
    bool                    m_insertPt;         // 是否可插入新点
    bool                    m_showSel;          // 是否亮显选中的图形
    bool                    m_boxsel;           // 是否开始框选
};

#endif // __GEOMETRY_MGCOMMAND_SELECT_H_
