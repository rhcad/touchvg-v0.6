// gixform.cpp: 实现坐标系管理类GiTransform
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#include "gixform.h"

//! GiTransform的内部数据
struct GiTransformImpl
{
    long        cxWnd;          //!< 显示窗口宽度，像素
    long        cyWnd;          //!< 显示窗口高度，像素
    long        dpiX;           //!< 显示设备每英寸的像素数X
    long        dpiY;           //!< 显示设备每英寸的像素数Y
    bool        ydown;          //!< 显示设备的+Y方向是否为向下
    Point2d     centerW;        //!< 显示窗口中心的世界坐标，默认(0,0)
    double      viewScale;      //!< 显示比例，默认100%
    Matrix2d    matM2W;         //!< 模型坐标系到世界坐标系的变换矩阵，默认单位矩阵

    Matrix2d    matW2M;         //!< 世界坐标系到模型坐标系的变换矩阵
    Matrix2d    matD2W;         //!< 显示坐标系到世界坐标系的变换矩阵
    Matrix2d    matW2D;         //!< 世界坐标系到显示坐标系的变换矩阵
    Matrix2d    matD2M;         //!< 显示坐标系到模型坐标系的变换矩阵
    Matrix2d    matM2D;         //!< 模型坐标系到显示坐标系的变换矩阵
    double      w2dx;           //!< 世界单位对应的像素数X
    double      w2dy;           //!< 世界单位对应的像素数Y

    bool        zoomEnabled;    //!< 是否允许放缩
    Point2d     tmpCenterW;     //!< 当前放缩结果，不论是否允许放缩
    double      tmpViewScale;   //!< 当前放缩结果，不论是否允许放缩
    long        zoomTimes;      //!< 放缩结果改变的次数

    double      minViewScale;   //!< 最小显示比例
    double      maxViewScale;   //!< 最大显示比例
    Box2d       rectLimitsW;    //!< 显示极限的世界坐标范围

    GiTransformImpl(bool _ydown)
        : cxWnd(1), cyWnd(1), dpiX(96), dpiY(96), ydown(_ydown), viewScale(1.0)
        , zoomEnabled(true), tmpViewScale(1.0), zoomTimes(0)
    {
        minViewScale = 0.01;   // 最小显示比例为1%
        maxViewScale = 5.0;    // 最大显示比例为500%
        rectLimitsW.set(Point2d::kOrigin(), 2e5, 2e5);

        updateTransforms();
    }

    void updateTransforms()
    {
        w2dx = viewScale * dpiX / 25.4;
        w2dy = viewScale * dpiY / 25.4;

        double wdy = ydown ? -w2dy : w2dy;
        double xc = cxWnd * 0.5;
        double yc = cyWnd * 0.5;

        matD2W.set(1.0 / w2dx, 0, 0, 1.0 / wdy,
            centerW.x - xc / w2dx, centerW.y - yc / wdy);
        matW2D.set(w2dx, 0, 0, wdy,
            xc - w2dx * centerW.x, yc - wdy * centerW.y);

        matD2M = matD2W * matW2M;
        matM2D = matM2W * matW2D;
    }

    void coptFrom(const GiTransformImpl* src)
    {
        cxWnd  = src->cxWnd;
        cyWnd  = src->cyWnd;
        dpiX = src->dpiX;
        dpiY = src->dpiY;
        ydown = src->ydown;
        centerW = src->centerW;
        viewScale = src->viewScale;
        matM2W = src->matM2W;
        matW2M = src->matW2M;
        matD2W = src->matD2W;
        matW2D = src->matW2D;
        matD2M = src->matD2M;
        matM2D = src->matM2D;
        w2dx = src->w2dx;
        w2dy = src->w2dy;
        minViewScale = src->minViewScale;
        maxViewScale = src->maxViewScale;
        rectLimitsW = src->rectLimitsW;
        tmpCenterW = src->tmpCenterW;
        tmpViewScale = src->tmpViewScale;
    }

    void zoomChanged()
    {
        giInterlockedIncrement(&zoomTimes);
    }

    bool zoomNoAdjust(const Point2d& pnt, double scale, bool* changed = NULL)
    {
        bool bChanged = false;

        if (pnt != centerW || !mgIsZero(scale - viewScale))
        {
            tmpCenterW = pnt;
            tmpViewScale = scale;
            bChanged = true;
            if (zoomEnabled)
            {
                centerW = pnt;
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

GiTransform::GiTransform(bool ydown)
{
    m_impl = new GiTransformImpl(ydown);
}

GiTransform::GiTransform(const GiTransform& src)
{
    m_impl = new GiTransformImpl(true);
    m_impl->coptFrom(src.m_impl);
}

GiTransform::~GiTransform()
{
    delete m_impl;
}

GiTransform& GiTransform::operator=(const GiTransform& src)
{
    if (this != &src)
        m_impl->coptFrom(src.m_impl);
    return *this;
}

long GiTransform::getDpiX() const { return m_impl->dpiX; }
long GiTransform::getDpiY() const { return m_impl->dpiY; }
long GiTransform::getWidth() const { return m_impl->cxWnd; }
long GiTransform::getHeight() const { return m_impl->cyWnd; }
Point2d GiTransform::getCenterW() const { return m_impl->centerW; }
double GiTransform::getViewScale() const { return m_impl->viewScale; }
double GiTransform::getWorldToDisplayX() const { return m_impl->w2dx; }
double GiTransform::getWorldToDisplayY() const { return m_impl->w2dy; }
double GiTransform::displayToModel(double px) const
    { return (Vector2d(0,px) * m_impl->matD2M).length(); }
const Matrix2d& GiTransform::modelToWorld() const
    { return m_impl->matM2W; }
const Matrix2d& GiTransform::worldToModel() const
    { return m_impl->matW2M; }
const Matrix2d& GiTransform::displayToWorld() const
    { return m_impl->matD2W; }
const Matrix2d& GiTransform::worldToDisplay() const
    { return m_impl->matW2D; }
const Matrix2d& GiTransform::displayToModel() const
    { return m_impl->matD2M; }
const Matrix2d& GiTransform::modelToDisplay() const
    { return m_impl->matM2D; }
double GiTransform::getMinViewScale() const
    { return m_impl->minViewScale; }
double GiTransform::getMaxViewScale() const
    { return m_impl->maxViewScale; }
Box2d GiTransform::getWorldLimits() const
    { return m_impl->rectLimitsW; }

long GiTransform::getZoomTimes() const
{
    return m_impl->zoomTimes;
}

void GiTransform::setWndSize(int width, int height)
{
    if ((m_impl->cxWnd != width || m_impl->cyWnd != height)
        && width > 1 && height > 1)
    {
        m_impl->cxWnd = width;
        m_impl->cyWnd = height;
        m_impl->updateTransforms();
        m_impl->zoomChanged();
    }
}

void GiTransform::setModelTransform(const Matrix2d& mat)
{
    if (mat.isInvertible() && m_impl->matM2W != mat)
    {
        m_impl->matM2W = mat;
        m_impl->matW2M = m_impl->matM2W.inverse();
        m_impl->matD2M = m_impl->matD2W * m_impl->matW2M;
        m_impl->matM2D = m_impl->matM2W * m_impl->matW2D;
        m_impl->zoomChanged();
    }
}

void GiTransform::setResolution(int dpiX, int dpiY)
{
    if (dpiY < 1) dpiY = dpiX;
    if (dpiX > 10 && dpiY > 10 
        && (m_impl->dpiX != dpiX || m_impl->dpiY != dpiY))
    {
        m_impl->dpiX = dpiX;
        m_impl->dpiY = dpiY;
        m_impl->updateTransforms();
        m_impl->zoomChanged();
    }
}

void GiTransform::setViewScaleRange(double minScale, double maxScale)
{
    if (minScale > maxScale)
        mgSwap(minScale, maxScale);

    minScale = mgMax(minScale, 1e-5);
    minScale = mgMin(minScale, 0.5);

    maxScale = mgMax(maxScale, 1.0);
    maxScale = mgMin(maxScale, 50.0);

    m_impl->minViewScale = minScale;
    m_impl->maxViewScale = maxScale;
}

Box2d GiTransform::setWorldLimits(const Box2d& rect)
{
    Box2d ret = m_impl->rectLimitsW;
    m_impl->rectLimitsW = rect;
    m_impl->rectLimitsW.normalize();
    return ret;
}

bool GiTransform::enableZoom(bool enabled)
{
    bool bOld = m_impl->zoomEnabled;
    m_impl->zoomEnabled = enabled;
    return bOld;
}

void GiTransform::getZoomValue(Point2d& centerW, double& viewScale) const
{
    centerW = m_impl->tmpCenterW;
    viewScale = m_impl->tmpViewScale;
}

bool GiTransform::zoom(Point2d centerW, double viewScale, bool* changed)
{
    viewScale = mgMax(viewScale, m_impl->minViewScale);
    viewScale = mgMin(viewScale, m_impl->maxViewScale);

    if (!m_impl->rectLimitsW.isEmpty())
    {
        double halfw = m_impl->cxWnd / m_impl->w2dx * 0.5;
        double halfh = m_impl->cyWnd / m_impl->w2dy * 0.5;

        if (centerW.x - halfw < m_impl->rectLimitsW.xmin)
            centerW.x += m_impl->rectLimitsW.xmin - (centerW.x - halfw);
        if (centerW.x + halfw > m_impl->rectLimitsW.xmax)
            centerW.x += m_impl->rectLimitsW.xmax - (centerW.x + halfw);
        if (2 * halfw >= m_impl->rectLimitsW.width())
            centerW.x = m_impl->rectLimitsW.center().x;

        if (centerW.y - halfh < m_impl->rectLimitsW.ymin)
            centerW.y += m_impl->rectLimitsW.ymin - (centerW.y - halfh);
        if (centerW.y + halfh > m_impl->rectLimitsW.ymax)
            centerW.y += m_impl->rectLimitsW.ymax - (centerW.y + halfh);
        if (2 * halfh >= m_impl->rectLimitsW.height())
            centerW.y = m_impl->rectLimitsW.center().y;

        // 如果显示比例很小使得窗口超界，就放大显示
        if (2 * halfw > m_impl->rectLimitsW.width()
            && 2 * halfh > m_impl->rectLimitsW.height())
        {
            viewScale *= mgMin(2 * halfw / m_impl->rectLimitsW.width(),
                2 * halfh / m_impl->rectLimitsW.height());
            if (viewScale > m_impl->maxViewScale)
                viewScale = m_impl->maxViewScale;
        }
    }

    m_impl->zoomNoAdjust(centerW, viewScale, changed);

    return true;
}

static inline bool ScaleOutRange(double scale, const GiTransformImpl* impl)
{
    return scale < impl->minViewScale - 1e-5
        || scale > impl->maxViewScale + 1e-5;
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
    if (h * m_impl->cxWnd > w * m_impl->cyWnd)
        w = h * m_impl->cxWnd / m_impl->cyWnd;
    else
        h = w * m_impl->cyWnd / m_impl->cxWnd;

    // 计算放缩前矩形中心的世界坐标
    Point2d ptW (ptCen * m_impl->matD2W);

    // 计算新显示比例
    double scale = m_impl->viewScale * m_impl->cyWnd / h;
    if (!adjust && ScaleOutRange(scale, m_impl))
        return false;
    scale = mgMax(scale, m_impl->minViewScale);
    scale = mgMin(scale, m_impl->maxViewScale);

    // 计算新显示比例下的显示窗口的世界坐标范围
    double halfw = m_impl->cxWnd / (m_impl->w2dx / m_impl->viewScale * scale) * 0.5;
    double halfh = m_impl->cyWnd / (m_impl->w2dy / m_impl->viewScale * scale) * 0.5;
    Box2d box (ptW, 2 * halfw, 2 * halfh);

    // 检查显示窗口的新坐标范围是否在极限范围内
    if (!m_impl->rectLimitsW.isEmpty() && !m_impl->rectLimitsW.contains(box))
    {
        if (adjust)
            AdjustCenterW(ptW, halfw, halfh, m_impl->rectLimitsW);
        else
            return false;
    }

    // 改变显示比例和位置
    return m_impl->zoomNoAdjust(ptW, scale);
}

bool GiTransform::zoomTo(const Box2d& rectWorld, const RECT* rcTo, bool adjust)
{
    // 如果图形范围的宽或高接近于零，就返回
    if (rectWorld.isEmpty())
        return false;

    // 计算像素到毫米的比例
    const double d2mmX = m_impl->viewScale / m_impl->w2dx;
    const double d2mmY = m_impl->viewScale / m_impl->w2dy;

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
        w = m_impl->cxWnd;
        h = m_impl->cyWnd;
        ptCen.set(m_impl->cxWnd * 0.5, m_impl->cyWnd * 0.5);
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
    if (!adjust && ScaleOutRange(scale, m_impl))
        return false;
    scale = mgMax(scale, m_impl->minViewScale);
    scale = mgMin(scale, m_impl->maxViewScale);

    // 计算在新显示比例下显示窗口中心的世界坐标
    Point2d ptW;
    ptW.x = rectWorld.center().x + (m_impl->cxWnd * d2mmX * 0.5 - ptCen.x) / scale;
    ptW.y = rectWorld.center().y - (m_impl->cyWnd * d2mmY * 0.5 - ptCen.y) / scale;

    // 检查新显示比例下显示窗口的世界坐标范围是否在极限范围内
    double halfw = m_impl->cxWnd * d2mmX  / scale * 0.5;
    double halfh = m_impl->cyWnd * d2mmY  / scale * 0.5;
    Box2d box (ptW, 2 * halfw, 2 * halfh);
    if (!m_impl->rectLimitsW.isEmpty() && !m_impl->rectLimitsW.contains(box))
    {
        if (adjust)
            AdjustCenterW(ptW, halfw, halfh, m_impl->rectLimitsW);
        else
            return false;
    }

    return m_impl->zoomNoAdjust(ptW, scale);
}

bool GiTransform::zoomTo(const Point2d& pntWorld, const POINT* pxAt, bool adjust)
{
    Point2d pnt = pntWorld * m_impl->matW2D;
    return zoomPan(
        (pxAt == NULL ? (m_impl->cxWnd * 0.5) : pxAt->x) - pnt.x, 
        (pxAt == NULL ? (m_impl->cyWnd * 0.5) : pxAt->y) - pnt.y, adjust);
}

bool GiTransform::zoomPan(double dxPixel, double dyPixel, bool adjust)
{
    // 计算新的显示窗口中心的世界坐标
    Vector2d vec (dxPixel, dyPixel);
    Point2d ptW (m_impl->centerW - vec * m_impl->matD2W);

    // 检查新显示比例下显示窗口的世界坐标范围是否在极限范围内
    if (!m_impl->rectLimitsW.isEmpty())
    {
        if (m_impl->zoomPanAdjust(ptW, dxPixel, dyPixel) && !adjust)
            return false;
    }
    if (ptW == m_impl->centerW)
        return false;

    return m_impl->zoomNoAdjust(ptW, m_impl->viewScale);
}

bool GiTransformImpl::zoomPanAdjust(Point2d &ptW, double dxPixel, double dyPixel) const
{
    bool bAdjusted = false;
    double halfw = cxWnd / w2dx * 0.5;
    double halfh = cyWnd / w2dy * 0.5;

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
    double scale = m_impl->viewScale;
    if (factor > 0)
        scale *= (1 + fabs(factor));
    else
        scale /= (1 + fabs(factor));

    if (adjust)
    {
        scale = mgMax(scale, m_impl->minViewScale);
        scale = mgMin(scale, m_impl->maxViewScale);
    }
    if (mgIsZero(scale - m_impl->viewScale))
        return false;
    return zoomScale(scale, pxAt, adjust);
}

bool GiTransform::zoomScale(double viewScale, const POINT* pxAt, bool adjust)
{
    // 检查显示比例
    if (!adjust && ScaleOutRange(viewScale, m_impl))
        return false;
    viewScale = mgMax(viewScale, m_impl->minViewScale);
    viewScale = mgMin(viewScale, m_impl->maxViewScale);

    // 得到放缩中心点的客户区坐标
    Point2d ptAt (m_impl->cxWnd * 0.5,  m_impl->cyWnd * 0.5);
    if (pxAt != NULL)
        ptAt.set(pxAt->x, pxAt->y);

    // 得到放缩中心点在放缩前的世界坐标
    Point2d ptAtW (ptAt * m_impl->matD2W);

    // 计算新显示比例下显示窗口中心的世界坐标
    Point2d ptW;
    double w2dx = m_impl->w2dx / m_impl->viewScale * viewScale;
    double w2dy = m_impl->w2dy / m_impl->viewScale * viewScale;
    ptW.x = ptAtW.x + (m_impl->cxWnd * 0.5 - ptAt.x) / w2dx;
    ptW.y = ptAtW.y - (m_impl->cyWnd * 0.5 - ptAt.y) / w2dy;

    // 检查新显示比例下显示窗口的世界坐标范围是否在极限范围内
    double halfw = m_impl->cxWnd / w2dx * 0.5;
    double halfh = m_impl->cyWnd / w2dy * 0.5;
    Box2d box (ptW, 2 * halfw, 2 * halfh);
    if (!m_impl->rectLimitsW.isEmpty() && !m_impl->rectLimitsW.contains(box))
    {
        if (adjust)
            AdjustCenterW(ptW, halfw, halfh, m_impl->rectLimitsW);
        else
            return false;
    }

    return m_impl->zoomNoAdjust(ptW, viewScale);
}
