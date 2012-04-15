// Step3View.h : interface of the CDrawShapeView class
//
#pragma once

#include "Step2View.h"
#include <mgcmd.h>

class CDrawShapeView : public CScrollShapeView
{
// Construction
public:
	CDrawShapeView(RandomParam& param);
	virtual ~CDrawShapeView();

// Attributes
public:
    GiContext   m_context;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDrawShapeView)
	//}}AFX_VIRTUAL
    protected:
    virtual void OnDynDraw(GiGraphics* gs);

// Generated message map functions
protected:
	//{{AFX_MSG(CDrawShapeView)
    afx_msg void OnUpdateCmds(CCmdUI* pCmdUI);
    afx_msg void OnCmds(UINT nID);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg LRESULT OnDelayLButtonUp(WPARAM wp, LPARAM lp);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Implementation
private:
    class MgViewProxy : public MgView
    {
    public:
        CDrawShapeView* view;
        MgViewProxy() : view(NULL) {}
    private:
        MgShapes* shapes() { return view->m_shapes; }
        GiTransform* xform() { return &view->m_xf; }
        GiGraphics* graph() { return &view->m_gs; }
        void redraw() { view->Invalidate(); }
        void regen() {
            view->m_gs.clearCachedBitmap();
            view->Invalidate();
        }
        const GiContext* context() {
            RandomParam().setShapeProp(&view->m_context);
            return &view->m_context;
        }
        bool shapeWillAdded(MgShape*) { return true; }
        bool shapeWillDeleted(MgShape*) { return true; }
    };

    const char* getCmdName(UINT nID) const;

    UINT        m_cmdID;
    MgViewProxy m_mgview;
    MgMotion    m_motion;
    BOOL        m_moved;
    BOOL        m_delayUp;
    long        m_downTime;
    UINT        m_downFlags;
};
