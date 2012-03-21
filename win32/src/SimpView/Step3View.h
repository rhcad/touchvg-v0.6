// Step3View.h : interface of the CDrawShapeView class
//
#pragma once

#include "Step2View.h"

class CDrawShapeView : public CScrollShapeView
{
// Construction
public:
	CDrawShapeView(RandomParam& param);
	virtual ~CDrawShapeView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawShapeView)
	//}}AFX_VIRTUAL
    protected:
    virtual void OnDynDraw(GiGraphics* gs);

// Generated message map functions
protected:
	//{{AFX_MSG(CDrawShapeView)
    afx_msg void OnUpdateCmds(CCmdUI* pCmdUI);
    afx_msg void OnCmds(UINT nID);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
private:
    UINT    m_nCmdID;
};
