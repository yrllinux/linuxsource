# Microsoft Developer Studio Generated NMAKE File, Based on mcuagtlib.dsp
!IF "$(CFG)" == ""
CFG=mcuagtlib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mcuagtlib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mcuagtlib - Win32 Release" && "$(CFG)" != "mcuagtlib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mcuagtlib.mak" CFG="mcuagtlib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mcuagtlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mcuagtlib - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "mcuagtlib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\10-common\lib\release\win32\mcuagt.lib"


CLEAN :
	-@erase "$(INTDIR)\AgentInterface.obj"
	-@erase "$(INTDIR)\agentmsgcenter.obj"
	-@erase "$(INTDIR)\AgentSnmp.obj"
	-@erase "$(INTDIR)\brdguard.obj"
	-@erase "$(INTDIR)\brdmanager.obj"
	-@erase "$(INTDIR)\configureagent.obj"
	-@erase "$(INTDIR)\ProcAlarm.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\10-common\lib\release\win32\mcuagt.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\snmp" /I "..\..\..\10-common\include\protocol" /D "_LIB" /D "_AFXDLL" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_UTF8" /Fp"$(INTDIR)\mcuagtlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mcuagtlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\release\win32\mcuagt.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AgentInterface.obj" \
	"$(INTDIR)\agentmsgcenter.obj" \
	"$(INTDIR)\AgentSnmp.obj" \
	"$(INTDIR)\brdguard.obj" \
	"$(INTDIR)\brdmanager.obj" \
	"$(INTDIR)\configureagent.obj" \
	"$(INTDIR)\ProcAlarm.obj"

"..\..\..\10-common\lib\release\win32\mcuagt.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mcuagtlib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\10-common\lib\debug\win32\mcuagt.lib"


CLEAN :
	-@erase "$(INTDIR)\AgentInterface.obj"
	-@erase "$(INTDIR)\agentmsgcenter.obj"
	-@erase "$(INTDIR)\AgentSnmp.obj"
	-@erase "$(INTDIR)\brdguard.obj"
	-@erase "$(INTDIR)\brdmanager.obj"
	-@erase "$(INTDIR)\configureagent.obj"
	-@erase "$(INTDIR)\ProcAlarm.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\10-common\lib\debug\win32\mcuagt.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\snmp" /I "..\..\..\10-common\include\protocol" /D "_LIB" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_UTF8" /Fp"$(INTDIR)\mcuagtlib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mcuagtlib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\debug\win32\mcuagt.lib" 
LIB32_OBJS= \
	"$(INTDIR)\AgentInterface.obj" \
	"$(INTDIR)\agentmsgcenter.obj" \
	"$(INTDIR)\AgentSnmp.obj" \
	"$(INTDIR)\brdguard.obj" \
	"$(INTDIR)\brdmanager.obj" \
	"$(INTDIR)\configureagent.obj" \
	"$(INTDIR)\ProcAlarm.obj"

"..\..\..\10-common\lib\debug\win32\mcuagt.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
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
!IF EXISTS("mcuagtlib.dep")
!INCLUDE "mcuagtlib.dep"
!ELSE 
!MESSAGE Warning: cannot find "mcuagtlib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mcuagtlib - Win32 Release" || "$(CFG)" == "mcuagtlib - Win32 Debug"
SOURCE=..\source\AgentInterface.cpp

"$(INTDIR)\AgentInterface.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\agentmsgcenter.cpp

"$(INTDIR)\agentmsgcenter.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\AgentSnmp.cpp

"$(INTDIR)\AgentSnmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\brdguard.cpp

"$(INTDIR)\brdguard.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\brdmanager.cpp

"$(INTDIR)\brdmanager.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\configureagent.cpp

"$(INTDIR)\configureagent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\ProcAlarm.cpp

"$(INTDIR)\ProcAlarm.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

