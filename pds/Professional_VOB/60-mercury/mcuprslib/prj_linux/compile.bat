#!/bin/sh
	
make -f makefile_82xx_d clean

make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/mcuprs_linux_ppc_82xx_d.txt
cp ./libmcuprs.a ../../../10-common/lib/debug/linux_ppc_82xx/

echo makefile_82xx_d completed!


make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/mcuprs_linux_ppc_82xx_r.txt
cp ./libmcuprs.a ../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_82xx_r completed!

make -f makefile_85xx_d clean

make -f makefile_85xx_d 1> ../../../10-common/version/compileinfo/mcuprs_linux_ppc_85xx_d.txt
cp ./libmcuprs.a ../../../10-common/lib/debug/linux_ppc_85xx/

echo makefile_85xx_d completed!


make -f makefile_85xx_r clean

make -f makefile_85xx_r 1> ../../../10-common/version/compileinfo/mcuprs_linux_ppc_85xx_r.txt
cp ./libmcuprs.a ../../../10-common/lib/release/linux_ppc_85xx/

echo makefile_85xx_r completed!

make -f makefile_rh_d clean

make -f makefile_rh_d 1> ../../../10-common/version/compileinfo/mcuprs_linux_g++_rh_d.txt
cp ./libprs8ke_lib.a ../../../10-common/lib/debug/linux/

echo makefile_rh_d completed!


make -f makefile_rh_r clean

make -f makefile_rh_r 1> ../../../10-common/version/compileinfo/mcuprs_linux_g++_rh_r.txt
cp ./libprs8ke_lib.a ../../../10-common/lib/release/linux/

echo makefile_rh_r completed!


make -f makefile_rh8000h_d clean

make -f makefile_rh8000h_d 1> ../../../10-common/version/compileinfo/mcuprs_linux_g++_rh8000h_d.txt
cp ./libprs8kh_lib.a ../../../10-common/lib/debug/linux/8khlib

echo makefile_rh8000h_d completed!


make -f makefile_rh8000h_r clean

make -f makefile_rh8000h_r 1> ../../../10-common/version/compileinfo/mcuprs_linux_g++_rh8000h_r.txt
cp ./libprs8kh_lib.a ../../../10-common/lib/release/linux/8khlib

echo makefile_rh8000h_r completed!


make -f makefile_rh8000i_r clean

make -f makefile_rh8000i_r 1> ../../../10-common/version/compileinfo/mcuprs_linux_g++_rh8000i_r.txt
cp ./libprs8ki_lib.a ../../../10-common/lib/release/linux/8kilib

echo makefile_rh8000i_r completed!



cd ..
