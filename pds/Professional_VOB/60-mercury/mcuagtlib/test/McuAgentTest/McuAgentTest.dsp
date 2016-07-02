# Microsoft Developer Studio Project File - Name="McuAgentTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=McuAgentTest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "McuAgentTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "McuAgentTest.mak" CFG="McuAgentTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "McuAgentTest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "McuAgentTest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "McuAgentTest"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "McuAgentTest - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "McuAgentTest - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\..\common" /I "..\..\..\..\..\10-common\include\platform" /I "..\..\..\..\..\10-common\include\mcu" /I "..\..\..\..\..\10-common\include\system1" /I "..\..\..\..\..\10-common\include\cppunit" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "McuAgentTest - Win32 Release"
# Name "McuAgentTest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\source\AbnormityTest.cpp
# End Source File
# Begin Source File

SOURCE=..\source\BoardAction.cpp
# End Source File
# Begin Source File

SOURCE=..\source\McuAction.cpp
# End Source File
# Begin Source File

SOURCE=.\McuAgentTestEntry.cpp
# End Source File
# Begin Source File

SOURCE=..\source\NmsActTest.cpp
# End Source File
# Begin Source File

SOURCE=..\source\SimuBoard.cpp
# End Source File
# Begin Source File

SOURCE=..\source\SimuMcu.cpp
# End Source File
# Begin Source File

SOURCE=..\source\SimuNMS.cpp
# End Source File
# Begin Source File

SOURCE=..\source\SimuSnmpLib.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\AbnormityTest.h
# End Source File
# Begin Source File

SOURCE=..\..\..\..\common\AgentInterface.h
# End Source File
# Begin Source File

SOURCE=..\include\BoardAction.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\brdmanager.h
# End Source File
# Begin Source File

SOURCE=..\include\McuAction.h
# End Source File
# Begin Source File

SOURCE=..\include\McuAgentTestEntry.h
# End Source File
# Begin Source File

SOURCE=..\include\NmsActTest.h
# End Source File
# Begin Source File

SOURCE=..\include\SimuBoard.h
# End Source File
# Begin Source File

SOURCE=..\include\SimuMcu.h
# End Source File
# Begin Source File

SOURCE=..\include\SimuNMS.h
# End Source File
# Begin Source File

SOURCE=..\include\SimuSnmpLib.h
# End Source File
# Begin Source File

SOURCE=..\include\snmpadp.h
# End Source File
# Begin Source File

SOURCE=..\include\Testevent.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
