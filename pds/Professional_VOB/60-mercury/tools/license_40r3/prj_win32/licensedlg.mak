# Microsoft Developer Studio Generated NMAKE File, Based on licensedlg.dsp
!IF "$(CFG)" == ""
CFG=licensedlg - Win32 Debug
!MESSAGE No configuration specified. Defaulting to licensedlg - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "licensedlg - Win32 Release" && "$(CFG)" != "licensedlg - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "licensedlg.mak" CFG="licensedlg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "licensedlg - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "licensedlg - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "licensedlg - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\licensedlg.exe"


CLEAN :
	-@erase "$(INTDIR)\licensedlg.obj"
	-@erase "$(INTDIR)\licensedlg.res"
	-@erase "$(INTDIR)\licensedlgDlg.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\licensedlg.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "." /I "../include" /I "../../common/include" /I "../../../10-common/include/platform" /I "../../../10-common/include/system1" /I "../../../10-common/include/protocol" /I "../../../10-common/include/mcu" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\licensedlg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\licensedlg.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\licensedlg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=osplib.lib kdvencrypt.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\licensedlg.pdb" /machine:I386 /out:"$(OUTDIR)\licensedlg.exe" /libpath:"../../../10-common/lib/release/win32" 
LINK32_OBJS= \
	"$(INTDIR)\licensedlg.obj" \
	"$(INTDIR)\licensedlgDlg.obj" \
	"$(INTDIR)\licensedlg.res"

"$(OUTDIR)\licensedlg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "licensedlg - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\licensedlg.exe"


CLEAN :
	-@erase "$(INTDIR)\licensedlg.obj"
	-@erase "$(INTDIR)\licensedlg.res"
	-@erase "$(INTDIR)\licensedlgDlg.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\licensedlg.exe"
	-@erase "$(OUTDIR)\licensedlg.ilk"
	-@erase "$(OUTDIR)\licensedlg.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "../include" /I "../../common/include" /I "../../../10-common/include/platform" /I "../../../10-common/include/system1" /I "../../../10-common/include/protocol" /I "../../../10-common/include/mcu" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)\licensedlg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\licensedlg.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\licensedlg.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=osplib.lib kdvencrypt.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\licensedlg.pdb" /debug /machine:I386 /out:"$(OUTDIR)\licensedlg.exe" /pdbtype:sept /libpath:"../../../10-common/lib/debug/win32" 
LINK32_OBJS= \
	"$(INTDIR)\licensedlg.obj" \
	"$(INTDIR)\licensedlgDlg.obj" \
	"$(INTDIR)\licensedlg.res"

"$(OUTDIR)\licensedlg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("licensedlg.dep")
!INCLUDE "licensedlg.dep"
!ELSE 
!MESSAGE Warning: cannot find "licensedlg.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "licensedlg - Win32 Release" || "$(CFG)" == "licensedlg - Win32 Debug"
SOURCE=..\source\licensedlg.cpp

"$(INTDIR)\licensedlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\licensedlg.rc

"$(INTDIR)\licensedlg.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=..\source\licensedlgDlg.cpp

"$(INTDIR)\licensedlgDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

