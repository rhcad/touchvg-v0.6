//! \file graphwin.h
//! \brief Define the graphics class for Windows: GiGraphWin
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_GRAPHWIN_H_
#define __GEOMETRY_GRAPHWIN_H_

#include <canvaswin.h>
#include <gigraph.h>

//! The graphics class for Windows.
/*! example: new GiGraphWin(GiCanvasGdip::Create);
    \ingroup GRAPH_WIN
*/
class GiGraphWin
{
public:
    GiTransform     xf;
    GiGraphics      gs;
    GiCanvasWin     *canvas;

    GiGraphWin(GiCanvasWin* (*creator)(GiGraphics*))
        : gs(&xf), canvas(creator(&gs))
    {
    }

    ~GiGraphWin()
    {
        delete canvas;
    }

    void setCanvas(GiCanvasWin* (*creator)(GiGraphics*))
    {
        GiCanvasWin *newcs = creator(&gs);
        newcs->copy(*canvas);
        delete canvas;
        canvas = newcs;
    }
};

#endif // __GEOMETRY_GRAPHWIN_H_
