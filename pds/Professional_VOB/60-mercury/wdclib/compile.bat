
@rem =============win32 version====================

cd  prj_win
set PRJ_DIR=./

if /I "%1" == "noclean" (

nmake /NOLOGO /B /f watchdogclient.mak CFG="watchdogclient - Win32 Debug" /x  ..\..\..\10-common\version\compileinfo\watchdogclient_win32_debug.txt 
nmake /NOLOGO /B /f watchdogclient.mak CFG="watchdogclient- Win32 Release" /x  ..\..\..\10-common\version\compileinfo\watchdogclient_win32_release.txt 

) else (

del /F /Q Debug\*.*
del /F /Q Release\*.*

nmake /NOLOGO /A /B /f watchdogclient.mak CFG="watchdogclient - Win32 Debug" /x  ..\..\..\10-common\version\CompileInfo\watchdogclient_win32_debug.txt 
nmake /NOLOGO /A /B /f watchdogclient.mak CFG="watchdogclient - Win32 Release" /x  ..\..\..\10-common\version\CompileInfo\watchdogclient_win32_release.txt 

)

cd ..


@rem =============vxWorks 8260 version===============

@rem =============copy NetDebugCfg.ini file===============

