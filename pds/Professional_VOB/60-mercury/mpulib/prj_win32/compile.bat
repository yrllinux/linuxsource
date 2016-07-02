del /F /Q debug\*.*
del ..\..\..\10-Common\lib\debug\win32\mpu.lib
@echo.
@echo      ==============================================
@echo      =      make win32 debug mpu lib      =
@echo      ==============================================
nmake /NOLOGO /A /B /f mpulib.mak CFG="MPULib - Win32 Debug"

del /F /Q release\*.*
del ..\..\..\10-Common\lib\release\win32\mpu.lib
@echo.
@echo      ==============================================
@echo      =      make win32 release mpu lib     =
@echo      ==============================================
nmake /NOLOGO /A /B /f mpulib.mak CFG="MPULib - Win32 Release"

