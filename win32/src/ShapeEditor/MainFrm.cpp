#include "stdafx.h"
#include "resource.h"
#include "MainFrm.h"
#include "EditorView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_CLOSE()
    ON_WM_SETFOCUS()
    ON_WM_ACTIVATE()
    ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
    ID_INDICATOR_MODIFIED,
    ID_INDICATOR_LINEWIDTH,
	ID_INDICATOR_X,
	ID_INDICATOR_Y,
    ID_INDICATOR_VIEWSCALE,
};

CMainFrame::CMainFrame()
{
    m_view = new CEditorView();
}

CMainFrame::~CMainFrame()
{
    delete m_view;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_SYSMENU;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

	return TRUE;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

    if (!m_view->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		return -1;
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
	        sizeof(indicators)/sizeof(UINT)))
	{
		return -1;
	}

	return 0;
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    if (m_view->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
        return TRUE;

    return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnClose()
{
    if (SaveModified())             // 退出前询问保存
        CFrameWnd::OnClose();
}

BOOL CMainFrame::LoadLastFile()
{
    CString file(AfxGetApp()->GetProfileString(_T("Last"), _T("File")));
    return !file.IsEmpty() && m_view->LoadFile(file);
}

void CMainFrame::OnFileOpen()
{
    CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
        _T("矢量图形文件 (*.vg, *.txt)|*.vg;*.txt|")
        _T("矢量图形文件 (*.vg)|*.vg|")
        _T("矢量文本文件 (*.txt)|*.txt||"), this);

    if (SaveModified() && dlg.DoModal() == IDOK) {
        if (m_view->LoadFile(dlg.GetPathName())) {
            AfxGetApp()->WriteProfileString(_T("Last"), _T("File"), dlg.GetPathName());
        }
    }
}

BOOL CMainFrame::LoadFile(LPCTSTR bkpic, LPCTSTR path,
                          LPCTSTR vgfile, LPCTSTR vgpic, int w, int h)
{
    TCHAR vgfile2[MAX_PATH], vgpic2[MAX_PATH];

    StrCpyN(vgfile2, path, MAX_PATH);
    PathAppend(vgfile2, vgfile);
    StrCpyN(vgpic2, path, MAX_PATH);
    PathAppend(vgpic2, vgpic);

    BOOL ret = m_view->LoadFile(bkpic, vgfile2, vgpic2, w, h);

    m_view->SetFocus();

    return ret;
}

void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
    CFrameWnd::OnSetFocus(pOldWnd);
    m_view->SetFocus();
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    if (nState == WA_ACTIVE) {
        m_view->CheckNewBitmap();
    }
	CFrameWnd::OnActivate(nState, pWndOther, bMinimized);
}

BOOL CMainFrame::SaveModified()
{
    if (!m_view->IsModified())
        return TRUE;

    CString prompt;

    AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, 
        PathFindFileName(m_view->GetFileName()));

	switch (AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
	{
	case IDCANCEL:
		return FALSE;       // don't continue

	case IDYES:
		// If so, either Save or Update, as appropriate
		m_view->SendMessage(WM_COMMAND, ID_FILE_SAVE);
		break;

	case IDNO:
		break;
    }

    return TRUE;
}

void CMainFrame::GetMessageString(UINT nID, CString& rMessage) const
{
    CString text(m_view->GetCmdCaption(nID));

    if (!text.IsEmpty()) {
        rMessage.Format(_T("启动 %s 命令"), text);
    }
    else {
        CFrameWnd::GetMessageString(nID, rMessage);
    }
}
