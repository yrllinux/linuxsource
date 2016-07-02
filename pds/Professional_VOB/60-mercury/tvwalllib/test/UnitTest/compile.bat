@echo 不再编译tvwalllib单元测试
@goto Stop

cd prj_win32

@echo.
@echo      =================================================
@echo      =  make win32 debug agent cppunit test - debug  =
@echo      =================================================
nmake /NOLOGO /A /B /f TvWallUnitTest.mak CFG="TvWallUnitTest - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f TvWallUnitTest.mak CFG="TvWallUnitTest - Win32 Debug" /x nmakeError.log >..\..\..\..\..\10-Common\version\compileinfo\tvwallunittest_d.txt

@if not exist "..\..\..\..\..\10-Common\version\debug\test\unittest\service2\tvwall/" mkdir "..\..\..\..\..\10-Common\version\debug\test\unittest\service2\tvwall"
copy /Y Debug\TvWallUnitTest.exe ..\..\..\..\..\10-Common\version\debug\test\unittest\service2\tvwall
@echo.
@echo      =================================================
@echo      = make win32 release agent cppunit test - debug =
@echo      =================================================
nmake /NOLOGO /A /B /f TvWallUnitTest.mak CFG="TvWallUnitTest - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f TvWallUnitTest.mak CFG="TvWallUnitTest - Win32 Release" /x nmakeError.log >..\..\..\..\..\10-Common\version\compileinfo\tvwallunittest_r.txt

@if not exist "..\..\..\..\..\10-Common\version\release\test\unittest\service2\tvwall" mkdir "..\..\..\..\..\10-Common\version\release\test\unittest\service2\tvwall"
copy /Y Release\TvWallUnitTest.exe ..\..\..\..\..\10-Common\version\release\test\unittest\service2\tvwall
cd ..

:Stop