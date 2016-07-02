#!/bin/bash
num=`grep "startmcu.sh" /etc/inittab | wc -l`
if [ "$num" = "0" ]; then
  echo Installing Kedacom MCU, please wait...
else
  echo Entry startmcu already existed in /etc/inittab, installation skipped. If install path changed, please remove the old one mannually and reinstall.
  exit
fi
sed '1,/^#here/d' $0 > mcu.tar.gz
tar zxvf mcu.tar.gz
rm mcu.tar.gz
cd mcu
echo mcu:2345:respawn:`pwd`/startmcu.sh >>/etc/inittab
rm -rf startmcu.sh
touch startmcu.sh
echo '#!/bin/bash' >./startmcu.sh
echo 'echo Starting Kedacom MCU...' >>./startmcu.sh
echo cd `pwd` >>./startmcu.sh
echo ldconfig `pwd` >>./startmcu.sh
#echo './guard' >>./startmcu.sh
echo './mcu_8000e' >>./startmcu.sh
echo './mmp_8000e' >>./startmcu.sh
echo './guard.sh >>./startmcu.sh
chmod a+x startmcu.sh
echo Installation completed at `pwd`.
echo Auto startup link created at the end of file /etc/inittab.
exit
#here
