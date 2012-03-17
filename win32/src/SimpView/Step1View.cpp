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
	: CBaseView(param.getShapeCount())
    , m_selection(NULL)
{
	param.initShapes(&m_shapes);
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
    if (m_selection) {
        GiContext context(-4, GiColor(0, 0, 255, 55));
        m_selection->draw(gs, &context);
    }
    CBaseView::OnDynDraw(gs);
}

void CRandomShapeView::OnMouseMove(UINT nFlags, CPoint point)
{
    CBaseView::OnMouseMove(nFlags, point);

    Box2d limits(Point2d(point.x, point.y), 10, 10);
    limits *= m_xf.displayToModel();

    double minDist = limits.width();
    ShapeItem* selection = NULL;

    for (int i = 0; i < m_shapes.getShapeCount(); i++)
    {
        ShapeItem* shape = m_shapes.getShape(i);
        if (shape->getExtent().isIntersect(limits))
        {
            double dist = _DBL_MAX;
            shape->hitTest(limits, dist);
            if (minDist > dist) {
                minDist = dist;
                selection = shape;
            }
        }
    }

    if (m_selection != selection) {
        m_selection = selection;
        Invalidate();
    }
}
