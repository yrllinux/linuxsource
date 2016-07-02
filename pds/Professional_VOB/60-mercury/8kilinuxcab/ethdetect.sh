#!/bin/bash

#############################################################
# detect if eth in use is broken and shift to other eth if so
# give priority to the use of eth_cur, only when it's down using eth_bak instead
#############################################################

FILE_ETHX="/opt/ethchoice"
ROUTEFILE="/opt/mcu/conf/multinetcfg.ini"
TMPROUTEFILE="/opt/tmproute"
ROUTEFILE_MULTIMANU="/opt/mcu/conf/multimanucfg.ini"
ETHDEBUGFILE="/opt/mcu/ethdetectdebug.ini"
ETHLOGFILE="/opt/mcu/ethdetectlog.log"
conn_ini=/opt/mcu/conf/connect.ini
ethdir=/etc/network/interfaces
mcucfg_ini=/opt/mcu/conf/mcucfg.ini
MCU="mcu_8000i"
MMP="mmp_8000i"
PRS="prs_8000i"
MCU_LISTENPORT="60000"
#add by pgf@2012-9-24
SNMP=snmpd
LOGS=kdvlogserver
GK="mcugk_redhat"
PXY="pxyserverapp_redhat"
INTERWORK="mcuinterworking"
REGSERVER="mcuregserver"
DETECTEE="detectee"
NETCAP="NetCap.sh"

SetDefaultGWandRoute()
{
	# deal with default gw-------------------
	
				defaultgw=`cat $ethdir | grep -3 "iface eth0 inet static" | grep "gateway" | awk '{print $NF}'`
        echo defaultgw=$defaultgw
        if [ "$defaultgw" = "" ]; then
                echo "no def gw is configured"
        else
       		#echo "ip route add default via $defaultgw dev eth$1"
                #ip route add default via $defaultgw dev eth$1
                #add default gateway [pgf@2012-9-27]
                echo "route add default gw $defaultgw dev eth$1"
                route add default gw $defaultgw dev eth$1
                #flush arp cache [pgf@2012-9-27]
                echo "ip route flush cache"
                ip route flush cache
        fi


        # deal with route------------------------
        routenum=0

        LINENUM=`cat $ROUTEFILE |awk '/\[eth0\]/ {print NR-1; exit 0;}'`
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
	pid=`ps aux | grep $1 | grep -v grep |  awk '{print $2}'`
	echo "restart $1,pid=\"$pid\""
	if [ "$pid" = "" -a -f "/opt/mcu/$1" ]
	then
		cd /opt/mcu && ./$1 &
		if [ $? = 0 ]
		then
			echo "restart $1 succeed"
		else
			echo  "restart $1 failed"
		fi
	fi
}
#restart gk,gk must start after mcu,make sure mcu get the 60000 port
restartgk()
{
  mcuport=`netstat -anp|grep $MCU|grep LISTEN|grep $MCU_LISTENPORT`
 #mcupid=`ps aux | grep $MCU | grep -v grep |  awk '{print $2}'`
  pid=`ps aux | grep $GK | grep -v grep |  awk '{print $2}'`	
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
	pid=$(ps -A | grep $1 | awk -F " " '{print $1}')
	
	if [ "$pid" != "" -a -f "/opt/mcu/$1" ]
	then
		echo "notify $1 10 pid=\"$pid\""
		kill -10 $pid
	fi
}
#---------------main process------------------

#zjj20131023 add for not permission denied by ubuntu while tcpdump process write file
/etc/init.d/apparmor teardown

ulimit -q unlimited
#check mac before run mcu_8000i add by pengguofeng@2013-8-29
/opt/mcu/check_mac.sh
/opt/mcu/gpudecoder.out

#add by pgf@2012-9-24
#exec startmcu.sh & guard.sh
echo "exec /opt/mcu/startmcu.sh and guard.sh" >> $ETHLOGFILE
/opt/mcu/startmcu.sh &
/opt/mcu/guard.sh &


if [ ! -f $mcucfg_ini ]
then
	mcucfg_ini=/opt/mcu/conf/mcucfg.bak
	echo "mcucfg not exists,use back file" >> $ETHLOGFILE
fi

bisstartregserver=0
if [ -f $mcucfg_ini ]
then
	result=$(grep "UseSip" ${mcucfg_ini})
	if [ "$result" != "" ]
	then
		result=${result:9:1}
		if [ "$result" = "1" ]
		then
			bisstartregserver=1
			echo "start regserver" >> $ETHLOGFILE
		fi
	fi
fi

bisethbackmode=0
if [ -f $mcucfg_ini ]
then
	result=$(grep "EthModeType" ${mcucfg_ini})
	if [ "$result" != "" ]
	then
		result=${result:14:1}
		if [ "$result" = "1" ]
		then
			bisethbackmode=1
			echo "start eth back mode" >> $ETHLOGFILE
		fi
	fi
fi


#global variable
detectinterval=6
bwrite=0
#when system startup ifdown all eth,then detect the link status ifup the linkon eth
if [ "$bisethbackmode" = "1" ]
then
ifconfig eth0 down
ifconfig eth1 down
ifconfig eth2 down
else
ifconfig eth0 up
ifconfig eth1 up
ifconfig eth2 up
fi

#load device modlue
insmod /lib/modules/3.2.0-29-generic/kernel/drivers/hwmon/coretemp.ko
insmod /opt/mcu/bcsr.ko

#snmp need start at first position
restart $SNMP
sleep $detectinterval

#modify by zjj,mcu start more earlier for do some thing what other process rely on
restart $MCU

bFirstLoop=1
#ifdown eth0 ,if immediatly use ethtool to test linkstatus may get wrong result,so sleep 6s first
sleep $detectinterval

#add apach script by zjj
#apach should start after mcu started because the script will read some file what mcu create while it's start
#so here after sleep 6 second,can't modify the order by random
/opt/mcu/apachesvrcfg &

while :
do
        # begin to detect
        # check if user want to detect (add lator if needed)
        # now user do want to detect always
        
        #add by pengguofeng 2012-9-14: set IP of eth1 to connect.ini
        if [ $bwrite -eq 0 ]
				then
					echo "[McuInfo]" > $conn_ini
					# -f must be -F
					# ipstr=`cat $ethdir/ifcfg-eth0 | grep IPADDR | awk -F = '{printf $2}'`
					ipstr=$(grep "address" ${ethdir} | head -n 1 | awk -F " "  '{printf $2}' )
					echo "McuIpAddr = $ipstr" >> $conn_ini
					bwrite=1
				fi
	if [ "$bisethbackmode" = "1" ]
	then
        debug=0
        #get debugmode in ethdetecdebug.ini
  			if [ ! -f $ETHDEBUGFILE ]
  			then
  				echo 0 > $ETHDEBUGFILE
  			else
  				debug=`cat $ETHDEBUGFILE`
  			fi

				#deubg = 1 means open deubg mode,write the stdout and stderr to logfile
				if [ "$debug" = 1 ]
				then
					exec 6>&1 1>>$ETHLOGFILE
					exec 7>&2 2>&1
					echo
					echo `date`
					#sizeof ethdetectlog.log > 2M,rewrite it
					logsize=`du -k $ETHLOGFILE | awk '{print $1}'`
					if [ $logsize -gt 2048 ]
					then
					    rm -rf $ETHLOGFILE && touch $ETHLOGFILE
					fi 
				else
					# stdout to /dev/null
					if [ -e /dev/fd/6 ]
					then
						exec 1>/dev/null 6>&-
					fi
					if [ -e /dev/fd/7 ]
					then
						exec 2>/dev/null 7>&- 
					fi

				fi
				
				
				ethcur=0
				ethbak=0
        # get which eth is in use
        if [ ! -f $FILE_ETHX ]; then
                echo "$FILE_ETHX doesn't exist right now!"
                echo -n x > $FILE_ETHX
        fi
        

        #ethinuse
        ethorg=`cat $FILE_ETHX`
        echo "ethorg: $ethorg"
        #invalid filecontent
				if [ "$ethorg" != 0 ] && [ "$ethorg" != 1  ] && [ "$ethorg" != x ]
				then
					echo "$FILE_ETHX has invalid ethid,set to x"
					echo -n "x" > $FILE_ETHX
					ethorg=x
				fi
					
        if [ "$ethorg" = "x" ]
        then
        	ethbak=1
        else
        	ethcur=$ethorg
        fi

				if [ $ethcur = 0 ]
				then
					ethbak=1
				fi

				echo "cur : eth$ethcur bak: eth$ethbak"
        bCurNoLink=`ethtool eth$ethcur | awk '{ if ($0 ~ /Speed: Unknown!/) {print 1; exit 0} }'`
        bBakNoLink=`ethtool eth$ethbak | awk '{ if ($0 ~ /Speed: Unknown!/) {print 1; exit 0} }'`
        
        if [ "$bCurNoLink" = 1 ] && [ "$bBakNoLink" = 1 ]
        then
        	echo "both are nolink"
        	echo -n x > $FILE_ETHX
        else
         	if [ "$bCurNoLink" != 1 ]
         	then
						echo "eth$ethcur is linkon use it"
					else
						echo "eth$ethcur is linkoff ,eth$ethbak is linkon ,use it" 
						temp=$ethcur
						ethcur=$ethbak
						ethbak=$temp
						        		
         	fi
         	ifconfig eth$ethcur up
					ifconfig eth$ethbak down
					echo -n $ethcur > $FILE_ETHX
					restart $MCU
	        #restart $GK
	        restartgk
	        restart $PRS
	        restart $MMP
	        restart $PXY
	        restart $SNMP
	        restart $LOGS
	        if [ "$bisstartregserver" == "1" ]
	        then
	        	restart $INTERWORK
			    	restart $REGSERVER  
			    fi
			    restart $NETCAP
					restart $DETECTEE			      		
	       	SetDefaultGWandRoute $ethcur
			    if [ "$ethcur" != "$ethorg" ] || [ "$bFirstLoop" = 1 ]
			 			then
			 				notify $MCU
			 		fi
			 		bFirstLoop=0;

        fi 	
	else
		restart $MCU
	  #restart $GK
		restartgk
		restart $PRS
		restart $MMP
		restart $PXY
		restart $SNMP
		restart $LOGS
		if [ "$bisstartregserver" == "1" ]
		then
			restart $INTERWORK
			restart $REGSERVER  
		fi
		restart $NETCAP
		restart $DETECTEE			  
	fi
       

        # sleep 6 sec
        sleep $detectinterval;

done 
