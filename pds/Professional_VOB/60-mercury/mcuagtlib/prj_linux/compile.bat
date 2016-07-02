#!/bin/sh

make -f makefile_82xx_d clean

make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/mcuagent_linux_ppc_82xx_d.txt
cp ./libmcuagent.a ../../../10-common/lib/debug/linux_ppc_82xx/

echo makefile_82xx_d completed

make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/mcuagent_linux_ppc_82xx_r.txt
cp ./libmcuagent.a ../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_82xx_r completed


make -f makefile_85xx_d clean

make -f makefile_85xx_d 1> ../../../10-common/version/compileinfo/mcuagent_linux_ppc_85xx_d.txt
cp ./libmcuagent.a ../../../10-common/lib/debug/linux_ppc_85xx/

echo makefile_85xx_d completed

make -f makefile_85xx_r clean

make -f makefile_85xx_r 1> ../../../10-common/version/compileinfo/mcuagent_linux_ppc_85xx_r.txt
cp ./libmcuagent.a ../../../10-common/lib/release/linux_ppc_85xx/

echo makefile_85xx_r completed

#make -f makefile_rh_d clean

#make -f makefile_rd_d 1> ../../../10-common/version/compileinfo/mcuagent_linux_rh_d.txt
#cp libmcuagent8ke ../../../10-common/lib/debug/linux/

#echo makefile_rh_d completed

make -f makefile_rh_r clean

make -f makefile_rh_r 1> ../../../10-common/version/compileinfo/mcuagent_linux_rh_r.txt
cp ./libmcuagent8ke.a ../../../10-common/lib/release/linux/

echo makefile_rh_r completed


#make -f makefile_rh8000h_d clean

#make -f makefile_rh8000h_d 1> ../../../10-common/version/compileinfo/mcuagent_linux_rh8000h_d.txt
#cp ./libmcuagent8kh.a ../../../10-common/lib/debug/linux/8khlib

#echo makefile_rh8000h_d completed

make -f makefile_rh8000h_r clean

make -f makefile_rh8000h_r 1> ../../../10-common/version/compileinfo/mcuagent_linux_rh8000h_r.txt
cp ./libmcuagent8kh.a ../../../10-common/lib/release/linux/8khlib

echo makefile_rh8000h_r completed


make -f makefile_rh8000i_r clean

make -f makefile_rh8000i_r 1> ../../../10-common/version/compileinfo/mcuagent_linux_rh8000i_r.txt
cp ./libmcuagent8ki.a ../../../10-common/lib/release/linux/8kilib

echo makefile_rh8000i_r completed


make -f makefile_82xx_8000b_d clean

make -f makefile_82xx_8000b_d 1> ../../../10-common/version/compileinfo/mcuagent_linux_ppc_82xx8000b_d.txt
cp ./libmcuagentmt.a ../../../10-common/lib/debug/linux_ppc_82xx/

echo makefile_82xx_8000b_d completed

make -f makefile_82xx_8000b_r clean

make -f makefile_82xx_8000b_r 1> ../../../10-common/version/compileinfo/mcuagent_linux_ppc_82xx8000b_r.txt
cp ./libmcuagentmt.a ../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_82xx_8000b_r completed

make -f makefile_85xx_8000b_d clean

make -f makefile_85xx_8000b_d 1> ../../../10-common/version/compileinfo/mcuagent_linux_ppc_85xx8000b_d.txt
cp ./libmcuagentmt.a ../../../10-common/lib/debug/linux_ppc_85xx/

echo makefile_85xx_8000b_d completed

make -f makefile_85xx_8000b_r clean

make -f makefile_85xx_8000b_r 1> ../../../10-common/version/compileinfo/mcuagent_linux_ppc_85xx8000b_r.txt
cp ./libmcuagentmt.a ../../../10-common/lib/release/linux_ppc_85xx/

echo makefile_85xx_8000b_r completed

make -f makefile_82xx_linux12_d clean

make -f makefile_82xx_linux12_d 1> ../../../10-common/version/compileinfo/mcuagent_linux12_ppc_82xx_d.txt
cp ./libmcuagent12.a ../../../10-common/lib/debug/linux_ppc_82xx/

echo makefile_82xx_linux12_d completed

make -f makefile_82xx_linux12_r clean

make -f makefile_82xx_linux12_r 1> ../../../10-common/version/compileinfo/mcuagent_linux12_ppc_82xx_r.txt
cp ./libmcuagent12.a ../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_82xx_linux12_r completed
