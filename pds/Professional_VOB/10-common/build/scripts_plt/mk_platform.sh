#!/bin/sh
##########################################################
# Kedacom.
# Linux enviroment Build script
# author: lantianyu@kedacom.com (Tel:8360)
##########################################################
this_file="mk_platform.sh"
this_dir=`pwd`

unset CROSS_COMPILE
export PATH=$PATH:/opt/ppc/bin:/opt/ppc_nofpu/bin:opt/arm/bin:/opt/montavista/pro/devkit/arm/v5t_le/bin

wapper_dir=$topdir/platform
brdwrapper_dir=$wapper_dir/20-cbb/platform/brdwrapper
nipwrapper_dir=$wapper_dir/20-cbb/platform/nipwrapper
dsplink_dir=$wapper_dir/20-cbb/platform/dvevm/dsplink

#output_path=$topdir/compile_output

#check param and input folder
check()
{
    if [ ! -d $brdwrapper_dir ] || [ ! -d $nipwrapper_dir ] || [ ! -d $dsplink_dir ]
     then
        echo "[$this_file]: platform directory missing." && exit 1
    fi
}

# build all
build()
{
    if cd $brdwrapper_dir ; then
        echo "[$this_file]: Building brdwrapper lib..."
        [ ! -x ./compile_lib ] && chmod +x ./compile_lib
        ./compile_lib
        cd - >/dev/null 2>&1
    fi

    if cd $nipwrapper_dir ; then
        echo "[$this_file]: Building nipwrapper..."
        [ ! -x ./compile_linux ] && chmod +x ./compile_linux
        ./compile_linux
        cd - >/dev/null 2>&1
    fi

    if cd $dsplink_dir ; then
        echo "[$this_file]: Building dsplink..."
        [ ! -x ./compile ] && chmod +x ./compile
        ./compile
        cd - >/dev/null 2>&1
    fi
}

#copy compile result to $output_path/platform
coyp_result()
{
    cd $topdir/platform || exit 1
    
    dst_wrapper=$output_path/platform
    if [ ! -d $dst_wrapper ]; then mkdir -p $dst_wrapper; fi

    mkdir -p $dst_wrapper/10-common/lib/cbb/platform
    mkdir -p $dst_wrapper/10-common/lib/cbb/dvevm
    mkdir -p $dst_wrapper/10-common/version
    cp -rfd 10-common/lib/cbb/platform/* $dst_wrapper/10-common/lib/cbb/platform
    cp -rfd 10-common/lib/cbb/dvevm/* $dst_wrapper/10-common/lib/cbb/dvevm
    cp -rfd 10-common/version/* $dst_wrapper/10-common/version
}

#copy to relative path
copy_to()
{
  source_path=$output_path/platform/10-common/lib/cbb/platform/${4}/linux_${2} 
  release_dir=${release_path}/lib/${4}/linux_${3}/

	if [ ! -d $release_dir ] ; then
			mkdir -p $release_dir;
	fi		
	
	#copy file
	cp  -rf ${source_path}/$1 $release_dir  ;
}

#release to target path
release()
{  		
		##davinci
		copy_to  libnipwrapper.a  arm_d6446   arm_d6446 release
		copy_to  libbrdwrapper.a  arm_d6446   arm_d6446 release
		copy_to  libnipwrapper.a  arm_d6446   arm_d6446 debug
		
		##ppc82xx
		copy_to  libnipwrapper.a 	ppc_82xx	ppc_82xx release
		copy_to  libtaoscodec.a 	ppc_82xx	ppc_82xx release
		copy_to  libdspcci.a 			ppc_82xx	ppc_82xx release
		copy_to  libfpgacodec.a 	ppc_82xx  ppc_82xx release
		copy_to  libbrdwrapper.a 	ppc_82xx	ppc_82xx release
		copy_to  libhpicm.a 			ppc_82xx	ppc_82xx release
		copy_to  libnipwrapper.a 	ppc_82xx	ppc_82xx debug
		copy_to  libhpicm.a 			ppc_82xx	ppc_82xx debug
		
		##ppc8313
		copy_to  lib8313_nipwrapper.a ppc_8313	ppc_82xx 	 release	
		
		##ppc85xx
		copy_to  libnipwrapper.a 	ppc_85xx   ppc_85xx	 release
		copy_to  libbrdwrapper.a 	ppc_85xx   ppc_85xx	 release
}


################### Main Program Start ##################
echo "####### begin $this_file #######"

begin_time_val=`date +%Y/%m/%d_%H:%M:%S`

##check
check

##build
build
echo "[$this_file]: make platform ok."

##copy_to
coyp_result

##release
release

end_time_val=`date +%Y/%m/%d_%H:%M:%S`
echo "[$this_file]: use time: $begin_time_val-$end_time_val"

echo "####### end $this_file #######"
####### End #######
