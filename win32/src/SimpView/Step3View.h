// Step3View.h : interface of the CDrawShapeView class
//
#pragma once

#include "Step2View.h"

class MgViewEx;

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
    bool showContextActions(const int* actions);

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
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg LRESULT OnDelayLButtonUp(WPARAM wp, LPARAM lp);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnUpdateContextItems(CCmdUI* pCmdUI);
    afx_msg void OnContextItems(UINT nID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
private:
    const char* getCmdName(UINT nID) const;

    UINT        m_cmdID;
    MgViewEx*   m_proxy;
    BOOL        m_moved;
    BOOL        m_delayUp;
    long        m_downTime;
    UINT        m_downFlags;
    int         m_actions[20];
};
