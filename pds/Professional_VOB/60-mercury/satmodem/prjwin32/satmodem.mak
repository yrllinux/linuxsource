# Microsoft Developer Studio Generated NMAKE File, Based on satmodem.dsp
!IF "$(CFG)" == ""
CFG=satmodem - Win32 Debug
!MESSAGE No configuration specified. Defaulting to satmodem - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "satmodem - Win32 Release" && "$(CFG)" != "satmodem - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "satmodem.mak" CFG="satmodem - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "satmodem - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "satmodem - Win32 Debug" (based on "Win32 (x86) Static Library")
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

!IF  "$(CFG)" == "satmodem - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\satmodem.lib"


CLEAN :
	-@erase "$(INTDIR)\cmdprotocol.obj"
	-@erase "$(INTDIR)\cmdtable.obj"
	-@erase "$(INTDIR)\modemcmdproxy.obj"
	-@erase "$(INTDIR)\modemservice.obj"
	-@erase "$(INTDIR)\msgpipe.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\satmodem.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\satmodem.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\satmodem.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\satmodem.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cmdprotocol.obj" \
	"$(INTDIR)\cmdtable.obj" \
	"$(INTDIR)\modemcmdproxy.obj" \
	"$(INTDIR)\modemservice.obj" \
	"$(INTDIR)\msgpipe.obj"

"$(OUTDIR)\satmodem.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "satmodem - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\10-common\lib\debug\win32\satmodem.lib"


CLEAN :
	-@erase "$(INTDIR)\cmdprotocol.obj"
	-@erase "$(INTDIR)\cmdtable.obj"
	-@erase "$(INTDIR)\modemcmdproxy.obj"
	-@erase "$(INTDIR)\modemservice.obj"
	-@erase "$(INTDIR)\msgpipe.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\10-common\lib\debug\win32\satmodem.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\satmodem.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\satmodem.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\debug\win32\satmodem.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cmdprotocol.obj" \
	"$(INTDIR)\cmdtable.obj" \
	"$(INTDIR)\modemcmdproxy.obj" \
	"$(INTDIR)\modemservice.obj" \
	"$(INTDIR)\msgpipe.obj"

"..\..\..\10-common\lib\debug\win32\satmodem.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
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
!IF EXISTS("satmodem.dep")
!INCLUDE "satmodem.dep"
!ELSE 
!MESSAGE Warning: cannot find "satmodem.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "satmodem - Win32 Release" || "$(CFG)" == "satmodem - Win32 Debug"
SOURCE=..\source\cmdprotocol.cpp

"$(INTDIR)\cmdprotocol.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\cmdtable.cpp

"$(INTDIR)\cmdtable.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\modemcmdproxy.cpp

"$(INTDIR)\modemcmdproxy.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\modemservice.cpp

"$(INTDIR)\modemservice.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\msgpipe.cpp

"$(INTDIR)\msgpipe.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

