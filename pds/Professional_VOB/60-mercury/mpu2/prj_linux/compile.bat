#!/bin/sh

make -f makefile_dm816x_r clean
make -f makefile_dm816x_r 
cp ./mpu2 ../../../10-common/version/release/os_linux/mpu2/mpu2

echo makefile_dm816x_r completed