// Step2View.cpp : implementation of the CScrollShapeView class
//

#include "stdafx.h"
#include "resource.h"
#include "Step2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CScrollShapeView::CScrollShapeView(RandomParam& param)
	: CRandomShapeView(param)
{
	m_bRealPan = true;
}

CScrollShapeView::~CScrollShapeView()
{
}

BEGIN_MESSAGE_MAP(CScrollShapeView, CRandomShapeView)
	//{{AFX_MSG_MAP(CScrollShapeView)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CScrollShapeView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CRandomShapeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// 显示出滚动条，暂时双向都不可滚动
	//
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE;
	si.nMin = 0;
	si.nMax = 1;
	::SetScrollInfo(m_hWnd, SB_HORZ, &si, FALSE);
	::SetScrollInfo(m_hWnd, SB_VERT, &si, FALSE);
	::EnableScrollBar(m_hWnd, SB_BOTH, ESB_DISABLE_BOTH);
	
	return 0;
}

void CScrollShapeView::OnZoomed()
{
	CRandomShapeView::OnZoomed();

	Box2d rect;

	// m_rcLimits: 极限范围对应的坐标范围, 像素
	rect = m_shapes->getExtent() * m_xf.modelToDisplay();
	rect.inflate(1, 1);
	rect.get(m_rcLimits.left, m_rcLimits.top, m_rcLimits.right, m_rcLimits.bottom);

	// m_rcScrWnd: 当前窗口对应的坐标范围, 像素
	rect = Box2d(m_xf.getCenterW(),
		m_xf.getWidth() / m_xf.getWorldToDisplayX(),
		m_xf.getHeight() / m_xf.getWorldToDisplayY());
	rect *= m_xf.worldToDisplay();
	rect.get(m_rcScrWnd.left, m_rcScrWnd.top, m_rcScrWnd.right, m_rcScrWnd.bottom);

	// 以m_rcLimits左上角为原点调整m_rcScrWnd和m_rcLimits
	::OffsetRect(&m_rcScrWnd, -m_rcLimits.left, -m_rcLimits.top);
	::OffsetRect(&m_rcLimits, -m_rcLimits.left, -m_rcLimits.top);

	// 避免m_rcScrWnd部分超出m_rcLimits
	::UnionRect(&m_rcLimits, &m_rcLimits, &m_rcScrWnd);

	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
	si.nMin = 0;
	
	if (m_rcLimits.right - m_rcLimits.left > m_rcScrWnd.right - m_rcScrWnd.left)
	{
		si.nMax = max(1, m_rcLimits.right - m_rcLimits.left);
		si.nPage = m_rcScrWnd.right - m_rcScrWnd.left;
		si.nPos = m_rcScrWnd.left;
		::EnableScrollBar(m_hWnd, SB_HORZ, ESB_ENABLE_BOTH);
		::SetScrollInfo(m_hWnd, SB_HORZ, &si, TRUE);
	}
	else
	{
		::EnableScrollBar(m_hWnd, SB_HORZ, ESB_DISABLE_BOTH);
	}
	
	if (m_rcLimits.bottom - m_rcLimits.top > m_rcScrWnd.bottom - m_rcScrWnd.top)
	{
		si.nMax = max(1, m_rcLimits.bottom - m_rcLimits.top);
		si.nPage = m_rcScrWnd.bottom - m_rcScrWnd.top;
		si.nPos = m_rcScrWnd.top;
		::EnableScrollBar(m_hWnd, SB_VERT, ESB_ENABLE_BOTH);
		::SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
	}
	else
	{
		::EnableScrollBar(m_hWnd, SB_VERT, ESB_DISABLE_BOTH);
	}
}

void CScrollShapeView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar*)
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
	::GetScrollInfo(m_hWnd, SB_HORZ, &si);
	int nPage = static_cast<int>(si.nPage);
	
	switch (nSBCode)
	{
	case SB_LEFT:				// Scroll to far left.
		si.nPos = si.nMin;
		break;
	case SB_LINELEFT:			// Scroll left.
		si.nPos = max(si.nPos - max(20, nPage / 10), si.nMin);
		break;
	case SB_LINERIGHT:			// Scroll right.
		si.nPos = min(si.nPos + max(20, nPage / 10), si.nMax);
		break;
	case SB_PAGELEFT:			// Scroll one page left.
		si.nPos = max(si.nPos - nPage, si.nMin);
		break;
	case SB_PAGERIGHT:			// Scroll one page right.
		si.nPos = min(si.nPos + nPage, si.nMax);
		break;
	case SB_RIGHT:				// Scroll to far right.
		si.nPos = si.nMax;
		break;
	case SB_THUMBPOSITION:		// Scroll to absolute position.
		si.nPos = nPos;
		m_sizePan.cx = m_sizePan.cy = 0;
		break;
	case SB_THUMBTRACK:			// Drag to absolute position.
		OnHScrThumbTrack(si, nPos);
		break;
	case SB_ENDSCROLL:			// End draging
		if (m_xf.zoomPan(m_rcScrWnd.left - si.nPos, 0))
		{
			::OffsetRect(&m_rcScrWnd, si.nPos - m_rcScrWnd.left, 0);
			m_sizePan.cx = 0;
			Invalidate();
		}
		else
			si.nPos = m_rcScrWnd.left;
		break;
	}
	
	::SetScrollPos(m_hWnd, SB_HORZ, si.nPos, TRUE);
}

void CScrollShapeView::OnHScrThumbTrack(SCROLLINFO &si, UINT nPos)
{
	si.nPos = nPos;
	m_sizePan.cx = m_rcScrWnd.left - si.nPos;
	Invalidate();
	if (m_bRealPan || fabs(m_xf.getViewScale() - 1.0) < 0.01)
	{
		if (m_xf.zoomPan(m_rcScrWnd.left - si.nPos, 0))
		{
			::OffsetRect(&m_rcScrWnd, si.nPos - m_rcScrWnd.left, 0);
			m_sizePan.cx = 0;
		}
		else
			si.nPos = m_rcScrWnd.left;
	}
}

void CScrollShapeView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar*)
{
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
	::GetScrollInfo(m_hWnd, SB_VERT, &si);
	int nPage = static_cast<int>(si.nPage);
	
	switch (nSBCode)
	{
	case SB_TOP:				// Scroll to far up.
		si.nPos = si.nMin;
		break;
	case SB_LINEUP:				// Scroll up.
		si.nPos = max(si.nPos - max(20, nPage / 10), si.nMin);
		break;
	case SB_LINEDOWN:			// Scroll down.
		si.nPos = min(si.nPos + max(20, nPage / 10), si.nMax);
		break;
	case SB_PAGEUP:				// Scroll one page up.
		si.nPos = max(si.nPos - nPage, si.nMin);
		break;
	case SB_PAGEDOWN:			// Scroll one page down.
		si.nPos = min(si.nPos + nPage, si.nMax);
		break;
	case SB_BOTTOM:				// Scroll to far down.
		si.nPos = si.nMax;
		break;
	case SB_THUMBPOSITION:		// Scroll to absolute position.
		si.nPos = nPos;
		m_sizePan.cx = m_sizePan.cy = 0;
		break;
	case SB_THUMBTRACK:			// Drag to absolute position.
		OnVScrThumbTrack(si, nPos);
		break;
	case SB_ENDSCROLL:			// End draging
		if (m_xf.zoomPan(0, m_rcScrWnd.top - si.nPos))
		{
			::OffsetRect(&m_rcScrWnd, 0, si.nPos - m_rcScrWnd.top);
			m_sizePan.cy = 0;
			Invalidate();
		}
		else
			si.nPos = m_rcScrWnd.top;
		break;
	}
	
	::SetScrollPos(m_hWnd, SB_VERT, si.nPos, TRUE);
}

void CScrollShapeView::OnVScrThumbTrack(SCROLLINFO &si, UINT nPos)
{
	si.nPos = nPos;
	m_sizePan.cy = m_rcScrWnd.top - si.nPos;
	Invalidate();
	if (m_bRealPan || fabs(m_xf.getViewScale() - 1.0) < 0.01)
	{
		if (m_xf.zoomPan(0, m_rcScrWnd.top - si.nPos))
		{
			::OffsetRect(&m_rcScrWnd, 0, si.nPos - m_rcScrWnd.top);
			m_sizePan.cy = 0;
		}
		else
			si.nPos = m_rcScrWnd.top;
	}
}
