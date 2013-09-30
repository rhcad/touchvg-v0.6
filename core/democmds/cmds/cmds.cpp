// cmds.cpp
#include "cmds.h"
#include <cmdsubject.h>
#include <mgspfactory.h>
#include "HitTestCmd.h"
#include <mgshapet.h>

class EduCmdObserver : public CmdObserverDefault
{
public:
    EduCmdObserver() {}
};

static EduCmdObserver _observer;

int DemoCmdsImpl::registerCmds(MGVIEW_HANDLE mgView)
{
    MgView* view = (MgView*)mgView;
    view->getCmdSubject()->registerObserver(&_observer);

    //MgShapeT<MgCube>::registerCreator(view->getShapeFactory());
    mgRegisterCommand<HitTestCmd>(view);
    
    return 1;
}

int DemoCmdsImpl::getDimensions(MGVIEW_HANDLE, float*, char*, int)
{
    return 0;
}
