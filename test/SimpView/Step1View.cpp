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

static inline void SetShapeProp(ShapeItem* pItem, RandomViewParam& param)
{
	pItem->lineColor.set(RandInt(0, 255), RandInt(0, 255), RandInt(0, 255));
	pItem->lineWidth = (Int16)RandInt(-10, 500);
	pItem->lineAlpha = (UInt8)RandInt(0, 255);
	pItem->lineStyle = (kLineStyle)(param.bRandomLineStyle ? RandInt(PS_SOLID, PS_NULL) : PS_SOLID);
}

CRandomShapeView::CRandomShapeView(RandomViewParam& param)
	: CBaseView(param.nLineCount + param.nArcCount)
{
	for (int i = 0; i < GetShapeCount(); i++)
	{
		bool bLine = (RandInt(0, 100) % 2 == 0);
		if (bLine && param.nLineCount > 0 || 0 == param.nArcCount)
		{
			LineItem* pItem = new LineItem();
			SetShape(i, pItem);
			param.nLineCount--;
			
			SetShapeProp(pItem, param);
			pItem->startpt.set(RandDbl(-1000.0, 1000.0), RandDbl(-1000.0, 1000.0));
			pItem->endpt.set(RandDbl(-1000.0, 1000.0), RandDbl(-1000.0, 1000.0));
		}
		else
		{
			ArcItem* pItem = new ArcItem();
			SetShape(i, pItem);
			param.nArcCount--;

			SetShapeProp(pItem, param);
			pItem->center.set(RandDbl(-1000.0, 1000.0), RandDbl(-1000.0, 1000.0));
			pItem->rx = RandDbl(1.0, 1000.0);
			pItem->ry = RandDbl(1.0, 1000.0);
			pItem->startAngle = RandDbl(0.0, _M_2PI);
			pItem->sweepAngle = RandDbl(0.0, M_PI_2 * 6);
		}
	}

	RecalcExtent();
}

CRandomShapeView::~CRandomShapeView()
{
}

BEGIN_MESSAGE_MAP(CRandomShapeView, CBaseView)
	//{{AFX_MSG_MAP(CRandomShapeView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
