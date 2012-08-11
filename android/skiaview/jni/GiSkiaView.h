//! \file GiSkiaView.h
//! \brief 定义支持Android平台的图形视图类 GiSkiaView
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __TOUCHVG_SKIAVIEW_H_
#define __TOUCHVG_SKIAVIEW_H_

#include "GiCanvasBase.h"

class GiCmdController;
class MgStorageBase;
struct MgShapes;

//! 支持Android平台的图形视图类
/*! \ingroup GRAPH_SKIA
*/
class GiSkiaView
{
public:
    GiSkiaView(GiCanvasBase* canvas);
    virtual ~GiSkiaView();
    
    //! 保存图形列表
    bool saveShapes(MgStorageBase* s);

    //! 加载图形列表，s为NULL则清空
    bool loadShapes(MgStorageBase* s);

    //! 添加测试图形
    int addTestingShapes();

    //! 返回视图宽度，像素
    int getWidth() const;

    //! 返回视图高度，像素
    int getHeight() const;

    //! 设置视图宽高，像素
    void onSize(int width, int height);

    //! 显示正式图形
    bool onDraw(GiCanvasBase& canvas);

    //! 显示临时动态图形
    bool onDynDraw(GiCanvasBase& canvas);

private:
    GiCanvasBase* 		_canvas;
    MgShapes*           _shapes;
    GiCmdController*    _cmdc;
};

#endif // __TOUCHVG_SKIAVIEW_H_
