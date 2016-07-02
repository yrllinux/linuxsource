#!/bin/sh

make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/detectee_linux_ppc_82xx_r.txt
cp ./detectee ../../../10-common/version/release/linux_ppc_82xx/

echo makefile_82xx_r completed

make -f makefile_82xx_linux12_r clean

make -f makefile_82xx_linux12_r 1> ../../../10-common/version/compileinfo/detectee_linux12_ppc_82xx_r.txt
cp ./detectee12 ../../../10-common/version/release/linux_ppc_82xx/

echo makefile_82xx_linux12_r completed

make -f makefile_85xx_linux12_r clean

make -f makefile_85xx_linux12_r 1> ../../../10-common/version/compileinfo/detectee_linux12_ppc_85xx_r.txt
cp ./detectee12 ../../../10-common/version/release/linux_ppc_85xx/

echo makefile_85xx_linux12_r completed

make -f makefile_dm816x_r clean
make -f makefile_dm816x_r 1> ../../../10-common/version/compileinfo/detectee_linux12_ppc_816x_r.txt
cp ./detectee12 ../../../10-common/version/release/linux_arm_dm81xx

echo makefile_dm816x_r completed

make -f makefile_8377_r clean
make -f makefile_8377_r 1> ../../../10-common/version/compileinfo/detectee_linux_8377_r.txt
cp ./detectee12 ../../../10-common/version/release/linux_ppc_8377_2637
echo makefile_8377_r completed!

make -f makefile_rh_r clean
make -f makefile_rh_r 1> ../../../10-common/version/compileinfo/detectee_linux_r.txt
cp ./detectee ../../../10-common/version/release/linux
echo makefile_rh_r completed!





