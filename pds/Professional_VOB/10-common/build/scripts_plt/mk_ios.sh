#!/bin/sh
##########################################################
# Kedacom.
# Linux enviroment Build script
# author: lantianyu@kedacom.com (Tel:8360)
##########################################################
this_file="mk_ios.sh"
this_dir=`pwd`

unset CROSS_COMPILE
export PATH=$PATH:/opt/ppc/bin:/opt/ppc_nofpu/bin:opt/arm/bin:/opt/montavista/pro/devkit/arm/v5t_le/bin

ppc8260_boards=(mau kdv7810 cri vri kdv8010 mcu mdsc dri16 kdv8010c kdv8010c1 dri apu kdv8005 vpu dec5 dsc dsi kdv8000b)
ppc8313_boards=(kdv7820 hdu hdmpu is3-mpc8313)
ppc85xx_boards=(hdsc kdv7620 is3-mpc8548)
ppc8377_boards=(mpc2 dri2 cri2)
davinci_boards=(ts5210 ts6610 ts7210 ts5210l ts6610l)
all_boards=(${ppc8260_boards[@]} ${ppc8313_boards[@]} ${ppc85xx_boards[@]} ${davinci_boards[@]} ${ppc8377_boards[@]})

input_path=$topdir/kernel
#output_path=$topdir/compile_output

#check param and input folder
check()
{
    cd $topdir || exit 1

    #check parameter
    check_ok=0
    for board in ${all_boards[@]}
      do
      if [ "$1" = $board ]; then check_ok=1; fi
    done
    if [ "$check_ok" -ne "1" ]; then echo "[$this_file]: (\$1=$1) parameter error" && exit 1; fi
    
    #check directory
    if [ ! -d $input_path/linux-2.6.10 ] || [ ! -d $input_path/linux-2.6.10_mvl401 ]; then echo "[$this_file]: kernel directory missing" && exit 1; fi
}

#set environment param
set_env()
{
    case "$1" in    
		   8313|8260|8377)
	    	unset CROSS_COMPILE
	    	export CROSS_COMPILE=ppc_82xx-
	    ;;
			 85xx)
	    	unset CROSS_COMPILE
	    	export CROSS_COMPILE=ppc_85xx-
	    ;;
     	davinci)
       	unset CROSS_COMPILE
        export CROSS_COMPILE=arm_v5t_le-
     ;;
    esac
    
    if [ "$cpu_platform" = "davinci" ] ; then
				  full_cpu_name="$cpu_platform" 
		elif [ "$cpu_platform" = "8377" ] ; then
					full_cpu_name=ppc8313 
		else
					full_cpu_name="ppc$cpu_platform";
		fi
		
		if [ "$cpu_platform" = "davinci" ] ; then
				  nip_name="$cpu_platform"
		elif [  "$cpu_platform" = "8260" ] ; then
				  nip_name=ppc_82xx
		elif [ "$cpu_platform" = "8377" ] ; then
				  nip_name=ppc_8313 		  
		else
					nip_name="ppc_$cpu_platform";
		fi
}

# build kernel
build()
{
    cd $topdir || exit 1
    
    #need nip
    case "$1" in
        ts5210|ts6610|ts7210|ts5210l|ts6610l)
           cpu_platform=davinci
           arch_style=arm
  				;;      
        mau|kdv7810|cri|vri|kdv8010|mcu|mdsc|dri16|kdv8010c|kdv8010c1|dri|apu|kdv8005|vpu|dec5|dsc|dsi|kdv8000b)
            cpu_platform=8260
            arch_style=ppc
           ;;
        kdv7820|hdu|hdmpu|is3-mpc8313)
        		cpu_platform=8313
        		arch_style=ppc
     				;;
     		hdsc|kdv7620|is3-mpc8548)
     				cpu_platform=85xx
     				arch_style=ppc
     				;;
     		mpc2|dri2|cri2)
     				cpu_platform=8377
     				arch_style=ppc
     				;;
    esac
    
    #set_env
    set_env $cpu_platform
    
    #compile nip
    if [ ! -d $output_path/nip/$full_cpu_name/ ]
       then
   		 				cd $this_dir || exit 1;
  						./mk_nip.sh $full_cpu_name $1 || exit 1 
    fi
    
    if [ "$cpu_platform" != "davinci" ]; then
        kernel_src_top=$input_path/linux-2.6.10
        kernel_build_top=$input_path/build/$1
    else
        kernel_src_top=$input_path/linux-2.6.10_mvl401
        kernel_build_top=$input_path/linux-2.6.10_mvl401
        cd $kernel_build_top || exit 1
        make clean
    fi
    
    #first time make
    if [ ! -d $kernel_build_top ] && [ "$cpu_platform" != "davinci" ]; then
    		mkdir -p $kernel_build_top;
    		cd $kernel_src_top;
				make mrproper;
    		make ARCH=$arch_style menuconfig O=$kernel_build_top;
     fi
    
    if [ ! -d $kernel_build_top/include/linux ]; then
          mkdir -p $kernel_build_top/include/linux;
    fi

    # copy new kernel config files
    if [ "$cpu_platform" != "davinci" ]; then
        cp -f $kernel_src_top/include/bconfigs/${cpu_platform}/$1/.config $kernel_build_top
        cp -f $kernel_src_top/include/bconfigs/${cpu_platform}/$1/include/linux/autoconf.h $kernel_build_top/include/linux/autoconf.h
    else
        cp -f $kernel_src_top/include/bconfigs/${cpu_platform}/$1/.config $kernel_build_top
        cp -f $kernel_src_top/include/bconfigs/${cpu_platform}/$1/include/linux/autoconf.h $kernel_build_top/include/linux/autoconf.h
    fi
		
    #compile
    cd $kernel_build_top 
   	
    make ARCH=$arch_style uImage || exit 1
    make ARCH=$arch_style modules || exit 1
    make ARCH=$arch_style modules_install || exit 1 
}

mk_ios()
{		
		#init param
	  order=b			
		target_nfs=$output_path/kernel/$cpu_platform/nfs
		source_nfs=$topdir/source/standard/$full_cpu_name/nfs
    version_path=$output_path/kernel/$cpu_platform/$1
		
	 if [ ! -d $version_path ] ; then
    		mkdir -p $version_path ;
   fi
   
   ##prepare linux.jffs2
    #copying standard nfs  
  	if [ -d $target_nfs ] ; then
  			rm -rf $target_nfs;			
  	fi			
   
    mkdir -p $output_path/kernel/$cpu_platform/nfs
    sudo cp -rfd $source_nfs/* $target_nfs
    sudo chown -R $LOGNAME $target_nfs
    sudo chgrp -R $LOGNAME $target_nfs 
        
    #seclect inittab with board style   
     case "$1" in
				kdv7810|mau)
							cp $topdir/kernel/linux-2.6.10/include/bconfigs/8260/$1/inittab $target_nfs/etc
							;;
				hdu|kdv7820)     
	            cp $topdir/kernel/linux-2.6.10/include/bconfigs/8313/$1/inittab $target_nfs/etc
							;;
				ts7210|ts5210l|ts7210|ts5210l|ts5210|ts6610)
              cp $topdir/kernel/linux-2.6.10_mvl401/include/bconfigs/davinci/$1/inittab $target_nfs/etc
    					;;
    	esac
    
    #copy kernel modules to file system 
    if [ -d $output_path/modules ] ; then
     		mv -f $output_path/modules  $target_nfs/;
 	  else
 	  		mkdir -p $target_nfs/modules;	
 	  fi
    
    #copy necessary files or lib to file system	
    cp -rf $kernel_src_top/include/bconfigs/$cpu_platform/$1/modules.sh $target_nfs/modules/
		cp -rf $kernel_src_top/include/bconfigs/$cpu_platform/$1/include/linux/autoconf.h $target_nfs/modules/
	
		if [ -f $kernel_src_top/include/bconfigs/$cpu_platform/$1/UBLFile.txt ]; then
				 cp -f $kernel_src_top/include/bconfigs/$cpu_platform/$1/UBLFile.txt $target_nfs/modules/;
		fi
		
		cp -rfd $output_path/nip/${full_cpu_name}/${nip_name}_userCmd $target_nfs/bin/userCmd
    cp -rfd $output_path/nip/${full_cpu_name}/${nip_name}_nip $target_nfs/bin/nip					
    cp -rfd $output_path/nip/${full_cpu_name}/lib${nip_name}_snmp*.so $target_nfs/lib/		
    cp -rfd $output_path/nip/${full_cpu_name}/${nip_name}_nip.sym $target_nfs/bin/nip.sym 
    cp -rfd $output_path/nip/${full_cpu_name}/tc $target_nfs/bin/
        
    rm -rf $target_nfs/etc/mibs 
    cp -rfd $topdir/nip/source/snmp/mibs $target_nfs/etc/ ||exit 1
    
    if [ ! -d $version_path/modules ] ; then
    		mkdir $version_path/modules ||exit 1
    fi
    
    #Back up modules content
    rm -rf $version_path/modules/* ||exit 1
    cp -f $target_nfs/modules/*  $version_path/modules/ ||exit 1
     
    #link necessary command
    cd  $target_nfs/bin
        if [ -e userCmd ]
            then
            if [ -f bootupdate ]; then rm bootupdate &> /dev/null; fi
            if [ -f ffdisk ]; then rm -fr ffdisk &> /dev/null; fi
            if [ -f fformat ]; then rm -fr fformat &> /dev/null; fi
            rm -fr fpgaupdate &> /dev/null
            rm -fr iosupdate &> /dev/null
            rm -fr mountjffs2 &> /dev/null
            rm -fr appupdate &> /dev/null
            rm -fr nipmonitor &> /dev/null
            rm -fr loadinfo &> /dev/null
           
            rm -fr initmount &> /dev/null
            rm -fr nandupdate &> /dev/null

            rm -fr allupdate &> /dev/null
            rm -fr hwinfo &> /dev/null
            
            ln -s userCmd allupdate
            ln -s userCmd hwinfo
            if [ "$cpu_platform" = "davinci" ] || [ "$cpu_platform" = "8313" ]|| [ "$cpu_platform" = "8377" ] || [ "$1" = "is3-mpc8548" ]; then
                rm -fr sysidx &> /dev/null
                ln -s userCmd initmount
                ln -s userCmd sysidx
              
            else
        #       ln -s userCmd bootupdate
        #       ln -s userCmd ffdisk
        #       ln -s userCmd fformat
                ln -s userCmd fpgaupdate
                ln -s userCmd iosupdate
                ln -s userCmd mountjffs2
                ln -s userCmd appupdate
                ln -s userCmd nipmonitor
                ln -s userCmd loadinfo
            fi
    fi
    
    
    ## make linux.jffs2
    if [ $cpu_platform = "davinci" ] ; then
    			order=l;
    fi
    
		$topdir/source/tools/mkfs.jffs2 -e 0x2000 -${order} -p --disable-compressor=rtime --compressor-priority=60:zlib -r $target_nfs -o $version_path/linux.jffs2 || exit 1

		if [ $cpu_platform = "davinci" ] ; then
			cp -f $kernel_build_top/arch/${arch_style}/boot/uImage $version_path/linux.ios ;
		else	
			cp -f $kernel_build_top/arch/${arch_style}/boot/images/uImage $version_path/linux.ios ;
		fi
		
		cd $version_path
		
	  if [ "$cpu_platform" = "davinci" ]; then
	  	mkimage -A arm -O linux -T ramdisk -C none -a 0x80200000 -e 0x80200000 -n "ramdisk image" -d linux.jffs2 ramdisk.jffs2 && mv ramdisk.jffs2 linux.jffs2
	  fi		  
		
	  if [ "$cpu_platform" = "8313" ] || [ "$cpu_platform" = "8377" ] || [ "$1" = "is3-mpc8548" ];  then
	  	mkimage -A ppc -O linux -T ramdisk -C none -a 0x0200000 -e 0x0200000 -n "ramdisk image" -d linux.jffs2 ramdisk.jffs2 && mv ramdisk.jffs2 linux.jffs2 -f
	  fi

		#create update.linux
	  $topdir/source/tools/updata linux.ios linux.jffs2 update.linux
	  chmod a+r update.linux
	  
		cd $version_path/../
	  tar -cf $1/$1_nfs.tar nfs/
 
   	rm -rf $target_nfs
}


copy_to()
{
   release_dir=${release_path}/version/${3}/os_linux/${2}
   
	#check folder
	if [ ! -d $release_dir ] ;then 
				mkdir -p $release_dir;
	fi 
	
	#copy file
	cp  -rf $1 $release_dir;
}
release()
{
		case $1 in
			kdv7810)
						copy_to $version_path/\* mt/7810 $2
						copy_to $version_path/\* mt/7910 $2
						copy_to $version_path/\* vpuhd $2
						;;
			kdv7820)
						copy_to $version_path/\* mt/7820 $2
						;;
			kdv7620)
						copy_to $version_path/\* mt/7620 $2
						;;
			ts7210)
						copy_to $version_path/\* mt/7210 $2
						copy_to $version_path/\* mt/7610 $2
						;;
			mau)
						copy_to $version_path/\* baphd $2
						;;
			hdmpu)
						copy_to $version_path/\* mpu $2
						;;
			hdu)
						copy_to $version_path/\* $1 $2
						;;
			is3-mpc8313)
						copy_to $version_path/\* $1 $2
						;;
			is3-mpc8548)
						copy_to $version_path/\* $1 $2
						;;
			ts6610)
						copy_to $version_path/\* mt/6610 $2
						;;
			ts5210l)
						copy_to $version_path/\* mt/5610 $2
						;;
			mpc2)
						copy_to $version_path/update.linux $1 $2
						;;
			dri2)
						copy_to $version_path/update.linux $1 $2
						;;
		  	cri2)
						copy_to $version_path/update.linux $1 $2
						;;	
			*)
					  #echo It is not required to copy!
						;;
   	esac   
}


################### Main Program Start ##################
echo "####### begin $this_file $1#######"

begin_time_val=`date +%Y/%m/%d_%H:%M:%S`

##check
check $1

##build
build $1
echo "[$this_file]: make $1 uImage ok."

##mk_ios
mk_ios $1
echo "[$this_file]: make $1 linux.update ok."

##release
release $1 release

end_time_val=`date +%Y/%m/%d_%H:%M:%S`
echo "[$this_file]: ($1) use time: $begin_time_val-$end_time_val"

echo "####### end $this_file $1#######"
####### End #######
