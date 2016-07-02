#!/bin/sh
	
make -f makefile_82xx_d clean

make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/commonlib_linux_ppc_82xx_d.txt
cp ./libcommonlib.a ../../../10-common/lib/debug/linux_ppc_82xx/

echo makefile_82xx_d completed!


make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/commonlib_linux_ppc_82xx_r.txt
cp ./libcommonlib.a ../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_82xx_r completed!

make -f makefile_85xx_d clean

make -f makefile_85xx_d 1> ../../../10-common/version/compileinfo/commonlib_linux_ppc_85xx_d.txt
cp ./libcommonlib.a ../../../10-common/lib/debug/linux_ppc_85xx/

echo makefile_85xx_d completed!


make -f makefile_85xx_r clean

make -f makefile_85xx_r 1> ../../../10-common/version/compileinfo/commonlib_linux_ppc_85xx_r.txt
cp ./libcommonlib.a ../../../10-common/lib/release/linux_ppc_85xx/

echo makefile_85xx_r completed!

make -f makefile_rh_d clean

make -f makefile_rh_d 1> ../../../10-common/version/compileinfo/commonlib_linux_g++_rh_d.txt
cp ./libcommonlib.a ../../../10-common/lib/debug/linux/

echo makefile_rh_d completed!


make -f makefile_rh_r clean

make -f makefile_rh_r 1> ../../../10-common/version/compileinfo/commonlib_linux_g++_rh_r.txt
cp ./libcommonlib.a ../../../10-common/lib/release/linux/

echo makefile_rh_r completed!

make -f makefile_dm816x_d clean

make -f makefile_dm816x_d 1> ../../../10-common/version/compileinfo/commonlib_linux_g++_816x_d.txt
cp ./libcommonlib.a ../../../10-common/lib/debug/linux_arm_dm81xx/

echo makefile_dm816x_d completed!


make -f makefile_dm816x_r clean

make -f makefile_dm816x_r 1> ../../../10-common/version/compileinfo/commonlib_linux_g++_816x_r.txt
cp ./libcommonlib.a ../../../10-common/lib/release/linux_arm_dm81xx/

echo makefile_dm816x_r completed!

cd ..
