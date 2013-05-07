#pragma once

#include <GraphView.h>

class CEditorView : public GiGraphViewMfc
{
public:
    CEditorView();
    virtual ~CEditorView();

    bool LoadFile(const CString& filename);
    bool LoadFile(LPCTSTR bkpic, LPCTSTR vgfile, LPCTSTR vgpic, int w = 0, int h = 0);

// Generated message map functions
protected:
    //{{AFX_MSG(CEditorView)
    afx_msg void OnUpdateCmds(CCmdUI* pCmdUI);
    afx_msg void OnCmds(UINT nID);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnFileNew();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    void OnSaved();
    UINT GetFirstCmdID() const;
    UINT GetContextActionMenuID() const;
    UINT GetContextActionFirstID() const;
    UINT GetDefaultContextMenuID(int& subMenuIndex) const;
    void UpdateStatusBar(CPoint point);

private:
    CSize   m_picSize;
    CString m_vgpic;
};
