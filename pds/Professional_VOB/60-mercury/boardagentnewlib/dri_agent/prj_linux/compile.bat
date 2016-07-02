#!/bin/sh

make -f makefile_82xx_d clean

make -f makefile_82xx_d 1> ../../../../10-common/version/compileinfo/driagent_linux_ppc_82xx_d.txt
cp ./libdriagent.a ../../../../10-common/lib/debug/linux_ppc_82xx/

echo makefile_82xx_d completed

make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../../10-common/version/compileinfo/driagent_linux_ppc_82xx_r.txt
cp ./libdriagent.a ../../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_82xx_r completed


make -f makefile_85xx_d clean

make -f makefile_85xx_d 1> ../../../../10-common/version/compileinfo/driagent_linux_ppc_85xx_d.txt
cp ./libdriagent.a ../../../../10-common/lib/debug/linux_ppc_85xx/

echo makefile_85xx_d completed

make -f makefile_85xx_r clean

make -f makefile_85xx_r 1> ../../../../10-common/version/compileinfo/driagent_linux_ppc_85xx_r.txt
cp ./libdriagent.a ../../../../10-common/lib/release/linux_ppc_85xx/

echo makefile_85xx_r completed

echo make redhat 
make -f makefile_rd_d clean
make -f makefile_rd_d 1> ../../../../10-common/version/compileinfo/driagent_linux__d.txt
cp ./libdriagent.a ../../../../10-common/lib/debug/linux/

echo make redhat

make -f makefile_rd_r clean

make -f makefile_rd_r 1> ../../../../10-common/version/compileinfo/driagent_linux_r.txt
cp ./libdriagent.a ../../../../10-common/lib/release/linux/

echo make ok

make -f makefile_82xx_linux12_d clean

make -f makefile_82xx_linux12_d 1> ../../../../10-common/version/compileinfo/driagent_linux12_ppc_82xx_d.txt
cp ./libdriagent12.a ../../../../10-common/lib/debug/linux_ppc_82xx/

echo makefile_82xx_linux12_d completed

make -f makefile_82xx_linux12_r clean

make -f makefile_82xx_linux12_r 1> ../../../../10-common/version/compileinfo/driagent_linux12_ppc_82xx_r.txt
cp ./libdriagent12.a ../../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_82xx_linux12_r completed