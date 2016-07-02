#!/bin/sh
	
make -f makefile_dm816x_d clean

make -f makefile_dm816x_d 1> ../../../10-common/version/compileinfo/hdu2lib_linux_816x_d.txt
cp ./libhdu2lib.a ../../../10-common/lib/debug/linux_arm_dm81xx/

echo makefile_dm816x_d completed!

make -f makefile_dm816x_r clean

make -f makefile_dm816x_r 1> ../../../10-common/version/compileinfo/hdu2lib_linux_816x_r.txt
cp ./libhdu2lib.a ../../../10-common/lib/release/linux_arm_dm81xx/

echo makefile_dm816x_r completed!
cd ..