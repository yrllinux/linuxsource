#!/bin/sh

make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/eapu_linux_ppc_82xx_r.txt

echo makefile_82xx_r completed!

cd ..
