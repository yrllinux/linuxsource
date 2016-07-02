#!/bin/sh
	
make -f makefile_82xx_linux12_d clean

make -f makefile_82xx_linux12_d 1> ../../../10-common/version/compileinfo/mp6_linux12_ppc_82xx_d.txt
cp ./libmp612.a ../../../10-common/lib/debug/linux_ppc_82xx/

echo makefile_82xx_linux12_d completed!


make -f makefile_82xx_linux12_r clean

make -f makefile_82xx_linux12_r 1> ../../../10-common/version/compileinfo/mp6_linux12_ppc_82xx_r.txt
cp ./libmp612.a ../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_82xx_linux12_r completed!

make -f makefile_rh_d clean

make -f makefile_rh_d 1> ../../../10-common/version/compileinfo/mp6_linux_g++_rh_d.txt
cp ./libmp8ke6.a ../../../10-common/lib/debug/linux/

echo makefile_rh_d completed!


make -f makefile_rh_r clean

make -f makefile_rh_r 1> ../../../10-common/version/compileinfo/mp6_linux_g++_rh_r.txt
cp ./libmp8ke6.a ../../../10-common/lib/release/linux/

echo makefile_rh_r completed!

cd ..
