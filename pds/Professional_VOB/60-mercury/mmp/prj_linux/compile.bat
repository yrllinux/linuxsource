#!/bin/sh

##make -f makefile_82xx_d clean

##make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/mmp_linux_ppc_82xx_d.txt
##cp ./mcueqp_mmp ../../../10-common/version/debug/linux_ppc_82xx/kdvvmp
##cp ./mcueqp_mmp ../../../10-common/version/debug/linux_ppc_82xx/kdvtvwall

##echo makefile_82xx_d completed!

make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/mmp_linux_ppc_82xx_r.txt
cp ./mcueqp_mmp ../../../10-common/version/release/linux_ppc_82xx/kdvvmp
cp ./mcueqp_mmp ../../../10-common/version/release/linux_ppc_82xx/kdvtvwall
echo makefile_82xx_r completed

##make -f makefile_85xx_d clean

##make -f makefile_85xx_d 1> ../../../10-common/version/compileinfo/mmp_linux_ppc_85xx_d.txt
##cp ./mcueqp_mmp ../../../10-common/version/debug/linux_ppc_85xx/kdvvmp

##echo makefile_85xx_d completed!

##make -f makefile_85xx_r clean

##make -f makefile_85xx_r 1> ../../../10-common/version/compileinfo/mmp_linux_ppc_85xx_r.txt
##cp ./mcueqp_mmp ../../../10-common/version/release/linux_ppc_85xx/kdvvmp

##echo makefile_85xx_r completed




##make -f makefile_apu_82xx_d clean

##make -f makefile_apu_82xx_d 1> ../../../10-common/version/compileinfo/apu_linux_ppc_82xx_d.txt
##cp ./mcueqp_apu ../../../10-common/version/debug/linux_ppc_82xx/kdvapu

##echo makefile_apu_82xx_d completed!

make -f makefile_apu_82xx_r clean

make -f makefile_apu_82xx_r 1> ../../../10-common/version/compileinfo/apu_linux_ppc_82xx_r.txt
cp ./mcueqp_apu ../../../10-common/version/release/linux_ppc_82xx/kdvapu

echo makefile_apu_82xx_r completed

##make -f makefile_apu_85xx_d clean

##make -f makefile_apu_85xx_d 1> ../../../10-common/version/compileinfo/apu_linux_ppc_85xx_d.txt
##cp ./mcueqp_apu ../../../10-common/version/debug/linux_ppc_85xx/kdvapu

##echo makefile_apu_85xx_d completed!

##make -f makefile_apu_85xx_r clean

##make -f makefile_apu_85xx_r 1> ../../../10-common/version/compileinfo/apu_linux_ppc_85xx_r.txt
##cp ./mcueqp_apu ../../../10-common/version/release/linux_ppc_85xx/kdvapu

##echo makefile_apu_85xx_r completed
