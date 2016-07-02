# Microsoft Developer Studio Generated NMAKE File, Based on drawbmplib.dsp
!IF "$(CFG)" == ""
CFG=drawbmplib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to drawbmplib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "drawbmplib - Win32 Release" && "$(CFG)" != "drawbmplib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "drawbmplib.mak" CFG="drawbmplib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "drawbmplib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "drawbmplib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "drawbmplib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\10-common\lib\release\win32\drawbmplib.lib"


CLEAN :
	-@erase "$(INTDIR)\drawbmp.obj"
	-@erase "$(INTDIR)\drawinterface.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\10-common\lib\release\win32\drawbmplib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "../include" /I "..\..\common\include" /I "../../../10-common/include/mcu" /I "10-common\include\protocol\" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\platform" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\drawbmplib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\drawbmplib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"../../../10-common/lib/release/win32/drawbmplib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\drawbmp.obj" \
	"$(INTDIR)\drawinterface.obj"

"..\..\..\10-common\lib\release\win32\drawbmplib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "drawbmplib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\10-common\lib\debug\win32\drawbmplib.lib"


CLEAN :
	-@erase "$(INTDIR)\drawbmp.obj"
	-@erase "$(INTDIR)\drawinterface.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\10-common\lib\debug\win32\drawbmplib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "../include" /I "..\..\common\include" /I "..\..\..\10-common/include/mcu" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\drawbmplib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\drawbmplib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"../../../10-common/lib/debug/win32/drawbmplib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\drawbmp.obj" \
	"$(INTDIR)\drawinterface.obj"

"..\..\..\10-common\lib\debug\win32\drawbmplib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("drawbmplib.dep")
!INCLUDE "drawbmplib.dep"
!ELSE 
!MESSAGE Warning: cannot find "drawbmplib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "drawbmplib - Win32 Release" || "$(CFG)" == "drawbmplib - Win32 Debug"
SOURCE=..\src\drawbmp.cpp

"$(INTDIR)\drawbmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\src\drawinterface.cpp

"$(INTDIR)\drawinterface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

