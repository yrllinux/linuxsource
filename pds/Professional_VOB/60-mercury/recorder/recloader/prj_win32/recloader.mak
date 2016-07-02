# Microsoft Developer Studio Generated NMAKE File, Based on recloader.dsp
!IF "$(CFG)" == ""
CFG=recloader - Win32 Debug
!MESSAGE No configuration specified. Defaulting to recloader - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "recloader - Win32 Release" && "$(CFG)" != "recloader - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "recloader.mak" CFG="recloader - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "recloader - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "recloader - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "recloader - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\recloader.exe"


CLEAN :
	-@erase "$(INTDIR)\recloader.obj"
	-@erase "$(INTDIR)\recloader.pch"
	-@erase "$(INTDIR)\recloader.res"
	-@erase "$(INTDIR)\recloaderdlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\recloader.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "." /I "..\include" /I "..\..\include" /I "..\..\..\..\10-common\include\protocol\\" /I "..\..\..\..\10-common\include\system1" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\recloader.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\recloader.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\recloader.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=Ws2_32.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\recloader.pdb" /machine:I386 /out:"$(OUTDIR)\recloader.exe" 
LINK32_OBJS= \
	"$(INTDIR)\recloader.obj" \
	"$(INTDIR)\recloaderdlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\recloader.res"

"$(OUTDIR)\recloader.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "recloader - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\recloader.exe"


CLEAN :
	-@erase "$(INTDIR)\recloader.obj"
	-@erase "$(INTDIR)\recloader.pch"
	-@erase "$(INTDIR)\recloader.res"
	-@erase "$(INTDIR)\recloaderdlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\recloader.exe"
	-@erase "$(OUTDIR)\recloader.ilk"
	-@erase "$(OUTDIR)\recloader.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "..\include" /I "..\..\include" /I "..\..\..\..\10-common\include\protocol\\" /I "..\..\..\..\10-common\include\system1" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\recloader.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\recloader.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\recloader.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=Ws2_32.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\recloader.pdb" /debug /machine:I386 /out:"$(OUTDIR)\recloader.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\recloader.obj" \
	"$(INTDIR)\recloaderdlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\recloader.res"

"$(OUTDIR)\recloader.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("recloader.dep")
!INCLUDE "recloader.dep"
!ELSE 
!MESSAGE Warning: cannot find "recloader.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "recloader - Win32 Release" || "$(CFG)" == "recloader - Win32 Debug"
SOURCE=..\source\recloader.cpp

"$(INTDIR)\recloader.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\recloader.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\recloader.rc

"$(INTDIR)\recloader.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=..\source\recloaderdlg.cpp

"$(INTDIR)\recloaderdlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\recloader.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "recloader - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "." /I "..\include" /I "..\..\include" /I "..\..\..\..\10-common\include\protocol\\" /I "..\..\..\..\10-common\include\system1" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\recloader.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\recloader.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "recloader - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "..\include" /I "..\..\include" /I "..\..\..\..\10-common\include\protocol\\" /I "..\..\..\..\10-common\include\system1" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\recloader.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\recloader.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

