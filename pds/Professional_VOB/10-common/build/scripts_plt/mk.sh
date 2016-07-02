#!/bin/sh
##########################################################
# Kedacom.
# Linux enviroment Build script
# author: lantianyu@kedacom.com (Tel:8360)
##########################################################
this_file="mk.sh"
this_dir=`pwd` 
export topdir=$(cd ../../../../;pwd)
unset CROSS_COMPILE
export PATH=$PATH:/opt/arm/bin:/opt/montavista/pro/devkit/arm/v5t_le/bin

export output_path=$topdir/compile_output
export release_path=$topdir/Professional_VOB/10-common

target_cpu=(ppc8260 ppc85xx davinci ppc8313)
ppc8260_boards=(mau kdv7810 cri vri kdv8010 mcu mdsc dri16 kdv8010c kdv8010c1 dri apu kdv8005 vpu dec5 dsc dsi kdv8000b)
ppc8313_boards=(kdv7820 hdu hdmpu)
ppc8377_boards=(mpc2 dri2 cri2)
ppc85xx_boards=(hdsc kdv7620)
davinci_boards=(ts5210 ts6610 ts7210 ts5210l ts6610l)

all_boards=(${ppc8260_boards[@]} ${ppc8313_boards[@]} ${ppc85xx_boards[@]} ${davinci_boards[@]} ${ppc8377_boards[@]})
need_boards=( hdu hdmpu dri2 mpc2 cri2) 
################################ Functions ###############################
check()
{
   cd $this_dir || exit 1

    if [ ! -f ./mk_nip.sh ] || [ ! -f ./mk_ios.sh ] || [ ! -f ./mk_platform.sh ] ; then echo "[$this_file]: some shell script missing" && exit 1; fi

    [ ! -x ./mk_nip.sh ] && chmod +x ./mk_nip.sh
    [ ! -x ./mk_ios.sh ] && chmod +x ./mk_ios.sh
    [ ! -x ./mk_platform.sh ] && chmod +x ./mk_platform.sh
    
    if [ -z $1 ]; then echo "[$this_file]: parameter error" && exit 1; fi
} 

build()
{ 		
		case "$1" in
        nip)
            cd $this_dir 
            ./mk_nip.sh $2  
              ;;
        platform)
            cd $this_dir 
             ./mk_platform.sh  
            ;;
        mau|kdv7810|cri|vri|kdv8010|mcu|mdsc|dri16|kdv8010c|kdv8010c1|dri|apu|kdv8005|vpu|dec5|dsc|dsi|kdv8000b|kdv7820|hdu|hdmpu|hdsc|kdv7620|ts5210|ts6610|ts7210|ts5210l|ts6610l|mpc2|dri2|cri2)
            cd $this_dir 
             ./mk_ios.sh $1 
            ;;
        is3-mpc8313)
            ./mk_nip.sh ppc8313 is3-mpc8313
            ./mk_ios.sh is3-mpc8313 
            ;;         
        is3-mpc8548)
            ./mk_nip.sh ppc85xx is3-mpc8548
            ./mk_ios.sh is3-mpc8548
            ;;         
        all)
            cd $this_dir 
            #is3-mpc8548 is3-mpc8313 have there own nip config
            ./mk_nip.sh ppc85xx is3-mpc8548
            ./mk_ios.sh is3-mpc8548
            
            ./mk_nip.sh ppc8313 is3-mpc8313
            ./mk_ios.sh is3-mpc8313  
            	
            for cpu in ${target_cpu[@]}
              do
               ./mk_nip.sh $cpu  
            done	
            	
            for board in ${need_boards[@]}
              do
               ./mk_ios.sh $board  
            done           
            ./mk_platform.sh  #dsplink in the platform need davinci_vmlinux.

            ;;
        *)
            echo "[$this_file]: no $1 target build-rule found"
            echo "[$this_file]: this script can only build: nip, platform, ${all_boards[@]}, and all"
            exit 1
            ;;
    esac
}

################### Main Program Start ##################
echo "####### begin $this_file #######"

begin_time_val=`date +%Y/%m/%d_%H:%M:%S`

##clear nip
rm -rf $output_path/nip
##clear platform
rm -rf $output_path/platform

##check
check $1

##build
build $1 $2 
             
end_time_val=`date +%Y/%m/%d_%H:%M:%S`
echo "[$this_file]: ($1) use time: $begin_time_val-$end_time_val"

echo "####### end $this_file #######"
####### End #######
