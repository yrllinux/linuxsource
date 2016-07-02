#!/bin/sh

#clean
rm -rf ./app
mkdir ./app

#apploader and config file
cp -f sysconfig.sh ./app
cp -f autorun.inf ./app
cp -f module.cfg  ./app
cp -f ../../../10-common/version/release/linux_ppc_85xx/apploader/apploader ./app/

#module file
cp -f ../../../10-common/version/release/os_linux/hdsc/bcsr.ko ./app/
cp -f ../../../10-common/version/release/os_linux/hdsc/led_k.ko ./app/
cp -f ../../../10-common/lib/release/linux_ppc_85xx/nipdebug/ppc85xx_debugmisc.ko ./app/debugmisc.ko

#app
cp -f ../../../10-common/version/release/linux_ppc_85xx/watchdog     ./app/
cp -f ../../../10-common/version/release/linux_ppc_85xx/kdvgk        ./app/
cp -f ../../../10-common/version/release/linux_ppc_85xx/pxyserverapp ./app/
cp -f ../../../10-common/version/release/linux_ppc_85xx/mp8000b      ./app/
cp -f ../../../10-common/version/release/linux_ppc_85xx/dcserver_r   ./app/

#change file attrib
chmod 777 -R ./app/*

if [ -x "./app/watchdog" ] && [ -x "./app/kdvgk" ] && [ -x "./app/pxyserverapp" ] && [ -x "./app/mp8000b" ] && [ -x "./app/dcserver_r" ]
then
	mkfs.jffs2 -e 0x20000 -b --disable-compressor=rtime --compressor-priority=60:zlib -r ./app -o app.image
	mv -f app.image    ../../../10-common/version/release/os_linux/hdsc/hdsc.image
else
	echo "ERROR: cannot read file!"
fi

