set PRJ_DIR=.\

@echo.
@echo      ==============================================
@echo      =       make 8260 debug DriAgent.a           =
@echo      ==============================================
set BUILD_SPEC=DriAgentDebug
make clean
make -e -S archive
make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release DriAgent.a         =
@echo      ==============================================
set BUILD_SPEC=DriAgentRelease
make clean
make -e -S archive
make clean

set BUILD_SPEC=
set PRJ_DIR=
