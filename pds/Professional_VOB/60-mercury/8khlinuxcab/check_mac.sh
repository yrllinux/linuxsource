#!/bin/bash

#####################################################
#  功能：比较70-net.rules和ifconfig得到的结果
#  修改履历：
#  2013-11-29     pengguofeng     修正bug:00164481
#
#####################################################

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
	echo "no need to check-mac due to no $file exist" >> $outfile
	exit 0
fi

cat $file >> $outfile

# 目前检测3块网卡
eth_arr=("eth0 eth1 eth2")
for eth in ${eth_arr[@]}
do

eth_mac_from_70net=$( cat ${file} | grep $eth | awk -F'"' '{print $8}' )

eth_mac_from_ifcfg=$(ifconfig $eth | grep HWaddr | awk '{print $5}' )

#upper
eth_mac_from_ifcfg=$( echo ${eth_mac_from_ifcfg} | tr [a-z] [A-Z] )
eth_mac_from_70net=$( echo ${eth_mac_from_70net} | tr [a-z] [A-Z] )

echo "ifcfg. eth:${eth} mac. ${eth_mac_from_ifcfg}"  >> $outfile

echo "70-net eth:${eth} mac. ${eth_mac_from_70net}"  >> $outfile
if [ x"${eth_mac_from_70net}" = x ]
then
	echo "70-net.files not set value of $eth, skip it" >> $outfile
	continue
fi

if [ "${eth_mac_from_ifcfg}" = "${eth_mac_from_70net}" ]
then
        echo "they are same, pass" >> $outfile
	continue
else
        echo "not same, remove 70net and reboot" >> $outfile
        rm -f ${file}
        reboot
fi
done

