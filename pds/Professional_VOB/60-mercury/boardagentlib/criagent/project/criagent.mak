# Microsoft Developer Studio Generated NMAKE File, Based on criagent.dsp
!IF "$(CFG)" == ""
CFG=CriAgent - Win32 Debug
!MESSAGE No configuration specified. Defaulting to CriAgent - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "CriAgent - Win32 Release" && "$(CFG)" != "CriAgent - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "criagent.mak" CFG="CriAgent - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CriAgent - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "CriAgent - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "CriAgent - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\10-common\lib\release\win32\criagent.lib"


CLEAN :
	-@erase "$(INTDIR)\criagent.obj"
	-@erase "$(INTDIR)\criconfig.obj"
	-@erase "$(INTDIR)\criguard.obj"
	-@erase "$(INTDIR)\criinit.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\..\10-common\lib\release\win32\criagent.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\common" /I "..\..\..\common\include" /I "..\..\..\..\10-common\include\mcu" /I "..\..\..\..\10-common\include\platform" /I "..\..\..\..\10-common\include\system1" /I "..\..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /Fp"$(INTDIR)\criagent.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\criagent.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\..\10-common\lib\release\win32\criagent.lib" 
LIB32_OBJS= \
	"$(INTDIR)\criagent.obj" \
	"$(INTDIR)\criconfig.obj" \
	"$(INTDIR)\criguard.obj" \
	"$(INTDIR)\criinit.obj"

"..\..\..\..\10-common\lib\release\win32\criagent.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "CriAgent - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\..\10-common\lib\debug\win32\criagent.lib" "$(OUTDIR)\criagent.bsc"


CLEAN :
	-@erase "$(INTDIR)\criagent.obj"
	-@erase "$(INTDIR)\criagent.sbr"
	-@erase "$(INTDIR)\criconfig.obj"
	-@erase "$(INTDIR)\criconfig.sbr"
	-@erase "$(INTDIR)\criguard.obj"
	-@erase "$(INTDIR)\criguard.sbr"
	-@erase "$(INTDIR)\criinit.obj"
	-@erase "$(INTDIR)\criinit.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\criagent.bsc"
	-@erase "..\..\..\..\10-common\lib\debug\win32\criagent.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\common\include" /I "..\include" /I "..\..\..\..\10-common\include\mcu" /I "..\..\..\..\10-common\include\platform" /I "..\..\..\..\10-common\include\system1" /I "..\..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\criagent.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\criagent.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\criagent.sbr" \
	"$(INTDIR)\criconfig.sbr" \
	"$(INTDIR)\criguard.sbr" \
	"$(INTDIR)\criinit.sbr"

"$(OUTDIR)\criagent.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\..\10-common\lib\debug\win32\criagent.lib" 
LIB32_OBJS= \
	"$(INTDIR)\criagent.obj" \
	"$(INTDIR)\criconfig.obj" \
	"$(INTDIR)\criguard.obj" \
	"$(INTDIR)\criinit.obj"

"..\..\..\..\10-common\lib\debug\win32\criagent.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("criagent.dep")
!INCLUDE "criagent.dep"
!ELSE 
!MESSAGE Warning: cannot find "criagent.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "CriAgent - Win32 Release" || "$(CFG)" == "CriAgent - Win32 Debug"
SOURCE=..\source\criagent.cpp

!IF  "$(CFG)" == "CriAgent - Win32 Release"


"$(INTDIR)\criagent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CriAgent - Win32 Debug"


"$(INTDIR)\criagent.obj"	"$(INTDIR)\criagent.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\criconfig.cpp

!IF  "$(CFG)" == "CriAgent - Win32 Release"


"$(INTDIR)\criconfig.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CriAgent - Win32 Debug"


"$(INTDIR)\criconfig.obj"	"$(INTDIR)\criconfig.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\criguard.cpp

!IF  "$(CFG)" == "CriAgent - Win32 Release"


"$(INTDIR)\criguard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CriAgent - Win32 Debug"


"$(INTDIR)\criguard.obj"	"$(INTDIR)\criguard.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\criinit.cpp

!IF  "$(CFG)" == "CriAgent - Win32 Release"


"$(INTDIR)\criinit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "CriAgent - Win32 Debug"


"$(INTDIR)\criinit.obj"	"$(INTDIR)\criinit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

