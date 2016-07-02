@echo off
echo VC ospclient build begin

rem ospclient.exe

if /I "%1" == "noclean" (
    echo build noclean debug
	nmake /NOLOGO /B /f ospclient.mak CFG="ospclient - Win32 Debug" /x nmakeError_D.log > ..\version\compileinfo\ospclient.txt
	
	echo build noclean release
    nmake /NOLOGO /B /f ospclient.mak CFG="ospclient - Win32 Release" /x nmakeError_R.log > ..\version\compileinfo\ospclient.txt
) else (
        echo build clean debug
        del /F /Q debug\*.*
        nmake /NOLOGO /A /B /f ospclient.mak CFG="ospclient - Win32 Debug" NO_EXTERNAL_DEPS="1" RECURSE="0"  /x nmakeError_D.log > ..\version\compileinfo\ospclient.txt
        
        echo build clean release
        del /F /Q release\*.*
        nmake /NOLOGO /A /B /f ospclient.mak CFG="ospclient - Win32 Release" NO_EXTERNAL_DEPS="1" RECURSE="0"  /x nmakeError_R.log > ..\version\compileinfo\ospclient.txt
)

echo VC ospclient over
echo on
