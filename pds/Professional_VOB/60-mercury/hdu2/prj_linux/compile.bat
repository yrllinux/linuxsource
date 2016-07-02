#!/bin/sh

make -f makefile_dm816x_d clean

make -f makefile_dm816x_d 1> ../../../10-common/version/compileinfo/hdu2_linux_dm816x_d.txt
cp ./hdu2 ../../../10-common/version/debug/os_linux/hdu2/hdu2

echo makefile_dm816x_d completed!

make -f makefile_dm816x_r clean

make -f makefile_dm816x_r 1> ../../../10-common/version/compileinfo/hdu2_linux_dm816x_r.txt
cp ./hdu2 ../../../10-common/version/release/os_linux/hdu2/hdu2
echo makefile_dm816x_r completed
