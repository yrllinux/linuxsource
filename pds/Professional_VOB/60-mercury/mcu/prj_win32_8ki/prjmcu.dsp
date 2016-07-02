# Microsoft Developer Studio Project File - Name="prjMcu" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=PRJMCU - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "prjmcu.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "prjmcu.mak" CFG="PRJMCU - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "prjMcu - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "prjMcu - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "prjMcu"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "prjMcu - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "release"
# PROP Intermediate_Dir "release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\t120" /I "..\..\..\10-common\include\nms1" /I "..\..\..\10-common\include\cbb\license" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_UTF8" /YX /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 mcuagt8ki.lib lanman.lib kdvlog.lib osplib.lib kdvsys.lib winbrdwrapper.lib commonlib.lib dataswitch.lib mp.lib kdv323.lib kdv323adp.lib mtadp.lib kdvaddrbook.lib kdvdatanet.lib fcnet.lib enfc.lib kdvmedianet.lib mcuprs.lib kdvnetbuf.lib kdvencrypt.lib snmpadp_mcu.lib agent_pp.lib snmp_pp.lib kdvum.lib kdvcascade.lib readlicenselib.lib hkapi.lib rockey2.lib ep1kdl20.lib h460pinholelib.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"nafxcw.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /out:"mcu.exe" /libpath:"..\..\..\10-common\lib\release\win32"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\t120" /I "..\..\..\10-common\include\nms1" /I "..\..\..\10-common\include\cbb\license" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /D "_8KI_" /D "_UTF8" /YX /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 mcuagt8ki.lib lanman.lib kdvlog.lib osplib.lib kdvsys.lib winbrdwrapper.lib commonlib.lib dataswitch.lib mp.lib kdv323.lib kdv323adp.lib mtadp.lib kdvaddrbook.lib kdvdatanet.lib fcnet.lib enfc.lib kdvmedianet.lib mcuprs.lib kdvnetbuf.lib kdvencrypt.lib snmpadp_mcu.lib agent_pp.lib snmp_pp.lib kdvum.lib kdvcascade.lib readlicenselib.lib hkapi.lib rockey2.lib ep1kdl20.lib h460pinholelib.lib /nologo /stack:0x7a1200 /subsystem:windows /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /out:"mcu.exe" /pdbtype:sept /libpath:"..\..\..\10-common\lib\debug\win32"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "prjMcu - Win32 Release"
# Name "prjMcu - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Eqp No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\source\eqpbase.cpp
# End Source File
# Begin Source File

SOURCE=..\source\tvwall.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\source\dcsssn.cpp
# End Source File
# Begin Source File

SOURCE=..\source\eqpssn.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mcsssn.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mcu.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mcucfg.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mcudata.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mcuguard.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mcuutility.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mcuvc.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mcuvcdcs.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mcuvceqp.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mcuvcmcs.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mcuvcmmcu.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mcuvcmt.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mcuvcswitch.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mcuvcvcs.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mpmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mpssn.cpp
# End Source File
# Begin Source File

SOURCE=..\source\msmanagerssn.cpp
# End Source File
# Begin Source File

SOURCE=..\source\mtadpssn.cpp
# End Source File
# Begin Source File

SOURCE=..\source\nplusmanager.cpp
# End Source File
# Begin Source File

SOURCE=..\source\vcsssn.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Eqp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\include\eqpbase.h
# End Source File
# Begin Source File

SOURCE=..\include\tvwall.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\include\dcsssn.h
# End Source File
# Begin Source File

SOURCE=..\include\eqpssn.h
# End Source File
# Begin Source File

SOURCE=..\..\common\include\evmcu.h
# End Source File
# Begin Source File

SOURCE="..\..\..\10-common\include\mcu\evmcudcs.h"
# End Source File
# Begin Source File

SOURCE=..\..\common\include\evmcueqp.h
# End Source File
# Begin Source File

SOURCE="..\..\..\10-Common\include\mcu\evmcumcs.h"
# End Source File
# Begin Source File

SOURCE=..\..\common\include\evmcumt.h
# End Source File
# Begin Source File

SOURCE="..\..\..\10-common\include\mcu\evmcuvcs.h"
# End Source File
# Begin Source File

SOURCE=..\include\licensekeyconst.h
# End Source File
# Begin Source File

SOURCE=..\include\mcsssn.h
# End Source File
# Begin Source File

SOURCE=..\include\mcucfg.h
# End Source File
# Begin Source File

SOURCE="..\..\..\10-Common\include\mcu\mcuconst.h"
# End Source File
# Begin Source File

SOURCE=..\include\mcudata.h
# End Source File
# Begin Source File

SOURCE="..\..\..\10-common\include\mcu\mcuerrcode.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\10-Common\include\mcu\mcuerrstr.txt"
# End Source File
# Begin Source File

SOURCE="..\..\..\10-common\include\mcu\mcuerrstr_english.txt"
# End Source File
# Begin Source File

SOURCE=..\include\mcuguard.h
# End Source File
# Begin Source File

SOURCE="..\..\..\10-common\include\mcu\mcupfmlmt.h"
# End Source File
# Begin Source File

SOURCE="..\..\..\10-Common\include\mcu\mcustruct.h"
# End Source File
# Begin Source File

SOURCE=..\include\mcuutility.h
# End Source File
# Begin Source File

SOURCE=..\include\mcuvc.h
# End Source File
# Begin Source File

SOURCE=..\..\common\include\mcuver.h
# End Source File
# Begin Source File

SOURCE=..\include\mpmanager.h
# End Source File
# Begin Source File

SOURCE=..\include\mpssn.h
# End Source File
# Begin Source File

SOURCE=..\include\msmanagerssn.h
# End Source File
# Begin Source File

SOURCE=..\include\mtadpssn.h
# End Source File
# Begin Source File

SOURCE=..\include\nplusmanager.h
# End Source File
# Begin Source File

SOURCE="..\..\..\10-common\include\system1\vccommon.h"
# End Source File
# Begin Source File

SOURCE=..\include\vcsssn.h
# End Source File
# Begin Source File

SOURCE="..\..\..\10-common\include\mcu\vcsstruct.h"
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Txt Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\prjmcu.mak
# End Source File
# Begin Source File

SOURCE=..\Readme.txt
# End Source File
# End Group
# End Target
# End Project
