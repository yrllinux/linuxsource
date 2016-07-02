#!/bin/bash

#author: Peng Guofeng
#date:	 2012.8.21
#purpose: To keep these programmes always running

mcu=mcu_8000h
mmp=mmp_8000h
prs=prs_8000h
snmp=snmpd
gk=kdvgk_redhat
pxy=pxyserverapp_redhat
logs=kdvlogserver
iw=interworking
regs=regserver
netcap=NetCap.sh
dete=detectee

#ethdir
ethdir=/etc/sysconfig/network-scripts

#connect.ini
conn_ini=/opt/mcu/conf/connect.ini
#conf_limit=/etc/security/limits.conf

#variable
interval=3
bwrite=0

ulimit -q unlimited

##increase system mailbox size due to kdv/kdm/g400
#if [ `ulimit -q` -ne "2000000" ];then
#	ulimit -q 2000000
#fi

#if cat $conf_limit | grep msgqueue | grep -v "^#" > /dev/null
#then
#	echo "$conf_limit has set value of msgqueue"
#else
#	echo -e "*\t\tsoft\t\tmsgqueue\t8192000" >> $conf_limit
#	echo -e "*\t\thard\t\tmsgqueue\t8192000" >> $conf_limit
#fi

#check mac before run mcu_8000h add by pengguofeng@2013-8-29
/opt/mcu/check_mac.sh

#exec startmcu.sh & guard.sh
echo "exec /opt/mcu/startmcu.sh and guard.sh"
/opt/mcu/startmcu.sh &
/opt/mcu/guard.sh &

#add iflink to /etc/udev/rules.d/70-persistent-net.rules
echo "exec /opt/mcu/add_iflink.sh & adp_route.sh"
/opt/mcu/add_iflink.sh &
/opt/mcu/adp_route.sh &

while [ 1 ]
do
if ifconfig | grep eth1 > /dev/null
then
	echo "eth1 is OK"
	
	if [ $bwrite -eq 0 ]
	then
		echo "[McuInfo]" > $conn_ini
		# -f must be -F
		ipstr=`cat $ethdir/ifcfg-eth1 | grep IPADDR | awk -F = '{printf $2}'`
		echo "McuIpAddr = $ipstr" >> $conn_ini
		bwrite=1
	fi
	
	sleep $interval
	
	if pgrep $mcu >/dev/null
	then
		echo "$mcu is running"
		sleep $interval
	else
		/opt/mcu/$mcu & 
	fi
	
	if pgrep $mmp >/dev/null
	then
		echo "$mmp is running"
		sleep $interval
	else
		/opt/mcu/$mmp &
	fi

	if pgrep $prs >/dev/null
	then
		echo "$prs is running"
		sleep $interval
	else
		/opt/mcu/$prs &
	fi

	if pgrep $snmp >/dev/null
	then
		echo "$snmp is running"
		sleep $interval
	else
		/opt/mcu/$snmp &
	fi

	if pgrep $gk >/dev/null
	then
		echo "$gk is running"
		sleep $interval
	else
		#if mcu has taken the port of 60000, then let gk run
		if netstat -anpt |grep $mcu | grep 60000 | grep LISTEN
		then
			/opt/mcu/$gk &
		fi
	fi

	#$pxy : too long program name!!
	if pgrep pxyserver >/dev/null
	then
		echo "$pxy is running"
		sleep $interval
	else
		/opt/mcu/$pxy &
	fi

	if pgrep $logs >/dev/null
	then
		echo "$logs is running"
		sleep $interval
	else
		/opt/mcu/$logs &
	fi

	if pgrep $iw >/dev/null
	then
		echo "$iw is running"
		sleep $interval
	else
		/opt/mcu/$iw &
	fi

	if pgrep $regs >/dev/null
	then
		echo "$regs is running"
		sleep $interval
	else
		/opt/mcu/$regs &
	fi
	
		if pgrep $netcap >/dev/null
	then
		echo "$netcap is running"
		sleep $interval
	else
		/opt/mcu/$netcap &
	fi
	
	if pgrep $dete >/dev/null
	then
		echo "$dete is running"
		sleep $interval
	else
		/opt/mcu/$dete &
	fi
	
else
	echo "eth1 is down"
	sleep $interval
fi
done
