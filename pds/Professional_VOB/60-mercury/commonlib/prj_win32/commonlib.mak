# Microsoft Developer Studio Generated NMAKE File, Based on commonlib.dsp
!IF "$(CFG)" == ""
CFG=commonlib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to commonlib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "commonlib - Win32 Release" && "$(CFG)" != "commonlib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "commonlib.mak" CFG="commonlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "commonlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "commonlib - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "commonlib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\10-common\lib\release\win32\commonlib.lib"


CLEAN :
	-@erase "$(INTDIR)\commonlib.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\10-common\lib\release\win32\commonlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\common\include" /I "..\..\..\10-Common\include\platform" /I "..\..\..\10-Common\include\mcu" /I "..\..\..\10-Common\include\system1" /I "..\..\..\10-Common\include\protocol" /I "..\..\common\include\platform_sdk\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\commonlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\release\win32\commonlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\commonlib.obj"

"..\..\..\10-common\lib\release\win32\commonlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "commonlib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\10-common\lib\debug\win32\commonlib.lib"


CLEAN :
	-@erase "$(INTDIR)\commonlib.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\10-common\lib\debug\win32\commonlib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-Common\include\platform" /I "..\..\..\10-Common\include\mcu" /I "..\..\..\10-Common\include\system1" /I "..\..\..\10-Common\include\protocol" /I "..\..\common\include\platform_sdk\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\commonlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\debug\win32\commonlib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\commonlib.obj"

"..\..\..\10-common\lib\debug\win32\commonlib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("commonlib.dep")
!INCLUDE "commonlib.dep"
!ELSE 
!MESSAGE Warning: cannot find "commonlib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "commonlib - Win32 Release" || "$(CFG)" == "commonlib - Win32 Debug"
SOURCE=..\source\commonlib.cpp

"$(INTDIR)\commonlib.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

