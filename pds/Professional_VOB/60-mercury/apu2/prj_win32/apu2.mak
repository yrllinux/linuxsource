# Microsoft Developer Studio Generated NMAKE File, Based on apu2.dsp
!IF "$(CFG)" == ""
CFG=apu2 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to apu2 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "apu2 - Win32 Release" && "$(CFG)" != "apu2 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "apu2.mak" CFG="apu2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "apu2 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "apu2 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "apu2 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\apu2.exe"


CLEAN :
	-@erase "$(INTDIR)\apu2.obj"
	-@erase "$(INTDIR)\apu2agent.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\apu2.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\..\60-mercury\apu2lib\include" /I "..\..\..\60-mercury\eqpbaselib\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\apu2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\apu2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=apu2lib.lib eqpbaselib.lib mcueqpsimu.lib kdvlog.lib kdvmedianet.lib OspLib.lib kdvsys.lib winbrdwrapper.lib basicboardagent.lib fcnet.lib enfc.lib kdvencrypt.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\apu2.pdb" /machine:I386 /out:"$(OUTDIR)\apu2.exe" /libpath:"..\..\..\10-common\lib\release\win32" 
LINK32_OBJS= \
	"$(INTDIR)\apu2.obj" \
	"$(INTDIR)\apu2agent.obj"

"$(OUTDIR)\apu2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "apu2 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\apu2.exe" "$(OUTDIR)\apu2.bsc"


CLEAN :
	-@erase "$(INTDIR)\apu2.obj"
	-@erase "$(INTDIR)\apu2.sbr"
	-@erase "$(INTDIR)\apu2agent.obj"
	-@erase "$(INTDIR)\apu2agent.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\apu2.bsc"
	-@erase "$(OUTDIR)\apu2.exe"
	-@erase "$(OUTDIR)\apu2.ilk"
	-@erase "$(OUTDIR)\apu2.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\..\60-mercury\apu2lib\include" /I "..\..\..\60-mercury\eqpbaselib\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\apu2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\apu2.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\apu2.sbr" \
	"$(INTDIR)\apu2agent.sbr"

"$(OUTDIR)\apu2.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=mcueqpsimu.lib apu2lib.lib eqpbaselib.lib kdvlog.lib kdvmedianet.lib OspLib.lib kdvsys.lib winbrdwrapper.lib basicboardagent.lib fcnet.lib enfc.lib kdvencrypt.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\apu2.pdb" /debug /machine:I386 /out:"$(OUTDIR)\apu2.exe" /pdbtype:sept /libpath:"..\..\..\10-common\lib\debug\win32" 
LINK32_OBJS= \
	"$(INTDIR)\apu2.obj" \
	"$(INTDIR)\apu2agent.obj"

"$(OUTDIR)\apu2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("apu2.dep")
!INCLUDE "apu2.dep"
!ELSE 
!MESSAGE Warning: cannot find "apu2.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "apu2 - Win32 Release" || "$(CFG)" == "apu2 - Win32 Debug"
SOURCE=..\source\apu2.cpp

!IF  "$(CFG)" == "apu2 - Win32 Release"


"$(INTDIR)\apu2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "apu2 - Win32 Debug"


"$(INTDIR)\apu2.obj"	"$(INTDIR)\apu2.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\apu2agent.cpp

!IF  "$(CFG)" == "apu2 - Win32 Release"


"$(INTDIR)\apu2agent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "apu2 - Win32 Debug"


"$(INTDIR)\apu2agent.obj"	"$(INTDIR)\apu2agent.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

