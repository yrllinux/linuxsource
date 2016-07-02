# Microsoft Developer Studio Generated NMAKE File, Based on eqpsimu.dsp
!IF "$(CFG)" == ""
CFG=eqpsimu - Win32 Debug
!MESSAGE No configuration specified. Defaulting to eqpsimu - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "eqpsimu - Win32 Release" && "$(CFG)" != "eqpsimu - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "eqpsimu.mak" CFG="eqpsimu - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "eqpsimu - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "eqpsimu - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "eqpsimu - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\eqpsimu.exe"


CLEAN :
	-@erase "$(INTDIR)\eqpsimu.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\eqpsimu.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\eqpsimu.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\eqpsimu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=OspLib.lib kdvsys.lib winbrdwrapper.lib basicboardagent.lib mmpagent.lib audmixlib.lib bas.lib mcuprs.lib tvwall.lib vmp.lib mpwlib.lib mcueqpsimu.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\eqpsimu.pdb" /machine:I386 /nodefaultlib:"mfc42d.lib mfcs42d.lib" /out:"$(OUTDIR)\eqpsimu.exe" /libpath:"..\..\..\10-Common\lib\release\win32" 
LINK32_OBJS= \
	"$(INTDIR)\eqpsimu.obj"

"$(OUTDIR)\eqpsimu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "eqpsimu - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\eqpsimu.exe"


CLEAN :
	-@erase "$(INTDIR)\eqpsimu.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\eqpsimu.exe"
	-@erase "$(OUTDIR)\eqpsimu.ilk"
	-@erase "$(OUTDIR)\eqpsimu.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\eqpsimu.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\eqpsimu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=OspLib.lib kdvsys.lib winbrdwrapper.lib basicboardagent.lib mmpagent.lib audmixlib.lib bas.lib mcuprs.lib tvwall.lib vmp.lib mpwlib.lib mcueqpsimu.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\eqpsimu.pdb" /debug /machine:I386 /out:"$(OUTDIR)\eqpsimu.exe" /pdbtype:sept /libpath:"..\..\..\10-Common\lib\debug\win32" 
LINK32_OBJS= \
	"$(INTDIR)\eqpsimu.obj"

"$(OUTDIR)\eqpsimu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("eqpsimu.dep")
!INCLUDE "eqpsimu.dep"
!ELSE 
!MESSAGE Warning: cannot find "eqpsimu.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "eqpsimu - Win32 Release" || "$(CFG)" == "eqpsimu - Win32 Debug"
SOURCE=..\source\eqpsimu.cpp

"$(INTDIR)\eqpsimu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

