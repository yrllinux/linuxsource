#!/bin/sh

make -f makefile_82xx_d clean

make -f makefile_82xx_d 1> ../../../../10-common/version/compileinfo/basicboardagent_linux_ppc_82xx_d.txt
cp ./basicboardagent.a ../../../../10-common/lib/debug/linux_ppc_82xx/

echo makefile_82xx_d completed

make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../../10-common/version/compileinfo/basicboardagent_linux_ppc_82xx_r.txt
cp ./basicboardagent.a ../../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_82xx_r completed


make -f makefile_85xx_d clean

make -f makefile_85xx_d 1> ../../../../10-common/version/compileinfo/basicboardagent_linux_ppc_85xx_d.txt
cp ./basicboardagent.a ../../../../10-common/lib/debug/linux_ppc_85xx/

echo makefile_85xx_d completed

make -f makefile_85xx_r clean

make -f makefile_85xx_r 1> ../../../../10-common/version/compileinfo/basicboardagent_linux_ppc_85xx_r.txt
cp ./basicboardagent.a ../../../../10-common/lib/release/linux_ppc_85xx/

echo makefile_85xx_r completed


echo make redhat 
make -f makefile_rd_d clean
make -f makefile_rd_d 1> ../../../../10-common/version/compileinfo/basicboardagent_linux__d.txt
cp ./basicboardagent.a ../../../../10-common/lib/debug/linux/

echo make redhat

make -f makefile_rd_r clean

make -f makefile_rd_r 1> ../../../../10-common/version/compileinfo/basicboardagent_linux_r.txt
cp ./basicboardagent.a ../../../../10-common/lib/release/linux/

echo make linux12_d

make -f makefile_mthd_82xx_d clean
make -f makefile_mthd_82xx_d 
cp ./basicboardagent12.a ../../../../10-common/lib/debug/linux/

echo make linux12_r

make -f makefile_mthd_82xx_r clean
make -f makefile_mthd_82xx_r
cp ./basicboardagent12.a ../../../../10-common/lib/release/linux/


make -f makefile_85xx_linux12_d clean

make -f makefile_85xx_linux12_d 1> ../../../../10-common/version/compileinfo/basicboardagent_linux12_ppc_85xx_d.txt
cp ./basicboardagent12.a ../../../../10-common/lib/debug/linux_ppc_85xx/

echo makefile_85xx_linux12_d completed

make -f makefile_85xx_linux12_r clean

make -f makefile_85xx_linux12_r 1> ../../../../10-common/version/compileinfo/basicboardagent_linux12_ppc_85xx_r.txt
cp ./basicboardagent12.a ../../../../10-common/lib/release/linux_ppc_85xx/

echo makefile_85xx_linux12_r completed

make -f makefile_dm816x_d clean

make -f makefile_dm816x_d 1> ../../../../10-common/version/compileinfo/basicboardagent_dm816x_d.txt
cp ./basicboardagent12.a ../../../../10-common/lib/debug/linux_arm_dm81xx/

echo makefile_dm816x_d completed

make -f makefile_dm816x_r clean

make -f makefile_dm816x_r 1> ../../../../10-common/version/compileinfo/basicboardagent_dm816x_r.txt
cp ./basicboardagent12.a ../../../../10-common/lib/release/linux_arm_dm81xx/

echo makefile_dm816x_r completed


make -f makefile_8377_d clean
make -f makefile_8377_d 1> ../../../../10-common/version/compileinfo/basicboardagent_8377_d.txt
cp ./basicboardagent12.a ../../../../10-common/lib/debug/linux_ppc_8377_2637/
echo makefile_8377_d completed

make -f makefile_8377_r clean
make -f makefile_8377_r 1> ../../../../10-common/version/compileinfo/basicboardagent_8377_r.txt
cp ./basicboardagent12.a ../../../../10-common/lib/release/linux_ppc_8377_2637/
echo makefile_8377_r completed


echo make ok