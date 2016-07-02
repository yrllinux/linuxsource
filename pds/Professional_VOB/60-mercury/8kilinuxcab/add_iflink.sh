#!/bin/bash

#author : peng guofeng
#date	: 2012-10-23
#purpose: add iflink to 70-persistent-net.rules

file=/etc/udev/rules.d/70-persistent-net.rules

for eth in eth0 eth1 eth2
do
	#1 get iflink of ethX
	iflink=$(cat /sys/class/net/$eth/iflink)
	
	#if null value,continue
	if [ ! "${iflink}" ];then
		echo "no value of iflink for $eth"
		continue;
	fi
	
	if cat $file | grep $eth | grep iflink
	then
		echo "$eth has value of iflink, so not overwrite it"
	else
		#2 add ATTR{iflink} to the end of line "ethX"
		sed -i '/'${eth}'/ s/^.*/&, ATTR{iflink}==\"'${iflink}'\"/' $file
	fi
done
