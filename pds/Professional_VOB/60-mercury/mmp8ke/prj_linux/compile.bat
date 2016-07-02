#!/bin/sh


make -f makefile_d clean

make -f makefile_d 1> ../../../10-common/version/compileinfo/mmp_linux_g++_rh_d.txt 2>&1
cp ./mmp_8000e ../../../10-common/version/debug/linux

echo makefile_d completed


make -f makefile clean

make -f makefile 1> ../../../10-common/version/compileinfo/mmp_linux_g++_rh_r.txt 2>&1
cp ./mmp_8000e ../../../10-common/version/release/linux

echo makefile completed


make -f makefile_rh8000h_d clean

make -f makefile_rh8000h_d 1> ../../../10-common/version/compileinfo/mmp_linux_g++_rh8000h_d.txt 2>&1
cp ./mmp_8000h ../../../10-common/version/debug/linux

echo makefile_rh8000h_d completed


make -f makefile_rh8000h_r clean

make -f makefile_rh8000h_r 1> ../../../10-common/version/compileinfo/mmp_linux_g++_rh8000h_r.txt 2>&1
cp ./mmp_8000h ../../../10-common/version/release/linux

echo makefile_rh8000h_r completed

make -f makefile_rh8000i_r clean

make -f makefile_rh8000i_r 1> ../../../10-common/version/compileinfo/mmp_linux_g++_rh8000i_r.txt 2>&1
cp ./mmp_8000i ../../../10-common/version/release/linux

echo makefile_rh8000i_r completed

cd ..