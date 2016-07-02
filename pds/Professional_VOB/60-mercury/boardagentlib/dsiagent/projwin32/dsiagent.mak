# Microsoft Developer Studio Generated NMAKE File, Based on dsiagent.dsp
!IF "$(CFG)" == ""
CFG=dsiagent - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dsiagent - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dsiagent - Win32 Release" && "$(CFG)" != "dsiagent - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dsiagent.mak" CFG="dsiagent - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dsiagent - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "dsiagent - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "dsiagent - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\10-common\lib\release\win32\dsiagent.lib"


CLEAN :
	-@erase "$(INTDIR)\dsiagent.obj"
	-@erase "$(INTDIR)\dsiconfig.obj"
	-@erase "$(INTDIR)\dsiguard.obj"
	-@erase "$(INTDIR)\dsiinit.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\..\10-common\lib\release\win32\dsiagent.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\common" /I "..\..\..\..\10-common\include\mcu" /I "..\..\..\..\10-common\include\platform" /I "..\..\..\..\10-common\include\system1" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /Fp"$(INTDIR)\dsiagent.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dsiagent.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\..\10-common\lib\release\win32\dsiagent.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dsiagent.obj" \
	"$(INTDIR)\dsiconfig.obj" \
	"$(INTDIR)\dsiguard.obj" \
	"$(INTDIR)\dsiinit.obj"

"..\..\..\..\10-common\lib\release\win32\dsiagent.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dsiagent - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\10-common\lib\debug\win32\dsiagent.lib"


CLEAN :
	-@erase "$(INTDIR)\dsiagent.obj"
	-@erase "$(INTDIR)\dsiconfig.obj"
	-@erase "$(INTDIR)\dsiguard.obj"
	-@erase "$(INTDIR)\dsiinit.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\..\10-common\lib\debug\win32\dsiagent.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\..\common\include" /I "..\..\..\..\10-common\include\mcu" /I "..\..\..\..\10-common\include\platform" /I "..\..\..\..\10-common\include\system1" /I "..\..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /Fp"$(INTDIR)\dsiagent.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dsiagent.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\..\10-common\lib\debug\win32\dsiagent.lib" 
LIB32_OBJS= \
	"$(INTDIR)\dsiagent.obj" \
	"$(INTDIR)\dsiconfig.obj" \
	"$(INTDIR)\dsiguard.obj" \
	"$(INTDIR)\dsiinit.obj"

"..\..\..\..\10-common\lib\debug\win32\dsiagent.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("dsiagent.dep")
!INCLUDE "dsiagent.dep"
!ELSE 
!MESSAGE Warning: cannot find "dsiagent.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dsiagent - Win32 Release" || "$(CFG)" == "dsiagent - Win32 Debug"
SOURCE=..\source\dsiagent.cpp

"$(INTDIR)\dsiagent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\dsiconfig.cpp

"$(INTDIR)\dsiconfig.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\dsiguard.cpp

"$(INTDIR)\dsiguard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\dsiinit.cpp

"$(INTDIR)\dsiinit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

