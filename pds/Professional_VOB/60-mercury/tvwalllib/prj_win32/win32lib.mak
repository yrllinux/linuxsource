# Microsoft Developer Studio Generated NMAKE File, Based on win32lib.dsp
!IF "$(CFG)" == ""
CFG=win32lib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to win32lib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "win32lib - Win32 Release" && "$(CFG)" != "win32lib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "win32lib.mak" CFG="win32lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "win32lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "win32lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "win32lib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\10-common\lib\release\win32\tvwall.lib"


CLEAN :
	-@erase "$(INTDIR)\TvWall.obj"
	-@erase "$(INTDIR)\TWInst.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\10-common\lib\release\win32\tvwall.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\common\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\nms1" /I "..\include" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HARD_CODEC" /Fp"$(INTDIR)\win32lib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\win32lib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\release\win32\tvwall.lib" 
LIB32_OBJS= \
	"$(INTDIR)\TvWall.obj" \
	"$(INTDIR)\TWInst.obj"

"..\..\..\10-common\lib\release\win32\tvwall.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "win32lib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\10-common\lib\debug\win32\tvwall.lib" "$(OUTDIR)\win32lib.bsc"


CLEAN :
	-@erase "$(INTDIR)\TvWall.obj"
	-@erase "$(INTDIR)\TvWall.sbr"
	-@erase "$(INTDIR)\TWInst.obj"
	-@erase "$(INTDIR)\TWInst.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\win32lib.bsc"
	-@erase "..\..\..\10-common\lib\debug\win32\tvwall.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\common\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\nms1" /I "..\include" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HARD_CODEC" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\win32lib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\win32lib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\TvWall.sbr" \
	"$(INTDIR)\TWInst.sbr"

"$(OUTDIR)\win32lib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\debug\win32\tvwall.lib" 
LIB32_OBJS= \
	"$(INTDIR)\TvWall.obj" \
	"$(INTDIR)\TWInst.obj"

"..\..\..\10-common\lib\debug\win32\tvwall.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("win32lib.dep")
!INCLUDE "win32lib.dep"
!ELSE 
!MESSAGE Warning: cannot find "win32lib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "win32lib - Win32 Release" || "$(CFG)" == "win32lib - Win32 Debug"
SOURCE=..\source\TvWall.cpp

!IF  "$(CFG)" == "win32lib - Win32 Release"


"$(INTDIR)\TvWall.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "win32lib - Win32 Debug"


"$(INTDIR)\TvWall.obj"	"$(INTDIR)\TvWall.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\TWInst.cpp

!IF  "$(CFG)" == "win32lib - Win32 Release"


"$(INTDIR)\TWInst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "win32lib - Win32 Debug"


"$(INTDIR)\TWInst.obj"	"$(INTDIR)\TWInst.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

