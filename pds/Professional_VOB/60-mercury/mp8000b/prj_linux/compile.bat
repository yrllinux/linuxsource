#!/bin/sh

##make -f makefile_82xx_d clean

##make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/mp8000b_linux_ppc_82xx_d.txt
##cp ./mp8000b ../../../10-common/version/debug/linux_ppc_82xx/

##echo makefile_82xx_d completed!

make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/mp8000b_linux_ppc_82xx_r.txt
cp ./mp8000b ../../../10-common/version/release/linux_ppc_82xx/

echo makefile_82xx_r completed

##make -f makefile_85xx_d clean

##make -f makefile_85xx_d 1> ../../../10-common/version/compileinfo/mp8000b_linux_ppc_85xx_d.txt
##cp ./mp8000b ../../../10-common/version/debug/linux_ppc_85xx/

##echo makefile_85xx_d completed!

make -f makefile_85xx_r clean

make -f makefile_85xx_r 1> ../../../10-common/version/compileinfo/mp8000b_linux_ppc_85xx_r.txt
cp ./mp8000b ../../../10-common/version/release/linux_ppc_85xx/

echo makefile_85xx_r completed

##make -f makefile_rh_d clean

##make -f makefile_rh_d 1> ../../../10-common/version/compileinfo/mp8000b_linux_rh_d.txt
##cp ./mp8000b ../../../10-common/version/debug/linux/

##echo makefile_rh_d completed!

##make -f makefile_rh_r clean

##make -f makefile_rh_r 1> ../../../10-common/version/compileinfo/mp8000b_linux_rh_r.txt
##cp ./mp8000b ../../../10-common/version/release/linux/

##echo makefile_rh_r completed
