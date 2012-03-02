// Step1View.h : interface of the CRandomShapeView class
//
#pragma once

#include "BaseView.h"

struct RandomViewParam
{
	long nLineCount;
	long nArcCount;
	BOOL bRandomLineStyle;
};

class CRandomShapeView : public CBaseView
{
// Construction
public:
	CRandomShapeView(RandomViewParam& param);
	virtual ~CRandomShapeView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRandomShapeView)
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CRandomShapeView)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
private:
};
