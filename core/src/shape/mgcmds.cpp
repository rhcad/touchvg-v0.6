// mgcmds.cpp
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include <string.h>
#include "mgcmdselect.h"
#include "mgcmderase.h"
#include "mgdrawrect.h"
#include "mgdrawlines.h"
#include "mgdrawsplines.h"
#include <mgbasicsp.h>
#include <mgshapet.h>

typedef std::pair<MgShapesLock::ShapesLocked, void*> ShapeObserver;
static std::vector<ShapeObserver>  s_shapeObservers;
static long s_shapesLockCount = 0;

MgShapesLock::MgShapesLock(MgShapes* sp) : m_sp(sp)
{
    if (1 == giInterlockedIncrement(&s_shapesLockCount)) {
        for (std::vector<ShapeObserver>::iterator it = s_shapeObservers.begin();
             it != s_shapeObservers.end(); ++it) {
            (it->first)(m_sp, it->second, true);
        }
    }
}

MgShapesLock::~MgShapesLock()
{
    if (0 == giInterlockedDecrement(&s_shapesLockCount)) {
        for (std::vector<ShapeObserver>::iterator it = s_shapeObservers.begin();
             it != s_shapeObservers.end(); ++it) {
            (it->first)(m_sp, it->second, false);
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

MgCommand* mgCreateCommand(const char* name)
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
        { MgCmdDrawLines::Name(), MgCmdDrawLines::Create },
        { MgCmdDrawSplines::Name(), MgCmdDrawSplines::Create },
    };

    for (unsigned i = 0; i < sizeof(cmds)/sizeof(cmds[0]); i++)
    {
        if (strcmp(cmds[i].name, name) == 0)
            return (cmds[i].creator)();
    }

    return NULL;
}

typedef std::pair<UInt32, MgShape* (*)()> TypeToCreator;
static std::vector<TypeToCreator>   s_shapeCreators;

void mgRegisterShapeCreators()
{
    s_shapeCreators.push_back(TypeToCreator(MgShapeT<MgLine>::Type(), MgShapeT<MgLine>::create));
    s_shapeCreators.push_back(TypeToCreator(MgShapeT<MgRect>::Type(), MgShapeT<MgRect>::create));
    s_shapeCreators.push_back(TypeToCreator(MgShapeT<MgEllipse>::Type(), MgShapeT<MgEllipse>::create));
    s_shapeCreators.push_back(TypeToCreator(MgShapeT<MgRoundRect>::Type(), MgShapeT<MgRoundRect>::create));
    s_shapeCreators.push_back(TypeToCreator(MgShapeT<MgLines>::Type(), MgShapeT<MgLines>::create));
    s_shapeCreators.push_back(TypeToCreator(MgShapeT<MgSplines>::Type(), MgShapeT<MgSplines>::create));
}

MgShape* mgCreateShape(UInt32 type)
{
    if (s_shapeCreators.empty())
        mgRegisterShapeCreators();
    
    for (std::vector<TypeToCreator>::const_iterator it = s_shapeCreators.begin();
         it != s_shapeCreators.end(); ++it) {
        if (it->first % 10000 == type)
            return (it->second)();
    }
    
    return NULL;
}
