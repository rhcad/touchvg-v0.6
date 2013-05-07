#pragma once

class CEditorView;

class CMainFrame : public CFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

    BOOL LoadLastFile();
    BOOL LoadFile(LPCTSTR bkpic, LPCTSTR path,
        LPCTSTR vgfile, LPCTSTR vgpic, int w, int h);

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);
    virtual void GetMessageString(UINT nID, CString& rMessage) const;

public:
	virtual ~CMainFrame();

protected:
	CStatusBar      m_wndStatusBar;
    CSplitterWnd    m_wndSplitter;
    CEditorView*    m_view;

    BOOL SaveModified();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnClose();
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
    afx_msg void OnFileOpen();
	DECLARE_MESSAGE_MAP()
};


