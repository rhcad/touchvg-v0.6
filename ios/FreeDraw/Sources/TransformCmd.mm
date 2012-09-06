//
//  TransformCmd.mm
//  FreeDraw
//  Created by Zhang Yungui on 2012-7-16.
//

#include <mgcmd.h>

class TransformCmd : public MgBaseCommand
{
protected:
    TransformCmd();
    virtual ~TransformCmd();

public:
    static const char* Name() { return "xfdemo"; }
    static MgCommand* Create() { return new TransformCmd; }

private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }

    virtual bool cancel(const MgMotion* sender);
    virtual bool initialize(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
    virtual void gatherShapes(const MgMotion* sender, MgShapes* shapes);
    virtual bool click(const MgMotion* sender);
    virtual bool doubleClick(const MgMotion* sender);
    virtual bool longPress(const MgMotion* sender);

    Point2d getPointM(int index, const MgMotion* sender);
    void setPointM(int index, const MgMotion* sender);


private:
    MgCommand*  _lastCmd;
    Matrix2d    _xfFirst;
    Point2d     _origin;
    Vector2d    _axis[2];
    int         _ptIndex;
};

void registerTransformCmd()
{
    mgRegisterCommand(TransformCmd::Name(), TransformCmd::Create);
}

TransformCmd::TransformCmd() : _lastCmd(NULL), _ptIndex(-1)
{
}

TransformCmd::~TransformCmd()
{
}

bool TransformCmd::cancel(const MgMotion* sender)
{
    _lastCmd = NULL;
    
    return true;
}

bool TransformCmd::initialize(const MgMotion* sender)
{
    _lastCmd = mgGetCommandManager()->getCommand();
    _ptIndex = -1;
    
    _origin = sender->point * sender->view->xform()->displayToWorld();
    _axis[0] = Vector2d(40.f, 0);
    _axis[1] = Vector2d(0, 40.f);
    _xfFirst.setCoordSystem(_axis[0], _axis[1], _origin);
    
    sender->view->redraw(true);
    
    return true;
}

Point2d TransformCmd::getPointM(int index, const MgMotion* sender)
{
    if (index > 0) {
        return (_origin + _axis[index == 1 ? 0 : 1]) * sender->view->xform()->worldToModel();
    }
    return _origin * sender->view->xform()->worldToModel();
}

void TransformCmd::setPointM(int index, const MgMotion* sender)
{
    Point2d point = sender->point * sender->view->xform()->displayToWorld();
    
    if (index > 0) {
        _axis[index == 1 ? 0 : 1] = point - _origin;
        if (!sender->pressDrag) {
            if (index == 1)     // move x-axis
                _axis[1] = _axis[0].perpVector();
            else
                _axis[0] = -_axis[1].perpVector();
        }
        
        Matrix2d mat(_axis[0], _axis[1], _origin * sender->view->xform()->worldToModel());
        mat *= _xfFirst.inverse();
        
        sender->view->xform()->setModelTransform(sender->view->shapes()->modelTransform() * mat);
        sender->view->regen();
    } else {
        _origin = point;
        sender->view->redraw(true);
    }
}

bool TransformCmd::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (_lastCmd)
        _lastCmd->draw(sender, gs);
    
    GiContext ctx(-10, GiColor(255, 0, 0, 128));
    gs->drawLine(&ctx, getPointM(0, sender), getPointM(1, sender));
    ctx.setLineColor(GiColor(0, 0, 255, 128));
    gs->drawLine(&ctx, getPointM(0, sender), getPointM(2, sender));
    
    return true;
}

bool TransformCmd::touchBegan(const MgMotion* sender)
{
    for (_ptIndex = 2; _ptIndex >= 0; _ptIndex--) {
        if (sender->view->xform()->displayToModel(5, true)
            > sender->pointM.distanceTo(getPointM(_ptIndex, sender))) {
            break;
        }
    }
    return _ptIndex >= 0 || (_lastCmd && _lastCmd->touchBegan(sender));
}

bool TransformCmd::touchMoved(const MgMotion* sender)
{
    if (_ptIndex >= 0) {
        setPointM(_ptIndex, sender);
    }
    return _ptIndex >= 0 || (_lastCmd && _lastCmd->touchMoved(sender));
}

bool TransformCmd::touchEnded(const MgMotion* sender)
{
    bool ret =  _ptIndex >= 0 || (_lastCmd && _lastCmd->touchEnded(sender));
    
    _ptIndex = -1;
    
    return ret;
}

void TransformCmd::gatherShapes(const MgMotion* sender, MgShapes* shapes)
{
    if (_lastCmd)
        _lastCmd->gatherShapes(sender, shapes);
}

bool TransformCmd::click(const MgMotion* sender)
{
    return _lastCmd && _lastCmd->click(sender);
}

bool TransformCmd::doubleClick(const MgMotion* sender)
{
    return _lastCmd && _lastCmd->doubleClick(sender);
}

bool TransformCmd::longPress(const MgMotion* sender)
{
    return _lastCmd && _lastCmd->longPress(sender);
}
