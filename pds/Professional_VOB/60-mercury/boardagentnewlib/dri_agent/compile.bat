@rem cd proj8260

@rem set PRJ_DIR=.\

@rem @echo.
@rem @echo      ==============================================
@rem @echo      =       make 8260 debug DriAgent.a           =
@rem @echo      ==============================================
@rem set BUILD_SPEC=DriAgentDebug
@rem make clean
@rem make -e -S archive
@rem make clean

@rem @echo.
@rem @echo      ==============================================
@rem @echo      =       make 8260 release DriAgent.a         =
@rem @echo      ==============================================
@rem set BUILD_SPEC=DriAgentRelease
@rem make clean
@rem make -e -S archive
@rem make clean

@rem set BUILD_SPEC=
@rem set PRJ_DIR=

@rem cd ..


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