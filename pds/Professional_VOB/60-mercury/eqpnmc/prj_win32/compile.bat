@echo off
del /F /Q debug\*.*
nmake /NOLOGO /A /B /f eqpnmc.mak CFG="eqpnmc - Win32 Debug" 
copy /Y debug\eqpnmc.exe ..\..\..\10-common\version\debug\win32


del /F /Q release\*.*
nmake /NOLOGO /A /B /f eqpnmc.mak CFG="eqpnmc - Win32 Release" 
copy /Y release\eqpnmc.exe ..\..\..\10-common\version\release\win32

cd..

echo on
