del /F /Q debug\*.*
del ..\..\..\10-Common\lib\debug\win32\bas.lib
@echo.
@echo      ==============================================
@echo      =      make win32 debug BAS lib      =
@echo      ==============================================
nmake /NOLOGO /A /B /f baslib.mak CFG="BASLib - Win32 Debug"

del /F /Q release\*.*
del ..\..\..\10-Common\lib\release\win32\bas.lib
@echo.
@echo      ==============================================
@echo      =      make win32 release BAS lib     =
@echo      ==============================================
nmake /NOLOGO /A /B /f baslib.mak CFG="BASLib - Win32 Release"

