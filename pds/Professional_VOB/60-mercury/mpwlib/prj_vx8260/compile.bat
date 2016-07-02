Rem call D:\tornado\host\x86-win32\bin\torvars.bat

set PRJ_DIR=.\

@echo.
@echo      ==============================================
@echo      =      make 8260 debug mpw.a                  =
@echo      ==============================================

set BUILD_SPEC=Mpwdebug
make clean
make -e archive
make clean

@echo.
@echo      ==============================================
@echo      =      make 8260 release mpw.a                =
@echo      ==============================================

set BUILD_SPEC=Mpwrelease
make clean
make -e archive
make clean
set BUILD_SPEC=

cd ..\
