//! \file gdiobj.h
//! \brief 定义和实现GDI资源管理类 KGDIObject

#ifndef __GDIOBJ_H_
#define __GDIOBJ_H_

//! GDI资源管理类
class KGDIObject
{
public:
    //! 默认构造函数
    KGDIObject() : m_hOld(NULL), m_hdc(NULL), m_hObj(NULL), m_bCreated(false)
    {
    }
    
    //! 指定资源句柄的构造函数
    /*! 如果hdc或hObj为空，则不将资源选入设备描述表，仅记下其值
        \param hdc 设备描述表的句柄
        \param hObj GDI资源句柄，例如画笔、画刷、字体资源句柄
        \param bCreated 给定的GDI资源句柄是否需要自动释放
    */
    KGDIObject(HDC hdc, HGDIOBJ hObj, bool bCreated = true)
        : m_hOld(NULL), m_hdc(hdc), m_hObj(hObj), m_bCreated(bCreated)
    {
        if (hObj != NULL && hdc != NULL)
            m_hOld = ::SelectObject(hdc, hObj);
    }
    
    //! 析构函数，自动释放
    ~KGDIObject()
    {
        Detach();
    }
    
    //! 指定新的GDI资源，释放原来的资源引用
    /*!
        \param hObj GDI资源句柄，例如画笔、画刷、字体资源句柄
        \param bCreated 给定的GDI资源句柄是否需要自动释放
    */
    void Attach(HGDIOBJ hObj, bool bCreated = true)
    {
        if (hObj == m_hObj)
            return;
        Detach();
        m_hObj = hObj;
        m_bCreated = bCreated;
        if (hObj != NULL && m_hdc != NULL)
            m_hOld = ::SelectObject(m_hdc, hObj);
    }
    
    //! 释放对资源的引用
    /*! 如果在构造或Attach函数中指定bCreated为true，则将自动删除GDI资源
    */
    void Detach()
    {
        if (m_hObj != NULL)
        {
            if (m_hdc != NULL)
                ::SelectObject(m_hdc, m_hOld);
            if (m_bCreated)
                ::DeleteObject(m_hObj);
            m_hObj = NULL;
            m_hOld = NULL;
        }
    }
    
    //! 返回GDI资源句柄
    HGDIOBJ GetHandle() const
    {
        return m_hObj;
    }
    
    //! 设置新的设备描述表, 必须是在未引用GDI资源时使用
    void SetHDC(HDC hdc)
    {
        if (m_hObj == NULL)
            m_hdc = hdc;
    }

private:
    HGDIOBJ     m_hOld;
    HDC         m_hdc;
    HGDIOBJ     m_hObj;
    bool        m_bCreated;
};

#endif // __GDIOBJ_H_