call D:\tornado\host\x86-win32\bin\torvars.bat
set PRJ_DIR=.\

set BUILD_SPEC=VmpDebug
make clean
make -e archive
make clean

set BUILD_SPEC=VmpRelease
make clean
make -e archive
make clean

set BUILD_SPEC=
cd ..
