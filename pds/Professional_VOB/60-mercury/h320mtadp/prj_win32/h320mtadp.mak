# Microsoft Developer Studio Generated NMAKE File, Based on h320mtadp.dsp
!IF "$(CFG)" == ""
CFG=h320mtadp - Win32 Debug
!MESSAGE No configuration specified. Defaulting to h320mtadp - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "h320mtadp - Win32 Release" && "$(CFG)" != "h320mtadp - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "h320mtadp.mak" CFG="h320mtadp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "h320mtadp - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "h320mtadp - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "h320mtadp - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\h320mtadp.exe"


CLEAN :
	-@erase "$(INTDIR)\MtAdp.obj"
	-@erase "$(INTDIR)\MtAdpCom.obj"
	-@erase "$(INTDIR)\MtAdpd.obj"
	-@erase "$(INTDIR)\MtAdpInst.obj"
	-@erase "$(INTDIR)\MtAdpNet.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\h320mtadp.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\video1" /I "..\..\..\10-common\include\algorithm" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\h320mtadp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\h320mtadp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kdvsys.lib osplib.lib kdvnetbuf.lib mcuprs.lib winbrdwrapper.lib basicboardagent.lib driagent.lib kdvh320.lib mccsim.lib KdvEncrypt.lib fcnet.lib enfc.lib kdvmedianet.lib h261addrtpheader.lib h263decformrtp.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\h320mtadp.pdb" /machine:I386 /nodefaultlib:"nafxcw.lib" /nodefaultlib:"libc.lib" /out:"$(OUTDIR)\h320mtadp.exe" /libpath:"..\..\..\10-common\lib\release\win32" 
LINK32_OBJS= \
	"$(INTDIR)\MtAdp.obj" \
	"$(INTDIR)\MtAdpCom.obj" \
	"$(INTDIR)\MtAdpd.obj" \
	"$(INTDIR)\MtAdpInst.obj" \
	"$(INTDIR)\MtAdpNet.obj"

"$(OUTDIR)\h320mtadp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "h320mtadp - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\h320mtadp.exe"


CLEAN :
	-@erase "$(INTDIR)\MtAdp.obj"
	-@erase "$(INTDIR)\MtAdpCom.obj"
	-@erase "$(INTDIR)\MtAdpd.obj"
	-@erase "$(INTDIR)\MtAdpInst.obj"
	-@erase "$(INTDIR)\MtAdpNet.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\h320mtadp.exe"
	-@erase "$(OUTDIR)\h320mtadp.ilk"
	-@erase "$(OUTDIR)\h320mtadp.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MLd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\common\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\video1" /I "..\..\..\10-common\include\algorithm" /I "..\..\..\10-common\include\protocol" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\h320mtadp.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\h320mtadp.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kdvsys.lib osplib.lib kdvnetbuf.lib mcuprs.lib winbrdwrapper.lib basicboardagent.lib driagent.lib kdvh320.lib mccsim.lib KdvEncrypt.lib fcnet.lib enfc.lib kdvmedianet.lib h261addrtpheader.lib h263decformrtp.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\h320mtadp.pdb" /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /nodefaultlib:"libcd.lib" /out:"$(OUTDIR)\h320mtadp.exe" /pdbtype:sept /libpath:"..\..\..\10-common\lib\debug\win32" 
LINK32_OBJS= \
	"$(INTDIR)\MtAdp.obj" \
	"$(INTDIR)\MtAdpCom.obj" \
	"$(INTDIR)\MtAdpd.obj" \
	"$(INTDIR)\MtAdpInst.obj" \
	"$(INTDIR)\MtAdpNet.obj"

"$(OUTDIR)\h320mtadp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("h320mtadp.dep")
!INCLUDE "h320mtadp.dep"
!ELSE 
!MESSAGE Warning: cannot find "h320mtadp.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "h320mtadp - Win32 Release" || "$(CFG)" == "h320mtadp - Win32 Debug"
SOURCE=..\source\MtAdp.cpp

"$(INTDIR)\MtAdp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\MtAdpCom.cpp

"$(INTDIR)\MtAdpCom.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\MtAdpd.cpp

"$(INTDIR)\MtAdpd.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\MtAdpInst.cpp

"$(INTDIR)\MtAdpInst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\source\MtAdpNet.cpp

"$(INTDIR)\MtAdpNet.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

