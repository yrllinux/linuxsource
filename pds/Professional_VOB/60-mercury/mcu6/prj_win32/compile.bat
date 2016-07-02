
@echo.
@echo      ==============================================
@echo      =      make win32 debug mcu6.exe              =
@echo      ==============================================

del /F /Q Debug\*.*
del ..\..\..\10-common\version\debug\win32\mcu6.exe
del mcu6.exe
nmake /NOLOGO /A /B /f prjmcu.mak CFG="prjMcu - Win32 Debug" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\mcu6Win_debug.txt
copy /Y mcu6.exe ..\..\..\10-common\version\debug\win32

@echo.
@echo      ==============================================
@echo      =      make win32 release mcu6.exe            =
@echo      ==============================================

del /F /Q Release\*.*
del ..\..\..\10-common\version\release\win32\mcu6.exe
del mcu6.exe
nmake /NOLOGO /A /B /f prjmcu.mak CFG="prjMcu - Win32 Release" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\mcu6Win_release.txt
copy /Y mcu6.exe ..\..\..\10-common\version\release\win32

cd ..\
