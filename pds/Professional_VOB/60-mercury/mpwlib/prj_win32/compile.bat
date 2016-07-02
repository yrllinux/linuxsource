
@echo.
@echo      ==============================================
@echo      =      make win32 debug mpw.lib               =
@echo      ==============================================

del /F /Q Debug\*.*
Rem del /F /Q ..\..\..\10-common\lib\debug\win32\mpwlib.lib
nmake /NOLOGO /A /B /f MpwLib.mak CFG="MpwLib - Win32 Debug" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\mpw_debug.txt

@echo.
@echo      ==============================================
@echo      =      make win32 release mpw.lib             =
@echo      ==============================================

del /F /Q Release\*.*
Rem del /F /Q ..\..\..\10-common\lib\release\win32\mpwlib.lib
nmake /NOLOGO /A /B /f MpwLib.mak CFG="MpwLib - Win32 Release" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\mpw_release.txt

cd ..\

