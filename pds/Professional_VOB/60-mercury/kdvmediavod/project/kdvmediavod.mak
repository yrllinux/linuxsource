# Microsoft Developer Studio Generated NMAKE File, Based on kdvmediavod.dsp
!IF "$(CFG)" == ""
CFG=kdvmediavod - Win32 Debug
!MESSAGE No configuration specified. Defaulting to kdvmediavod - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "kdvmediavod - Win32 Release" && "$(CFG)" != "kdvmediavod - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kdvmediavod.mak" CFG="kdvmediavod - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "kdvmediavod - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "kdvmediavod - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "kdvmediavod - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\kdvmediavod.exe"


CLEAN :
	-@erase "$(INTDIR)\AddUserDlg.obj"
	-@erase "$(INTDIR)\kdvmediavod.obj"
	-@erase "$(INTDIR)\kdvmediavod.pch"
	-@erase "$(INTDIR)\kdvmediavod.res"
	-@erase "$(INTDIR)\kdvmediavodapp.obj"
	-@erase "$(INTDIR)\kdvmediavoddlg.obj"
	-@erase "$(INTDIR)\kdvmediavodins.obj"
	-@erase "$(INTDIR)\kdvmediavodutils.obj"
	-@erase "$(INTDIR)\ServerConfigDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\UserManageDlg.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\kdvmediavod.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\kdvmediavod.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\kdvmediavod.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\kdvmediavod.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=RPCtrl.lib asflib.lib osplib.lib kdvmedianet.lib kdvencrypt.lib kdvsys.lib fcnet.lib enfc.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\kdvmediavod.pdb" /machine:I386 /nodefaultlib:"nafxcw.lib" /out:"$(OUTDIR)\kdvmediavod.exe" /libpath:"..\..\..\10-common\lib\release\win32" 
LINK32_OBJS= \
	"$(INTDIR)\AddUserDlg.obj" \
	"$(INTDIR)\kdvmediavod.obj" \
	"$(INTDIR)\kdvmediavodapp.obj" \
	"$(INTDIR)\kdvmediavoddlg.obj" \
	"$(INTDIR)\kdvmediavodins.obj" \
	"$(INTDIR)\kdvmediavodutils.obj" \
	"$(INTDIR)\ServerConfigDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\UserManageDlg.obj" \
	"$(INTDIR)\kdvmediavod.res"

"$(OUTDIR)\kdvmediavod.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "kdvmediavod - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\kdvmediavod.exe"


CLEAN :
	-@erase "$(INTDIR)\AddUserDlg.obj"
	-@erase "$(INTDIR)\kdvmediavod.obj"
	-@erase "$(INTDIR)\kdvmediavod.pch"
	-@erase "$(INTDIR)\kdvmediavod.res"
	-@erase "$(INTDIR)\kdvmediavodapp.obj"
	-@erase "$(INTDIR)\kdvmediavoddlg.obj"
	-@erase "$(INTDIR)\kdvmediavodins.obj"
	-@erase "$(INTDIR)\kdvmediavodutils.obj"
	-@erase "$(INTDIR)\ServerConfigDlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\UserManageDlg.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\kdvmediavod.exe"
	-@erase "$(OUTDIR)\kdvmediavod.ilk"
	-@erase "$(OUTDIR)\kdvmediavod.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\kdvmediavod.pch" /Yu /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\kdvmediavod.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\kdvmediavod.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=RPCtrl.lib asflib.lib osplib.lib kdvmedianet.lib enfc.lib fcnet.lib kdvencrypt.lib kdvsys.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\kdvmediavod.pdb" /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /out:"$(OUTDIR)\kdvmediavod.exe" /pdbtype:sept /libpath:"..\..\..\10-common\lib\debug\win32" 
LINK32_OBJS= \
	"$(INTDIR)\AddUserDlg.obj" \
	"$(INTDIR)\kdvmediavod.obj" \
	"$(INTDIR)\kdvmediavodapp.obj" \
	"$(INTDIR)\kdvmediavoddlg.obj" \
	"$(INTDIR)\kdvmediavodins.obj" \
	"$(INTDIR)\kdvmediavodutils.obj" \
	"$(INTDIR)\ServerConfigDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\UserManageDlg.obj" \
	"$(INTDIR)\kdvmediavod.res"

"$(OUTDIR)\kdvmediavod.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("kdvmediavod.dep")
!INCLUDE "kdvmediavod.dep"
!ELSE 
!MESSAGE Warning: cannot find "kdvmediavod.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "kdvmediavod - Win32 Release" || "$(CFG)" == "kdvmediavod - Win32 Debug"
SOURCE=.\AddUserDlg.cpp

!IF  "$(CFG)" == "kdvmediavod - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\kdvmediavod.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\AddUserDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\kdvmediavod.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "kdvmediavod - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\kdvmediavod.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\AddUserDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\kdvmediavod.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\source\kdvmediavod.cpp

!IF  "$(CFG)" == "kdvmediavod - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kdvmediavod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "kdvmediavod - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\kdvmediavod.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\kdvmediavod.rc

"$(INTDIR)\kdvmediavod.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\kdvmediavodapp.cpp

!IF  "$(CFG)" == "kdvmediavod - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\kdvmediavod.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kdvmediavodapp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\kdvmediavod.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "kdvmediavod - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\kdvmediavod.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\kdvmediavodapp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\kdvmediavod.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\kdvmediavoddlg.cpp

!IF  "$(CFG)" == "kdvmediavod - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\kdvmediavod.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kdvmediavoddlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\kdvmediavod.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "kdvmediavod - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\kdvmediavod.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\kdvmediavoddlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\kdvmediavod.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\source\kdvmediavodins.cpp

!IF  "$(CFG)" == "kdvmediavod - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kdvmediavodins.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "kdvmediavod - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\kdvmediavodins.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\source\kdvmediavodutils.cpp

!IF  "$(CFG)" == "kdvmediavod - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kdvmediavodutils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "kdvmediavod - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\kdvmediavodutils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\ServerConfigDlg.cpp

!IF  "$(CFG)" == "kdvmediavod - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\kdvmediavod.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ServerConfigDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\kdvmediavod.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "kdvmediavod - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\kdvmediavod.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\ServerConfigDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\kdvmediavod.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "kdvmediavod - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\kdvmediavod.pch" /Yc /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\kdvmediavod.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "kdvmediavod - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\kdvmediavod.pch" /Yc /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\kdvmediavod.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\UserManageDlg.cpp

!IF  "$(CFG)" == "kdvmediavod - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\kdvmediavod.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\UserManageDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\kdvmediavod.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "kdvmediavod - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\\" /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\kdvmediavod.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\UserManageDlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\kdvmediavod.pch"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

