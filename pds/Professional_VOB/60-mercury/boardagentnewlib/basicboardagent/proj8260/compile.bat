set PRJ_DIR=.\

@echo.
@echo      ==============================================
@echo      =       make 8260 debug BasicBoardAgent.a           =
@echo      ==============================================
set BUILD_SPEC=BasicBoardAgentDebug
make clean
make -e -S archive
make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release BasicBoardAgent.a         =
@echo      ==============================================
set BUILD_SPEC=BasicBoardAgentRelease
make clean
make -e -S archive
make clean

set BUILD_SPEC=
set PRJ_DIR=
