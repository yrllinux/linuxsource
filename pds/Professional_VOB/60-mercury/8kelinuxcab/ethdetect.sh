#!/bin/sh

#############################################################
# detect if eth in use is broken and shift to other eth if so
#############################################################

#eth record
FILE_ETHX="/opt/ethchoice"

#route record
ROUTEFILE="/opt/mcu/conf/multinetcfg.ini"
TMPROUTEFILE="/opt/tmproute"
ROUTEFILE_MULTIMANU="/opt/mcu/conf/multimanucfg.ini"
conn_ini=/opt/mcu/conf/connect.ini
ethdir=/etc/sysconfig/network-scripts

#progress
MCU="mcu_8000e"
MMP="mmp_8000e"
PRS="prs_8000e"
MCU_LISTENPORT="60000"
GK="mcugk_redhat"
PXY="pxyserverapp_redhat"
DETECTEE="detectee"
NETCAP="NetCap.sh"

#usage: func ethid
SetDefaultGWandRoute()
{
	# deal with default gw-------------------
        defaultgw=`cat /etc/sysconfig/network-scripts/ifcfg-eth$1 | grep "GATEWAY" |awk -F= '{print $NF}'`
        echo defaultgw=$defaultgw
        if [ "$defaultgw" = "" ]; then
                echo "no def gw is configured"
        else
       		echo "ip route add default via $defaultgw dev eth$1"
                ip route add default via $defaultgw dev eth$1
        fi


        # deal with route------------------------
        routenum=0

        LINENUM=`cat $ROUTEFILE |awk '/\[eth1\]/ {print NR-1; exit 0;}'`
        if [ "$LINENUM" = "" ]; then
                return 0;
        fi
        cat $ROUTEFILE | head -n $LINENUM > $TMPROUTEFILE

        routenum=`cat $TMPROUTEFILE | grep Num | awk -F= '{print $NF}'`
        if [ "$routenum" = "" ]; then
                echo "no route is configured"
                return 0
        fi
        if [ $routenum -eq 0 ]; then
                echo "no route is configured as routenum is 0"
                return 0
        fi

        routeMax=64
        idx=0
        while :
        do
                if [ $idx -ge $routenum ]; then
                        echo idx:$idx ge $routenum, break
                        break;
                fi

                if [ $idx -ge $routeMax ]; then
                        echo "routenum reach the max:$routeMax"
                        break;
                fi

                echo "setroute $idx"
                #setroute
                Gw=`cat $TMPROUTEFILE |grep Gw$idx | awk -F= '{print $NF}'`
                Mask=`cat $TMPROUTEFILE |grep Mask$idx | awk -F= '{print $NF}'`
                Ip=`cat $TMPROUTEFILE |grep Ip$idx | awk -F= '{print $NF}'`

                echo "route add -net $Ip netmask $Mask gw $Gw dev eth$1"
                route add -net $Ip netmask $Mask gw $Gw dev eth$1

                let idx=idx+1
        done
}

#restart <prog_name>
restart()
{
	pid=`ps -aux | grep $1 | grep -v grep |  awk '{print $2}'`
	
	if [ "$pid" = "" -a -f "/opt/mcu/$1" ]
	then
		cd /opt/mcu && ./$1 &
	fi
}
#restart gk,gk must start after mcu,make sure mcu get the 60000port
restartgk()
{
  mcuport=`netstat -anp|grep $MCU|grep LISTEN|grep $MCU_LISTENPORT`
 #mcupid=`ps -aux | grep $MCU | grep -v grep |  awk '{print $2}'`
  pid=`ps -aux | grep $GK | grep -v grep |  awk '{print $2}'`	
 	echo "restart gk " 
	if [ "$mcuport" != "" -a "$pid" = "" -a -f "/opt/mcu/$GK" ]
	then
		echo "mcu listen port $MCU_LISTENPORT"
		cd /opt/mcu && ./$GK &
	fi

}
#notify <prog_name>
notify()
{
	pid=`ps -aux | grep $1 | grep -v grep |  awk 'BEGIN {minpid=0x8000} {if ($2<minpid){minpid=$2}}END{print minpid}'`
	
	if [ "$pid" != "" -a -f "/opt/mcu/$1" ]
	then
		kill -10 $pid
	fi
}

#---------------main process------------------

#global variable
detectinterval=6
bFirstLoop=1
bwrite=0

while :
do
	#add by pengguofeng 2012-9-14: set IP of eth1 to connect.ini
        if [ $bwrite -eq 0 ]
	then
		echo "[McuInfo]" > $conn_ini
		# -f must be -F
		ipstr=`cat $ethdir/ifcfg-eth1 | grep IPADDR | awk -F = '{printf $2}'`
		echo "McuIpAddr = $ipstr" >> $conn_ini
		bwrite=1
	fi

	# sleep for a while
	sleep $detectinterval;
        
	#ethcur
	ethcur=0
	
	#ethbak
	ethbak=1
	
	if [ ! -f $FILE_ETHX ]; then
          echo "$FILE_ETHX doesn't exist right now!"
          echo -n x > $FILE_ETHX
          continue;
   fi

	#ethorg
	ethorg=`cat $FILE_ETHX`
	
	#invalid filecontent
	if [ "$ethorg" != 0 ] && [ "$ethorg" != 1  ] && [ "$ethorg" != x ]
	then
		echo "$FILE_ETHX has invalid ethid"
		echo -n "x" > $FILE_ETHX
		continue
	fi
	
	#check eth0 & eth1: "1" :down  "": OK
	eth0_link=`mii-tool | grep eth0 | awk '{ if ($0 ~ /no link/) {print 1; exit 0} }'`
	eth1_link=`mii-tool | grep eth1 | awk '{ if ($0 ~ /no link/) {print 1; exit 0} }'`
	
	
	#set ethcur, eth0 has priority 
	if [ "$eth0_link" = "" ]
	then
		echo "use eth0"
		ethcur=0
	else if [ "$eth1_link" = "" ]
		then
			echo "use eth1"
			ethcur=1
			ethbak=0
		else
			echo "both are down"
			echo -n "x" > $FILE_ETHX
			continue
		fi
	fi
	

		echo -n $ethcur > $FILE_ETHX
		#restart ethernet
		ethcur_up=`ifconfig eth$ethcur |awk '{ if ($0 ~ /UP/) {print 1; exit 0} }'`
		ethbak_up=`ifconfig eth$ethbak |awk '{ if ($0 ~ /UP/) {print 1; exit 0} }'`
		if [ "$ethbak_up" = 1 ]
			then
				ifdown eth$ethbak
		fi
		if [ "$ethcur_up" = "" ]
			then
				ifup eth$ethcur
		fi
		
       		
 		#set route
 		SetDefaultGWandRoute $ethcur
 		
		#restart progress if not start
 		restart $MCU
 		#restart $GK
 		restartgk
 		restart $PRS
 		restart $MMP
 		restart $PXY
 		restart $DETECTEE
 		restart $NETCAP
 		
 		#send SIGUSR1 signal to mcu
 		if [ "$ethcur" != "$ethorg" ] || [ "$bFirstLoop" = 1 ]
 		then
 			notify $MCU
 		fi
 		
 		bFirstLoop=0;

done

