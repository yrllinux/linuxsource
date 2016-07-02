#!/bin/sh

make -f makefile_82xx_d clean

make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/isslavesys_linux_ppc_82xx_d.txt
cp -f ./isslavesys ../../../10-common/debug/release/os_linux/is3-mpc8313/

echo makefile_82xx_d completed

make -f makefile_82xx_r clean

make -f makefile_82xx_r clean all 1> ../../../10-common/version/compileinfo/isslavesys_linux_ppc_82xx_r.txt
cp -f ./isslavesys ../../../10-common/version/release/os_linux/is3-mpc8313/

echo makefile_82xx_r completed