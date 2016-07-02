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

ALL : "..\..\..\10-common\lib\release\win32\vmp.lib"


CLEAN :
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vmpcfg.obj"
	-@erase "$(INTDIR)\vmpinst.obj"
	-@erase "..\..\..\10-common\lib\release\win32\vmp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HARD_CODEC" /D "_AFXDLL" /Fp"$(INTDIR)\win32lib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\release\win32\vmp.lib" 
LIB32_OBJS= \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\vmpcfg.obj" \
	"$(INTDIR)\vmpinst.obj"

"..\..\..\10-common\lib\release\win32\vmp.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "win32lib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\10-common\lib\Debug\win32\vmp.lib" "$(OUTDIR)\win32lib.bsc"


CLEAN :
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\main.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vmpcfg.obj"
	-@erase "$(INTDIR)\vmpcfg.sbr"
	-@erase "$(INTDIR)\vmpinst.obj"
	-@erase "$(INTDIR)\vmpinst.sbr"
	-@erase "$(OUTDIR)\win32lib.bsc"
	-@erase "..\..\..\10-common\lib\Debug\win32\vmp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HARD_CODEC" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\win32lib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
	"$(INTDIR)\main.sbr" \
	"$(INTDIR)\vmpcfg.sbr" \
	"$(INTDIR)\vmpinst.sbr"

"$(OUTDIR)\win32lib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\Debug\win32\vmp.lib" 
LIB32_OBJS= \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\vmpcfg.obj" \
	"$(INTDIR)\vmpinst.obj"

"..\..\..\10-common\lib\Debug\win32\vmp.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
SOURCE=..\source\main.cpp

!IF  "$(CFG)" == "win32lib - Win32 Release"


"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "win32lib - Win32 Debug"


"$(INTDIR)\main.obj"	"$(INTDIR)\main.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\vmpcfg.cpp

!IF  "$(CFG)" == "win32lib - Win32 Release"


"$(INTDIR)\vmpcfg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "win32lib - Win32 Debug"


"$(INTDIR)\vmpcfg.obj"	"$(INTDIR)\vmpcfg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\vmpinst.cpp

!IF  "$(CFG)" == "win32lib - Win32 Release"


"$(INTDIR)\vmpinst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "win32lib - Win32 Debug"


"$(INTDIR)\vmpinst.obj"	"$(INTDIR)\vmpinst.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

