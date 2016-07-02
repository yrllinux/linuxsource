#!/bin/bash

####前120s，每隔2s检测一下mcu8kh看是否mcu启动成功,0表示已升级，1表示正常，成功则退出脚本####
FILEPATH="/opt/mcu/data/runstatus_8kh.stchk";
FILEROLLBACK="/opt/mcu/mcuorg.tar.gz";
loop=0;
while [ $loop -lt 60 ];
do 
	sleep 2;
	if [ ! -f $FILEPATH ]; then
  	echo "file $FILEPATH not exist!";
	else
  	str=`cat $FILEPATH| grep "mcu8kh = "`;
  	val=`echo $str|cut -d '=' -f 2`;
  	val=${val// /};
  	if (( $val == 1 ));then
			echo "mcu startup succeesfully,exit!"; 
  		exit 1 ;
  	fi
	fi

  loop=$(($loop+1));
done
  
#after 120s,mcu8kh still =0,roolback,reboot

cd /opt/mcu
if [ ! -f $FILEROLLBACK ] ;then
	echo "file $FILEROLLBACK not exist!";
else
	echo "rollback reboot"  
	  chmod a+x mcuorg.tar.gz
		tar xzvf mcuorg.tar.gz
		reboot
fi
  
  




##########Follows are old strategy###########
#n=0;
#loop=0;
#while [ 1 ];
#do
#	sleep 40;

#	filepath="/opt/mcu/data/runstatus_8kh.stchk";
#	if [ ! -f  $filepath ]; then
#		echo "file not exist!";
#		break;
#	fi
#	str=`cat $filepath | grep "mcu8kh = "`;	
#        val=`echo $str | cut  -d '=' -f 2`;
#        val=${val// /};
#        #echo "val=${val}";
#        if (( $val != 1 )); then let n=n+1; fi
#        if [ $n -eq 3 ]; then
#		n=0;
#                break;
#        fi

#        let loop=loop+1;        
#done

#action after break:
#canupdate=1;
#cd /opt/mcu
#filepathorg="./mcuorg.tar.gz";
#if [ ! -f  $filepathorg ]; then
#    echo "file not exist! update roll back fail!";
#    canupdate=0;
#fi

#if [ $canupdate -eq 1 ]; then
#	echo "begin to update rollback!"
#	chmod a+x mcuorg.tar.gz
#	tar xzvf mcuorg.tar.gz
#	reboot
#fi

#exit 1
