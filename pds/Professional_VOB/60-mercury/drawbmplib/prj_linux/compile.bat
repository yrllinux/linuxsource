#!/bin/sh

make -f makefile_dm816x_r clean

make -f makefile_dm816x_r 1> ../../../10-common/version/compileinfo/draw_linux_ppc_dm816x_r.txt

echo makefile_dm816x_r completed!

make -f makefile_dm816x_d clean

make -f makefile_dm816x_d 1> ../../../10-common/version/compileinfo/draw_linux_ppc_dm816x_d.txt

echo makefile_dm816x_d completed!

cd ..
