# Microsoft Developer Studio Generated NMAKE File, Based on kdvcascade.dsp
!IF "$(CFG)" == ""
CFG=MMcuLib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to MMcuLib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "MMcuLib - Win32 Release" && "$(CFG)" != "MMcuLib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "kdvcascade.mak" CFG="MMcuLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MMcuLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "MMcuLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "MMcuLib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\..\..\10-common\lib\release\win32\kdvcascade.lib"


CLEAN :
	-@erase "$(INTDIR)\h323mcuconn.obj"
	-@erase "$(INTDIR)\tcpsocket.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\xmlcodec.obj"
	-@erase "..\..\..\10-common\lib\release\win32\kdvcascade.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\protocol\h323stack" /I "..\..\..\10-common\include\mcu" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\kdvcascade.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\release\win32\kdvcascade.lib" 
LIB32_OBJS= \
	"$(INTDIR)\h323mcuconn.obj" \
	"$(INTDIR)\tcpsocket.obj" \
	"$(INTDIR)\xmlcodec.obj"

"..\..\..\10-common\lib\release\win32\kdvcascade.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "MMcuLib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\10-common\lib\debug\win32\kdvcascade.lib" "$(OUTDIR)\kdvcascade.bsc"


CLEAN :
	-@erase "$(INTDIR)\h323mcuconn.obj"
	-@erase "$(INTDIR)\h323mcuconn.sbr"
	-@erase "$(INTDIR)\tcpsocket.obj"
	-@erase "$(INTDIR)\tcpsocket.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\xmlcodec.obj"
	-@erase "$(INTDIR)\xmlcodec.sbr"
	-@erase "$(OUTDIR)\kdvcascade.bsc"
	-@erase "..\..\..\10-common\lib\debug\win32\kdvcascade.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\..\10-common\include\protocol\h323stack" /I "..\..\..\10-common\include\mcu" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\kdvcascade.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\h323mcuconn.sbr" \
	"$(INTDIR)\tcpsocket.sbr" \
	"$(INTDIR)\xmlcodec.sbr"

"$(OUTDIR)\kdvcascade.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\10-common\lib\debug\win32\kdvcascade.lib" 
LIB32_OBJS= \
	"$(INTDIR)\h323mcuconn.obj" \
	"$(INTDIR)\tcpsocket.obj" \
	"$(INTDIR)\xmlcodec.obj"

"..\..\..\10-common\lib\debug\win32\kdvcascade.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("kdvcascade.dep")
!INCLUDE "kdvcascade.dep"
!ELSE 
!MESSAGE Warning: cannot find "kdvcascade.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "MMcuLib - Win32 Release" || "$(CFG)" == "MMcuLib - Win32 Debug"
SOURCE=..\source\h323mcuconn.cpp

!IF  "$(CFG)" == "MMcuLib - Win32 Release"


"$(INTDIR)\h323mcuconn.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MMcuLib - Win32 Debug"


"$(INTDIR)\h323mcuconn.obj"	"$(INTDIR)\h323mcuconn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\tcpsocket.cpp

!IF  "$(CFG)" == "MMcuLib - Win32 Release"


"$(INTDIR)\tcpsocket.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MMcuLib - Win32 Debug"


"$(INTDIR)\tcpsocket.obj"	"$(INTDIR)\tcpsocket.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\xmlcodec.cpp

!IF  "$(CFG)" == "MMcuLib - Win32 Release"


"$(INTDIR)\xmlcodec.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "MMcuLib - Win32 Debug"


"$(INTDIR)\xmlcodec.obj"	"$(INTDIR)\xmlcodec.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

