#!/bin/bash

#author : peng  guofeng
#create @ 2012-10-16
#purpose: auto adapt route when link change
interval=6
ifcfg=/etc/sysconfig/network-scripts/ifcfg-eth0
gw=$(cat $ifcfg | grep GATEWAY | grep -v ^#GATEWAY | awk -F'=' '{print $2}')

#function add by pengguofeng@2013-6-6
#purpose: send signal SIGUSR2 to MCU
notify()
{
	pid=`ps -ef | grep $1 | grep -v grep | awk '{print $2}' | sed -n 1p`
	
	if [ "$pid" != "" ]
	then
		echo "notify $1 12 pid=\"$pid\""
		kill -12 $pid
	fi
}

while :
do
sleep $interval
#1 find current default route dev
ecur=$(route -n | grep UG | awk '{print $8}' | sed -n 1p)
ebak=""
badd=0

if [ "$ecur" = "" ];then
	ecur=eth0
	badd=1
fi

[ $ecur = eth0 ] && ebak=eth1 || ebak=eth0

#2 check from ecur to ebak
if ifconfig $ecur | grep RUNNING > /dev/null
then
	if [ $badd = 1 ];then
		route add default gw $gw dev $ecur
	fi
	continue
else
	if ifconfig $ebak | grep RUNNING > /dev/null
	then
		ifconfig $ecur down
		route add default gw $gw dev $ebak
		ifconfig $ecur up
		ip route flush cache
		notify mcu/mcu
	else
		continue
	fi
fi
done
