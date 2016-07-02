
@rem =============win32 version====================


@rem =============del win32 *.dep files====================

del /F /Q prj_win32\kdvmediavodtest.dep


cd  project
set PRJ_DIR=./

if /I "%1" == "noclean" (

nmake /NOLOGO /B /f kdvmediavodtest.mak CFG="kdvmediavodtest - Win32 Debug" /x  ..\..\..\..\..\10-common\version\CompileInfo\kdvmeidavod\test\kdvmediavodtest_win32_debug.txt 
nmake /NOLOGO /B /f kdvmediavodtest.mak CFG="kdvmediavodtest - Win32 Release" /x  ..\..\..\..\..\10-common\version\CompileInfo\kdvmeidavod\test\kdvmediavodtest_win32_release.txt 

) else (

del /F /Q Debug\*.*
del /F /Q Release\*.*

nmake /NOLOGO /B /f kdvmediavodtest.mak CFG="kdvmediavodtest - Win32 Debug" /x  ..\..\..\..\..\10-common\version\CompileInfo\kdvmeidavod\test\kdvmediavodtest_win32_debug.txt 
nmake /NOLOGO /B /f kdvmediavodtest.mak CFG="kdvmediavodtest - Win32 Release" /x  ..\..\..\..\..\10-common\version\CompileInfo\kdvmeidavod\test\kdvmediavodtest_win32_release.txt 


)

cd ..

