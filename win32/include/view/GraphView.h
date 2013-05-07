//! \file GraphView.h
//! \brief 定义MFC绘图窗口基类 GiGraphViewMfc
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_MFC_GRAPHVIEW_H
#define TOUCHVG_MFC_GRAPHVIEW_H

#include <mgshapedoc.h>
#include <mgshape.h>
#include <graphwin.h>

class MgViewMfc;
class GiGdipImage;

//! MFC绘图窗口基类
/*! \ingroup GRAPH_WIN
*/
class GiGraphViewMfc : public CWnd
{
public:
    GiGraphViewMfc();
    virtual ~GiGraphViewMfc();

    MgShapeDoc* doc() { return m_doc; }
    GiTransform* xform() { return &m_graph.xf; }
    GiGraphics* graph() { return &m_graph.gs; }
    void shapeAdded(MgShape* shape);
    bool showContextActions(const int* actions);
    CString GetCmdCaption(UINT nID);

    GiContext& context();
    void applyContext(const GiContext& ctx, int mask, int apply = 1);
    bool setCommand(const char* name);
    CString getCommand() const;
    void regen();

    bool LoadImage(LPCTSTR filename);
    void CheckNewBitmap();
    void SetBackColor(COLORREF color);

    bool LoadShapeFile(LPCTSTR filename);
    bool LoadContent(LPCTSTR content);
    CString GetContent() const;
    bool IsModified() const;
    void SetModified(bool modified);
    CString GetFileName() const { return m_filename; }

    Box2d getPageRectW() const;
    void setPageRectW(const Box2d& rectW);

    bool ExportImage(LPCTSTR filename, int w, int h);

// Overrides
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void DrawShapes(GiGraphics* gs);
    virtual void DynDraw(GiGraphics* gs);
    virtual void OnZoomed(bool clear = true);

// Implementation
protected:
    MgShapeDoc*     m_doc;              // 图形数据
    GiGraphWin      m_graph;            // 图形系统对象
private:
    SIZE            m_pan;              // 动态平移显示的偏移
    COLORREF        m_bkColor;          // 窗口背景颜色

    RECT            m_rcLimits;         // 滚动条极限范围
    RECT            m_rcScrWnd;         // 滚动条当前窗口
    bool            m_bRealPan;         // 动态平移时, true:立即显示新内容, false:仅平移窗口图像

    
    MgShape*        m_shapeAdded;
    GiGdipImage*    m_image;
    int             m_lastChanges;

    CString         m_filename;
    CString         m_tmpPicFile;
    CString         m_picFile;
    CString         m_lastTime;

    MgViewMfc*      m_proxy;
    bool            m_moved;
    bool            m_delayUp;
    long            m_downTime;
    UINT            m_downFlags;
    int             m_actions[20];

// Generated message map functions
public:
    //{{AFX_MSG(GiGraphViewMfc)
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnUpdateViewGray(CCmdUI* pCmdUI);
    afx_msg void OnViewGray();
    afx_msg void OnUpdateAntiAlias(CCmdUI* pCmdUI);
    afx_msg void OnViewAntiAlias();
    afx_msg void OnViewBkColor();
    afx_msg void OnViewBkImage();
    afx_msg void OnUpdateViewScale(CCmdUI* pCmdUI);
    afx_msg void OnZoomExtent();
    afx_msg void OnZoomWidth();
    afx_msg void OnZoom100();
    afx_msg void OnZoomIn();
    afx_msg void OnZoomOut();
    afx_msg void OnPanLeft();
    afx_msg void OnPanRight();
    afx_msg void OnPanUp();
    afx_msg void OnPanDown();
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnUpdateRealPan(CCmdUI* pCmdUI);
    afx_msg void OnViewRealPan();

    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg LRESULT OnDelayLButtonUp(WPARAM wp, LPARAM lp);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg void OnUpdateContextItems(CCmdUI* pCmdUI);
    afx_msg void OnContextItems(UINT nID);
    afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnInitialUpdate();
    afx_msg void OnUpdateSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnSelectAll();
    afx_msg void OnUpdateClearAll(CCmdUI* pCmdUI);
	afx_msg void OnClearAll();
    afx_msg void OnUpdateModified(CCmdUI* pCmdUI);
    afx_msg void OnUpdateFileReload(CCmdUI* pCmdUI);
    afx_msg void OnFileReload();
    afx_msg void OnUpdateLineWidth(CCmdUI* pCmdUI);
    afx_msg void OnUpdatePenMode(CCmdUI* pCmdUI);
    afx_msg void OnPenMode();
    afx_msg void OnLineColor();
    afx_msg void OnLineBold();
    afx_msg void OnLineThick();
    afx_msg void OnLineStyle();
    afx_msg void OnLineTransparent();
    afx_msg void OnLineOpaque();
    afx_msg void OnFillColor();
    afx_msg void OnFillTransparent();
    afx_msg void OnFillOpaque();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    LPCSTR GetCmdName(UINT nID);
    void OnUpdateCmds(CCmdUI* pCmdUI);
    void OnCmds(UINT nID);
    virtual void OnSaved() {}
    virtual UINT GetFirstCmdID() const { return 0; }
    virtual UINT GetContextActionMenuID() const { return 0; }
    virtual UINT GetContextActionFirstID() const { return 0; }
    virtual UINT GetDefaultContextMenuID(int& subMenuIndex) const;

private:
    void OnPaint_(GiCanvasWin *cv);
    BOOL OnZoomKeyDown(UINT nChar);
    void OnHScrThumbTrack(SCROLLINFO &si, UINT nPos);
    void OnVScrThumbTrack(SCROLLINFO &si, UINT nPos);
    bool NeedUpdatePan();
    void Cancel();
    bool HasBackground() const;
    void DrawBackground(GiCanvasWin *cv);
    CString GetModifyTime(LPCTSTR filename);
};

#endif // TOUCHVG_MFC_GRAPHVIEW_H
