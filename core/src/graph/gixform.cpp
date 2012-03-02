// gixform.cpp: 实现坐标系管理类GiTransform
// Copyright (c) 2004-2012, Zhang Yungui
// License: GPL, https://github.com/rhcad/graph2d

#include "gixform.h"

_GEOM_BEGIN

//! 放缩范围
struct ZoomLimit
{
    double      dMinViewScale;  //!< 最小显示比例
    double      dMaxViewScale;  //!< 最大显示比例
    Box2d       rectLimitsW;    //!< 显示极限的世界坐标范围

    ZoomLimit()
    {
        dMinViewScale = 0.01;   // 最小显示比例为1%
        dMaxViewScale = 5.0;    // 最大显示比例为500%
        rectLimitsW.set(Point2d::kOrigin(), 2e5, 2e5);
    }
};

//! GiTransform的内部数据
struct GiTransform::Data : public ZoomLimit
{
    GiTransform*    xform;      //!< 拥有者
    long        cxWnd;          //!< 显示窗口宽度，像素
    long        cyWnd;          //!< 显示窗口高度，像素
    long        curDpiX;        //!< 显示设备每英寸的像素数X
    long        curDpiY;        //!< 显示设备每英寸的像素数Y
    Point2d     pntCenterW;     //!< 显示窗口中心的世界坐标，默认(0,0)
    double      viewScale;      //!< 显示比例，默认100%
    Matrix2d    matM2W;         //!< 模型坐标系到世界坐标系的变换矩阵，默认单位矩阵

    Matrix2d    matW2M;         //!< 世界坐标系到模型坐标系的变换矩阵
    Matrix2d    matD2W;         //!< 显示坐标系到世界坐标系的变换矩阵
    Matrix2d    matW2D;         //!< 世界坐标系到显示坐标系的变换矩阵
    Matrix2d    matD2M;         //!< 显示坐标系到模型坐标系的变换矩阵
    Matrix2d    matM2D;         //!< 模型坐标系到显示坐标系的变换矩阵
    double      dW2DX;          //!< 世界单位对应的像素数X
    double      dW2DY;          //!< 世界单位对应的像素数Y

    bool        zoomEnabled;    //!< 是否允许放缩
    Point2d     tmpCenterW;     //!< 当前放缩结果，不论是否允许放缩
    double      tmpViewScale;   //!< 当前放缩结果，不论是否允许放缩
    long        zoomTimes;      //!< 放缩结果改变的次数

    Data(GiTransform* p) : xform(p)
        , cxWnd(1), cyWnd(1), curDpiX(96), curDpiY(96), viewScale(1.0)
        , zoomEnabled(true), tmpViewScale(1.0), zoomTimes(0)
    {
        updateTransforms();
    }

    void updateTransforms()
    {
        dW2DX = viewScale * curDpiX / 25.4;
        dW2DY = viewScale * curDpiY / 25.4;

        double xc = cxWnd * 0.5;
        double yc = cyWnd * 0.5;
        matD2W.set(1.0 / dW2DX, 0, 0, -1.0 / dW2DY,
            pntCenterW.x - xc / dW2DX, pntCenterW.y + yc / dW2DY);
        matW2D.set(dW2DX, 0, 0, -dW2DY,
            xc - dW2DX * pntCenterW.x, yc + dW2DY * pntCenterW.y);

        matD2M = matD2W * matW2M;
        matM2D = matM2W * matW2D;
    }

    void coptFrom(const Data* pSrc)
    {
        cxWnd  = pSrc->cxWnd;
        cyWnd  = pSrc->cyWnd;
        curDpiX = pSrc->curDpiX;
        curDpiY = pSrc->curDpiY;
        pntCenterW = pSrc->pntCenterW;
        viewScale = pSrc->viewScale;
        matM2W = pSrc->matM2W;
        matW2M = pSrc->matW2M;
        matD2W = pSrc->matD2W;
        matW2D = pSrc->matW2D;
        matD2M = pSrc->matD2M;
        matM2D = pSrc->matM2D;
        dW2DX = pSrc->dW2DX;
        dW2DY = pSrc->dW2DY;
        dMinViewScale = pSrc->dMinViewScale;
        dMaxViewScale = pSrc->dMaxViewScale;
        rectLimitsW = pSrc->rectLimitsW;
        tmpCenterW = pSrc->tmpCenterW;
        tmpViewScale = pSrc->tmpViewScale;
    }

    void zoomChanged()
    {
        giInterlockedIncrement(&zoomTimes);
    }

    bool zoomNoAdjust(const Point2d& pnt, double scale, bool* changed = NULL)
    {
        bool bChanged = false;

        if (pnt != pntCenterW || !mgIsZero(scale - viewScale))
        {
            tmpCenterW = pnt;
            tmpViewScale = scale;
            bChanged = true;
            if (zoomEnabled)
            {
                pntCenterW = pnt;
                viewScale = scale;
                updateTransforms();
                zoomChanged();
            }
        }
        if (changed != NULL)
            *changed = bChanged;

        return bChanged;
    }

    bool zoomPanAdjust(Point2d &ptW, double dxPixel, double dyPixel) const;
};

GiTransform::GiTransform()
{
    m_data = new Data(this);
}

GiTransform::GiTransform(const GiTransform& src)
{
    m_data = new Data(this);
    m_data->coptFrom(src.m_data);
}

GiTransform::~GiTransform()
{
    delete m_data;
}

GiTransform& GiTransform::operator=(const GiTransform& src)
{
    if (this != &src)
        m_data->coptFrom(src.m_data);
    return *this;
}

long GiTransform::getDpiX() const { return m_data->curDpiX; }
long GiTransform::getDpiY() const { return m_data->curDpiY; }
long GiTransform::getWidth() const { return m_data->cxWnd; }
long GiTransform::getHeight() const { return m_data->cyWnd; }
Point2d GiTransform::getCenterW() const { return m_data->pntCenterW; }
double GiTransform::getViewScale() const { return m_data->viewScale; }
double GiTransform::getWorldToDisplayX() const { return m_data->dW2DX; }
double GiTransform::getWorldToDisplayY() const { return m_data->dW2DY; }
const Matrix2d& GiTransform::modelToWorld() const
    { return m_data->matM2W; }
const Matrix2d& GiTransform::worldToModel() const
    { return m_data->matW2M; }
const Matrix2d& GiTransform::displayToWorld() const
    { return m_data->matD2W; }
const Matrix2d& GiTransform::worldToDisplay() const
    { return m_data->matW2D; }
const Matrix2d& GiTransform::displayToModel() const
    { return m_data->matD2M; }
const Matrix2d& GiTransform::modelToDisplay() const
    { return m_data->matM2D; }
double GiTransform::getMinViewScale() const
    { return m_data->dMinViewScale; }
double GiTransform::getMaxViewScale() const
    { return m_data->dMaxViewScale; }
Box2d GiTransform::getWorldLimits() const
    { return m_data->rectLimitsW; }

long GiTransform::getZoomTimes() const
{
    return m_data->zoomTimes;
}

void GiTransform::setWndSize(int width, int height)
{
    if ((m_data->cxWnd != width || m_data->cyWnd != height)
        && width > 1 && height > 1)
    {
        m_data->cxWnd = width;
        m_data->cyWnd = height;
        m_data->updateTransforms();
        m_data->zoomChanged();
    }
}

void GiTransform::setModelTransform(const Matrix2d& mat)
{
    if (mat.isInvertible() && m_data->matM2W != mat)
    {
        m_data->matM2W = mat;
        m_data->matW2M = m_data->matM2W.inverse();
        m_data->matD2M = m_data->matD2W * m_data->matW2M;
        m_data->matM2D = m_data->matM2W * m_data->matW2D;
        m_data->zoomChanged();
    }
}

void GiTransform::setResolution(int dpiX, int dpiY)
{
    if (dpiX > 10 && dpiY > 10 
        && (m_data->curDpiX != dpiX || m_data->curDpiY != dpiY))
    {
        m_data->curDpiX = dpiX;
        m_data->curDpiY = dpiY;
        m_data->updateTransforms();
        m_data->zoomChanged();
    }
}

void GiTransform::setViewScaleRange(double minScale, double maxScale)
{
    if (minScale > maxScale)
        mgSwap(minScale, maxScale);

    minScale = mgMax(minScale, 1e-5);
    minScale = mgMin(minScale, 0.5);

    maxScale = mgMax(maxScale, 1.0);
    maxScale = mgMin(maxScale, 20.0);

    m_data->dMinViewScale = minScale;
    m_data->dMaxViewScale = maxScale;
}

Box2d GiTransform::setWorldLimits(const Box2d& rect)
{
    Box2d ret = m_data->rectLimitsW;
    m_data->rectLimitsW = rect;
    m_data->rectLimitsW.normalize();
    return ret;
}

bool GiTransform::enableZoom(bool enabled)
{
    bool bOld = m_data->zoomEnabled;
    m_data->zoomEnabled = enabled;
    return bOld;
}

void GiTransform::getZoomValue(Point2d& pntCenterW, double& viewScale) const
{
    pntCenterW = m_data->tmpCenterW;
    viewScale = m_data->tmpViewScale;
}

bool GiTransform::zoom(Point2d pntCenterW, double viewScale, bool* changed)
{
    viewScale = mgMax(viewScale, m_data->dMinViewScale);
    viewScale = mgMin(viewScale, m_data->dMaxViewScale);

    if (!m_data->rectLimitsW.isEmpty())
    {
        double halfw = m_data->cxWnd / m_data->dW2DX * 0.5;
        double halfh = m_data->cyWnd / m_data->dW2DY * 0.5;

        if (pntCenterW.x - halfw < m_data->rectLimitsW.xmin)
            pntCenterW.x += m_data->rectLimitsW.xmin - (pntCenterW.x - halfw);
        if (pntCenterW.x + halfw > m_data->rectLimitsW.xmax)
            pntCenterW.x += m_data->rectLimitsW.xmax - (pntCenterW.x + halfw);
        if (2 * halfw >= m_data->rectLimitsW.width())
            pntCenterW.x = m_data->rectLimitsW.center().x;

        if (pntCenterW.y - halfh < m_data->rectLimitsW.ymin)
            pntCenterW.y += m_data->rectLimitsW.ymin - (pntCenterW.y - halfh);
        if (pntCenterW.y + halfh > m_data->rectLimitsW.ymax)
            pntCenterW.y += m_data->rectLimitsW.ymax - (pntCenterW.y + halfh);
        if (2 * halfh >= m_data->rectLimitsW.height())
            pntCenterW.y = m_data->rectLimitsW.center().y;

        // 如果显示比例很小使得窗口超界，就放大显示
        if (2 * halfw > m_data->rectLimitsW.width()
            && 2 * halfh > m_data->rectLimitsW.height())
        {
            viewScale *= mgMin(2 * halfw / m_data->rectLimitsW.width(),
                2 * halfh / m_data->rectLimitsW.height());
            if (viewScale > m_data->dMaxViewScale)
                viewScale = m_data->dMaxViewScale;
        }
    }

    m_data->zoomNoAdjust(pntCenterW, viewScale, changed);

    return true;
}

static inline bool ScaleOutRange(double scale, const ZoomLimit* pData)
{
    return scale < pData->dMinViewScale - 1e-5
        || scale > pData->dMaxViewScale + 1e-5;
}

static void AdjustCenterW(Point2d &ptW, double halfw, double halfh, 
                          const Box2d& rectLimitsW)
{
    if (ptW.x - halfw < rectLimitsW.xmin)
        ptW.x += rectLimitsW.xmin - (ptW.x - halfw);
    if (ptW.x + halfw > rectLimitsW.xmax)
        ptW.x += rectLimitsW.xmax - (ptW.x + halfw);
    if (2 * halfw >= rectLimitsW.width())
        ptW.x = rectLimitsW.center().x;

    if (ptW.y - halfh < rectLimitsW.ymin)
        ptW.y += rectLimitsW.ymin - (ptW.y - halfh);
    if (ptW.y + halfh > rectLimitsW.ymax)
        ptW.y += rectLimitsW.ymax - (ptW.y + halfh);
    if (2 * halfh >= rectLimitsW.height())
        ptW.y = rectLimitsW.center().y;
}

bool GiTransform::zoomWnd(const POINT& pt1, const POINT& pt2, bool adjust)
{
    // 计算开窗矩形的中心和宽高
    Point2d ptCen ((pt2.x + pt1.x) * 0.5, (pt2.y + pt1.y) * 0.5);
    double w = fabs(static_cast<double>(pt2.x - pt1.x));
    double h = fabs(static_cast<double>(pt2.y - pt1.y));
    if (w < 4 || h < 4)
        return false;

    // 中心不变，扩大开窗矩形使得宽高比例和显示窗口相同
    if (h * m_data->cxWnd > w * m_data->cyWnd)
        w = h * m_data->cxWnd / m_data->cyWnd;
    else
        h = w * m_data->cyWnd / m_data->cxWnd;

    // 计算放缩前矩形中心的世界坐标
    Point2d ptW (ptCen * m_data->matD2W);

    // 计算新显示比例
    double scale = m_data->viewScale * m_data->cyWnd / h;
    if (!adjust && ScaleOutRange(scale, m_data))
        return false;
    scale = mgMax(scale, m_data->dMinViewScale);
    scale = mgMin(scale, m_data->dMaxViewScale);

    // 计算新显示比例下的显示窗口的世界坐标范围
    double halfw = m_data->cxWnd / (m_data->dW2DX / m_data->viewScale * scale) * 0.5;
    double halfh = m_data->cyWnd / (m_data->dW2DY / m_data->viewScale * scale) * 0.5;
    Box2d box (ptW, 2 * halfw, 2 * halfh);

    // 检查显示窗口的新坐标范围是否在极限范围内
    if (!m_data->rectLimitsW.isEmpty() && !m_data->rectLimitsW.isInside(box))
    {
        if (adjust)
            AdjustCenterW(ptW, halfw, halfh, m_data->rectLimitsW);
        else
            return false;
    }

    // 改变显示比例和位置
    return m_data->zoomNoAdjust(ptW, scale);
}

bool GiTransform::zoomTo(const Box2d& rectWorld, const RECT* rcTo, bool adjust)
{
    // 如果图形范围的宽或高接近于零，就返回
    if (rectWorld.isEmpty())
        return false;

    // 计算像素到毫米的比例
    const double d2mmX = m_data->viewScale / m_data->dW2DX;
    const double d2mmY = m_data->viewScale / m_data->dW2DY;

    // 计算目标窗口区域(毫米)
    double w = 0, h = 0;
    Point2d ptCen;

    if (rcTo != NULL)
    {
        w = fabs(static_cast<double>(rcTo->right - rcTo->left));
        h = fabs(static_cast<double>(rcTo->bottom - rcTo->top));
        ptCen.x = (rcTo->left + rcTo->right) * 0.5;
        ptCen.y = (rcTo->top + rcTo->bottom) * 0.5;
    }
    if (w < 4 || h < 4)
    {
        w = m_data->cxWnd;
        h = m_data->cyWnd;
        ptCen.set(m_data->cxWnd * 0.5, m_data->cyWnd * 0.5);
    }
    if (w < 4 || h < 4)
        return false;
    w *= d2mmX;
    h *= d2mmY;
    ptCen.scaleBy(d2mmX, d2mmY);

    // 计算新显示比例 (中心不变，缩小窗口区域使得宽高比例和图形范围相同)
    double scale;
    if (h * rectWorld.width() > w * rectWorld.height())
    {
        //h = w * rectWorld.height() / rectWorld.width();
        scale = w / rectWorld.width();
    }
    else
    {
        //w = h * rectWorld.width() / rectWorld.height();
        scale = h / rectWorld.height();
    }

    // 检查显示比例
    if (!adjust && ScaleOutRange(scale, m_data))
        return false;
    scale = mgMax(scale, m_data->dMinViewScale);
    scale = mgMin(scale, m_data->dMaxViewScale);

    // 计算在新显示比例下显示窗口中心的世界坐标
    Point2d ptW;
    ptW.x = rectWorld.center().x + (m_data->cxWnd * d2mmX * 0.5 - ptCen.x) / scale;
    ptW.y = rectWorld.center().y - (m_data->cyWnd * d2mmY * 0.5 - ptCen.y) / scale;

    // 检查新显示比例下显示窗口的世界坐标范围是否在极限范围内
    double halfw = m_data->cxWnd * d2mmX  / scale * 0.5;
    double halfh = m_data->cyWnd * d2mmY  / scale * 0.5;
    Box2d box (ptW, 2 * halfw, 2 * halfh);
    if (!m_data->rectLimitsW.isEmpty() && !m_data->rectLimitsW.isInside(box))
    {
        if (adjust)
            AdjustCenterW(ptW, halfw, halfh, m_data->rectLimitsW);
        else
            return false;
    }

    return m_data->zoomNoAdjust(ptW, scale);
}

bool GiTransform::zoomTo(const Point2d& pntWorld, const POINT* pxAt, bool adjust)
{
    Point2d pnt = pntWorld * m_data->matW2D;
    return zoomPan(
        (pxAt == NULL ? (m_data->cxWnd * 0.5) : pxAt->x) - pnt.x, 
        (pxAt == NULL ? (m_data->cyWnd * 0.5) : pxAt->y) - pnt.y, adjust);
}

bool GiTransform::zoomPan(double dxPixel, double dyPixel, bool adjust)
{
    // 计算新的显示窗口中心的世界坐标
    Vector2d vec (dxPixel, dyPixel);
    Point2d ptW (m_data->pntCenterW - vec * m_data->matD2W);

    // 检查新显示比例下显示窗口的世界坐标范围是否在极限范围内
    if (!m_data->rectLimitsW.isEmpty())
    {
        if (m_data->zoomPanAdjust(ptW, dxPixel, dyPixel) && !adjust)
            return false;
    }
    if (ptW == m_data->pntCenterW)
        return false;

    return m_data->zoomNoAdjust(ptW, m_data->viewScale);
}

bool GiTransform::Data::zoomPanAdjust(Point2d &ptW, 
                                      double dxPixel, double dyPixel) const
{
    bool bAdjusted = false;
    double halfw = cxWnd / dW2DX * 0.5;
    double halfh = cyWnd / dW2DY * 0.5;

    if (dxPixel > 0 && ptW.x - halfw < rectLimitsW.xmin)
    {
        bAdjusted = true;
        ptW.x += rectLimitsW.xmin - (ptW.x - halfw);
    }
    if (dxPixel < 0 && ptW.x + halfw > rectLimitsW.xmax)
    {
        bAdjusted = true;
        ptW.x += rectLimitsW.xmax - (ptW.x + halfw);
    }
    if (fabs(dxPixel) > 0 && 2 * halfw >= rectLimitsW.width())
    {
        bAdjusted = true;
        ptW.x = rectLimitsW.center().x;
    }
    if (dyPixel < 0 && ptW.y - halfh < rectLimitsW.ymin)
    {
        bAdjusted = true;
        ptW.y += rectLimitsW.ymin - (ptW.y - halfh);
    }
    if (dyPixel > 0 && ptW.y + halfh > rectLimitsW.ymax)
    {
        bAdjusted = true;
        ptW.y += rectLimitsW.ymax - (ptW.y + halfh);
    }
    if (fabs(dyPixel) > 0 && 2 * halfh >= rectLimitsW.height())
    {
        bAdjusted = true;
        ptW.y = rectLimitsW.center().y;
    }

    return bAdjusted;
}

bool GiTransform::zoomByFactor(double factor, const POINT* pxAt, bool adjust)
{
    double scale = m_data->viewScale;
    if (factor > 0)
        scale *= (1 + fabs(factor));
    else
        scale /= (1 + fabs(factor));

    if (adjust)
    {
        scale = mgMax(scale, m_data->dMinViewScale);
        scale = mgMin(scale, m_data->dMaxViewScale);
    }
    if (mgIsZero(scale - m_data->viewScale))
        return false;
    return zoomScale(scale, pxAt, adjust);
}

bool GiTransform::zoomScale(double viewScale, const POINT* pxAt, bool adjust)
{
    // 检查显示比例
    if (!adjust && ScaleOutRange(viewScale, m_data))
        return false;
    viewScale = mgMax(viewScale, m_data->dMinViewScale);
    viewScale = mgMin(viewScale, m_data->dMaxViewScale);

    // 得到放缩中心点的客户区坐标
    Point2d ptAt (m_data->cxWnd * 0.5,  m_data->cyWnd * 0.5);
    if (pxAt != NULL)
        ptAt.set(pxAt->x, pxAt->y);

    // 得到放缩中心点在放缩前的世界坐标
    Point2d ptAtW (ptAt * m_data->matD2W);

    // 计算新显示比例下显示窗口中心的世界坐标
    Point2d ptW;
    double dW2DX = m_data->dW2DX / m_data->viewScale * viewScale;
    double dW2DY = m_data->dW2DY / m_data->viewScale * viewScale;
    ptW.x = ptAtW.x + (m_data->cxWnd * 0.5 - ptAt.x) / dW2DX;
    ptW.y = ptAtW.y - (m_data->cyWnd * 0.5 - ptAt.y) / dW2DY;

    // 检查新显示比例下显示窗口的世界坐标范围是否在极限范围内
    double halfw = m_data->cxWnd / dW2DX * 0.5;
    double halfh = m_data->cyWnd / dW2DY * 0.5;
    Box2d box (ptW, 2 * halfw, 2 * halfh);
    if (!m_data->rectLimitsW.isEmpty() && !m_data->rectLimitsW.isInside(box))
    {
        if (adjust)
            AdjustCenterW(ptW, halfw, halfh, m_data->rectLimitsW);
        else
            return false;
    }

    return m_data->zoomNoAdjust(ptW, viewScale);
}

_GEOM_END
