set PRJ_DIR=.\

@echo.
@echo      ==============================================
@echo      =       make 8260 debug DsiAgent.a           =
@echo      ==============================================
set BUILD_SPEC=DsiAgentDebug
make clean
make -e -S archive
make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release DsiAgent.a         =
@echo      ==============================================
set BUILD_SPEC=DsiAgentRelease
make clean
make -e -S archive
make clean

set BUILD_SPEC=
set PRJ_DIR=
