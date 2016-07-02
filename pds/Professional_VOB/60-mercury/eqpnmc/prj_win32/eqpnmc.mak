# Microsoft Developer Studio Generated NMAKE File, Based on eqpnmc.dsp
!IF "$(CFG)" == ""
CFG=eqpnmc - Win32 Debug
!MESSAGE No configuration specified. Defaulting to eqpnmc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "eqpnmc - Win32 Release" && "$(CFG)" != "eqpnmc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "eqpnmc.mak" CFG="eqpnmc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "eqpnmc - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "eqpnmc - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "eqpnmc - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\eqpnmc.exe"


CLEAN :
	-@erase "$(INTDIR)\eqpagt.obj"
	-@erase "$(INTDIR)\eqpnmc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\eqpnmc.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include\\" /I "..\..\common\include\\" /I "..\..\..\10-common\include\platform\\" /I "..\..\..\10-common\include\system1\\" /I "..\..\..\60-mercury\eqpagt\eqpagtsysinfolib\include\\" /I "..\..\..\60-mercury\eqpagt\eqpagtcommbaselib\include\\" /I "..\..\..\10-common\include\snmp\\" /I "..\..\..\10-common\include\protocol\\" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\eqpnmc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\eqpnmc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=osplib.lib kdvsys.lib kdvlog.lib snmp_pp.lib agent_pp.lib snmpadp_rcd.lib eqpagtcommbase.lib eqpagtsysinfo.lib /nologo /entry:"mainCRTStartup" /subsystem:console /incremental:no /pdb:"$(OUTDIR)\eqpnmc.pdb" /machine:I386 /nodefaultlib:"nafxcw.lib" /nodefaultlib:"libc.lib" /nodefaultlib:"libcmt.lib" /nodefaultlib:"mfc42d.lib" /nodefaultlib:"mfcs42d.lib" /nodefaultlib:"msvcrtd.lib" /out:"$(OUTDIR)\eqpnmc.exe" /libpath:"..\..\..\10-common\lib\release\win32" /subsystem:"windows" 
LINK32_OBJS= \
	"$(INTDIR)\eqpagt.obj" \
	"$(INTDIR)\eqpnmc.obj"

"$(OUTDIR)\eqpnmc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "eqpnmc - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\eqpnmc.exe"


CLEAN :
	-@erase "$(INTDIR)\eqpagt.obj"
	-@erase "$(INTDIR)\eqpnmc.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\eqpnmc.exe"
	-@erase "$(OUTDIR)\eqpnmc.ilk"
	-@erase "$(OUTDIR)\eqpnmc.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\include\\" /I "..\..\common\include\\" /I "..\..\..\10-common\include\platform\\" /I "..\..\..\10-common\include\system1\\" /I "..\..\..\60-mercury\eqpagt\eqpagtsysinfolib\include\\" /I "..\..\..\60-mercury\eqpagt\eqpagtcommbaselib\include\\" /I "..\..\..\10-common\include\snmp\\" /I "..\..\..\10-common\include\protocol\\" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\eqpnmc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\eqpnmc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=osplib.lib kdvsys.lib kdvlog.lib snmp_pp.lib agent_pp.lib snmpadp_rcd.lib eqpagtcommbase.lib eqpagtsysinfo.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\eqpnmc.pdb" /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /out:"$(OUTDIR)\eqpnmc.exe" /pdbtype:sept /libpath:"..\..\..\10-common\lib\debug\win32" 
LINK32_OBJS= \
	"$(INTDIR)\eqpagt.obj" \
	"$(INTDIR)\eqpnmc.obj"

"$(OUTDIR)\eqpnmc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("eqpnmc.dep")
!INCLUDE "eqpnmc.dep"
!ELSE 
!MESSAGE Warning: cannot find "eqpnmc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "eqpnmc - Win32 Release" || "$(CFG)" == "eqpnmc - Win32 Debug"
SOURCE=..\source\eqpagt.cpp

"$(INTDIR)\eqpagt.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\eqpnmc.cpp

"$(INTDIR)\eqpnmc.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

