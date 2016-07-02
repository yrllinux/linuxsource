cd prj_win32

@echo.
@echo      ==============================================
@echo      =  making mcu cppunit test - debug   =
@echo      ==============================================
nmake /NOLOGO /A /B /f McuUnitTest.mak CFG="McuUnitTest - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f McuUnitTest.mak CFG="McuUnitTest - Win32 Debug" /x nmakeError.log >..\..\..\..\..\10-common\version\compileinfo\mcuunittest_d.txt

@if not exist "..\..\..\..\..\10-common\version\debug\test\unittest\service2\mcu/" mkdir "..\..\..\..\..\10-common\version\debug\test\unittest\service2\mcu" 
copy /Y Debug\McuUnitTest.exe ..\..\..\..\..\10-common\version\debug\test\unittest\service2\mcu

@echo.
@echo      ===============================================
@echo      =  making mcu cppunit test - release  =
@echo      ===============================================
nmake /NOLOGO /A /B /f McuUnitTest.mak CFG="McuUnitTest - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f McuUnitTest.mak CFG="McuUnitTest - Win32 Release" /x nmakeError.log >..\..\..\..\..\10-common\version\compileinfo\mcuunittest_r.txt

@if not exist "..\..\..\..\..\10-common\version\release\test\unittest\service2\mcu/" mkdir "..\..\..\..\..\10-common\version\release\test\unittest\service2\mcu"
copy /Y Release\McuUnitTest.exe ..\..\..\..\..\10-common\version\release\test\unittest\service2\mcu


cd ..\..\MultiMtSimuLib
call compile.bat

cd ..\simdcs
call compile.bat

cd ..\msunittest
call compile.bat

cd ..\..\prj_win32

@echo.
@echo      ==============================================
@echo      =      make win32 test debug mcu.exe            =
@echo      ==============================================

del /F /Q TestDebug\*.*
nmake /NOLOGO /A /B /f prjmcu.mak CFG="prjMcu - Win32 TestDebug" /x nmakeError.log >..\..\..\..\..\10-common\version\compileinfo\mcu_d.txt
copy /Y mcu.exe ..\..\..\10-common\version\debug\test\unittest\service2\mcu

@echo.
@echo      ==============================================
@echo      =      make win32 test release mcu.exe       =
@echo      ==============================================

del /F /Q TestRelease\*.*
nmake /NOLOGO /A /B /f prjmcu.mak CFG="prjMcu - Win32 TestRelease" /x nmakeError.log >..\..\..\..\..\10-common\version\compileinfo\mcu_r.txt
copy /Y mcu.exe ..\..\..\10-common\version\release\test\unittest\service2\mcu

