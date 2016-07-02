# Microsoft Developer Studio Generated NMAKE File, Based on mpulib.dsp
!IF "$(CFG)" == ""
CFG=mpulib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mpulib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mpulib - Win32 Release" && "$(CFG)" != "mpulib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mpulib.mak" CFG="mpulib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mpulib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mpulib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "mpulib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\mpulib.lib"


CLEAN :
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\mpubas.obj"
	-@erase "$(INTDIR)\mpudvmp.obj"
	-@erase "$(INTDIR)\mpuebap.obj"
	-@erase "$(INTDIR)\mpuevpu.obj"
	-@erase "$(INTDIR)\mpustruct.obj"
	-@erase "$(INTDIR)\mpusvmp.obj"
	-@erase "$(INTDIR)\mpuutility.obj"
	-@erase "$(INTDIR)\mpuvmphwtest.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\mpulib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\mpulib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mpulib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\mpulib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\mpubas.obj" \
	"$(INTDIR)\mpudvmp.obj" \
	"$(INTDIR)\mpuebap.obj" \
	"$(INTDIR)\mpuevpu.obj" \
	"$(INTDIR)\mpustruct.obj" \
	"$(INTDIR)\mpusvmp.obj" \
	"$(INTDIR)\mpuutility.obj" \
	"$(INTDIR)\mpuvmphwtest.obj"

"$(OUTDIR)\mpulib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mpulib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\10-common\lib\Debug\win32\mpulib.lib"


CLEAN :
	-@erase "$(INTDIR)\main.obj"
	-@erase "$(INTDIR)\mpubas.obj"
	-@erase "$(INTDIR)\mpudvmp.obj"
	-@erase "$(INTDIR)\mpuebap.obj"
	-@erase "$(INTDIR)\mpuevpu.obj"
	-@erase "$(INTDIR)\mpustruct.obj"
	-@erase "$(INTDIR)\mpusvmp.obj"
	-@erase "$(INTDIR)\mpuutility.obj"
	-@erase "$(INTDIR)\mpuvmphwtest.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "..\..\..\10-common\lib\Debug\win32\mpulib.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-Common\include\platform" /I "..\..\..\10-Common\include\mcu" /I "..\..\..\10-Common\include\system1" /I "..\..\..\10-Common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\mpulib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mpulib.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\Debug\win32\mpulib.lib" 
LIB32_OBJS= \
	"$(INTDIR)\main.obj" \
	"$(INTDIR)\mpubas.obj" \
	"$(INTDIR)\mpudvmp.obj" \
	"$(INTDIR)\mpuebap.obj" \
	"$(INTDIR)\mpuevpu.obj" \
	"$(INTDIR)\mpustruct.obj" \
	"$(INTDIR)\mpusvmp.obj" \
	"$(INTDIR)\mpuutility.obj" \
	"$(INTDIR)\mpuvmphwtest.obj"

"..\..\..\10-common\lib\Debug\win32\mpulib.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("mpulib.dep")
!INCLUDE "mpulib.dep"
!ELSE 
!MESSAGE Warning: cannot find "mpulib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mpulib - Win32 Release" || "$(CFG)" == "mpulib - Win32 Debug"
SOURCE=..\source\main.cpp

"$(INTDIR)\main.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpubas.cpp

"$(INTDIR)\mpubas.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpudvmp.cpp

"$(INTDIR)\mpudvmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpuebap.cpp

"$(INTDIR)\mpuebap.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpuevpu.cpp

"$(INTDIR)\mpuevpu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpustruct.cpp

"$(INTDIR)\mpustruct.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpusvmp.cpp

"$(INTDIR)\mpusvmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpuutility.cpp

"$(INTDIR)\mpuutility.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpuvmphwtest.cpp

"$(INTDIR)\mpuvmphwtest.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

