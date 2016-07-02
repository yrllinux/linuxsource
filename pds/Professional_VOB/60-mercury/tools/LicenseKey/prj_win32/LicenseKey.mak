# Microsoft Developer Studio Generated NMAKE File, Based on LicenseKey.dsp
!IF "$(CFG)" == ""
CFG=LicenseKey - Win32 Debug
!MESSAGE No configuration specified. Defaulting to LicenseKey - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "LicenseKey - Win32 Release" && "$(CFG)" != "LicenseKey - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LicenseKey.mak" CFG="LicenseKey - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LicenseKey - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "LicenseKey - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "LicenseKey - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\LicenseKey.exe"


CLEAN :
	-@erase "$(INTDIR)\LicenseKey.obj"
	-@erase "$(INTDIR)\LicenseKey.res"
	-@erase "$(INTDIR)\LicenseKeySheet.obj"
	-@erase "$(INTDIR)\LicensePage.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\LicenseKey.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "." /I "..\include" /I "..\..\..\..\10-common\include\mcu" /I "..\..\..\..\10-common\include\platform" /I "..\..\..\..\10-common\include\system1" /I "..\..\..\..\10-common\include\cbb\license" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\LicenseKey.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LicenseKey.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=osplib.lib kdvsys.lib kdvencrypt.lib readlicenselib.lib hkapi.lib  ep1kdl20.lib rockey2.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\LicenseKey.pdb" /machine:I386 /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\LicenseKey.exe" /libpath:"..\..\..\..\10-Common\lib\release\win32" /libpath:"..\..\..\..\b0-thirdparty\haikey\lib\release\win32" 
LINK32_OBJS= \
	"$(INTDIR)\LicenseKey.obj" \
	"$(INTDIR)\LicenseKeySheet.obj" \
	"$(INTDIR)\LicensePage.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\LicenseKey.res"

"$(OUTDIR)\LicenseKey.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "LicenseKey - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\LicenseKey.exe"


CLEAN :
	-@erase "$(INTDIR)\LicenseKey.obj"
	-@erase "$(INTDIR)\LicenseKey.res"
	-@erase "$(INTDIR)\LicenseKeySheet.obj"
	-@erase "$(INTDIR)\LicensePage.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\LicenseKey.exe"
	-@erase "$(OUTDIR)\LicenseKey.ilk"
	-@erase "$(OUTDIR)\LicenseKey.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "..\include" /I "..\..\..\..\10-common\include\mcu" /I "..\..\..\..\10-common\include\platform" /I "..\..\..\..\10-common\include\system1" /I "..\..\..\..\10-common\include\cbb\license" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\LicenseKey.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\LicenseKey.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=osplib.lib kdvsys.lib kdvencrypt.lib readlicenselib.lib hkapi.lib ep1kdl20.lib rockey2.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\LicenseKey.pdb" /debug /machine:I386 /out:"$(OUTDIR)\LicenseKey.exe" /pdbtype:sept /libpath:"..\..\..\..\10-Common\lib\debug\win32" /libpath:"..\..\..\..\b0-thirdparty\haikey\lib\debug\win32" 
LINK32_OBJS= \
	"$(INTDIR)\LicenseKey.obj" \
	"$(INTDIR)\LicenseKeySheet.obj" \
	"$(INTDIR)\LicensePage.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\LicenseKey.res"

"$(OUTDIR)\LicenseKey.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("LicenseKey.dep")
!INCLUDE "LicenseKey.dep"
!ELSE 
!MESSAGE Warning: cannot find "LicenseKey.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "LicenseKey - Win32 Release" || "$(CFG)" == "LicenseKey - Win32 Debug"
SOURCE=..\source\LicenseKey.cpp

"$(INTDIR)\LicenseKey.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\LicenseKey.rc

"$(INTDIR)\LicenseKey.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=..\source\LicenseKeySheet.cpp

"$(INTDIR)\LicenseKeySheet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\LicensePage.cpp

"$(INTDIR)\LicensePage.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

"$(INTDIR)\StdAfx.obj" : $(SOURCE) "$(INTDIR)"



!ENDIF 

