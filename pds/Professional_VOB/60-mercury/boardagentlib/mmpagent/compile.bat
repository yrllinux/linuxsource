cd proj8260

set PRJ_DIR=.\

@echo.
@echo      ==============================================
@echo      =       make 8260 debug MmpAgent.a           =
@echo      ==============================================
set BUILD_SPEC=MmpAgentDebug
make clean
make -e -S archive > ../../../../10-COMMON/version/CompileInfo/mmpagent_d.txt
make clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release MmpAgent.a         =
@echo      ==============================================
set BUILD_SPEC=MmpAgentRelease
make clean
make -e -S archive > ../../../../10-COMMON/version/CompileInfo/mmpagent_r.txt
make clean

set BUILD_SPEC=
set PRJ_DIR=

cd ..

cd project

@echo.
@echo      ==============================================
@echo      =      making mmpagent win32 lib - debug     =
@echo      ==============================================

nmake /NOLOGO /A /B /f mmpagent.mak CFG="mmpagent - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f mmpagent.mak CFG="mmpagent - Win32 Debug" /x ../../../../10-COMMON/version/CompileInfo/mmpnmakeError_d.log

@echo.
@echo      ==============================================
@echo      =     making mmpagent win32 lib - release    =
@echo      ==============================================

nmake /NOLOGO /A /B /f mmpagent.mak CFG="mmpagent - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f mmpagent.mak CFG="mmpagent - Win32 Release" /x ../../../../10-COMMON/version/CompileInfo/mmpnmakeError_r.log

cd ..
cd ..
