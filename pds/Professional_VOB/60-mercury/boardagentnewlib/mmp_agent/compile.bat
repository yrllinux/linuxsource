@rem cd proj8260

@rem set PRJ_DIR=.\

@rem @echo.
@rem @echo      ==============================================
@rem @echo      =       make 8260 debug MmpAgent.a           =
@rem @echo      ==============================================
@rem set BUILD_SPEC=MmpAgentDebug
@rem make clean
@rem make -e -S archive > ../../../../10-COMMON/version/CompileInfo/mmpagent_d.txt
@rem make clean

@rem @echo.
@rem @echo      ==============================================
@rem @echo      =       make 8260 release MmpAgent.a         =
@rem @echo      ==============================================
@rem set BUILD_SPEC=MmpAgentRelease
@rem make clean
@rem make -e -S archive > ../../../../10-COMMON/version/CompileInfo/mmpagent_r.txt
@rem make clean

@rem set BUILD_SPEC=
@rem set PRJ_DIR=

@rem cd ..

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
