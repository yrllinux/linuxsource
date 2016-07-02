#!/bin/sh
	
make -f makefile_82xx_d clean

make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/watchdoglib_linux_ppc_82xx_d.txt
cp ./libwatchdog.a ../../../10-common/lib/debug/linux_ppc_82xx/

echo makefile_82xx_d completed!


make -f makefile_82xx_r clean

make -f makefile_82xx_r 1> ../../../10-common/version/compileinfo/watchdoglib_linux_ppc_82xx_r.txt
cp ./libwatchdog.a ../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_82xx_r completed!

make -f makefile_85xx_d clean

make -f makefile_85xx_d 1> ../../../10-common/version/compileinfo/watchdoglib_linux_ppc_855xx_d.txt
cp ./libwatchdog.a ../../../10-common/lib/debug/linux_ppc_85xx/

echo makefile_85xx_d completed!


make -f makefile_85xx_r clean

make -f makefile_85xx_r 1> ../../../10-common/version/compileinfo/watchdoglib_linux_ppc_85xx_r.txt
cp ./libwatchdog.a ../../../10-common/lib/release/linux_ppc_85xx/

echo makefile_85xx_r completed!

make -f Makefile_rh_d clean

make -f Makefile_rh_d
cp ./libwatchdog.a ../../../10-common/lib/debug/linux/

echo Makefile_rh_d completed!

make -f makefile_rh_r clean

make -f makefile_rh_r
cp ./libwatchdog.a ../../../10-common/lib/release/linux/

echo makefile_rh_r completed!

make -f makefile_82xx_linux12_d clean

make -f makefile_82xx_linux12_d 1> ../../../10-common/version/compileinfo/watchdoglib_linux12_ppc_82xx_d.txt
cp ./libwatchdog12.a ../../../10-common/lib/debug/linux_ppc_82xx/

echo makefile_82xx_linux12_d completed!


make -f makefile_82xx_linux12_r clean

make -f makefile_82xx_linux12_r 1> ../../../10-common/version/compileinfo/watchdoglib_linux12_ppc_82xx_r.txt
cp ./libwatchdog12.a ../../../10-common/lib/release/linux_ppc_82xx/

echo makefile_82xx_linux12_r!


make -f makefile_85xx_linux12_d clean

make -f makefile_85xx_linux12_d 1> ../../../10-common/version/compileinfo/watchdoglib_linux12_ppc_855xx_d.txt
cp ./libwatchdog12.a ../../../10-common/lib/debug/linux_ppc_85xx/

echo makefile_85xx_linux12_d completed!


make -f makefile_85xx_linux12_r clean

make -f makefile_85xx_linux12_r 1> ../../../10-common/version/compileinfo/watchdoglib_linux12_ppc_85xx_r.txt
cp ./libwatchdog12.a ../../../10-common/lib/release/linux_ppc_85xx/

echo makefile_85xx_linux12_r completed!

cd ..
