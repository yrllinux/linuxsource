#!/bin/bash

echo "update 8000H/8000H-M/800L start ..."

# 参数定义
init_6=/etc/init/mcu
init_4=/etc/inittab
log_file=/usr/kdvlog/conf/kdvlog.ini
dst_path=/opt/mcu
# 指定mcu类型的，标准格式　IsMcu8000H-M = 1或0
mcutype_file=/opt/mcutype
cur_path=$(pwd)

# 取mcu类型，没有文件mcutype_file，表示不是8000H-M
mcu_type=$(cat $mcutype_file | grep "IsMcu8000H-M" | awk -F'=' '{print $2}')
Is8khm=$( [[ $mcu_type -eq 1 ]] && echo 1 || echo 0 )

# os版本号
ver=$(lsb_release -r)
ver=${ver:9:1}

# 6.x平台清空$init_6
if [ $ver -ge 6 ]
then
	if [ -d ${init_6} ]
	then
		rm -f ${init_6}/*
	else
		mkdir -p ${init_6}
	fi
fi

# 低于redhat6.0平台的版本，不支持升级8000H-M
if [ $Is8khm -eq 1 -a $ver -lt 6 ]
then
		echo "redhat 4.x/5.x not support 8000H-M, exit 1"
		exit 1
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
	
# 主处理
if [ $Is8khm -eq 1 ]
then # 8000H-M
	if [ $ver -ge 6 ]
	then
		# 移mcu_8khm.conf，并删除/opt/mcu/etc下的conf
		mv ${dst_path}/etc/mcu_8khm.conf  ${init_6}/
		rm -fr ${dst_path}/etc
		rm -f  ${dst_path}/ethdetect.sh
		# move check_mac.sh to /bin add by pengguofeng@2013-8-29
		# check_mac.sh 业务文件，放入/opt/mcu
		#mv ${dst_path}/check_mac.sh  /bin/
		
		# 8000H-M目前仅使用eth1口，直接写死
		echo -n 1 > /opt/ethchoice
	fi
else # 8000H or 800L
	if [ $ver -ge 6 ]
	then
		# 移mcu_8kh.conf，并删除/opt/mcu/etc下的conf
		mv ${dst_path}/etc/mcu_8kh.conf  ${init_6}/
		rm -fr ${dst_path}/etc
		rm -f  ${dst_path}/start_mcu8khm.sh
		rm -f ${dst_path}/add_iflink.sh
		# move check_mac.sh to /bin add by pengguofeng@2013-8-29
		# check_mac.sh 业务文件，放入/opt/mcu
		#mv ${dst_path}/check_mac.sh  /bin/
	else
		# 删除包含/opt/mcu的所有行
		sed -i '/opt\/mcu/d' ${init_4}
		
		# 为兼容发布版本的8000H
		echo stm:2345:once:/opt/mcu/stm.sh  >> ${init_4}
		# 启动业务的脚本
		echo ethd:2345:respawn:/opt/mcu/ethdetect.sh 	>> ${init_4}
		
		# 删除多余文件
		rm -fr ${dst_path}/etc
		rm -f ${dst_path}/add_iflink.sh
		rm -f ${dst_path}/start_mcu8khm.sh
	fi	
fi

# 加载kdvlog.ini，如果系统已有，则不覆盖
if [ -f $log_file ]
then
	echo "$log_file exist, not overwrite it"
else
	echo "mv ${dst_path}/kdvlog.ini  $log_file"
	mv ${dst_path}/kdvlog.ini  $log_file
fi

# 开机链接动态库
cd ${dst_path}
echo > startmcu.sh
chmod a+x startmcu.sh
echo '#!/bin/bash' 			> startmcu.sh
echo 'echo Starting Kedacom MCU...' 	>> startmcu.sh
echo "cd ${dst_path}" 			>> startmcu.sh
echo "ldconfig ${dst_path}" 		>> startmcu.sh

# 升级完成
echo KDV8000H update complete at `pwd`. You need to reboot.
exit 0

#here
