#!/bin/sh

make -f makefile_82xx_linux12_r clean

make -f makefile_82xx_linux12_r 1> ../../../10-common/version/compileinfo/mtaccesscri2_v6_linux12_ppc_82xx_r.txt
cp ./mtaccesscri612 ../../../10-common/version/release/linux_ppc_82xx/mtaccesscri612

echo makefile_82xx_linux12_r completed


