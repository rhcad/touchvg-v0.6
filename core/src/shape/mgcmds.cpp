// mgcmds.cpp
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include <string.h>
#include <map>
#include "mgcmdselect.h"
#include "mgcmderase.h"
#include "mgdrawrect.h"
#include "mgdrawline.h"
#include "mgdrawlines.h"
#include "mgdrawsplines.h"
#include "mgdrawtriang.h"
#include <mgshapet.h>
#include "mggrid.h"

extern int g_newShapeID;

MgCommand* mgCreateCoreCommand(const char* name)
{
    typedef MgCommand* (*FCreate)();
    struct Cmd {
        const char* name;
        FCreate creator;
    };
    const Cmd cmds[] = {
        { MgCmdSelect::Name(), MgCmdSelect::Create },
        { MgCmdErase::Name(), MgCmdErase::Create },
        { MgCmdDrawRect::Name(), MgCmdDrawRect::Create },
        { MgCmdDrawSquare::Name(), MgCmdDrawSquare::Create },
        { MgCmdDrawEllipse::Name(), MgCmdDrawEllipse::Create },
        { MgCmdDrawCircle::Name(), MgCmdDrawCircle::Create },
        { MgCmdDrawLine::Name(), MgCmdDrawLine::Create },
        { MgCmdDrawFixedLine::Name(), MgCmdDrawFixedLine::Create },
        { MgCmdDrawPolygon::Name(), MgCmdDrawPolygon::Create },
        { MgCmdDrawQuadrangle::Name(), MgCmdDrawQuadrangle::Create },
        { MgCmdDrawLines::Name(), MgCmdDrawLines::Create },
        { MgCmdDrawFreeLines::Name(), MgCmdDrawFreeLines::Create },
        { MgCmdDrawSplines::Name(), MgCmdDrawSplines::Create },
        { MgCmdDrawTriangle::Name(), MgCmdDrawTriangle::Create },
        { MgCmdDrawGrid::Name(), MgCmdDrawGrid::Create },
        { MgCmdArc3P::Name(), MgCmdArc3P::Create },
        { MgCmdArcCSE::Name(), MgCmdArcCSE::Create },
        { MgCmdArcTan::Name(), MgCmdArcTan::Create },
    };
    
    for (unsigned i = 0; i < sizeof(cmds)/sizeof(cmds[0]); i++)
    {
        if (strcmp(cmds[i].name, name) == 0)
            return (cmds[i].creator)();
    }
    
    return NULL;
}

typedef std::pair<MgShapesLock::ShapesLocked, void*> ShapeObserver;
static std::vector<ShapeObserver>  s_shapeObservers;
static MgLockRW s_dynLock;

#ifdef _WIN32
void giSleep(int ms) { Sleep(ms); }
#else
#include <unistd.h>
void giSleep(int ms) { usleep(ms * 1000); }
#endif

// MgLockRW
//

MgLockRW::MgLockRW() : _editFlags(0)
{
    _counts[0] = _counts[1] = _counts[2] = 0;
}

bool MgLockRW::lock(bool forWrite, int timeout)
{
    bool ret = false;
    
    if (1 == giInterlockedIncrement(_counts)) {     // first locked
        giInterlockedIncrement(_counts + (forWrite ? 2 : 1));
        ret = true;
    }
    else {
        ret = !forWrite && 0 == _counts[2];         // for read and not locked for write
        for (int i = 0; i < timeout && !ret; i += 25) {
            giSleep(25);
            ret = forWrite ? (!_counts[1] && !_counts[2]) : !_counts[2];
        }
        if (ret) {
            giInterlockedIncrement(_counts + (forWrite ? 2 : 1));
        }
        else {
            giInterlockedDecrement(_counts);
        }
    }
    
    return ret;
}

long MgLockRW::unlock(bool forWrite)
{
    giInterlockedDecrement(_counts + (forWrite ? 2 : 1));
    return giInterlockedDecrement(_counts);
}

bool MgLockRW::firstLocked()
{
    return _counts[0] == 1;
}

bool MgLockRW::lockedForRead()
{
    return _counts[0] > 0;
}

bool MgLockRW::lockedForWrite()
{
    return _counts[2] > 0;
}

// MgShapesLock
//

MgShapesLock::MgShapesLock(MgShapes* sp, int flags, int timeout) : shapes(sp)
{
    bool forWrite = (flags != 0);
    m_mode = shapes && shapes->getLockData()->lock(forWrite, timeout) ? (forWrite ? 2 : 1) : 0;
    if (m_mode == 2 && flags == Unknown)
        m_mode |= 4;
    if (m_mode == 2 && shapes->getLockData()->firstLocked()) {
        shapes->getLockData()->setEditFlags(flags);
        for (std::vector<ShapeObserver>::iterator it = s_shapeObservers.begin();
             it != s_shapeObservers.end(); ++it) {
            (it->first)(shapes, it->second, true);
        }
    }
}

MgShapesLock::~MgShapesLock()
{
    bool ended = false;
    
    if (locked() && shapes) {
        ended = (0 == shapes->getLockData()->unlock((m_mode & 2) != 0));
    }
    if (m_mode == 2 && ended) {
        shapes->afterChanged();
        for (std::vector<ShapeObserver>::iterator it = s_shapeObservers.begin();
             it != s_shapeObservers.end(); ++it) {
            (it->first)(shapes, it->second, false);
        }
    }
}

void MgShapesLock::registerObserver(ShapesLocked func, void* obj)
{
    if (func) {
        unregisterObserver(func, obj);
        s_shapeObservers.push_back(ShapeObserver(func, obj));
    }
}

void MgShapesLock::unregisterObserver(ShapesLocked func, void* obj)
{
    for (std::vector<ShapeObserver>::iterator it = s_shapeObservers.begin();
         it != s_shapeObservers.end(); ++it) {
        if (it->first == func && it->second == obj) {
            s_shapeObservers.erase(it);
            break;
        }
    }
}

bool MgShapesLock::locked()
{
    return m_mode != 0;
}

bool MgShapesLock::lockedForRead(MgShapes* sp)
{
    return sp->getLockData()->lockedForRead();
}

bool MgShapesLock::lockedForWrite(MgShapes* sp)
{
    return sp->getLockData()->lockedForWrite();
}

// MgDynShapeLock
//

MgDynShapeLock::MgDynShapeLock(bool forWrite, int timeout)
{
    m_mode = s_dynLock.lock(forWrite, timeout) ? (forWrite ? 2 : 1) : 0;
}

MgDynShapeLock::~MgDynShapeLock()
{
    if (locked()) {
        s_dynLock.unlock(m_mode == 2);
    }
}

bool MgDynShapeLock::locked()
{
    return m_mode != 0;
}

bool MgDynShapeLock::lockedForRead()
{
    return s_dynLock.lockedForRead();
}

bool MgDynShapeLock::lockedForWrite()
{
    return s_dynLock.lockedForWrite();
}

// mgRegisterShapeCreator, mgCreateShape
//

static std::map<int, MgShape* (*)()>   s_shapeCreators;

static void registerCoreCreators()
{
    s_shapeCreators[MgShapeT<MgLine>::Type() & 0xFFFF] = MgShapeT<MgLine>::create;
    s_shapeCreators[MgShapeT<MgRect>::Type() & 0xFFFF] = MgShapeT<MgRect>::create;
    s_shapeCreators[MgShapeT<MgEllipse>::Type() & 0xFFFF] = MgShapeT<MgEllipse>::create;
    s_shapeCreators[MgShapeT<MgRoundRect>::Type() & 0xFFFF] = MgShapeT<MgRoundRect>::create;
    s_shapeCreators[MgShapeT<MgLines>::Type() & 0xFFFF] = MgShapeT<MgLines>::create;
    s_shapeCreators[MgShapeT<MgSplines>::Type() & 0xFFFF] = MgShapeT<MgSplines>::create;
    s_shapeCreators[MgShapeT<MgGrid>::Type() & 0xFFFF] = MgShapeT<MgGrid>::create;
    s_shapeCreators[MgShapeT<MgImageShape>::Type() & 0xFFFF] = MgShapeT<MgImageShape>::create;
    s_shapeCreators[MgShapeT<MgArc>::Type() & 0xFFFF] = MgShapeT<MgArc>::create;
}

void mgRegisterShapeCreator(int type, MgShape* (*factory)())
{
    if (s_shapeCreators.empty()) {
        registerCoreCreators();
    }
    type = type & 0xFFFF;
    if (type > 20) {
        if (factory) {
            s_shapeCreators[type] = factory;
        }
        else {
            s_shapeCreators.erase(type);
        }
    }
}

MgShape* mgCreateShape(int type)
{
    if (s_shapeCreators.empty())
        registerCoreCreators();
    
    std::map<int, MgShape* (*)()>::const_iterator it = s_shapeCreators.find(type & 0xFFFF);
    return (it != s_shapeCreators.end()) ? (it->second)() : NULL;
}

//! 添加一个容纳图像的矩形图形.
/*! \ingroup GEOM_SHAPE
    \param sender 指定目标视图
    \param name 图像的标识名称
    \param width 图像矩形的宽度，单位为点
    \param height 图像矩形的高度，单位为点
    \return 新图形对象，NULL表示失败
 */
MgShape* mgAddImageShape(const MgMotion* sender, const char* name, float width, float height)
{
    if (!name || width < 1 || height < 1)
        return NULL;
    
    Vector2d size(Vector2d(width, height) * sender->view->xform()->displayToWorld());
    while (fabsf(size.x) > 200.f || fabsf(size.y) > 200.f) {
        size *= 0.95f;
    }
    Box2d rect(sender->view->xform()->getWndRectW() + Vector2d(10.f, -10.f));
    rect = Box2d(rect.leftTop(), rect.leftTop() + size);
    rect *= sender->view->xform()->worldToModel();
    
    MgShapeT<MgImageShape> shape;
    MgImageShape* imagesp = (MgImageShape*)shape.shape();
    
    shape.context()->setLineStyle(kGiLineNull);         // 默认没有边框
    shape.context()->setFillColor(GiColor::White());    // 设为实填充，避免在中心无法点中
    imagesp->setName(name);
    imagesp->setRect2P(rect.leftTop(), rect.rightBottom());
    
    MgShapesLock locker(sender->view->shapes(), MgShapesLock::Add);
    if (sender->view->shapeWillAdded(&shape)) {
        MgShape* newsp = sender->view->shapes()->addShape(shape);
        sender->view->shapeAdded(newsp);
        
        g_newShapeID = newsp->getID();
        mgGetCommandManager()->setCommand(sender, "select");
        
        return newsp;
    }
    
    return NULL;
}

//! 得到当前图形的各种度量尺寸
int mgGetDimensions(MgView* view, float* vars, char* types, int count)
{
    for (int i = 0; i < count; i++) {
        vars[i] = 0;
        types[i] = 0;
    }
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    return cmd ? cmd->getDimensions(view, vars, types, count) : 0;
}

//! 返回选择包络框，显示坐标
void mgGetBoundingViewBox(Box2d& box, const MgMotion* sender)
{
    MgCommand* cmd = mgGetCommandManager()->getCommand();
    Box2d selbox;
    
    if (cmd && strcmp(cmd->getName(), MgCmdSelect::Name()) == 0) {
        MgCmdSelect* sel = (MgCmdSelect*)cmd;
        selbox = sel->getBoundingBox(sender);
    }
    
    box = selbox.isEmpty() ? Box2d(sender->pointM, 0, 0) : selbox;
    box *= sender->view->xform()->modelToDisplay();
    box.normalize();
}
