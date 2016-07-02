# Microsoft Developer Studio Generated NMAKE File, Based on audmixlib.dsp
!IF "$(CFG)" == ""
CFG=audmixlib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to audmixlib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "audmixlib - Win32 Release" && "$(CFG)" != "audmixlib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "audmixlib.mak" CFG="audmixlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "audmixlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "audmixlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "audmixlib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\10-common\lib\release\win32\audmixlib.lib"


CLEAN :
	-@erase "$(INTDIR)\audmixer.obj"
	-@erase "$(INTDIR)\audmixinst.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\10-common\lib\release\win32\audmixlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HARD_CODEC" /Fp"$(INTDIR)\audmixlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\audmixlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\release\win32\audmixlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\audmixer.obj" \
	"$(INTDIR)\audmixinst.obj"

"..\..\..\10-common\lib\release\win32\audmixlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "audmixlib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\10-common\lib\debug\win32\audmixlib.lib" "$(OUTDIR)\audmixlib.bsc"


CLEAN :
	-@erase "$(INTDIR)\audmixer.obj"
	-@erase "$(INTDIR)\audmixer.sbr"
	-@erase "$(INTDIR)\audmixinst.obj"
	-@erase "$(INTDIR)\audmixinst.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\audmixlib.bsc"
	-@erase "..\..\..\10-common\lib\debug\win32\audmixlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\10-common\include\platform" /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\mt" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /D "HARD_CODEC" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\audmixlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\audmixlib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\audmixer.sbr" \
	"$(INTDIR)\audmixinst.sbr"

"$(OUTDIR)\audmixlib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\debug\win32\audmixlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\audmixer.obj" \
	"$(INTDIR)\audmixinst.obj"

"..\..\..\10-common\lib\debug\win32\audmixlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("audmixlib.dep")
!INCLUDE "audmixlib.dep"
!ELSE 
!MESSAGE Warning: cannot find "audmixlib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "audmixlib - Win32 Release" || "$(CFG)" == "audmixlib - Win32 Debug"
SOURCE=..\source\audmixer.cpp

!IF  "$(CFG)" == "audmixlib - Win32 Release"


"$(INTDIR)\audmixer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "audmixlib - Win32 Debug"


"$(INTDIR)\audmixer.obj"	"$(INTDIR)\audmixer.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\audmixinst.cpp

!IF  "$(CFG)" == "audmixlib - Win32 Release"


"$(INTDIR)\audmixinst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "audmixlib - Win32 Debug"


"$(INTDIR)\audmixinst.obj"	"$(INTDIR)\audmixinst.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

