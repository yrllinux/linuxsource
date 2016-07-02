#!/bin/sh

# **********************makefile_82xx_d
#make -f makefile_82xx_d clean
#
#make -f makefile_82xx_d 1> ../../../10-common/version/compileinfo/mcu_linux_ppc_82xx_d.txt
#cp ./mcu ../../../10-common/version/debug/linux_ppc_82xx/
#
#echo makefile_82xx_d completed!
# **********************makefile_82xx_d

# **********************makefile_82xx_linux12_d
#make -f makefile_82xx_linux12_d clean
#
#make -f makefile_82xx_linux12_d 1> ../../../10-common/version/compileinfo/mcu_linux12_ppc_82xx_d.txt
#cp ./mcu ../../../10-common/version/debug/linux_ppc_82xx/mcu12
#
#echo makefile_82xx_linux12_d completed!
# **********************makefile_82xx_linux12_d

# **********************makefile_82xx_8000b_d
#make -f makefile_82xx_8000b_d clean
#
#make -f makefile_82xx_8000b_d 1> ../../../10-common/version/compileinfo/mcu_linux_ppc_82xx_8000b_d.txt
#cp ./mcu ../../../10-common/version/debug/linux_ppc_82xx/mcu_8000b
#
#echo makefile_82xx_8000b_d completed!
# **********************makefile_82xx_8000b_d

# **********************makefile_82xx_8000c_d
#make -f makefile_82xx_8000c_d clean
#
#make -f makefile_82xx_8000c_d 1> ../../../10-common/version/compileinfo/mcu_linux_ppc_82xx_8000c_d.txt
#cp ./mcu ../../../10-common/version/debug/linux_ppc_82xx/mcu_8000c
#
#echo makefile_82xx_8000c_d completed
# **********************makefile_82xx_8000c_d

# **********************makefile_rh8000e_d
#make -f makefile_rh8000e_d clean
#
#make -f makefile_rh8000e_d 1> ../../../10-common/version/compileinfo/mcu_linux_rh_8000e_d.txt
#
#cp ./mcu_8000e ../../../10-common/version/debug/linux/mcu_8000e
# **********************makefile_rh8000e_d

# **********************makefile_82xx_r
# make -f makefile_82xx_r clean

make -f makefile_82xx_r clean all 1> ../../../10-common/version/compileinfo/mcu_linux_ppc_82xx_r.txt
cp ./mcu ../../../10-common/version/release/linux_ppc_82xx/

echo makefile_82xx_r completed

# **********************makefile_82xx_linux12_r
# make -f makefile_82xx_linux12_r clean

make -f makefile_82xx_linux12_r clean all 1> ../../../10-common/version/compileinfo/mcu_linux12_ppc_82xx_r.txt
cp ./mcu ../../../10-common/version/release/linux_ppc_82xx/mcu2

echo makefile_82xx_linux12_r completed

# **********************makefile_82xx_8000b_r
# make -f makefile_82xx_8000b_r clean

make -f makefile_82xx_8000b_r clean all 1> ../../../10-common/version/compileinfo/mcu_linux_ppc_82xx_8000b_r.txt
cp ./mcu ../../../10-common/version/release/linux_ppc_82xx/mcu_8000b

echo makefile_82xx_8000b_r completed

# **********************makefile_82xx_8000c_r
# make -f makefile_82xx_8000c_r clean

#make -f makefile_82xx_8000c_r clean all 1> ../../../10-common/version/compileinfo/mcu_linux_ppc_82xx_8000c_r.txt
#cp ./mcu ../../../10-common/version/release/linux_ppc_82xx/mcu_8000c

#echo makefile_82xx_8000c_r completed

# **********************makefile_rh8000e_r
# make -f makefile_rh8000e_r clean

make -f makefile_rh8000e_r clean all 1> ../../../10-common/version/compileinfo/mcu_linux_rh_8000e_r.txt
cp ./mcu_8000e ../../../10-common/version/release/linux/mcu_8000e

echo makefile_rh8000e_r completed

make -f makefile_rh8000h_r clean all 1> ../../../10-common/version/compileinfo/mcu_linux_rh_8000h_r.txt
cp ./mcu_8000h ../../../10-common/version/release/linux/mcu_8000h

echo makefile_rh8000h_r completed

make -f makefile_rh800l_r clean all 1> ../../../10-common/version/compileinfo/mcu_linux_rh_800l_r.txt
cp ./mcu_800l ../../../10-common/version/release/linux/mcu_800l

echo makefile_rh800l_r completed

make -f makefile_rh8000h_m_r clean all 1> ../../../10-common/version/compileinfo/mcu_linux_rh_8000h_m_r.txt
cp ./mcu_8000h_m ../../../10-common/version/release/linux/mcu_8000h_m

echo makefile_rh8000h_m_r completed

make -f makefile_rh8000i_r clean all 1> ../../../10-common/version/compileinfo/mcu_linux_rh_8000i_r.txt
cp ./mcu_8000i ../../../10-common/version/release/linux/mcu_8000i

echo makefile_rh8000i_r completed

make -f makefile_rh8000i_usb_r clean all 1> ../../../10-common/version/compileinfo/mcu_linux_rh_8000i_r.txt
cp ./mcu_8000i_usb ../../../10-common/version/release/linux/mcu_8000i_usb

echo makefile_rh8000i_usb_r completed

