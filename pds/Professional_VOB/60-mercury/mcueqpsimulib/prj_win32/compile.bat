@echo.
@echo      ==============================================
@echo      =      make win32 debug mcueqpsimu.lib       =
@echo      ==============================================
del /F /Q Debug\*.*
nmake /NOLOGO /A /B /f mcueqpsimu.mak CFG="mcueqpsimu - Win32 Debug" /x nmakeError.log 
nmake /NOLOGO /A /B /f mcueqpsimu_hd.mak CFG="mcueqpsimu - Win32 Debug" /x nmakeError_hd.log 

@echo.
@echo      ==============================================
@echo      =      make win32 release mcueqpsimu.lib     =
@echo      ==============================================
del /F /Q Release\*.*
nmake /NOLOGO /A /B /f mcueqpsimu.mak CFG="mcueqpsimu - Win32 Release" /x nmakeError.log
nmake /NOLOGO /A /B /f mcueqpsimu_hd.mak CFG="mcueqpsimu - Win32 Release" /x nmakeError_hd.log

cd ..
