cd prj_win32

@echo.
@echo      ==============================================
@echo      =       making msunittest - debug        	=
@echo      ==============================================
nmake /NOLOGO /A /B /f msunittest.mak CFG="msunittest - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f msunittest.mak CFG="msunittest - Win32 Debug" /x nmakeError.log >..\..\..\..\..\10-common\version\compileinfo\mcuunittestmstest_d.txt

copy /Y Debug\msunittest.exe ..\..\..\..\..\10-common\version\debug\test\unittest\service2\mcu
copy /Y simmcscfg.ini ..\..\..\..\..\10-common\version\debug\test\unittest\service2\mcu

@echo.
@echo      ==============================================
@echo      =       making msunittest - release      	=
@echo      ==============================================
nmake /NOLOGO /A /B /f msunittest.mak CFG="msunittest - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f msunittest.mak CFG="msunittest - Win32 Release" /x nmakeError.log >..\..\..\..\..\10-common\version\compileinfo\mcuunittestmstest_r.txt

copy /Y Release\msunittest.exe ..\..\..\..\..\10-common\version\release\test\unittest\service2\mcu
copy /Y simmcscfg.ini ..\..\..\..\..\10-common\version\release\test\unittest\service2\mcu

cd ..
