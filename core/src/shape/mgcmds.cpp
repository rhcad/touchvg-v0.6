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
#include <mgbasicsp.h>
#include <mgshapet.h>

MgCommand* mgCreateCoreCommand(const char* name)
{
    typedef MgCommand* (*FCreate)();
    struct Cmd {
        const char* name;
        FCreate creator;
    };
    const Cmd cmds[] = {
        { MgCommandSelect::Name(), MgCommandSelect::Create },
        { MgCommandErase::Name(), MgCommandErase::Create },
        { MgCmdDrawRect::Name(), MgCmdDrawRect::Create },
        { MgCmdDrawSquare::Name(), MgCmdDrawSquare::Create },
        { MgCmdDrawEllipse::Name(), MgCmdDrawEllipse::Create },
        { MgCmdDrawCircle::Name(), MgCmdDrawCircle::Create },
        { MgCmdDrawDiamond::Name(), MgCmdDrawDiamond::Create },
        { MgCmdDrawLine::Name(), MgCmdDrawLine::Create },
        { MgCmdDrawFixedLine::Name(), MgCmdDrawFixedLine::Create },
        { MgCmdDrawPolygon::Name(), MgCmdDrawPolygon::Create },
        { MgCmdDrawQuadrangle::Name(), MgCmdDrawQuadrangle::Create },
        { MgCmdDrawLines::Name(), MgCmdDrawLines::Create },
        { MgCmdDrawFreeLines::Name(), MgCmdDrawFreeLines::Create },
        { MgCmdDrawSplines::Name(), MgCmdDrawSplines::Create },
        { MgCmdDrawTriangle::Name(), MgCmdDrawTriangle::Create },
        { MgCmdParallelogram::Name(), MgCmdParallelogram::Create },
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

static std::map<UInt32, MgShape* (*)()>   s_shapeCreators;

static void registerCoreCreators()
{
    s_shapeCreators[MgShapeT<MgLine>::Type() % 10000] = MgShapeT<MgLine>::create;
    s_shapeCreators[MgShapeT<MgRect>::Type() % 10000] = MgShapeT<MgRect>::create;
    s_shapeCreators[MgShapeT<MgEllipse>::Type() % 10000] = MgShapeT<MgEllipse>::create;
    s_shapeCreators[MgShapeT<MgRoundRect>::Type() % 10000] = MgShapeT<MgRoundRect>::create;
    s_shapeCreators[MgShapeT<MgDiamond>::Type() % 10000] = MgShapeT<MgDiamond>::create;
    s_shapeCreators[MgShapeT<MgParallelogram>::Type() % 10000] = MgShapeT<MgParallelogram>::create;
    s_shapeCreators[MgShapeT<MgLines>::Type() % 10000] = MgShapeT<MgLines>::create;
    s_shapeCreators[MgShapeT<MgSplines>::Type() % 10000] = MgShapeT<MgSplines>::create;
}

void mgRegisterShapeCreator(UInt32 type, MgShape* (*factory)())
{
    if (s_shapeCreators.empty()) {
        registerCoreCreators();
    }
    type = type % 10000;
    if (type > 20) {
        if (factory) {
            s_shapeCreators[type] = factory;
        }
        else {
            s_shapeCreators.erase(type);
        }
    }
}

MgShape* mgCreateShape(UInt32 type)
{
    if (s_shapeCreators.empty())
        registerCoreCreators();
    
    std::map<UInt32, MgShape* (*)()>::const_iterator it = s_shapeCreators.find(type % 10000);
    return (it != s_shapeCreators.end()) ? (it->second)() : NULL;
}
