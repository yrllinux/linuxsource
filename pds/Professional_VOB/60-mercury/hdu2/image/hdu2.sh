#!/bin/sh
#set socket recv max buff size to 2M
echo 2097152 > /proc/sys/net/core/rmem_max
#set socket send max buff size to 2M
echo 2097152 > /proc/sys/net/core/wmem_max
cd /usr/bin/
./loadmodules_ti8168.sh
./logs &
./hdu2 &
./detectee &
