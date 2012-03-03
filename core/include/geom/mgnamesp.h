//! \file mgnamesp.h
//! \brief 定义通用图形平台命名空间
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#ifndef __GEOMETRY_NAMESPACE_H_
#define __GEOMETRY_NAMESPACE_H_

#if defined(__APPLE__)
#define _GEOM_BEGIN
#define _GEOM_END
#else
#define _GEOM_BEGIN     namespace gs {
#define _GEOM_END       };
#endif

#endif // __GEOMETRY_NAMESPACE_H_