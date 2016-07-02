# Microsoft Developer Studio Project File - Name="eqpagtcommbase" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=eqpagtcommbase - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "eqpagtcommbase.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "eqpagtcommbase.mak" CFG="eqpagtcommbase - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "eqpagtcommbase - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "eqpagtcommbase - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "eqpagtcommbase"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "eqpagtcommbase - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\include\\" /I "..\..\..\..\60-mercury\common\include\platform_sdk\include" /I "..\..\..\..\60-mercury\common\include\\" /I "..\..\..\..\10-common\include\system1\\" /I "..\..\..\..\10-common\include\snmp\\" /I "..\..\..\..\10-common\include\platform\\" /I "..\..\..\..\10-common\include\protocol\\" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\..\10-common\lib\release\win32\eqpagtcommbase.lib"

!ELSEIF  "$(CFG)" == "eqpagtcommbase - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\include\\" /I "..\..\..\..\60-mercury\common\include\platform_sdk\include" /I "..\..\..\..\60-mercury\common\include\\" /I "..\..\..\..\10-common\include\system1\\" /I "..\..\..\..\10-common\include\snmp\\" /I "..\..\..\..\10-common\include\platform\\" /I "..\..\..\..\10-common\include\protocol\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\..\10-common\lib\debug\win32\eqpagtcommbase.lib"

!ENDIF 

# Begin Target

# Name "eqpagtcommbase - Win32 Release"
# Name "eqpagtcommbase - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\source\eqpagtcfg.cpp
# End Source File
# Begin Source File

SOURCE=..\source\eqpagtcommbase.cpp
# End Source File
# Begin Source File

SOURCE=..\source\eqpagtipinfo.cpp
# End Source File
# Begin Source File

SOURCE=..\source\eqpagtscan.cpp
# End Source File
# Begin Source File

SOURCE=..\source\eqpagtsnmp.cpp
# End Source File
# Begin Source File

SOURCE=..\source\eqpagtutility.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\eqpagtcfg.h
# End Source File
# Begin Source File

SOURCE=..\include\eqpagtcommbase.h
# End Source File
# Begin Source File

SOURCE=..\include\eqpagtipinfo.h
# End Source File
# Begin Source File

SOURCE=..\include\eqpagtscan.h
# End Source File
# Begin Source File

SOURCE=..\include\eqpagtsnmp.h
# End Source File
# Begin Source File

SOURCE=..\include\eqpagtutility.h
# End Source File
# End Group
# Begin Group "Txt Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\eqpagtcommbase.mak
# End Source File
# Begin Source File

SOURCE=..\readme.txt
# End Source File
# End Group
# End Target
# End Project
