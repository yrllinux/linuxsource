cd proj8260

set PRJ_DIR=.\

@echo.
@echo      ==============================================
@echo      =       make 8260 debug CriAgent.a           =
@echo      ==============================================
set BUILD_SPEC=CriAgentDebug
make clean
make -e -S archive
make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release CriAgent.a         =
@echo      ==============================================
set BUILD_SPEC=CriAgentRelease
make clean
make -e -S archive
make clean

set BUILD_SPEC=
set PRJ_DIR=

cd ..
cd conf
copy brdcfg.ini ../../../../10-common/version/config

cd ..
cd ..