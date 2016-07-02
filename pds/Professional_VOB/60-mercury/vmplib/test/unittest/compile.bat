@echo 不再编译vmplib单元测试
@goto Stop

cd prj_win32

@echo.
@echo      ==============================================
@echo      =  making board manager cppunit test - debug =
@echo      ==============================================
nmake /NOLOGO /A /B /f VMPUintTest.mak CFG="VMPUintTest - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f VMPUintTest.mak CFG="VMPUintTest - Win32 Debug" /x nmakeError.log > ..\..\..\..\..\10-Common\version\compileinfo\vmpunittest_d.txt

@if not exist "..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\vmp/" mkdir "..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\vmp"
copy /Y Debug\vmpunittest.exe ..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\vmp

@echo.
@echo      ===============================================
@echo      = making board manager cppunit test - release =
@echo      ===============================================
nmake /NOLOGO /A /B /f VMPUintTest.mak CFG="VMPUintTest - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f VMPUintTest.mak CFG="VMPUintTest - Win32 Release" /x nmakeError.log > ..\..\..\..\..\10-Common\version\compileinfo\vmpunittest_r.txt

@if not exist "..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\vmp/" mkdir "..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\vmp"
copy /Y Release\vmpunittest.exe ..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\vmp

cd ..

:Stop