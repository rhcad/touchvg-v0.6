// ChildFrm.cpp : implementation of the CStep1Frame class
//

#include "stdafx.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CWnd* CreateChildView(UINT nFrameID);

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
    //{{AFX_MSG_MAP(CStep1Frame)
    ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
    ON_WM_SETFOCUS()
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

CChildFrame::CChildFrame()
    : m_pwndView(NULL)
{
}

CChildFrame::~CChildFrame()
{
    if (m_pwndView != NULL)
    {
        delete m_pwndView;
        m_pwndView = NULL;
    }
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if (!CMDIChildWnd::PreCreateWindow(cs))
        return FALSE;

    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
    cs.lpszClass = AfxRegisterWndClass(0);

    return TRUE;
}

void CChildFrame::OnFileClose()
{
    SendMessage(WM_CLOSE);
}

BOOL CChildFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle,
                            CWnd* pParentWnd, CCreateContext* pContext)
{
    ASSERT(NULL == m_pwndView);
    m_pwndView = CreateChildView(nIDResource);

    return m_pwndView != NULL
        && CMDIChildWnd::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext);
}

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // create a view to occupy the client area of the frame
    if (!m_pwndView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
        CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
    {
        TRACE0("Failed to create view window\n");
        return -1;
    }

    return 0;
}

void CChildFrame::OnSetFocus(CWnd* pOldWnd)
{
    CMDIChildWnd::OnSetFocus(pOldWnd);

    if (::IsWindow(m_pwndView->GetSafeHwnd()))
        m_pwndView->SetFocus();
}

BOOL CChildFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    if (::IsWindow(m_pwndView->GetSafeHwnd())
        && m_pwndView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
        return TRUE;

    return CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
