#!/bin/bash

#8kiupdatefile

echo "update 8000I start ..."

# 参数定义
init_6=/etc/init/mcu
log_file=/usr/kdvlog/conf/kdvlog.ini
dst_path=/opt/mcu

cur_path=$(pwd)


# 清空$init_6

if [ -d ${init_6} ]
then
	rm -f ${init_6}/*
else
	mkdir -p ${init_6}
fi



# 没有/opt/mcu则需要创建
if [ ! -d ${dst_path} ]
then
	mkdir -p ${dst_path}
fi


# 解压文件
sed '1,/^#here/d' $0 > mcu.tar.gz
tar zxvf ${cur_path}/mcu.tar.gz -C /opt
rm -rf ${cur_path}/mcu.tar.gz
	

# 移mcu_8ki.conf，并删除/opt/mcu/etc下的conf
mv ${dst_path}/etc/mcu_8ki.conf  ${init_6}/
rm -fr ${dst_path}/etc
# rm -f  ${dst_path}/start_mcu8khm.sh
rm -f ${dst_path}/add_iflink.sh


# 加载kdvlog.ini，如果系统已有，则删除
if [ -f $log_file ]
then
	echo "$log_file exist, remove it"
	rm -rf $log_file
fi

echo "mv ${dst_path}/kdvlog.ini  $log_file"
mv ${dst_path}/kdvlog.ini  $log_file


# 开机链接动态库
cd ${dst_path}
echo > startmcu.sh
chmod a+x startmcu.sh
echo '#!/bin/bash' 			> startmcu.sh
echo 'echo Starting Kedacom MCU...' 	>> startmcu.sh
echo "cd ${dst_path}" 			>> startmcu.sh
echo "ldconfig ${dst_path}" 		>> startmcu.sh

# 升级完成
echo KDV8000I update complete at `pwd`. You need to reboot.
exit 0

#here
