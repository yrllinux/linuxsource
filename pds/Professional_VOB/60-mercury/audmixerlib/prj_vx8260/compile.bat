
call D:\tornado\host\x86-win32\bin\torvars.bat


set PRJ_DIR=.\
@echo.
@echo     ==========================================
@echo     =    make AudMixer MPC8260 debug lib     =
@echo     ==========================================
set BUILD_SPEC=debug
make  clean
make  -e archive > ./../../../10-COMMON/version/CompileInfo/AudMixer_8260_debug.txt
make  clean
@echo.
@echo     ===========================================
@echo     =    make AudMixer MPC8260 release lib    =
@echo     ===========================================
set BUILD_SPEC=release
make clean
make -e archive > ./../../../10-COMMON/version/CompileInfo/AudMixer_8260_release.txt
make clean


set BUILD_SPEC=
