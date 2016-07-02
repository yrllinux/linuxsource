
@echo.
@echo      ==============================================
@echo      =      make win32 mcu IntTest lib            =
@echo      ==============================================

cd prjinttest
nmake /f prjinttest.mak clean CFG="prjIntTest - Win32 Debug"
nmake /NOLOGO /A /B /f prjinttest.mak CFG="prjIntTest - Win32 Debug"

nmake /f prjinttest.mak clean CFG="prjIntTest - Win32 Release"
nmake /NOLOGO /A /B /f prjinttest.mak CFG="prjIntTest - Win32 Release"

cd ..\

