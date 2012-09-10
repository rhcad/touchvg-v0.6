// mgcmdmgr.cpp: 实现命令管理器类
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgcmdmgr.h"
#include "mgcmdselect.h"
#include <mgbasicsp.h>

MgCommand* mgCreateCoreCommand(const char* name);
float mgDisplayMmToModel(float mm, const MgMotion* sender);

typedef std::map<std::string, MgCommand* (*)()> Factories;
static Factories    _factories;
static MgCmdManagerImpl* s_manager = NULL;
static MgCmdManagerImpl s_tmpmgr(true);

void mgRegisterCommand(const char* name, MgCommand* (*factory)())
{
    if (!factory) {
        _factories.erase(name);
    }
    else {
        _factories[name] = factory;
    }
}

MgCommandManager* mgGetCommandManager()
{
    return s_manager ? s_manager : &s_tmpmgr;
}

MgCmdManagerImpl::MgCmdManagerImpl(bool tmpobj)
{
    if (!s_manager && !tmpobj)
        s_manager = this;
}

MgCmdManagerImpl::~MgCmdManagerImpl()
{
    unloadCommands();
    if (s_manager == this)
        s_manager = NULL;
}

void MgCmdManagerImpl::unloadCommands()
{
    for (CMDS::iterator it = _cmds.begin(); it != _cmds.end(); ++it)
        it->second->release();
    _cmds.clear();
    _cmdname = "";
}

const char* MgCmdManagerImpl::getCommandName()
{
    return _cmdname.c_str();
}

MgCommand* MgCmdManagerImpl::getCommand()
{
    CMDS::iterator it = _cmds.find(_cmdname);
    return it != _cmds.end() ? it->second : NULL;
}

bool MgCmdManagerImpl::setCommand(const MgMotion* sender, const char* name)
{
    cancel(sender);

    CMDS::iterator it = _cmds.find(name);
    if (it == _cmds.end())
    {
        MgCommand* cmd = NULL;
        Factories::iterator itf = _factories.find(name);
        
        if (itf != _factories.end()) {
            cmd = itf->second ? (itf->second)() : NULL;
        }
        if (!cmd) {
            cmd = mgCreateCoreCommand(name);
        }
        if (cmd) {
            _cmds[name] = cmd;
            it = _cmds.find(name);
        }
    }
    
    bool ret = (it != _cmds.end() && it->second->initialize(sender));
    _cmdname = ret ? name : "";     // change it at end of initialization

    return ret;
}

bool MgCmdManagerImpl::cancel(const MgMotion* sender)
{
    CMDS::iterator it = _cmds.find(_cmdname);
    if (it != _cmds.end()) {
        return it->second->cancel(sender);
    }
    return false;
}

UInt32 MgCmdManagerImpl::getSelection(MgView* view, UInt32 count, MgShape** shapes, bool forChange)
{
    if (_cmdname == MgCommandSelect::Name() && view) {
        MgCommandSelect* sel = (MgCommandSelect*)getCommand();
        return sel ? sel->getSelection(view, count, shapes, forChange) : 0;
    }
    return 0;
}

bool MgCmdManagerImpl::dynamicChangeEnded(MgView* view, bool apply)
{
    bool changed = false;
    if (_cmdname == MgCommandSelect::Name() && view) {
        MgCommandSelect* sel = (MgCommandSelect*)getCommand();
        changed = sel && sel->dynamicChangeEnded(view, apply);
    }
    return changed;
}

MgSelection* MgCmdManagerImpl::getSelection(MgView* view)
{
    return view && _cmdname == MgCommandSelect::Name()
        ? (MgCommandSelect*)getCommand() : NULL;
}

MgSnap* MgCmdManagerImpl::getSnap()
{
    return this;
}

typedef struct {
    Point2d pt;
    Point2d base;
    float dist;
    int type;
} SnapItem;

static int snapHV(const Point2d& basePt, Point2d& newPt, SnapItem arr[3])
{
    int type = -1;
    float diff = arr[1].dist;
    
    diff = arr[1].dist - fabs(newPt.x - basePt.x);
    if (diff > _MGZERO || (diff > - _MGZERO
                           && fabs(newPt.y - basePt.y) < fabs(newPt.y - arr[1].base.y))) {
        arr[1].dist = (float)fabs(newPt.x - basePt.x);
        arr[1].base = basePt;
        newPt.x = basePt.x;
        arr[1].pt = newPt;
        type = 1;
    }
    diff = arr[2].dist - fabs(newPt.y - basePt.y);
    if (diff > _MGZERO || (diff > - _MGZERO
                     && fabs(newPt.x - basePt.x) < fabs(newPt.x - arr[2].base.x))) {
        arr[2].dist = (float)fabs(newPt.y - basePt.y);
        arr[2].base = basePt;
        newPt.y = basePt.y;
        arr[2].pt = newPt;
        type = 2;
    }
    
    return type;
}

static void snapPoints(const MgMotion* sender, MgShape* shape, SnapItem arr[3], Point2d* matchpt)
{
    Box2d snapbox(sender->pointM, 2 * arr[0].dist, 0);
    GiTransform* xf = sender->view->xform();
    Box2d wndbox(Box2d(0, 0, xf->getWidth(), xf->getHeight()) * xf->displayToModel());
    void* it = NULL;
    
    for (MgShape* sp = sender->view->shapes()->getFirstShape(it);
         sp; sp = sender->view->shapes()->getNextShape(it)) {
        if (shape && shape->getID() == sp->getID())
            continue;
        bool allOnBox = !matchpt && sp->shape()->getExtent().isIntersect(snapbox);
        if (allOnBox || sp->shape()->getExtent().isIntersect(wndbox)) {
            UInt32 n = sp->shape()->getHandleCount();
            bool curve = sp->shape()->isKindOf(MgSplines::Type());
            
            for (UInt32 i = 0; i < n; i++) {
                if (curve && i > 0 && i + 1 < n)
                    continue;
                Point2d pnt(sp->shape()->getHandlePoint(i));
                if (allOnBox) {
                    float dist = pnt.distanceTo(sender->pointM);
                    if (arr[0].dist > dist) {
                        arr[0].dist = dist;
                        arr[0].pt = pnt;
                        arr[0].type = 5;
                    }
                }
                if (wndbox.contains(pnt)) {
                    Point2d newPt (sender->pointM);
                    int type = snapHV(pnt, newPt, arr);
                    if (type > 0 && type <= 2) {
                        arr[type].type = type + 2;
                    }
                }
                int d = matchpt && shape ? (int)shape->shape()->getHandleCount() - 1 : -1;
                for (; d >= 0; d--) {
                    Point2d ptd (shape->shape()->getHandlePoint(d));
                    float dist = pnt.distanceTo(ptd);
                    if (arr[0].dist > dist) {
                        arr[0].dist = dist;
                        arr[0].pt = pnt;
                        arr[0].type = 5;
                        *matchpt = sender->pointM + (pnt - ptd);
                    }
                }
            }
        }
    }
    sender->view->shapes()->freeIterator(it);
}

Point2d MgCmdManagerImpl::snapPoint(const MgMotion* sender, MgShape* shape, int hotHandle)
{
    if (shape && hotHandle >= shape->shape()->getHandleCount()) {
        hotHandle = -1;
    }
    _ptSnap = sender->pointM;
    
    GiTransform* xf = sender->view->xform();
    SnapItem arr[3] = {
        { _ptSnap, _ptSnap, xf->displayToModel(5, true), -1 },   // XY
        { _ptSnap, _ptSnap, xf->displayToModel(5, true), -1 },   // X,Vert
        { _ptSnap, _ptSnap, xf->displayToModel(5, true), -1 },   // Y,Horz
    };
    
    if (shape && hotHandle > 0) {
        Point2d pt (sender->pointM);
        int type = snapHV(shape->shape()->getPoint(hotHandle - 1), pt, arr);
        if (type > 0 && type <= 2) {
            arr[type].type = type;
        }
    }
    Point2d pnt(-1e10f, -1e10f);
    bool matchpt = shape && shape->getParent() && hotHandle < 0;
    
    snapPoints(sender, shape, arr, matchpt ? &pnt : NULL);
    
    if (arr[0].type >= 0) {
        _ptSnap = arr[0].pt;
        _snapType[0] = arr[0].type;
    }
    else {
        _snapType[0] = arr[1].type;
        if (arr[1].type > 0) {
            _ptSnap.x = arr[1].pt.x;
            _snapBase[0] = arr[1].base;
        }
        _snapType[1] = arr[2].type;
        if (arr[2].type > 0) {
            _ptSnap.y = arr[1].pt.y;
            _snapBase[1] = arr[2].base;
        }
    }
    
    return matchpt && pnt.x > -1e8f ? pnt : _ptSnap;
}

int MgCmdManagerImpl::getSnappedType()
{
    return _snapType[0] >= 5 ? _snapType[0] : -1;
}

bool MgCmdManagerImpl::draw(const MgMotion* sender, GiGraphics* gs)
{
    bool ret = false;
    
    if (sender->dragging) {
        if (_snapType[0] >= 5) {
            GiContext ctx(-2, GiColor(128, 128, 128, 200), kGiLineDash);
            ret = gs->drawEllipse(&ctx, _ptSnap, gs->xf().displayToModel(8.f, true));
        }
        else {
            GiContext ctx(0, GiColor(128, 128, 128, 200), kGiLineDash);
            if (_snapType[0] == 3) {
                ret = gs->drawLine(&ctx, _snapBase[0], _ptSnap);
            }
            if (_snapType[1] == 4) {
                ret = gs->drawLine(&ctx, _snapBase[1], _ptSnap);
            }
        }
    }
    
    return ret;
}
