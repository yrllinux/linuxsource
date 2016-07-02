@echo.
@echo      ==============================================
@echo      =      make win32 debug common.lib           =
@echo      ==============================================
del /F /Q Debug\*.*
nmake /NOLOGO /A /B /f commonlib.mak CFG="commonlib - Win32 Debug" /x nmakeError.log

@echo.
@echo      ==============================================
@echo      =      make win32 release common.lib         =
@echo      ==============================================
del /F /Q Release\*.*
nmake /NOLOGO /A /B /f commonlib.mak CFG="commonlib - Win32 Release" /x nmakeError.log

cd ..

