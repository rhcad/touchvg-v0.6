// Step3View.cpp : implementation of the CDrawShapeView class
//

#include "stdafx.h"
#include "resource.h"
#include "Step3View.h"
#include <mgcmd.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const UINT WM_DELAY_LBUTTONUP = WM_USER + 101;

class MgViewEx : public MgView
{
public:
    CBaseView*      view;
    MgMotion        motion;

    MgViewEx(CBaseView* _view) : view(_view) {
        motion.view = this;
    }
private:
    MgShapes* shapes() { return view->m_shapes; }
    GiTransform* xform() { return &view->m_graph->xf; }
    GiGraphics* graph() { return &view->m_graph->gs; }
    void redraw(bool) { view->Invalidate(); }
    void regen() {
        view->m_graph->gs.clearCachedBitmap();
        view->Invalidate();
    }
    GiContext* context() {
        RandomParam().setShapeProp(view->m_shapes->context());
        return view->m_shapes->context();
    }
    bool useFingle() { return false; }
    void shapeAdded(MgShape* shape) { view->shapeAdded(shape); }
    bool shapeWillAdded(MgShape*) { return true; }
    bool shapeWillDeleted(MgShape*) { return true; }
    bool shapeCanRotated(MgShape*) { return true; }
    bool longPressSelection(int) { return false; }
    bool drawHandle(GiGraphics*, const Point2d&, bool) { return false; }
};

CDrawShapeView::CDrawShapeView(RandomParam& param)
	: CScrollShapeView(param), m_cmdID(0)
    , m_moved(FALSE), m_delayUp(FALSE), m_downTime(0), m_downFlags(0)
{
    m_proxy = new MgViewEx(this);
}

CDrawShapeView::~CDrawShapeView()
{
    delete m_proxy;
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
    mgGetCommandManager()->setCommand(&m_proxy->motion, getCmdName(nID));
}

void CDrawShapeView::OnDynDraw(GiGraphics* gs)
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    if (cmd) {
        cmd->draw(&m_proxy->motion, gs);
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

        m_proxy->motion.point = Point2d((float)point.x, (float)point.y);
        m_proxy->motion.pointM = m_proxy->motion.point * m_graph->xf.displayToModel();

        if (!m_moved && mgHypot(m_proxy->motion.point.x - m_proxy->motion.startPoint.x, 
            m_proxy->motion.point.y - m_proxy->motion.startPoint.y) > 5)
        {
            m_moved = TRUE;
            if (cmd) cmd->touchBegan(&m_proxy->motion);
        }
        else if (m_moved)
        {
            if (cmd) cmd->touchMoved(&m_proxy->motion);
        }

        m_proxy->motion.lastPoint = m_proxy->motion.point;
        m_proxy->motion.lastPointM = m_proxy->motion.pointM;
    }
}

void CDrawShapeView::OnLButtonDown(UINT nFlags, CPoint point)
{
    CBaseView::OnLButtonDown(nFlags, point);
    SetCapture();

    m_proxy->motion.startPoint = Point2d((float)point.x, (float)point.y);
    m_proxy->motion.startPointM = m_proxy->motion.startPoint * m_graph->xf.displayToModel();
    m_proxy->motion.point = m_proxy->motion.startPoint;
    m_proxy->motion.pointM = m_proxy->motion.startPointM;
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
        if (cmd) cmd->touchEnded(&m_proxy->motion);
    }
    else if (Point2d((float)point.x, (float)point.y) == m_proxy->motion.startPoint)
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
            if (cmd) cmd->click(&m_proxy->motion);

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
    if (cmd) cmd->doubleClick(&m_proxy->motion);
}
