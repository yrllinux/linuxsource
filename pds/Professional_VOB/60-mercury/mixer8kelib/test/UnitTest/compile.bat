@echo off
echo 不再编译AudMixLib单元测试
goto Stop

cd prj_win32

@echo.
@echo      ==============================================
@echo      =  making audio mixer cppunit test - debug   =
@echo      ==============================================
nmake /NOLOGO /A /B /f MixerUnitTest.mak CFG="MixerUnitTest - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f MixerUnitTest.mak CFG="MixerUnitTest - Win32 Debug" /x nmakeError.log > ..\..\..\..\..\10-Common\version\compileinfo\MixerUnitTest_d.txt

@if not exist "..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\audmixer/" mkdir "..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\audmixer"
copy /Y Debug\mixerUnitTest.exe ..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\audmixer

@echo.
@echo      ===============================================
@echo      =  making audio mixer cppunit test - release  =
@echo      ===============================================
nmake /NOLOGO /A /B /f MixerUnitTest.mak CFG="MixerUnitTest - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f MixerUnitTest.mak CFG="MixerUnitTest - Win32 Release" /x nmakeError.log > ..\..\..\..\..\10-Common\version\compileinfo\MixerUnitTest_r.txt

@if not exist "..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\audmixer" mkdir "..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\audmixer"
copy /Y Release\mixerUnitTest.exe ..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\audmixer
cd ..

:Stop


echo on

