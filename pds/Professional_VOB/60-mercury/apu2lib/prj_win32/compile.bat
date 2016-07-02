del /F /Q debug\*.*
del ..\..\..\10-Common\lib\debug\win32\apu2lib.lib
@echo.
@echo      ==============================================
@echo      =      make win32 debug  apu2lib             =
@echo      ==============================================
nmake /NOLOGO /A /B /f apu2lib.mak CFG="apu2lib - Win32 Debug"

del /F /Q release\*.*
del ..\..\..\10-Common\lib\release\win32\apu2lib.lib
@echo.
@echo      ==============================================
@echo      =      make win32 release apu2lib            =
@echo      ==============================================
nmake /NOLOGO /A /B /f apu2lib.mak CFG="apu2lib - Win32 Release"