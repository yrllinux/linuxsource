#!/bin/sh

##make -f makefile_82xx_d clean

##make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/mtaccesscri_linux_ppc_82xx_d.txt
##cp ./mtaccesscri ../../../10-common/version/debug/linux_ppc_82xx/

##echo makefile_82xx_d completed!

make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/mtaccesscri_linux_ppc_82xx_r.txt
cp ./mtaccesscri ../../../10-common/version/release/linux_ppc_82xx/

echo makefile_82xx_r completed

#make -f makefile_85xx_d clean

##make -f makefile_85xx_d 1> ../../../10-common/version/compileinfo/mtaccesscri_linux_ppc_85xx_d.txt
##cp ./mtaccesscri ../../../10-common/version/debug/linux_ppc_85xx/

##echo makefile_85xx_d completed!

make -f makefile_85xx_r clean

make -f makefile_85xx_r 1> ../../../10-common/version/compileinfo/mtaccesscri_linux_ppc_85xx_r.txt
cp ./mtaccesscri ../../../10-common/version/release/linux_ppc_85xx/

echo makefile_85xx_r completed

##make -f makefile_82xx_hdi_r clean

##make -f makefile_82xx_hdi_r 1> ../../../10-common/version/compileinfo/kdvhdi.txt
##cp ./mtaccesscri ../../../10-common/version/release/linux_ppc_82xx/kdvhdi

##echo makefile_82xx_hdi_r completed


##make -f makefile_82xx_linux12_d clean

##make -f makefile_82xx_linux12_d 1> ../../../10-common/version/compileinfo/mtaccesscri_linux12_ppc_82xx_d.txt
##cp ./mtaccesscri12 ../../../10-common/version/debug/linux_ppc_82xx/

##echo makefile_82xx_linux12_d completed!

make -f makefile_82xx_linux12_r clean

make -f makefile_82xx_linux12_r 1> ../../../10-common/version/compileinfo/mtaccesscri_linux12_ppc_82xx_r.txt
cp ./mtaccesscri12 ../../../10-common/version/release/linux_ppc_82xx/

echo makefile_82xx_linux12_r completed

#make -f makefile_85xx_linux12_d clean

##make -f makefile_85xx_linux12_d 1> ../../../10-common/version/compileinfo/mtaccesscri_linux12_ppc_85xx_d.txt
##cp ./mtaccesscri12 ../../../10-common/version/debug/linux_ppc_85xx/

##echo makefile_85xx_linux12_d completed!

make -f makefile_85xx_linux12_r clean

make -f makefile_85xx_linux12_r 1> ../../../10-common/version/compileinfo/mtaccesscri_linux12_ppc_85xx_r.txt
cp ./mtaccesscri12 ../../../10-common/version/release/linux_ppc_85xx/

echo makefile_85xx_linux12_r completed


##make -f makefile_85xx_hdi_linux12_r clean
##make -f makefile_85xx_hdi_linux12_r > ../../../10-common/version/compileinfo/mtaccesscri_linux_ppc_85xx_hdi_linux12_r.txt 2>&1
##cp ./mtaccesscri12 ../../../10-common/version/release/linux_ppc_85xx/kdvhdi12

##echo makefile_85xx_hdi_linux12_r completed!


##make -f makefile_82xx_hdi_linux12_r clean
##make -f makefile_82xx_hdi_linux12_r > ../../../10-common/version/compileinfo/mtaccesscri_linux_ppc_82xx_hdi_linux12_r.txt 2>&1
##cp ./mtaccesscri12 ../../../10-common/version/release/linux_ppc_82xx/kdvhdi12

##echo makefile_82xx_hdi_linux12_r completed!

make -f makefile_82xx_linux12_is21_r clean
make -f makefile_82xx_linux12_is21_r > ../../../10-common/version/compileinfo/mtaccesscri_linux_ppc_82xx_linux12_is21_r.txt 2>&1
cp ./mtaccesscri12_is21 ../../../10-common/version/release/linux_ppc_82xx

echo makefile_82xx_linux12_is21_r completed!

#make -f makefile_82xx_linux12_is21_d clean
#make -f makefile_82xx_linux12_is21_d > ../../../10-common/version/compileinfo/mtaccesscri_linux_ppc_82xx_linux12_is21_d.txt 2>&1
#cp ./mtaccesscri12_is21 ../../../10-common/version/debug/linux_ppc_82xx

#echo makefile_82xx_linux12_is21_d completed!


