#!/bin/bash


TMP_FILE="/opt/tmpinittab"

LINENUM=`cat /etc/inittab | awk '/stm/ {print NR-1 ; exit 0; }'`
if [ "$LINENUM" = "" ]; then
	echo "first update, just add program to end of the file"
else
	cat /etc/inittab | head -n $LINENUM > $TMP_FILE
	cat $TMP_FILE > /etc/inittab
fi

echo stm:2345:once:/opt/mcu/startmcu.sh >>/etc/inittab
#echo netc:2345:once:/opt/mcu/netcfg.sh >> /etc/inittab
echo snmp:2345:respawn:/opt/mcu/snmpd >> /etc/inittab
echo gad:2345:once:/opt/mcu/guard.sh >> /etc/inittab
#echo mmp:2345:respawn:/opt/mcu/mmp_8000e >> /etc/inittab
#echo prs:2345:respawn:/opt/mcu/prs_8000e >> /etc/inittab
#echo mcue:2345:respawn:/opt/mcu/mcu_8000e >> /etc/inittab
echo ethd:2345:respawn:/opt/mcu/ethdetect.sh >> /etc/inittab
#echo gk:2345:respawn:/opt/mcu/kdvgk_redhat >> /etc/inittab
#echo pxy:2345:respawn:/opt/mcu/pxyserverapp_redhat >> /etc/inittab
echo logs:2345:respawn:/opt/mcu/kdvlogserver >> /etc/inittab

sed '1,/^#here/d' $0 > mcu.tar.gz

oldpath=`pwd`
cd ../..
tar zxvf $oldpath/mcu.tar.gz
rm -rf $oldpath/mcu.tar.gz
cd $oldpath/..

rm -rf startmcu.sh
touch startmcu.sh
chmod a+x startmcu.sh
echo '#!/bin/bash' >./startmcu.sh
echo 'echo Starting Kedacom MCU...' >>./startmcu.sh
echo cd `pwd` >>./startmcu.sh
echo ldconfig `pwd` >>./startmcu.sh
#echo './mcu_8000e &' >>./startmcu.sh
#echo './mmp_8000e' >>./startmcu.sh
#echo './guard.sh' >>./startmcu.sh

echo KDV8000E update complete at `pwd`. You need to reboot.

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
