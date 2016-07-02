#!/bin/bash

if [ ! -d /opt/mcu/ftp ]
then
	mkdir /opt/mcu/ftp
fi

chown kedacomvc /user/mcu/log
chown kedacomvc /user/mcu/tmp
chown kedacomvc /opt/mcu/ftp

chmod u+w /opt/mcu/ftp
chmod u+x /opt/mcu/ftp
chmod u+r /opt/mcu/ftp

chmod u+w /usr/mcu/tmp

chmod u-w /usr/mcu/log
chmod u-x /usr/mcu/log

exit 1

