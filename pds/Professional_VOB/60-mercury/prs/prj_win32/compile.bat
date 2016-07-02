
@echo.
@echo      ==============================================
@echo      =      make win32 debug prs8ke.exe              =
@echo      ==============================================

del /F /Q Debug\*.*
del ..\..\..\10-common\version\debug\win32_8000e\prs8ke.exe
del prs8ke.exe
nmake /NOLOGO /A /B /f prs8ke.mak CFG="prs8ke - Win32 Debug" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\winprs8ke_debug.txt
copy /Y prs8ke.exe ..\..\..\10-common\version\debug\winprs_8000e

@echo.
@echo      ==============================================
@echo      =      make win32 release prs8ke.exe         =
@echo      ==============================================

del /F /Q Release\*.*
del ..\..\..\10-common\version\release\win32_8000e\prs8ke.exe
del prs8ke.exe
nmake /NOLOGO /A /B /f prs8ke.mak CFG="prs8ke - Win32 Release" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\winprs8ke_release.txt
copy /Y prs8ke.exe ..\..\..\10-common\version\release\winprs_8000e

cd ..\
