//! \file GiSkiaView.h
//! \brief 定义支持Android平台的图形视图类 GiSkiaView
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __TOUCHVG_SKIAVIEW_H_
#define __TOUCHVG_SKIAVIEW_H_

#include <gigraph.h>
#include <mgshapes.h>

class GiSkiaCanvas;
class GiCmdController;
class MgStorageBase;

//! 支持Android平台的图形视图类
/*! \ingroup GRAPH_SKIA
*/
class GiSkiaView
{
public:
    //! 给定已创建的图形列表，构造出图形视图对象
    /*!
        \param shapes 使用 createShapes() 创建的图形列表，或与其他视图共享图形列表
     */
    GiSkiaView(MgShapes* shapes);
    virtual ~GiSkiaView();

    //! 创建图形列表，使用 MgShapes::release() 释放
    static MgShapes* createShapes();
    
    //! 保存图形列表
    bool saveShapes(MgStorageBase* s);
    
    //! 加载图形列表
    bool loadShapes(MgStorageBase* s);

private:
    GiTransform         m_xf;
    GiGraphics          m_gs;
    GiSkiaCanvas*       m_canvas;
    MgShapes*           m_shapes;
    GiCmdController*    m_cmdc;
};

#endif // __TOUCHVG_SKIAVIEW_H_
