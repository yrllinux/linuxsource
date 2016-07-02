cd prj_win32

@echo.
@echo      ==============================================
@echo      =       making multimtsimulib - debug        =
@echo      ==============================================
nmake /NOLOGO /A /B /f MultiMtSimuLib.mak CFG="MultiMtSimuLib - Win32 TestDebug" CLEAN
nmake /NOLOGO /A /B /f MultiMtSimuLib.mak CFG="MultiMtSimuLib - Win32 TestDebug" /x nmakeError.log >..\..\..\..\..\10-common\version\compileinfo\mcuunittestmultimtsimulib_d.txt

@echo.
@echo      ==============================================
@echo      =       making multimtsimulib - release      =
@echo      ==============================================
nmake /NOLOGO /A /B /f MultiMtSimuLib.mak CFG="MultiMtSimuLib - Win32 TestRelease" CLEAN
nmake /NOLOGO /A /B /f MultiMtSimuLib.mak CFG="MultiMtSimuLib - Win32 TestRelease" /x nmakeError.log >..\..\..\..\..\10-common\version\compileinfo\mcuunittestmultimtsimulib_r.txt

cd ..
