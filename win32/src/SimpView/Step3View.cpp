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

CDrawShapeView::CDrawShapeView(RandomParam& param)
	: CScrollShapeView(param), m_nCmdID(ID_CMD_SELECT)
{
}

CDrawShapeView::~CDrawShapeView()
{
}

BEGIN_MESSAGE_MAP(CDrawShapeView, CScrollShapeView)
	//{{AFX_MSG_MAP(CDrawShapeView)
    ON_UPDATE_COMMAND_UI_RANGE(ID_CMD_SELECT, ID_CMD_SPLINES, OnUpdateCmds)
    ON_COMMAND_RANGE(ID_CMD_SELECT, ID_CMD_SPLINES, OnCmds)
    ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

class MgViewProxy : public MgView
{
    CBaseView*  _view;
public:
    MgViewProxy(CBaseView* view) : _view(view) {}
private:
    MgShapes* shapes() { return _view->m_shapes; }
    GiTransform* xform() { return &_view->m_xf; }
    GiGraphics* graph() { return _view->m_gs; }
    void redraw() { _view->Invalidate(); }
    void regen() {
        _view->m_gs->clearCachedBitmap();
        _view->Invalidate();
    }
};

void CDrawShapeView::OnUpdateCmds(CCmdUI* pCmdUI)
{
    pCmdUI->SetCheck(m_nCmdID == pCmdUI->m_nID ? 1 : 0);
}

void CDrawShapeView::OnCmds(UINT nID)
{
    m_nCmdID = nID;
}

void CDrawShapeView::OnDynDraw(GiGraphics* gs)
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    if (cmd) {
        //cmd->draw()
    }
}

void CDrawShapeView::OnMouseMove(UINT nFlags, CPoint point)
{
    CBaseView::OnMouseMove(nFlags, point);
}
