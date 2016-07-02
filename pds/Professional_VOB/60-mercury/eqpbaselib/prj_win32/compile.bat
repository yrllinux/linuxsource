del /F /Q debug\*.*
del ..\..\..\10-Common\lib\debug\win32\eqpbaselib.lib
@echo.
@echo      ==============================================
@echo      =      make win32 debug  eqpbaselib          =
@echo      ==============================================
nmake /NOLOGO /A /B /f eqpbaselib.mak CFG="eqpbaselib - Win32 Debug"

del /F /Q release\*.*
del ..\..\..\10-Common\lib\release\win32\eqpbaselib.lib
@echo.
@echo      ==============================================
@echo      =      make win32 release eqpbaselib         =
@echo      ==============================================
nmake /NOLOGO /A /B /f eqpbaselib.mak CFG="eqpbaselib - Win32 Release"