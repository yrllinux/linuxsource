# Microsoft Developer Studio Generated NMAKE File, Based on windsc.dsp
!IF "$(CFG)" == ""
CFG=WinDsc - Win32 Debug
!MESSAGE No configuration specified. Defaulting to WinDsc - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "WinDsc - Win32 Release" && "$(CFG)" != "WinDsc - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "windsc.mak" CFG="WinDsc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "WinDsc - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "WinDsc - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "WinDsc - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\windsc.exe"


CLEAN :
	-@erase "$(INTDIR)\mp8000b.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\windsc.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\mcu" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\windsc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\windsc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kdvlog.lib osplib.lib kdvsys.lib basicboardagent.lib winbrdwrapper.lib dscagent.lib lanman.lib mcuprs.lib mp.lib mtadp.lib kdvnetbuf.lib kdvcascade.lib kdv323.lib kdvmedianet.lib kdvencrypt.lib kdvdatanet.lib kdv323adp.lib dataswitch.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\windsc.pdb" /machine:I386 /out:"$(OUTDIR)\windsc.exe" /libpath:"..\..\..\10-common\lib\release\win32" 
LINK32_OBJS= \
	"$(INTDIR)\mp8000b.obj"

"$(OUTDIR)\windsc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "WinDsc - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\windsc.exe"


CLEAN :
	-@erase "$(INTDIR)\mp8000b.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\windsc.exe"
	-@erase "$(OUTDIR)\windsc.ilk"
	-@erase "$(OUTDIR)\windsc.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\mcu" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\windsc.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\windsc.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kdvlog.lib osplib.lib kdvsys.lib basicboardagent.lib winbrdwrapper.lib dscagent.lib lanman.lib mcuprs.lib mp.lib mtadp.lib kdvnetbuf.lib kdvcascade.lib kdv323.lib kdvmedianet.lib kdvencrypt.lib kdvdatanet.lib kdv323adp.lib dataswitch.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\windsc.pdb" /debug /machine:I386 /nodefaultlib:"msvcrtd.lib nafxcwd.lib" /out:"$(OUTDIR)\windsc.exe" /pdbtype:sept /libpath:"..\..\..\10-common\lib\debug\win32" 
LINK32_OBJS= \
	"$(INTDIR)\mp8000b.obj"

"$(OUTDIR)\windsc.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("windsc.dep")
!INCLUDE "windsc.dep"
!ELSE 
!MESSAGE Warning: cannot find "windsc.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "WinDsc - Win32 Release" || "$(CFG)" == "WinDsc - Win32 Debug"
SOURCE=..\source\mp8000b.cpp

"$(INTDIR)\mp8000b.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

