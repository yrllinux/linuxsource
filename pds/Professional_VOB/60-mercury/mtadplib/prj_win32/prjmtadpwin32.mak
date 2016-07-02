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

ALL : "..\..\..\10-common\lib\release\Win32\mtadp.lib"


CLEAN :
	-@erase "$(INTDIR)\gk.obj"
	-@erase "$(INTDIR)\mtadp.obj"
	-@erase "$(INTDIR)\mtadpd.obj"
	-@erase "$(INTDIR)\mtadputils.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\10-common\lib\release\Win32\mtadp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\mt" /I "..\..\..\10-common\include\vrs" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\radstack" /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol\h323stack" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\cbb\license" /D "_LIB" /D "_CONFIG_FROM_FILE" /D "_UTF8" /D "WIN32" /D "NDEBUG" /D "_MBCS" /Fp"$(INTDIR)\prjmtadpwin32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\prjmtadpwin32.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\release\Win32\mtadp.lib" 
LIB32_OBJS= \
	"$(INTDIR)\gk.obj" \
	"$(INTDIR)\mtadp.obj" \
	"$(INTDIR)\mtadpd.obj" \
	"$(INTDIR)\mtadputils.obj"

"..\..\..\10-common\lib\release\Win32\mtadp.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "prjMtadpWin32 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\10-common\lib\debug\Win32\mtadp.lib" "$(OUTDIR)\prjmtadpwin32.bsc"


CLEAN :
	-@erase "$(INTDIR)\gk.obj"
	-@erase "$(INTDIR)\gk.sbr"
	-@erase "$(INTDIR)\mtadp.obj"
	-@erase "$(INTDIR)\mtadp.sbr"
	-@erase "$(INTDIR)\mtadpd.obj"
	-@erase "$(INTDIR)\mtadpd.sbr"
	-@erase "$(INTDIR)\mtadputils.obj"
	-@erase "$(INTDIR)\mtadputils.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\prjmtadpwin32.bsc"
	-@erase "..\..\..\10-common\lib\debug\Win32\mtadp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\mt" /I "..\..\..\10-common\include\vrs" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\radstack" /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol\h323stack" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\cbb\license" /D "_LIB" /D "_AFXDLL" /D "_CONFIG_FROM_FILE" /D "_UTF8" /D "WIN32" /D "_DEBUG" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\prjmtadpwin32.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\prjmtadpwin32.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\gk.sbr" \
	"$(INTDIR)\mtadp.sbr" \
	"$(INTDIR)\mtadpd.sbr" \
	"$(INTDIR)\mtadputils.sbr"

"$(OUTDIR)\prjmtadpwin32.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\debug\Win32\mtadp.lib" 
LIB32_OBJS= \
	"$(INTDIR)\gk.obj" \
	"$(INTDIR)\mtadp.obj" \
	"$(INTDIR)\mtadpd.obj" \
	"$(INTDIR)\mtadputils.obj"

"..\..\..\10-common\lib\debug\Win32\mtadp.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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

!IF  "$(CFG)" == "prjMtadpWin32 - Win32 Release"


"$(INTDIR)\gk.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMtadpWin32 - Win32 Debug"


"$(INTDIR)\gk.obj"	"$(INTDIR)\gk.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mtadp.cpp

!IF  "$(CFG)" == "prjMtadpWin32 - Win32 Release"


"$(INTDIR)\mtadp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMtadpWin32 - Win32 Debug"


"$(INTDIR)\mtadp.obj"	"$(INTDIR)\mtadp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mtadpd.cpp

!IF  "$(CFG)" == "prjMtadpWin32 - Win32 Release"


"$(INTDIR)\mtadpd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMtadpWin32 - Win32 Debug"


"$(INTDIR)\mtadpd.obj"	"$(INTDIR)\mtadpd.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mtadputils.cpp

!IF  "$(CFG)" == "prjMtadpWin32 - Win32 Release"


"$(INTDIR)\mtadputils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prjMtadpWin32 - Win32 Debug"


"$(INTDIR)\mtadputils.obj"	"$(INTDIR)\mtadputils.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

