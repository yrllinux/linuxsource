
@echo.
@echo      ==============================================
@echo      =      make win32 debug mp6.lib               =
@echo      ==============================================

del /F /Q Debug\*.*
Rem del /F /Q ..\..\..\10-common\lib\debug\win32\mp6.lib
nmake /NOLOGO /A /B /f mp6lib.mak CFG="mp6lib - Win32 Debug" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\mp6_debug.txt

@echo.
@echo      ==============================================
@echo      =      make win32 release mp6.lib             =
@echo      ==============================================

del /F /Q Release\*.*
Rem del /F /Q ..\..\..\10-common\lib\release\win32\mp6.lib
nmake /NOLOGO /A /B /f mp6lib.mak CFG="mp6lib - Win32 Release" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\mp6_release.txt

cd ..\

