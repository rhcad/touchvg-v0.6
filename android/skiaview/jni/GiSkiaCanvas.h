//! \file GiSkiaCanvas.h
//! \brief 定义用Skia实现的图形显示类 GiSkiaCanvas
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __TOUCHVG_SKIACANVAS_H_
#define __TOUCHVG_SKIACANVAS_H_

#include <gigraph.h>
#include <mgshapes.h>

//! 用Skia实现的图形显示类
/*! \ingroup GRAPH_SKIA
*/
class GiSkiaCanvas
{
public:
    GiSkiaCanvas();
    virtual ~GiSkiaCanvas();
};

#endif // __TOUCHVG_SKIACANVAS_H_
