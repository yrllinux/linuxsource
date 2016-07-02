
@echo.
@echo      ==============================================
@echo      =      make win32 debug apu2.exe             =
@echo      ==============================================
del /F /Q Debug\*.*
del ..\..\..\10-common\version\debug\win32\apu2.exe
del Debug\apu2.exe
nmake /NOLOGO /A /B /f apu2.mak CFG="apu2 - Win32 Debug" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\apu2Win_debug.txt
copy /Y Debug\apu2.exe ..\..\..\10-common\version\debug\win32


@echo
@echo      ==============================================
@echo      =      make win32 release apu2.exe           =
@echo      ==============================================
del /F /Q Release\*.*
del ..\..\..\10-common\version\release\win32\apu2.exe
del Release\apu2.exe
nmake /NOLOGO /A /B /f apu2.mak CFG="apu2 - Win32 Release" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\apu2Win_release.txt
copy /Y Release\apu2.exe ..\..\..\10-common\version\release\win32