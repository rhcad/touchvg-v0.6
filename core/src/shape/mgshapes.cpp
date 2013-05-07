// mgshapes.cpp: 实现图形列表类 MgShapes
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include <mgshapedoc.h>
#include <map>
#include <vector>
#include <mgshapet.h>
#include <mgcomposite.h>
#include "mggrid.h"

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

MgShapesLock::MgShapesLock(MgShapeDoc* d, int flags, int timeout) : doc(d)
{
    bool forWrite = (flags != 0);
    _mode = d && d->getLockData()->lock(forWrite, timeout) ? (forWrite ? 2 : 1) : 0;
    if (_mode == 2 && flags == Unknown)
        _mode |= 4;
    if (_mode == 2 && doc->getLockData()->firstLocked()) {
        doc->getLockData()->setEditFlags(flags);
        for (std::vector<ShapeObserver>::iterator it = s_shapeObservers.begin();
             it != s_shapeObservers.end(); ++it) {
            (it->first)(doc, it->second, true);
        }
    }
}

MgShapesLock::~MgShapesLock()
{
    bool ended = false;
    
    if (locked() && doc) {
        ended = (0 == doc->getLockData()->unlock((_mode & 2) != 0));
    }
    if (_mode == 2 && ended) {
        doc->afterChanged();
        for (std::vector<ShapeObserver>::iterator it = s_shapeObservers.begin();
             it != s_shapeObservers.end(); ++it) {
            (it->first)(doc, it->second, false);
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
    return _mode != 0;
}

bool MgShapesLock::lockedForRead(MgShapeDoc* doc)
{
    return doc->getLockData()->lockedForRead();
}

bool MgShapesLock::lockedForWrite(MgShapeDoc* doc)
{
    return doc->getLockData()->lockedForWrite();
}

// MgDynShapeLock
//

MgDynShapeLock::MgDynShapeLock(bool forWrite, int timeout)
{
    _mode = s_dynLock.lock(forWrite, timeout) ? (forWrite ? 2 : 1) : 0;
}

MgDynShapeLock::~MgDynShapeLock()
{
    if (locked()) {
        s_dynLock.unlock(_mode == 2);
    }
}

bool MgDynShapeLock::locked()
{
    return _mode != 0;
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
    s_shapeCreators[MgShapeT<MgGroup>::Type() & 0xFFFF] = MgShapeT<MgGroup>::create;
    s_shapeCreators[MgShapeT<MgLine>::Type() & 0xFFFF] = MgShapeT<MgLine>::create;
    s_shapeCreators[MgShapeT<MgRect>::Type() & 0xFFFF] = MgShapeT<MgRect>::create;
    s_shapeCreators[MgShapeT<MgEllipse>::Type() & 0xFFFF] = MgShapeT<MgEllipse>::create;
    s_shapeCreators[MgShapeT<MgRoundRect>::Type() & 0xFFFF] = MgShapeT<MgRoundRect>::create;
    s_shapeCreators[MgShapeT<MgDiamond>::Type() & 0xFFFF] = MgShapeT<MgDiamond>::create;
    s_shapeCreators[MgShapeT<MgLines>::Type() & 0xFFFF] = MgShapeT<MgLines>::create;
    s_shapeCreators[MgShapeT<MgSplines>::Type() & 0xFFFF] = MgShapeT<MgSplines>::create;
    s_shapeCreators[MgShapeT<MgGrid>::Type() & 0xFFFF] = MgShapeT<MgGrid>::create;
    s_shapeCreators[MgShapeT<MgImageShape>::Type() & 0xFFFF] = MgShapeT<MgImageShape>::create;
    s_shapeCreators[MgShapeT<MgArc>::Type() & 0xFFFF] = MgShapeT<MgArc>::create;
    s_shapeCreators[MgShapeT<MgGrid>::Type() & 0xFFFF] = MgShapeT<MgGrid>::create;
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

#include <mgshapest.h>
#include <list>

MgShapes* mgCreateShapes(MgObject* owner, int index)
{
    return new MgShapesT<std::list<MgShape*> >(owner, owner ? index : -1);
}
