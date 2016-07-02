#!/bin/sh

make -f makefile_82xx_8000i_r clean

make -f makefile_82xx_8000i_r 1> ../../../10-common/version/compileinfo/prs_linux_g++_rh8000h_r.txt 2>&1
cp ./bas_8000i ../../../10-common/version/release/linux_ppc_82xx/


echo makefile_82xx_8000i_r completed


cd ..