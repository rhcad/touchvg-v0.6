//! \file mgshapetype.h
//! \brief 定义图形类型号 MgShapeType
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_SHAPETYPE_H_
#define __GEOMETRY_SHAPETYPE_H_

//! 图形类型号
/*! \ingroup GEOM_SHAPE
*/
typedef enum {
    kMgShapeList = 1,           //!< 图形列表,MgShapes
    kMgShapeWrapper = 2,        //!< 封装图形,MgShape
    kMgShapeBase = 3,           //!< 图形基类,MgBaseShape
    kMgShapeBaseRect = 4,       //!< 矩形基类,MgBaseRect
    kMgShapeBaseLines = 5,      //!< 折线基类,MgBaseLines
    kMgShapeComposite = 6,      //!< 复合图形,MgComposite
    kMgShapeBaseMark = 7,       //!< 符号基类,MgBaseMark
    kMgShapeDoc = 8,            //!< 图形文档,MgShapeDoc

    kMgShapeGroup = 9,          //!< 成组图形
    kMgShapeLine = 10,          //!< 线段
    kMgShapeRect = 11,          //!< 矩形、正方形
    kMgShapeEllipse = 12,       //!< 椭圆、圆
    kMgShapeRoundRect = 13,     //!< 圆角矩形
    kMgShapeDiamond = 14,       //!< 菱形
    kMgShapeLines = 15,         //!< 折线、多边形
    kMgShapeSplines = 16,       //!< 样条曲线
    kMgShapeParallelogram = 17, //!< 平行四边形
    kMgShapeImage = 18,         //!< 图像矩形
    kMgShapeArc = 19,           //!< 圆弧
    kMgShapeGrid = 20,          //!< 网格
    kMgShapeCube = 21,          //!< 立方体、长方体
    kMgShapeCubeProj = 22,      //!< 立方体三视图
    kMgShapeCylinder = 23,      //!< 圆柱体
} MgShapeType;

#endif // __GEOMETRY_SHAPETYPE_H_
