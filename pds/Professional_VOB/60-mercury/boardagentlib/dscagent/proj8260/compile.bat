set PRJ_DIR=.\

@echo.
@echo      ==============================================
@echo      =       make 8260 debug DscAgent.a           =
@echo      ==============================================
set BUILD_SPEC=DscAgentDebug
make clean
make -e -S archive
make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release DscAgent.a         =
@echo      ==============================================
set BUILD_SPEC=DscAgentRelease
make clean
make -e -S archive
make clean

set BUILD_SPEC=
set PRJ_DIR=
