@echo 不再编译recorder单元测试
@goto Stop

@echo.
@echo      ==============================================
@echo      =      making recorder cppunit test          =
@echo      ==============================================

cd prj_win32
del /F /Q .\debug\*.* 
del /F /Q .\release\*.* 
@echo.
@echo      ====================================================
@echo      =  make win32 debug recorder cppunit test - debug  =
@echo      ====================================================
nmake /NOLOGO /A /B /f RecUnitTest.mak CFG="RecUnitTest - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f RecUnitTest.mak CFG="RecUnitTest - Win32 Debug" /x nmakeError.log >..\..\..\..\..\10-Common\version\compileinfo\RecoderUnittest_d.txt

@if not exist "..\..\..\..\..\10-common\version\debug\test\unittest\service2\recorder/" mkdir "..\..\..\..\..\10-common\version\debug\test\unittest\service2\recorder"
copy /Y Debug\recorderCppUnit.exe ..\..\..\..\..\10-common\version\debug\test\unittest\service2\recorder\recorderCppUnit.exe
@echo.
@echo      ======================================================
@echo      = make win32 release recorder cppunit test - release =
@echo      ======================================================
nmake /NOLOGO /A /B /f RecUnitTest.mak CFG="RecUnitTest - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f RecUnitTest.mak CFG="RecUnitTest - Win32 Release" /x nmakeError.log >..\..\..\..\..\10-Common\version\compileinfo\RecoderUnittest_r.txt

@if not exist "..\..\..\..\..\10-common\version\release\test\unittest\service2\recorder/" mkdir "..\..\..\..\..\10-common\version\release\test\unittest\service2\recorder"
copy /Y Release\recorderCppUnit.exe ..\..\..\..\..\10-common\version\release\test\unittest\service2\recorder\recorderCppUnit.exe


@if not exist "..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\recorder\conf/" mkdir "..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\recorder\conf"
copy /Y ..\conf\recAutoTestCfg.ini ..\..\..\..\..\10-common\version\debug\test\unittest\service2\recorder\conf

@if not exist "..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\recorder\conf/" mkdir "..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\recorder\conf"
copy /Y ..\conf\recAutoTestCfg.ini ..\..\..\..\..\10-common\version\release\test\unittest\service2\recorder\conf

cd ..

:Stop