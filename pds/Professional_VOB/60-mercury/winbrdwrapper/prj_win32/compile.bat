@echo off
del /F /Q debug\*.*
nmake /NOLOGO /A /B /f winbrdwrapper.mak CFG="winbrdwrapper - Win32 Debug" 

del /F /Q release\*.*
nmake /NOLOGO /A /B /f winbrdwrapper.mak CFG="winbrdwrapper - Win32 Release" 
cd..

echo on