// giimage.cpp: 实现图像文件打开和显示的辅助类 GiImage
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#include "giimage.h"
#include <olectl.h>
#ifdef _MSC_VER
#pragma comment(lib,"ole32.lib")
#pragma comment(lib,"olepro32.lib")
#endif

GiImage::GiImage() : m_picture(NULL), m_stream(NULL)
{
}

GiImage::GiImage(const char* filename) : m_picture(NULL), m_stream(NULL)
{
    open(filename);
}

GiImage::GiImage(const wchar_t* filename) : m_picture(NULL), m_stream(NULL)
{
    open(filename);
}

GiImage::~GiImage()
{
    clear();
}

void GiImage::clear()
{
    if (m_picture != NULL)
    {
        m_picture->Release();
        m_picture = NULL;
    }
    if (m_stream != NULL)
    {
        m_stream->Release();
        m_stream = NULL;
    }
}

static bool LoadImage(HANDLE hfile, kOpenImageError& err, 
                      LPPICTURE& picture, LPSTREAM& stream)
{
    HGLOBAL hGlobal = NULL;
    LPVOID pvData = NULL;
    DWORD fileSize;
    DWORD bytesRead = 0;

    picture = NULL;
    stream = NULL;

    fileSize = ::GetFileSize(hfile, NULL);
    if ((int)fileSize < 1)
        err = kOpenImage_NullFile;
    else if (fileSize > 16L*1024*1024)
        err = kOpenImage_TooLarge;
    else
    {
        hGlobal = ::GlobalAlloc(GMEM_MOVEABLE, fileSize);
        if (hGlobal != NULL)
            pvData = ::GlobalLock(hGlobal);

        if (NULL == pvData)
        {
            err = kOpenImage_NoMemory;
            ::GlobalFree(hGlobal);
        }
        else
        {
            ::ReadFile(hfile, pvData, fileSize, &bytesRead, NULL);
            ::GlobalUnlock(hGlobal);

            if (bytesRead == fileSize)
            {
                if (FAILED(::CreateStreamOnHGlobal(hGlobal, TRUE, &stream)))
                    err = kOpenImage_NoMemory;
            }
            else
            {
                ::GlobalFree(hGlobal);
                err = kOpenImage_ReadFail;
            }
        }
    }

    if (stream != NULL)
    {
        ::OleLoadPicture(stream, 0, FALSE, IID_IPicture, (LPVOID*)&picture);
        if (picture == NULL)
        {
            stream->Release();
            stream = NULL;
            err = kOpenImage_NotPicture;
        }
    }

    return picture != NULL;
}

bool GiImage::open(const char* filename, kOpenImageError* perr)
{
    clear();

    HANDLE hfile = INVALID_HANDLE_VALUE;
    kOpenImageError err = kOpenImage_OK;

    if (NULL == filename || 0 == *filename)
        err = kOpenImage_NullString;
    else
    {
        hfile = ::CreateFileA(filename, 
            GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

        if (INVALID_HANDLE_VALUE == hfile)
            err = kOpenImage_FileNotExist;
    }
    if (hfile != INVALID_HANDLE_VALUE)
    {
        LoadImage(hfile, err, m_picture, m_stream);
        ::CloseHandle(hfile);
    }

    if (perr != NULL)
        *perr = err;

    return m_picture != NULL;
}

bool GiImage::open(const wchar_t* filename, kOpenImageError* perr)
{
    clear();

    HANDLE hfile = INVALID_HANDLE_VALUE;
    kOpenImageError err = kOpenImage_OK;

    if (NULL == filename || 0 == *filename)
        err = kOpenImage_NullString;
    else
    {
        hfile = ::CreateFileW(filename, 
            GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

        if (INVALID_HANDLE_VALUE == hfile)
            err = kOpenImage_FileNotExist;
    }
    if (hfile != INVALID_HANDLE_VALUE)
    {
        LoadImage(hfile, err, m_picture, m_stream);
        ::CloseHandle(hfile);
    }

    if (perr != NULL)
        *perr = err;

    return m_picture != NULL;
}

long GiImage::getHmWidth() const
{
    OLE_XSIZE_HIMETRIC w = 0;
    if (m_picture != NULL)
        m_picture->get_Width(&w);
    return w;
}

long GiImage::getHmHeight() const
{
    OLE_YSIZE_HIMETRIC h = 0;
    if (m_picture != NULL)
        m_picture->get_Height(&h);
    return h;
}

HBITMAP GiImage::getBitmap() const
{
    OLE_HANDLE h = (OLE_HANDLE)NULL;
    if (m_picture != NULL)
        m_picture->get_Handle(&h);
    return HBITMAP(h);
}

bool GiImage::draw(GiGraphWin& graph, const Box2d& rectW, bool fast) const
{
    return graph.drawImage(getHmWidth(), getHmHeight(), getBitmap(), rectW, fast);
}
