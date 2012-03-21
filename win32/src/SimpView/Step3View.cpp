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
}

void CDrawShapeView::OnMouseMove(UINT nFlags, CPoint point)
{
    CBaseView::OnMouseMove(nFlags, point);
}
