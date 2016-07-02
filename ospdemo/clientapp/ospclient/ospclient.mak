# Microsoft Developer Studio Generated NMAKE File, Based on ospclient.dsp
!IF "$(CFG)" == ""
CFG=ospclient - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ospclient - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ospclient - Win32 Release" && "$(CFG)" != "ospclient - Win32 Debug"
!MESSAGE 指定的配置 "$(CFG)" 无效.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ospclient.mak" CFG="ospclient - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ospclient - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ospclient - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ospclient - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\version\release\ospclient.exe"


CLEAN :
	-@erase "$(INTDIR)\clientinstance.obj"
	-@erase "$(INTDIR)\clientospapp.obj"
	-@erase "$(INTDIR)\ospclient.obj"
	-@erase "$(INTDIR)\ospclient.pch"
	-@erase "$(INTDIR)\ospclient.res"
	-@erase "$(INTDIR)\ospclientdlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\version\release\ospclient.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "..\include" /I "..\..\ospcommon" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\ospclient.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\ospclient.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ospclient.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\ospclient.pdb" /machine:I386 /out:"..\version\release\ospclient.exe" 
LINK32_OBJS= \
	"$(INTDIR)\clientinstance.obj" \
	"$(INTDIR)\clientospapp.obj" \
	"$(INTDIR)\ospclient.obj" \
	"$(INTDIR)\ospclientdlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\ospclient.res" \
	".\OspDll.lib"

"..\version\release\ospclient.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\version\release\ospclient.exe"
   del ..\version\release\ospclient.lib
	del ..\version\release\ospclient.exp
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ELSEIF  "$(CFG)" == "ospclient - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\version\debug\ospclient.exe"


CLEAN :
	-@erase "$(INTDIR)\clientinstance.obj"
	-@erase "$(INTDIR)\clientospapp.obj"
	-@erase "$(INTDIR)\ospclient.obj"
	-@erase "$(INTDIR)\ospclient.pch"
	-@erase "$(INTDIR)\ospclient.res"
	-@erase "$(INTDIR)\ospclientdlg.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\ospclient.pdb"
	-@erase "..\version\debug\ospclient.exe"
	-@erase "..\version\debug\ospclient.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\ospcommon" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\ospclient.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\ospclient.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ospclient.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\ospclient.pdb" /debug /machine:I386 /out:"../version/debug/ospclient.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\clientinstance.obj" \
	"$(INTDIR)\clientospapp.obj" \
	"$(INTDIR)\ospclient.obj" \
	"$(INTDIR)\ospclientdlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\ospclient.res" \
	".\OspDll.lib"

"..\version\debug\ospclient.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

SOURCE="$(InputPath)"
DS_POSTBUILD_DEP=$(INTDIR)\postbld.dep

ALL : $(DS_POSTBUILD_DEP)

$(DS_POSTBUILD_DEP) : "..\version\debug\ospclient.exe"
   del ..\version\debug\ospclient.ilk
	del ..\version\debug\ospclient.exp
	del ..\version\debug\ospclient.lib
	echo Helper for Post-build step > "$(DS_POSTBUILD_DEP)"

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ospclient.dep")
!INCLUDE "ospclient.dep"
!ELSE 
!MESSAGE Warning: cannot find "ospclient.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ospclient - Win32 Release" || "$(CFG)" == "ospclient - Win32 Debug"
SOURCE=..\source\clientinstance.cpp

"$(INTDIR)\clientinstance.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\ospclient.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\clientospapp.cpp

"$(INTDIR)\clientospapp.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\ospclient.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ospclient.cpp

"$(INTDIR)\ospclient.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\ospclient.pch"


SOURCE=.\ospclient.rc

"$(INTDIR)\ospclient.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\ospclientdlg.cpp

"$(INTDIR)\ospclientdlg.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\ospclient.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "ospclient - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "..\include" /I "..\..\ospcommon" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\ospclient.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\ospclient.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "ospclient - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\ospcommon" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Fp"$(INTDIR)\ospclient.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\ospclient.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

