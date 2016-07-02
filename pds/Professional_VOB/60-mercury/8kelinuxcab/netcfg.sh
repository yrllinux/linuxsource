#!/bin/sh

###################################
# Check which wire is ok and then up this one,down the other.
# If both wires are linked, we use eth0.
# Config default gateway to the eth which is on 
###################################

#prepare the file to write eth number
FILE_ETHX="/opt/ethchoice"
touch $FILE_ETHX
chmod a+wr $FILE_ETHX
>$FILE_ETHX

#down eth2
ifdown eth2

bothup=1
upid="";
#check eth1 first for convenience in situation when both eth0 and eth1 is wired up
for loop in  eth1 eth0
do
        ethChoiceCfg="/etc/sysconfig/network-scripts/ifcfg-$loop"

        linkStat=`mii-tool -v |grep "$loop" |awk '{if ($0~/no link/) {print "1"; exit 0}}'`

        if [ "$linkStat" = "1" ]; then
		bothup=0
		ifdown $ethChoiceCfg
	
        else
        	#upid=`echo $loop |awk '{print substr($0,'4','1')}'`
		upid=$ethChoiceCfg
        fi
done

if [ "$upid" = "" ]; then
	echo no eth is link on! error!
	# If both link is down, we choose eth0 up, eth1 down
	ifup eth0
	echo "x" > $FILE_ETHX
	exit 1	
fi

#if both link is on, we choose eth0,so ifdown eth1
if [ "$bothup" = "1" ]; then
	ifdown eth1
fi

#get default gw 
defaultgw=`cat $upid | grep "GATEWAY" |awk -F= '{print $NF}'`
echo defaultgw=$defaultgw
if [ "$defaultgw" = "" ]; then
	echo no def gw is configured
	exit 1
fi

#get ethx(eth0 or eth1) 
ethx=`echo $upid | tail -c 5`
echo ethx=$ethx
if [ "$ethx" = "" ]; then
	echo no ethx is ok,exit 1
	exit 1
fi

#write ethx to file
echo $upid | tail -c 2 |head -c 1 > $FILE_ETHX



#add default gw to ethx
echo ip route add default via $defaultgw dev $ethx
ip route add default via $defaultgw dev $ethx


