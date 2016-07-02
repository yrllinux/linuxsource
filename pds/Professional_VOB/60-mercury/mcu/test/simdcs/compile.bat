cd prj_win32

@echo.
@echo      ==============================================
@echo      =       making simdcslib - debug        	=
@echo      ==============================================
nmake /NOLOGO /A /B /f simdcs.mak CFG="simdcs - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f simdcs.mak CFG="simdcs - Win32 Debug" /x nmakeError.log >..\..\..\..\..\10-common\version\compileinfo\mcuunittestsimdcs_d.txt

copy /Y Debug\simdcs.exe ..\..\..\..\..\10-common\version\debug\test\unittest\service2\mcu

@echo.
@echo      ==============================================
@echo      =       making simdcslib - release      	=
@echo      ==============================================
nmake /NOLOGO /A /B /f simdcs.mak CFG="simdcs - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f simdcs.mak CFG="simdcs - Win32 Release" /x nmakeError.log >..\..\..\..\..\10-common\version\compileinfo\mcuunittestsimdcs_r.txt

copy /Y Release\simdcs.exe ..\..\..\..\..\10-common\version\release\test\unittest\service2\mcu

cd ..
