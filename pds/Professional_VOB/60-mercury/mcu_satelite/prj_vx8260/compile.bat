call D:\tornado\host\x86-win32\bin\torvars.bat
set PRJ_DIR=.\

@rem echo.
@rem echo      ==============================================
@rem @echo      =         make 8260 debug mcu.out/.z         =
@rem @echo      ==============================================

@rem set BUILD_SPEC=PPCEC603gnu
@rem make  clean
@rem make  -e -S mcu.out
@rem copy /Y mcu.out   ..\..\..\10-common\version\debug\8260\mcu.out
@rem deflate <mcu.out> ..\..\..\10-common\version\debug\8260\mcu.z
@rem del mcu.out
@rem make  clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release mcu.out/.z         =
@echo      ==============================================

set BUILD_SPEC=release
make  clean
make  -e -S mcu.out
copy /Y mcu.out   ..\..\..\10-common\version\release\8260\mcu.out
deflate <mcu.out> ..\..\..\10-common\version\release\8260\mcu.z
del mcu.out
make  clean

@rem @echo.
@rem @echo      ==============================================
@rem @echo      =      make 8260  debug mcu_8010.out/.z      =
@rem @echo      ==============================================

@rem set BUILD_SPEC=mcu8010dbg
@rem make  clean
@rem make  -e -S mcu.out
@rem rename mcu.out mcu_8010.out
@rem copy /Y mcu_8010.out   ..\..\..\10-common\version\debug\8260\mcu_8010.out
@rem deflate <mcu_8010.out> ..\..\..\10-common\version\debug\8260\mcu_8010.z
@rem del mcu_8010.out
@rem make  clean

@echo.
@echo      ==============================================
@echo      =       make 8260 release mcu_8010.out/.z    =
@echo      ==============================================

set BUILD_SPEC=mcu8010rls
make  clean
make  -e -S mcu.out
rename mcu.out mcu_8010.out
copy /Y mcu_8010.out   ..\..\..\10-common\version\release\8260\mcu_8010.out
deflate <mcu_8010.out> ..\..\..\10-common\version\release\8260\mcu_8010.z
del mcu_8010.out
make  clean

@echo KDV8000C 不再支持 VxWorks 操作系统
@REM @echo.
@REM @echo      ==============================================
@REM @echo      =      make 8260  debug mcu_8000c.out/.z      =
@REM @echo      ==============================================

@REM set BUILD_SPEC=mcu8000c_d
@REM make  clean
@REM make  -e -S mcu.out
@REM rename mcu.out mcu_8000c.out
@REM copy /Y mcu_8000c.out   ..\..\..\10-common\version\debug\8260\mcu_8000c.out
@REM deflate <mcu_8000c.out> ..\..\..\10-common\version\debug\8260\mcu_8000c.z
@REM del mcu_8000c.out
@REM make  clean

@REM @echo.
@REM @echo      ==============================================
@REM @echo      =       make 8260 release mcu_8000c.out/.z    =
@REM @echo      ==============================================

@REM set BUILD_SPEC=mcu8000c_r
@REM make  clean
@REM make  -e -S mcu.out
@REM rename mcu.out mcu_8000c.out
@REM copy /Y mcu_8000c.out   ..\..\..\10-common\version\release\8260\mcu_8000c.out
@REM deflate <mcu_8000c.out> ..\..\..\10-common\version\release\8260\mcu_8000c.z
@REM del mcu_8000c.out
@REM make  clean

@set BUILD_SPEC=

cd ..\