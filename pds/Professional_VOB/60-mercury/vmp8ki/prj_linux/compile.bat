#!/bin/sh
	
make -f makefile_82xx_d clean

make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/vmp_linux_g++_rh_d.txt 2>&1
cp ./vmp_8000i ../../../10-common/version/debug/linux_ppc_82xx/
echo Makefile_82xx_d completed!

make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/vmp_linux_g++_rh_r.txt 2>&1
cp ./vmp_8000i ../../../10-common/version/release/linux_ppc_82xx/
echo Makefile_82xx_r completed!



cd ..
