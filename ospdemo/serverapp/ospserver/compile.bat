@echo off
echo VC Server ospserver build begin

rem ospserver.exe

if /I "%1" == "noclean" (
    echo build noclean debug
	nmake /NOLOGO /B /f ospserver.mak CFG="ospserver - Win32 Debug" /x nmakeError_D.log > ..\version\compileinfo\ospserver_d.txt
	
    echo build noclean release
    nmake /NOLOGO /B /f ospserver.mak CFG="ospserver - Win32 Release" /x nmakeError_R.log > ..\version\compileinfo\ospserver_r.txt
) else (
        echo build clean debug
        del /F /Q debug\*.*
        nmake /NOLOGO /A /B /f ospserver.mak CFG="ospserver - Win32 Debug" NO_EXTERNAL_DEPS="1" RECURSE="0"  /x nmakeError_D.log  > ..\version\compileinfo\ospserver_d.txt
        
        echo build clean release
        del /F /Q release\*.*
        nmake /NOLOGO /A /B /f ospserver.mak CFG="ospserver - Win32 Release" NO_EXTERNAL_DEPS="1" RECURSE="0"  /x nmakeError_R.log  > ..\version\compileinfo\ospserver_r.txt
)

echo VC ospserver over
echo on
