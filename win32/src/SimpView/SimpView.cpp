// SimpView.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SimpView.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "NewViewDlg.h"
#include "Step3View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimpViewApp

BEGIN_MESSAGE_MAP(CSimpViewApp, CWinApp)
	//{{AFX_MSG_MAP(CSimpViewApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSimpViewApp::CSimpViewApp()
{
}

CSimpViewApp theApp;

BOOL CSimpViewApp::InitInstance()
{
	RandomParam::init();

	CMDIFrameWnd* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// Create main MDI frame window
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	// Load shared MDI menus and accelerator table
	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_STEP1_VIEW));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_STEP1_VIEW));

	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();
    pFrame->PostMessage(WM_COMMAND, ID_FILE_NEW);

	return TRUE;
}

int CSimpViewApp::ExitInstance() 
{
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);

	return CWinApp::ExitInstance();
}

CWnd* CreateChildView(UINT nFrameID)
{
	CNewViewDlg dlg;
	CBaseView* pView = NULL;

	if (IDOK != dlg.DoModal())
		return NULL;

	if (IDR_STEP1_VIEW == nFrameID)
	{
		RandomParam param;
        param.lineCount = dlg.m_bUseRand ? dlg.m_nLineCount : 0;
        param.rectCount = dlg.m_bUseRand ? param.rectCount : 0;
		param.arcCount = dlg.m_bUseRand ? dlg.m_nArcCount : 0;
        param.curveCount = dlg.m_bUseRand ? dlg.m_nCurveCount : 0;
		param.randomLineStyle = !!dlg.m_bRandomLineStyle;

        if (dlg.m_bWithCmd)
            pView = new CDrawShapeView(param);
		else if (dlg.m_bScrollBar)
			pView = new CScrollShapeView(param);
		else
			pView = new CRandomShapeView(param);
	}

	return pView;
}

void CSimpViewApp::OnFileNew() 
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);

	// create a new MDI child window
	pFrame->CreateNewChild(
		RUNTIME_CLASS(CChildFrame), IDR_STEP1_VIEW, m_hMDIMenu, m_hMDIAccel);
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CSimpViewApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CSimpViewApp message handlers

