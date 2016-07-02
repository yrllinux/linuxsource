#!/bin/sh



make -f makefile_85xx_linux12_r clean

make -f makefile_85xx_linux12_r 1> ../../../10-common/version/compileinfo/mtaccesscri_linux12_ppc_85xx_r.txt
cp ./mtaccessmtadp ../../../10-common/version/release/linux_ppc_85xx/

echo makefile_85xx_linux12_r completed



