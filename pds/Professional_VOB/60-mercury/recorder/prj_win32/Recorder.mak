# Microsoft Developer Studio Generated NMAKE File, Based on Recorder.dsp
!IF "$(CFG)" == ""
CFG=Recorder - Win32 Debug
!MESSAGE No configuration specified. Defaulting to Recorder - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Recorder - Win32 Release" && "$(CFG)" != "Recorder - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Recorder.mak" CFG="Recorder - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Recorder - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Recorder - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Recorder - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\Recorder.exe"


CLEAN :
	-@erase "$(INTDIR)\ChnInst.obj"
	-@erase "$(INTDIR)\ConfAgent.obj"
	-@erase "$(INTDIR)\QuitRecDlg.obj"
	-@erase "$(INTDIR)\RecApp.obj"
	-@erase "$(INTDIR)\Recorder.obj"
	-@erase "$(INTDIR)\Recorder.res"
	-@erase "$(INTDIR)\RecorderDlg.obj"
	-@erase "$(INTDIR)\RecServInst.obj"
	-@erase "$(INTDIR)\RecServLog.obj"
	-@erase "$(INTDIR)\State.obj"
	-@erase "$(INTDIR)\StateBase.obj"
	-@erase "$(INTDIR)\StateMgr.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Recorder.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /Od /I "." /I "..\include" /I "..\wmsSDK\include" /I "..\..\common\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\nms1" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\..\b0-thirdparty\haikey\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "REC_CPPUNIT_TEST" /D "_RECSERV_" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\Recorder.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Recorder.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kdvlog.lib winmm.lib OspLib.lib kdvsys.lib fcnet.lib enfc.lib KdvMediaNet.lib KdvEncrypt.lib RPCtrl.lib asflib.lib voddblib.lib usbverify.lib /nologo /subsystem:windows /incremental:no /pdb:"$(OUTDIR)\Recorder.pdb" /machine:I386 /nodefaultlib:"nafxcwd.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"LIBC.lib" /nodefaultlib:"nafxcw.lib" /out:"$(OUTDIR)\Recorder.exe" /libpath:"..\..\..\10-Common\lib\release\win32" /libpath:"..\..\..\b0-thirdparty\haikey\lib\release\win32" 
LINK32_OBJS= \
	"$(INTDIR)\RecServInst.obj" \
	"$(INTDIR)\RecServLog.obj" \
	"$(INTDIR)\State.obj" \
	"$(INTDIR)\StateBase.obj" \
	"$(INTDIR)\StateMgr.obj" \
	"$(INTDIR)\ChnInst.obj" \
	"$(INTDIR)\ConfAgent.obj" \
	"$(INTDIR)\QuitRecDlg.obj" \
	"$(INTDIR)\RecApp.obj" \
	"$(INTDIR)\Recorder.obj" \
	"$(INTDIR)\RecorderDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Recorder.res"

"$(OUTDIR)\Recorder.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Recorder - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\Recorder.exe" "$(OUTDIR)\Recorder.bsc"


CLEAN :
	-@erase "$(INTDIR)\ChnInst.obj"
	-@erase "$(INTDIR)\ChnInst.sbr"
	-@erase "$(INTDIR)\ConfAgent.obj"
	-@erase "$(INTDIR)\ConfAgent.sbr"
	-@erase "$(INTDIR)\QuitRecDlg.obj"
	-@erase "$(INTDIR)\QuitRecDlg.sbr"
	-@erase "$(INTDIR)\RecApp.obj"
	-@erase "$(INTDIR)\RecApp.sbr"
	-@erase "$(INTDIR)\Recorder.obj"
	-@erase "$(INTDIR)\Recorder.res"
	-@erase "$(INTDIR)\Recorder.sbr"
	-@erase "$(INTDIR)\RecorderDlg.obj"
	-@erase "$(INTDIR)\RecorderDlg.sbr"
	-@erase "$(INTDIR)\RecServInst.obj"
	-@erase "$(INTDIR)\RecServInst.sbr"
	-@erase "$(INTDIR)\RecServLog.obj"
	-@erase "$(INTDIR)\RecServLog.sbr"
	-@erase "$(INTDIR)\State.obj"
	-@erase "$(INTDIR)\State.sbr"
	-@erase "$(INTDIR)\StateBase.obj"
	-@erase "$(INTDIR)\StateBase.sbr"
	-@erase "$(INTDIR)\StateMgr.obj"
	-@erase "$(INTDIR)\StateMgr.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Recorder.bsc"
	-@erase "$(OUTDIR)\Recorder.exe"
	-@erase "$(OUTDIR)\Recorder.ilk"
	-@erase "$(OUTDIR)\Recorder.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I "." /I "..\include" /I "..\wmsSDK\include" /I "..\..\common\include" /I "..\..\..\10-common\include\platform" /I "..\..\..\10-common\include\mcu" /I "..\..\..\10-common\include\nms1" /I "..\..\..\10-common\include\system1" /I "..\..\..\10-common\include\protocol" /I "..\..\..\b0-thirdparty\haikey\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /D "REC_CPPUNIT_TEST" /D "_RECSERV_" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\Recorder.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Recorder.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\RecServInst.sbr" \
	"$(INTDIR)\RecServLog.sbr" \
	"$(INTDIR)\State.sbr" \
	"$(INTDIR)\StateBase.sbr" \
	"$(INTDIR)\StateMgr.sbr" \
	"$(INTDIR)\ChnInst.sbr" \
	"$(INTDIR)\ConfAgent.sbr" \
	"$(INTDIR)\QuitRecDlg.sbr" \
	"$(INTDIR)\RecApp.sbr" \
	"$(INTDIR)\Recorder.sbr" \
	"$(INTDIR)\RecorderDlg.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\Recorder.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kdvlog.lib winmm.lib OspLib.lib kdvsys.lib fcnet.lib enfc.lib KdvMediaNet.lib KdvEncrypt.lib RPCtrl.lib asflib.lib voddblib.lib usbverify.lib /nologo /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)\Recorder.pdb" /debug /machine:I386 /nodefaultlib:"nafxcwd.lib" /nodefaultlib:"libcd.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"LIBC.lib" /nodefaultlib:"msvcrt.lib" /out:"$(OUTDIR)\Recorder.exe" /pdbtype:sept /libpath:"..\..\..\10-Common\lib\debug\win32" /libpath:"..\..\..\b0-thirdparty\haikey\lib\debug\win32" 
LINK32_OBJS= \
	"$(INTDIR)\RecServInst.obj" \
	"$(INTDIR)\RecServLog.obj" \
	"$(INTDIR)\State.obj" \
	"$(INTDIR)\StateBase.obj" \
	"$(INTDIR)\StateMgr.obj" \
	"$(INTDIR)\ChnInst.obj" \
	"$(INTDIR)\ConfAgent.obj" \
	"$(INTDIR)\QuitRecDlg.obj" \
	"$(INTDIR)\RecApp.obj" \
	"$(INTDIR)\Recorder.obj" \
	"$(INTDIR)\RecorderDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\Recorder.res"

"$(OUTDIR)\Recorder.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Recorder.dep")
!INCLUDE "Recorder.dep"
!ELSE 
!MESSAGE Warning: cannot find "Recorder.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Recorder - Win32 Release" || "$(CFG)" == "Recorder - Win32 Debug"
SOURCE=..\source\RecServInst.cpp

!IF  "$(CFG)" == "Recorder - Win32 Release"


"$(INTDIR)\RecServInst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Recorder - Win32 Debug"


"$(INTDIR)\RecServInst.obj"	"$(INTDIR)\RecServInst.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\RecServLog.cpp

!IF  "$(CFG)" == "Recorder - Win32 Release"


"$(INTDIR)\RecServLog.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Recorder - Win32 Debug"


"$(INTDIR)\RecServLog.obj"	"$(INTDIR)\RecServLog.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\State.cpp

!IF  "$(CFG)" == "Recorder - Win32 Release"


"$(INTDIR)\State.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Recorder - Win32 Debug"


"$(INTDIR)\State.obj"	"$(INTDIR)\State.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\StateBase.cpp

!IF  "$(CFG)" == "Recorder - Win32 Release"


"$(INTDIR)\StateBase.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Recorder - Win32 Debug"


"$(INTDIR)\StateBase.obj"	"$(INTDIR)\StateBase.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\StateMgr.cpp

!IF  "$(CFG)" == "Recorder - Win32 Release"


"$(INTDIR)\StateMgr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Recorder - Win32 Debug"


"$(INTDIR)\StateMgr.obj"	"$(INTDIR)\StateMgr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\ChnInst.cpp

!IF  "$(CFG)" == "Recorder - Win32 Release"


"$(INTDIR)\ChnInst.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Recorder - Win32 Debug"


"$(INTDIR)\ChnInst.obj"	"$(INTDIR)\ChnInst.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\ConfAgent.cpp

!IF  "$(CFG)" == "Recorder - Win32 Release"


"$(INTDIR)\ConfAgent.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Recorder - Win32 Debug"


"$(INTDIR)\ConfAgent.obj"	"$(INTDIR)\ConfAgent.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\QuitRecDlg.cpp

!IF  "$(CFG)" == "Recorder - Win32 Release"


"$(INTDIR)\QuitRecDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Recorder - Win32 Debug"


"$(INTDIR)\QuitRecDlg.obj"	"$(INTDIR)\QuitRecDlg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\RecApp.cpp

!IF  "$(CFG)" == "Recorder - Win32 Release"


"$(INTDIR)\RecApp.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Recorder - Win32 Debug"


"$(INTDIR)\RecApp.obj"	"$(INTDIR)\RecApp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\source\Recorder.cpp

!IF  "$(CFG)" == "Recorder - Win32 Release"


"$(INTDIR)\Recorder.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Recorder - Win32 Debug"


"$(INTDIR)\Recorder.obj"	"$(INTDIR)\Recorder.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\Recorder.rc

"$(INTDIR)\Recorder.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=..\source\RecorderDlg.cpp

!IF  "$(CFG)" == "Recorder - Win32 Release"


"$(INTDIR)\RecorderDlg.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Recorder - Win32 Debug"


"$(INTDIR)\RecorderDlg.obj"	"$(INTDIR)\RecorderDlg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Recorder - Win32 Release"


"$(INTDIR)\StdAfx.obj" : $(SOURCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "Recorder - Win32 Debug"


"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr" : $(SOURCE) "$(INTDIR)"


!ENDIF 


!ENDIF 

