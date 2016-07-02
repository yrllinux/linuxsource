@echo.
@echo      ==============================================
@echo      =      make win32 debug eqpsimu.exe          =
@echo      ==============================================
nmake /NOLOGO /A /B /f eqpsimu.mak CFG="eqpsimu - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f eqpsimu.mak CFG="eqpsimu - Win32 Debug" /x nmakeError.log
copy /Y debug\eqpsimu.exe ..\..\..\10-common\version\debug\win32\eqpsimulator.exe
copy /y nmakeError.log ..\..\..\10-common\version\compileinfo\eqpsimu_d.txt
@echo.
@echo      ==============================================
@echo      =      make win32 release eqpsimu.lib        =
@echo      ==============================================
nmake /NOLOGO /A /B /f eqpsimu.mak CFG="eqpsimu - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f eqpsimu.mak CFG="eqpsimu - Win32 Release" /x nmakeError.log
copy /Y release\eqpsimu.exe ..\..\..\10-common\version\release\win32\eqpsimulator.exe
copy /y nmakeError.log ..\..\..\10-common\version\compileinfo\eqpsimu_r.txt

