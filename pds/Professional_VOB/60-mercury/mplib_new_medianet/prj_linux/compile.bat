#!/bin/sh
	
make -f makefile_82xx_d clean

make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/mp_linux_ppc_82xx_d.txt
cp ./libmp.a ../../../10-common/lib/debug/linux_ppc_82xx/

echo makefile_82xx_d completed!


make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/mp_linux_ppc_82xx_r.txt
cp ./libmp.a ../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_82xx_r completed!

make -f makefile_85xx_d clean

make -f makefile_85xx_d 1> ../../../10-common/version/compileinfo/mp_linux_ppc_855xx_d.txt
cp ./libmp.a ../../../10-common/lib/debug/linux_ppc_85xx/

echo makefile_85xx_d completed!


make -f makefile_85xx_r clean

make -f makefile_85xx_r 1> ../../../10-common/version/compileinfo/mp_linux_ppc_85xx_r.txt
cp ./libmp.a ../../../10-common/lib/release/linux_ppc_85xx/

echo makefile_85xx_r completed!


	
	

make -f makefile_mdsc_82xx_d clean

make -f makefile_mdsc_82xx_d 1> ../../../10-common/version/compileinfo/mp_mdsc_linux_ppc_82xx_d.txt
cp ./libmp_mdsc.a ../../../10-common/lib/debug/linux_ppc_82xx/

echo makefile_mdsc_82xx_d completed!


make -f makefile_mdsc_82xx_r clean

make -f makefile_mdsc_82xx_r 1> ../../../10-common/version/compileinfo/mp_mdec_linux_ppc_82xx_r.txt
cp ./libmp_mdsc.a ../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_mdsc_82xx_r completed!

make -f makefile_mdsc_85xx_d clean

make -f makefile_mdsc_85xx_d 1> ../../../10-common/version/compileinfo/mp_mdsc_linux_ppc_855xx_d.txt
cp ./libmp_mdsc.a ../../../10-common/lib/debug/linux_ppc_85xx/

echo makefile_mdsc_85xx_d completed!


make -f makefile_mdsc_85xx_r clean

make -f makefile_mdsc_85xx_r 1> ../../../10-common/version/compileinfo/mp_mdsc_linux_ppc_855xx_r.txt
cp ./libmp_mdsc.a ../../../10-common/lib/release/linux_ppc_85xx/

echo makefile_mdsc_85xx_r completed!


make -f makefile_rh_d clean

make -f makefile_rh_d
cp ./libmp.a ../../../10-common/lib/debug/linux/

echo makefile_rh_d completed!


make -f makefile_rh_r clean

make -f makefile_rh_r
cp ./libmp.a ../../../10-common/lib/release/linux/

echo makefile_rh_r completed!

cd ..
