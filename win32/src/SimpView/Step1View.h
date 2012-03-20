// Step1View.h : interface of the CRandomShapeView class
//
#pragma once

#include "BaseView.h"

class CRandomShapeView : public CBaseView
{
// Construction
public:
	CRandomShapeView(RandomParam& param);
	virtual ~CRandomShapeView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRandomShapeView)
	//}}AFX_VIRTUAL
    protected:
    virtual void OnDynDraw(GiGraphics* gs);

// Generated message map functions
protected:
	//{{AFX_MSG(CRandomShapeView)
	//}}AFX_MSG
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

// Implementation
private:
    GiShape*    m_selection;
    Point2d     m_ptNear;
    Point2d     m_ptSnap;
    Int32       m_segment;
};
