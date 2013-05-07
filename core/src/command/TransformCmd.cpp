// TransformCmd.cpp: 实现坐标系变换测试命令
// Author: Zhang Yungui, 2012.7.16
// License: LGPL, https://github.com/rhcad/touchvg

#include <mgcmd.h>
#include <stdio.h>
#include <list>

float mgDisplayMmToModel(float mm, GiGraphics* gs);

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
    virtual bool doContextAction(const MgMotion* sender, int action);

    Point2d getPointM(int index, const MgMotion* sender);
    void setPointW(int index, const MgMotion* sender);


private:
    MgCommand*  _lastCmd;
    Matrix2d    _xfFirst;
    Point2d     _origin;
    Vector2d    _axis[2];
    int         _ptIndex;
};

class HitTestCmd : public MgBaseCommand
{
protected:
    HitTestCmd() : _tol(0), _curid(0), _segment(-1) { _cur.dist = 1e10f; }
public:
    static const char* Name() { return "hittest"; }
    static MgCommand* Create() { return new HitTestCmd; }
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }

    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool mouseHover(const MgMotion* sender) { return touchMoved(sender); }

private:
    struct Item {
        float   dist;
        Point2d nearpt;
        Point2d pt;
    };
    float   _tol;
    Item    _cur;
    std::list<Item> _items;
    int     _curid;
    int     _segment;
};

void registerTransformCmd()
{
    mgRegisterCommand(TransformCmd::Name(), TransformCmd::Create);
    mgRegisterCommand(HitTestCmd::Name(), HitTestCmd::Create);
}

bool HitTestCmd::draw(const MgMotion* sender, GiGraphics* gs)
{
    GiContext ctx(0, GiColor(172, 0, 0, 64));

    for (std::list<Item>::const_iterator it = _items.begin();
         it != _items.end(); ++it) {
        gs->drawLine(&ctx, it->nearpt, it->pt);
    }
    MgShape* sp = sender->view->shapes()->findShape(_curid);
    if (sp) {
        GiContext ctxSel(0, GiColor(0, 0, 255, 128));
        sp->draw(2, *gs, &ctxSel, _segment);
    }
    if (_cur.dist < 1e8f) {
        ctx.setLineAlpha(128);
        ctx.setLineWidth(-1, false);
        gs->drawLine(&ctx, _cur.nearpt, _cur.pt);
    }

    return true;
}

bool HitTestCmd::touchMoved(const MgMotion* sender)
{
    if (mgIsZero(_tol)) {
        _tol = mgDisplayMmToModel(20, sender->view->graph());
    }
    
    Point2d nearpt;
    Box2d box(sender->pointM, 2 * _tol, 0);
    MgShape* sp = sender->view->shapes()->hitTest(box, nearpt, &_segment);
    float mindist = mgDisplayMmToModel(0.5f, sender->view->graph());
    
    _curid = sp ? sp->getID() : 0;
    _cur.dist = sp ? sp->shape()->hitTest2(box.center(), _tol, nearpt) : 1e10f;
    _cur.nearpt = nearpt;
    _cur.pt = box.center();
    sender->view->redraw(true);
    
    if (sp && sender->dragging) {
        //for (float x = -0.25f * _tol; x < 0.25f * _tol; x += _tol * 0.25f) {
        //for (float y = -0.25f * _tol; y < 0.25f * _tol; y += _tol * 0.25f) {
        //box.set(sender->pointM + Vector2d(x, y), 2 * _tol, 0);
        Item item;
        item.dist = sp->shape()->hitTest2(box.center(), _tol, nearpt);
        item.nearpt = nearpt;
        item.pt = box.center();
        if (item.dist < _tol) {
            std::list<Item>::const_iterator it = _items.begin();
            for (; it != _items.end() && !it->pt.isEqualTo(item.pt, Tol(mindist)); ++it) ;
            if (it == _items.end()) {
                _items.push_back(item);
            }
        }
    }

    return true;
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
    sender->view->redraw(true);
    
    return true;
}

bool TransformCmd::initialize(const MgMotion* sender)
{
    MgCommand* lastCmd = mgGetCommandManager()->getCommand();
    if (lastCmd != this) {
        _lastCmd = lastCmd;
    }
    _ptIndex = -1;
    
    if (_axis[0] == _axis[1]) {
        _origin = sender->view->xform()->getCenterW();
        _axis[0] = Vector2d(40.f, 0);
        _axis[1] = Vector2d(0, 40.f);
        _xfFirst.setCoordSystem(_axis[0], _axis[1], _origin);
    }
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

void TransformCmd::setPointW(int index, const MgMotion* sender)
{
    Point2d point = sender->point * sender->view->xform()->displayToWorld();
    
    if (index > 0) {
        float a = (point - _origin).angle2();
        float len = _origin.distanceTo(point);
        a = floorf(0.5f + a * _M_R2D / 5) * 5 * _M_D2R;
        len = floorf(0.5f + len / 5) * 5;
        _axis[index == 1 ? 0 : 1].setAngleLength(a, len);
        
        Matrix2d mat(_axis[0], _axis[1], _origin * sender->view->xform()->worldToModel());
        mat *= _xfFirst.inverse();
        mat = sender->view->doc()->modelTransform() * mat;
        
        sender->view->xform()->setModelTransform(mat);
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
    
    char text[20];
    GiContext ctx(-10, GiColor(255, 0, 0, 128));
    gs->drawLine(&ctx, getPointM(0, sender), getPointM(1, sender));

    Point2d pt(getPointM(1, sender) * gs->xf().modelToDisplay());
#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC8
    sprintf_s(text, sizeof(text), "X %d %dd", 
#else
    sprintf(text, "X %d %dd", 
#endif
        mgRound(_axis[0].length()), mgRound(_axis[0].angle2() * _M_R2D));
    gs->rawTextCenter(text, pt.x, pt.y, 40);
    
    ctx.setLineColor(GiColor(0, 0, 255, 128));
    gs->drawLine(&ctx, getPointM(0, sender), getPointM(2, sender));
    pt = getPointM(2, sender) * gs->xf().modelToDisplay();
#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC8
    sprintf_s(text, sizeof(text), "Y %d %dd", 
#else
    sprintf(text, "Y %d %dd", 
#endif
        mgRound(_axis[1].length()), mgRound(_axis[1].angle2() * _M_R2D));
    gs->rawTextCenter(text, pt.x, pt.y, 40);
    
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
        setPointW(_ptIndex, sender);
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

bool TransformCmd::doContextAction(const MgMotion* sender, int action)
{
    return _lastCmd && _lastCmd->doContextAction(sender, action);
}
