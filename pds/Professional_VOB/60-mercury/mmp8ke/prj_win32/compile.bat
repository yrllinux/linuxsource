
@echo.
@echo      ==============================================
@echo      =      make win32 debug winmmp.exe              =
@echo      ==============================================

del /F /Q Debug\*.*
del ..\..\..\10-common\version\debug\win32_8000e\winmmp.exe
del winmmp.exe
nmake /NOLOGO /A /B /f winmmp.mak CFG="winmmp - Win32 Debug" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\winmmp_debug.txt
copy /Y winmmp.exe ..\..\..\10-common\version\debug\win32_8000e

@echo.
@echo      ==============================================
@echo      =      make win32 release winmmp.exe         =
@echo      ==============================================

del /F /Q Release\*.*
del ..\..\..\10-common\version\release\win32_8000e\winmmp.exe
del winmmp.exe
nmake /NOLOGO /A /B /f winmmp.mak CFG="winmmp - Win32 Release" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\winmmp_release.txt
copy /Y winmmp.exe ..\..\..\10-common\version\release\win32_8000e

cd ..\
