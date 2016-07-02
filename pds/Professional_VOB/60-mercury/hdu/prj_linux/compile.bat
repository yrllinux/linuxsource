#!/bin/sh

make -f makefile_82xx_d clean

make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/hdu_linux_ppc_82xx_d.txt
cp ./hdu ../../../10-common/version/debug/os_linux/hdu/hdu_a/hdu
cp ./hdu ../../../10-common/version/debug/os_linux/hdu/hdu_b/hdu

echo makefile_82xx_d completed!

make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/mmp_linux_ppc_82xx_r.txt
cp ./hdu ../../../10-common/version/release/os_linux/hdu/hdu_a/hdu
cp ./hdu ../../../10-common/version/release/os_linux/hdu/hdu_b/hdu
echo makefile_82xx_r completed

make -f makefile_82xx_dvi_r clean
make -f makefile_82xx_dvi_r 1> ../../../10-common/version/compileinfo/hdu_linux_ppc_82xx_dvi_r.txt
cp ./hdudvi ../../../10-common/version/release/os_linux/hdu/hdu_d/hdu
echo makefile_82xx_dvi_r completed


##make -f makefile_85xx_d clean

##make -f makefile_85xx_d 1> ../../../10-common/version/compileinfo/mmp_linux_ppc_85xx_d.txt
##cp ./hdu ../../../10-common/version/debug/linux_ppc_85xx/kdvhdu

##echo makefile_85xx_d completed!

##make -f makefile_85xx_r clean

##make -f makefile_85xx_r 1> ../../../10-common/version/compileinfo/mmp_linux_ppc_85xx_r.txt
##cp ./hdu ../../../10-common/version/release/linux_ppc_85xx/kdvhdu

##echo makefile_85xx_r completed