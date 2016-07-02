#!/bin/sh
	
make -f makefile_82xx_d clean

make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/satmodempxy_linux_ppc_82xx_d.txt
cp ./libsatmodempxy.a ../../../10-common/lib/debug/linux_ppc_82xx/

echo makefile_82xx_d completed!


make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/satmodempxy_linux_ppc_82xx_r.txt
cp ./libsatmodempxy.a ../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_82xx_r completed!


cd ..
