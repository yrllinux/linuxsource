#!/bin/sh

make -f makefile_82xx_r clean
make -f makefile_82xx_r 
cp ./kdvvmphd ../../../10-common/version/release/os_linux/vpuhd

echo makefile_82xx_r completed
