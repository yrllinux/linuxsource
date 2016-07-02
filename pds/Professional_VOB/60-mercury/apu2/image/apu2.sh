#!/bin/sh
#set socket recv max buff size to 2M
echo 2097152 > /proc/sys/net/core/rmem_max
#set socket send max buff size to 2M
echo 2097152 > /proc/sys/net/core/wmem_max
cd /usr/bin/

echo "boot c667x"
if [ -d apu2ep ]; then
	cd apu2ep
	./c667x_boot /dev/c667x_pcie_ep
	cd ../
fi

ifconfig eth1 up
ifconfig lo up

./kdvlogserver &
./apu2 &
./detectee &
