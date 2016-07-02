# Microsoft Developer Studio Generated NMAKE File, Based on mcu.dsp
!IF "$(CFG)" == ""
CFG=mcu - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mcu - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mcu - Win32 Debug" && "$(CFG)" != "mcu - Win32 Unicode Debug" && "$(CFG)" != "mcu - Win32 Release MinSize" && "$(CFG)" != "mcu - Win32 Release MinDependency" && "$(CFG)" != "mcu - Win32 Unicode Release MinSize" && "$(CFG)" != "mcu - Win32 Unicode Release MinDependency"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mcu.mak" CFG="mcu - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mcu - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mcu - Win32 Unicode Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mcu - Win32 Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mcu - Win32 Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mcu - Win32 Unicode Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mcu - Win32 Unicode Release MinDependency" (based on "Win32 (x86) Dynamic-Link Library")
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

!IF  "$(CFG)" == "mcu - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\mcu.dll" ".\Debug\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\ConfData.obj"
	-@erase "$(INTDIR)\ConfProperty.obj"
	-@erase "$(INTDIR)\MCCtrl.obj"
	-@erase "$(INTDIR)\MCStatusCallback.obj"
	-@erase "$(INTDIR)\MCStreamCallback.obj"
	-@erase "$(INTDIR)\mcu.obj"
	-@erase "$(INTDIR)\mcu.pch"
	-@erase "$(INTDIR)\mcu.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mcu.dll"
	-@erase "$(OUTDIR)\mcu.exp"
	-@erase "$(OUTDIR)\mcu.ilk"
	-@erase "$(OUTDIR)\mcu.lib"
	-@erase "$(OUTDIR)\mcu.pdb"
	-@erase ".\mcu.h"
	-@erase ".\mcu.tlb"
	-@erase ".\mcu_i.c"
	-@erase ".\Debug\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /ZI /Od /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\mcu.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\mcu.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mcu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib osplib.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\mcu.pdb" /debug /machine:I386 /nodefaultlib:"LIBCMTD.lib" /def:".\mcu.def" /out:"$(OUTDIR)\mcu.dll" /implib:"$(OUTDIR)\mcu.lib" /pdbtype:sept /libpath:"..\..\..\10-common\lib\debug\win32" 
DEF_FILE= \
	".\mcu.def"
LINK32_OBJS= \
	"$(INTDIR)\ConfData.obj" \
	"$(INTDIR)\ConfProperty.obj" \
	"$(INTDIR)\MCCtrl.obj" \
	"$(INTDIR)\MCStatusCallback.obj" \
	"$(INTDIR)\MCStreamCallback.obj" \
	"$(INTDIR)\mcu.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\mcu.res"

"$(OUTDIR)\mcu.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\Debug
TargetPath=.\Debug\mcu.dll
InputPath=.\Debug\mcu.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
<< 
	

!ELSEIF  "$(CFG)" == "mcu - Win32 Unicode Debug"

OUTDIR=.\DebugU
INTDIR=.\DebugU
# Begin Custom Macros
OutDir=.\DebugU
# End Custom Macros

ALL : "$(OUTDIR)\mcu.dll" ".\mcu.tlb" ".\mcu.h" ".\mcu_i.c" ".\DebugU\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\ConfData.obj"
	-@erase "$(INTDIR)\ConfProperty.obj"
	-@erase "$(INTDIR)\MCCtrl.obj"
	-@erase "$(INTDIR)\MCStatusCallback.obj"
	-@erase "$(INTDIR)\MCStreamCallback.obj"
	-@erase "$(INTDIR)\mcu.obj"
	-@erase "$(INTDIR)\mcu.pch"
	-@erase "$(INTDIR)\mcu.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mcu.dll"
	-@erase "$(OUTDIR)\mcu.exp"
	-@erase "$(OUTDIR)\mcu.ilk"
	-@erase "$(OUTDIR)\mcu.lib"
	-@erase "$(OUTDIR)\mcu.pdb"
	-@erase ".\mcu.h"
	-@erase ".\mcu.tlb"
	-@erase ".\mcu_i.c"
	-@erase ".\DebugU\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /ZI /Od /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /Fp"$(INTDIR)\mcu.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\mcu.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mcu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\mcu.pdb" /debug /machine:I386 /def:".\mcu.def" /out:"$(OUTDIR)\mcu.dll" /implib:"$(OUTDIR)\mcu.lib" /pdbtype:sept 
DEF_FILE= \
	".\mcu.def"
LINK32_OBJS= \
	"$(INTDIR)\ConfData.obj" \
	"$(INTDIR)\ConfProperty.obj" \
	"$(INTDIR)\MCCtrl.obj" \
	"$(INTDIR)\MCStatusCallback.obj" \
	"$(INTDIR)\MCStreamCallback.obj" \
	"$(INTDIR)\mcu.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\mcu.res"

"$(OUTDIR)\mcu.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\DebugU
TargetPath=.\DebugU\mcu.dll
InputPath=.\DebugU\mcu.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
<< 
	

!ELSEIF  "$(CFG)" == "mcu - Win32 Release MinSize"

OUTDIR=.\ReleaseMinSize
INTDIR=.\ReleaseMinSize
# Begin Custom Macros
OutDir=.\ReleaseMinSize
# End Custom Macros

ALL : "$(OUTDIR)\mcu.dll" ".\ReleaseMinSize\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\ConfData.obj"
	-@erase "$(INTDIR)\ConfProperty.obj"
	-@erase "$(INTDIR)\MCCtrl.obj"
	-@erase "$(INTDIR)\MCStatusCallback.obj"
	-@erase "$(INTDIR)\MCStreamCallback.obj"
	-@erase "$(INTDIR)\mcu.obj"
	-@erase "$(INTDIR)\mcu.pch"
	-@erase "$(INTDIR)\mcu.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\mcu.dll"
	-@erase "$(OUTDIR)\mcu.exp"
	-@erase "$(OUTDIR)\mcu.lib"
	-@erase ".\mcu.h"
	-@erase ".\mcu.tlb"
	-@erase ".\mcu_i.c"
	-@erase ".\ReleaseMinSize\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /O1 /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Fp"$(INTDIR)\mcu.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\mcu.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mcu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  osplib.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\mcu.pdb" /machine:I386 /nodefaultlib:"libcmt.lib msvcrt.lib" /def:".\mcu.def" /out:"$(OUTDIR)\mcu.dll" /implib:"$(OUTDIR)\mcu.lib" /libpath:"..\..\..\10-common\lib\release\win32" 
DEF_FILE= \
	".\mcu.def"
LINK32_OBJS= \
	"$(INTDIR)\ConfData.obj" \
	"$(INTDIR)\ConfProperty.obj" \
	"$(INTDIR)\MCCtrl.obj" \
	"$(INTDIR)\MCStatusCallback.obj" \
	"$(INTDIR)\MCStreamCallback.obj" \
	"$(INTDIR)\mcu.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\mcu.res"

"$(OUTDIR)\mcu.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\ReleaseMinSize
TargetPath=.\ReleaseMinSize\mcu.dll
InputPath=.\ReleaseMinSize\mcu.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
<< 
	

!ELSEIF  "$(CFG)" == "mcu - Win32 Release MinDependency"

OUTDIR=.\ReleaseMinDependency
INTDIR=.\ReleaseMinDependency
# Begin Custom Macros
OutDir=.\ReleaseMinDependency
# End Custom Macros

ALL : "$(OUTDIR)\mcu.dll" ".\ReleaseMinDependency\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\ConfData.obj"
	-@erase "$(INTDIR)\ConfProperty.obj"
	-@erase "$(INTDIR)\MCCtrl.obj"
	-@erase "$(INTDIR)\MCStatusCallback.obj"
	-@erase "$(INTDIR)\MCStreamCallback.obj"
	-@erase "$(INTDIR)\mcu.obj"
	-@erase "$(INTDIR)\mcu.pch"
	-@erase "$(INTDIR)\mcu.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\mcu.dll"
	-@erase "$(OUTDIR)\mcu.exp"
	-@erase "$(OUTDIR)\mcu.lib"
	-@erase ".\mcu.h"
	-@erase ".\mcu.tlb"
	-@erase ".\mcu_i.c"
	-@erase ".\ReleaseMinDependency\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /O1 /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Fp"$(INTDIR)\mcu.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\mcu.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mcu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib  osplib.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\mcu.pdb" /machine:I386 /nodefaultlib:"libcmt.lib" /def:".\mcu.def" /out:"$(OUTDIR)\mcu.dll" /implib:"$(OUTDIR)\mcu.lib" /libpath:"..\..\..\10-common\lib\release\win32" 
DEF_FILE= \
	".\mcu.def"
LINK32_OBJS= \
	"$(INTDIR)\ConfData.obj" \
	"$(INTDIR)\ConfProperty.obj" \
	"$(INTDIR)\MCCtrl.obj" \
	"$(INTDIR)\MCStatusCallback.obj" \
	"$(INTDIR)\MCStreamCallback.obj" \
	"$(INTDIR)\mcu.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\mcu.res"

"$(OUTDIR)\mcu.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\ReleaseMinDependency
TargetPath=.\ReleaseMinDependency\mcu.dll
InputPath=.\ReleaseMinDependency\mcu.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
<< 
	

!ELSEIF  "$(CFG)" == "mcu - Win32 Unicode Release MinSize"

OUTDIR=.\ReleaseUMinSize
INTDIR=.\ReleaseUMinSize
# Begin Custom Macros
OutDir=.\ReleaseUMinSize
# End Custom Macros

ALL : "$(OUTDIR)\mcu.dll" ".\mcu.tlb" ".\mcu.h" ".\mcu_i.c" ".\ReleaseUMinSize\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\ConfData.obj"
	-@erase "$(INTDIR)\ConfProperty.obj"
	-@erase "$(INTDIR)\MCCtrl.obj"
	-@erase "$(INTDIR)\MCStatusCallback.obj"
	-@erase "$(INTDIR)\MCStreamCallback.obj"
	-@erase "$(INTDIR)\mcu.obj"
	-@erase "$(INTDIR)\mcu.pch"
	-@erase "$(INTDIR)\mcu.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\mcu.dll"
	-@erase "$(OUTDIR)\mcu.exp"
	-@erase "$(OUTDIR)\mcu.lib"
	-@erase ".\mcu.h"
	-@erase ".\mcu.tlb"
	-@erase ".\mcu_i.c"
	-@erase ".\ReleaseUMinSize\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /O1 /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Fp"$(INTDIR)\mcu.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\mcu.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mcu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\mcu.pdb" /machine:I386 /def:".\mcu.def" /out:"$(OUTDIR)\mcu.dll" /implib:"$(OUTDIR)\mcu.lib" 
DEF_FILE= \
	".\mcu.def"
LINK32_OBJS= \
	"$(INTDIR)\ConfData.obj" \
	"$(INTDIR)\ConfProperty.obj" \
	"$(INTDIR)\MCCtrl.obj" \
	"$(INTDIR)\MCStatusCallback.obj" \
	"$(INTDIR)\MCStreamCallback.obj" \
	"$(INTDIR)\mcu.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\mcu.res"

"$(OUTDIR)\mcu.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\ReleaseUMinSize
TargetPath=.\ReleaseUMinSize\mcu.dll
InputPath=.\ReleaseUMinSize\mcu.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
<< 
	

!ELSEIF  "$(CFG)" == "mcu - Win32 Unicode Release MinDependency"

OUTDIR=.\ReleaseUMinDependency
INTDIR=.\ReleaseUMinDependency
# Begin Custom Macros
OutDir=.\ReleaseUMinDependency
# End Custom Macros

ALL : "$(OUTDIR)\mcu.dll" ".\mcu.tlb" ".\mcu.h" ".\mcu_i.c" ".\ReleaseUMinDependency\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\ConfData.obj"
	-@erase "$(INTDIR)\ConfProperty.obj"
	-@erase "$(INTDIR)\MCCtrl.obj"
	-@erase "$(INTDIR)\MCStatusCallback.obj"
	-@erase "$(INTDIR)\MCStreamCallback.obj"
	-@erase "$(INTDIR)\mcu.obj"
	-@erase "$(INTDIR)\mcu.pch"
	-@erase "$(INTDIR)\mcu.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\mcu.dll"
	-@erase "$(OUTDIR)\mcu.exp"
	-@erase "$(OUTDIR)\mcu.lib"
	-@erase ".\mcu.h"
	-@erase ".\mcu.tlb"
	-@erase ".\mcu_i.c"
	-@erase ".\ReleaseUMinDependency\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /O1 /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Fp"$(INTDIR)\mcu.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\mcu.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mcu.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\mcu.pdb" /machine:I386 /def:".\mcu.def" /out:"$(OUTDIR)\mcu.dll" /implib:"$(OUTDIR)\mcu.lib" 
DEF_FILE= \
	".\mcu.def"
LINK32_OBJS= \
	"$(INTDIR)\ConfData.obj" \
	"$(INTDIR)\ConfProperty.obj" \
	"$(INTDIR)\MCCtrl.obj" \
	"$(INTDIR)\MCStatusCallback.obj" \
	"$(INTDIR)\MCStreamCallback.obj" \
	"$(INTDIR)\mcu.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\mcu.res"

"$(OUTDIR)\mcu.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\ReleaseUMinDependency
TargetPath=.\ReleaseUMinDependency\mcu.dll
InputPath=.\ReleaseUMinDependency\mcu.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	if "%OS%"=="" goto NOTNT 
	if not "%OS%"=="Windows_NT" goto NOTNT 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	goto end 
	:NOTNT 
	echo Warning : Cannot register Unicode DLL on Windows 95 
	:end 
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

MTL_PROJ=

!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("mcu.dep")
!INCLUDE "mcu.dep"
!ELSE 
!MESSAGE Warning: cannot find "mcu.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mcu - Win32 Debug" || "$(CFG)" == "mcu - Win32 Unicode Debug" || "$(CFG)" == "mcu - Win32 Release MinSize" || "$(CFG)" == "mcu - Win32 Release MinDependency" || "$(CFG)" == "mcu - Win32 Unicode Release MinSize" || "$(CFG)" == "mcu - Win32 Unicode Release MinDependency"
SOURCE=.\ConfData.cpp

"$(INTDIR)\ConfData.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mcu.pch"


SOURCE=.\ConfProperty.cpp

"$(INTDIR)\ConfProperty.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mcu.pch" ".\mcu.h"


SOURCE=.\MCCtrl.cpp

"$(INTDIR)\MCCtrl.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mcu.pch" ".\mcu.h"


SOURCE=.\MCStatusCallback.cpp

"$(INTDIR)\MCStatusCallback.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mcu.pch" ".\mcu.h"


SOURCE=.\MCStreamCallback.cpp

"$(INTDIR)\MCStreamCallback.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mcu.pch" ".\mcu.h"


SOURCE=.\mcu.cpp

"$(INTDIR)\mcu.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mcu.pch" ".\mcu.h" ".\mcu_i.c"


SOURCE=.\mcu.idl
MTL_SWITCHES=/tlb ".\mcu.tlb" /h "mcu.h" /iid "mcu_i.c" /Oicf 

".\mcu.tlb"	".\mcu.h"	".\mcu_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


SOURCE=.\mcu.rc

"$(INTDIR)\mcu.res" : $(SOURCE) "$(INTDIR)" ".\mcu.tlb"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "mcu - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /ZI /Od /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /I "..\..\common\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\mcu.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\mcu.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mcu - Win32 Unicode Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /ZI /Od /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /Fp"$(INTDIR)\mcu.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\mcu.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mcu - Win32 Release MinSize"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Fp"$(INTDIR)\mcu.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\mcu.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mcu - Win32 Release MinDependency"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_ATL_STATIC_REGISTRY" /Fp"$(INTDIR)\mcu.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\mcu.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mcu - Win32 Unicode Release MinSize"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_DLL" /D "_ATL_MIN_CRT" /Fp"$(INTDIR)\mcu.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\mcu.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mcu - Win32 Unicode Release MinDependency"

CPP_SWITCHES=/nologo /MT /W3 /O1 /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_USRDLL" /D "_UNICODE" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /Fp"$(INTDIR)\mcu.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\mcu.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

