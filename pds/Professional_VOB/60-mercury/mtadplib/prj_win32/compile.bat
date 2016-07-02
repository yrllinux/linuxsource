

@echo ***********************  make win32 debug mtadp.lib **************************

@rem If /I  "%1" == "" Or "%1" == "clean" 
nmake /NOLOGO /A /B /f prjmtadpwin32.mak CFG="prjMtadpWin32 - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f prjmtadpwin32.mak CFG="prjMtadpWin32 - Win32 Debug" /x nmakeError.log


@echo ***********************  make win32 release mtadp.lib **************************

@rem If /I  "%1" == "" Or "%1" == "clean" 
nmake /NOLOGO /A /B /f prjmtadpwin32.mak CFG="prjMtadpWin32 - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f prjmtadpwin32.mak CFG="prjMtadpWin32 - Win32 Release" /x nmakeError.log

cd ..