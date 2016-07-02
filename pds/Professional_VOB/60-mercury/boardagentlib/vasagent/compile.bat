cd proj860

set PRJ_DIR=.\

@echo.
@echo      ==============================================
@echo      =       make 860 debug VasAgent.a           =
@echo      ==============================================
set BUILD_SPEC=VasAgentDebug
make clean
make -e -S archive > ../../../../10-COMMON/version/CompileInfo/vasagent_d.txt
make clean

@echo.
@echo      ==============================================
@echo      =       make 860 release VasAgent.a         =
@echo      ==============================================
set BUILD_SPEC=VasAgentRelease
make clean
make -e -S archive > ../../../../10-COMMON/version/CompileInfo/vasagent_r.txt
make clean

set BUILD_SPEC=
set PRJ_DIR=

cd ..
cd ..