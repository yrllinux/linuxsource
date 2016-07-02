@echo off
@rem =============win32 version====================

del /F /Q Debug\*.*
del /F /Q Release\*.*

rem nmake /NOLOGO /A /B /f h320mtadp.mak CFG="h320mtadp - Win32 Debug" /x nmakeError.log > ..\..\..\10-common\version\CompileInfo\h320mtadp_Win32_D.txt 

nmake /NOLOGO /A /B /f h320mtadp.mak CFG="h320mtadp - Win32 Release" /x nmakeError.log > ..\..\..\10-common\version\CompileInfo\h320mtadp_Win32_R.txt 

rem copy /Y Debug\h320mtadp.exe ..\..\..\10-common\version\debug\win32
copy /Y Release\h320mtadp.exe ..\..\..\10-common\version\Release\win32

echo 不再编译H320单元测试
rem copy /Y debug\h320mtadp.exe ..\..\..\10-common\version\debug\test\unittest\service2\h320mtadp
rem copy /Y Release\h320mtadp.exe ..\..\..\10-common\version\release\test\unittest\service2\h320mtadp
rem copy /Y MccCfg.ini ..\..\..\10-common\version\debug\test\unittest\service2\h320mtadp
rem copy /Y MccCfg.ini ..\..\..\10-common\version\release\test\unittest\service2\h320mtadp

cd ..

echo on