# Microsoft Developer Studio Generated NMAKE File, Based on hdu2lib.dsp
!IF "$(CFG)" == ""
CFG=hdu2lib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to hdu2lib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "hdu2lib - Win32 Release" && "$(CFG)" != "hdu2lib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hdu2lib.mak" CFG="hdu2lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hdu2lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "hdu2lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "hdu2lib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\10-Common\lib\release\win32\hdu2lib.lib"


CLEAN :
	-@erase "$(INTDIR)\hdu2app.obj"
	-@erase "$(INTDIR)\hdu2test.obj"
	-@erase "$(INTDIR)\hdu2inst.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\10-Common\lib\release\win32\hdu2lib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\include" /I "..\..\common\include" /I "..\..\hdu2\include" /I "..\..\..\10-Common\include\platform" /I "..\..\..\10-Common\include\mcu" /I "..\..\..\10-Common\include\system1" /I "..\..\..\10-Common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HARD_CODEC" /Fp"$(INTDIR)\hdu2lib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hdu2lib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-Common\lib\release\win32\hdu2lib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\hdu2app.obj" \
	"$(INTDIR)\hdu2test.obj" \
	"$(INTDIR)\hdu2inst.obj"

"..\..\..\10-Common\lib\release\win32\hdu2lib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hdu2lib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\10-Common\lib\debug\win32\hdu2lib.lib"


CLEAN :
	-@erase "$(INTDIR)\hdu2app.obj"
	-@erase "$(INTDIR)\hdu2test.obj"
	-@erase "$(INTDIR)\hdu2inst.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\10-Common\lib\debug\win32\hdu2lib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\hdu2\include" /I "..\..\..\10-Common\include\platform" /I "..\..\..\10-Common\include\mcu" /I "..\..\..\10-Common\include\system1" /I "..\..\..\10-Common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_LIB" /D "HARD_CODEC" /Fp"$(INTDIR)\hdu2lib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hdu2lib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-Common\lib\debug\win32\hdu2lib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\hdu2app.obj" \
	"$(INTDIR)\hdu2test.obj" \
	"$(INTDIR)\hdu2inst.obj"

"..\..\..\10-Common\lib\debug\win32\hdu2lib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("hdu2lib.dep")
!INCLUDE "hdu2lib.dep"
!ELSE 
!MESSAGE Warning: cannot find "hdu2lib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "hdu2lib - Win32 Release" || "$(CFG)" == "hdu2lib - Win32 Debug"
SOURCE=..\source\hdu2app.cpp

"$(INTDIR)\hdu2app.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\hdu2test.cpp

"$(INTDIR)\hdu2test.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\hdu2inst.cpp

"$(INTDIR)\hdu2inst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

