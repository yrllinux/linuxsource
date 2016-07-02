set PRJ_DIR=.\

@echo.
@echo      ==============================================
@echo      =       make 8260 debug MmpAgent.a           =
@echo      ==============================================
set BUILD_SPEC=MmpAgentDebug
make clean
make -e -S archive
make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release MmpAgent.a         =
@echo      ==============================================
set BUILD_SPEC=MmpAgentRelease
make clean
make -e -S archive
make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 debug MmpAgentMt.a         =
@echo      ==============================================
@echo set BUILD_SPEC=MmpAgentMtDebug
@echo make clean
@echo make -e -S archive
@echo make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release MmpAgentMt.a       =
@echo      ==============================================
@echo set BUILD_SPEC=MmpAgentMtRelease
@echo make clean
@echo make -e -S archive
@echo make clean

set BUILD_SPEC=
set PRJ_DIR=
