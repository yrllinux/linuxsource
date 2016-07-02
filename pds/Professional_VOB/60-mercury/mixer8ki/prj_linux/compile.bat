#!/bin/sh


make -f makefile_82xx_8000i_d clean

make -f makefile_82xx_8000i_d 1> ../../../10-common/version/compileinfo/mixer8ki_linux_g++_rh_d.txt 2>&1
cp ./mixer_8ki ../../../10-common/version/debug/linux_ppc_82xx/

echo makefile_d completed


make -f makefile_82xx_8000i_r clean

make -f makefile_82xx_8000i_r 1> ../../../10-common/version/compileinfo/mixer8ki_linux_g++_rh_r.txt 2>&1
cp ./mixer_8ki ../../../10-common/version/release/linux_ppc_82xx/

echo makefile completed



cd ..