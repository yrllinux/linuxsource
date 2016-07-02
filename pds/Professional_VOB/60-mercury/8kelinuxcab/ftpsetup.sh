#!/bin/bash
filepath="/opt/mcu/tool/rpminstalled";
filename="vsftpd-2.0.5-16.el5.i386.rpm";

chown admin /opt/mcu/ftp
chown admin /opt/mcu/conf
chown admin /opt/mcu/webroot

if [ ! -f  $filepath ]; then
        cd /opt/mcu/tool
        if [ ! -f $filename ]; then
                rpm -ivh vsftpd-2.0.5-16.el5.i386.rpm
                rpm -qi vsftpd
                service vsftpd start
                chkconfig --level 3 vsftpd on
                adduser -d /opt/mcu/ftp -g ftp -s /sbin/nologin admin
                echo admin |passwd --stdin admin
                touch rpminstalled
        fi
fi

exit 1

