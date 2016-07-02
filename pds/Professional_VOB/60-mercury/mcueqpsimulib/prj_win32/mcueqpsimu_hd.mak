# Microsoft Developer Studio Generated NMAKE File, Based on mcueqpsimu_hd.dsp
!IF "$(CFG)" == ""
CFG=mcueqpsimu - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mcueqpsimu - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mcueqpsimu - Win32 Release" && "$(CFG)" != "mcueqpsimu - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mcueqpsimu_hd.mak" CFG="mcueqpsimu - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mcueqpsimu - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mcueqpsimu - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "mcueqpsimu - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\10-common\lib\release\win32\mcueqpsimu.lib"


CLEAN :
	-@erase "$(INTDIR)\bas.obj"
	-@erase "$(INTDIR)\hdu.obj"
	-@erase "$(INTDIR)\hdu2.obj"
	-@erase "$(INTDIR)\kdvmixer_apu2.obj"
	-@erase "$(INTDIR)\kdvmixer_eapu.obj"
	-@erase "$(INTDIR)\mcueqpsimu.obj"
	-@erase "$(INTDIR)\mixer.obj"
	-@erase "$(INTDIR)\mpu.obj"
	-@erase "$(INTDIR)\mpu2.obj"
	-@erase "$(INTDIR)\mpu2bas.obj"
	-@erase "$(INTDIR)\mpu2vmp.obj"
	-@erase "$(INTDIR)\prs.obj"
	-@erase "$(INTDIR)\tvwall.obj"
	-@erase "$(INTDIR)\unreslove.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vmp.obj"
	-@erase "..\..\..\10-common\lib\release\win32\mcueqpsimu.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\..\common\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /D "HARD_CODEC" /D "_AFXDLL" /D "_HD_" /Fp"$(INTDIR)\mcueqpsimu_hd.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mcueqpsimu_hd.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\release\win32\mcueqpsimu.lib" 
LIB32_OBJS= \
	"$(INTDIR)\bas.obj" \
	"$(INTDIR)\hdu.obj" \
	"$(INTDIR)\hdu2.obj" \
	"$(INTDIR)\kdvmixer_eapu.obj" \
	"$(INTDIR)\mcueqpsimu.obj" \
	"$(INTDIR)\mixer.obj" \
	"$(INTDIR)\mpu.obj" \
	"$(INTDIR)\mpu2.obj" \
	"$(INTDIR)\mpu2bas.obj" \
	"$(INTDIR)\mpu2vmp.obj" \
	"$(INTDIR)\prs.obj" \
	"$(INTDIR)\tvwall.obj" \
	"$(INTDIR)\unreslove.obj" \
	"$(INTDIR)\vmp.obj" \
	"$(INTDIR)\kdvmixer_apu2.obj"

"..\..\..\10-common\lib\release\win32\mcueqpsimu.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mcueqpsimu - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\..\..\10-common\lib\Debug\win32\mcueqpsimu.lib"


CLEAN :
	-@erase "$(INTDIR)\bas.obj"
	-@erase "$(INTDIR)\hdu.obj"
	-@erase "$(INTDIR)\hdu2.obj"
	-@erase "$(INTDIR)\kdvmixer_apu2.obj"
	-@erase "$(INTDIR)\kdvmixer_eapu.obj"
	-@erase "$(INTDIR)\mcueqpsimu.obj"
	-@erase "$(INTDIR)\mixer.obj"
	-@erase "$(INTDIR)\mpu.obj"
	-@erase "$(INTDIR)\mpu2.obj"
	-@erase "$(INTDIR)\mpu2bas.obj"
	-@erase "$(INTDIR)\mpu2vmp.obj"
	-@erase "$(INTDIR)\prs.obj"
	-@erase "$(INTDIR)\tvwall.obj"
	-@erase "$(INTDIR)\unreslove.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vmp.obj"
	-@erase "..\..\..\10-common\lib\Debug\win32\mcueqpsimu.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\common\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HARD_CODEC" /D "_AFXDLL" /D "_HD_" /Fp"$(INTDIR)\mcueqpsimu_hd.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mcueqpsimu_hd.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\Debug\win32\mcueqpsimu.lib" 
LIB32_OBJS= \
	"$(INTDIR)\bas.obj" \
	"$(INTDIR)\hdu.obj" \
	"$(INTDIR)\hdu2.obj" \
	"$(INTDIR)\kdvmixer_eapu.obj" \
	"$(INTDIR)\mcueqpsimu.obj" \
	"$(INTDIR)\mixer.obj" \
	"$(INTDIR)\mpu.obj" \
	"$(INTDIR)\mpu2.obj" \
	"$(INTDIR)\mpu2bas.obj" \
	"$(INTDIR)\mpu2vmp.obj" \
	"$(INTDIR)\prs.obj" \
	"$(INTDIR)\tvwall.obj" \
	"$(INTDIR)\unreslove.obj" \
	"$(INTDIR)\vmp.obj" \
	"$(INTDIR)\kdvmixer_apu2.obj"

"..\..\..\10-common\lib\Debug\win32\mcueqpsimu.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("mcueqpsimu_hd.dep")
!INCLUDE "mcueqpsimu_hd.dep"
!ELSE 
!MESSAGE Warning: cannot find "mcueqpsimu_hd.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mcueqpsimu - Win32 Release" || "$(CFG)" == "mcueqpsimu - Win32 Debug"
SOURCE=..\source\bas.cpp

"$(INTDIR)\bas.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\hdu.cpp

"$(INTDIR)\hdu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\hdu2.cpp

"$(INTDIR)\hdu2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\kdvmixer_apu2.cpp

"$(INTDIR)\kdvmixer_apu2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\kdvmixer_eapu.cpp

"$(INTDIR)\kdvmixer_eapu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mcueqpsimu.cpp

"$(INTDIR)\mcueqpsimu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mixer.cpp

"$(INTDIR)\mixer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpu.cpp

"$(INTDIR)\mpu.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpu2.cpp

"$(INTDIR)\mpu2.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpu2bas.cpp

"$(INTDIR)\mpu2bas.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\mpu2vmp.cpp

"$(INTDIR)\mpu2vmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\prs.cpp

"$(INTDIR)\prs.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\tvwall.cpp

"$(INTDIR)\tvwall.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\unreslove.cpp

"$(INTDIR)\unreslove.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\vmp.cpp

"$(INTDIR)\vmp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

