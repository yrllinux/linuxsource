#!/bin/sh

make -f makefile_redhat_d clean

make -f makefile_redhat_d 1> ../../../10-common/version/compileinfo/simbrdwrapper_d.txt
cp ./libsimbrdwrapper.a ../../../10-common/lib/debug/linux/

echo makefile_redhat_d completed!


make -f makefile_redhat_r clean

make -f makefile_redhat_r 1> ../../../10-common/version/compileinfo/simbrdwrapper_r.txt
cp ./libsimbrdwrapper.a ../../../10-common/lib/release/linux/

echo makefile_redhat_r completed!

cd ..
