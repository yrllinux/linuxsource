
set PRJ_DIR=.\

@echo.
@echo      ==============================================
@echo      =       make 8260 debug mcueqpsimu.a         =
@echo      ==============================================
set BUILD_SPEC=McuEqpSimuDebug
make clean
make -e -S archive
make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release McuAgent.a         =
@echo      ==============================================
set BUILD_SPEC=McuEqpSimuRelease
make clean
make -e -S archive
make clean

set PRJ_DIR=

cd ..
