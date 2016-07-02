#!/bin/sh

#this script is used to start or stop tcpdump cmd file: /opt/mcu/pcap/CapChoice 
#content 1:start,0:stop,x:nothing
SAVEFILE=/opt/mcu/pcap/UsrNetCap
CAPCHOICEFILE=/opt/mcu/pcap/CapChoice
EACHFILESIZE=40
FILENUM=5
sleepinterval=3
mkdir -p /opt/mcu/pcap
chmod 777 /opt/mcu/pcap


while :
do
 sleep $sleepinterval
#get choice
if [ -f $CAPCHOICEFILE ]
then
flag=`cat $CAPCHOICEFILE`
else
	flag="X"
	echo -n "X" > $CAPCHOICEFILE
fi

if [[ "$flag" != "1" && "$flag" != "0" ]]
then
   echo "$CAPCHOICEFILE content not 0 or 1!"
   flag="X"
fi



#start 
if [ "$flag" = "1" ]
then
  echo "remove last cap files!"
  for ((i=0;i<$FILENUM;i++))
  do
    if [ -f $SAVEFILE$i* ]
     then
       rm -f $SAVEFILE$i*
    fi
  done
  echo "start netcap now!"
  tcpdump -C $EACHFILESIZE -W $FILENUM -s 0 -w $SAVEFILE &
  echo -n "X" > $CAPCHOICEFILE
  sleepinterval=1
#stop
else 
if [ "$flag" = "0" ]
	then
  	echo "stop tcpdump now!"
  	tcpdumppid=`pgrep tcpdump`
  	kill -2 $tcpdumppid
  	echo "change all filenames with .pcap end!"
  	for ((i=0;i<$FILENUM;i++))
  	do
    	if [ -f "$SAVEFILE"$i ]
    	then
      	 mv -f $SAVEFILE$i $SAVEFILE$i.pcap
    	fi
  	done
  	echo -n "X" > $CAPCHOICEFILE
  	sleepinterval=3
 	fi
fi 

done