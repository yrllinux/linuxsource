# Microsoft Developer Studio Generated NMAKE File, Based on prslib.dsp
!IF "$(CFG)" == ""
CFG=prslib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to prslib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "prslib - Win32 Release" && "$(CFG)" != "prslib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "prslib.mak" CFG="prslib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "prslib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "prslib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "prslib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\10-common\lib\release\win32\mcuprs.lib"


CLEAN :
	-@erase "$(INTDIR)\mcuprs.obj"
	-@erase "$(INTDIR)\mcuprsguard.obj"
	-@erase "$(INTDIR)\mcuprsinst.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\10-common\lib\release\win32\mcuprs.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\nms1" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /Fp"$(INTDIR)\prslib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\prslib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\release\win32\mcuprs.lib" 
LIB32_OBJS= \
	"$(INTDIR)\mcuprs.obj" \
	"$(INTDIR)\mcuprsguard.obj" \
	"$(INTDIR)\mcuprsinst.obj"

"..\..\..\10-common\lib\release\win32\mcuprs.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "prslib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\10-common\lib\debug\win32\mcuprs.lib" "$(OUTDIR)\prslib.bsc"


CLEAN :
	-@erase "$(INTDIR)\mcuprs.obj"
	-@erase "$(INTDIR)\mcuprs.sbr"
	-@erase "$(INTDIR)\mcuprsguard.obj"
	-@erase "$(INTDIR)\mcuprsguard.sbr"
	-@erase "$(INTDIR)\mcuprsinst.obj"
	-@erase "$(INTDIR)\mcuprsinst.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\prslib.bsc"
	-@erase "..\..\..\10-common\lib\debug\win32\mcuprs.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\nms1" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\prslib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\prslib.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\mcuprs.sbr" \
	"$(INTDIR)\mcuprsguard.sbr" \
	"$(INTDIR)\mcuprsinst.sbr"

"$(OUTDIR)\prslib.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\debug\win32\mcuprs.lib" 
LIB32_OBJS= \
	"$(INTDIR)\mcuprs.obj" \
	"$(INTDIR)\mcuprsguard.obj" \
	"$(INTDIR)\mcuprsinst.obj"

"..\..\..\10-common\lib\debug\win32\mcuprs.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("prslib.dep")
!INCLUDE "prslib.dep"
!ELSE 
!MESSAGE Warning: cannot find "prslib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "prslib - Win32 Release" || "$(CFG)" == "prslib - Win32 Debug"
SOURCE=..\source\mcuprs.cpp

!IF  "$(CFG)" == "prslib - Win32 Release"


"$(INTDIR)\mcuprs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prslib - Win32 Debug"


"$(INTDIR)\mcuprs.obj"	"$(INTDIR)\mcuprs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcuprsguard.cpp

!IF  "$(CFG)" == "prslib - Win32 Release"


"$(INTDIR)\mcuprsguard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prslib - Win32 Debug"


"$(INTDIR)\mcuprsguard.obj"	"$(INTDIR)\mcuprsguard.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\mcuprsinst.cpp

!IF  "$(CFG)" == "prslib - Win32 Release"


"$(INTDIR)\mcuprsinst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "prslib - Win32 Debug"


"$(INTDIR)\mcuprsinst.obj"	"$(INTDIR)\mcuprsinst.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

