
@echo.
@echo      ==============================================
@echo      =      make win32 debug mp.lib               =
@echo      ==============================================

del /F /Q Debug\*.*
Rem del /F /Q ..\..\..\10-common\lib\debug\win32\mp.lib
nmake /NOLOGO /A /B /f prjmp.mak CFG="prjMp - Win32 Debug" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\mp_debug.txt

@echo.
@echo      ==============================================
@echo      =      make win32 release mp.lib             =
@echo      ==============================================

del /F /Q Release\*.*
Rem del /F /Q ..\..\..\10-common\lib\release\win32\mp.lib
nmake /NOLOGO /A /B /f prjmp.mak CFG="prjMp - Win32 Release" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\mp_release.txt

cd ..\

