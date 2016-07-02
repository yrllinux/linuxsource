# Microsoft Developer Studio Generated NMAKE File, Based on eqpagtcommbase.dsp
!IF "$(CFG)" == ""
CFG=eqpagtcommbase - Win32 Debug
!MESSAGE No configuration specified. Defaulting to eqpagtcommbase - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "eqpagtcommbase - Win32 Release" && "$(CFG)" != "eqpagtcommbase - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "eqpagtcommbase.mak" CFG="eqpagtcommbase - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "eqpagtcommbase - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "eqpagtcommbase - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "eqpagtcommbase - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\..\10-common\lib\release\win32\eqpagtcommbase.lib"


CLEAN :
	-@erase "$(INTDIR)\eqpagtcfg.obj"
	-@erase "$(INTDIR)\eqpagtcommbase.obj"
	-@erase "$(INTDIR)\eqpagtipinfo.obj"
	-@erase "$(INTDIR)\eqpagtscan.obj"
	-@erase "$(INTDIR)\eqpagtsnmp.obj"
	-@erase "$(INTDIR)\eqpagtutility.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\..\..\..\10-common\lib\release\win32\eqpagtcommbase.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include\\" /I "..\..\..\..\60-mercury\common\include\platform_sdk\include" /I "..\..\..\..\60-mercury\common\include\\" /I "..\..\..\..\10-common\include\system1\\" /I "..\..\..\..\10-common\include\snmp\\" /I "..\..\..\..\10-common\include\platform\\" /I "..\..\..\..\10-common\include\protocol\\" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\eqpagtcommbase.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\eqpagtcommbase.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\..\10-common\lib\release\win32\eqpagtcommbase.lib" 
LIB32_OBJS= \
	"$(INTDIR)\eqpagtcfg.obj" \
	"$(INTDIR)\eqpagtcommbase.obj" \
	"$(INTDIR)\eqpagtipinfo.obj" \
	"$(INTDIR)\eqpagtscan.obj" \
	"$(INTDIR)\eqpagtsnmp.obj" \
	"$(INTDIR)\eqpagtutility.obj"

"..\..\..\..\10-common\lib\release\win32\eqpagtcommbase.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "eqpagtcommbase - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\..\10-common\lib\debug\win32\eqpagtcommbase.lib"


CLEAN :
	-@erase "$(INTDIR)\eqpagtcfg.obj"
	-@erase "$(INTDIR)\eqpagtcommbase.obj"
	-@erase "$(INTDIR)\eqpagtipinfo.obj"
	-@erase "$(INTDIR)\eqpagtscan.obj"
	-@erase "$(INTDIR)\eqpagtsnmp.obj"
	-@erase "$(INTDIR)\eqpagtutility.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\..\10-common\lib\debug\win32\eqpagtcommbase.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\include\\" /I "..\..\..\..\60-mercury\common\include\platform_sdk\include" /I "..\..\..\..\60-mercury\common\include\\" /I "..\..\..\..\10-common\include\system1\\" /I "..\..\..\..\10-common\include\snmp\\" /I "..\..\..\..\10-common\include\platform\\" /I "..\..\..\..\10-common\include\protocol\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\eqpagtcommbase.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\eqpagtcommbase.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\..\10-common\lib\debug\win32\eqpagtcommbase.lib" 
LIB32_OBJS= \
	"$(INTDIR)\eqpagtcfg.obj" \
	"$(INTDIR)\eqpagtcommbase.obj" \
	"$(INTDIR)\eqpagtipinfo.obj" \
	"$(INTDIR)\eqpagtscan.obj" \
	"$(INTDIR)\eqpagtsnmp.obj" \
	"$(INTDIR)\eqpagtutility.obj"

"..\..\..\..\10-common\lib\debug\win32\eqpagtcommbase.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("eqpagtcommbase.dep")
!INCLUDE "eqpagtcommbase.dep"
!ELSE 
!MESSAGE Warning: cannot find "eqpagtcommbase.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "eqpagtcommbase - Win32 Release" || "$(CFG)" == "eqpagtcommbase - Win32 Debug"
SOURCE=..\source\eqpagtcfg.cpp

"$(INTDIR)\eqpagtcfg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\eqpagtcommbase.cpp

"$(INTDIR)\eqpagtcommbase.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\eqpagtipinfo.cpp

"$(INTDIR)\eqpagtipinfo.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\eqpagtscan.cpp

"$(INTDIR)\eqpagtscan.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\eqpagtsnmp.cpp

"$(INTDIR)\eqpagtsnmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\eqpagtutility.cpp

"$(INTDIR)\eqpagtutility.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

