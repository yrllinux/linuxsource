# Microsoft Developer Studio Project File - Name="mixer8kilib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mixer8kilib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mixer8kilib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mixer8kilib.mak" CFG="mixer8kilib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mixer8kilib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mixer8kilib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "mixer8kilib"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mixer8kilib - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\include" /I "..\..\common\include" /I "..\..\common\include\platform_sdk\include" /I "..\..\..\10-common\include\mcu8000e" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\algorithm" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\platform" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\10-common\lib\release\win32_8000h\mixer_8000h.lib"

!ELSEIF  "$(CFG)" == "mixer8kilib - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\common\include\platform_sdk\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\algorithm" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\platform" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /D "_8KI_" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\10-common\lib\debug\win32_8000i\mixer_8000i.lib"

!ENDIF 

# Begin Target

# Name "mixer8kilib - Win32 Release"
# Name "mixer8kilib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\source\audbasinst.cpp
# End Source File
# Begin Source File

SOURCE=..\source\audmixer.cpp
# End Source File
# Begin Source File

SOURCE=..\source\audmixinst.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\audbasinst.h
# End Source File
# Begin Source File

SOURCE=..\include\audmixinst.h
# End Source File
# Begin Source File

SOURCE=..\include\errmixer.h
# End Source File
# Begin Source File

SOURCE=..\include\evmixer.h
# End Source File
# Begin Source File

SOURCE=..\..\common\include\mmpcommon.h
# End Source File
# End Group
# Begin Group "Txt Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mixer8kilib.mak
# End Source File
# Begin Source File

SOURCE=..\readme.txt
# End Source File
# End Group
# End Target
# End Project
