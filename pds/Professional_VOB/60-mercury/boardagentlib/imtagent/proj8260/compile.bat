set PRJ_DIR=.\

@echo.
@echo      ==============================================
@echo      =       make 8260 debug ImtAgent.a           =
@echo      ==============================================
set BUILD_SPEC=ImtAgentDebug
make clean
make -e -S archive
make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release ImtAgent.a         =
@echo      ==============================================
set BUILD_SPEC=ImtAgentRelease
make clean
make -e -S archive
make clean

set BUILD_SPEC=
set PRJ_DIR=
