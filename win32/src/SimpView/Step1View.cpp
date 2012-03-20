// Step1View.cpp : implementation of the CRandomShapeView class
//

#include "stdafx.h"
#include "resource.h"
#include "Step1View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRandomShapeView::CRandomShapeView(RandomParam& param)
	: m_selection(NULL)
{
	param.initShapes(m_shapes);
}

CRandomShapeView::~CRandomShapeView()
{
}

BEGIN_MESSAGE_MAP(CRandomShapeView, CBaseView)
	//{{AFX_MSG_MAP(CRandomShapeView)
    ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CRandomShapeView::OnDynDraw(GiGraphics* gs)
{
    if (m_selection)
    {
        GiContext context(-4, GiColor(0, 0, 255, 55));
        m_selection->draw(*gs, &context);

        context.setLineWidth(0);
        gs->drawEllipse(&context, Box2d(m_ptNear, 1, 1));
        gs->drawLine(&context, m_ptNear, m_ptSnap);
    }
    CBaseView::OnDynDraw(gs);
}

void CRandomShapeView::OnMouseMove(UINT nFlags, CPoint point)
{
    CBaseView::OnMouseMove(nFlags, point);
    
    Box2d rect(Point2d(point.x, point.y), 50, 0);
    
    rect *= m_xf.displayToModel();
    m_ptSnap = rect.center();
    m_selection = m_shapes->hitTest(rect, m_ptNear, m_segment);
    Invalidate();
}
