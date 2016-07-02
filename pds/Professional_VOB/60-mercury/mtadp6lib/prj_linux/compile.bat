#!/bin/sh

echo start to make Ipv6 module...

echo make makefile_82xx_mpc_embedded_linux12_v6_r
make -f makefile_82xx_mpc_embedded_linux12_v6_r clean all > ../../../10-common/version/compileinfo/mtadp_linux_ppc_82xx_mpc_embedded_linux12_v6_r.txt
cp ./libmtadp612embedded.a ../../../10-common/lib/release/linux_ppc_82xx/
echo makefile_82xxhdi_linux12_v6_r for 8000A completed!

echo make makefile_82xx_linux12_v6_r
make -f makefile_82xx_linux12_v6_r clean all > ../../../10-common/version/compileinfo/mtadp_linux_ppc_82xx_linux12_v6_r.txt
cp ./libmtadp612.a ../../../10-common/lib/release/linux_ppc_82xx/
echo makefile_82xx_linux12_v6_r for CRI2 completed!

echo make makefile_rh_v6_r
make -f makefile_rh_v6_r clean all > ../../../10-common/version/compileinfo/mtadp_linux_rh_8000e_v6_r.txt
cp ./libmtadp8ke6.a ../../../10-common/lib/release/linux/
echo makefile_rh_v6_r for 8000G completed!

cd ..
