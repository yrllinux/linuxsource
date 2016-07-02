@rem cd proj8260

@rem set PRJ_DIR=.\

@rem @echo.
@rem @echo      ==============================================
@rem @echo      =       make 8260 debug BasicBoardAgent.a    =
@rem @echo      ==============================================
@rem set BUILD_SPEC=BasicBoardAgentDebug
@rem make clean
@rem make -e -S archive
@rem make clean

@rem @echo.
@rem @echo      ==============================================
@rem @echo      =       make 8260 release BasicBoardAgent.a  =
@rem @echo      ==============================================
@rem set BUILD_SPEC=BasicBoardAgentRelease
@rem make clean
@rem make -e -S archive
@rem make clean

@rem set BUILD_SPEC=
@rem set PRJ_DIR=

@rem cd ..

cd prj_win32

@echo.
@echo      ==============================================
@echo      =  making BasicBoardAgent - win32 debug      =
@echo      ==============================================
nmake /NOLOGO /A /B /f basicboardagent.mak CFG="BasicBoardAgent - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f basicboardagent.mak CFG="BasicBoardAgent - Win32 Debug" /x nmakeError.log

@echo.
@echo      ==============================================
@echo      =  making BasicBoardAgent - win32 release    =
@echo      ==============================================
nmake /NOLOGO /A /B /f basicboardagent.mak CFG="BasicBoardAgent - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f basicboardagent.mak CFG="BasicBoardAgent - Win32 Release" /x nmakeError.log


cd ..