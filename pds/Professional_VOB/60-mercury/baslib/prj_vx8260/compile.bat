@echo if "%1"=="" goto cmpl
@echo call D:\tornado\host\x86-win32\bin\torvars.bat
@echo :cmpl

set PRJ_DIR=.\
@echo.
@echo     ==========================================
@echo     =   make BAS MPC8260 debug lib    =
@echo     ==========================================
set BUILD_SPEC=debug
make  clean
make  -e archive
make  clean
@echo.
@echo     ==========================================
@echo     =   make BAS MPC8260 release lib  =
@echo     ==========================================
set BUILD_SPEC=release
make clean
make -e archive
make clean


set BUILD_SPEC=
