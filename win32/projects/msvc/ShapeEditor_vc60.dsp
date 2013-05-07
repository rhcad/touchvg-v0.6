# Microsoft Developer Studio Project File - Name="ShapeEditor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ShapeEditor - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ShapeEditor_vc60.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ShapeEditor_vc60.mak" CFG="ShapeEditor - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ShapeEditor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ShapeEditor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ShapeEditor - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "../../../build/vc60/Release"
# PROP BASE Intermediate_Dir "../../../build/vc60/obj/ShapeEditor/Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "../../../build/vc60/Release"
# PROP Intermediate_Dir "../../../build/vc60/obj/ShapeEditor/Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W2 /GX /O2 /I "..\..\..\core\include\geom" /I "..\..\..\core\include\graph" /I "..\..\..\core\include\shape" /I "..\..\..\core\include\command" /I "..\..\..\core\include\testgraph" /I "..\..\..\core\src\json" /I "..\..\include\graphwin" /I "..\..\include\view" /I "..\..\src\ShapeEditor" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 shlwapi.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /machine:I386 /out:"../../../build/vc60/Release/ShapeEditor.exe"

!ELSEIF  "$(CFG)" == "ShapeEditor - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "../../../build/vc60/Debug"
# PROP BASE Intermediate_Dir "../../../build/vc60/obj/ShapeEditor/Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "../../../build/vc60/Debug"
# PROP Intermediate_Dir "../../../build/vc60/obj/ShapeEditor/Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W2 /Gm /GX /ZI /Od /I "..\..\..\core\include\geom" /I "..\..\..\core\include\graph" /I "..\..\..\core\include\shape" /I "..\..\..\core\include\command" /I "..\..\..\core\include\testgraph" /I "..\..\..\core\src\json" /I "..\..\include\graphwin" /I "..\..\include\view" /I "..\..\src\ShapeEditor" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "_UNICODE" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 shlwapi.lib /nologo /entry:"wWinMainCRTStartup" /subsystem:windows /debug /machine:I386 /out:"../../../build/vc60/Debug/ShapeEditor.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ShapeEditor - Win32 Release"
# Name "ShapeEditor - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;def;r;odl;idl;hpj;bat"
# Begin Group "command"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\core\src\command\mgactions.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgcmddraw.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgcmderase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgcmderase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgcmdmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgcmdmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgcmds.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgcmdselect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgcmdselect.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgdrawline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgdrawline.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgdrawlines.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgdrawlines.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgdrawrect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgdrawrect.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgdrawsplines.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgdrawsplines.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgdrawtriang.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\mgdrawtriang.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\command\TransformCmd.cpp
# End Source File
# End Group
# Begin Group "shape"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\core\src\shape\mgellipse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\shape\mggrid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\shape\mgline.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\shape\mglines.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\shape\mgrdrect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\shape\mgrect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\shape\mgshape.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\shape\mgshapes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\shape\mgshapedoc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\shape\mgcomposite.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\shape\mgsplines.cpp
# End Source File
# End Group
# Begin Group "graph"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\core\src\graph\gigraph.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\graph\gipath.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\graph\giplclip.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\graph\gixform.cpp
# End Source File
# End Group
# Begin Group "geom"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\core\src\geom\mgbase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\geom\mgbnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\geom\mgbox.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\geom\mgcurv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\geom\mgdblpt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\geom\mglnrel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\geom\mgmat.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\geom\mgnear.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\geom\mgnearbz.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\core\src\geom\mgvec.cpp
# End Source File
# End Group
# Begin Group "testgraph"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\core\include\testgraph\RandomShape.cpp
# End Source File
# End Group
# Begin Group "graphwin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\graphwin\canvasgdi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\graphwin\canvasgdip.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\graphwin\canvaswin.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\graphwin\giimage.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\graphwin\giimagep.cpp
# End Source File
# End Group
# Begin Group "json"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\core\src\json\mgjsonstorage.cpp
# End Source File
# End Group
# Begin Group "view"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\view\GraphView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\include\view\GraphView.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\ShapeEditor\CmdLineParams.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ShapeEditor\EditorView.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ShapeEditor\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ShapeEditor\ShapeEditor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\ShapeEditor\stdafx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "command_"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\core\include\command\mgaction.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\command\mgcmd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\command\mgcmddraw.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\command\mgselect.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\command\mgsnap.h
# End Source File
# End Group
# Begin Group "shape_"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\core\include\shape\mgbasicsp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\shape\mggrid.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\shape\mgjsonstorage.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\shape\mgobject.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\shape\mgshape.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\shape\mgshape_.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\shape\mgshapes.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\shape\mgshapest.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\shape\mgshapet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\shape\mgshapedoc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\shape\mgcomposite.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\shape\mgstorage.h
# End Source File
# End Group
# Begin Group "graph_"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\core\include\graph\gicanvas.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\graph\gicanvdr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\graph\gicolor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\graph\gicontxt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\graph\gidef.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\graph\gigraph.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\graph\gigraph_.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\graph\gipath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\graph\gixform.h
# End Source File
# End Group
# Begin Group "geom_"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\core\include\geom\mgbase.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\geom\mgbnd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\geom\mgbox.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\geom\mgcurv.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\geom\mgdef.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\geom\mglnrel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\geom\mgmat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\geom\mgnear.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\geom\mgpnt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\geom\mgtol.h
# End Source File
# Begin Source File

SOURCE=..\..\..\core\include\geom\mgvec.h
# End Source File
# End Group
# Begin Group "testgraph_"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\core\include\testgraph\RandomShape.h
# End Source File
# End Group
# Begin Group "graphwin_"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\include\graphwin\canvasgdi.h
# End Source File
# Begin Source File

SOURCE=..\..\include\graphwin\canvasgdip.h
# End Source File
# Begin Source File

SOURCE=..\..\include\graphwin\canvaswin.h
# End Source File
# Begin Source File

SOURCE=..\..\include\graphwin\gdiobj.h
# End Source File
# Begin Source File

SOURCE=..\..\include\graphwin\giimage.h
# End Source File
# Begin Source File

SOURCE=..\..\include\graphwin\giimagep.h
# End Source File
# Begin Source File

SOURCE=..\..\include\graphwin\graphwin.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\ShapeEditor\CmdLineParams.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ShapeEditor\EditorView.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ShapeEditor\MainFrm.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ShapeEditor\readints.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ShapeEditor\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ShapeEditor\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ShapeEditor\targetver.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\..\src\ShapeEditor\res\ShapeEditor.ico
# End Source File
# Begin Source File

SOURCE=..\..\src\ShapeEditor\ShapeEditor.rc
# End Source File
# End Group
# End Target
# End Project
