# Microsoft Developer Studio Generated NMAKE File, Based on ospserver.dsp
!IF "$(CFG)" == ""
CFG=ospserver - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ospserver - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ospserver - Win32 Release" && "$(CFG)" != "ospserver - Win32 Debug"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ospserver.mak" CFG="ospserver - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ospserver - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ospserver - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ospserver - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\version\release\ospserver.exe"


CLEAN :
	-@erase "$(INTDIR)\ospserver.obj"
	-@erase "$(INTDIR)\ospserver.pch"
	-@erase "$(INTDIR)\ospserver.res"
	-@erase "$(INTDIR)\ospserverdlg.obj"
	-@erase "$(INTDIR)\serverinstance.obj"
	-@erase "$(INTDIR)\serverospapp.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\version\release\ospserver.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\include" /I "..\..\ospcommon" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\ospserver.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\ospserver.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ospserver.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\ospserver.pdb" /machine:I386 /out:"../version/release/ospserver.exe" 
LINK32_OBJS= \
	"$(INTDIR)\ospserver.obj" \
	"$(INTDIR)\ospserverdlg.obj" \
	"$(INTDIR)\serverinstance.obj" \
	"$(INTDIR)\serverospapp.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\ospserver.res" \
	".\OspDll.lib"

"..\version\release\ospserver.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\version\release\ospserver.exe"
   del ..\version\release\ospserver.lib
	del ..\version\release\ospserver.exp
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "ospserver - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\version\debug\ospserver.exe"


CLEAN :
	-@erase "$(INTDIR)\ospserver.obj"
	-@erase "$(INTDIR)\ospserver.pch"
	-@erase "$(INTDIR)\ospserver.res"
	-@erase "$(INTDIR)\ospserverdlg.obj"
	-@erase "$(INTDIR)\serverinstance.obj"
	-@erase "$(INTDIR)\serverospapp.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ospserver.pdb"
	-@erase "..\version\debug\ospserver.exe"
	-@erase "..\version\debug\ospserver.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\ospcommon" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\ospserver.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\ospserver.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ospserver.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\ospserver.pdb" /debug /machine:I386 /out:"../version/debug/ospserver.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\ospserver.obj" \
	"$(INTDIR)\ospserverdlg.obj" \
	"$(INTDIR)\serverinstance.obj" \
	"$(INTDIR)\serverospapp.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\ospserver.res" \
	".\OspDll.lib"

"..\version\debug\ospserver.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\version\debug\ospserver.exe"
   del ..\version\debug\ospserver.exp
	del ..\version\debug\ospserver.lib
	del ..\version\debug\ospserver.ilk
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ospserver.dep")
!INCLUDE "ospserver.dep"
!ELSE 
!MESSAGE Warning: cannot find "ospserver.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ospserver - Win32 Release" || "$(CFG)" == "ospserver - Win32 Debug"
SOURCE=.\ospserver.cpp

"$(INTDIR)\ospserver.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\ospserver.pch"


SOURCE=.\ospserver.rc

"$(INTDIR)\ospserver.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\ospserverdlg.cpp

"$(INTDIR)\ospserverdlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\ospserver.pch"


SOURCE=..\source\serverinstance.cpp

"$(INTDIR)\serverinstance.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\ospserver.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\serverospapp.cpp

"$(INTDIR)\serverospapp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\ospserver.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "ospserver - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\include" /I "..\..\ospcommon" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\ospserver.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\ospserver.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "ospserver - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\ospcommon" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\ospserver.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\ospserver.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

