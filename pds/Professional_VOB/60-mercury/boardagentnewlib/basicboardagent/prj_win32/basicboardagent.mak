# Microsoft Developer Studio Generated NMAKE File, Based on basicboardagent.dsp
!IF "$(CFG)" == ""
CFG=BasicBoardAgent - Win32 Debug
!MESSAGE No configuration specified. Defaulting to BasicBoardAgent - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "BasicBoardAgent - Win32 Release" && "$(CFG)" != "BasicBoardAgent - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "basicboardagent.mak" CFG="BasicBoardAgent - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BasicBoardAgent - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "BasicBoardAgent - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "BasicBoardAgent - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\10-common\lib\release\win32\basicboardagent.lib"


CLEAN :
	-@erase "$(INTDIR)\boardagentbasic.obj"
	-@erase "$(INTDIR)\boardconfigbasic.obj"
	-@erase "$(INTDIR)\boardguardbasic.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\..\10-common\lib\release\win32\basicboardagent.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\common" /I "..\..\..\common\include" /I "..\..\..\..\10-common\include\mcu" /I "..\..\..\..\10-common\include\platform" /I "..\..\..\..\10-common\include\system1" /I "..\..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /Fp"$(INTDIR)\basicboardagent.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\basicboardagent.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\..\10-common\lib\release\win32\basicboardagent.lib" 
LIB32_OBJS= \
	"$(INTDIR)\boardagentbasic.obj" \
	"$(INTDIR)\boardconfigbasic.obj" \
	"$(INTDIR)\boardguardbasic.obj"

"..\..\..\..\10-common\lib\release\win32\basicboardagent.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "BasicBoardAgent - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\..\10-common\lib\debug\win32\basicboardagent.lib" "$(OUTDIR)\basicboardagent.bsc"


CLEAN :
	-@erase "$(INTDIR)\boardagentbasic.obj"
	-@erase "$(INTDIR)\boardagentbasic.sbr"
	-@erase "$(INTDIR)\boardconfigbasic.obj"
	-@erase "$(INTDIR)\boardconfigbasic.sbr"
	-@erase "$(INTDIR)\boardguardbasic.obj"
	-@erase "$(INTDIR)\boardguardbasic.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\basicboardagent.bsc"
	-@erase "..\..\..\..\10-common\lib\debug\win32\basicboardagent.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\common\include" /I "..\include" /I "..\..\..\..\10-common\include\mcu" /I "..\..\..\..\10-common\include\platform" /I "..\..\..\..\10-common\include\system1" /I "..\..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\basicboardagent.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\basicboardagent.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\boardagentbasic.sbr" \
	"$(INTDIR)\boardconfigbasic.sbr" \
	"$(INTDIR)\boardguardbasic.sbr"

"$(OUTDIR)\basicboardagent.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\..\10-common\lib\debug\win32\basicboardagent.lib" 
LIB32_OBJS= \
	"$(INTDIR)\boardagentbasic.obj" \
	"$(INTDIR)\boardconfigbasic.obj" \
	"$(INTDIR)\boardguardbasic.obj"

"..\..\..\..\10-common\lib\debug\win32\basicboardagent.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("basicboardagent.dep")
!INCLUDE "basicboardagent.dep"
!ELSE 
!MESSAGE Warning: cannot find "basicboardagent.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "BasicBoardAgent - Win32 Release" || "$(CFG)" == "BasicBoardAgent - Win32 Debug"
SOURCE=..\source\boardagentbasic.cpp

!IF  "$(CFG)" == "BasicBoardAgent - Win32 Release"


"$(INTDIR)\boardagentbasic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "BasicBoardAgent - Win32 Debug"


"$(INTDIR)\boardagentbasic.obj"	"$(INTDIR)\boardagentbasic.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\boardconfigbasic.cpp

!IF  "$(CFG)" == "BasicBoardAgent - Win32 Release"


"$(INTDIR)\boardconfigbasic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "BasicBoardAgent - Win32 Debug"


"$(INTDIR)\boardconfigbasic.obj"	"$(INTDIR)\boardconfigbasic.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\boardguardbasic.cpp

!IF  "$(CFG)" == "BasicBoardAgent - Win32 Release"


"$(INTDIR)\boardguardbasic.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "BasicBoardAgent - Win32 Debug"


"$(INTDIR)\boardguardbasic.obj"	"$(INTDIR)\boardguardbasic.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

