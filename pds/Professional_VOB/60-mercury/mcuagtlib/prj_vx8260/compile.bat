rem call D:\tornado\host\x86-win32\bin\torvars.bat

set PRJ_DIR=.\

@echo.
@echo      ==============================================
@echo      =       make 8260 debug McuAgent.a           =
@echo      ==============================================
set BUILD_SPEC=McuAgentDebug
make clean
make -e -S archive
make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release McuAgent.a         =
@echo      ==============================================
set BUILD_SPEC=McuAgentRelease
make clean
make -e -S archive
make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 debug McuAgentMt.a         =
@echo      ==============================================
set BUILD_SPEC=McuAgentMtDebug
make clean
make -e -S archive
make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release McuAgentMt.a       =
@echo      ==============================================
set BUILD_SPEC=McuAgentMtRelease
make clean
make -e -S archive
make clean

set PRJ_DIR=

cd ..
