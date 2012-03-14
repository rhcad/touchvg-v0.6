// NewViewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "NewViewDlg.h"
#include "shape.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CNewViewDlg::CNewViewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNewViewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewViewDlg)
	m_bScrollBar = TRUE;
	m_bRandomLineStyle = TRUE;
	//}}AFX_DATA_INIT
    m_nLineCount = RandomParam::RandInt(0, 1000);
	m_nArcCount = RandomParam::RandInt(0, 600);
}

void CNewViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewViewDlg)
	DDX_Text(pDX, IDC_NEW_LINECOUNT, m_nLineCount);
	DDV_MinMaxLong(pDX, m_nLineCount, 0, 50000);
	DDX_Text(pDX, IDC_NEW_ARCCOUNT, m_nArcCount);
	DDV_MinMaxLong(pDX, m_nArcCount, 0, 50000);
	DDX_Check(pDX, IDC_NEW_SCROLLBAR, m_bScrollBar);
	DDX_Check(pDX, IDC_NEW_RANDLNSTYLE, m_bRandomLineStyle);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNewViewDlg, CDialog)
	//{{AFX_MSG_MAP(CNewViewDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
