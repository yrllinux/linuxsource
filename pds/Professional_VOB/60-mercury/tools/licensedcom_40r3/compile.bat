
@echo.
@echo      ==============================================
@echo      =      make win32 debug mcu_license_DCom              =
@echo      ==============================================

mkdir ..\..\..\..\10-common\version\debug\win32\mculicensedcom
del /F /Q Debug\*.*
nmake /NOLOGO /A /B /f mculicense.mak CFG="mculicense - Win32 Debug" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\mculicensedcom_debug.txt
copy /Y debug\mculicense.dll ..\..\..\10-common\version\debug\win32\mculicensedcom
copy /Y mculicense.idl ..\..\..\10-common\version\debug\win32\mculicensedcom
copy /Y mculicense_i.c ..\..\..\10-common\version\debug\win32\mculicensedcom
copy /Y mculicense_p.c ..\..\..\10-common\version\debug\win32\mculicensedcom
copy /Y mculicense.tlb ..\..\..\10-common\version\debug\win32\mculicensedcom

@echo.
@echo      ==============================================
@echo      =      make win32 release mcu_license_DCom            =
@echo      ==============================================

mkdir ..\..\..\..\10-common\version\release\win32\mculicensedcom
del /F /Q ReleaseMinSize\*.*
nmake /NOLOGO /A /B /f mculicense.mak CFG="mculicense - Win32 Release MinSize" /x nmakeError.log > ..\..\..\10-common\version\compileInfo\mculicensedcom_release.txt
copy /Y ReleaseMinSize\mculicense.dll ..\..\..\10-common\version\release\win32\mculicensedcom
copy /Y mculicense.idl ..\..\..\10-common\version\release\win32\mculicensedcom
copy /Y mculicense_i.c ..\..\..\10-common\version\release\win32\mculicensedcom
copy /Y mculicense_p.c ..\..\..\10-common\version\release\win32\mculicensedcom
copy /Y mculicense.tlb ..\..\..\10-common\version\release\win32\mculicensedcom
