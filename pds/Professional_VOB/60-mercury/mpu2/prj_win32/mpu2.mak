# Microsoft Developer Studio Generated NMAKE File, Based on mpu2.dsp
!IF "$(CFG)" == ""
CFG=mpu2 - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mpu2 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mpu2 - Win32 Release" && "$(CFG)" != "mpu2 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mpu2.mak" CFG="mpu2 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mpu2 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "mpu2 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "mpu2 - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\mpu2.exe"

!ELSE 

ALL : "mpu2lib - Win32 Release" "$(OUTDIR)\mpu2.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"mpu2lib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\mpu2.obj"
	-@erase "$(INTDIR)\mpu2agent.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\mpu2.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\mpu2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mpu2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\mpu2.pdb" /machine:I386 /out:"$(OUTDIR)\mpu2.exe" 
LINK32_OBJS= \
	"$(INTDIR)\mpu2.obj" \
	"$(INTDIR)\mpu2agent.obj" \
	"..\..\mpu2lib\prj_win32\Release\mpu2lib.lib"

"$(OUTDIR)\mpu2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mpu2 - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\mpu2.exe"

!ELSE 

ALL : "mpu2lib - Win32 Debug" "$(OUTDIR)\mpu2.exe"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"mpu2lib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\mpu2.obj"
	-@erase "$(INTDIR)\mpu2agent.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mpu2.exe"
	-@erase "$(OUTDIR)\mpu2.ilk"
	-@erase "$(OUTDIR)\mpu2.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-Common\include\platform" /I "..\..\..\10-Common\include\mcu" /I "..\..\..\10-Common\include\system1" /I "..\..\..\10-Common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\mpu2.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mpu2.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib mpu2lib.lib drawbmp.lib lanman.lib freetype.lib commonlib.lib basicboardagent.lib kdvlog.lib osplib.lib kdvsys.lib winbrdwrapper.lib mcueqpsimu.lib kdvmedianet.lib fcnet.lib enfc.lib kdvencrypt.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\mpu2.pdb" /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /out:"$(OUTDIR)\mpu2.exe" /pdbtype:sept /libpath:"..\..\..\10-common\lib\debug\win32" /libpath:"..\..\..\10-common\lib\debug\win32_816x" 
LINK32_OBJS= \
	"$(INTDIR)\mpu2.obj" \
	"$(INTDIR)\mpu2agent.obj" \
	"..\..\..\10-common\lib\debug\win32_816x\mpu2lib.lib"

"$(OUTDIR)\mpu2.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("mpu2.dep")
!INCLUDE "mpu2.dep"
!ELSE 
!MESSAGE Warning: cannot find "mpu2.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mpu2 - Win32 Release" || "$(CFG)" == "mpu2 - Win32 Debug"
SOURCE=..\source\mpu2.cpp

"$(INTDIR)\mpu2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpu2agent.cpp

"$(INTDIR)\mpu2agent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!IF  "$(CFG)" == "mpu2 - Win32 Release"

"mpu2lib - Win32 Release" : 
   cd "\svn_root\pds\Professional_VOB\60-mercury\mpu2lib\prj_win32"
   $(MAKE) /$(MAKEFLAGS) /F ".\mpu2lib.mak" CFG="mpu2lib - Win32 Release" 
   cd "..\..\mpu2\prj_win32"

"mpu2lib - Win32 ReleaseCLEAN" : 
   cd "\svn_root\pds\Professional_VOB\60-mercury\mpu2lib\prj_win32"
   $(MAKE) /$(MAKEFLAGS) /F ".\mpu2lib.mak" CFG="mpu2lib - Win32 Release" RECURSE=1 CLEAN 
   cd "..\..\mpu2\prj_win32"

!ELSEIF  "$(CFG)" == "mpu2 - Win32 Debug"

"mpu2lib - Win32 Debug" : 
   cd "\svn_root\pds\Professional_VOB\60-mercury\mpu2lib\prj_win32"
   $(MAKE) /$(MAKEFLAGS) /F ".\mpu2lib.mak" CFG="mpu2lib - Win32 Debug" 
   cd "..\..\mpu2\prj_win32"

"mpu2lib - Win32 DebugCLEAN" : 
   cd "\svn_root\pds\Professional_VOB\60-mercury\mpu2lib\prj_win32"
   $(MAKE) /$(MAKEFLAGS) /F ".\mpu2lib.mak" CFG="mpu2lib - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\..\mpu2\prj_win32"

!ENDIF 


!ENDIF 

