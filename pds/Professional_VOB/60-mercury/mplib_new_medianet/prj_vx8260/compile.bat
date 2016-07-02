Rem call D:\tornado\host\x86-win32\bin\torvars.bat

set PRJ_DIR=.\

@echo.
@echo      ==============================================
@echo      =      make 8260 debug mp.a                  =
@echo      ==============================================

set BUILD_SPEC=MpDebug
make clean
make -e archive
make clean

@echo.
@echo      ==============================================
@echo      =      make 8260 release mp.a                =
@echo      ==============================================

set BUILD_SPEC=MpRelease
make clean
make -e archive
make clean
set BUILD_SPEC=

cd ..\
