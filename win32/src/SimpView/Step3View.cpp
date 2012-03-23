// Step3View.cpp : implementation of the CDrawShapeView class
//

#include "stdafx.h"
#include "resource.h"
#include "Step3View.h"
#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT WM_DELAY_LBUTTONUP = WM_USER + 101;

CDrawShapeView::CDrawShapeView(RandomParam& param)
	: CScrollShapeView(param), m_cmdID(ID_CMD_SELECT)
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
    ON_MESSAGE_VOID(WM_DELAY_LBUTTONUP, OnDelayLButtonUp)
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

    if ((nFlags & MK_LBUTTON) && (m_downFlags & MK_LBUTTON))
    {
        MgCommand* cmd = mgGetCommandManager()->getCommand();

        m_motion.point = point;
        m_motion.pointM = Point2d(point.x, point.y) * m_xf.displayToModel();

        if (!m_moved && mgHypot(point.x - m_motion.startPoint.x, point.y - m_motion.startPoint.y) > 5)
        {
            m_moved = TRUE;
            if (cmd) cmd->touchesBegan(&m_motion);
        }
        else if (m_moved)
        {
            if (cmd) cmd->touchesMoved(&m_motion);
        }

        m_motion.lastPoint = m_motion.point;
        m_motion.lastPointM = m_motion.pointM;
    }
}

void CDrawShapeView::OnLButtonDown(UINT nFlags, CPoint point)
{
    CBaseView::OnLButtonDown(nFlags, point);
    SetCapture();

    if (!m_delayUp)
    {
        m_motion.startPoint = point;
        m_motion.startPointM = Point2d(point.x, point.y) * m_xf.displayToModel();
        m_motion.point = m_motion.startPoint;
        m_motion.pointM = m_motion.startPointM;
        m_moved = FALSE;
        m_delayUp = FALSE;
        m_downTime = GetTickCount();
        m_downFlags = nFlags;
    }
}

void CDrawShapeView::OnLButtonUp(UINT nFlags, CPoint point)
{
    CBaseView::OnLButtonUp(nFlags, point);
    ReleaseCapture();

    if (m_moved)
    {
        MgCommand* cmd = mgGetCommandManager()->getCommand();
        if (cmd) cmd->touchesEnded(&m_motion);
    }
    else if (!m_delayUp && point == m_motion.startPoint)
    {
        PostMessage(WM_DELAY_LBUTTONUP);
        m_delayUp = TRUE;
    }
}

void CDrawShapeView::OnDelayLButtonUp()
{
    if (m_delayUp)
    {
        if (GetTickCount() - m_downTime < 5000)
        {
            PostMessage(WM_DELAY_LBUTTONUP);
        }
        else
        {
            MgCommand* cmd = mgGetCommandManager()->getCommand();
            if (cmd) cmd->click(&m_motion);

            m_delayUp = FALSE;
            m_downFlags = 0;
        }
    }
}

void CDrawShapeView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    CBaseView::OnLButtonDblClk(nFlags, point);

    m_delayUp = FALSE;
    m_downFlags = 0;

    MgCommand* cmd = mgGetCommandManager()->getCommand();
    if (cmd) cmd->doubleClick(&m_motion);
}
