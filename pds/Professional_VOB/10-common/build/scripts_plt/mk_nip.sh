#!/bin/sh
##########################################################
# Kedacom.
# Linux enviroment Build script
# author: lantianyu@kedacom.com (Tel:8360)
##########################################################

#########################################################
# Usage:
#    mk_nip.sh 
#########################################################
this_file="mk_nip.sh"
this_dir=`pwd`

unset CROSS_COMPILE
export PATH=$PATH:/opt/ppc/bin:/opt/ppc_nofpu/bin:opt/arm/bin:/opt/montavista/pro/devkit/arm/v5t_le/bin

nip_platform=(ppc8260 ppc85xx davinci ppc8313 )
input_path=$topdir/nip
output_path=$topdir/compile_output

#check param and input folder
check()
{
    cd $topdir || exit 1
		
		#check parameter
    check_ok=0
    for style in ${nip_platform[@]}
    	do
    			if [ "$style" = "$1" ];then check_ok=1;fi
   		done
    	
   if [ "$check_ok" -ne "1" ]; then echo "[$this_file]: (\$1=$1) parameter error" && exit 1; fi
   
    #check directory
    if [ ! -d $input_path/source ]; then echo "[$this_file]: nip directory missing" && exit 1; fi
}

#set environment param
set_env()
{	
		 case "$1" in
		 		ppc8260)
	    	unset CROSS_COMPILE
	    	export CROSS_COMPILE=ppc_82xx-
	    	cpu_platform=8260
	    ;;
  		  ppc8313)
	    	unset CROSS_COMPILE
	    	export CROSS_COMPILE=ppc_82xx-
	    	cpu_platform=8313
	    ;;
			 ppc85xx)
			 unset CROSS_COMPILE
	     export CROSS_COMPILE=ppc_85xx-
	     cpu_platform=85xx
	    ;;
     	 davinci)
       unset CROSS_COMPILE
       export CROSS_COMPILE=arm_v5t_le-
       cpu_platform=davinci
     	;;
    esac
}

# build, and copy the result to $output_path/nip/
build()
{	
    cd $input_path/source || exit 1

		if [ "$1" = "ppc8260" ] ; then
    	release_name=ppc_82xx;
    else
    	release_name=$( echo $1 | sed 's/ppc/ppc_/' );
    fi

		##set cross_compile
 		set_env $1
	
		rm -rf $input_path/obj/*
	
 		case "$1" in
		 		ppc85xx)
		 				if [ "$2" == "is3-mpc8548" ] ; then
		 					make clean 	 			
							CFLAGS="-msoft-float -pipe -ffixed-r2 -mmultiple -Wa,-me500" make board=mpc8548 || exit 1
	  				else	
		 					make clean 	 			
							CFLAGS="-msoft-float -pipe -ffixed-r2 -mmultiple -Wa,-me500" make || exit 1
	  				fi 
				 ;;
		 		davinci)
		 				make board=davinci clean || exit 1
						make board=davinci  || exit 1
				 ;;		
				ppc8313) 
				    if [ "$2" == "is3-mpc8313" ] ; then
				    	make board=is3  clean || exit 1
							make board=is3  || exit 1
	  				else	
       				make board=mpc8313e  clean || exit 1
							make board=mpc8313e  || exit 1
	  				fi 
				;;			 
				ppc8260)
					 make  clean|| exit 1
					 make  || exit 1
			 ;;
		esac
		
		   
    #copy_to
		cd $topdir
    
    if [ ! -d  $output_path/nip/$1 ] ; then 
    	mkdir -p $output_path/nip/$1 || exit 1; 
    fi

	 rm -rf  $output_path/nip/$1/*
   cp -rf $input_path/obj/* $output_path/nip/$1;
   rm -rf $input_path/obj/*
    
    #build nipdebug
    cd $topdir/app/nipdebug
    
    make S=1
    
    #copy_to
		cd $topdir
		
		#check directory
    if [ ! -d  $output_path/nipdebug/$1 ] ; then 
    	mkdir -p $output_path/nipdebug/$1 || exit 1; 
    fi
    
    if [ "$1" != "ppc8313" ] ; then
       cp -rfd app/nipdebug/obj/$release_name/* $output_path/nipdebug/$1;    
	  else	
       cp -rfd app/nipdebug/obj/ppc_82xx/* $output_path/nipdebug/$1; 
	  fi 	
	  build_tc $1
}

#copy function
copy_to()
{
  release_dir=${release_path}/lib/${4}/linux_${3}/${2}
	release_folder=$( echo $release_dir | sed 's/[0-9a-zA-Z_]*\.[a-zA-Z0-9]*$//' )

	if [ ! -d $release_folder ] ; then
			mkdir -p $release_folder;
	fi		
	
	#copy file
	cp  -rf $1 $release_dir;
}

# release to target foldr
release()
{	
		case $1 in
			davinci)
					copy_to $output_path/nip/$1/lib${release_name}_snmpagent.a 	.  arm_d6446  $2
					copy_to $output_path/nip/$1/lib${release_name}_snmp.a  			.  arm_d6446  $2	
					copy_to $output_path/nip/$1/lib${release_name}_nip_api.a 		.  arm_d6446  $2
					copy_to $output_path/nip/$1/lib${release_name}_api.a  			.	 arm_d6446  $2	
					copy_to $output_path/nip/$1/lib${release_name}_dataswitch.a  			.	 arm_d6446  $2	
					copy_to $output_path/nipdebug/$1/\* nipdebug arm_d6446  $2	
			;;
			ppc8260)
					copy_to $output_path/nip/$1/lib${release_name}_snmpagent.a nip/lib${release_name}_snmpagent_12.a 		$release_name		$2
					copy_to $output_path/nip/$1/lib${release_name}_snmp.a nip/lib${release_name}_snmp_12.a  						$release_name		$2
					copy_to $output_path/nip/$1/lib${release_name}_nip_api.a nip/lib${release_name}_nip_api_12.a 				$release_name		$2
					copy_to $output_path/nip/$1/lib${release_name}_dataswitch.a lib${release_name}_dataswitch_12.a 			$release_name		$2
					copy_to $output_path/nipdebug/$1/\* nipdebug $release_name		$2
			;;
			ppc8313)
					copy_to $output_path/nip/$1/lib${release_name}_snmpagent\* 	.  ppc_82xx  $2
					copy_to $output_path/nip/$1/lib${release_name}_snmp\*   		.  ppc_82xx	 $2	
					copy_to $output_path/nip/$1/lib${release_name}_nip_api.a 		.  ppc_82xx  $2
					copy_to $output_path/nip/$1/lib${release_name}_api.a  			.	 ppc_82xx	 $2	
					copy_to $output_path/nip/$1/lib${release_name}_dataswitch.a .  ppc_82xx  $2
					copy_to $output_path/nipdebug/$1/\* nipdebug  ppc_82xx  $2
			;;
			ppc85xx)
					 	if [ "$3" == "is3-mpc8548" ] ; then
				  		cp $output_path/nip/$1/lib${release_name}_snmpagent.a 	$output_path/nip/$1/lib${release_name}_nand_snmpagent.a
							cp $output_path/nip/$1/lib${release_name}_snmp.a  $output_path/nip/$1/lib${release_name}_nand_snmp.a  
							cp $output_path/nip/$1/lib${release_name}_nip_api.a	$output_path/nip/$1/lib${release_name}_nand_nip_api.a
							cp $output_path/nip/$1/lib${release_name}_api.a  	$output_path/nip/$1/lib${release_name}_nand_api.a
							
							copy_to $output_path/nip/$1/lib${release_name}_nand_snmpagent.a 	.  $release_name  $2
							copy_to $output_path/nip/$1/lib${release_name}_nand_snmp.a  			.  $release_name	$2	
							copy_to $output_path/nip/$1/lib${release_name}_nand_nip_api.a 		.  $release_name  $2
							copy_to $output_path/nip/$1/lib${release_name}_nand_api.a  			.	 $release_name  $2	
							#copy $output_path/nip/$1/lib${release_name}_dataswitch.a
							copy_to $output_path/nipdebug/$1/\* nipdebug $release_name  $2	
	  				else	
				  		copy_to $output_path/nip/$1/lib${release_name}_snmpagent.a 	.  $release_name  $2
							copy_to $output_path/nip/$1/lib${release_name}_snmp.a  			.  $release_name	$2	
							copy_to $output_path/nip/$1/lib${release_name}_nip_api.a 		.  $release_name  $2
							copy_to $output_path/nip/$1/lib${release_name}_api.a  			.	 $release_name  $2	
							#copy_to $output_path/nip/$1/lib${release_name}_dataswitch.a .  $release_name  $2
							copy_to $output_path/nipdebug/$1/\* nipdebug $release_name  $2	
	  				fi 

			;;
		esac
}


build_tc()
{
	cd $topdir/app/iproute2 
	
	./compile.sh 
	
	cp tc/tc $output_path/nip/$1 -f
}

################### Main Program Start ##################
echo "####### begin $this_file $1#######"

begin_time_val=`date +%Y/%m/%d_%H:%M:%S`

##check
check $1 

build $1 $2

release $1 release $2
release $1 debug	$2

end_time_val=`date +%Y/%m/%d_%H:%M:%S`
echo "[$this_file]: ($1 $2) use time: $begin_time_val-$end_time_val"

echo "####### end $this_file $1#######"
####### End #######
