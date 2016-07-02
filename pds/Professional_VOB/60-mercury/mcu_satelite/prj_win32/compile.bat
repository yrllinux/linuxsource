
@echo.
@echo      ==============================================
@echo      =      make win32 debug mcu.exe              =
@echo      ==============================================

del /F /Q Debug\*.*
del ..\..\..\10-common\version\debug\win32\mcu.exe
del mcu.exe
nmake /NOLOGO /A /B /f prjmcu.mak CFG="prjMcu - Win32 Debug" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\mcuWin_debug.txt
copy /Y mcu.exe ..\..\..\10-common\version\debug\win32

@echo.
@echo      ==============================================
@echo      =      make win32 release mcu.exe            =
@echo      ==============================================

del /F /Q Release\*.*
del ..\..\..\10-common\version\release\win32\mcu.exe
del mcu.exe
nmake /NOLOGO /A /B /f prjmcu.mak CFG="prjMcu - Win32 Release" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\mcuWin_release.txt
copy /Y mcu.exe ..\..\..\10-common\version\release\win32

cd ..\
