@rem cd proj8260

@rem set PRJ_DIR=.\

@rem @echo.
@rem @echo      ==============================================
@rem @echo      =       make 8260 debug ImtAgent.a           =
@rem @echo      ==============================================
@rem set BUILD_SPEC=ImtAgentDebug
@rem make clean
@rem make -e -S archive > ../../../../10-COMMON/version/CompileInfo/imtagent_d.txt
@rem make clean

@rem @echo.
@rem @echo      ==============================================
@rem @echo      =       make 8260 release ImtAgent.a         =
@rem @echo      ==============================================
@rem set BUILD_SPEC=ImtAgentRelease
@rem make clean
@rem make -e -S archive > ../../../../10-COMMON/version/CompileInfo/imtagent_r.txt
@rem make clean

@rem set BUILD_SPEC=
@rem set PRJ_DIR=

@rem cd ..
@rem cd ..

cd project

@echo.
@echo      ==============================================
@echo      =  making ImtAgent - debug   =
@echo      ==============================================
nmake /NOLOGO /A /B /f imtagent.mak CFG="ImtAgent - Win32 Debug" CLEAN
nmake /NOLOGO /A /B /f imtagent.mak CFG="ImtAgent - Win32 Debug" /x nmakeError.log

@echo.
@echo      ===============================================
@echo      =  making ImtAgent - release  =
@echo      ===============================================
nmake /NOLOGO /A /B /f Imtagent.mak CFG="ImtAgent - Win32 Release" CLEAN
nmake /NOLOGO /A /B /f Imtagent.mak CFG="ImtAgent - Win32 Release" /x nmakeError.log


cd ..