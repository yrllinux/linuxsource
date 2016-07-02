

@echo ***********************  make win32 debug mtadp6.lib **************************

@rem If /I  "%1" == "" Or "%1" == "clean" 
nmake /NOLOGO /A /B /f prjmtadpwin32.mak CFG="prjMtadpWin32 - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f prjmtadpwin32.mak CFG="prjMtadpWin32 - Win32 Debug" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\mtadp6_debug.txt



@echo ***********************  make win32 release mtadp6.lib **************************

@rem If /I  "%1" == "" Or "%1" == "clean" 
nmake /NOLOGO /A /B /f prjmtadpwin32.mak CFG="prjMtadpWin32 - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f prjmtadpwin32.mak CFG="prjMtadpWin32 - Win32 Release" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\mtadp6_release.txt


cd ..