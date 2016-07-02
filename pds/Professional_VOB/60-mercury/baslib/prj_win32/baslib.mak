# Microsoft Developer Studio Generated NMAKE File, Based on baslib.dsp
!IF "$(CFG)" == ""
CFG=BASLib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to BASLib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "BASLib - Win32 Release" && "$(CFG)" != "BASLib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "baslib.mak" CFG="BASLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BASLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "BASLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "BASLib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\10-Common\lib\release\win32\bas.lib"


CLEAN :
	-@erase "$(INTDIR)\basApp.obj"
	-@erase "$(INTDIR)\basInst.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\10-Common\lib\release\win32\bas.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\include" /I "..\..\common\include" /I "..\..\..\10-Common\include\platform" /I "..\..\..\10-Common\include\mcu" /I "..\..\..\10-Common\include\system1" /I "..\..\..\10-Common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HARD_CODEC" /Fp"$(INTDIR)\baslib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\baslib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-Common\lib\release\win32\bas.lib" 
LIB32_OBJS= \
	"$(INTDIR)\basApp.obj" \
	"$(INTDIR)\basInst.obj"

"..\..\..\10-Common\lib\release\win32\bas.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "BASLib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\10-Common\lib\debug\win32\bas.lib" "$(OUTDIR)\baslib.bsc"


CLEAN :
	-@erase "$(INTDIR)\basApp.obj"
	-@erase "$(INTDIR)\basApp.sbr"
	-@erase "$(INTDIR)\basInst.obj"
	-@erase "$(INTDIR)\basInst.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\baslib.bsc"
	-@erase "..\..\..\10-Common\lib\debug\win32\bas.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-Common\include\platform" /I "..\..\..\10-Common\include\mcu" /I "..\..\..\10-Common\include\system1" /I "..\..\..\10-Common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HARD_CODEC" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\baslib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\basApp.sbr" \
	"$(INTDIR)\basInst.sbr"

"$(OUTDIR)\baslib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-Common\lib\debug\win32\bas.lib" 
LIB32_OBJS= \
	"$(INTDIR)\basApp.obj" \
	"$(INTDIR)\basInst.obj"

"..\..\..\10-Common\lib\debug\win32\bas.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("baslib.dep")
!INCLUDE "baslib.dep"
!ELSE 
!MESSAGE Warning: cannot find "baslib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "BASLib - Win32 Release" || "$(CFG)" == "BASLib - Win32 Debug"
SOURCE=..\source\basApp.cpp

!IF  "$(CFG)" == "BASLib - Win32 Release"


"$(INTDIR)\basApp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "BASLib - Win32 Debug"


"$(INTDIR)\basApp.obj"	"$(INTDIR)\basApp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\basInst.cpp

!IF  "$(CFG)" == "BASLib - Win32 Release"


"$(INTDIR)\basInst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "BASLib - Win32 Debug"


"$(INTDIR)\basInst.obj"	"$(INTDIR)\basInst.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

