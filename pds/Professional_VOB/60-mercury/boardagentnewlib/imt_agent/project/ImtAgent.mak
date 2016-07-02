# Microsoft Developer Studio Generated NMAKE File, Based on ImtAgent.dsp
!IF "$(CFG)" == ""
CFG=ImtAgent - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ImtAgent - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ImtAgent - Win32 Release" && "$(CFG)" != "ImtAgent - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ImtAgent.mak" CFG="ImtAgent - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ImtAgent - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "ImtAgent - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ImtAgent - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\10-common\lib\release\win32\ImtAgent.lib"


CLEAN :
	-@erase "$(INTDIR)\imt_agent.obj"
	-@erase "$(INTDIR)\imt_config.obj"
	-@erase "$(INTDIR)\imtinit.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\..\10-common\lib\release\win32\ImtAgent.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\BasicBoardAgent\include" /I "..\..\mediabrdagent\include" /I "..\include" /I "..\..\..\common" /I "..\..\..\..\10-common\include\mcu" /I "..\..\..\..\10-common\include\platform" /I "..\..\..\..\10-common\include\system1" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /D "IMT" /Fp"$(INTDIR)\ImtAgent.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ImtAgent.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\..\10-common\lib\release\win32\ImtAgent.lib" 
LIB32_OBJS= \
	"$(INTDIR)\imt_agent.obj" \
	"$(INTDIR)\imt_config.obj" \
	"$(INTDIR)\imtinit.obj"

"..\..\..\..\10-common\lib\release\win32\ImtAgent.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ImtAgent - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\10-common\lib\debug\win32\ImtAgent.lib"


CLEAN :
	-@erase "$(INTDIR)\imt_agent.obj"
	-@erase "$(INTDIR)\imt_config.obj"
	-@erase "$(INTDIR)\imtinit.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\..\10-common\lib\debug\win32\ImtAgent.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\BasicBoardAgent\include" /I "..\..\mediabrdagent\include" /I "..\include" /I "..\..\..\common\include" /I "..\..\..\..\10-common\include\mcu" /I "..\..\..\..\10-common\include\platform" /I "..\..\..\..\10-common\include\system1" /I "..\..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "_AFXDLL" /D "IMT" /Fp"$(INTDIR)\ImtAgent.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ImtAgent.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\..\10-common\lib\debug\win32\ImtAgent.lib" 
LIB32_OBJS= \
	"$(INTDIR)\imt_agent.obj" \
	"$(INTDIR)\imt_config.obj" \
	"$(INTDIR)\imtinit.obj"

"..\..\..\..\10-common\lib\debug\win32\ImtAgent.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ImtAgent.dep")
!INCLUDE "ImtAgent.dep"
!ELSE 
!MESSAGE Warning: cannot find "ImtAgent.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ImtAgent - Win32 Release" || "$(CFG)" == "ImtAgent - Win32 Debug"
SOURCE=..\source\imt_agent.cpp

"$(INTDIR)\imt_agent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\imt_config.cpp

"$(INTDIR)\imt_config.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\imtinit.cpp

"$(INTDIR)\imtinit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

