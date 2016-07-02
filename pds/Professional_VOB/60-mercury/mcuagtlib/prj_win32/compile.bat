@echo.
@echo      ==============================================
@echo      =      make win32 debug mcuagt.lib           =
@echo      ==============================================
del /F /Q Debug\*.*
nmake /NOLOGO /A /B /f mcuagtlib.mak CFG="mcuagtlib - Win32 Debug" /x nmakeError.log 

@echo.
@echo      ==============================================
@echo      =      make win32 release mcuagt.lib         =
@echo      ==============================================
del /F /Q Release\*.*
nmake /NOLOGO /A /B /f mcuagtlib.mak CFG="mcuagtlib - Win32 Release" /x nmakeError.log

cd ..
