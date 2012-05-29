//! \file mgselect.h
//! \brief 定义选择集接口
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_MGSELECTION_H_
#define __GEOMETRY_MGSELECTION_H_

#include <mgshape.h>

struct MgView;
struct MgMotion;

//! 选择集接口
/*! \ingroup GEOM_SHAPE
    \interface MgSelection
*/
struct MgSelection {
    enum kSelState { kSelNone, kSelOneShape, kSelMultiShapes, kSelVertex };
    
    //! 得到当前选择的图形
    virtual UInt32 getSelection(MgView* view, UInt32 count, MgShape** shapes, bool forChange) = 0;
    
    //! 返回选择状态
    virtual kSelState getSelectState(MgView* view) = 0;
    
    //! 选中所有图形
    virtual bool selectAll(MgView* view) = 0;
    
    //! 删除选中的图形
    virtual bool deleteSelection(MgView* view) = 0;
    
    //! 对选中的图形复制出新图形，并选中新图形
    virtual bool cloneSelection(MgView* view) = 0;
    
    //! 清除选择集
    virtual void resetSelection(MgView* view) = 0;
    
    //! 删除当前图形的选中的顶点
    virtual bool deleteVertext(const MgMotion* sender) = 0;
    
    //! 在当前位置插入一个顶点
    virtual bool insertVertext(const MgMotion* sender) = 0;
    
    //! 切换当前图形是否闭合
    virtual bool switchClosed(MgView* view) = 0;
};

#endif // __GEOMETRY_MGSELECTION_H_
