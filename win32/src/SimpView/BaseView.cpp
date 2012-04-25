// BaseView.cpp : implementation of the CBaseView class
//

#include "stdafx.h"
#include "resource.h"
#include "BaseView.h"
#include <canvasgdip.h>
#include <canvasgdi.h>
#include <mgshapest.h>
#include <list>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBaseView::CBaseView()
{
    m_shapes = new MgShapesT<std::list<MgShape*> >;
	m_gdip = true;
    m_graph = new GiGraphWin(m_gdip ? GiCanvasGdip::Create : GiCanvasGdi::Create);

    m_pan.cx = m_pan.cy = 0;
    m_bkColor = GetSysColor(COLOR_WINDOW);
}

CBaseView::~CBaseView()
{
    if (m_shapes)
        m_shapes->release();
	delete m_graph;
}

BEGIN_MESSAGE_MAP(CBaseView, CWnd)
	//{{AFX_MSG_MAP(CBaseView)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_SIZE()
    ON_UPDATE_COMMAND_UI(ID_VIEW_GRAY, OnUpdateViewGray)
	ON_COMMAND(ID_VIEW_GRAY, OnViewGray)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GDIP, OnUpdateViewGdip)
	ON_COMMAND(ID_VIEW_GDIP, OnViewGdip)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ANTIALIAS, OnUpdateAntiAlias)
	ON_COMMAND(ID_VIEW_ANTIALIAS, OnViewAntiAlias)
	ON_COMMAND(ID_VIEW_BKCOLOR, OnViewBkColor)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_VIEWSCALE, OnUpdateViewScale)
	ON_COMMAND(ID_ZOOM_EXTENT, OnZoomExtent)
	ON_COMMAND(ID_VIEW_ZOOMIN, OnZoomIn)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnZoomOut)
	ON_COMMAND(ID_PAN_LEFT, OnPanLeft)
	ON_COMMAND(ID_PAN_RIGHT, OnPanRight)
	ON_COMMAND(ID_PAN_UP, OnPanUp)
	ON_COMMAND(ID_PAN_DOWN, OnPanDown)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CBaseView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

BOOL CBaseView::OnEraseBkgnd(CDC*) 
{
	return TRUE;							// 在OnPaint中会清除背景
}

void CBaseView::OnPaint() 
{
	CPaintDC dc(this);
    GiGraphics &gs = m_graph->gs;

	dc.SetBkColor(m_bkColor);				    // 在beginPaint()中应用背景色

	if (m_graph->canvas->beginPaint(dc))        // 准备绘图，使用绘图缓冲
	{
		// 显示先前保存的正式图形内容
		if (m_pan.cx != 0 || m_pan.cy != 0)     // 动态平移
			gs.clearWindow();			        // 清除缓存图外的背景
		if (!gs.drawCachedBitmap((float)m_pan.cx, (float)m_pan.cy))
		{
			if (0 == m_pan.cx && 0 == m_pan.cy)
				gs.clearWindow();		        // 清除背景
			DrawAll(&gs);                       // 显示正式图形
			gs.saveCachedBitmap();	            // 保存正式图形内容
		}

		OnDynDraw(&gs);                         // 显示动态图形

		m_graph->canvas->endPaint();            // 提交绘图结果到窗口
	}
}

void CBaseView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	// 设置显示窗口的大小为客户区大小(不包含滚动条)
	if (nType != SIZE_MINIMIZED && cx > 1 && cy > 1)
	{
		m_graph->xf.setWndSize(cx, cy);
		OnZoomed();
	}
}

void CBaseView::OnZoomed()
{
	Invalidate();
}

void CBaseView::DrawAll(GiGraphics* gs)
{
	m_shapes->draw(*gs);
}

void CBaseView::OnZoomExtent() 
{
	if (m_graph->xf.zoomTo(m_shapes->getExtent() * m_graph->xf.modelToWorld(), NULL))
	{
		OnZoomed();
	}
}

void CBaseView::OnViewGray() 
{
	m_graph->gs.setColorMode(GiGraphics::kColorGray == m_graph->gs.getColorMode()
        ? GiGraphics::kColorReal : GiGraphics::kColorGray);
    m_graph->gs.clearCachedBitmap();
    Invalidate();
}

void CBaseView::OnUpdateViewGray(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(GiGraphics::kColorGray == m_graph->gs.getColorMode() ? 1 : 0);
}

void CBaseView::OnUpdateViewGdip(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_gdip ? 1 : 0);
}

void CBaseView::OnViewGdip() 
{
	m_gdip = !m_gdip;
    m_graph->setCanvas(m_gdip ? GiCanvasGdip::Create : GiCanvasGdi::Create);
	Invalidate();
}

void CBaseView::OnUpdateAntiAlias(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_graph->gs.isAntiAliasMode() ? 1 : 0);
}

void CBaseView::OnViewAntiAlias() 
{
	m_graph->gs.setAntiAliasMode(!m_graph->gs.isAntiAliasMode());
	m_graph->gs.clearCachedBitmap();
	Invalidate();
}

void CBaseView::OnViewBkColor() 
{
	CColorDialog dlg (m_bkColor);
	if (IDOK == dlg.DoModal())
	{
		m_bkColor = dlg.GetColor();
		m_graph->gs.clearCachedBitmap();
		Invalidate();
	}
}

void CBaseView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd::OnMouseMove(nFlags, point);

	CStatusBar* pStatusBar = DYNAMIC_DOWNCAST(CStatusBar, 
		AfxGetMainWnd()->GetDlgItem(AFX_IDW_STATUS_BAR));
	ASSERT(pStatusBar != NULL);

	Point2d pnt((float)point.x, (float)point.y);
	pnt *= m_graph->xf.displayToModel();

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

void CBaseView::OnUpdateViewScale(CCmdUI* pCmdUI) 
{
	CString str;
	str.Format(_T("%.2lf%%"), m_graph->xf.getViewScale() * 100.0);
	pCmdUI->SetText(str);
}

void CBaseView::OnZoomIn() 
{
	if (m_graph->xf.zoomByFactor(0.2f))
	{
		OnZoomed();
	}
}

void CBaseView::OnZoomOut() 
{
	if (m_graph->xf.zoomByFactor(-0.2f))
	{
		OnZoomed();
	}
}

void CBaseView::OnPanLeft() 
{
	if (m_graph->xf.zoomPan(50, 0))
	{
		OnZoomed();
	}
}

void CBaseView::OnPanRight() 
{
	if (m_graph->xf.zoomPan(-50, 0))
	{
		OnZoomed();
	}
}

void CBaseView::OnPanUp() 
{
	if (m_graph->xf.zoomPan(0, 50))
	{
		OnZoomed();
	}
}

void CBaseView::OnPanDown() 
{
	if (m_graph->xf.zoomPan(0, -50))
	{
		OnZoomed();
	}
}
