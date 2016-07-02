cd proj8260

set PRJ_DIR=.\

@echo.
@echo      ==============================================
@echo      =       make 8260 debug DsiAgent.a           =
@echo      ==============================================
set BUILD_SPEC=DsiAgentDebug
make clean
make -e -S archive > ../../../../10-COMMON/version/CompileInfo/dsiagent_d.txt
make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release DsiAgent.a         =
@echo      ==============================================
set BUILD_SPEC=DsiAgentRelease
make clean
make -e -S archive > ../../../../10-COMMON/version/CompileInfo/dsiagent_r.txt
make clean

set BUILD_SPEC=
set PRJ_DIR=

cd ..
cd ..