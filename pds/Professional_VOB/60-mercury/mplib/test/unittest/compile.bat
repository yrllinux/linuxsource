@echo 不再编译mplib单元测试
@goto Stop

echo making mp cppunit test

cd prj_win32

@echo.
@echo      ==============================================
@echo      =       making mp cppunit test - debug       =
@echo      ==============================================
nmake /NOLOGO /A /B /f prj_win32.mak CFG="prj_win32 - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f prj_win32.mak CFG="prj_win32 - Win32 Debug" /x nmakeError.log >..\..\..\..\..\10-Common\version\compileinfo\mpunittest_d.txt

@if not exist "..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\mplib/" mkdir "..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\mplib"
copy /Y Debug\mpunittest.exe ..\..\..\..\..\10-Common\version\debug\test\UnitTest\service2\mplib\mpunittest.exe

@echo.
@echo      ===============================================
@echo      =      making mp cppunit test - release       =
@echo      ===============================================
nmake /NOLOGO /A /B /f prj_win32.mak CFG="prj_win32 - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f prj_win32.mak CFG="prj_win32 - Win32 Release" /x nmakeError.log >..\..\..\..\..\10-Common\version\compileinfo\mpunittest_r.txt

@if not exist "..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\mplib/" mkdir "..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\mplib"
copy /Y Release\mpunittest.exe ..\..\..\..\..\10-Common\version\release\test\UnitTest\service2\mplib\mpunittest.exe

cd ..

:Stop