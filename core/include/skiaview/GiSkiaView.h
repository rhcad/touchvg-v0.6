//! \file GiSkiaView.h
//! \brief 定义支持Android平台的图形视图类 GiSkiaView
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __TOUCHVG_SKIAVIEW_H_
#define __TOUCHVG_SKIAVIEW_H_

#include <gigraph.h>

class GiSkiaCanvas;
class GiCmdController;
class MgStorageBase;
struct MgShapes;

//! 支持Android平台的图形视图类
/*! \ingroup GRAPH_SKIA
*/
class GiSkiaView
{
public:
    GiSkiaView();
    virtual ~GiSkiaView();
    
    //! 保存图形列表
    bool saveShapes(MgStorageBase* s);
    
    //! 加载图形列表
    bool loadShapes(MgStorageBase* s);

    void setDpi(float dpi);
    void onsize(int width, int height);
    void ondraw(int canvas);

private:
    GiTransform         m_xf;
    GiGraphics          m_gs;
    GiSkiaCanvas*       m_canvas;
    MgShapes*           m_shapes;
    GiCmdController*    m_cmdc;
};

#endif // __TOUCHVG_SKIAVIEW_H_
