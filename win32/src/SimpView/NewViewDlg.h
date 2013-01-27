// NewViewDlg.h : header file
//
#pragma once

class CNewViewDlg : public CDialog
{
// Construction
public:
	CNewViewDlg(CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CNewViewDlg)
	enum { IDD = IDD_NEW_VIEW };
	long	m_nLineCount;
	long	m_nArcCount;
    long	m_nCurveCount;
	BOOL	m_bScrollBar;
	BOOL	m_bRandomLineStyle;
    BOOL	m_bWithCmd;
    BOOL    m_bAddShapes;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewViewDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
