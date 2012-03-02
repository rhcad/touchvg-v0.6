// Step2View.h : interface of the CScrollShapeView class
//
#pragma once

#include "Step1View.h"

class CScrollShapeView : public CRandomShapeView
{
// Construction
public:
	CScrollShapeView(RandomViewParam& param);
	virtual ~CScrollShapeView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScrollShapeView)
	//}}AFX_VIRTUAL
	virtual void OnZoomed();

// Generated message map functions
protected:
	//{{AFX_MSG(CScrollShapeView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
private:
	void OnHScrThumbTrack(SCROLLINFO &si, UINT nPos);
	void OnVScrThumbTrack(SCROLLINFO &si, UINT nPos);

private:
	RECT			m_rcLimits;			// 滚动条极限范围
	RECT			m_rcScrWnd;			// 滚动条当前窗口
	bool			m_bRealPan;			// 动态平移时, true:立即显示新内容, false:仅平移窗口图像
};
