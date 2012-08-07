// GiSkiaView.cpp
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "GiSkiaView.h"
#include "GiSkiaCanvas.h"
#include "GiCmdController.h"
#include <mgshapest.h>
#include <list>
//#include <GraphicsJNI.h>

GiSkiaView::GiSkiaView() : m_gs(&m_xf)
{
    m_canvas = new GiSkiaCanvas();
    m_shapes = new MgShapesT<std::list<MgShape*> >;
    m_cmdc = new GiCmdController;
}

GiSkiaView::~GiSkiaView()
{
    delete m_cmdc;
    delete m_canvas;
    if (m_shapes)
        m_shapes->release();
}

bool GiSkiaView::saveShapes(MgStorageBase* s)
{
    return s && m_shapes && m_shapes->save(s);
}

bool GiSkiaView::loadShapes(MgStorageBase* s)
{
    return s && m_shapes && m_shapes->load(s);
}

int GiSkiaView::getWidth() const
{
    return m_xf.getWidth();
}

int GiSkiaView::getHeight() const
{
    return m_xf.getHeight();
}

void GiSkiaView::onSize(int width, int height)
{
    m_xf.setWndSize(width, height);
}

bool GiSkiaView::onDraw(jobject canvas)
{
	//SkCanvas* canv = GraphicsJNI::getNativeCanvas(env, canvas);
	//SkPaint paint;
	//paint.setColor(SK_ColorRED);
	//canv->drawText("hello skia", 10, 20, 20, paint);
    return !!canvas;
}
