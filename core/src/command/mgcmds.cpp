// mgcmds.cpp
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include <string.h>
#include <mgshapet.h>
#include <mgbasicsp.h>
#include "mgcmdselect.h"
#include "mgcmderase.h"
#include "mgdrawrect.h"
#include "mgdrawline.h"
#include "mgdrawlines.h"
#include "mgdrawsplines.h"
#include "mgdrawtriang.h"

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
        { MgCmdDrawDiamond::Name(), MgCmdDrawDiamond::Create },
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
    
    MgShapesLock locker(sender->view->doc(), MgShapesLock::Add);
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
