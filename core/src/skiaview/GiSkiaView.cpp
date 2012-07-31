// GiSkiaView.cpp
// Copyright (c) 2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "GiSkiaView.h"
#include "GiSkiaCanvas.h"
#include "GiCmdController.h"
#include <mgshapest.h>
#include <list>

MgShapes* GiSkiaView::createShapes()
{
    return new MgShapesT<std::list<MgShape*> >;
}

GiSkiaView::GiSkiaView(MgShapes* shapes) : m_gs(&m_xf), m_shapes(shapes)
{
    m_canvas = new GiSkiaCanvas();
    m_cmdc = new GiCmdController;
}

GiSkiaView::~GiSkiaView()
{
    delete m_cmdc;
    delete m_canvas;
}

bool GiSkiaView::saveShapes(MgStorageBase* s)
{
    return s && m_shapes && m_shapes->save(s);
}

bool GiSkiaView::loadShapes(MgStorageBase* s)
{
    return s && m_shapes && m_shapes->load(s);
}
