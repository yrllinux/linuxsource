# Microsoft Developer Studio Generated NMAKE File, Based on McuAgentTest.dsp
!IF "$(CFG)" == ""
CFG=McuAgentTest - Win32 Debug
!MESSAGE No configuration specified. Defaulting to McuAgentTest - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "McuAgentTest - Win32 Release" && "$(CFG)" != "McuAgentTest - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "McuAgentTest.mak" CFG="McuAgentTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "McuAgentTest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "McuAgentTest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "McuAgentTest - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\McuAgentTest.exe"


CLEAN :
	-@erase "$(INTDIR)\AbnormityTest.obj"
	-@erase "$(INTDIR)\BoardAction.obj"
	-@erase "$(INTDIR)\McuAction.obj"
	-@erase "$(INTDIR)\McuAgentTestEntry.obj"
	-@erase "$(INTDIR)\NmsActTest.obj"
	-@erase "$(INTDIR)\SimuBoard.obj"
	-@erase "$(INTDIR)\SimuMcu.obj"
	-@erase "$(INTDIR)\SimuNMS.obj"
	-@erase "$(INTDIR)\SimuSnmpLib.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\McuAgentTest.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\McuAgentTest.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\McuAgentTest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\McuAgentTest.pdb" /machine:I386 /out:"$(OUTDIR)\McuAgentTest.exe" 
LINK32_OBJS= \
	"$(INTDIR)\AbnormityTest.obj" \
	"$(INTDIR)\BoardAction.obj" \
	"$(INTDIR)\McuAction.obj" \
	"$(INTDIR)\McuAgentTestEntry.obj" \
	"$(INTDIR)\NmsActTest.obj" \
	"$(INTDIR)\SimuBoard.obj" \
	"$(INTDIR)\SimuMcu.obj" \
	"$(INTDIR)\SimuNMS.obj" \
	"$(INTDIR)\SimuSnmpLib.obj"

"$(OUTDIR)\McuAgentTest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "McuAgentTest - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\McuAgentTest.exe"


CLEAN :
	-@erase "$(INTDIR)\AbnormityTest.obj"
	-@erase "$(INTDIR)\BoardAction.obj"
	-@erase "$(INTDIR)\McuAction.obj"
	-@erase "$(INTDIR)\McuAgentTestEntry.obj"
	-@erase "$(INTDIR)\NmsActTest.obj"
	-@erase "$(INTDIR)\SimuBoard.obj"
	-@erase "$(INTDIR)\SimuMcu.obj"
	-@erase "$(INTDIR)\SimuNMS.obj"
	-@erase "$(INTDIR)\SimuSnmpLib.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\McuAgentTest.exe"
	-@erase "$(OUTDIR)\McuAgentTest.ilk"
	-@erase "$(OUTDIR)\McuAgentTest.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\..\common" /I "..\..\..\..\..\10-common\include\platform" /I "..\..\..\..\..\10-common\include\mcu" /I "..\..\..\..\..\10-common\include\system1" /I "..\..\..\..\..\10-common\include\cppunit" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\McuAgentTest.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\McuAgentTest.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\McuAgentTest.pdb" /debug /machine:I386 /out:"$(OUTDIR)\McuAgentTest.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\AbnormityTest.obj" \
	"$(INTDIR)\BoardAction.obj" \
	"$(INTDIR)\McuAction.obj" \
	"$(INTDIR)\McuAgentTestEntry.obj" \
	"$(INTDIR)\NmsActTest.obj" \
	"$(INTDIR)\SimuBoard.obj" \
	"$(INTDIR)\SimuMcu.obj" \
	"$(INTDIR)\SimuNMS.obj" \
	"$(INTDIR)\SimuSnmpLib.obj"

"$(OUTDIR)\McuAgentTest.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("McuAgentTest.dep")
!INCLUDE "McuAgentTest.dep"
!ELSE 
!MESSAGE Warning: cannot find "McuAgentTest.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "McuAgentTest - Win32 Release" || "$(CFG)" == "McuAgentTest - Win32 Debug"
SOURCE=..\source\AbnormityTest.cpp

"$(INTDIR)\AbnormityTest.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\McuAgentTest.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\BoardAction.cpp

"$(INTDIR)\BoardAction.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\McuAgentTest.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\McuAction.cpp

"$(INTDIR)\McuAction.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\McuAgentTest.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\McuAgentTestEntry.cpp

"$(INTDIR)\McuAgentTestEntry.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\McuAgentTest.pch"


SOURCE=..\source\NmsActTest.cpp

"$(INTDIR)\NmsActTest.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\McuAgentTest.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\SimuBoard.cpp

"$(INTDIR)\SimuBoard.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\McuAgentTest.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\SimuMcu.cpp

"$(INTDIR)\SimuMcu.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\McuAgentTest.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\SimuNMS.cpp

"$(INTDIR)\SimuNMS.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\McuAgentTest.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\SimuSnmpLib.cpp

"$(INTDIR)\SimuSnmpLib.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\McuAgentTest.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

