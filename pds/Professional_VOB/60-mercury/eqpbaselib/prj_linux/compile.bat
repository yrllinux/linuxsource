#!/bin/sh

make -f makefile_8377_d clean
make -f makefile_8377_d 1> ../../../10-common/version/compileinfo/eqpbaselib_linux_ppc_8377_d.txt
echo makefile_8377_d completed!

make -f makefile_8377_r clean
make -f makefile_8377_r 1> ../../../10-common/version/compileinfo/eqpbaselib_linux_ppc_8377_r.txt
echo makefile_8377_r completed!

cd ..
