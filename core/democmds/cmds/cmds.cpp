// cmds.cpp
#include "cmds.h"
#include <cmdsubject.h>
#include <mgspfactory.h>
#include "HitTestCmd.h"
#include <mgshapet.h>

class DemoCmdsObserver : public CmdObserverDefault
{
public:
    DemoCmdsObserver() {}
};

static DemoCmdsObserver _observer;

int DemoCmdsImpl::registerCmds(MgView* view)
{
    view->getCmdSubject()->registerObserver(&_observer);

    //MgShapeT<MgCube>::registerCreator(view->getShapeFactory());
    mgRegisterCommand<HitTestCmd>(view);
    
    return 1;
}

int DemoCmdsImpl::getDimensions(MgView*, float*, char*, int)
{
    return 0;
}
