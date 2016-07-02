#!/bin/sh

make -f makefile_8377_d clean
make -f makefile_8377_d 1> ../../../10-common/version/compileinfo/apu2_linux_8377_d.txt
cp ./apu2 ../../../10-common/version/debug/os_linux/apu2/apu2
echo makefile_8377_d completed!

make -f makefile_8377_r clean
make -f makefile_8377_r 1> ../../../10-common/version/compileinfo/apu2_linux_8377_r.txt
cp ./apu2 ../../../10-common/version/release/os_linux/apu2/apu2
echo makefile_8377_r completed!
