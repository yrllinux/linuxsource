del /F /Q debug\*.*
del ..\..\..\10-Common\lib\debug\win32\mpu2.lib
@echo.
@echo      ==============================================
@echo      =      make win32 debug mpu lib      =
@echo      ==============================================
nmake /NOLOGO /A /B /f mpulib.mak CFG="MPU2Lib - Win32 Debug"

del /F /Q release\*.*
del ..\..\..\10-Common\lib\release\win32\mpu2.lib
@echo.
@echo      ==============================================
@echo      =      make win32 release mpu lib     =
@echo      ==============================================
nmake /NOLOGO /A /B /f mpu2lib.mak CFG="MPU2Lib - Win32 Release"

