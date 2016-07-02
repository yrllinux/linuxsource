#!/bin/bash
#set -x

sed '1,/^#here/d' $0 > mcu.tar.gz

#no matter where kdv8000h.bin is, spec path to /opt/mcu
#oldpath=`pwd`
oldpath=/opt/mcu
tar_path=`pwd`
#find / -name "mcu.tar.gz"
tar zxvf $tar_path/mcu.tar.gz -C /opt
rm -rf $tar_path/mcu.tar.gz

#2012-9-6: use `lsb_release -r`, show: "Release: x.x"
#set startup config file only for redhat 6.x
ver=`lsb_release -r`
#if [[ "$(uname -r)" =~ 2.6.32 ]]
if [ ${ver:9:1} -ge 6 ]
then
	echo "for redhat 6.x kernal:$(uname -r)"
	dir=/etc/init/mcu
	if [ -d $dir ];then
		rm -f $dir/*
	else
		mkdir -p $dir
	fi
	mv $oldpath/etc/*  $dir/
	
	#echo "++++++++++++++++++++"
	#ls -l $dir/
	#echo "++++++++++++++++++++"
fi

#add ethchoice,set value as 1
# -n means not add '\n'
#redhat 6.x just use eth1
#if [[ "$(uname -r)" =~ 2.6.32 ]]
if [ ${ver:9:1} -ge 6 ] 
then
	echo -n 1 > /opt/ethchoice
fi

#overwrite kdvlog.ini
log_file=/usr/kdvlog/conf/kdvlog.ini

if [ -f $log_file ]
then
	echo "$log_file exist, not overwrite"
	#rm -f $log_file
else
	mkdir -p /usr/kdvlog/conf/
	echo "mv /opt/mcu/kdvlog.ini $log_file"
	mv /opt/mcu/kdvlog.ini $log_file
fi

#ldconfig
cd /opt/mcu
echo > startmcu.sh
chmod a+x startmcu.sh
echo '#!/bin/bash' >./startmcu.sh
echo 'echo Starting Kedacom MCU...' >>./startmcu.sh
echo "cd $oldpath" >>./startmcu.sh
echo "ldconfig $oldpath" >>./startmcu.sh

echo KDV8000H update complete at `pwd`. You need to reboot.

#if(($bsetup==1)); then
#	echo mcu:2345:respawn:`pwd`/startmcu.sh >>/etc/inittab
#	echo Installation completed at `pwd`.
#	echo Auto startup link created at the end of file /etc/inittab.
#fi

#if(($bsetup==0)); then
#	echo KDV8000E update complete at `pwd`. You need to reboot.
#fi
 
exit
#here
