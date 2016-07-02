#!/bin/sh

make -f makefile_82xx_r clean
make -f makefile_82xx_r 
cp ./mpu ../../../10-common/version/release/os_linux/mpu/mpu_a/mpu

echo makefile_82xx_r completed

make -f makefile_82xx_b_r clean
make -f makefile_82xx_b_r

cp ./mpu ../../../10-common/version/release/os_linux/mpu/mpu_b/mpu

echo makefile_82xx_b_r completed