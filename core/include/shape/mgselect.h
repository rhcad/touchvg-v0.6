//! \file mgselect.h
//! \brief 定义选择集接口
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGSELECTION_H_
#define __GEOMETRY_MGSELECTION_H_

#include <mgshape.h>

struct MgView;
struct MgMotion;

//! 选择状态
typedef enum {
    kMgSelNone,         //!< 没有选择图形
    kMgSelOneShape,     //!< 选择了一个图形
    kMgSelMultiShapes,  //!< 选择了多个图形
    kMgSelVertex        //!< 选择了一个图形的节点
} MgSelState;

//! 选择集接口
/*! \ingroup GEOM_SHAPE
    \interface MgSelection
*/
struct MgSelection {
    //! 得到当前选择的图形
    virtual UInt32 getSelection(MgView* view, UInt32 count, MgShape** shapes, bool forChange = false) = 0;
    
    //! 返回选择状态
    virtual MgSelState getSelectState(MgView* view) = 0;
    
    //! 选中所有图形
    virtual bool selectAll(MgView* view) = 0;
    
    //! 删除选中的图形
    virtual bool deleteSelection(MgView* view) = 0;
    
    //! 对选中的图形复制出新图形，并选中新图形
    virtual bool cloneSelection(MgView* view) = 0;
    
    //! 清除选择集
    virtual void resetSelection(MgView* view) = 0;

    //! 添加选择图形
    virtual bool addSelection(MgView* view, UInt32 shapeID) = 0;
    
    //! 删除当前图形的选中的顶点
    virtual bool deleteVertext(const MgMotion* sender) = 0;
    
    //! 在当前位置插入一个顶点
    virtual bool insertVertext(const MgMotion* sender) = 0;
    
    //! 切换当前图形是否闭合
    virtual bool switchClosed(MgView* view) = 0;
    
    //! 返回当前选中图形是否固定边长
    virtual bool isFixedLength(MgView* view) = 0;
    
    //! 设置当前选中图形是否固定边长
    virtual bool setFixedLength(MgView* view, bool fixed) = 0;
};

#endif // __GEOMETRY_MGSELECTION_H_
