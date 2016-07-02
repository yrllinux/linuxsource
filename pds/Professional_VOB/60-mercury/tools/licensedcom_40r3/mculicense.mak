# Microsoft Developer Studio Generated NMAKE File, Based on mculicense.dsp
!IF "$(CFG)" == ""
CFG=mculicense - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mculicense - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mculicense - Win32 Debug" && "$(CFG)" != "mculicense - Win32 Release MinSize"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mculicense.mak" CFG="mculicense - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mculicense - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mculicense - Win32 Release MinSize" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "mculicense - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\mculicense.dll" ".\mculicense.tlb" ".\mculicense.h" ".\mculicense_i.c" ".\Debug\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\mculicense.obj"
	-@erase "$(INTDIR)\mculicense.pch"
	-@erase "$(INTDIR)\mculicense.res"
	-@erase "$(INTDIR)\OprKey.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mculicense.dll"
	-@erase "$(OUTDIR)\mculicense.exp"
	-@erase "$(OUTDIR)\mculicense.ilk"
	-@erase "$(OUTDIR)\mculicense.lib"
	-@erase "$(OUTDIR)\mculicense.pdb"
	-@erase ".\mculicense.h"
	-@erase ".\mculicense.tlb"
	-@erase ".\mculicense_i.c"
	-@erase ".\Debug\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\10-common\include\mcu" /I "..\..\..\..\10-common\include\platform" /I "..\..\..\..\10-common\include\system1" /I "..\..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\mculicense.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\mculicense.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mculicense.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=osplib.lib kdvencrypt.lib /nologo /subsystem:windows /dll /incremental:yes /pdb:"$(OUTDIR)\mculicense.pdb" /debug /machine:I386 /def:".\mculicense.def" /out:"$(OUTDIR)\mculicense.dll" /implib:"$(OUTDIR)\mculicense.lib" /pdbtype:sept /libpath:"..\..\..\..\10-common\lib\debug\win32" 
DEF_FILE= \
	".\mculicense.def"
LINK32_OBJS= \
	"$(INTDIR)\mculicense.obj" \
	"$(INTDIR)\OprKey.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\mculicense.res"

"$(OUTDIR)\mculicense.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\Debug
TargetPath=.\Debug\mculicense.dll
InputPath=.\Debug\mculicense.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
<< 
	

!ELSEIF  "$(CFG)" == "mculicense - Win32 Release MinSize"

OUTDIR=.\ReleaseMinSize
INTDIR=.\ReleaseMinSize
# Begin Custom Macros
OutDir=.\ReleaseMinSize
# End Custom Macros

ALL : "$(OUTDIR)\mculicense.dll" ".\ReleaseMinSize\regsvr32.trg"


CLEAN :
	-@erase "$(INTDIR)\mculicense.obj"
	-@erase "$(INTDIR)\mculicense.pch"
	-@erase "$(INTDIR)\mculicense.res"
	-@erase "$(INTDIR)\OprKey.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\mculicense.dll"
	-@erase "$(OUTDIR)\mculicense.exp"
	-@erase "$(OUTDIR)\mculicense.lib"
	-@erase ".\mculicense.h"
	-@erase ".\mculicense.tlb"
	-@erase ".\mculicense_i.c"
	-@erase ".\ReleaseMinSize\regsvr32.trg"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O1 /I "..\..\..\..\10-common\include\mcu" /I "..\..\..\..\10-common\include\platform" /I "..\..\..\..\10-common\include\system1" /I "..\..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /Fp"$(INTDIR)\mculicense.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
MTL_PROJ=
RSC=rc.exe
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\mculicense.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mculicense.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=osplib.lib kdvencrypt.lib /nologo /subsystem:windows /dll /incremental:no /pdb:"$(OUTDIR)\mculicense.pdb" /machine:I386 /def:".\mculicense.def" /out:"$(OUTDIR)\mculicense.dll" /implib:"$(OUTDIR)\mculicense.lib" /libpath:"..\..\..\..\10-common\lib\release\win32" 
DEF_FILE= \
	".\mculicense.def"
LINK32_OBJS= \
	"$(INTDIR)\mculicense.obj" \
	"$(INTDIR)\OprKey.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\mculicense.res"

"$(OUTDIR)\mculicense.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

OutDir=.\ReleaseMinSize
TargetPath=.\ReleaseMinSize\mculicense.dll
InputPath=.\ReleaseMinSize\mculicense.dll
SOURCE="$(InputPath)"

"$(OUTDIR)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	<<tempfile.bat 
	@echo off 
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
<< 
	

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("mculicense.dep")
!INCLUDE "mculicense.dep"
!ELSE 
!MESSAGE Warning: cannot find "mculicense.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mculicense - Win32 Debug" || "$(CFG)" == "mculicense - Win32 Release MinSize"
SOURCE=.\mculicense.cpp

"$(INTDIR)\mculicense.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mculicense.pch"


SOURCE=.\mculicense.idl

!IF  "$(CFG)" == "mculicense - Win32 Debug"

MTL_SWITCHES=/tlb ".\mculicense.tlb" /h "mculicense.h" /iid "mculicense_i.c" /Oicf 

".\mculicense.tlb"	".\mculicense.h"	".\mculicense_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mculicense - Win32 Release MinSize"

MTL_SWITCHES=/tlb ".\mculicense.tlb" /h "mculicense.h" /iid "mculicense_i.c" /Oicf 

".\mculicense.tlb"	".\mculicense.h"	".\mculicense_i.c" : $(SOURCE) "$(INTDIR)"
	$(MTL) @<<
  $(MTL_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\mculicense.rc

"$(INTDIR)\mculicense.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\OprKey.cpp

"$(INTDIR)\OprKey.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\mculicense.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "mculicense - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\10-common\include\mcu" /I "..\..\..\..\10-common\include\platform" /I "..\..\..\..\10-common\include\system1" /I "..\..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /Fp"$(INTDIR)\mculicense.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\mculicense.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "mculicense - Win32 Release MinSize"

CPP_SWITCHES=/nologo /MD /W3 /GX /O1 /I "..\..\..\..\10-common\include\mcu" /I "..\..\..\..\10-common\include\platform" /I "..\..\..\..\10-common\include\system1" /I "..\..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_USRDLL" /D "_ATL_DLL" /Fp"$(INTDIR)\mculicense.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\mculicense.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

