# Microsoft Developer Studio Generated NMAKE File, Based on prjmtadpwin32.dsp
!IF "$(CFG)" == ""
CFG=prjMtadpWin32 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to prjMtadpWin32 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "prjMtadpWin32 - Win32 Release" && "$(CFG)" != "prjMtadpWin32 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "prjmtadpwin32.mak" CFG="prjMtadpWin32 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "prjMtadpWin32 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "prjMtadpWin32 - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "prjMtadpWin32 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\10-common\lib\release\Win32\mtadp6.lib"


CLEAN :
	-@erase "$(INTDIR)\gk.obj"
	-@erase "$(INTDIR)\mtadp.obj"
	-@erase "$(INTDIR)\mtadpd.obj"
	-@erase "$(INTDIR)\mtadputils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\10-common\lib\release\Win32\mtadp6.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\mt" /I "..\..\..\10-common\include\vrs" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\radstack" /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol\h323stack" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\cbb\license" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_UTF8" /D "_LIB" /D "_CONFIG_FROM_FILE" /Fp"$(INTDIR)\prjmtadpwin32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\prjmtadpwin32.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\release\Win32\mtadp6.lib" 
LIB32_OBJS= \
	"$(INTDIR)\gk.obj" \
	"$(INTDIR)\mtadp.obj" \
	"$(INTDIR)\mtadpd.obj" \
	"$(INTDIR)\mtadputils.obj"

"..\..\..\10-common\lib\release\Win32\mtadp6.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "prjMtadpWin32 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\10-common\lib\debug\Win32\mtadp6.lib"


CLEAN :
	-@erase "$(INTDIR)\gk.obj"
	-@erase "$(INTDIR)\mtadp.obj"
	-@erase "$(INTDIR)\mtadpd.obj"
	-@erase "$(INTDIR)\mtadputils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\10-common\lib\debug\Win32\mtadp6.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\mt" /I "..\..\..\10-common\include\vrs" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\radstack" /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol\h323stack" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\cbb\license" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_UTF8" /D "_AFXDLL" /D "_CONFIG_FROM_FILE" /Fp"$(INTDIR)\prjmtadpwin32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\prjmtadpwin32.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\debug\Win32\mtadp6.lib" 
LIB32_OBJS= \
	"$(INTDIR)\gk.obj" \
	"$(INTDIR)\mtadp.obj" \
	"$(INTDIR)\mtadpd.obj" \
	"$(INTDIR)\mtadputils.obj"

"..\..\..\10-common\lib\debug\Win32\mtadp6.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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
!IF EXISTS("prjmtadpwin32.dep")
!INCLUDE "prjmtadpwin32.dep"
!ELSE 
!MESSAGE Warning: cannot find "prjmtadpwin32.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "prjMtadpWin32 - Win32 Release" || "$(CFG)" == "prjMtadpWin32 - Win32 Debug"
SOURCE=..\source\gk.cpp

"$(INTDIR)\gk.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mtadp.cpp

"$(INTDIR)\mtadp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mtadpd.cpp

"$(INTDIR)\mtadpd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mtadputils.cpp

"$(INTDIR)\mtadputils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

