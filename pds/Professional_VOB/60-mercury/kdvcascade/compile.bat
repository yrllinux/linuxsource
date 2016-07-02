@rem =============win32 version====================

cd  prj_win32


if /I "%1" == "noclean" (

nmake /NOLOGO /A /B /f kdvcascade.mak CFG="MMcuLib - Win32 Debug" /x ..\..\..\10-common\version\compileinfo\kdvcascade_win32_debug.txt 
nmake /NOLOGO /A /B /f kdvcascade.mak CFG="MMcuLib - Win32 Release" /x ..\..\..\10-common\version\compileinfo\kdvcascade_win32_release.txt 

) else (

del /F /Q Debug\*.*
del /F /Q Release\*.*

nmake /NOLOGO /A /B /f kdvcascade.mak CFG="MMcuLib - Win32 Debug" /x ..\..\..\10-common\version\compileinfo\kdvcascade_win32_debug.txt 
nmake /NOLOGO /A /B /f kdvcascade.mak CFG="MMcuLib - Win32 Release" /x ..\..\..\10-common\version\compileinfo\kdvcascade_win32_release.txt 

)



