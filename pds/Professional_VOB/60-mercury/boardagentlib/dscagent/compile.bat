cd proj8260

set PRJ_DIR=.\

@echo.
@echo      ==============================================
@echo      =       make 8260 debug DscAgent.a           =
@echo      ==============================================
set BUILD_SPEC=DscAgentDebug
make clean
make -e -S archive > ../../../../10-COMMON/version/CompileInfo/dscagent_d.txt
make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release DscAgent.a         =
@echo      ==============================================
set BUILD_SPEC=DscAgentRelease
make clean
make -e -S archive > ../../../../10-COMMON/version/CompileInfo/dscagent_r.txt
make clean

set BUILD_SPEC=
set PRJ_DIR=

cd ..
cd ..