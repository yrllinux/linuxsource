@echo off

rem call D:\tornado\host\x86-win32\bin\torvars.bat

set PRJ_DIR=.\

echo ***********************  make 8260 debug mtadp.a **************************

set BUILD_SPEC=MtAdpLibDebug
make clean
make  -e archive

echo ***********************  make 8260 release mtadp.a **************************

set BUILD_SPEC=MtAdpLibRelease
make clean
make -e archive

rem echo ***********************  make 8260 debug_8010 mtadp.a **************************

rem set BUILD_SPEC=MtAdp8000BLibDebug
rem make clean
rem make  -e archive

rem echo ***********************  make 8260 release_8010 mtadp.a **************************

rem set BUILD_SPEC=MtAdp8000BLibRelease
rem make clean
rem make -e archive

set BUILD_SPEC=

cd ..

echo on