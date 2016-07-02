# Microsoft Developer Studio Project File - Name="prjMtadpWin32" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=prjMtadpWin32 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "prjmtadpwin32.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "prjmtadpwin32.mak" CFG="prjMtadpWin32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "prjMtadpWin32 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "prjMtadpWin32 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "prjmtadpwin32"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "prjMtadpWin32 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\mt" /I "..\..\..\10-common\include\vrs" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\radstack" /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol\h323stack" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\cbb\license" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_CONFIG_FROM_FILE" /D "_UTF8" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\10-common\lib\release\Win32\mtadp.lib"

!ELSEIF  "$(CFG)" == "prjMtadpWin32 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\mt" /I "..\..\..\10-common\include\vrs" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\radstack" /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol\h323stack" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\cbb\license" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /D "_CONFIG_FROM_FILE" /D "_8KE_" /D "_UTF8" /FR /YX /FD /GZ /c
# SUBTRACT CPP /X
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\10-common\lib\debug\Win32\mtadp.lib"

!ENDIF 

# Begin Target

# Name "prjMtadpWin32 - Win32 Release"
# Name "prjMtadpWin32 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\source\gk.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mtadp.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mtadpd.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mtadputils.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\common\include\bindmtadp.h
# End Source File
# Begin Source File

SOURCE=..\include\mastruct36.h
# End Source File
# Begin Source File

SOURCE=..\include\mastructv4r4b2.h
# End Source File
# Begin Source File

SOURCE=..\include\mtadp.h
# End Source File
# Begin Source File

SOURCE=..\include\mtadputils.h
# End Source File
# End Group
# Begin Group "Txt Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\prjmtadpwin32.mak
# End Source File
# Begin Source File

SOURCE=..\readme.txt
# End Source File
# End Group
# End Target
# End Project
