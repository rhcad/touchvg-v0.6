// SimpView.h : main header file for the SIMPVIEW application
//

#if !defined(AFX_SIMPVIEW_H__D7AC4E71_EFC2_456A_9436_6E0D2E86AEF7__INCLUDED_)
#define AFX_SIMPVIEW_H__D7AC4E71_EFC2_456A_9436_6E0D2E86AEF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSimpViewApp:
// See SimpView.cpp for the implementation of this class
//

class CSimpViewApp : public CWinApp
{
public:
	CSimpViewApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpViewApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;

public:
	//{{AFX_MSG(CSimpViewApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPVIEW_H__D7AC4E71_EFC2_456A_9436_6E0D2E86AEF7__INCLUDED_)
