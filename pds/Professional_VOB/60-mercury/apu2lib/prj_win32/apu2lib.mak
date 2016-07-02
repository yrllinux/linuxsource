# Microsoft Developer Studio Generated NMAKE File, Based on apu2lib.dsp
!IF "$(CFG)" == ""
CFG=apu2lib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to apu2lib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "apu2lib - Win32 Release" && "$(CFG)" != "apu2lib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "apu2lib.mak" CFG="apu2lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "apu2lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "apu2lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "apu2lib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\10-common\lib\release\win32\apu2lib.lib"


CLEAN :
	-@erase "$(INTDIR)\apu2baschnnl.obj"
	-@erase "$(INTDIR)\apu2baseqp.obj"
	-@erase "$(INTDIR)\apu2basinst.obj"
	-@erase "$(INTDIR)\apu2chnnl.obj"
	-@erase "$(INTDIR)\apu2eqp.obj"
	-@erase "$(INTDIR)\apu2inst.obj"
	-@erase "$(INTDIR)\libInit.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\10-common\lib\release\win32\apu2lib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\..\..\10-common\include\platform" /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\mt" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\eqpbaselib\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\apu2lib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\apu2lib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\release\win32\apu2lib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\apu2baschnnl.obj" \
	"$(INTDIR)\apu2baseqp.obj" \
	"$(INTDIR)\apu2basinst.obj" \
	"$(INTDIR)\apu2chnnl.obj" \
	"$(INTDIR)\apu2eqp.obj" \
	"$(INTDIR)\apu2inst.obj" \
	"$(INTDIR)\libInit.obj"

"..\..\..\10-common\lib\release\win32\apu2lib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "apu2lib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\10-common\lib\debug\win32\apu2lib.lib"


CLEAN :
	-@erase "$(INTDIR)\apu2baschnnl.obj"
	-@erase "$(INTDIR)\apu2baseqp.obj"
	-@erase "$(INTDIR)\apu2basinst.obj"
	-@erase "$(INTDIR)\apu2chnnl.obj"
	-@erase "$(INTDIR)\apu2eqp.obj"
	-@erase "$(INTDIR)\apu2inst.obj"
	-@erase "$(INTDIR)\libInit.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\10-common\lib\debug\win32\apu2lib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\..\..\10-common\include\platform" /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\mt" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\eqpbaselib\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\apu2lib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\apu2lib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\debug\win32\apu2lib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\apu2baschnnl.obj" \
	"$(INTDIR)\apu2baseqp.obj" \
	"$(INTDIR)\apu2basinst.obj" \
	"$(INTDIR)\apu2chnnl.obj" \
	"$(INTDIR)\apu2eqp.obj" \
	"$(INTDIR)\apu2inst.obj" \
	"$(INTDIR)\libInit.obj"

"..\..\..\10-common\lib\debug\win32\apu2lib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("apu2lib.dep")
!INCLUDE "apu2lib.dep"
!ELSE 
!MESSAGE Warning: cannot find "apu2lib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "apu2lib - Win32 Release" || "$(CFG)" == "apu2lib - Win32 Debug"
SOURCE=..\source\apu2baschnnl.cpp

"$(INTDIR)\apu2baschnnl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\apu2baseqp.cpp

"$(INTDIR)\apu2baseqp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\apu2basinst.cpp

"$(INTDIR)\apu2basinst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\apu2chnnl.cpp

"$(INTDIR)\apu2chnnl.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\apu2eqp.cpp

"$(INTDIR)\apu2eqp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\apu2inst.cpp

"$(INTDIR)\apu2inst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\libInit.cpp

"$(INTDIR)\libInit.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

