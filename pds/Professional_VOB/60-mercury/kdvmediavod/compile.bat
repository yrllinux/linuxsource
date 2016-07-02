
@rem =============win32 version====================


@rem =============del win32 *.dep files====================

del /F /Q project\kdvmediavod.dep


cd  project
set PRJ_DIR=./

if /I "%1" == "noclean" (

nmake /NOLOGO /A /B /f kdvmediavod.mak CFG="kdvmediavod - Win32 Debug" /x  ..\..\..\10-common\version\CompileInfo\kdvmediavod_win32_debug.txt 
nmake /NOLOGO /A /B /f kdvmediavod.mak CFG="kdvmediavod - Win32 Release" /x  ..\..\..\10-common\version\CompileInfo\kdvmediavod_win32_release.txt 


) else (

del /F /Q Debug\*.*
del /F /Q Release\*.*

nmake /NOLOGO /A /B /f kdvmediavod.mak CFG="kdvmediavod - Win32 Debug" /x  ..\..\..\10-common\version\CompileInfo\kdvmediavod_win32_debug.txt 
nmake /NOLOGO /A /B /f kdvmediavod.mak CFG="kdvmediavod - Win32 Release" /x  ..\..\..\10-common\version\CompileInfo\kdvmediavod_win32_release.txt 

)

COPY /Y Debug\kdvmediavod.exe  ..\..\..\10-common\version\debug\win32\kdvmediavod\kdvmediavod.exe
copy /Y config.ini ..\..\..\10-common\version\debug\win32\kdvmediavod\config.ini

COPY /Y Release\kdvmediavod.exe  ..\..\..\10-common\version\release\win32\kdvmediavod\kdvmediavod.exe
copy /Y config.ini ..\..\..\10-common\version\release\win32\kdvmediavod\config.ini

cd ..

