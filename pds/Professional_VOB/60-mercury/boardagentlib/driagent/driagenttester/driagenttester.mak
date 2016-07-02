# Microsoft Developer Studio Generated NMAKE File, Based on driagenttester.dsp
!IF "$(CFG)" == ""
CFG=DriAgentTester - Win32 Debug
!MESSAGE No configuration specified. Defaulting to DriAgentTester - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "DriAgentTester - Win32 Release" && "$(CFG)" != "DriAgentTester - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "driagenttester.mak" CFG="DriAgentTester - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DriAgentTester - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "DriAgentTester - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "DriAgentTester - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\driagenttester.exe"


CLEAN :
	-@erase "$(INTDIR)\DriAgentTester.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\driagenttester.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\driagenttester.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\driagenttester.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\driagenttester.pdb" /machine:I386 /out:"$(OUTDIR)\driagenttester.exe" 
LINK32_OBJS= \
	"$(INTDIR)\DriAgentTester.obj" \
	"..\..\..\..\10-common\lib\debug\win32\kdvsyslib.lib" \
	"..\..\..\..\10-Common\lib\debug\win32\OspLib.lib" \
	"..\..\..\..\10-Common\version\test\UnitTest\Service2\boardagent\driagent.lib"

"$(OUTDIR)\driagenttester.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "DriAgentTester - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\10-common\version\test\unittest\service2\boardagent\driagtstub.exe"


CLEAN :
	-@erase "$(INTDIR)\DriAgentTester.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\driagtstub.pdb"
	-@erase "..\..\..\..\10-common\version\test\unittest\service2\boardagent\driagtstub.exe"
	-@erase "..\..\..\..\10-common\version\test\unittest\service2\boardagent\driagtstub.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\common" /I "..\..\..\..\10-common\include\mcu" /I "..\..\..\..\10-common\include\platform" /I "..\..\..\..\10-common\include\system1" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\driagenttester.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\driagenttester.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\driagtstub.pdb" /debug /machine:I386 /out:"..\..\..\..\10-common\version\test\unittest\service2\boardagent\driagtstub.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\DriAgentTester.obj" \
	"..\..\..\..\10-common\lib\debug\win32\kdvsyslib.lib" \
	"..\..\..\..\10-Common\lib\debug\win32\OspLib.lib" \
	"..\..\..\..\10-Common\version\test\UnitTest\Service2\boardagent\driagent.lib"

"..\..\..\..\10-common\version\test\unittest\service2\boardagent\driagtstub.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
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
!IF EXISTS("driagenttester.dep")
!INCLUDE "driagenttester.dep"
!ELSE 
!MESSAGE Warning: cannot find "driagenttester.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "DriAgentTester - Win32 Release" || "$(CFG)" == "DriAgentTester - Win32 Debug"
SOURCE=.\DriAgentTester.cpp

"$(INTDIR)\DriAgentTester.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

