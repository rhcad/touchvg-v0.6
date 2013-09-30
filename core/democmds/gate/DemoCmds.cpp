// DemoCmds.cpp
#include "DemoCmds.h"
#include "cmds.h"

int DemoCmdsGate::registerCmds(MGVIEW_HANDLE mgView)
{
    return DemoCmdsImpl::registerCmds(mgView);
}

int DemoCmdsGate::getDimensions(MGVIEW_HANDLE mgView, 
                                mgvector<float>& vars, mgvector<char>& types)
{
    int n = vars.count() < types.count() ? vars.count() : types.count();
    return DemoCmdsImpl::getDimensions(mgView, 
        vars.address(), types.address(), n);
}
