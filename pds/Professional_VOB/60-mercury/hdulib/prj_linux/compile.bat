#!/bin/sh
	
make -f makefile_82xx_d clean

make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/hdu_linux_ppc_82xx_d.txt
cp ./libhdulib.a ../../../10-common/lib/debug/linux_ppc_82xx/

echo makefile_82xx_d completed!


make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/hdu_linux_ppc_82xx_r.txt
cp ./libhdulib.a ../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_82xx_r completed!

make -f makefile_85xx_d clean

make -f makefile_85xx_d 1> ../../../10-common/version/compileinfo/hdu_linux_ppc_85xx_d.txt
cp ./libhdulib.a ../../../10-common/lib/debug/linux_ppc_85xx/

echo makefile_85xx_d completed!

make -f makefile_85xx_r clean

make -f makefile_85xx_r 1> ../../../10-common/version/compileinfo/hdu_linux_ppc_85xx_r.txt
cp ./libhdulib.a ../../../10-common/lib/release/linux_ppc_85xx/

echo makefile_85xx_r completed!

make -f makefile_82xx_dvi_r clean

make -f makefile_82xx_dvi_r 1> ../../../10-common/version/compileinfo/hdu_linux_ppc_82xx_dvi_r.txt
cp ./libhdudvilib.a ../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_85xx_r completed!

cd ..