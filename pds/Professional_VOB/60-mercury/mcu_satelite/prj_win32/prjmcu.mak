# Microsoft Developer Studio Generated NMAKE File, Based on prjmcu.dsp
!IF "$(CFG)" == ""
CFG=prjMcu - Win32 TestDebug
!MESSAGE No configuration specified. Defaulting to prjMcu - Win32 TestDebug.
!ENDIF 

!IF "$(CFG)" != "prjMcu - Win32 Release" && "$(CFG)" != "prjMcu - Win32 Debug" && "$(CFG)" != "prjMcu - Win32 TestDebug" && "$(CFG)" != "prjMcu - Win32 TestRelease"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "prjmcu.mak" CFG="prjMcu - Win32 TestDebug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "prjMcu - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "prjMcu - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "prjMcu - Win32 TestDebug" (based on "Win32 (x86) Console Application")
!MESSAGE "prjMcu - Win32 TestRelease" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "prjMcu - Win32 Release"

OUTDIR=.\release
INTDIR=.\release

!IF "$(RECURSE)" == "0" 

ALL : ".\mcu.exe"

!ELSE 

ALL : "mcuagtlib - Win32 Release" "prjMtadpWin32 - Win32 Release" ".\mcu.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"prjMtadpWin32 - Win32 ReleaseCLEAN" "mcuagtlib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dcsssn.obj"
	-@erase "$(INTDIR)\eqpssn.obj"
	-@erase "$(INTDIR)\mcsssn.obj"
	-@erase "$(INTDIR)\mcu.obj"
	-@erase "$(INTDIR)\mcucfg.obj"
	-@erase "$(INTDIR)\mcudata.obj"
	-@erase "$(INTDIR)\mcuguard.obj"
	-@erase "$(INTDIR)\mcuutility.obj"
	-@erase "$(INTDIR)\mcuvc.obj"
	-@erase "$(INTDIR)\mcuvcdcs.obj"
	-@erase "$(INTDIR)\mcuvceqp.obj"
	-@erase "$(INTDIR)\mcuvcmcs.obj"
	-@erase "$(INTDIR)\mcuvcmmcu.obj"
	-@erase "$(INTDIR)\mcuvcmt.obj"
	-@erase "$(INTDIR)\mcuvcnms.obj"
	-@erase "$(INTDIR)\mcuvcswitch.obj"
	-@erase "$(INTDIR)\mcuvcvcs.obj"
	-@erase "$(INTDIR)\modemssn.obj"
	-@erase "$(INTDIR)\mpmanager.obj"
	-@erase "$(INTDIR)\mpssn.obj"
	-@erase "$(INTDIR)\msmanagerssn.obj"
	-@erase "$(INTDIR)\mtadpssn.obj"
	-@erase "$(INTDIR)\mtssn.obj"
	-@erase "$(INTDIR)\nplusmanager.obj"
	-@erase "$(INTDIR)\udpdispatch.obj"
	-@erase "$(INTDIR)\udpnms.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vcsssn.obj"
	-@erase ".\mcu.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\t120" /I "..\..\..\10-common\include\nms1" /I "..\..\..\10-common\include\cbb\license" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_SATELITE_" /Fp"$(INTDIR)\prjmcu.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\prjmcu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=osplib.lib kdvsys.lib winbrdwrapper.lib mcuagt.lib dataswitch.lib mp.lib kdv323.lib kdv323adp.lib mtadp.lib kdvaddrbook.lib kdvdatanet.lib fcnet.lib enfc.lib kdvmedianet.lib mcuprs.lib kdvnetbuf.lib kdvencrypt.lib snmpadp_mcu.lib agent_pp.lib snmp_pp.lib kdvum.lib kdvcascade.lib readlicenselib.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\mcu.pdb" /machine:I386 /nodefaultlib:"nafxcw.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /out:"mcu.exe" /libpath:"..\..\..\10-common\lib\release\win32" 
LINK32_OBJS= \
	"$(INTDIR)\dcsssn.obj" \
	"$(INTDIR)\eqpssn.obj" \
	"$(INTDIR)\mcsssn.obj" \
	"$(INTDIR)\mcu.obj" \
	"$(INTDIR)\mcucfg.obj" \
	"$(INTDIR)\mcudata.obj" \
	"$(INTDIR)\mcuguard.obj" \
	"$(INTDIR)\mcuutility.obj" \
	"$(INTDIR)\mcuvc.obj" \
	"$(INTDIR)\mcuvcdcs.obj" \
	"$(INTDIR)\mcuvceqp.obj" \
	"$(INTDIR)\mcuvcmcs.obj" \
	"$(INTDIR)\mcuvcmmcu.obj" \
	"$(INTDIR)\mcuvcmt.obj" \
	"$(INTDIR)\mcuvcnms.obj" \
	"$(INTDIR)\mcuvcswitch.obj" \
	"$(INTDIR)\mcuvcvcs.obj" \
	"$(INTDIR)\modemssn.obj" \
	"$(INTDIR)\mpmanager.obj" \
	"$(INTDIR)\mpssn.obj" \
	"$(INTDIR)\msmanagerssn.obj" \
	"$(INTDIR)\mtadpssn.obj" \
	"$(INTDIR)\mtssn.obj" \
	"$(INTDIR)\nplusmanager.obj" \
	"$(INTDIR)\udpdispatch.obj" \
	"$(INTDIR)\udpnms.obj" \
	"$(INTDIR)\vcsssn.obj" \
	"..\..\..\10-common\lib\release\Win32\mtadp.lib" \
	"..\..\..\10-common\lib\release\win32\mcuagt.lib"

".\mcu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

!IF "$(RECURSE)" == "0" 

ALL : ".\mcu.exe"

!ELSE 

ALL : "mcuagtlib - Win32 Debug" "prjMtadpWin32 - Win32 Debug" ".\mcu.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"prjMtadpWin32 - Win32 DebugCLEAN" "mcuagtlib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dcsssn.obj"
	-@erase "$(INTDIR)\eqpssn.obj"
	-@erase "$(INTDIR)\mcsssn.obj"
	-@erase "$(INTDIR)\mcu.obj"
	-@erase "$(INTDIR)\mcucfg.obj"
	-@erase "$(INTDIR)\mcudata.obj"
	-@erase "$(INTDIR)\mcuguard.obj"
	-@erase "$(INTDIR)\mcuutility.obj"
	-@erase "$(INTDIR)\mcuvc.obj"
	-@erase "$(INTDIR)\mcuvcdcs.obj"
	-@erase "$(INTDIR)\mcuvceqp.obj"
	-@erase "$(INTDIR)\mcuvcmcs.obj"
	-@erase "$(INTDIR)\mcuvcmmcu.obj"
	-@erase "$(INTDIR)\mcuvcmt.obj"
	-@erase "$(INTDIR)\mcuvcnms.obj"
	-@erase "$(INTDIR)\mcuvcswitch.obj"
	-@erase "$(INTDIR)\mcuvcvcs.obj"
	-@erase "$(INTDIR)\modemssn.obj"
	-@erase "$(INTDIR)\mpmanager.obj"
	-@erase "$(INTDIR)\mpssn.obj"
	-@erase "$(INTDIR)\msmanagerssn.obj"
	-@erase "$(INTDIR)\mtadpssn.obj"
	-@erase "$(INTDIR)\mtssn.obj"
	-@erase "$(INTDIR)\nplusmanager.obj"
	-@erase "$(INTDIR)\udpdispatch.obj"
	-@erase "$(INTDIR)\udpnms.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vcsssn.obj"
	-@erase "$(OUTDIR)\mcu.pdb"
	-@erase ".\mcu.exe"
	-@erase ".\mcu.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\t120" /I "..\..\..\10-common\include\nms1" /I "..\..\..\10-common\include\cbb\license" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /D "_SATELITE_" /Fp"$(INTDIR)\prjmcu.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\prjmcu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=osplib.lib kdvsys.lib winbrdwrapper.lib mcuagt.lib dataswitch.lib mp.lib kdv323.lib kdv323adp.lib mtadp.lib kdvaddrbook.lib kdvdatanet.lib fcnet.lib enfc.lib kdvmedianet.lib mcuprs.lib kdvnetbuf.lib kdvencrypt.lib snmpadp_mcu.lib agent_pp.lib snmp_pp.lib kdvum.lib kdvcascade.lib readlicenselib.lib osplib.lib kdvsys.lib winbrdwrapper.lib mcuagt.lib dataswitch.lib mp.lib kdv323.lib kdv323adp.lib mtadp.lib kdvaddrbook.lib kdvdatanet.lib fcnet.lib enfc.lib kdvmedianet.lib mcuprs.lib kdvnetbuf.lib kdvencrypt.lib snmpadp_mcu.lib agent_pp.lib snmp_pp.lib kdvum.lib kdvcascade.lib readlicenselib.lib satmodem.lib /nologo /stack:0x7a1200 /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\mcu.pdb" /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /out:"mcu.exe" /pdbtype:sept /libpath:"..\..\..\10-common\lib\debug\win32" 
LINK32_OBJS= \
	"$(INTDIR)\dcsssn.obj" \
	"$(INTDIR)\eqpssn.obj" \
	"$(INTDIR)\mcsssn.obj" \
	"$(INTDIR)\mcu.obj" \
	"$(INTDIR)\mcucfg.obj" \
	"$(INTDIR)\mcudata.obj" \
	"$(INTDIR)\mcuguard.obj" \
	"$(INTDIR)\mcuutility.obj" \
	"$(INTDIR)\mcuvc.obj" \
	"$(INTDIR)\mcuvcdcs.obj" \
	"$(INTDIR)\mcuvceqp.obj" \
	"$(INTDIR)\mcuvcmcs.obj" \
	"$(INTDIR)\mcuvcmmcu.obj" \
	"$(INTDIR)\mcuvcmt.obj" \
	"$(INTDIR)\mcuvcnms.obj" \
	"$(INTDIR)\mcuvcswitch.obj" \
	"$(INTDIR)\mcuvcvcs.obj" \
	"$(INTDIR)\modemssn.obj" \
	"$(INTDIR)\mpmanager.obj" \
	"$(INTDIR)\mpssn.obj" \
	"$(INTDIR)\msmanagerssn.obj" \
	"$(INTDIR)\mtadpssn.obj" \
	"$(INTDIR)\mtssn.obj" \
	"$(INTDIR)\nplusmanager.obj" \
	"$(INTDIR)\udpdispatch.obj" \
	"$(INTDIR)\udpnms.obj" \
	"$(INTDIR)\vcsssn.obj" \
	"..\..\..\10-common\lib\debug\Win32\mtadp.lib" \
	"..\..\..\10-common\lib\debug\win32\mcuagt.lib"

".\mcu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"

OUTDIR=.\TestDebug
INTDIR=.\TestDebug

!IF "$(RECURSE)" == "0" 

ALL : ".\mcu.exe"

!ELSE 

ALL : ".\mcu.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dcsssn.obj"
	-@erase "$(INTDIR)\eqpssn.obj"
	-@erase "$(INTDIR)\mcsssn.obj"
	-@erase "$(INTDIR)\mcu.obj"
	-@erase "$(INTDIR)\mcucfg.obj"
	-@erase "$(INTDIR)\mcudata.obj"
	-@erase "$(INTDIR)\mcuguard.obj"
	-@erase "$(INTDIR)\mcuutility.obj"
	-@erase "$(INTDIR)\mcuvc.obj"
	-@erase "$(INTDIR)\mcuvcdcs.obj"
	-@erase "$(INTDIR)\mcuvceqp.obj"
	-@erase "$(INTDIR)\mcuvcmcs.obj"
	-@erase "$(INTDIR)\mcuvcmmcu.obj"
	-@erase "$(INTDIR)\mcuvcmt.obj"
	-@erase "$(INTDIR)\mcuvcnms.obj"
	-@erase "$(INTDIR)\mcuvcswitch.obj"
	-@erase "$(INTDIR)\mcuvcvcs.obj"
	-@erase "$(INTDIR)\modemssn.obj"
	-@erase "$(INTDIR)\mpmanager.obj"
	-@erase "$(INTDIR)\mpssn.obj"
	-@erase "$(INTDIR)\msmanagerssn.obj"
	-@erase "$(INTDIR)\mtadpssn.obj"
	-@erase "$(INTDIR)\mtssn.obj"
	-@erase "$(INTDIR)\nplusmanager.obj"
	-@erase "$(INTDIR)\udpdispatch.obj"
	-@erase "$(INTDIR)\udpnms.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vcsssn.obj"
	-@erase "$(OUTDIR)\mcu.pdb"
	-@erase ".\mcu.exe"
	-@erase ".\mcu.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\t120" /I "..\..\..\10-common\include\nms1" /I "..\..\..\10-common\include\cbb\license" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\prjmcu.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\prjmcu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=osplib.lib kdvsys.lib mcuagt.lib dataswitch.lib mp.lib mtadp.lib kdvaddrbook.lib kdvdatanet.lib fcnet.lib enfc.lib kdvmedianet.lib mcuprs.lib kdvnetbuf.lib kdvencrypt.lib snmpadp_mcu.lib agent_pp.lib snmp_pp.lib kdvum.lib kdvcascade.lib multimtsimu.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\mcu.pdb" /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /out:"mcu.exe" /pdbtype:sept /libpath:"..\..\..\10-common\lib\debug\win32" 
LINK32_OBJS= \
	"$(INTDIR)\dcsssn.obj" \
	"$(INTDIR)\eqpssn.obj" \
	"$(INTDIR)\mcsssn.obj" \
	"$(INTDIR)\mcu.obj" \
	"$(INTDIR)\mcucfg.obj" \
	"$(INTDIR)\mcudata.obj" \
	"$(INTDIR)\mcuguard.obj" \
	"$(INTDIR)\mcuutility.obj" \
	"$(INTDIR)\mcuvc.obj" \
	"$(INTDIR)\mcuvcdcs.obj" \
	"$(INTDIR)\mcuvceqp.obj" \
	"$(INTDIR)\mcuvcmcs.obj" \
	"$(INTDIR)\mcuvcmmcu.obj" \
	"$(INTDIR)\mcuvcmt.obj" \
	"$(INTDIR)\mcuvcnms.obj" \
	"$(INTDIR)\mcuvcswitch.obj" \
	"$(INTDIR)\mcuvcvcs.obj" \
	"$(INTDIR)\modemssn.obj" \
	"$(INTDIR)\mpmanager.obj" \
	"$(INTDIR)\mpssn.obj" \
	"$(INTDIR)\msmanagerssn.obj" \
	"$(INTDIR)\mtadpssn.obj" \
	"$(INTDIR)\mtssn.obj" \
	"$(INTDIR)\nplusmanager.obj" \
	"$(INTDIR)\udpdispatch.obj" \
	"$(INTDIR)\udpnms.obj" \
	"$(INTDIR)\vcsssn.obj"

".\mcu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"

OUTDIR=.\TestRelease
INTDIR=.\TestRelease
# Begin Custom Macros
OutDir=.\TestRelease
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : ".\mcu.exe" "$(OUTDIR)\prjmcu.bsc"

!ELSE 

ALL : ".\mcu.exe" "$(OUTDIR)\prjmcu.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\dcsssn.obj"
	-@erase "$(INTDIR)\dcsssn.sbr"
	-@erase "$(INTDIR)\eqpssn.obj"
	-@erase "$(INTDIR)\eqpssn.sbr"
	-@erase "$(INTDIR)\mcsssn.obj"
	-@erase "$(INTDIR)\mcsssn.sbr"
	-@erase "$(INTDIR)\mcu.obj"
	-@erase "$(INTDIR)\mcu.sbr"
	-@erase "$(INTDIR)\mcucfg.obj"
	-@erase "$(INTDIR)\mcucfg.sbr"
	-@erase "$(INTDIR)\mcudata.obj"
	-@erase "$(INTDIR)\mcudata.sbr"
	-@erase "$(INTDIR)\mcuguard.obj"
	-@erase "$(INTDIR)\mcuguard.sbr"
	-@erase "$(INTDIR)\mcuutility.obj"
	-@erase "$(INTDIR)\mcuutility.sbr"
	-@erase "$(INTDIR)\mcuvc.obj"
	-@erase "$(INTDIR)\mcuvc.sbr"
	-@erase "$(INTDIR)\mcuvcdcs.obj"
	-@erase "$(INTDIR)\mcuvcdcs.sbr"
	-@erase "$(INTDIR)\mcuvceqp.obj"
	-@erase "$(INTDIR)\mcuvceqp.sbr"
	-@erase "$(INTDIR)\mcuvcmcs.obj"
	-@erase "$(INTDIR)\mcuvcmcs.sbr"
	-@erase "$(INTDIR)\mcuvcmmcu.obj"
	-@erase "$(INTDIR)\mcuvcmmcu.sbr"
	-@erase "$(INTDIR)\mcuvcmt.obj"
	-@erase "$(INTDIR)\mcuvcmt.sbr"
	-@erase "$(INTDIR)\mcuvcnms.obj"
	-@erase "$(INTDIR)\mcuvcnms.sbr"
	-@erase "$(INTDIR)\mcuvcswitch.obj"
	-@erase "$(INTDIR)\mcuvcswitch.sbr"
	-@erase "$(INTDIR)\mcuvcvcs.obj"
	-@erase "$(INTDIR)\mcuvcvcs.sbr"
	-@erase "$(INTDIR)\modemssn.obj"
	-@erase "$(INTDIR)\modemssn.sbr"
	-@erase "$(INTDIR)\mpmanager.obj"
	-@erase "$(INTDIR)\mpmanager.sbr"
	-@erase "$(INTDIR)\mpssn.obj"
	-@erase "$(INTDIR)\mpssn.sbr"
	-@erase "$(INTDIR)\msmanagerssn.obj"
	-@erase "$(INTDIR)\msmanagerssn.sbr"
	-@erase "$(INTDIR)\mtadpssn.obj"
	-@erase "$(INTDIR)\mtadpssn.sbr"
	-@erase "$(INTDIR)\mtssn.obj"
	-@erase "$(INTDIR)\mtssn.sbr"
	-@erase "$(INTDIR)\nplusmanager.obj"
	-@erase "$(INTDIR)\nplusmanager.sbr"
	-@erase "$(INTDIR)\udpdispatch.obj"
	-@erase "$(INTDIR)\udpdispatch.sbr"
	-@erase "$(INTDIR)\udpnms.obj"
	-@erase "$(INTDIR)\udpnms.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vcsssn.obj"
	-@erase "$(INTDIR)\vcsssn.sbr"
	-@erase "$(OUTDIR)\prjmcu.bsc"
	-@erase ".\mcu.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\t120" /I "..\..\..\10-common\include\nms1" /I "..\..\..\10-common\include\cbb\license" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\prjmcu.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\prjmcu.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\dcsssn.sbr" \
	"$(INTDIR)\eqpssn.sbr" \
	"$(INTDIR)\mcsssn.sbr" \
	"$(INTDIR)\mcu.sbr" \
	"$(INTDIR)\mcucfg.sbr" \
	"$(INTDIR)\mcudata.sbr" \
	"$(INTDIR)\mcuguard.sbr" \
	"$(INTDIR)\mcuutility.sbr" \
	"$(INTDIR)\mcuvc.sbr" \
	"$(INTDIR)\mcuvcdcs.sbr" \
	"$(INTDIR)\mcuvceqp.sbr" \
	"$(INTDIR)\mcuvcmcs.sbr" \
	"$(INTDIR)\mcuvcmmcu.sbr" \
	"$(INTDIR)\mcuvcmt.sbr" \
	"$(INTDIR)\mcuvcnms.sbr" \
	"$(INTDIR)\mcuvcswitch.sbr" \
	"$(INTDIR)\mcuvcvcs.sbr" \
	"$(INTDIR)\modemssn.sbr" \
	"$(INTDIR)\mpmanager.sbr" \
	"$(INTDIR)\mpssn.sbr" \
	"$(INTDIR)\msmanagerssn.sbr" \
	"$(INTDIR)\mtadpssn.sbr" \
	"$(INTDIR)\mtssn.sbr" \
	"$(INTDIR)\nplusmanager.sbr" \
	"$(INTDIR)\udpdispatch.sbr" \
	"$(INTDIR)\udpnms.sbr" \
	"$(INTDIR)\vcsssn.sbr"

"$(OUTDIR)\prjmcu.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=osplib.lib kdvsys.lib mcuagt.lib dataswitch.lib mp.lib mtadp.lib kdvaddrbook.lib kdvdatanet.lib fcnet.lib enfc.lib kdvmedianet.lib mcuprs.lib kdvnetbuf.lib kdvencrypt.lib snmpadp_mcu.lib agent_pp.lib snmp_pp.lib kdvum.lib kdvcascade.lib multimtsimu.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\mcu.pdb" /machine:I386 /nodefaultlib:"nafxcw.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /out:"mcu.exe" /libpath:"..\..\..\10-common\lib\release\win32" 
LINK32_OBJS= \
	"$(INTDIR)\dcsssn.obj" \
	"$(INTDIR)\eqpssn.obj" \
	"$(INTDIR)\mcsssn.obj" \
	"$(INTDIR)\mcu.obj" \
	"$(INTDIR)\mcucfg.obj" \
	"$(INTDIR)\mcudata.obj" \
	"$(INTDIR)\mcuguard.obj" \
	"$(INTDIR)\mcuutility.obj" \
	"$(INTDIR)\mcuvc.obj" \
	"$(INTDIR)\mcuvcdcs.obj" \
	"$(INTDIR)\mcuvceqp.obj" \
	"$(INTDIR)\mcuvcmcs.obj" \
	"$(INTDIR)\mcuvcmmcu.obj" \
	"$(INTDIR)\mcuvcmt.obj" \
	"$(INTDIR)\mcuvcnms.obj" \
	"$(INTDIR)\mcuvcswitch.obj" \
	"$(INTDIR)\mcuvcvcs.obj" \
	"$(INTDIR)\modemssn.obj" \
	"$(INTDIR)\mpmanager.obj" \
	"$(INTDIR)\mpssn.obj" \
	"$(INTDIR)\msmanagerssn.obj" \
	"$(INTDIR)\mtadpssn.obj" \
	"$(INTDIR)\mtssn.obj" \
	"$(INTDIR)\nplusmanager.obj" \
	"$(INTDIR)\udpdispatch.obj" \
	"$(INTDIR)\udpnms.obj" \
	"$(INTDIR)\vcsssn.obj"

".\mcu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("prjmcu.dep")
!INCLUDE "prjmcu.dep"
!ELSE 
!MESSAGE Warning: cannot find "prjmcu.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "prjMcu - Win32 Release" || "$(CFG)" == "prjMcu - Win32 Debug" || "$(CFG)" == "prjMcu - Win32 TestDebug" || "$(CFG)" == "prjMcu - Win32 TestRelease"
SOURCE=..\source\dcsssn.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\dcsssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\dcsssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\dcsssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\dcsssn.obj"	"$(INTDIR)\dcsssn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\eqpssn.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\eqpssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\eqpssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\eqpssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\eqpssn.obj"	"$(INTDIR)\eqpssn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcsssn.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mcsssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mcsssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mcsssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mcsssn.obj"	"$(INTDIR)\mcsssn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcu.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mcu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mcu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mcu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mcu.obj"	"$(INTDIR)\mcu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcucfg.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mcucfg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mcucfg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mcucfg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mcucfg.obj"	"$(INTDIR)\mcucfg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcudata.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mcudata.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mcudata.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mcudata.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mcudata.obj"	"$(INTDIR)\mcudata.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcuguard.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mcuguard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mcuguard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mcuguard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mcuguard.obj"	"$(INTDIR)\mcuguard.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcuutility.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mcuutility.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mcuutility.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mcuutility.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mcuutility.obj"	"$(INTDIR)\mcuutility.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcuvc.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mcuvc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mcuvc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mcuvc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mcuvc.obj"	"$(INTDIR)\mcuvc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcuvcdcs.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mcuvcdcs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mcuvcdcs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mcuvcdcs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mcuvcdcs.obj"	"$(INTDIR)\mcuvcdcs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcuvceqp.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mcuvceqp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mcuvceqp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mcuvceqp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mcuvceqp.obj"	"$(INTDIR)\mcuvceqp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcuvcmcs.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mcuvcmcs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mcuvcmcs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mcuvcmcs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mcuvcmcs.obj"	"$(INTDIR)\mcuvcmcs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcuvcmmcu.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mcuvcmmcu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mcuvcmmcu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mcuvcmmcu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mcuvcmmcu.obj"	"$(INTDIR)\mcuvcmmcu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcuvcmt.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mcuvcmt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mcuvcmt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mcuvcmt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mcuvcmt.obj"	"$(INTDIR)\mcuvcmt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcuvcnms.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mcuvcnms.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mcuvcnms.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mcuvcnms.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mcuvcnms.obj"	"$(INTDIR)\mcuvcnms.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcuvcswitch.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mcuvcswitch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mcuvcswitch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mcuvcswitch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mcuvcswitch.obj"	"$(INTDIR)\mcuvcswitch.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcuvcvcs.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mcuvcvcs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mcuvcvcs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mcuvcvcs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mcuvcvcs.obj"	"$(INTDIR)\mcuvcvcs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\modemssn.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\modemssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\modemssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\modemssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\modemssn.obj"	"$(INTDIR)\modemssn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mpmanager.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mpmanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mpmanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mpmanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mpmanager.obj"	"$(INTDIR)\mpmanager.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mpssn.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mpssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mpssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mpssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mpssn.obj"	"$(INTDIR)\mpssn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\msmanagerssn.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\msmanagerssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\msmanagerssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\msmanagerssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\msmanagerssn.obj"	"$(INTDIR)\msmanagerssn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mtadpssn.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mtadpssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mtadpssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mtadpssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mtadpssn.obj"	"$(INTDIR)\mtadpssn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mtssn.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\mtssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\mtssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\mtssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\mtssn.obj"	"$(INTDIR)\mtssn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\nplusmanager.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\nplusmanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\nplusmanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\nplusmanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\nplusmanager.obj"	"$(INTDIR)\nplusmanager.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\udpdispatch.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\udpdispatch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\udpdispatch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\udpdispatch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\udpdispatch.obj"	"$(INTDIR)\udpdispatch.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\udpnms.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\udpnms.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\udpnms.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\udpnms.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\udpnms.obj"	"$(INTDIR)\udpnms.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\vcsssn.cpp

!IF  "$(CFG)" == "prjMcu - Win32 Release"


"$(INTDIR)\vcsssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"


"$(INTDIR)\vcsssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"


"$(INTDIR)\vcsssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"


"$(INTDIR)\vcsssn.obj"	"$(INTDIR)\vcsssn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

!IF  "$(CFG)" == "prjMcu - Win32 Release"

"prjMtadpWin32 - Win32 Release" : 
   cd "\ccroot\zhangbaoqing_Professional\Professional_VOB\60-mercury\mtadplib_satelite\prj_win32"
   $(MAKE) /$(MAKEFLAGS) /F ".\prjmtadpwin32_sat.mak" CFG="prjMtadpWin32 - Win32 Release" 
   cd "..\..\mcu_satelite\prj_win32"

"prjMtadpWin32 - Win32 ReleaseCLEAN" : 
   cd "\ccroot\zhangbaoqing_Professional\Professional_VOB\60-mercury\mtadplib_satelite\prj_win32"
   $(MAKE) /$(MAKEFLAGS) /F ".\prjmtadpwin32_sat.mak" CFG="prjMtadpWin32 - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\mcu_satelite\prj_win32"

!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"

"prjMtadpWin32 - Win32 Debug" : 
   cd "\ccroot\zhangbaoqing_Professional\Professional_VOB\60-mercury\mtadplib_satelite\prj_win32"
   $(MAKE) /$(MAKEFLAGS) /F ".\prjmtadpwin32_sat.mak" CFG="prjMtadpWin32 - Win32 Debug" 
   cd "..\..\mcu_satelite\prj_win32"

"prjMtadpWin32 - Win32 DebugCLEAN" : 
   cd "\ccroot\zhangbaoqing_Professional\Professional_VOB\60-mercury\mtadplib_satelite\prj_win32"
   $(MAKE) /$(MAKEFLAGS) /F ".\prjmtadpwin32_sat.mak" CFG="prjMtadpWin32 - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\mcu_satelite\prj_win32"

!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"

!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"

!ENDIF 

!IF  "$(CFG)" == "prjMcu - Win32 Release"

"mcuagtlib - Win32 Release" : 
   cd "\ccroot\zhangbaoqing_Professional\Professional_VOB\60-mercury\mcuagtlib\prj_win32"
   $(MAKE) /$(MAKEFLAGS) /F ".\mcuagtlib.mak" CFG="mcuagtlib - Win32 Release" 
   cd "..\..\mcu_satelite\prj_win32"

"mcuagtlib - Win32 ReleaseCLEAN" : 
   cd "\ccroot\zhangbaoqing_Professional\Professional_VOB\60-mercury\mcuagtlib\prj_win32"
   $(MAKE) /$(MAKEFLAGS) /F ".\mcuagtlib.mak" CFG="mcuagtlib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\mcu_satelite\prj_win32"

!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"

"mcuagtlib - Win32 Debug" : 
   cd "\ccroot\zhangbaoqing_Professional\Professional_VOB\60-mercury\mcuagtlib\prj_win32"
   $(MAKE) /$(MAKEFLAGS) /F ".\mcuagtlib.mak" CFG="mcuagtlib - Win32 Debug" 
   cd "..\..\mcu_satelite\prj_win32"

"mcuagtlib - Win32 DebugCLEAN" : 
   cd "\ccroot\zhangbaoqing_Professional\Professional_VOB\60-mercury\mcuagtlib\prj_win32"
   $(MAKE) /$(MAKEFLAGS) /F ".\mcuagtlib.mak" CFG="mcuagtlib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\mcu_satelite\prj_win32"

!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestDebug"

!ELSEIF  "$(CFG)" == "prjMcu - Win32 TestRelease"

!ENDIF 


!ENDIF 

