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

//! 支持Android平台的图形视图类
/*! \ingroup GRAPH_SKIA
*/
class GiSkiaView
{
public:
    GiSkiaView(MgShapes* shapes);
    virtual ~GiSkiaView();

    static MgShapes* createShapes();

private:
    GiTransform         m_xf;
    GiGraphics          m_gs;
    GiSkiaCanvas*       m_canvas;
    MgShapes*           m_shapes;
    GiCmdController*    m_cmdc;
};

#endif // __TOUCHVG_SKIAVIEW_H_
