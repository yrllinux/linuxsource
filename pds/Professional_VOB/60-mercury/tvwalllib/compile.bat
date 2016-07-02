@rem Rem D:\tornado\host\x86-win32\bin\torvars.bat

@rem cd prj_Vx8260
@rem call compile.bat
@rem cd..

cd prj_win32

@echo.
@echo      ==============================================
@echo      =      make win32 debug tvwall lib      =
@echo      ==============================================
nmake /NOLOGO /A /B /f win32lib.mak CFG="win32lib - Win32 Debug"


@echo.
@echo      ==============================================
@echo      =      make win32 debug tvwall lib      =
@echo      ==============================================
nmake /NOLOGO /A /B /f win32lib.mak CFG="win32lib - Win32 Release"

cd ..
