#ifdef _WIN32
#include "giimagep.h"
#include "gidrgdip.h"

#define ULONG_PTR DWORD
#include <objbase.h>
#include <GdiPlus.h>

#define G Gdiplus

_GEOM_BEGIN

struct GiGdipImage::Impl
{
    G::Bitmap*  bmp;

    Impl() : bmp(NULL) {}
};

GiGdipImage::GiGdipImage(const wchar_t* filename)
{
    m_impl = new Impl();
    open(filename);
}

GiGdipImage::GiGdipImage(HBITMAP hbm)
{
    m_impl = new Impl();
    m_impl->bmp = new G::Bitmap(hbm, NULL);
}

GiGdipImage::~GiGdipImage()
{
    clear();
    delete m_impl;
}

void GiGdipImage::clear()
{
    if (m_impl->bmp != NULL)
    {
        delete m_impl->bmp;
        m_impl->bmp = NULL;
    }
}

bool GiGdipImage::open(const wchar_t* filename)
{
    clear();

    if (filename != NULL && *filename != 0)
    {
        m_impl->bmp = new G::Bitmap(filename);
    }

    return getWidth() > 0;
}

long GiGdipImage::getDpiX() const
{
    return m_impl->bmp ? mgRound(m_impl->bmp->GetHorizontalResolution()) : 0;
}

long GiGdipImage::getDpiY() const
{
    return m_impl->bmp ? mgRound(m_impl->bmp->GetVerticalResolution()) : 0;
}

long GiGdipImage::getWidth() const
{
    return m_impl->bmp ? (long)m_impl->bmp->GetWidth() : 0;
}

long GiGdipImage::getHeight() const
{
    return m_impl->bmp ? (long)m_impl->bmp->GetHeight() : 0;
}

long GiGdipImage::getHmWidth() const
{
    long ret = 0;

    if (m_impl->bmp != NULL)
    {
        UINT w = m_impl->bmp->GetWidth();
        G::REAL dpi = m_impl->bmp->GetHorizontalResolution();
        ret = mgRound(w / dpi * 2540);
    }

    return ret;
}

long GiGdipImage::getHmHeight() const
{
    long ret = 0;

    if (m_impl->bmp != NULL)
    {
        UINT h = m_impl->bmp->GetHeight();
        G::REAL dpi = m_impl->bmp->GetVerticalResolution();
        ret = mgRound(h / dpi * 2540);
    }

    return ret;
}

HBITMAP GiGdipImage::createBitmap(GiColor bkColor) const
{
    HBITMAP hBmp = NULL;

    if (m_impl->bmp != NULL)
    {
        G::Color colorBackground(bkColor.r, bkColor.g, bkColor.b);
        m_impl->bmp->GetHBITMAP(colorBackground, &hBmp);
    }

    return hBmp;
}

bool GiGdipImage::draw(GiGraphWin& graph, const Box2d& rectW, bool fast) const
{
    bool ret = false;

    if (getWidth() > 0)
    {
        if (graph.getGraphType() == 2)
        {
            GiGraphGdip* gs = (GiGraphGdip*)(&graph);
            ret = gs->drawGdipImage(
                getHmWidth(), getHmHeight(), m_impl->bmp, rectW, fast);
        }
        else
        {
            HBITMAP hBmp = createBitmap(graph.getBkColor());
            ret = graph.drawImage(getHmWidth(), getHmHeight(),
                hBmp, rectW, fast);
            ::DeleteObject(hBmp);
        }
    }

    return ret;
}

static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    G::ImageCodecInfo* pImageCodecInfo = NULL;

    G::GetImageEncodersSize(&num, &size);
    if (size == 0)
        return -1;  // Failure

    pImageCodecInfo = (G::ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL)
        return -1;  // Failure

    G::GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j)
    {
        if ( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

struct ExtEncoder
{
    const wchar_t* szFileExt;
    const wchar_t* encoderType;
};

static ExtEncoder s_extEncoders[] = {
    { L".bmp",  L"image/bmp" },
    { L".dib",  L"image/bmp" },
    { L".rle",  L"image/bmp" },

    { L".jpg",  L"image/jpeg" },
    { L".jpeg", L"image/jpeg" },
    { L".jpe",  L"image/jpeg" },
    { L".jfif", L"image/jpeg" },

    { L".tif",  L"image/tiff" },
    { L".tiff", L"image/tiff" },

    { L".gif",  L"image/gif" },
    { L".png",  L"image/png" },
    { L".wmf",  L"image/x-wmf" },
    { L".emf",  L"image/x-emf" },
    { L".icon", L"image/x-icon" },
};
const int s_extCount = sizeof(s_extEncoders)/sizeof(s_extEncoders[0]);

struct EncoderItem
{
    const wchar_t*  encoderType;
    CLSID           clsidEncoder;
    EncoderItem() { ZeroMemory(this, sizeof(EncoderItem)); }
};

static EncoderItem s_Encoders[s_extCount];

bool GiGdipImage::getEncoder(const wchar_t* filename, WCHAR format[20], CLSID& clsidEncoder)
{
    const wchar_t* ext = filename ? wcsrchr(filename, '.') : NULL;
    if (NULL == ext)
        return false;

    int i = s_extCount;
    while (--i >= 0)
    {
        if (lstrcmpiW(s_extEncoders[i].szFileExt, ext) == 0)
        {
            if (NULL == s_Encoders[i].encoderType)
            {
                GetEncoderClsid(s_extEncoders[i].encoderType, &s_Encoders[i].clsidEncoder);
                s_Encoders[i].encoderType = s_extEncoders[i].encoderType;
            }
            lstrcpyW(format, s_extEncoders[i].encoderType);
            clsidEncoder = s_Encoders[i].clsidEncoder;
            break;
        }
    }

    return i >= 0;
}

GiGdipImage* GiGdipImage::thumbnailImage(UInt32 maxWidth, UInt32 maxHeight)
{
    GiGdipImage* image = NULL;
    UINT width = getWidth();
    UINT height = getHeight();

    if (width > 0 && height > 0)
    {
        if (maxWidth < 1 || maxWidth > width)
            maxWidth = width;
        if (maxHeight < 1 || maxHeight > height)
            maxHeight = height;

        if (maxWidth < width || maxHeight < height)
        {
            if (width / height > maxWidth / maxHeight)
            {
                width = maxHeight * width / height;
                height = maxHeight;
            }
            else
            {
                height = maxWidth * height / width;
                width = maxWidth;
            }
        }

        G::Image* tbbmp = m_impl->bmp->GetThumbnailImage(width, height, NULL, NULL);
        if (tbbmp != NULL)
        {
            image = new GiGdipImage();
            image->m_impl->bmp = (G::Bitmap*)tbbmp;
        }
    }

    return image;
}

bool GiGdipImage::save(const wchar_t* filename, ULONG quality)
{
    bool ret = false;
    WCHAR format[20];
    CLSID clsidEncoder;
    
    if (this != NULL && getWidth() > 0
        && getEncoder(filename, format, clsidEncoder))
    {
        if (lstrcmpW(L"image/jpeg", format) == 0)
        {
            G::EncoderParameters encoderParameters;
            
            encoderParameters.Count = 1;
            encoderParameters.Parameter[0].Guid = G::EncoderQuality;
            encoderParameters.Parameter[0].Type = G::EncoderParameterValueTypeLong;
            encoderParameters.Parameter[0].NumberOfValues = 1;
            encoderParameters.Parameter[0].Value = &quality;
            
            ret = (G::Ok == m_impl->bmp->Save(filename, &clsidEncoder, &encoderParameters));
        }
        else
        {
            ret = (G::Ok == m_impl->bmp->Save(filename, &clsidEncoder, NULL));
        }
    }
    
    return ret;
}

_GEOM_END
#endif //_WIN32