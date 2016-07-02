#!/bin/bash

eth0_mac_from_ifcfg=$(ifconfig eth0 | grep HWaddr | awk '{print $5}' )

#debug mode 1: debug mode ^1: release mode
# while debug mode,will write to file
debug=$1

if [ x$debug = "x1" ]
then
        outfile=/opt/check-mac
else
        outfile=/dev/null
fi

date >> $outfile

ifconfig eth0 >> $outfile

file=/etc/udev/rules.d/70-persistent-net.rules

if [ ! -f $file ] 
then
	echo "no need to check-mac due to no $file" >> $outfile
	exit 0
fi

eth0_mac_from_70net=$( cat ${file} | grep eth0 | awk -F'"' '{print $8}' )

#upper
eth0_mac_from_ifcfg=$( echo ${eth0_mac_from_ifcfg} | tr [a-z] [A-Z] )
eth0_mac_from_70net=$( echo ${eth0_mac_from_70net} | tr [a-z] [A-Z] )

echo "ifcfg. eth0 mac. ${eth0_mac_from_ifcfg}"  >> $outfile

echo "70-net eth0 mac. ${eth0_mac_from_70net}"  >> $outfile

if [ "${eth0_mac_from_ifcfg}" = "${eth0_mac_from_70net}" ]
then
        echo "they are same, pass" >> $outfile
else
        echo "not same, remove 70net need reboot" >> $outfile
        rm -f ${file}
        reboot
fi
