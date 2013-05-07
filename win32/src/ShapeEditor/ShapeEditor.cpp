#include "stdafx.h"
#include "resource.h"
#include "MainFrm.h"
#include "CmdLineParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CShapeEditorApp : public CWinApp
{
public:
	CShapeEditorApp();

    virtual BOOL InitInstance();

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

CShapeEditorApp theApp;

BEGIN_MESSAGE_MAP(CShapeEditorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()

CShapeEditorApp::CShapeEditorApp()
{
}

BOOL CShapeEditorApp::InitInstance()
{
	// 启用可视化方式
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
#ifdef afxAmbientActCtx
    afxAmbientActCtx = FALSE;           // 避免AfxDeactivateActCtx异常
#endif

	AfxOleInit();

	SetRegistryKey(_T("Founder"));
    SetCmdLineParams(m_lpCmdLine);      // 分析命令行

	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame || !pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pFrame;
    pFrame->SetIcon(LoadIcon(IDR_MAINFRAME), FALSE);

	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

    LPCTSTR value;
    CString bkpic, path, vgfile(_T("endorse.vg")), vgpic(_T("endorse.png"));
    int w = 1024, h = 768;

    if (FindCmdLineParam(_T("bkpic"), &value))
        bkpic = value;
    if (FindCmdLineParam(_T("path"), &value))
        path = value;
    if (FindCmdLineParam(_T("vgfile"), &value) && *value)
        vgfile = value;
    if (FindCmdLineParam(_T("vgpic"), &value) && *value)
        vgpic = value;
    if (FindCmdLineParam(_T("w"), &value))
        w = _ttoi(value);
    if (FindCmdLineParam(_T("h"), &value))
        h = _ttoi(value);

    if (path.IsEmpty()) {
        pFrame->LoadLastFile();
    }
    else {
        pFrame->LoadFile(bkpic, path, vgfile, vgpic, w, h);
    }

	return TRUE;
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

void CShapeEditorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
