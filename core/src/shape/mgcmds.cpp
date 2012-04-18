// mgcmds.cpp
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#include <string.h>
#include "mgcmdselect.h"
#include "mgcmderase.h"
#include "mgdrawrect.h"
#include "mgdrawlines.h"
#include "mgdrawsplines.h"

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
