cd proj8260

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

cd ..


cd project

@echo.
@echo      ==============================================
@echo      =  making DriAgent - debug   =
@echo      ==============================================
nmake /NOLOGO /A /B /f driagent.mak CFG="DriAgent - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f driagent.mak CFG="DriAgent - Win32 Debug" /x nmakeError.log

@echo.
@echo      ===============================================
@echo      =  making DriAgent - release  =
@echo      ===============================================
nmake /NOLOGO /A /B /f driagent.mak CFG="DriAgent - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f driagent.mak CFG="DriAgent - Win32 Release" /x nmakeError.log

cd ..
cd ..