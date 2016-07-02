@echo 不再编译prslib单元测试
@goto Stop
cd prj_win32

@echo.
@echo      =================================================
@echo      =  make win32 debug agent cppunit test - debug  =
@echo      =================================================
nmake /NOLOGO /A /B /f prsunittest.mak CFG="prsunittest - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f prsunittest.mak CFG="prsunittest - Win32 Debug" /x nmakeError.log > ..\..\..\..\..\10-Common\version\compileinfo\prsunittest_d.txt

@if not exist "..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\mcuprs/" mkdir "..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\mcuprs"
copy /Y Debug\prsunittest.exe ..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\mcuprs

@echo.
@echo      =================================================
@echo      = make win32 release agent cppunit test - debug =
@echo      =================================================
nmake /NOLOGO /A /B /f prsunittest.mak CFG="prsunittest - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f prsunittest.mak CFG="prsunittest - Win32 Release" /x nmakeError.log > ..\..\..\..\..\10-Common\version\compileinfo\prsunittest_r.txt

@if not exist "..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\mcuprs/" mkdir "..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\mcuprs"
copy /Y Release\prsunittest.exe ..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\mcuprs

cd ..

:Stop
