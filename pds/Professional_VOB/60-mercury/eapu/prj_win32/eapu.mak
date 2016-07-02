# Microsoft Developer Studio Generated NMAKE File, Based on eapu.dsp
!IF "$(CFG)" == ""
CFG=eapu - Win32 Debug
!MESSAGE No configuration specified. Defaulting to eapu - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "eapu - Win32 Release" && "$(CFG)" != "eapu - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "eapu.mak" CFG="eapu - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "eapu - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "eapu - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "eapu - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\eapu.exe"

!ELSE 

ALL : "$(OUTDIR)\eapu.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\eapu.obj"
	-@erase "$(INTDIR)\eapuagent.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\eapu.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\eapu.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\eapu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kdvlog.lib OspLib.lib kdvsys.lib winbrdwrapper.lib basicboardagent.lib mmpagent.lib audmixlib.lib bas.lib mcuprs.lib tvwall.lib vmp.lib mpwlib.lib mcueapu.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\eapu.pdb" /machine:I386 /nodefaultlib:"mfc42d.lib mfcs42d.lib" /out:"$(OUTDIR)\eapu.exe" /libpath:"..\..\..\10-Common\lib\release\win32" 
LINK32_OBJS= \
	"$(INTDIR)\eapu.obj" \
	"$(INTDIR)\eapuagent.obj"

"$(OUTDIR)\eapu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "eapu - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\eapu.exe"

!ELSE 

ALL : "$(OUTDIR)\eapu.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\eapu.obj"
	-@erase "$(INTDIR)\eapuagent.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\eapu.exe"
	-@erase "$(OUTDIR)\eapu.ilk"
	-@erase "$(OUTDIR)\eapu.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\..\60-mercury\hdu\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\eapu.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\eapu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kdvlog.lib kdvmedianet.lib OspLib.lib kdvsys.lib winbrdwrapper.lib basicboardagent.lib fcnet.lib enfc.lib kdvencrypt.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\eapu.pdb" /debug /machine:I386 /out:"$(OUTDIR)\eapu.exe" /pdbtype:sept /libpath:"..\..\..\10-Common\lib\debug\win32" 
LINK32_OBJS= \
	"$(INTDIR)\eapu.obj" \
	"$(INTDIR)\eapuagent.obj"

"$(OUTDIR)\eapu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("eapu.dep")
!INCLUDE "eapu.dep"
!ELSE 
!MESSAGE Warning: cannot find "eapu.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "eapu - Win32 Release" || "$(CFG)" == "eapu - Win32 Debug"
SOURCE=..\source\eapu.cpp

"$(INTDIR)\eapu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\eapuagent.cpp

"$(INTDIR)\eapuagent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!IF  "$(CFG)" == "eapu - Win32 Release"

!ELSEIF  "$(CFG)" == "eapu - Win32 Debug"

!ENDIF 

!IF  "$(CFG)" == "eapu - Win32 Release"

!ELSEIF  "$(CFG)" == "eapu - Win32 Debug"

!ENDIF 

!IF  "$(CFG)" == "eapu - Win32 Release"

!ELSEIF  "$(CFG)" == "eapu - Win32 Debug"

!ENDIF 


!ENDIF 

