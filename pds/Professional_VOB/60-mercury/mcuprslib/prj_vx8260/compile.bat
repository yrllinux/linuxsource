set PRJ_DIR=.\

@echo.
@echo      ==============================================
@echo      =       make 8260 debug mcuprs.a           =
@echo      ==============================================
set BUILD_SPEC=McuPrsDebug
make clean
make -e -S archive
make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release mcuprs.a         =
@echo      ==============================================
set BUILD_SPEC=McuPrsRelease
make clean
make -e -S archive
make clean

set BUILD_SPEC=
set PRJ_DIR=

cd ..