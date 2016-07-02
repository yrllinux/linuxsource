@echo.
@echo      ==============================================
@echo      =      make win32 debug draw lib      =
@echo      ==============================================
nmake /NOLOGO /A /B /f drawbmplib.mak CFG="drawbmplib - Win32 Debug"

@echo.
@echo      ==============================================
@echo      =      make win32 release draw lib      =
@echo      ==============================================
nmake /NOLOGO /A /B /f drawbmplib.mak CFG="drawbmplib - Win32 Release"



