// mgactions.cpp: 实现上下文动作分发接口
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgcmdmgr.h"
#include <mgselect.h>
#include <mgbasicsp.h>

bool MgCmdManagerImpl::showInDrawing(const MgMotion* sender, const MgShape* shape)
{
    return showInSelect(sender, kMgSelDraw, shape, Box2d(sender->pointM, 0, 0));
}

bool MgCmdManagerImpl::showInSelect(const MgMotion* sender, int selState, const MgShape* shape, const Box2d& selbox)
{
    int actions[10];
    int n = 0;
    bool islines = shape && shape->shapec()->isKindOf(MgBaseLines::Type());
    bool fixedLength = shape && shape->shapec()->getFlag(kMgFixedLength);
    bool locked = shape && shape->shapec()->getFlag(kMgShapeLocked);
    bool closed = shape && shape->shapec()->isClosed();
    
    if (selState > kMgSelNone && selState <= kMgSelVertex && shape) {
        const MgBaseShape* sp = shape->shapec();
        if (sp->isKindOf(MgLine::Type())) {
            actions[n++] = kMgActionBreak;
        }
        if (sp->isClosed() && sp->getPointCount() == 4) {
            if (sp->isKindOf(MgLine::Type())
                || sp->isKindOf(MgRect::Type())
                || sp->isKindOf(MgDiamond::Type())
                || sp->isKindOf(MgLines::Type())
                || sp->isKindOf(MgParallelogram::Type())) {
                actions[n++] = kMgActionBreak;
            }
        }
    }
    
    switch (selState) {
        case kMgSelNone:
            actions[n++] = kMgActionSelAll;
            actions[n++] = kMgActionDraw;
            break;
            
        case kMgSelOneShape:
        case kMgSelMultiShapes:
            actions[n++] = kMgActionDelete;
            actions[n++] = kMgActionClone;
            actions[n++] = fixedLength ? kMgActionFreeLength : kMgActionFixedLength;
            actions[n++] = locked ? kMgActionUnlocked : kMgActionLocked;
            if (selState == kMgSelOneShape) {
                actions[n++] = kMgActionEditVertex;
            }
            else {
                actions[n++] = kMgActionSelReset;
            }
            break;
            
        case kMgSelVertexes:
            if (islines) {
                actions[n++] = closed ? kMgActionOpened : kMgActionClosed;
                actions[n++] = kMgActionAddVertex;
            }
            actions[n++] = fixedLength ? kMgActionFreeLength : kMgActionFixedLength;
            actions[n++] = locked ? kMgActionUnlocked : kMgActionLocked;
            actions[n++] = kMgActionHideVertex;
            break;
            
        case kMgSelVertex:
            if (islines) {
                actions[n++] = closed ? kMgActionOpened : kMgActionClosed;
                actions[n++] = kMgActionDelVertex;
            }
            actions[n++] = fixedLength ? kMgActionFreeLength : kMgActionFixedLength;
            actions[n++] = locked ? kMgActionUnlocked : kMgActionLocked;
            actions[n++] = kMgActionHideVertex;
            break;
            
        case kMgSelDraw:
            break;
    }
    actions[n++] = 0;
    
    if (!selbox.isEmpty()) {
        sender->view->redraw(true);
    }
    
    Box2d box(selbox.isEmpty() ? Box2d(sender->pointM, 0, 0) : selbox);
    box *= sender->view->xform()->modelToDisplay();
    
    return sender->view->showContextActions(selState, actions, box);
}

void MgCmdManagerImpl::doAction(const MgMotion* sender, int action)
{
    MgView* view = sender->view;
    MgSelection *sel = mgGetCommandManager()->getSelection(view);
    
    switch (action) {
        case kMgActionSelAll:
            if (sel)
                sel->selectAll(view);
            break;
            
        case kMgActionSelReset:
            if (sel)
                sel->resetSelection(view);
            break;
            
        case kMgActionDraw:
            setCommand(sender, "@draw");
            break;
            
        case kMgActionCancel:
            setCommand(sender, "select");
            break;
            
        case kMgActionDelete:
            if (sel) sel->deleteSelection(view);
            break;
            
        case kMgActionClone:
            if (sel) sel->cloneSelection(view);
            break;
            
        case kMgActionBreak:
            setCommand(sender, "break");
            break;
            
        case kMgActionFixedLength:
        case kMgActionFreeLength:
            if (sel) sel->setFixedLength(view, !sel->isFixedLength(view));
            break;
            
        case kMgActionLocked:
        case kMgActionUnlocked:
            if (sel) sel->setLocked(view, !sel->isLocked(view));
            break;
            
        case kMgActionEditVertex:
        case kMgActionHideVertex:
            if (sel) sel->setVertexMode(view, !sel->isVertexMode(view));
            break;
            
        case kMgActionClosed:
        case kMgActionOpened:
            if (sel) sel->switchClosed(view);
            break;
            
        case kMgActionAddVertex:
            if (sel) sel->insertVertext(sender);
            break;
            
        case kMgActionDelVertex:
            if (sel) sel->deleteVertext(sender);
            break;
            
        default:
            break;
    }
}
