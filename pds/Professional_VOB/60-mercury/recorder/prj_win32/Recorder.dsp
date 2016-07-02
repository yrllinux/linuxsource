# Microsoft Developer Studio Project File - Name="Recorder" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Recorder - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Recorder.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Recorder.mak" CFG="Recorder - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Recorder - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Recorder - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/software/implement_h323/Recorder", HJDEAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Recorder - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Od /I "." /I "..\include" /I "..\wmsSDK\include" /I "..\..\common\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\nms1" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\..\b0-thirdparty\haikey\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "REC_CPPUNIT_TEST" /D "_RECSERV_" /FD /c
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
# ADD LINK32 kdvlog.lib winmm.lib OspLib.lib kdvsys.lib fcnet.lib enfc.lib KdvMediaNet.lib KdvEncrypt.lib RPCtrl.lib asflib.lib voddblib.lib usbverify.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"nafxcwd.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"LIBC.lib" /nodefaultlib:"nafxcw.lib" /libpath:"..\..\..\10-Common\lib\release\win32" /libpath:"..\..\..\b0-thirdparty\haikey\lib\release\win32"
# SUBTRACT LINK32 /map

!ELSEIF  "$(CFG)" == "Recorder - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "." /I "..\include" /I "..\wmsSDK\include" /I "..\..\common\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\nms1" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\..\b0-thirdparty\haikey\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "REC_CPPUNIT_TEST" /D "_RECSERV_" /FR /FD /GZ /c
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
# ADD LINK32 kdvlog.lib winmm.lib OspLib.lib kdvsys.lib fcnet.lib enfc.lib KdvMediaNet.lib KdvEncrypt.lib RPCtrl.lib asflib.lib voddblib.lib usbverify.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"LIBC.lib" /nodefaultlib:"msvcrt.lib" /pdbtype:sept /libpath:"..\..\..\10-Common\lib\debug\win32" /libpath:"..\..\..\b0-thirdparty\haikey\lib\debug\win32"

!ENDIF 

# Begin Target

# Name "Recorder - Win32 Release"
# Name "Recorder - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "RecServer No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\RecServInst.cpp
# End Source File
# Begin Source File

SOURCE=..\source\RecServLog.cpp
# End Source File
# Begin Source File

SOURCE=..\source\State.cpp
# End Source File
# Begin Source File

SOURCE=..\source\StateBase.cpp
# End Source File
# Begin Source File

SOURCE=..\source\StateMgr.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\source\ChnInst.cpp
# End Source File
# Begin Source File

SOURCE=..\source\ConfAgent.cpp
# End Source File
# Begin Source File

SOURCE=..\source\QuitRecDlg.cpp
# End Source File
# Begin Source File

SOURCE=..\source\RecApp.cpp
# End Source File
# Begin Source File

SOURCE=..\source\Recorder.cpp
# End Source File
# Begin Source File

SOURCE=.\Recorder.rc
# End Source File
# Begin Source File

SOURCE=..\source\RecorderDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "RecServer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\RecServ_Def.h
# End Source File
# Begin Source File

SOURCE=..\include\RecServInst.h
# End Source File
# Begin Source File

SOURCE=..\include\State.h
# End Source File
# Begin Source File

SOURCE=..\include\StateBase.h
# End Source File
# Begin Source File

SOURCE=..\include\StateMgr.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\INCLUDE\ChnInst.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\ConfAgent.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\evrec.h
# End Source File
# Begin Source File

SOURCE=..\include\QuitRecDlg.h
# End Source File
# Begin Source File

SOURCE=..\INCLUDE\RecApp.h
# End Source File
# Begin Source File

SOURCE=..\include\RecError.h
# End Source File
# Begin Source File

SOURCE=..\include\Recorder.h
# End Source File
# Begin Source File

SOURCE=..\include\RecorderDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\KDC.ico
# End Source File
# Begin Source File

SOURCE=.\res\Recorder.ico
# End Source File
# Begin Source File

SOURCE=.\res\Recorder.rc2
# End Source File
# End Group
# Begin Group "Txt Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\readme.txt
# End Source File
# Begin Source File

SOURCE=.\Recorder.mak
# End Source File
# End Group
# End Target
# End Project
