//! \file iosgraph.h
//! \brief Define the graphics class for iOS: GiGraphIos
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_GRAPHIOS_H_
#define __GEOMETRY_GRAPHIOS_H_

#include "ioscanvas.h"
#include <gigraph.h>

//! The graphics class for iOS.
/*! \ingroup GRAPH_IOS
*/
class GiGraphIos
{
public:
    GiTransform xf;
    GiGraphics  gs;
    GiCanvasIos canvas;

    GiGraphIos(float dpi = 0) : gs(&xf), canvas(&gs, dpi) {}
};

#endif // __GEOMETRY_GRAPHIOS_H_
