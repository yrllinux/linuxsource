#!/bin/sh

make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/mpu_linux_ppc_82xx_r.txt

echo makefile_82xx_r completed!

make -f makefile_82xx_d clean

make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/mpu_linux_ppc_82xx_d.txt

echo makefile_82xx_d completed!



make -f makefile_82xx_b_r clean

make -f makefile_82xx_b_r 1> ../../../10-common/version/compileinfo/mpu_linux_ppc_82xx_b_r.txt

make -f makefile_82xx_b_d clean

make -f makefile_82xx_b_d 1> ../../../10-common/version/compileinfo/mpu_linux_ppc_82xx_b_d.txt


cd ..
