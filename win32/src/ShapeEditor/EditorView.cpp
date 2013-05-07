#include "stdafx.h"
#include "EditorView.h"
#include "resource.h"

CEditorView::CEditorView()
{
}

CEditorView::~CEditorView()
{
}

BEGIN_MESSAGE_MAP(CEditorView, GiGraphViewMfc)
    //{{AFX_MSG_MAP(CEditorView)
    ON_COMMAND(ID_VIEW_BKCOLOR, OnViewBkColor)
    ON_COMMAND(ID_VIEW_BKIMAGE, OnViewBkImage)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_VIEWSCALE, OnUpdateViewScale)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_MODIFIED, OnUpdateModified)
    ON_COMMAND(ID_ZOOM_EXTENT, OnZoomExtent)
    ON_COMMAND(ID_ZOOM_100, OnZoom100)
    ON_COMMAND(ID_VIEW_ZOOMIN, OnZoomIn)
    ON_COMMAND(ID_VIEW_ZOOMOUT, OnZoomOut)
    ON_COMMAND(ID_PAN_LEFT, OnPanLeft)
    ON_COMMAND(ID_PAN_RIGHT, OnPanRight)
    ON_COMMAND(ID_PAN_UP, OnPanUp)
    ON_COMMAND(ID_PAN_DOWN, OnPanDown)
    ON_UPDATE_COMMAND_UI_RANGE(ID_CMD_FIRST, ID_CMD_LAST, OnUpdateCmds)
    ON_COMMAND_RANGE(ID_CMD_FIRST, ID_CMD_LAST, OnCmds)
    ON_WM_MOUSEMOVE()
    ON_COMMAND(ID_FILE_NEW, OnFileNew)
    ON_UPDATE_COMMAND_UI_RANGE(ID_DUMMY_1, ID_DUMMY_20, OnUpdateContextItems)
    ON_COMMAND_RANGE(ID_DUMMY_1, ID_DUMMY_20, OnContextItems)
    ON_UPDATE_COMMAND_UI(ID_FILE_RELOAD, OnUpdateFileReload)
    ON_COMMAND(ID_FILE_RELOAD, OnFileReload)
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_LINEWIDTH, OnUpdateLineWidth)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PENMODE, OnUpdatePenMode)
    ON_COMMAND(ID_VIEW_PENMODE, OnPenMode)
    ON_COMMAND(ID_LINE_COLOR, OnLineColor)
    ON_COMMAND(ID_LINE_BOLD, OnLineBold)
    ON_COMMAND(ID_LINE_THICK, OnLineThick)
    ON_COMMAND(ID_LINE_STYLE, OnLineStyle)
    ON_COMMAND(ID_LINE_TRANSPARENT, OnLineTransparent)
    ON_COMMAND(ID_LINE_OPAQUE, OnLineOpaque)
    ON_COMMAND(ID_FILL_COLOR, OnFillColor)
    ON_COMMAND(ID_FILL_TRANSPARENT, OnFillTransparent)
    ON_COMMAND(ID_FILL_OPAQUE, OnFillOpaque)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CEditorView::UpdateStatusBar(CPoint point)
{
    CStatusBar* pStatusBar = DYNAMIC_DOWNCAST(CStatusBar,
        AfxGetMainWnd()->GetDlgItem(AFX_IDW_STATUS_BAR));
    ASSERT(pStatusBar != NULL);

    Point2d pnt((float)point.x, (float)point.y);
    pnt *= xform()->displayToModel();

    CString str;
    int index = pStatusBar->CommandToIndex(ID_INDICATOR_X);
    if (index >= 0)
    {
        str.Format(_T("X: %.2lf"), pnt.x);
        pStatusBar->SetPaneText(index, str);
    }

    index = pStatusBar->CommandToIndex(ID_INDICATOR_Y);
    if (index >= 0)
    {
        str.Format(_T("Y: %.2lf"), pnt.y);
        pStatusBar->SetPaneText(index, str);
    }
}

void CEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
    GiGraphViewMfc::OnMouseMove(nFlags, point);
    UpdateStatusBar(point);
}

void CEditorView::OnUpdateCmds(CCmdUI* pCmdUI)
{
    GiGraphViewMfc::OnUpdateCmds(pCmdUI);
}

void CEditorView::OnCmds(UINT nID)
{
    GiGraphViewMfc::OnCmds(nID);
}

UINT CEditorView::GetFirstCmdID() const
{
    return ID_CMD_FIRST;
}

UINT CEditorView::GetContextActionMenuID() const
{
    return IDR_CONTEXTMENU;
}

UINT CEditorView::GetContextActionFirstID() const
{
    return ID_DUMMY_1;
}

UINT CEditorView::GetDefaultContextMenuID(int& subMenuIndex) const
{
    subMenuIndex = 1;
    return IDR_MAINFRAME;
}

bool CEditorView::LoadFile(const CString& filename)
{
    return LoadFile(_T(""), filename, _T(""));
}

bool CEditorView::LoadFile(LPCTSTR bkpic, LPCTSTR vgfile, LPCTSTR vgpic, int w, int h)
{
    LoadImage(bkpic);
    bool ret = LoadShapeFile(vgfile);

    if (m_doc->getPageRectW().isEmpty() && w > 0 && h > 0) {
        m_doc->setPageRectW(Box2d(Point2d(), (float)w, (float)h), 0);
    }
    m_vgpic = vgpic;
    m_picSize = CSize(w, h);

    if (ret && m_doc->getShapeCount() > 0) {
        PostMessage(WM_COMMAND, ID_CMD_SELECT);
    }

    return ret;
}

void CEditorView::OnFileNew()
{
    LoadFile(_T(""));
}

void CEditorView::OnSaved()
{
    AfxGetApp()->WriteProfileString(_T("Last"), _T("File"), GetFileName());

    if (m_vgpic.IsEmpty()) {
        m_vgpic = GetFileName();
        int pos = m_vgpic.ReverseFind('.');
        if (pos > 0)
            m_vgpic = m_vgpic.Left(pos);
        m_vgpic += _T(".png");
    }
    if (m_picSize.cx < 1 || m_picSize.cy < 1) {
        m_picSize.cx = 1024;
        m_picSize.cy = 768;
    }
    ExportImage(m_vgpic, m_picSize.cx, m_picSize.cy);
}
