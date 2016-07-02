#!/bin/sh

echo makefile_82xx_linux12_v6_r for 8000A
make  -f makefile_82xx_linux12_v6_r clean all > ../../../10-common/version/compileinfo/mcu_linux_ppc_82xx_v6_r.txt
if [ ! -f ../../../10-common/version/release/linux_ppc_82xx/v6 ]
then
	mkdir -p ../../../10-common/version/release/linux_ppc_82xx/v6
fi
cp ./mcu ../../../10-common/version/release/linux_ppc_82xx/v6/mcu2
echo makefile_82xx_linux12_v6_r completed

echo makefile_rh8000e_v6_r for 8000E
make -f makefile_rh8000e_v6_r clean all > ../../../10-common/version/compileinfo/mcu_linux_rh_8000e_v6_r.txt

if [ ! -f ../../../10-common/version/release/linux/v6 ]
then
        mkdir -p ../../../10-common/version/release/linux/v6
fi
cp ./mcu_8000e ../../../10-common/version/release/linux/v6/mcu_8000e
echo makefile_rh8000e_v6_r completed 


