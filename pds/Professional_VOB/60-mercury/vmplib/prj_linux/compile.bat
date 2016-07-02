#!/bin/sh
	
make -f makefile_82xx_d clean

make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/vmp_linux_ppc_82xx_d.txt
cp ./libvmp.a ../../../10-common/lib/debug/linux_ppc_82xx/

echo makefile_82xx_d completed!


make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/vmp_linux_ppc_82xx_r.txt
cp ./libvmp.a ../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_82xx_r completed!

make -f makefile_85xx_d clean

make -f makefile_85xx_d 1> ../../../10-common/version/compileinfo/vmp_linux_ppc_85xx_d.txt
cp ./libvmp.a ../../../10-common/lib/debug/linux_ppc_85xx/

echo makefile_85xx_d completed!


make -f makefile_85xx_r clean

make -f makefile_85xx_r 1> ../../../10-common/version/compileinfo/vmp_linux_ppc_85xx_r.txt
cp ./libvmp.a ../../../10-common/lib/release/linux_ppc_85xx/

echo makefile_85xx_r completed!

echo no make -f Makefile_rh_d clean

echo no make -f Makefile_rh_d 1> ../../../10-common/version/compileinfo/vmp_linux_g++_rh_d.txt
echo no cp ./libvmp.a ../../../10-common/lib/debug/linux/

echo Makefile_rh_d completed!


echo no make -f Makefile_rh_r clean

echo no make -f Makefile_rh_r 1> ../../../10-common/version/compileinfo/vmp_linux_g++_rh_r.txt
echo no cp ./libvmp.a ../../../10-common/lib/release/linux/

echo Makefile_rh_r completed!

cd ..
