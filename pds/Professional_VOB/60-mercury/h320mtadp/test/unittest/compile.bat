@echo 不再编译H320单元测试
@goto Stop
@rem =============h320mtadp unittest version====================

mkdir "..\..\..\..\10-common\version\debug\test\unittest\service2\h320mtadp\simmt"
mkdir "..\..\..\..\10-common\version\release\test\unittest\service2\h320mtadp\simmt"
mkdir "..\..\..\..\10-common\version\debug\test\unittest\service2\h320mtadp\conf"
mkdir "..\..\..\..\10-common\version\release\test\unittest\service2\h320mtadp\conf"

cd simmcu\project

del /F /Q Debug\*.*
del /F /Q Release\*.*

nmake /NOLOGO /A /B /f simmcu.mak CFG="simmcu - Win32 Debug" /x nmakeError.log > ..\..\..\..\..\..\10-common\version\CompileInfo\H320mtadp_simmcu_Win32_D.txt 

nmake /NOLOGO /A /B /f simmcu.mak CFG="simmcu - Win32 Release" /x nmakeError.log > ..\..\..\..\..\..\10-common\version\CompileInfo\H320mtadp_simmcu_Win32_R.txt 

copy /Y Debug\simmcu.exe ..\..\..\..\..\..\10-common\version\debug\test\unittest\service2\h320mtadp\simmcu.exe
copy /Y Release\simmcu.exe ..\..\..\..\..\..\10-common\version\release\test\unittest\service2\h320mtadp\simmcu.exe

copy /Y conf\brdcfg.ini ..\..\..\..\..\..\10-common\version\debug\test\unittest\service2\h320mtadp\conf
copy /Y conf\brdcfgdebug.ini ..\..\..\..\..\..\10-common\version\debug\test\unittest\service2\h320mtadp\conf
copy /Y conf\mcucfg.ini ..\..\..\..\..\..\10-common\version\debug\test\unittest\service2\h320mtadp\conf
copy /Y conf\mcudebug.ini ..\..\..\..\..\..\10-common\version\debug\test\unittest\service2\h320mtadp\conf

copy /Y conf\brdcfg.ini ..\..\..\..\..\..\10-common\version\release\test\unittest\service2\h320mtadp\conf
copy /Y conf\brdcfgdebug.ini ..\..\..\..\..\..\10-common\version\release\test\unittest\service2\h320mtadp\conf
copy /Y conf\mcucfg.ini ..\..\..\..\..\..\10-common\version\release\test\unittest\service2\h320mtadp\conf
copy /Y conf\mcudebug.ini ..\..\..\..\..\..\10-common\version\release\test\unittest\service2\h320mtadp\conf

cd ..\..\simmt\project

del /F /Q Release\*.*

nmake /NOLOGO /A /B /f simmt.mak CFG="simmt - Win32 Debug" /x nmakeError.log > ..\..\..\..\..\..\10-common\version\CompileInfo\H320mtadp_simmt_Win32_D.txt 

nmake /NOLOGO /A /B /f simmt.mak CFG="simmt - Win32 Release" /x nmakeError.log > ..\..\..\..\..\..\10-common\version\CompileInfo\H320mtadp_simmt_Win32_R.txt 

copy /Y Release\simmt.exe ..\..\..\..\..\..\10-common\version\debug\test\unittest\service2\h320mtadp\simmt\simmt.exe
copy /Y Release\simmt.exe ..\..\..\..\..\..\10-common\version\release\test\unittest\service2\h320mtadp\simmt\simmt.exe
copy /Y MccCfg.ini ..\..\..\..\..\..\10-common\version\debug\test\unittest\service2\h320mtadp\simmt\MccCfg.ini
copy /Y MccCfg.ini ..\..\..\..\..\..\10-common\version\release\test\unittest\service2\h320mtadp\simmt\MccCfg.ini

cd ..\..\

:Stop