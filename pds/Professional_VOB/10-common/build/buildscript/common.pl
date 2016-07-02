#!usr/bin/perl
# ======================================================================
# ABC ABC
# Perl Source File -- Created with SAPIEN Technologies PrimalScript 3.1
#
# NAME: common.pl
#
# AUTHOR: pengjing , chenhuiren
# DATE  : 2007-7-26 ~ 2007-9-27
#
# PURPOSE: Global Var Set , Not include module build process , Require "version" input
#
# ======================================================================
# log file define -----
# BuildError.log
# BuildInfo.txt
# checkfile.log
# checkreadme.log
# notify.log
#
# define variable ab. -----
# P = path
# N = name
# F = file
# D = dir
# R = release
# S = result
# L = log
# M = mail
# W = Whole ( Whole Path )
# CF = config file
# IF = info file
# RM = readme config
# RRM = release readme
# CRM = check readme
# PM = Product Manager
# TE = Test Engineer
# prj = project
# ver = version
# modu = module
# err = error
# 
# special global variable define -----
# $OS : Operation System ( 1 = linux / 0 = windows )
# $OSstring : Operation System System ( "linux" / "windows" )
# $builddatetime : Current yymmdd-hhmm
# $builddate : Current yymmdd
# $workpath : Script work path
# $version : Specific Project Version Name
# $verworkP :
# $vererrL :
# $place : $module : 
# $module : 
# BUILDINFO : BuildInfo.txt
# BUILDERROR : BuildError.log
# $UCMprj :
# $versionCFN :
# $commoncompP :
# $compileIFP :
# $moduleCFP :
# $checkreadme :
# $releasekreadme :
# $PMM :
# $CMOM :
# $CMOM :
# $SMTP :
# $SMTP :
# $mailfrom :
# $mailfrom :
# $sendmail :
# $connect_server :
# $connect_user :
# $connect_passwd : 
# $DynviewN :
# $SnapviewP :
# $ReleaseP :
# $TEM :
# =====================================================================================================================
# 调用common.pl之前,应当先获取本次编译的项目版本号
# 获取当前操作系统
if ( $ENV{'OS'} =~ /windows/i )
{
	$OS = 0; # Windows操作系统
	$OSstring = "Windows";
}
else
{
	$OS = 1; # Unix操作系统
	$OSstring = "Linux";
}
# 获取当前时间和工作路径
$builddatetime = &getdatetime;
$builddate = &getdate;
# 获取当前工作路径
if ( $workpath eq "" )
{
	$workpath = &getcurrpath;
}
else
{
	$workpath = &revisepath(1,$workpath);
}
# =====================================================================================================================
# 脚本所在目录下的以版本号命名的文件夹不存在则创建
if ( !-e $workpath.$version && !mkdir($workpath.$version) )
{
	$place = "Make_Version_Dir";
	print "Failed Make Dir '$workpath$version' !\n";
	&command("pause");
	exit 1; # 创建版本号命名的文件夹失败则退出整个程序
}
$verworkP = &revisepath(1,$workpath.$version); # 修正工作路径下的版本路径为可用格式 , 用于直接与log文件连接
# 在版本号命名的文件夹下创建以脚本运行时间日期命名的文件夹
if ( !-e $verworkP.$builddatetime && !mkdir($verworkP.$builddatetime) )
{
	$place = "Make_Version_Datetime_Dir";
	print "Failed Make Dir '$verworkP$builddatetime' !\n";
	&command("pause");
	exit 1; # 创建日期时间命名的文件夹失败则退出整个程序
}
$verworkP = &revisepath(1,$verworkP.$builddatetime); # 修正工作路径下的版本路径为可用格式 , 用于直接与log文件连接
# 打开编译错误信息日志BuildError.log准备写入
if ( !open(BUILDERROR,">>$verworkP"."BuildError.log") )
{
	print "Error : Failed Open File '$verworkP"."BuildError.log' !\n";
	&command("pause");
	exit 1; # 打不开日志文件则退出整个程序
}
&printerror(1,"OS : $OSstring\nAutoBuild Script Path : $workpath\nAutoBuild Time : $builddatetime\n");
$place = ""; # 用于记录子程序名
$module = ""; # 用于记录模块名
$vererrL = $verworkP."error.log"; # 由于error.log多用于记录系统命令的错误信息，需便于直接调用该路径
if ( $OS )
{
	my $info = `unalias cp`;
	my $str = "Unalias cp Successfully !\n";
	$str = "Unalias cp info : $info\n" if ( $info != /^\s*$/);
	&printerror(1,$str);
}
# =====================================================================================================================
# 打开项目配置文件main.ini，读取所有配置信息
$place = "Get_Project_Config";
print "\n$module - $place......\n";
if ( !open(PROJECT,$workpath."main.ini") )
{
	&printerror(0,"Failed Open File '$workpath"."main.ini' !\n");
	&command("pause");
	exit 1; # 打不开项目配置文件main.ini则退出整个程序
}
local @prjfile = <PROJECT>;
close(PROJECT);
chomp(@prjfile);
# 读取main.ini中静态信息
$BuildServer = &getprjconf("hostname");
$proline = &getprjconf("productline");
$UCMprj = &getprjconf("UCMproject"); # 读取当前编译项目
#print "zhangtingting:[$UCMprj]\n";

$compileinfo_p = &getprjconf("compileinfo_path");
$compileinfo_p = &revisepath(1,$compileinfo_p);



&printerror(1,"Build Server : $BuildServer\nUCMproject : $UCMprj\n");
$commoncompP = &getprjconf("commoncomp_path");
$compileIFP = &getprjconf("compileinfo_path");
$moduleCFP = &getprjconf("moduleconf_path");
my $linux_ismount = &getprjconf("linux_ismount");
$ISM = 0; # 将用户填写的checkreadme信息转为bool值供后续判断
$ISM = 1 if (( $linux_ismount eq "" ) || ( $linux_ismount eq "yes" ) || ( $linux_ismount eq "y" )); # 仅当填写值为空或不填或等于"yes"时才为真
undef($linux_ismount);
undef(@prjfile);
# 校验项目配置文件名
if ( $UCMprj eq "" )
{
	&printerror(0,"Failed Found Key Value 'UCMproject' in '$workpath"."main.ini' !\n");
	&command("pause");
	exit 1; # 获取到的关键信息为空则退出整个程序
}
# 校验关键通用配置信息
if ( $commoncompP eq "" )
{
	&printerror(0,"Failed Found Key Value 'commoncomp_path' in '$workpath"."main.ini' !\n");
	&command("pause");
	exit 1; # 获取到的关键信息为空则退出整个程序
}
# 校验关键通用配置信息
if ( $compileIFP eq "" )
{
	&printerror(0,"Failed Found Key Value 'compileinfo_path' in '$workpath"."main.ini' !\n");
	&command("pause");
	exit 1; # 获取到的关键信息为空则退出整个程序
}
# 校验关键通用配置信息
if ( $moduleCFP eq "" )
{
	&printerror(0,"Failed Found Key Value 'moduleconf_path' in '$workpath"."main.ini' !\n");
	&command("pause");
	exit 1; # 获取到的关键信息为空则退出整个程序
}
# 将用户填写的路径修正为可用格式
$commoncompP = &revisepath(0,$commoncompP);
$compileIFP = &revisepath(0,$compileIFP);
$moduleCFP = &revisepath(0,$moduleCFP);
# 打开项目版本配置文件，读取所有配置信息
$place = "Get_Version_Config";
print "\n$module - $place......\n";

print "workpath:[$workpath]\n";
print "UCMprj.ini:[$workpath.$UCMprj.ini]\n";
my $temp_path = $workpath."$UCMprj.ini";
print "temp_path:[$temp_path]\n";

#$workpath."$UCMprj.ini";

if ( !open(VERSION,$temp_path) )
{
	
	&printerror(0,"Failed Open file '$workpath$UCMprj".".ini' !\n");
	&command("pause");
	exit 1; # 打不开项目版本配置文件则退出整个程序
}else{
  #print "hhiiiiii.......\n";

}
@verfile = <VERSION>;
close(VERSION);
chomp(@verfile);
# 读取project.ini中静态信息
if ( $OS )
{
	@pv = &getline(0,"l",@verfile); # 查找编译平台行
	@pv = &shiftvalue("l",@pv); # 去除编译平台打头的值和空格
}
else
{
	@pv = &getline(0,"w",@verfile); # 查找编译平台行
	@pv = &shiftvalue("w",@pv); # 去除编译平台打头的值和空格
}
undef(@verfile);
@v = &getline(0,$version,@pv);
undef(@pv);
if ( @v == 0 )
{
	&printerror(0,"Failed Found Version Config '$version' in '$workpath$versionCFN' !\n","v");
	&command("pause");
	exit 1; # 找不到版本号所在的行则退出整个程序
}
@vervalue = &getvalue (" ",$v[0]); # 如果找到多行仅从找到的第一行中取值 1编译平台 2版本号 3动态视图名 4静态视图本地位置 5发布位置 6测试负责人邮件地址
undef(@v);
if ( @vervalue < 4 ) # 本来一行应当必需前5个值 , 由于去除了编译平台值 , 因此校验为4个值
{
	&printerror(0,"Wrong Format in '$v[0]' !\n","v");
	&command("pause");
	exit 1; # 找到版本号所在的行但该行关键字缺少则退出整个程序
}
$SnapviewP = &revisepath(1,$vervalue[2]); # 获取静态视图源码存储路径 , 并修正为可用格式
$place = "Validate_CommonPath";
print "\n$module - $place......\n";
# 校验关键通用路径是否存在(共用组件路径,编译信息路径,模块配置文件路径)
print "SnapviewP.commoncompP:[$SnapviewP.$commoncompP]\n";
$compileinfo_p = $SnapviewP.$compileinfo_p;
print "SnapviewP.compileinfo_p:[$compileinfo_p]\n";
###add 20130322
if ((($ISM eq 1)&&($OS eq 0))||($ISM eq 0))
{
	print "\n- del compileinfo.....\n";
	my $tmptmp="*.*";
  &command("del","f",$compileinfo_p.$tmptmp);
	}


if ( ! -e $SnapviewP.$commoncompP )
{
	&printerror(0,"Failed Found Path '$SnapviewP$commoncompP' !\n");
	&command("pause");
	exit 1; # 找不到通用关键路径则退出整个程序
}
if ( ! -e $SnapviewP.$compileIFP )
{
	&printerror(0,"Failed Found Path '$SnapviewP$compileIFP' !\n");
	&command("pause");
	exit 1; # 找不到通用关键路径则退出整个程序
}
if ( ! -e $SnapviewP.$moduleCFP )
{
	&printerror(0,"Failed Found Path '$SnapviewP$moduleCFP' !\n");
	&command("pause");
	exit 1; # 找不到通用关键路径则退出整个程序
}
$DynviewN = $vervalue[1]; # 获取动态视图名
$ReleaseP = &revisepath(1,$vervalue[3]); # 获取版本发布路径 , 并修正为可用格式
$TEM = $vervalue[4]; # 获取版本测试负责人email , 并修正为可用格式
undef(@vervalue);
# =====================================================================================================================
# 获取并保存系统环境变量 , 用于环境变量调用getenv()
use Env qw(@PATH);
@SYSPATH = @PATH;
#&getenv("LinuxCC") if ( $OS );
use Env qw(@INCLUDE);
@SYSINCLUDE = @INCLUDE;
use Env qw(@LIB);
@SYSLIB = @LIB;
&getenv("DEFENV");

# =====================================================================================================================
# 获取项目配置文件信息
sub getprjconf
{
	# input: @prjfile , $key
	# output: keyword
	my ($key) = @_;
	my @found = &getline (1,$key,@prjfile); # 找关键字所在行
	if ( @found == 0 )
	{
		&printerror(0,"Not Config '$key' in 'main.ini' !\n",$key);
		return ""; # 没有找到指定的关键字$key行则返回空值
	}
	else
	{
		my @value = &getvalue ("=",$found[0]); # 获取关键字的值 , 如果找到多行则从找到的第一行中取值
		if ( $value[1] eq "" )
		{
			&printerror(0,"Wrong Format '$key' in '$found[0]' !\n",$key);
			return ""; # 找到指定的关键字$key行但获取值为空则返回空值
		}
		else
		{
			return $value[1];
		}
	}
}
# =====================================================================================================================
# 登录版本机,如果在main.ini中没有指定,默认使用172.16.0.99
sub getconnect
{
	# input: 
	# output: 
	my ($user,$passwd,@addrs) = @_;
	$place = "Get_Release_Connection";
	print "\n$module - $place......\n";
	my $addr;
	foreach $addr (@addrs)
	{
		my $address = &revisepath(1,$addr);
		chop($address);
		if ( !$OS ) # Windows
		{
			my $netuse = `net use`;
			system("net use $address $passwd /user:$user 2>$vererrL") if ( $netuse !~ /$address/ );
			&printerror(0,"Failded Connect to Release Place '$address' !\n") if ( &geterror );
		}
		else # Linux
		{
			if ( !-e $address ) # 如果本地路径不存在,创建路径,并mount发布路径
			{
				&command("mkdir",$address);
				if ( !&geterror ) # 有错误信息
				{
					system("mount -t cifs -o username=$user,password=$passwd /$address $address 2>$vererrL");
					&printerror(0,"Failded Mount Release Place '/$address' to Local Dir '$address' !\n") if ( &geterror );
				}
				else
				{
					&printerror(0,"Failded Create Local Dir '$address' !\n");
				}
			}
			else # 如果本地路径存在,检查是否已经mount,没有则mount发布路径
			{
				my $mount = `mount`;
				if ( $mount !~ /\/$address\s+on\s+$address/i )
				{
					system("mount -t cifs -o username=$user,password=$passwd /$address $address 2>$vererrL");
					&printerror(0,"Failded Mount Release Place '/$address' to Local Dir '$address' !\n") if ( &geterror );
				}
			}
		}
	}
}
# 获取指定的环境变量 , 如果增加环境变量请在sp脚本中增加参数校验
sub getenv
{
	# input: @env_keys
	# output: Globe Var of Build Env
	my (@envkeys) = @_;
	$place = "Get_Env";
	print "\n$module - $place......\n";
	my @envfile;
	# 打开编译环境配置文件env.ini
	if ( !open(ENV,$workpath."env.ini") )
	{
		&printerror(0,"Failed Open File '$workpath"."env.ini' !\n");
		&command("pause");
		exit 1; # 找不到环境变量配置文件则退出整个程序
	}
	@envfile = <ENV>;
	close(ENV);
	chomp(@envfile);
	# 开始循环处理每个关键字
	my ($envkey,$env);
	foreach $envkey (@envkeys) # 逐个处理传入的环境关键字
	{
		my @found = &getline(0,$envkey,@envfile);
		if ( @found == 0 ) # 没有找到环境关键字的行
		{
			&printerror(0,"Failed Found Env Key '$envkey' in '$workpath"."env.ini' !\n");
			if (( $envkey =~ /^LinuxCC$/i ) || ( $envkey =~ /^GROUPS$/i ) || ( $envkey =~ /^DEFENV$/i ))
			{
				next; # 如果是设置CC环境变量和默认环境变量"ENV" , 则找不到时跳过
			}
			else
			{
				&command("pause");
				exit 1; # 找不到环境变量则退出整个程序
			}
		}
		&printerror(1,"$place : $envkey : \n");
		@found = &shiftvalue($envkey,@found); # 去除环境关键字打头的关键字和空格
		if ( $envkey =~ /^VC[\d\._]*$/i )
		{
			foreach $f (@found) # 逐行处理环境关键字的行
			{
				my @value = &getvalue("=",$f); # 分离环境变量value[0]与环境变量值value[1]
				if ( $value[0] =~ /^PATH$/i ){
					push (@PATH,$value[1]);
					$env = &command("env","PATH");
				}elsif ( $value[0] =~ /^INCLUDE$/i ){
					push (@INCLUDE,$value[1]);
					$env = &command("env","INCLUDE");
				}elsif ( $value[0] =~ /^LIB$/i ){
					push (@LIB,$value[1]);
					$env = &command("env","LIB");
				}elsif ( $value[0] =~ /^MSDevDir$/i ){
					use Env qw($MSDevDir);
					$MSDevDir = $value[1];
					$env = &command("env","MSDevDir");
				}elsif ( $value[0] =~ /^MSVCDir$/i ){
					use Env qw($MSVCDir);
					$MSVCDir = $value[1];
					$env = &command("env","MSVCDir");
				}elsif ( $value[0] =~ /^VSINSTALLDIR$/i ){
					use Env qw($VSINSTALLDIR);
					$VSINSTALLDIR = $value[1];
					$env = &command("env","VSINSTALLDIR");
				}elsif ( $value[0] =~ /^VCINSTALLDIR$/i ){
					use Env qw($VCINSTALLDIR);
					$VCINSTALLDIR = $value[1];
					$env = &command("env","VCINSTALLDIR");
				}elsif ( $value[0] =~ /^FrameworkDir$/i ){
					use Env qw($FrameworkDir);
					$FrameworkDir = $value[1];
					$env = &command("env","FrameworkDir");
				}elsif ( $value[0] =~ /^FrameworkVersion$/i ){
					use Env qw($FrameworkVersion);
					$FrameworkVersion = $value[1];
					$env = &command("env","FrameworkVersion");
				}elsif ( $value[0] =~ /^FrameworkSDKDir$/i ){
					use Env qw($FrameworkSDKDir);
					$FrameworkSDKDir = $value[1];
					$env = &command("env","FrameworkSDKDir");
				}elsif ( $value[0] =~ /^DevEnvDir$/i ){
					use Env qw($DevEnvDir);
					$DevEnvDir = $value[1];
					$env = &command("env","DevEnvDir");
				}elsif ( $value[0] =~ /^LIBPATH$/i ){
					use Env qw($LIBPATH);
					$LIBPATH = $value[1];
					$env = &command("env","LIBPATH");
				}elsif ( $value[0] =~ /^ICPP_COMPILER10$/i ){
					use Env qw($ICPP_COMPILER10);
					$ICPP_COMPILER10 = $value[1];
					$env = &command("env","ICPP_COMPILER10");
				}else{
					&printerror(0,"Unknow Key Var '$value[0]' of $envkey in '$workpath"."env.ini' !\n");
				}
				&printerror(1,$env);
			}
		}
		elsif ( $envkey =~ /^CCS[\d\._]*$/i )
		{				
			foreach $f (@found) # 逐行处理环境关键字的行
			{
				my @value = &getvalue("=",$f); # 分离环境变量value[0]与环境变量值value[1]
				if ( $value[0] =~ /^PATH$/i ){
					push (@PATH,$value[1]);
					$env = &command("env","PATH");
				}elsif ( $value[0] =~ /^C54X_C_DIR$/i ){
					use Env qw($C54X_C_DIR);
					$C54X_C_DIR = $value[1];
					$env = &command("env","C54X_C_DIR");
				}elsif ( $value[0] =~ /^C54X_A_DIR$/i ){
					use Env qw($C54X_A_DIR);
					$C54X_A_DIR = $value[1];
					$env = &command("env","C54X_A_DIR");
				}elsif ( $value[0] =~ /^BSL5416_DIR$/i ){
					use Env qw($BSL5416_DIR);
					$BSL5416_DIR = $value[1];
					$env = &command("env","BSL5416_DIR");
				}elsif ( $value[0] =~ /^TI_DIR$/i ){
					use Env qw($TI_DIR);
					$TI_DIR = $value[1];
					$env = &command("env","TI_DIR");
				}elsif ( $value[0] =~ /^C55X_A_DIR$/i ){
					use Env qw($C55X_A_DIR);
					$C55X_A_DIR = $value[1];
					$env = &command("env","C55X_A_DIR");
				}elsif ( $value[0] =~ /^C55X_C_DIR$/i ){
					use Env qw($C55X_C_DIR);
					$C55X_C_DIR = $value[1];
					$env = &command("env","C55X_C_DIR");
				}elsif ( $value[0] =~ /^C55X_CONFIG_FILE$/i ){
					use Env qw($C55X_CONFIG_FILE);
					$C55X_CONFIG_FILE = $value[1];
					$env = &command("env","C55X_CONFIG_FILE");
				}elsif ( $value[0] =~ /^TMS470_C_DIR$/i ){
					use Env qw($TMS470_C_DIR);
					$TMS470_C_DIR = $value[1];
					$env = &command("env","TMS470_C_DIR");
				}elsif ( $value[0] =~ /^TMS470_A_DIR$/i ){
					use Env qw($TMS470_A_DIR);
					$TMS470_A_DIR = $value[1];
					$env = &command("env","TMS470_A_DIR");
				}elsif ( $value[0] =~ /^C6X_C_DIR$/i ){
					use Env qw($C6X_C_DIR);
					$C6X_C_DIR = $value[1];
					$env = &command("env","C6X_C_DIR");
				}elsif ( $value[0] =~ /^C6X_A_DIR$/i ){
					use Env qw($C6X_A_DIR);
					$C6X_A_DIR = $value[1];
					$env = &command("env","C6X_A_DIR");
				}elsif ( $value[0] =~ /^D_SRC$/i ){
					use Env qw($D_SRC);
					$D_SRC = $value[1];
					$env = &command("env","D_SRC");
				}elsif ( $value[0] =~ /^BSL6416_DIR$/i ){
					use Env qw($BSL6416_DIR);
					$BSL6416_DIR = $value[1];
					$env = &command("env","BSL6416_DIR");
				}elsif ( $value[0] =~ /^BSL6713_DIR$/i ){
					use Env qw($BSL6713_DIR);
					$BSL6713_DIR = $value[1];
					$env = &command("env","BSL6713_DIR");
				}else{
					&printerror(0,"Unknow Key Var '$value[0]' of $envkey in '$workpath"."env.ini' !\n");
				}
				&printerror(1,$env);
			}
		}
		elsif ( $envkey =~ /^ETI[\d\._]*$/i )
		{
			foreach $f (@found) # 逐行处理环境关键字的行
			{
				my @value = &getvalue("=",$f); # 分离环境变量value[0]与环境变量值value[1]
				if ( $value[0] =~ /^PATH$/i ){
					push (@PATH,$value[1]);
					$env = &command("env","PATH");
				}elsif ( $value[0] =~ /^EQUATOR_ROOT$/i ){
					use Env qw($EQUATOR_ROOT);
					$EQUATOR_ROOT = $value[1];
					$env = &command("env","EQUATOR_ROOT");
				}elsif ( $value[0] =~ /^ETI_TOOLKIT$/i ){
					use Env qw($ETI_TOOLKIT);
					$ETI_TOOLKIT = $value[1];
					$env = &command("env","ETI_TOOLKIT");
				}else{
					&printerror(0,"Unknow Key Var '$value[0]' of '$envkey' in '$workpath"."env.ini' !\n");
				}
				&printerror(1,$env);
			}
		}
		elsif ( $envkey =~ /^Tornado[\d\._]*$/i )
		{
			foreach $f (@found) # 逐行处理环境关键字的行
			{
				my @value = &getvalue("=",$f); # 分离环境变量value[0]与环境变量值value[1]
				if ( $value[0] =~ /^PATH$/i ){
					push (@PATH,$value[1]);
					$env = &command("env","PATH");
				}elsif ( $value[0] =~ /^WIND_BASE$/i ){
					use Env qw($WIND_BASE);
					$WIND_BASE = $value[1];
					$env = &command("env","WIND_BASE");
				}elsif ( $value[0] =~ /^WIND_HOST_TYPE$/i ){
					use Env qw($WIND_HOST_TYPE);
					$WIND_HOST_TYPE = $value[1];
					$env = &command("env","WIND_HOST_TYPE");
				}else{
					&printerror(0,"Unknow Key Var '$value[0]' of $envkey in '$workpath"."env.ini' !\n");
				}
				&printerror(1,$env);
			}
		}
		elsif ( $envkey =~ /^InstallShield[\d\._]*$/i )
		{
			foreach $f (@found) # 逐行处理环境关键字的行
			{
				my @value = &getvalue("=",$f); # 分离环境变量value[0]与环境变量值value[1]
				if ( $value[0] =~ /^InstallShield12Build$/i ){
					use Env qw($InstallShield12Build);
					$InstallShield12Build = $value[1];
					$env = &command("env","InstallShield12Build");
				}elsif ( $value[0] =~ /^InstallShield12Script$/i ){
					use Env qw($InstallShield12Script);
					$InstallShield12Script = $value[1];
					$env = &command("env","InstallShield12Script");
				}else{
					&printerror(0,"Unknow Key Var '$value[0]' of $envkey in '$workpath"."env.ini' !\n");
				}
				&printerror(1,$env);
			}
		}
		elsif ( $envkey =~ /^linuxintel[\d\._]*$/i )
		{
			foreach $f (@found) # 逐行处理环境关键字的行
			{
				my @value = &getvalue("=",$f); # 分离环境变量value[0]与环境变量值value[1]
				if ( $value[0] =~ /^PATH$/i ){
					push (@PATH,$value[1]);
					$env = &command("env","PATH");
				}elsif ( $value[0] =~ /^INCLUDE$/i ){
					push (@INCLUDE,$value[1]);
					$env = &command("env","INCLUDE");
				}elsif ( $value[0] =~ /^LIB$/i ){
					push (@LIB,$value[1]);
					$env = &command("env","LIB");
				}elsif ( $value[0] =~ /^LIBRARY_PATH$/i ){
					use Env qw($LIBRARY_PATH);
					$LIBRARY_PATH = $value[1];
					$env = &command("env","LIBRARY_PATH");
				}elsif ( $value[0] =~ /^LD_LIBRARY_PATH$/i ){
					use Env qw($LD_LIBRARY_PATH);
					$LD_LIBRARY_PATH = $value[1];
					$env = &command("env","LD_LIBRARY_PATH");
				}elsif ( $value[0] =~ /^DYLD_LIBRARY_PATH$/i ){
					use Env qw($DYLD_LIBRARY_PATH);
					$DYLD_LIBRARY_PATH = $value[1];
					$env = &command("env","DYLD_LIBRARY_PATH");
				}elsif ( $value[0] =~ /^NLSPATH$/i ){
					use Env qw($NLSPATH);
					$NLSPATH = $value[1];
					$env = &command("env","NLSPATH");
				}elsif ( $value[0] =~ /^MANPATH$/i ){
					use Env qw($MANPATH);
					$MANPATH = $value[1];
					$env = &command("env","MANPATH");
				}elsif ( $value[0] =~ /^INTEL_LICENSE_FILE$/i ){
					use Env qw($INTEL_LICENSE_FILE);
					$INTEL_LICENSE_FILE = $value[1];
					$env = &command("env","INTEL_LICENSE_FILE");
				}elsif ( $value[0] =~ /^CPATH$/i ){
					use Env qw($CPATH);
					$CPATH = $value[1];
					$env = &command("env","CPATH");
				}elsif ( $value[0] =~ /^FPATH$/i ){
					use Env qw($FPATH);
					$FPATH = $value[1];
					$env = &command("env","FPATH");
				}elsif ( $value[0] =~ /^IPPROOT$/i ){
					use Env qw($IPPROOT);
					$IPPROOT = $value[1];
					$env = &command("env","IPPROOT");
				}else{
					&printerror(0,"Unknow Key Var '$value[0]' of $envkey in '$workpath"."env.ini' !\n");
				}
				&printerror(1,$env);
			}
		}
		elsif ( $envkey =~ /^sign[\d\._]*$/i )
		{
			foreach $f (@found) # 逐行处理环境关键字的行
			{
				my @value = &getvalue("=",$f); # 分离环境变量value[0]与环境变量值value[1]
				if ( $value[0] =~ /^PATH$/i ){
					push (@PATH,$value[1]);
					$env = &command("env","PATH");
				}else{
					&printerror(0,"Unknow Key Var '$value[0]' of $envkey in '$workpath"."env.ini' !\n");
				}
				&printerror(1,$env);
			}
		}
		elsif ( $envkey =~ /^GTK[\d\._]*$/i )
		{
			foreach $f (@found) # 逐行处理环境关键字的行
			{
				my @value = &getvalue("=",$f); # 分离环境变量value[0]与环境变量值value[1]
				if ( $value[0] =~ /^INCLUDE$/i ){
					push (@INCLUDE,$value[1]);
					$env = &command("env","INCLUDE");
				}elsif ( $value[0] =~ /^LIB$/i ){
					push (@LIB,$value[1]);
					$env = &command("env","LIB");
				}else{
					&printerror(0,"Unknow Key Var '$value[0]' of $envkey in '$workpath"."env.ini' !\n");
				}
				&printerror(1,$env);
			}
		}
		elsif (( $envkey =~ /^PPC[\d\._]*$/i ) || ( $envkey =~ /^ARM[\d\._]*$/i ) || ( $envkey =~ /^EQT[\d\._]*$/i ) || ( $envkey =~ /^DAVINCI[\d\._]*$/i ))
		{
			foreach $f (@found) # 逐行处理环境关键字的行
			{
				my @value = &getvalue("=",$f); # 分离环境变量value[0]与环境变量值value[1]
				if ( $value[0] =~ /^PATH$/i ){
					push (@PATH,$value[1]);
					$env = &command("env","PATH");
				}else{
					&printerror(0,"Unknow Key Var '$value[0]' of '$envkey' in '$workpath"."env.ini' !\n");
				}
				&printerror(1,$env);
			}
		}
		elsif ( $envkey =~ /^GROUPS[\d\._]*$/i )
		{
			foreach $f (@found) # 逐行处理环境关键字的行
			{
				my @value = &getvalue("=",$f); # 分离环境变量value[0]与环境变量值value[1]
				if ( $value[0] =~ /^CLEARCASE_GROUPS$/i ){
					use Env qw($CLEARCASE_GROUPS);
					$CLEARCASE_GROUPS = $value[1] if ( $value[1] ne "" );
					$env = &command("env","CLEARCASE_GROUPS");
				}else{
					&printerror(0,"Unknow Key Var '$value[0]' of '$envkey' in '$workpath"."env.ini' !\n");
				}
				&printerror(1,$env);
			}
		}
		elsif ( $envkey =~ /^LinuxCC[\d\._]*$/i )
		{
			foreach $f (@found) # 逐行处理环境关键字的行
			{
				my @value = &getvalue("=",$f); # 分离环境变量value[0]与环境变量值value[1]
				if ( $value[0] =~ /^PATH$/i ){
					my @part = &getvalue(":",$value[1]);
					foreach $p (@part)
					{
						my @fd = grep(/$p/,@SYSPATH);
						if ( @fd == 0 )
						{
							push (@PATH,$p);
							push (@SYSPATH,$p);
						}
					}
					undef(@fd);
					undef($p);
					undef(@part);
					$env = &command("env","PATH");
				}else{
					&printerror(0,"Unknow Key Var '$value[0]' of $envkey in '$workpath"."env.ini' !\n");
				}
				&printerror(1,$env);
			}
		}
		elsif ( $envkey =~ /^DEFENV$/i )
		{
			@f = &getline(1,$OSstring,@found);
			if ( @f == 0 )
			{
				&printerror(0,"Failed Found $OSstring Default Env Key in '$workpath"."env.ini' !\n");
			}
			else
			{
				my @value = &getvalue("=",$f[0]); # 分离环境变量value[0]与环境变量值value[1]
				$defenv = $value[1];
				$defenv = "-" if ( $defenv eq "" ); # 如果默认环境变量为空,则避免module函数参数错误
				&printerror(1,"$OSstring Default Env Key : $defenv\n");
			}
			undef(@f);
		}
		else
		{
			&printerror(0,"UnPreDefine Key Var '$value[0]' of '$envkey' in '$workpath"."common.pl' !\n");
		}
	}
}
# =====================================================================================================================
# 调用模块编译之前 , 做关于编译环境的必要处理 , 例如创建log文件和编译信息文件等
sub preprocess
{
	# input: $options
	# output: none
	my ($options) = @_;
	# 如果更新 , 则在模块编译之前更新common中指定的目录
	if ( $options =~ /u/ )
	{
		$place = "Update_Common_SourceCode";
		print "\n$module - $place......\n";

		if ( $OS )
		{
			&module("u","-","common");
		}
		else
		{
			&module("u","GROUPS","common");
		}
	}
	# 打开main.ini文件读取必要信息
	if ( open(PROJECT,$workpath."main.ini") )
	{
		$place = "Get_Project_Config";
		print "\n$module - $place......\n";
		@prjfile = <PROJECT>;
		close(PROJECT);
		chomp(@prjfile);
	}
	else
	{
		&printerror(0,"Failed Open File '$workpath"."main.ini' !\n");
	}
	# 如果校验 , 则预定义所有模块校验文件结果为通过 , 在模块调用过程中若有不通过的模块则置0 , 用于邮件发送
	if ( $options =~ /c/ )
	{
		$place = "Create_NoPassFile_Log";
		print "\n$module - $place......\n";
		my $checkreadme = &getprjconf("checkreadme");
		$CRM = 0; # 将用户填写的checkreadme信息转为bool值供后续判断
		$CRM = 1 if (( $checkreadme eq "" ) || ( $checkreadme eq "yes" ) || ( $checkreadme eq "y" )); # 仅当填写值为空或不填或等于"yes"时$CRM才为真
		undef($checkreadme);
		$allcheckfileS = 1;
		if ( $options !~ /f/ )
		{
			my $releasekreadme = &getprjconf("releasereadme");
			$RRM = 0; # 将用户填写的releasekreadme信息转为bool值供后续判断
			$RRM = 1 if (( $releasekreadme eq "" ) || ( $releasekreadme eq "yes" ) || ( $releasekreadme eq "y" )); # 仅当填写值为空或不填或等于"yes"时$RRM才为真
			undef($releasekreadme);
			if ( $CRM )
			{
				$place = "Create_AllReadme_Log";
				print "\n$module - $place......\n";
				if ( open (ALLREADME,">$verworkP"."AllReadme.txt") ) # 记录校验未通过readme的临时文件 , 将nopassreadme.log文件清空
				{
					&getallmodules;
					#print ALLREADME "UCMproject : $UCMprj\nVersion : $version\nModules : "."@allmodules"."\nCheck Readme Time : $builddate\n";
					close (ALLREADME);
					undef(@allmodules);
				}
				else
				{
					 &printerror(0,"Failed Open File '$verworkP"."AllReadme.txt' !\n");
				}
			}
		}
	}
	# 如果发布,则获取发布路径
	if ( $options =~ /r/ )
	{
		# 获取登录到指定的版本服务器的相关信息
		my $connect_address = &getprjconf("connect_addrs");
		my @connect_addrs = &getvalue(",",$connect_address);
		undef($connect_address);
		my $connect_user = &getprjconf("connect_user");
		my $connect_passwd = &getprjconf("connect_passwd");
		# 创建链接
		&getconnect($connect_user,$connect_passwd,@connect_addrs) if (( $connect_user ne "" ) && ( @connect_addrs > 0 ));
		undef(@connect_addrs);
		undef($connect_user);
		undef($connect_passwd);
		my $TSstr = &getprjconf("buildtime_span");
		$TS = 180*60; # 默认180分钟 , 换成秒
		if (( $TSstr =~ /^\d+$/ ) && ( $TSstr <= 1440 )) # 最大1440分钟 , 即一天
		{
			$TS = $TSstr*60; # 换成秒
		}
		my $winfirst = &getprjconf("windowsfirst");
		$WF = 0; # 将用户填写的releasekreadme信息转为bool值供后续判断
		$WF = 1 if (( $winfirst eq "" ) || ( $winfirst eq "yes" ) || ( $winfirst eq "y" )); # 仅当填写值为空或不填或等于"yes"时$WF才为真
	}
	if ( $options =~ /n/ )
	{
		my $sendmail = &getprjconf("sendmail");
		$MAIL = 0; # 将用户填写的sendmail信息转为bool值供后续判断
		$MAIL = 1 if (( $sendmail eq "" ) || ( $sendmail eq "yes" ) || ( $sendmail eq "y" )); # 仅当填写值为空或不填或等于"yes"时$MAIL才为真
		undef($sendmail);
		if ( $MAIL )
		{
			$PMM = &getprjconf("PMmail");
			$CMOM = &getprjconf("CMOmail");
			$CMOM = "sqlmail\@kdcrd.com" if ( $CMOM eq "" );
			$SMTP = &getprjconf("MAIL_SMTP_IP"); # 用于邮件通知的SMTP设置
			$SMTP = "10.5.0.54" if ( $SMTP eq "" );
			$mailfrom = &getprjconf("MAIL_FROM"); # 用于邮件通知的发件人设置
			$mailfrom = "sqlmail\@kedacom.com" if ( $mailfrom eq "" );
		}
	}
	undef(@prjfile);
}
# 调用模块编译之后 , 做关于编译环境的必要处理 , 例如发布编译报告,发布readme,发布编译信息等
sub afterprocess
{
	# input: $options
	# output: none
	my ($options) = @_;
	# 如果发布 , 则发布编译信息和common中指定的目录和文件
	if ( $options =~ /r/ )
	{
		# 如果是集成编译 , 发布common中指定的目录和文件
		if ( $options =~ /i/ )
		{
			$place = "Release_Common_Files_Dirs";
			print "\n$module - $place......\n";
			&module("r","-","common");
		}
		# 如果需要发布readme , 则发布AllReadme.txt文件
		if ( $RRM )
		{
			$place = "Release_AllReadme";
			print "\n$module - $place......\n";
			my $WreadmeFRP = &revisepath(1,$RP."readme"); # 修正路径为可用格式
			&command("copy","f",$verworkP."AllReadme.txt",$WreadmeFRP);
			&geterror; # 有错误信息则写入BuildError.log
		}
	}
	# 如果编译 , 制作编译信息文件
	if ( $options =~ /b/ )
	{
		$place = "Make_BuildInfo";
		print "\n$module - $place......\n";
		# 打开编译信息BuildInfo.txt
		if ( open(BUILDINFO,">$verworkP"."BuildInfo_".$OSstring."_".$builddatetime.".txt") )
		{
			my $currenttime = &getdatetime;
			print BUILDINFO "Product Line : $proline\nProject : $UCMprj\nVersion : $version\nBuild Server : $BuildServer\nAutoBuild Script Path : $workpath\nBuildTime : $builddatetime ~ $currenttime\n";
			&getallmodules;
			print BUILDINFO "Modules : "."@allmodules"."\n";
			undef(@allmodules);
			print BUILDINFO "ReleasPlace : $RP\n" if ( $options =~ /r/ );
			# Baseline写入编译信息  ###hanjian 20120817 delete 
###			if ( open(BASELINE,"all-code_".$builddatetime."_update.log") )
###			{
###				@blfile = <BASELINE>;
###				close(BASELINE);
###				print BUILDINFO ("\n============================= Stream update Info =============================\n"."@blfile"."\n");
###				undef(@blfile);
###			}
###			else
###			{
###				$place = "Read_updateLog";
###				&printerror(0,"Failed Open File '$verworkP"."baseline.log' !\n");
###			}
			# 编译时间写入编译信息
			if ( open(TIME,$verworkP."buildtime.log") )
			{
				@timefile = <TIME>;
				close(TIME);
				print BUILDINFO ("\n============================= Module Build Info =============================\n"."@timefile"."\n");
				undef(@timefile);
			}
			else
			{
				$place = "Read_BuildTimeLog";
				&printerror(0,"Failed Open File '$verworkP"."buildtime.log' !\n");
			}
			# 校验文件结果写入编译信息
			if ( $options =~ /c/ )
			{
				if ( open(CHECK,$verworkP."checkfile.log") )
				{
					@checkfile = <CHECK>;
					close(CHECK);
					print BUILDINFO ("\n============================= Modules Check Info =============================\n"."@checkfile"."\n");
					undef(@checkfile);
				}
				else
				{
					$place = "Read_CheckFileLog";
					&printerror(0,"Failed Open File '$verworkP"."checkfile.log' !\n");
				}
			}
			print BUILDINFO "\n\nEND\n";
			close(BUILDINFO);
			# 发布编译信息文件
			if ( $options =~ /r/ )
			{
				$place = "Release_BuildInfo";
				print "\n$module - $place......\n";
				&command("copy","f",$verworkP."BuildInfo_".$OSstring."_".$builddatetime.".txt",$RP); # 发布BuildInfo
				
				#####hanjian 20120810 
				
				&command("copy","f",$verworkP."all-code_".$builddatetime."_update.log",$RP); # 发布整体编译时，全部代码svn update的结果记录文件
				
				&geterror;
			}
		}
		else
		{
			$place = "Create_BuildInfo";
			&printerror(0,"Failed Open File '$verworkP"."buildinfo_$OSstring_$builddatetime.txt' !\n");
		}
	}
	# 如果开启通知功能 , 模块编译完成后提供编译状态报告
	if (( $options =~ /n/ ) && $MAIL )
	{
		$place = "Build_Status_Report";
		print "\n$module - $place......\n";
		if ( $options =~ /i/ )
		{
			&notify(0,1);
		}
		else
		{
			&notify(0,0);
		}
	}
}
# 模块编译主过程，用于外部调用
sub module
{
	# parameter usage -----
	# 0 moduleoptions : not necessary
	# 0.0 defause none : whole build process , check file , notify bases spbuild rule
	# 0.1 -u : update src code
	# 0.2 -b : build(compile)
	# 0.30 -c : check file and readme
	# 0.31 -cf : check file
	# 0.4 -r : release
	# 0.50 -n : notify rules , must be used with -c that means check and notify
	# 0.51 -i : handle bases on intbuild rule or on spbuild rule
	# 1 envkeylist : the env keys of the project , necessary but allow none , more than one keys seperate by ","
	# 2 modules : modules of the version , necessary
	# input : $moduleoptions , $moduleenvs , @modules
	# output : BuildError.txt
	my ($moduleoptions,$moduleenvs,@modules) = @_;
	$module = "";
	&printerror(1,"\n~~~~~~~~~~~~~~~ Modules Process Start ~~~~~~~~~~~~~~\n"); # 用于区分多次调用module的打印信息
	&printerror(1,"ModulesOptions : $moduleoptions\nEnv : $moduleenvs\nModules : "."@modules"."\n");
	# 获取demo文件信息，用于module配置错误时的正确引导
#	open(DEMO,$SnapviewP.$moduleCFP."demo.ini");
#	local @demofile = <DEMO>; # 读取文件内容
#	close(DEMO);
#	chomp(@demofile); # 去除换行符
	# =====================================================================================================================
	# 如果环境变量关键字不空 , 设置环境变量
	if ( $moduleenvs !~ /^-*$/ )
	{
		my @envs = &getvalue(",",$moduleenvs);
		if ( $moduleoptions =~ /b/ )
		{
			# 清除上一次获取getenv对编译环境的改变
			undef(@PATH);
			@PATH = @SYSPATH;
			undef(@INCLUDE);
			@INCLUDE = @SYSINCLUDE;
			undef(@LIB);
			@LIB = @SYSLIB;
		}
		&getenv(@envs);
	}
	# =====================================================================================================================
	# 多模块依次调用
	local ($WmoduleCF,@modufile); # $module已经定义为全局
	for ( my $i = 0 ; $i < @modules ; $i ++ )
	{
		$module = $modules[$i];
		# 获取模块配置文件路径
		$WmoduleCF = $SnapviewP.$moduleCFP.$module.".ini";
		# 读模块配置文件信息
		if ( open(MODULE,$WmoduleCF) )
		{
			@modufile = <MODULE>;
			close(MODULE);
			chomp(@modufile);
			# 对每个模块调用模块编译过程
			&process($moduleoptions);
			undef(@modufile);
		}
		else
		{
			&printerror(0,"Failed Open Module Config File '$WmoduleCF' !\n");
			&printerror(0,"Skip Module Build !\n");
			next; # 打不开模块配置文件则跳过该模块编译过程
		}
	}
	$module = "";
	&printerror(1,"\n--------------- Modules Process End ---------------\n"); # 用于区分多次调用module的打印信息
}
# 根据模块编译流程定义模块编译过程
sub process
{
	# input: $options , $CRM , $RRM
	# output: none
	# 根据用户对编译过程的选择从模块配置文件获取本次编译流程所需的配置信息
	my ($options) = @_;
	local ($srccodePs,$owner,$ownerM);
	&getmoduconf_s if (( $options =~ /u/ ) || ( $options =~ /c/ ) || ( $options =~ /n/ ));
	local ($compileFP,$compileFN,$readmeFP,$readmeFN);
	&getmoduconf_b if (( $options =~ /b/ ) || (( $options =~ /c/ ) && ( $options !~ /f/ ) && ( $CRM || $RRM )) || ( $options =~ /r/ ));
	local (@checkFs,@releaseFs); # 用于存放校验文件的文件名和路径/发布文件的文件名、路径和发布路径 ; 在getmoduconf_c赋值在del/checkfile/releasefiles使用
	&getmoduconf_c if (( $options =~ /b/ ) || ( $options =~ /c/ ) || ( $options =~ /r/ ));
	local (@releaseDs); # 用于存放发布目录的目录名和路径 ; 在getmoduconf_r赋值在releasefiles使用
	&getmoduconf_r if ( $options =~ /r/ );
	# 根据用户对编译过程的选择调用子程序
	&del if ( $options =~ /b/ );
	local $updateS = 1;
	if ( $options =~ /u/ )
	{
		&update;
		&disreadonly; # update源码后，去除源码的只读属性
	}
#	print "updateS : $updateS\n";
	local $compileS = 1;
	&compile if ( $options =~ /b/ );
	# 用于发送邮件判断 , 校验结果为0时发送邮件 , 仅当校验不通过时发送邮件 , 若main.ini中配置不校验或未配置校验信息也不发送邮件
	local $checkfileS = 1;
	local $checkreadmeS = 1;
	local ($CFpassinfo,$RMpassinfo);
	if ( $options =~ /c/ )
	{
		&checkfile;
		# 子过程选择参数为cf时不校验readme ; 项目配置文件中配置需要校验readme时才校验readme
		####&checkreadme if ( $CRM && ( $options !~ /f/ )); ###20120822 delete by hanjian 暂时注掉
	}
#	print "checkfileS : $checkfileS\ncheckreadmeS : $checkreadmeS\nCFpassinfo : $CFpassinfo\nRMpassinfo : $RMpassinfo\n";
	if ( $options =~ /r/ )
	{
		&releasefiles;
		# 项目配置文件中配置需要发布readme时才发布readme ; 校验文件结果通过时才发布readme
		&releasereadme if ( $RRM && ( $checkfileS == 1 ));
		&releasecompileinfo;
		&releaseupdatelog if ( $options =~ /u/ );
	}
	if (( $options =~ /n/ ) && $MAIL )
	{
		if ( !$checkfileS || ( $CRM && !$checkreadmeS )) # 单个模块的通知规则 , 仅校验不通过时才通知
		{
			if ( $options =~ /i/ )
			{
				&notify(1,1);
			}
			else
			{
				&notify(1,0);
			}
		}
	}
}
# =====================================================================================================================
# 获取模块配置文件信息
sub getmoduconf
{
	# input: $keyword , @modufile
	# output: lines found in @modufile begin with the keyword
	my ($key) = @_;
	$place = "Get_Module_Config_$key";
	print "\n$module - $place......\n";
	my @l;
	if ($OS)
	{
		@l = &getline (0,$key."l",@modufile); # linux操作系统在关键字后加"l"查找
	}
	else
	{
		@l = &getline (0,$key."w",@modufile); # windows操作系统在关键字后加"w"查找
	}
	&printerror(0,"Not Config '$key' in '$WmoduleCF' !\n",$key) if ( @l == 0 );
	return @l;
}
# 获取模块源码信息
sub getmoduconf_s
{
	# input: none
	# output: $srccodePs , $owner , $ownerM
	my @s = &getmoduconf("s"); # 从模块配置文件获取's'关键字行
	if ( @s != 0 )
	{
		my @value = &getvalue (" ",$s[0]); # 从该行获取所有关键值 1源码路径列表 2负责人 3邮件地址
		if ( $value[1] =~ /^-*$/ ) # 如果源码路径列表为空 , 或以"-"代表为空
		{
			$srccodePs = "";
			&printerror(0,"Wrong Format of Source Code Path in '$s[0]' !\n","s");
		}
		else
		{
			$srccodePs = $value[1];
		}
		if ( $value[2] =~ /^-*$/ )
		{
			$owner = "";
		}
		else
		{
			$owner = $value[2];
		}
		if (( $value[3] =~ /^-*$/ ) || ( $value[3] !~ /@/ ))
		{
			$ownerM = "sqlmail\@kedacom.com";
		}
		else
		{
			$ownerM = $value[3];
		}
	}
	else
	{
		$srccodePs = "";
	}
#	print "srccodePs : $srccodePs\nowner : $owner\nownerM : $ownerM\n";
}
# 获取模块编译和readme信息
sub getmoduconf_b
{
	# input: none
	# output: $compileFP , $compileFN , $readmeFP , $readmeFN
	my @b = &getmoduconf("b"); # 从模块配置文件获取'b'关键字行
	if ( @b == 0 ) # 没找到匹配的行
	{
		$compileFN = "";
		$readmeFN = "";
	}
	else
	{
		my @value = &getvalue (" ",$b[0]); # 从该行获取所有关键值 1compile文件路径 2compile文件名称 3readme文件路径 4readme文件名称
		if (( $value[1] =~ /^-*$/ ) || ( $value[2] =~ /^-*$/ )) # 如果compile文件路径或compile文件名称都不为空 , 其值才有效
		{
			$compileFP = "";
			$compileFN = "";
			&printerror(0,"Wrong Format of Compile File in '$b[0]' !\n","b");
		}
		else
		{
			$compileFP = &revisepath(0,$value[1]); # 修正路径为可用格式
			$compileFN = $value[2];
		}
		if ( $value[4] =~ /^-*$/ )
		{
			$readmeFP = "";
			$readmeFN = "";
			&printerror(0,"Wrong Format of Readme File in '$b[0]' !\n","b");
		}
		else
		{
			if ( $value[3] =~ /^-*$/ ) # 如果compile文件路径或compile文件名称都不为空 , 其值才有效
			{
				if ( $compileFP eq "" ) # 而compile文件路径不为空 , 则认为readme文件路径即compile文件路径
				{
					$readmeFP = "";
					$readmeFN = "";
					&printerror(0,"Wrong Format of Readme File in '$b[0]' !\n","b");
				}
				else
				{
					$readmeFP = $compileFP;
					$readmeFN = $value[4];
				}
			}
			else # 如果readme文件路径为空 , 
			{
				$readmeFP = &revisepath(0,$value[3]); # 修正路径为可用格式
				$readmeFN = $value[4];
			}
		}
	}
#	print "compileFP : $compileFP\ncompileFN : $compileFN\nreadmeFP : $readmeFP\nreadmeFN : $readmeFN\n";
}
# 获取模块校验和发布文件信息
sub getmoduconf_c
{	# input: none
	# output: @checkFs , @releaseFs
	my @c = &getmoduconf("c"); # 从模块配置文件获取'c'关键字行
	if ( @c != 0 ) # 从模块配置文件找到了'c'关键字行
	{
		my ($c,@checkFNs,$checkFN,$checkFP,$FRP,@FRPs);
		foreach $c (@c) # 从每一行获取信息
		{
			my @value = &getvalue (" ",$c); # 从该行获取所有关键值 1校验文件名称列表 2文件路径 3发布路径列表
			if ( @value < 3 ) # 必填关键值为前3个
			{
				&printerror(0,"Wrong Format in line '$c[0]' !\n","c");
				next; # 必填关键值缺少则跳过该行
			}
			@checkFNs = &getvalue(",",$value[1]); # 将校验文件名称列表转为数组
#			@checkFNs = split(/,/,$value[1]); # 将校验文件名称列表转为数组
			$checkFP = &revisepath(0,$value[2]); # 修正路径为可用格式
			if ( $value[3] =~ /^-*$/ ) # 从该行获取的'发布路径列表'为空 , 或以"-"代表为空
			{
				foreach $checkFN (@checkFNs)
				{
					# 推入checkF格式为 : 0文件名称 1文件路径
					push (@checkFs,$checkFN." ".$checkFP);
#					print "checkF : $checkFN $checkFP\n";
				}
			}
			else
			{
				foreach $checkFN (@checkFNs)
				{
					# 推入checkF格式为 : 0文件名称 1文件路径 2发布路径列表
					push (@checkFs,$checkFN." ".$checkFP." ".$value[3]);
					# 推入releaseF格式为 : 0文件名称 1文件路径 2发布路径列表
					push (@releaseFs,$checkFN." ".$checkFP." ".$value[3]);
#					print "checkF/releaseF : $checkFN $checkFP $value[3]\n";
				}
			}
		}
		&printerror(0,"Failed Get Release Files Config in '$WmoduleCF' !\n") if ( @releaseFs == 0 );
	}
	else
	{
		undef(@checkFs);
		undef(@releaseFs);
	}
}
# 获取模块发布目录信息
sub getmoduconf_r
{
	# input: none
	# output: @releaseDs
	my @r = &getmoduconf("r"); # 从模块配置文件获取'r'关键字行
	if ( @r != 0 ) # 从模块配置文件找到了'r'关键字行
	{
		my ($r,@RDNs,$RDN,$RDP);
		foreach $r (@r) # 从每一行获取信息
		{
			my @value = &getvalue (" ",$r); # 从该行获取所有关键值 1发布文件夹名称列表 2文件夹路径 3发布路径列表
			if ( @value < 4 ) # 必填关键值为前4个
			{
				&printerror(0,"Wrong Format in '$r[0]' !\n","r");
				next; # 必填关键值缺少则跳过该行
			}
			@RDNs = &getvalue(",",$value[1]); # 将校验文件名称列表转为数组
#			@RDNs = split(/,/,$value[1]); # 将校验文件名称列表转为数组
			foreach $RDN (@RDNs)
			{
				# 推入releaseDs格式为 : 0目录名称 1目录路径 2发布路径列表
				push (@releaseDs,$RDN." ".$value[2]." ".$value[3]);
#				print "releaseD : $RDN $RDP $value[3]\n";
			}
		}
		&printerror(0,"Failed Get Release Dirs Config in '$WmoduleCF' !\n") if ( @releaseDs == 0 );
	}
	else
	{
		undef(@releaseDs);
	}
}
# =====================================================================================================================
#add by hanjian 20120824

#sub deletecompileinfo
#{
#	print"delete compileinfo\n";
	
#	$compilePPP = &getprjconf("compileinfo_path");
	
#	$compilePPP = &revisepath(1,$compilePPP);
#	my $compileinfopath=$SnapviewP.$compilePPP."*.*";
#	print "\n!!!!!compile info path: $compileinfopath\n";
#	&command("del","f",$compileinfopath);
	
#}
# =====================================================================================================================
# 模块编译'删除'子过程
sub del
{
	# input: $SnapviewP , $commoncompP , @checkFs
	# output: none
	$place = "Delete_CheckFiles";
	print "\n$module - $place......\n";
	# 删除校验文件
	if ( @checkFs != 0 ) # 从配置文件中获取的校验文件信息不为空
	{
		my ($checkF,$WcheckF,$WcheckFs);
		foreach $checkF (@checkFs) # 获取每一个校验文件的信息
		{
			my @value = &getvalue(" ",$checkF); # 获取该校验文件信息 0文件名称 1文件路径 2发布路径列表
			$value[0] =~ s/(y{2}|y{4})mmdd/\*/ig; # chenhuiren 090610 
			$WcheckF = $SnapviewP.$commoncompP.$value[1].$value[0]; # 连接路径和文件名
#			print "WcheckF : $WcheckF\n";
			$WcheckFs = $WcheckFs.$WcheckF." "; # 将多个校验文件用空格隔开作为del命令的项一次执行
		}
		&command("del","f",$WcheckFs); # 调用删除命令并输出错误信息到error.log中
	}
	&geterror; # 有错误信息则写入BuildError.log
}

# 模块编译'更新源文件'子过程
sub update
{
	# input: $SnapviewP , $srccodePs , $module , $builddatetime
	# output: update log
	$place = "Update_SourceCode";
	print "\n$module - $place......\n";
	if ( $srccodePs ne "" )
	{
		my ($srccodeP,$WsrccodeP,$WsrccodePs);
		&cleanerror;
		my @srccodeP = &getvalue(",",$srccodePs); # 将多个源码路径分割成数组
#		my @srccodeP = split(/,/,$srccodePs); # 将多个源码路径分割成数组
		#####my $updateLFN = $module."_".$builddatetime.".updt"; # 定义更新源码的日志文件名称
		my $updateLFN = $module."_".$builddatetime."_update.log"; # 定义更新源码的日志文件名称  modify by hanjian 20120803 ;modify by hanjian 20120810
		
		foreach $srccodeP (@srccodeP)
		{
			$srccodeP = &revisepath(2,$srccodeP); # 修正路径为可以格式
			$WsrccodeP = $SnapviewP.$srccodeP; # 拼接成完整路径
#			print "WsrccodeP : $WsrccodeP\n";
			$WsrccodePs = $WsrccodePs.$WsrccodeP." "; # 将多个源码路径用空格隔开作为update命令的项一次执行
		}
		&command("update",$WsrccodePs,$verworkP.$updateLFN); # 调用更新源码命令并输出错误信息到error.log中
		if ( &geterror ) # 有错误信息
		{
			$updateS = 0;
			&printerror(0,"@errors");
		}
	}
	else
	{
		$updateS = 0;
		&printerror(0,"Failed Get SourceCode Path Config !\n");
		&printerror(0,"Skip Update Process !\n");
	}
}


# 去除源码只读属性
sub disreadonly
{
	# input: $SnapviewP , $srccodePs , $module , $builddatetime
	# output: update log
	$place = "DisReadonly_SourceCode";
	print "\n$module - $place......\n";
	if (( $srccodePs ne "" ) && $updateS )
	{
		my ($srccodeP,$WsrccodeP);
		my @srccodeP = &getvalue(",",$srccodePs); # 将多个源码路径分割成数组
#		my @srccodeP = split(/,/,$srccodePs); # 将多个源码路径分割成数组
		my $updateLFN = $module."_".$builddatetime.".updt"; # 定义更新源码的日志文件名称
		foreach $srccodeP (@srccodeP)
		{
			$srccodeP = &revisepath(0,$srccodeP); # 修正路径为可以格式
			$WsrccodeP = $SnapviewP.$srccodeP; # 拼接成完整路径
#			print "WsrccodeP : $WsrccodeP\n";
			&command("disreadonly",$WsrccodeP); # 调用更新源码命令并输出错误信息到error.log中
		}
		&geterror; # 有错误信息则写入BuildError.log
	}
}
# 模块编译'编译'子过程
sub compile
{
	# input: $SnapviewP , $compileFP , $compileFN , $module , $OSstring , $workpath
	# output: $module_$OSstring_cmo.txt
	$place = "Compile";
	print "\n$module - $place......\n";
	if (( $compileFN ne "" ) && $updateS )
	{
		my $WcompileFP = $SnapviewP.$compileFP;
#		print "WcompileFP : $WcompileFP\n";
		my $WcompileIF = $SnapviewP.$compileIFP.$module."_".$OSstring.".txt"; # 定义由CMO获取的编译信息文件名称
		if ( chdir($WcompileFP) ) # 进入编译路径
		{
			my $currenttime = &getdatetime;
			&writelog($verworkP."buildtime.log","$module Compile Time : $currenttime"); # 将所有文件校验结果写入日志文件
#			print BUILDINFO ("\n$module Compile Time : $currenttime"); # 将编译起始时间写入编译信息文件
			&command("exe",$compileFN,$WcompileIF); # 调用编译命令并输出错误信息到CMO的编译信息文件中
			$currenttime = &getdatetime;
			&writelog($verworkP."buildtime.log","~ $currenttime\n"); # 将所有文件校验结果写入日志文件
#			print BUILDINFO ("~ $currenttime\n"); # 将编译终止时间写入编译信息文件
			&printerror(0,"Failed Access Work Path '$workpath' !\n") if ( !chdir($workpath) ); # 返回当前工作路径
		}
		else
		{
			$compileS = 0;
			&printerror(0,"Failed Access Compile Path '$WcompileFP' !\n");
			&printerror(0,"Skip Compile Process !\n");
		}
	}
	else
	{
		$compileS = 0;
		&printerror(0,"Failed in Update Process !\n") if ( !$updateS );
		&printerror(0,"Failed Get Compile File Config !\n") if ( $compileFN ne "" );
		&printerror(0,"Skip Compile Process !\n");
	}
}
# 模块编译'校验文件'子过程
sub checkfile
{
	# input: @checkFs
	# output: $checkfileS , checkfile.log
	$place = "Check_File";
	print "\n$module - $place......\n";
	my ($errorinfo,$passinfo,$nopassinfo);
	if ( @checkFs != 0 ) # 从模块配置文件获取的校验文件信息不为空时
	{
		my ($checkF,$WcheckF);
		undef(@releaseFs); # 清除已定义的发布文件，校验通过的文件才发布
		my $date4 = &getdate; # 获取当前日期yyyymmdd # chenhuiren 090610 
		my $date2 = substr($date4,2);# 获取当前日期yymmdd # chenhuiren 090610
		# chenhuiren 090610 start
		foreach $checkF (@checkFs)
		{
			$checkF =~ s/(y{4}mmdd)/$date4/ig;
			$checkF =~ s/(y{2}mmdd)/$date2/ig;
		}
		# chenhuiren 090610 end
		foreach $checkF (@checkFs) # 处理每一个校验文件
		{
			my @value = &getvalue (" ",$checkF); # 获取该校验文件信息 0文件名称 1文件路径 2发布路径列表
			$WcheckF = $SnapviewP.$commoncompP.$value[1].$value[0];
#			print "WcheckF : $WcheckF\n";
			if (( -e $WcheckF ) && ( -s $WcheckF )) # 校验规则：校验文件存在且不为空
			{
				push (@releaseFs,$checkF) if ( $value[2] ne "" ); # 校验通过且发布路径不为空则推入releaseFs格式为: 0文件名称 1文件路径 2发布路径列表
				$passinfo = $passinfo."Pass 通过 ".$WcheckF."\n";
			}
			else
			{
				$checkfileS = 0; # 只要有一个文件校验不通过,校验结果即为0
				$nopassinfo = $nopassinfo."NotPass 不通过 ".$WcheckF."\n";
			}
		}
		$allcheckfileS = 0 if ( !$checkfileS );
		&writelog($verworkP."nopassfile.log","--- $module / $owner ---\n".$nopassinfo."\n") if ( !$checkfileS ); # 将校验未通过文件写入临时文件nopassfile.log
		$CFpassinfo = $nopassinfo.$passinfo;
		print "Check Files Result :\n$CFpassinfo";
	}
	else
	{
		$errorinfo = "Not Config Check Files in $WmoduleCF !\n";
	}
	&writelog($verworkP."checkfile.log","$builddatetime / $module / $owner\n".$CFpassinfo.$errorinfo."\n"); # 将所有文件校验结果写入日志文件
	return $checkfileS;
}
# 模块编译'校验readme'子过程
sub checkreadme
{
	# input: $SnapviewP , $readmeFP , $readmeFN
	# output: $checkreadmeS , checkreadme.log
	# readme校验规则 : 找当天日期的行 , 找到则校验通过 , 且获取该行起到第一个由"="组成的行之间的所有行到AllReadme.txt中
	$place = "Check_Readme";
	print "\n$module - $place......\n";
	my $errorinfo;
	if ( $readmeFN ne "" ) # 模块配置文件中配置了readme
	{
		my $WreadmeF = $SnapviewP.$readmeFP.$readmeFN;
#		print "WreadmeF : $WreadmeF\n";
		if ( open(README,$WreadmeF) )
		{
			my @readme = <README>;
			close(README);
#			my $mydate = &getdate; # readme校验规则 , 暂以当天日期校验
			$mydate = substr($builddate,2); # 根据公司的readme规范 , 校验日期格式为YYMMDD
			my @found = grep(/^\s*版本号.+$mydate\s*$/i,@readme);
			if ( @found == 0 ) # 未找到满足条件的行
			{
				$checkreadmeS = 0;
				$RMpassinfo = "NotPass 不通过 ".$WreadmeF."\n";
			}
			else  # 找到满足条件的行
			{
				$RMpassinfo = "Pass 通过 ".$WreadmeF."\n";
				my ($line,@lines);
				my $start = 0; # 记录从readme文件中找到符合校验条件的第一行
				my $end = 0; # 记录从readme文件中找到符合校验条件之后的最后一行
				foreach $line (@readme)
				{
					$start = 1 if ( $line =~ /^\s*版本号.+$mydate\s*$/i );
					$end = 1 if ( $start && ( $line =~ /^=+$/i ));
					push (@lines,$line) if ( $start && !$end );
					last if ( $end );
				}
				&writelog($verworkP."AllReadme.txt","$builddatetime / $module / $owner\n"."@lines"."\n");
			}
		}
		else
		{
			&printerror(0,"Failed Open file '$WreadmeF' !\n");
			$errorinfo = "Failed Open file $WreadmeF !\n";
		}
		&writelog($verworkP."nopassreadme.log","--- $module / $owner ---\n".$RMpassinfo."\n") if ( !$checkreadmeS ); # 将校验未通过文件写入临时文件nopassreadme.log
#		print "checkreadmeS : $checkreadmeS\n$RMpassinfo";
		print "Check Readme Result :\n$RMpassinfo";
	}
	else
	{
		$errorinfo = "Readme File was Not Config in $WmoduleCF !\n";
	}
	# 将readme校验结果写入日志文件
	&writelog($verworkP."CheckReadme.log","$builddatetime / $module / $owner\n".$RMpassinfo.$errorinfo."\n"); # 将readme校验结果写入日志文件
	return $checkreadmeS;
}
# 模块编译'发布文件和目录'子过程
sub releasefiles
{
	# input: $RP , $SnapviewP , $commoncompP , @releaseFs , @releaseDs
	# output: none
	$place = "Release_Files_Dirs";
	print "\n$module - $place......\n";
	# 发布校验文件
	if ( @releaseFs != 0 ) # 从模块配置文件获取的校验文件发布信息或经校验后的文件发布信息不为空时
	{
		print "$module - Release_File...\n";
		my ($releaseF,$WRF,$FRP,$WFRP,@FRPs);
		foreach $releaseF (@releaseFs) # 处理每一个发布文件
		{
			my @value = &getvalue(" ",$releaseF); # 获取该发布文件信息 0文件名称 1文件路径 2发布路径列表
			$WRF = $SnapviewP.$commoncompP.$value[1].$value[0];
			@FRPs = &getvalue(",",$value[2]); # 将发布路径列表转为数组
#			@FRPs = split(/,/,$value[2]); # 将发布路径列表转为数组
			foreach $FRP (@FRPs)
			{
				$FRP = &revisepath(0,$FRP); # 修正路径为可用格式
				$WFRP = $RP.$FRP;
#				print "WRF : $WRF\nWFRP : $WFRP\n";
				&command("copy","f",$WRF,$WFRP); # 调用覆盖拷贝源码命令并输出错误信息到error.log中
			}
		}
	}
	# 发布文件和文件夹
	if ( @releaseDs != 0 ) # 从模块配置文件获取的发布目录信息不为空时
	{
		print "$module - Release_Dir...\n";
		my ($releaseD,$WRD,$DRP,$WDRP,@DRPs);
		foreach $releaseD (@releaseDs) # 处理每一个发布目录
		{
			my @value = &getvalue(" ",$releaseD); # 获取该发布目录信息 0目录名称 1目录路径 2发布路径列表
			my $RDP = &revisepath(0,$value[1]); # 修正路径为可用格式
			$WRD = $SnapviewP.$commoncompP.$RDP.$value[0];
			@DRPs = &getvalue(",",$value[2]); # 将发布路径列表转为数组
#			@DRPs = split(/,/,$value[2]); # 将发布路径列表转为数组
			my $file; # 用于标识$WRD是文件1还是文件夹0
			if ( -e $WRD )
			{
				$file = 1;
				$file = 0 if ( -d $WRD );
				foreach $DRP (@DRPs)
				{
					$DRP = &revisepath(0,$DRP); # 修正路径为可用格式
	#				print "WRD : $WRD\nWDRP : $WDRP\n";
					if ( $file )
					{
						$WDRP = $RP.$DRP;
						&command("copy","f",$WRD,$WDRP); # 调用覆盖拷贝源码命令并输出错误信息到error.log中
					}
					else
					{
						$WDRP = $RP.$DRP.$value[0];
						$WDRP = $RP.$DRP if ( $OS );
						&command("copy","d",$WRD,$WDRP); # 调用覆盖拷贝源码命令并输出错误信息到error.log中
					}
				}
			}
			else
			{
				&printerror(0,"Failed Found Release File or Dir '$WRD' !\n");
			}
		}
	}
	&geterror; # 有错误信息则写入BuildError.log
}
# 模块编译'发布readme'子过程
sub releasereadme
{
	# input: $SnapviewP , $readmeFP , $readmeFN , $RP
	# output: none
	$place = "Release_Readme";
	print "\n$module - $place......\n";
	if ( $readmeFN ne "" ) # 模块配置文件中配置了readme
	{
		my $WreadmeF = $SnapviewP.$readmeFP.$readmeFN;
		my $WreadmeFRP = &revisepath(1,$RP."readme"); # 修正路径为可用格式
#		print "WreadmeF : $WreadmeF\nWreadmeFRP : $WreadmeFRP\n";
		if ( -e $WreadmeF )
		{
			&command("copy","f",$WreadmeF,$WreadmeFRP); # 调用覆盖拷贝源码命令并输出错误信息到error.log中
			# 将发布位置上的readme文件更名为以模块名加"_"加原readme文件名
			rename($WreadmeFRP.$readmeFN,$WreadmeFRP.$module."_".$readmeFN) || &printerror(0,"Failed Rename File '$WreadmeFRP$readmeFN' !\n");
			&geterror; # 有错误信息则写入BuildError.log
		}
		else
		{
			&printerror(0,"Failed Found Readme File '$WreadmeF' !\n");
		}
	}
}
# 模块编译'发布compileinfo'子过程
sub releasecompileinfo
{
	# input: $SnapviewP , $compileIFP , $module , $RP
	# output: none
	$place = "Release_CompileInfo";
	print "\n$module - $place......\n";
	if ( $compileS )
	{
		my $WcompileIF = $SnapviewP.$compileIFP.$module."*"; # 本模块的编译信息为编译信息路径下所有以模块名打头的文件
		my $WcompileIFRP = &revisepath(1,$RP."compileinfo"); # 修正路径为可用格式
#		print "WcompileIF : $WcompileIF\nWcompileIFRP : $WcompileIFRP\n";
		&command("copy","f",$WcompileIF,$WcompileIFRP); # 调用更新拷贝源码命令并输出错误信息到error.log中
		&geterror; # 有错误信息则写入BuildError.log
	}
}
# 模块编译'发布updatelog'子过程
sub releaseupdatelog
{
	# input: $updateL , $module , $RP
	# output: none
	$place = "Release_UpdateLog";
	print "\n$module - $place......\n";
	if ( $updateS )
	{
		#my $WupdateL = $verworkP."$module"."_".$builddatetime.".updt"; # 根据本脚本中即定的updatelog命名规则和位置发布
		
		my $WupdateL = $verworkP."$module"."_".$builddatetime."_update.log"; # 根据本脚本中即定的updatelog命名规则和位置发布 ###modify by hanjian 20120810
		my $WupdateLRP = &revisepath(1,$RP."updatelog"); # 修正路径为可用格式
		
   	print "WupdateL : $WupdateL\nWupdateLRP : $WupdateLRP\n"; ####20120810 --test

		&command("copy","f",$WupdateL,$WupdateLRP); # 调用更新拷贝源码命令并输出错误信息到error.log中
		&geterror; # 有错误信息则写入BuildError.log
	}
}
# 模块编译'通知'子过程
sub notify
{
	# input: $RP , $OS , $ucmproject , $version , $builddatetime , $modulelist , $PMM , $TEM , $SMTP , $mailfrom
	# $CFpassinfo , $RMpassinfo , @allnopassfile , @allnopassreadme , $allcheckfileS
	# output: none
	$place = "Notify";
	print "\n$module - $place......\n";
	my ($type,$rule) = @_;
	my (@text,$to,$subject,$message,$notifyRP);
	$notifyRP = $RP;
	if ( $OS ) # 如果是linux操作系统 , 则需要将notifyRP转换为windows的路径格式
	{
		$OS = 0;
		$notifyRP = &revisepath(1,$RP);
		$OS = 1;
	}
#	@allnopassreadme = ("无未通过readme\n") if ( @allnopassreadme == 0 );
	$text[0] = "AutoBuild_Notify: ";
	&getallmodules;
	$text[1] = " , 您好!\n\n本次编译信息如下: \n项目名称: $UCMprj\n项目版本号: $version\n自动脚本运行开始时间: $builddatetime\n参与本次自动脚本运行的模块列表: \n"."@allmodules"."\n版本发布位置: $notifyRP\n\n";
	undef(@allmodules);
	$text[2] = "编译环境及编译时间等: 请在发布位置下的BuildInfo文件中查找;\n源码更新记录: 请在发布位置下的updatelog文件夹中查找;\n编译信息: 请在发布位置下的compileinfo文件夹中查找.\n(ReadMe: 请在发布位置下的readme文件夹中查找.)\n\n如有疑问请致电您的配置管理员或发送邮件到$CMOM。\n该邮件由系统发送请勿回复。谢谢。\n\n";
	if ( !$checkfileS )
	{
		open (NOPASSF,$verworkP."nopassfile.log"); # 读取校验未通过文件的临时文件nopassfile.log
		my @allnopassfile = <NOPASSF>;
		close (NOPASSF);
		@allnopassfile = ("无未通过文件\n") if ( @allnopassfile == 0 );
		if ( $type )
		{
			$text[3] = "文件校验未通过";
			$text[4] = "校验文件结果: \n$CFpassinfo\n";
			$text[5] = "附: 本模块之前所有校验未通过的文件\n@allnopassfile\n";
		}
		else
		{
			$text[5] = "附1.1: 所有校验未通过的文件\n@allnopassfile\n";
		}
	}
	#########add by hanjian 20120417(mail update.log)################hanjian 20120810 
	open (UPDATE333,$verworkP."all-code_".$builddatetime."_update.log"); # 读取文件update.log
		my @updatelogss = <UPDATE333>;
		close (UPDATE333);
##		@updatelogss = ("各模块源码更新记录: 请在发布位置下的updatelog文件夹中查找\n") if ( @updatelogss == 0 );
##	  $text[6]= $text[6]."整体的updatelog:\n @updatelogss\n";
	  if ( @updatelogss == 0 )
	  {
	  	$text[6]= "各模块源码更新记录: 请在发布位置下的updatelog文件夹中查找\n";
	  }
	  else
	  {
	  	$text[6]= $text[6]."整体的updatelog:\n @updatelogss\n";
	  } 
	  
	################################################################
	if ( $CRM && !$checkreadmeS )
	{
		open (NOPASSRM,$verworkP."nopassreadme.log"); # 读取校验未通过readme的临时文件nopassreadme.log
		my @allnopassreadme = <NOPASSRM>;
		close (NOPASSRM);
		@allnopassreadme = ("无未通过readme\n") if ( @allnopassreadme == 0 );
		if ( $type )
		{
			$text[3] = $text[3]."Readme校验未通过";
			$text[4] = $text[4]."校验Readme结果: \n$RMpassinfo\n";
		}
		else
		{
			$text[5] = $text[5]."附1.2: 所有校验未通过的readme\n@allnopassreadme\n";
		}
	}
	if ($type) # 单个模块的通知规则 , 该规则与spbuild或intbuild有关 , 仅校验不通过时才通知
	{
		$subject = $text[0]."$owner 模块$module参与$UCMprj $version $builddatetime的编译 , $text[3]!";
		$text[4] = "您负责的模块: $module\n".$text[4];
		$message = $owner.$text[1].$text[4].$text[2].$text[5];
		$to = $ownerM;
		$to = $to.",".$gownerM if ( $rule ); # 如果是intbuild , 则单个模块通知小组负责人
		&writelog($verworkP."sendmail.log","$builddatetime / $module / $to\n");
		&mail($to,$subject,$message);
	}
	else # 所有模块的通知规则 , 该规则与spbuild或intbuild有关 , 编译报告
	{
		$subject = $text[0]."$UCMprj $version $builddatetime的编译报告";
		$message = "$UCMprj $version 版本相关负责人".$text[1].$text[2].$text[5].$text[6];
		if ($rule) # intbuild通知规则
		{
			$to = $CMOM.",".$PMM.",".$TEM;
		}
		else # spbuild通知规则
		{
			$to = $CMOM;
			$to = $to.",".$TEM if ( $allcheckfileS );
		}
		&writelog($verworkP."sendmail.log","$builddatetime / $to\n");
		&mail($to,$subject,$message);
	}
}
# 用于发送邮件
sub mail
{
	# input: recipients , attachment
	# output: email status log
	my ($to,$subject,$message) = @_;
	$place = "Send_Mail";
	print "$place...\n";
	use Mail::Sendmail;
	%mail = ( To      => $to,
	          From    => $mailfrom,
	          Subject => $subject,
	          Message => $message
	         );
	$mail{Smtp} = $SMTP;
	$mail{'Content-Type'} = 'text/plain; charset="GB2312"';
	if ( sendmail(%mail) )
	{
		&writelog($verworkP."sendmail.log","Successfully Send Mail !\n".$Mail::Sendmail::log."\n"); # 将所有文件校验结果写入日志文件
		print "Successfully Send Mail !\n";
	}
	else
	{
		&writelog($verworkP."sendmail.log","Failed Send Mail !\n".$Mail::Sendmail::error."\n"); # 将所有文件校验结果写入日志文件
		&printerror(0,"Failed Send Mail !\n");
	}
	no Mail::Sendmail;
}
# 获取参与脚本运行的所有模块 , 用于写入BUILDINFO,AllReadme(如果被调用)和邮件发送
sub getallmodules
{
	# input: allmodules.log
	# output: @allmodules
	if ( open (ALL,$verworkP."allmodules.log") ) # 读取校验未通过readme的临时文件nopassreadme.log
	{
		@allmodules = <ALL>;
		close(ALL);
	}
	else
	{
		&printerror(0,"Failed Open File '$verworkP"."allmodules.log' !\n");
	}
}
# =====================================================================================================================
# 系统命令调用函数
sub command
{
	# input: commandkey (update , exe , copy , del , updatecopy , pause), command items
	# output: $vererrL
	my ($cmd,$item0,$item1,$item2) = @_;
	if ( $cmd eq "env" )
	{
		print "Command : $cmd : $item0\n";
		my $output = "";
		if ( $OS )
		{
			$output = `env | grep $item0`;
		}
		else
		{
			$output = `set $item0`;
		}
		return $output;
		undef($output);
	}
	elsif ( $cmd eq "del" )
	{
		print "Command : $cmd : $item1\n";
		if ( $OS )
		{
			if ($item0 eq "d"){
				system("rm -fr $item1 2>$vererrL"); # 递归删除目录下所有文件和目录
			}else{ # if ($item0 eq "f")
				system("rm -f $item1 2>$vererrL");
			}
		}
		else
		{
			if ($item0 eq "d"){
				system("del /s /f /q $item1 2>>$vererrL"); # 递归删除目录下所有文件
			}else{ # if ($item0 eq "f")
				system("del /f /q $item1 2>>$vererrL");
			}
		}
	}
	elsif ( $cmd eq "update" )
	{
		print "Command : $cmd : $item0 $item1\n";
		######system("cleartool update -f -ove -log $item1 $item0 2>$vererrL");
		if ( $OS )
		{
			#system("export LANG="en_US.UTF-8"");
				$ENV{LANG} = "en_US.UTF-8";
        system("svn update  --accept theirs-full --username kdvsvnlinux --password 'kdvsvnkdvsvn' --no-auth-cache $item0  >$item1");####--- modify by hanjian 20120821  for update 
		    system("svn update  --accept theirs-full --username kdvsvnlinux --password 'kdvsvnkdvsvn' --no-auth-cache $item0 2>$vererrL");
		}
		else
		{
			  system("svn update  --accept theirs-full --username build90 --password kdv40build08 --no-auth-cache $item0  >$item1");####--- modify by hanjian 20120821  for update 
		    system("svn update  --accept theirs-full --username build90 --password kdv40build08 --no-auth-cache $item0 2>$vererrL");
		}
	  
	  
		
		#system("svn update --username root --password 'kdckdc' --no-auth-cache $item0  >$item1");####--- modify by hanjian 20120803  for update 
		#system("svn update --username root --password 'kdckdc' --no-auth-cache $item0 2>$vererrL"); ####---- add by hanjian 20120803  for update 
	}
	elsif ( $cmd eq "disreadonly" )
	{
		print "Command : $cmd : $item0\n";
		$item0 = $item0."*"; # 对disreadonly参数路径的特殊处理
		if ( $OS )
		{
			system("chmod -R 755 $item0 2>$vererrL"); # 递归去除目录$item0下所有文件的只读属性
		}
		else
		{
			system("attrib -R $item0 /s 2>>$vererrL"); # 递归去除目录$item0下所有文件的只读属性
		}
	}
	elsif ( $cmd eq "exe" )
	{
		my $mycurrpath = &getcurrpath;
		print "Command : $cmd : $mycurrpath$item0 $item1\n";
		if ( $OS )
		{
			system("./$item0 >$item1 2>&1"); # 执行文件$item0 , 写入日志$item1
		}
		else
		{
			system("$item0 >$item1 2>>&1"); # 执行文件$item0 , 写入日志$item1
		}
		undef($mycurrpath);
	}
	elsif ( $cmd eq "copy" )
	{
		print "Command : $cmd : $item1 $item2\n";
		if ( $OS )
		{
			system("mkdir -p $item2 2>$vererrL") if ( !-e $item2 ); # 由于linux复制命令cp不支持自动创建父目录 , 因此先判断目标目录是否存在 , 目标目录不存在，则先创建该目录
			if ($item0 eq "d"){
				system("cp -fr --preserve=timestamps --dereference $item1 $item2 2>$vererrL");
			}else{ # if ($item0 eq "f")
				system("cp -f --preserve=timestamps --dereference $item1 $item2 2>$vererrL");
			}
		}
		else
		{
			if ($item0 eq "d"){
				system("xcopy $item1 $item2 /e /i /y /c /k /v 2>>$vererrL");
			}else{ # if ($item0 eq "f")
				system("xcopy $item1 $item2 /i /y /c /k /v 2>>$vererrL");
			}
		}
	}
	elsif ( $cmd eq "mkbl" )
	{
		my $mycurrpath = &getcurrpath;
		print "Command : $cmd : $mycurrpath\n";
		my $output = `cleartool mkbl -all 2>$vererrL`;
		return $output;
		undef($output);
	}
	elsif ( $cmd eq "chstream" )
	{
		my $mycurrpath = &getcurrpath;
		print "Command : $cmd : $mycurrpath\n";
		system("cleartool chstream -recommended -default -cview 2>$vererrL");
	}
	elsif ( $cmd eq "lsstream" )
	{
		my $mycurrpath = &getcurrpath;
		print "Command : $cmd : $mycurrpath\n";
		my $output = `cleartool lsstream -fmt \"\%\[rec\_bls\]p\\n\" -cview 2>$vererrL`;
		return $output;
		undef($output);
	}
	elsif ( $cmd eq "setcs" )
	{
		my $mycurrpath = &getcurrpath;
		print "Command : $cmd : $mycurrpath\n";
		system("cleartool setcs -stream 2>$vererrL");
	}
	elsif ( $cmd eq "mkdir" )
	{
		print "Command : $cmd : $item0\n";
		if ( $OS )
		{
			system("mkdir --mode=750 -p $item0 2>$vererrL");
		}
		else
		{
			system("mkdir $item0 2>$vererrL");
		}
	}
	elsif ( $cmd eq "pause" ) # 暂停命令
	{
		print "Press Enter to continue ...\n";
		<STDIN>;
#		`pause`;
	}
}
# 用于将指定的字串打印到屏幕 , 错误信息日志BUILDERROR和编译信息BUILDINFO , 并提供正确引导信息
sub printerror
{
	# input: print type , print string , show key
	# output: none
	my ($type,$string,$key) = @_;
	$string = "Error : $module : $place : ".$string if ( $type == 0 );
	print BUILDERROR $string; # 将打印的字串写入日志 , 记录模块和位置
	print $string;  # 打印字串到屏幕
#	if ( $key ne "" )
#	{
#		my @found = &getline(0,"#$key",@demofile);
#		if ( @found != 0 )
#		{
#			print join("\n",@found); # 从引导文件读取以打印关键字开头的正确引导信息
#			print "\n";
#		}
#		@found = &getline(0,"# ",@demofile);
#		if ( @found != 0 )
#		{
#			print join("\n",@found); # 从引导文件读取通用的正确引导信息
#			print "\n";
#		}
#		@found = &getline(0,"$key",@demofile);
#		if ( @found != 0 )
#		{
#			print "Example:\n";
#			print join("\n",@found); # 从引导文件读取通用的正确引导信息
#			print "\n";
#		}
#	}
}
# 将指定的字串写入指定的日志
sub writelog
{
	# input: log file path , string to write
	# output: none
	my ($logfile,$string) = @_;
	if ( open (LOG,">>$logfile") )
	{
		print LOG $string;
		close(LOG);
	}
	else
	{
		&printerror(0,"Failed Open File '$logfile' !\n");
	}
}
# 读error文件,根据类型判断是否报错,之后打印日志并清除error文件
sub geterror
{
	# input: type(err: return 1 when found "error"; warn: return 1 when found "error" or "warnning"; except: return 0 when found except $string in first line )
	# output: found resule
	my ($type,$string) = @_;
	my $result = 0;
	if ( -s $vererrL )
	{
		if ( open (ERROR,$vererrL) )
		{
			my @errors = <ERROR>;
			close (ERROR);
			if ( @errors > 0 )
			{
				if ( $type =~ /err/i )
				{
					my @found = grep(/error/i,@errors);
					$result = 1 if ( @found > 0 );
				}
				elsif ( $type =~ /warn/i )
				{
					my @found = grep(/warn|error/i,@errors);
					$result = 1 if ( @found > 0 );
				}
				elsif ( $type =~ /except/i )
				{
					$result = 1;
					$result = 0 if (( @errors == 1 ) && ( $string ne "" ) && ( $errors[0] =~ /$string/i ));
				}
				else
				{
					$result = 1;
				}
				&printerror(0,"@errors");
			}
			else
			{
				$result = 0;
			}
		}
		else
		{
			&printerror(0,"Failed Open Error File '$vererrL' !\n");
			$result = 1;
		}
	}
	# 清除error文件内容
	if ( open (ERROR,">$vererrL") )
	{
		close (ERROR);
	}
	else
	{
		&printerror(0,"Failed Clean Error File '$vererrL' !\n");
	}
	return $result;
}

# 清除文件内容
sub cleanerror
{
	# input: none
	# output: none
	undef (@errors);
	if ( open (ERROR,">$vererrL") )
	{
		close (ERROR);
		return 1;
	}
	else
	{
		&printerror(0,"Failed Clean Error File '$vererrL' !\n");
		return 0;
	}
}

# =====================================================================================================================
# 修正路径为可用格式 , 可用格式为: 所有路径根据$OS均以'\'或'/'结尾 ; 根据修正类型不同去掉开头的'\'或'/' , 或以'\\''/'开头
sub revisepath
{
	# input: 修正类型(1-修正开头 , 0-不修正开头) , 修正路径
	# output: 可用格式的路径 : 1-linux修正为'/'开头'/'结尾 , windows修正为'\\'开头'\'结尾 ; 0-linux修正为'/'结尾 , windows修正为'\'结尾 , 不处理开头
	my ($type,$path) = @_;
	if ( $path =~ /^[\\\/]+$/ )
	{
		return "";
	}
	else
	{
		my (@part,$path1);
		@part = split(/[\\\/]+/,$path); # 以多个连续'\'或'/'的组合切分需修正的路径 , 与操作系统无关
		shift(@part) if ( $part[0] eq "" ); # 如果'\'或'/'打头 , 则去掉切分数组的第一个元素 , 在连接数组元素时即可去除开头的'\'或'/'
#		shift(@part) if ( $path =~ /^[\\\/]+/ );
		if ( $OS ) # linux
		{
			$path1 = join("/",@part); # 以'/'连接
			$path1 = $path1."/" if ( $type != 2 ); # 由于连接时自动去除了尾部的'/' , 因此加上
			$path1 = "/".$path1 if (( $type == 1 ) && ( $path =~ /^[\\\/]+/ )); # 若以'/'开头同时修正类型为修正开头 , 则以'/'打头
		}
		else # windows
		{
			$path1 = join("\\",@part); # linux以'\'连接
			$path1 = $path1."\\" if ( $type != 2 ); # 由于连接时自动去除了尾部的'\' , 因此加上
			$path1 = "\\\\".$path1 if (( $type == 1 ) && ( $path =~ /^[\\\/]+/ )); # 若以'/'开头同时修正类型为修正开头 , 则以'\\'打头代表网络路径
		}
		return $path1;
	}
}
# 从指定数组中查找以指定关键字打头的元素(在多行中查找满足条件的行)
sub getline
{
	# input: 查找类型(0-以关键字开头 1-以关键字加"="开头) , lines to be find , 查找关键字
	# output: lines that beginwith the keyword, ignore blank/tab
	my ($type,$keyword,@lines) = @_;
	my ($l,@line);
	if ( $type == 0 )
	{
		@line = grep(/^\s*$keyword\s+/i,@lines); # 以关键字+空格或tab打头
	}
	elsif ( $type == 1 )
	{
		@line = grep(/^\s*$keyword\s*=/i,@lines); # 均忽略关键字和"="前后的空格和tab
	}
	elsif ( $type == 2 )
	{
		@line = grep(/^\s*$keyword.*\s+/i,@lines); # 以关键字+任意字符+空格或tab打头
	}
	foreach $l (@line)
	{
		$l =~ s/^\s+|\s+$//g ; # 去除已找到行的首尾空格和tab
	}
	return (@line);
}
# 以指定关键字切分字符串
sub getvalue
{
	# input: 分割符 , 被切字符串
	# output: 分割好的各部分
	my ($separator,$line) = @_;
	my @value;
	if ( $separator eq " " )
	{
		@value = split(/\s+/,$line); # 忽略分割符前后的空格和tab
	}
	else
	{
		@value = split(/\s*$separator+\s*/i,$line); # 忽略分割符前后的空格和tab
	}
	shift(@value) if ( $value[0] eq "" );
	return (@value);
}
#
sub shiftvalue
{
	# input: $key , 
	# output: 分割好的各部分
	my ($value,@array) = @_;
	my $a;
	foreach $a (@array)
	{
		$a =~ s/^$value\s+//ig; # 去除打头的值和空格
	}
	return @array;
}
# 获取当前工作路径
sub getcurrpath
{
	# get current work dir
	my $curpath;
	if ( $OS )
	{
		$curpath = `pwd`;
	}
	else
	{
		$curpath = `cd`;
	}
	chomp($curpath);
	$curpath = &revisepath(1,$curpath);
	return $curpath;
}
# 获取当天日期yymmdd
sub getdate
{
    my ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $time) = localtime();
    return sprintf("%4d%2.2d%2.2d", $year + 1900, $mon + 1, $mday);
}
# 获取当天日期-时间yymmdd-hhmm
sub getdatetime
{
	# input: none
	# output: current date-time
    my ($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $time) = localtime();
    return sprintf("%4d%2.2d%2.2d-%2.2d%2.2d", $year + 1900, $mon + 1, $mday,  $hour, $min);
}
# =====================================================================================================================
return 1;
# End