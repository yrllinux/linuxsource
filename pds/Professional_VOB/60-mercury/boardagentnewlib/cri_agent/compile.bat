@rem cd proj8260

@rem set PRJ_DIR=.\

@rem @echo.
@rem @echo      ==============================================
@rem @echo      =       make 8260 debug CriAgent.a           =
@rem @echo      ==============================================
@rem set BUILD_SPEC=CriAgentDebug
@rem make clean
@rem make -e -S archive
@rem make clean

@rem @echo.
@rem @echo      ==============================================
@rem @echo      =       make 8260 release CriAgent.a         =
@rem @echo      ==============================================
@rem set BUILD_SPEC=CriAgentRelease
@rem make clean
@rem make -e -S archive
@rem make clean

@rem set BUILD_SPEC=
@rem set PRJ_DIR=

@rem cd ..
@rem cd conf
@rem copy brdcfg.ini ../../../../10-common/version/config

@rem cd ..
@rem cd ..


cd project

@echo.
@echo      ==============================================
@echo      =  making CriAgent - debug   =
@echo      ==============================================
nmake /NOLOGO /A /B /f criagent.mak CFG="CriAgent - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f criagent.mak CFG="CriAgent - Win32 Debug" /x nmakeError.log

@echo.
@echo      ===============================================
@echo      =  making CriAgent - release  =
@echo      ===============================================
nmake /NOLOGO /A /B /f criagent.mak CFG="CriAgent - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f criagent.mak CFG="CriAgent - Win32 Release" /x nmakeError.log

cd ..