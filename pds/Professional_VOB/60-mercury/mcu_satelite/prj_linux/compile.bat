#!/bin/sh


##make -f makefile_82xx_d clean

##make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/mcu_linux_ppc_82xx_d.txt
##cp ./mcu ../../../10-common/version/debug/linux_ppc_82xx/

##echo makefile_82xx_d completed!

make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/mcu_linux_ppc_82xx_r.txt
cp ./mcu ../../../10-common/version/release/linux_ppc_82xx/

echo makefile_82xx_r completed

##make -f makefile_82xx_8000b_d clean

##make -f makefile_82xx_8000b_d 1> ../../../10-common/version/compileinfo/mcu_linux_ppc_82xx_8000b_d.txt
##cp ./mcu ../../../10-common/version/debug/linux_ppc_82xx/mcu_8000b

##echo makefile_82xx_8000b_d completed!

make -f makefile_82xx_8000b_r clean

make -f makefile_82xx_8000b_r 1> ../../../10-common/version/compileinfo/mcu_linux_ppc_82xx_8000b_r.txt
cp ./mcu ../../../10-common/version/release/linux_ppc_82xx/mcu_8000b

echo makefile_82xx_8000b_r completed

##make -f makefile_82xx_8000c_d clean

##make -f makefile_82xx_8000c_d 1> ../../../10-common/version/compileinfo/mcu_linux_ppc_82xx_8000c_d.txt
##cp ./mcu ../../../10-common/version/debug/linux_ppc_82xx/mcu_8000c

##echo makefile_82xx_8000c_d completed

##make -f makefile_82xx_8000c_r clean

##make -f makefile_82xx_8000c_r 1> ../../../10-common/version/compileinfo/mcu_linux_ppc_82xx_8000c_r.txt
##cp ./mcu ../../../10-common/version/release/linux_ppc_82xx/mcu_8000c

echo makefile_82xx_8000c_r completed