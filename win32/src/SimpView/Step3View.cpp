// Step3View.cpp : implementation of the CDrawShapeView class
//

#include "stdafx.h"
#include "resource.h"
#include "Step3View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const UINT WM_DELAY_LBUTTONUP = WM_USER + 101;

CDrawShapeView::CDrawShapeView(RandomParam& param)
	: CScrollShapeView(param), m_cmdID(0)
    , m_moved(FALSE), m_delayUp(FALSE), m_downTime(0), m_downFlags(0)
{
    m_mgview.view = this;
    m_motion.view = &m_mgview;
}

CDrawShapeView::~CDrawShapeView()
{
}

BEGIN_MESSAGE_MAP(CDrawShapeView, CScrollShapeView)
	//{{AFX_MSG_MAP(CDrawShapeView)
    ON_UPDATE_COMMAND_UI_RANGE(ID_CMD_SELECT, ID_CMD_SPLINES, OnUpdateCmds)
    ON_COMMAND_RANGE(ID_CMD_SELECT, ID_CMD_SPLINES, OnCmds)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
    ON_MESSAGE(WM_DELAY_LBUTTONUP, OnDelayLButtonUp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

const char* CDrawShapeView::getCmdName(UINT nID) const
{
    if (nID == ID_CMD_SELECT)
        return "select";
    if (nID == ID_CMD_LINE)
        return "line";
    if (nID == ID_CMD_RECT)
        return "rect";
    if (nID == ID_CMD_ELLIPSE)
        return "ellipse";
    if (nID == ID_CMD_LINES)
        return "lines";
    if (nID == ID_CMD_SPLINES)
        return "splines";
    return "";
}

void CDrawShapeView::OnUpdateCmds(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_cmdID == pCmdUI->m_nID ? 1 : 0);
}

void CDrawShapeView::OnCmds(UINT nID)
{
    m_cmdID = nID;
    mgGetCommandManager()->setCommand(&m_motion, getCmdName(nID));
}

void CDrawShapeView::OnDynDraw(GiGraphics* gs)
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    if (cmd) {
        cmd->draw(&m_motion, gs);
    }
}

void CDrawShapeView::OnMouseMove(UINT nFlags, CPoint point)
{
    CBaseView::OnMouseMove(nFlags, point);

    if (!m_delayUp
        && (nFlags & MK_LBUTTON)
        && (m_downFlags & MK_LBUTTON))
    {
        MgCommand* cmd = mgGetCommandManager()->getCommand();

        m_motion.point = Point2d((float)point.x, (float)point.y);
        m_motion.pointM = m_motion.point * m_xf.displayToModel();

        if (!m_moved && mgHypot(m_motion.point.x - m_motion.startPoint.x, 
            m_motion.point.y - m_motion.startPoint.y) > 5)
        {
            m_moved = TRUE;
            if (cmd) cmd->touchBegan(&m_motion);
        }
        else if (m_moved)
        {
            if (cmd) cmd->touchMoved(&m_motion);
        }

        m_motion.lastPoint = m_motion.point;
        m_motion.lastPointM = m_motion.pointM;
    }
}

void CDrawShapeView::OnLButtonDown(UINT nFlags, CPoint point)
{
    CBaseView::OnLButtonDown(nFlags, point);
    SetCapture();

    m_motion.startPoint = Point2d((float)point.x, (float)point.y);
    m_motion.startPointM = m_motion.startPoint * m_xf.displayToModel();
    m_motion.point = m_motion.startPoint;
    m_motion.pointM = m_motion.startPointM;
    m_moved = FALSE;
    m_delayUp = FALSE;
    m_downTime = GetTickCount();
    m_downFlags = nFlags;
}

void CDrawShapeView::OnLButtonUp(UINT nFlags, CPoint point)
{
    CBaseView::OnLButtonUp(nFlags, point);
    ReleaseCapture();

    if (m_delayUp)
    {
        m_delayUp = FALSE;
    }
    else if (m_moved)
    {
        MgCommand* cmd = mgGetCommandManager()->getCommand();
        if (cmd) cmd->touchEnded(&m_motion);
    }
    else if (Point2d((float)point.x, (float)point.y) == m_motion.startPoint)
    {
        PostMessage(WM_DELAY_LBUTTONUP, m_downTime, m_downFlags);
        m_delayUp = TRUE;
    }
}

LRESULT CDrawShapeView::OnDelayLButtonUp(WPARAM wp, LPARAM lp)
{
    long downTime = wp;
    UINT downFlags = lp;

    if (downTime != m_downTime)
    {
        m_delayUp = FALSE;
    }
    else if (m_delayUp)
    {
        if (GetTickCount() - m_downTime < 100)
        {
            PostMessage(WM_DELAY_LBUTTONUP, downTime, downFlags);
        }
        else
        {
            MgCommand* cmd = mgGetCommandManager()->getCommand();
            if (cmd) cmd->click(&m_motion);

            m_delayUp = FALSE;
            m_downFlags = 0;
        }
    }

    return 0;
}

void CDrawShapeView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    CBaseView::OnLButtonDblClk(nFlags, point);

    m_delayUp = FALSE;
    m_downFlags = 0;

    MgCommand* cmd = mgGetCommandManager()->getCommand();
    if (cmd) cmd->doubleClick(&m_motion);
}
