@echo.
@echo      ==============================================
@echo      =      make win32 debug license tools        =
@echo      ==============================================
del /F /Q Debug\*.*
nmake /NOLOGO /A /B /f licensedlg.mak CFG="licensedlg - Win32 Debug" /x nmakeError.log
@if not exist "..\..\..\10-common\version\debug\win32\mcu/" mkdir "..\..\..\10-common\version\debug\win32\mcu" 

copy debug\licensedlg.exe ..\..\..\10-common\version\debug\win32\mcu\licensegen.exe

@echo.
@echo      ==============================================
@echo      =      make win32 release license tools      =
@echo      ==============================================
del /F /Q Release\*.*
nmake /NOLOGO /A /B /f licensedlg.mak CFG="licensedlg - Win32 Release" /x nmakeError.log
@if not exist "..\..\..\10-common\version\release\win32\mcu/" mkdir "..\..\..\10-common\version\release\win32\mcu" 
copy release\licensedlg.exe ..\..\..\10-common\version\release\win32\mcu\licensegen.exe

cd ..
