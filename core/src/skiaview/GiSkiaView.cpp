// GiSkiaView.cpp
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "GiSkiaView.h"
#include "GiSkiaCanvas.h"
#include "GiCmdController.h"
#include <mgshapest.h>
#include <list>

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

void GiSkiaView::setDpi(float dpi)
{
}

void GiSkiaView::onsize(int width, int height)
{
}

void GiSkiaView::ondraw(void* canvas)
{
}

