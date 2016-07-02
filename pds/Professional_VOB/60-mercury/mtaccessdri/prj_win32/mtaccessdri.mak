# Microsoft Developer Studio Generated NMAKE File, Based on mtaccessdri.dsp
!IF "$(CFG)" == ""
CFG=mtaccessdri - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mtaccessdri - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mtaccessdri - Win32 Release" && "$(CFG)" != "mtaccessdri - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mtaccessdri.mak" CFG="mtaccessdri - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mtaccessdri - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "mtaccessdri - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "mtaccessdri - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : ".\windri.exe"


CLEAN :
	-@erase "$(INTDIR)\mtaccessdri.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase ".\windri.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\mcu" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\mtaccessdri.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mtaccessdri.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=lanman.lib kdvlog.lib osplib.lib kdvsys.lib basicboardagent.lib winbrdwrapper.lib driagent.lib mcuprs.lib mp.lib mtadp.lib kdvnetbuf.lib kdvcascade.lib kdv323.lib kdvmedianet.lib kdvencrypt.lib kdvdatanet.lib kdv323adp.lib dataswitch.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\windri.pdb" /machine:I386 /nodefaultlib:"msvcrt.lib libc.lib nafxcw.lib" /out:"windri.exe" /libpath:"..\..\..\10-common\lib\release\win32" 
LINK32_OBJS= \
	"$(INTDIR)\mtaccessdri.obj"

".\windri.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mtaccessdri - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : ".\windri.exe"


CLEAN :
	-@erase "$(INTDIR)\mtaccessdri.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\windri.pdb"
	-@erase ".\windri.exe"
	-@erase ".\windri.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\common\include" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\mcu" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /Fp"$(INTDIR)\mtaccessdri.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mtaccessdri.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=lanman.lib kdvlog.lib osplib.lib kdvsys.lib basicboardagent.lib winbrdwrapper.lib driagent.lib mcuprs.lib mp.lib mtadp.lib kdvnetbuf.lib kdvcascade.lib kdv323.lib kdvmedianet.lib kdvencrypt.lib kdvdatanet.lib kdv323adp.lib dataswitch.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\windri.pdb" /debug /machine:I386 /nodefaultlib:"msvcrtd.lib libcd.lib nafxcwd.lib" /out:"windri.exe" /pdbtype:sept /libpath:"..\..\..\10-common\lib\debug\win32" 
LINK32_OBJS= \
	"$(INTDIR)\mtaccessdri.obj"

".\windri.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("mtaccessdri.dep")
!INCLUDE "mtaccessdri.dep"
!ELSE 
!MESSAGE Warning: cannot find "mtaccessdri.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mtaccessdri - Win32 Release" || "$(CFG)" == "mtaccessdri - Win32 Debug"
SOURCE=..\source\mtaccessdri.cpp

"$(INTDIR)\mtaccessdri.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

