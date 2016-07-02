# Microsoft Developer Studio Generated NMAKE File, Based on prj_hdu.dsp
!IF "$(CFG)" == ""
CFG=prj_hdu - Win32 Debug
!MESSAGE No configuration specified. Defaulting to prj_hdu - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "prj_hdu - Win32 Release" && "$(CFG)" != "prj_hdu - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "prj_hdu.mak" CFG="prj_hdu - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "prj_hdu - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "prj_hdu - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "prj_hdu - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\prj_hdu.exe"


CLEAN :
	-@erase "$(INTDIR)\hdu.obj"
	-@erase "$(INTDIR)\hduagent.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\prj_hdu.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\include" /I "..\..\common\include" /I "..\..\..\10-Common\include\platform" /I "..\..\..\10-Common\include\mcu" /I "..\..\..\10-Common\include\system1" /I "..\..\..\10-Common\include\protocol" /I "..\..\hdulib\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\prj_hdu.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\prj_hdu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=hdulib.lib basicboardagent.lib osplib.lib kdvsys.lib winbrdwrapper.lib mcueqpsimu.lib kdvmedianet.lib fcnet.lib enfc.lib kdvencrypt.lib msvcrt.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\prj_hdu.pdb" /machine:I386 /out:"$(OUTDIR)\prj_hdu.exe" /libpath:"..\..\..\10-common\lib\release\win32" 
LINK32_OBJS= \
	"$(INTDIR)\hdu.obj" \
	"$(INTDIR)\hduagent.obj"

"$(OUTDIR)\prj_hdu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "prj_hdu - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\prj_hdu.exe"


CLEAN :
	-@erase "$(INTDIR)\hdu.obj"
	-@erase "$(INTDIR)\hduagent.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\prj_hdu.exe"
	-@erase "$(OUTDIR)\prj_hdu.ilk"
	-@erase "$(OUTDIR)\prj_hdu.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-Common\include\platform" /I "..\..\..\10-Common\include\mcu" /I "..\..\..\10-Common\include\system1" /I "..\..\..\10-Common\include\protocol" /I "..\..\hdulib\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\prj_hdu.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\prj_hdu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kdvlog.lib hdulib.lib basicboardagent.lib osplib.lib kdvsys.lib winbrdwrapper.lib mcueqpsimu.lib kdvmedianet.lib fcnet.lib enfc.lib kdvencrypt.lib msvcrt.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\prj_hdu.pdb" /debug /machine:I386 /out:"$(OUTDIR)\prj_hdu.exe" /pdbtype:sept /libpath:"..\..\..\10-common\lib\debug\win32" 
LINK32_OBJS= \
	"$(INTDIR)\hdu.obj" \
	"$(INTDIR)\hduagent.obj"

"$(OUTDIR)\prj_hdu.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("prj_hdu.dep")
!INCLUDE "prj_hdu.dep"
!ELSE 
!MESSAGE Warning: cannot find "prj_hdu.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "prj_hdu - Win32 Release" || "$(CFG)" == "prj_hdu - Win32 Debug"
SOURCE=..\source\hdu.cpp

"$(INTDIR)\hdu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\hduagent.cpp

"$(INTDIR)\hduagent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

