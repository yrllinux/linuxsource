#!/bin/sh
	
##make -f makefile_82xx_d clean

##make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/h320mtadp_linux_ppc_82xx_d.txt


##echo no make makefile_82xx_d!


make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/h320mtadp_linux_ppc_82xx_r.txt
cp ./h320mtadp ../../../10-common/version/release/linux_ppc_82xx/
cp ./h320mtadp ../../../10-common/version/debug/linux_ppc_82xx/

echo makefile_82xx_r completed!

##make -f makefile_85xx_d clean

##make -f makefile_85xx_d 1> ../../../10-common/version/compileinfo/h320mtadp_linux_ppc_85xx_d.txt
##cp ./h320mtadp ../../../10-common/version/debug/linux_ppc_85xx/

##echo makefile_85xx_d completed!


##make -f makefile_85xx_r clean

##make -f makefile_85xx_r 1> ../../../10-common/version/compileinfo/h320mtadp_linux_ppc_85xx_r.txt
##cp ./h320mtadp ../../../10-common/version/release/linux_ppc_85xx/

##echo makefile_85xx_r completed!

