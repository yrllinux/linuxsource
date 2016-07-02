# Microsoft Developer Studio Generated NMAKE File, Based on prjmcu.dsp
!IF "$(CFG)" == ""
CFG=prjMcu - Win32 Debug
!MESSAGE No configuration specified. Defaulting to prjMcu - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "prjMcu - Win32 Release" && "$(CFG)" != "prjMcu - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "prjmcu.mak" CFG="prjMcu - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "prjMcu - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "prjMcu - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "prjMcu - Win32 Release"

OUTDIR=.\release
INTDIR=.\release

ALL : ".\mcu_8000b.exe"


CLEAN :
	-@erase "$(INTDIR)\dcsssn.obj"
	-@erase "$(INTDIR)\eqpbase.obj"
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
	-@erase "$(INTDIR)\mcuvcswitch.obj"
	-@erase "$(INTDIR)\mcuvcvcs.obj"
	-@erase "$(INTDIR)\mpmanager.obj"
	-@erase "$(INTDIR)\mpssn.obj"
	-@erase "$(INTDIR)\msmanagerssn.obj"
	-@erase "$(INTDIR)\mtadpssn.obj"
	-@erase "$(INTDIR)\nplusmanager.obj"
	-@erase "$(INTDIR)\tvwall.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vcsssn.obj"
	-@erase ".\mcu_8000b.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\t120" /I "..\..\..\10-common\include\nms1" /I "..\..\..\10-common\include\cbb\license" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "_MINIMCU_" /D "_UTF8" /Fp"$(INTDIR)\prjmcu.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\prjmcu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=lanman.lib kdvlog.lib kdv323.lib kdv323adp.lib readlicenselib.lib osplib.lib kdvsys.lib winbrdwrapper.lib mcuagtmt.lib dataswitch.lib mp.lib mtadp.lib kdvaddrbook.lib kdvdatanet.lib fcnet.lib enfc.lib kdvmedianet.lib mcuprs.lib kdvnetbuf.lib kdvencrypt.lib snmpadp_mcu.lib agent_pp.lib snmp_pp.lib kdvum.lib kdvcascade.lib audmixlib.lib vmp.lib bas.lib mcueqpsimu.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\mcu_8000b.pdb" /machine:I386 /nodefaultlib:"nafxcw.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /out:"mcu_8000b.exe" /libpath:"..\..\..\10-common\lib\release\win32" 
LINK32_OBJS= \
	"$(INTDIR)\eqpbase.obj" \
	"$(INTDIR)\tvwall.obj" \
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
	"$(INTDIR)\mcuvcswitch.obj" \
	"$(INTDIR)\mcuvcvcs.obj" \
	"$(INTDIR)\mpmanager.obj" \
	"$(INTDIR)\mpssn.obj" \
	"$(INTDIR)\msmanagerssn.obj" \
	"$(INTDIR)\mtadpssn.obj" \
	"$(INTDIR)\nplusmanager.obj" \
	"$(INTDIR)\vcsssn.obj"

".\mcu_8000b.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "prjMcu - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : ".\mcu_8000b.exe"


CLEAN :
	-@erase "$(INTDIR)\dcsssn.obj"
	-@erase "$(INTDIR)\eqpbase.obj"
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
	-@erase "$(INTDIR)\mcuvcswitch.obj"
	-@erase "$(INTDIR)\mcuvcvcs.obj"
	-@erase "$(INTDIR)\mpmanager.obj"
	-@erase "$(INTDIR)\mpssn.obj"
	-@erase "$(INTDIR)\msmanagerssn.obj"
	-@erase "$(INTDIR)\mtadpssn.obj"
	-@erase "$(INTDIR)\nplusmanager.obj"
	-@erase "$(INTDIR)\tvwall.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vcsssn.obj"
	-@erase "$(OUTDIR)\mcu_8000b.pdb"
	-@erase ".\mcu_8000b.exe"
	-@erase ".\mcu_8000b.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\t120" /I "..\..\..\10-common\include\nms1" /I "..\..\..\10-common\include\cbb\license" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /D "_MINIMCU_" /D "_UTF8" /Fp"$(INTDIR)\prjmcu.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\prjmcu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=lanman.lib kdvlog.lib kdv323.lib kdv323adp.lib readlicenselib.lib osplib.lib kdvsys.lib winbrdwrapper.lib mcuagtmt.lib dataswitch.lib mp.lib mtadp.lib kdvaddrbook.lib kdvdatanet.lib fcnet.lib enfc.lib kdvmedianet.lib mcuprs.lib kdvnetbuf.lib kdvencrypt.lib snmpadp_mcu.lib agent_pp.lib snmp_pp.lib kdvum.lib kdvcascade.lib audmixlib.lib vmp.lib bas.lib mcueqpsimu.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\mcu_8000b.pdb" /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /out:"mcu_8000b.exe" /pdbtype:sept /libpath:"..\..\..\10-common\lib\debug\win32" 
LINK32_OBJS= \
	"$(INTDIR)\eqpbase.obj" \
	"$(INTDIR)\tvwall.obj" \
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
	"$(INTDIR)\mcuvcswitch.obj" \
	"$(INTDIR)\mcuvcvcs.obj" \
	"$(INTDIR)\mpmanager.obj" \
	"$(INTDIR)\mpssn.obj" \
	"$(INTDIR)\msmanagerssn.obj" \
	"$(INTDIR)\mtadpssn.obj" \
	"$(INTDIR)\nplusmanager.obj" \
	"$(INTDIR)\vcsssn.obj"

".\mcu_8000b.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

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


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("prjmcu.dep")
!INCLUDE "prjmcu.dep"
!ELSE 
!MESSAGE Warning: cannot find "prjmcu.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "prjMcu - Win32 Release" || "$(CFG)" == "prjMcu - Win32 Debug"
SOURCE=..\source\eqpbase.cpp

"$(INTDIR)\eqpbase.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\tvwall.cpp

"$(INTDIR)\tvwall.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\dcsssn.cpp

"$(INTDIR)\dcsssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\eqpssn.cpp

"$(INTDIR)\eqpssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mcsssn.cpp

"$(INTDIR)\mcsssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mcu.cpp

"$(INTDIR)\mcu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mcucfg.cpp

"$(INTDIR)\mcucfg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mcudata.cpp

"$(INTDIR)\mcudata.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mcuguard.cpp

"$(INTDIR)\mcuguard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mcuutility.cpp

"$(INTDIR)\mcuutility.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mcuvc.cpp

"$(INTDIR)\mcuvc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mcuvcdcs.cpp

"$(INTDIR)\mcuvcdcs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mcuvceqp.cpp

"$(INTDIR)\mcuvceqp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mcuvcmcs.cpp

"$(INTDIR)\mcuvcmcs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mcuvcmmcu.cpp

"$(INTDIR)\mcuvcmmcu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mcuvcmt.cpp

"$(INTDIR)\mcuvcmt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mcuvcswitch.cpp

"$(INTDIR)\mcuvcswitch.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mcuvcvcs.cpp

"$(INTDIR)\mcuvcvcs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpmanager.cpp

"$(INTDIR)\mpmanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpssn.cpp

"$(INTDIR)\mpssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\msmanagerssn.cpp

"$(INTDIR)\msmanagerssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mtadpssn.cpp

"$(INTDIR)\mtadpssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\nplusmanager.cpp

"$(INTDIR)\nplusmanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\vcsssn.cpp

"$(INTDIR)\vcsssn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

