@echo.
@echo      ==============================================
@echo      =      make win32 debug VMP lib      =
@echo      ==============================================
nmake /NOLOGO /A /B /f vmplib.mak CFG="vmplib - Win32 Debug"


@echo.
@echo      ==============================================
@echo      =      make win32 debug VMP lib      =
@echo      ==============================================
nmake /NOLOGO /A /B /f vmplib.mak CFG="vmplib - Win32 Release"

cd ..



