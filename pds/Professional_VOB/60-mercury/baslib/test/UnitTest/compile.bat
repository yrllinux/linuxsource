@echo 不再编译BASLig单元测试
@goto Stop
echo making bas cppunit test

cd prj_win32

@echo.
@echo      ==============================================
@echo      =  making board manager cppunit test - debug =
@echo      ==============================================
nmake /NOLOGO /A /B /f BasUnitTest.mak CFG="BasUnitTest - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f BasUnitTest.mak CFG="BasUnitTest - Win32 Debug" /x nmakeError.log > ..\..\..\..\..\10-Common\version\compileinfo\bas_unittest_d.txt

@if not exist "..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\bas/" mkdir "..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\bas"
copy /Y Debug\basCppUnit.exe ..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\bas

@echo.
@echo      ===============================================
@echo      = making board manager cppunit test - release =
@echo      ===============================================
nmake /NOLOGO /A /B /f BasUnitTest.mak CFG="BasUnitTest - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f BasUnitTest.mak CFG="BasUnitTest - Win32 Release" /x nmakeError_r.log > ..\..\..\..\..\10-Common\version\compileinfo\bas_unittest_r.txt

@if not exist "..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\bas/" mkdir "..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\bas"
copy /Y Release\basCppUnit.exe ..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\bas

cd ..

:Stop