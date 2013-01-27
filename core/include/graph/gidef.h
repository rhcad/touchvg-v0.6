//! \file gidef.h
//! \brief 定义图形接口库的基本宏
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef __GEOMETRY_GIDEF_H_
#define __GEOMETRY_GIDEF_H_

#ifndef SWIG
#ifdef _MACOSX
#include <libkern/OSAtomic.h>
inline long giInterlockedIncrement(volatile long *p) { return OSAtomicIncrement32((volatile int32_t *)p); }
inline long giInterlockedDecrement(volatile long *p) { return OSAtomicDecrement32((volatile int32_t *)p); }
#elif !defined(_WIN32)
inline long giInterlockedIncrement(volatile long *p) { return ++*p; }
inline long giInterlockedDecrement(volatile long *p) { return --*p; }
#else
#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
inline long giInterlockedIncrement(volatile long *p) { return InterlockedIncrement(p); }
inline long giInterlockedDecrement(volatile long *p) { return InterlockedDecrement(p); }
#endif
#endif // SWIG

//! 矢量路径节点类型
/*! \see GiPath
*/
typedef enum {
    kGiCloseFigure = 1,
    kGiLineTo = 2,
    kGiBeziersTo = 4,
    kGiMoveTo = 6,
} GiPathNode;

//! 颜色模式定义
typedef enum {
    kGiColorReal,       //!< 真彩
    kGiColorGray,       //!< 灰度，256级
    kGiColorMono        //!< 单色，背景色和反色
} GiColorMode;

#endif // __GEOMETRY_GIDEF_H_
