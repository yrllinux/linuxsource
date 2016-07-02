del /F /Q release\*.*
nmake /NOLOGO /A /B /f recloader.mak CFG="recloader - Win32 Release" 
copy /Y release\recloader.exe ..\..\..\..\10-common\version\release\win32

cd ..\..
