# Microsoft Developer Studio Generated NMAKE File, Based on prjmp.dsp
!IF "$(CFG)" == ""
CFG=prjMp - Win32 Debug
!MESSAGE No configuration specified. Defaulting to prjMp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "prjMp - Win32 Release" && "$(CFG)" != "prjMp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "prjmp.mak" CFG="prjMp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "prjMp - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "prjMp - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "prjMp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\..\..\10-common\lib\release\win32\mp.lib" "$(OUTDIR)\prjmp.bsc"


CLEAN :
	-@erase "$(INTDIR)\mp.obj"
	-@erase "$(INTDIR)\mp.sbr"
	-@erase "$(INTDIR)\mpdata.obj"
	-@erase "$(INTDIR)\mpdata.sbr"
	-@erase "$(INTDIR)\mpinst.obj"
	-@erase "$(INTDIR)\mpinst.sbr"
	-@erase "$(INTDIR)\mputility.obj"
	-@erase "$(INTDIR)\mputility.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\prjmp.bsc"
	-@erase "..\..\..\10-common\lib\release\win32\mp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\prjmp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\prjmp.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\mp.sbr" \
	"$(INTDIR)\mpdata.sbr" \
	"$(INTDIR)\mpinst.sbr" \
	"$(INTDIR)\mputility.sbr"

"$(OUTDIR)\prjmp.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\release\win32\mp.lib" 
LIB32_OBJS= \
	"$(INTDIR)\mp.obj" \
	"$(INTDIR)\mpdata.obj" \
	"$(INTDIR)\mpinst.obj" \
	"$(INTDIR)\mputility.obj"

"..\..\..\10-common\lib\release\win32\mp.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "prjMp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\10-common\lib\debug\win32\mp.lib" "$(OUTDIR)\prjmp.bsc"


CLEAN :
	-@erase "$(INTDIR)\mp.obj"
	-@erase "$(INTDIR)\mp.sbr"
	-@erase "$(INTDIR)\mpdata.obj"
	-@erase "$(INTDIR)\mpdata.sbr"
	-@erase "$(INTDIR)\mpinst.obj"
	-@erase "$(INTDIR)\mpinst.sbr"
	-@erase "$(INTDIR)\mputility.obj"
	-@erase "$(INTDIR)\mputility.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\prjmp.bsc"
	-@erase "..\..\..\10-common\lib\debug\win32\mp.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\prjmp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\prjmp.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\mp.sbr" \
	"$(INTDIR)\mpdata.sbr" \
	"$(INTDIR)\mpinst.sbr" \
	"$(INTDIR)\mputility.sbr"

"$(OUTDIR)\prjmp.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\debug\win32\mp.lib" 
LIB32_OBJS= \
	"$(INTDIR)\mp.obj" \
	"$(INTDIR)\mpdata.obj" \
	"$(INTDIR)\mpinst.obj" \
	"$(INTDIR)\mputility.obj"

"..\..\..\10-common\lib\debug\win32\mp.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("prjmp.dep")
!INCLUDE "prjmp.dep"
!ELSE 
!MESSAGE Warning: cannot find "prjmp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "prjMp - Win32 Release" || "$(CFG)" == "prjMp - Win32 Debug"
SOURCE=..\source\mp.cpp

"$(INTDIR)\mp.obj"	"$(INTDIR)\mp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpdata.cpp

"$(INTDIR)\mpdata.obj"	"$(INTDIR)\mpdata.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpinst.cpp

"$(INTDIR)\mpinst.obj"	"$(INTDIR)\mpinst.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mputility.cpp

"$(INTDIR)\mputility.obj"	"$(INTDIR)\mputility.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

