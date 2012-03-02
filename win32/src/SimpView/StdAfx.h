// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <graphwin.h>
using namespace gs;

static inline double RandDbl(double dMin, double dMax)
{
	return (double)rand() / (RAND_MAX + 1) * (dMax - dMin) + dMin;
}

static inline long RandInt(long nMin, long nMax)
{
	return MulDiv(rand(), nMax - nMin, RAND_MAX + 1) + nMin;
}

static inline UInt8 RandUInt8(UInt8 nMin, UInt8 nMax)
{
	return (UInt8)RandInt(nMin, nMax);
}
