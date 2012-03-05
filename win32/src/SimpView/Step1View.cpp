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
{
	param.initShapes(&m_shapes);
}

CRandomShapeView::~CRandomShapeView()
{
}

BEGIN_MESSAGE_MAP(CRandomShapeView, CBaseView)
	//{{AFX_MSG_MAP(CRandomShapeView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
