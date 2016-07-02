#!usr/bin/perl
# ======================================================================
#
# Perl Source File -- Created with SAPIEN Technologies PrimalScript 3.1
#
# NAME: module build process
#
# AUTHOR: pengjing
# DATE  : 2007-7-26 ~ 2007-9-4
#
# PURPOSE: 
#
# ======================================================================
#
# parameter usage -----
# 0 options : not necessary
# 0.0 defause none : whole build process , check file , notify bases spbuild rule
# 0.1 -u : update src code
# 0.2 -b : build(compile)
# 0.30 -c : check file and readme
# 0.31 -cf : check file
# 0.4 -r : release
# 0.50 -n : notify bases on spbuild rule , must be used with -c that means check and notify
# 0.51 -ni : notify bases on intbuild rule , must be used with -c that means check and notify
# 1 version : the version of the project , necessary
# 2 modulelist : module of the version , necessary
# 3 specRP : specifical release path , not necessary
#
# log file define -----
# checkfile.log
# checkreadme.log
# notify.log
# BuildError.log
# BuildInfo.txt
# =====================================================================================================================
print "\n=======================================================================\n";
print "\n                             SP BUILD START                            \n";
print "\n=======================================================================\n";
# 校验脚本输入参数
$place = "Validate_Script_Para";
print "\n$module - $place......\n";
# 以"-"打头则认为输入了options,则参数总数是3或4个
for ( my $i = 0 ; $i < @ARGV; $i++ )
{
	if ( $ARGV[$i] =~ /^-/ ) # "-"标识options
	{
		if ( $spoptions eq "" )
		{
			$spoptions = $ARGV[$i];
			$spoptions =~ s/^-//g; # 去除参数标识符'-'之后进行处理
			if ( $spoptions eq "" )
			{
				print "Invalid Option '$ARGV[$i]' !\n";
				exit 1; # 处理后的关键字仍然为空则退出整个程序
			}
			$spoptions = lc($spoptions); # 将参数转化为小写
			my @part = split("",$spoptions); # 分隔输入的编译过程关键字 , 逐个校验
			my @preoptions = qw(u b c f r n);
			for( my $j = 0; $j < @part ; $j++ ) # 判断输入的关键字是否正确
			{
				my @found = grep(/$part[$j]/,@preoptions);
				if ( @found == 0 )
				{
					print "Unknow Option '$part[$j]' in '$ARGV[$i]' !\n";
					exit 1; # 处理后的关键字仍然为空则退出整个程序
				}
			}
			undef(@preoptions);
			undef(@part);
			undef(@found);
			undef($j);
			if ((( $spoptions =~ /n/ ) && (( $spoptions !~ /c/ ) || ( $spoptions !~ /r/ ))) || ( $spoptions =~ /f/ ) && ( $spoptions !~ /c/ ))
			{
				print "Unable 'Notify' without 'Check' and 'Release', or\nUnable Identify 'i' without 'n' , or Unable Identify 'f' without 'c' !\n";
				exit 1; # 处理后的关键字之间不匹配则退出整个程序(匹配规则为:n必须与c同时使用 , i必须与n同时使用 , f必须与c同时使用)
			}
		}
		else
		{
			print "ReDefine Options '$ARGV[$i]' after '$spoptions' !\n";
			exit 1; # 重定义参数 , 则退出
		}
	}
	elsif ( $ARGV[$i] =~ /^\+/ ) # "-"标识options
	{
		if ( $spenvs eq "" )
		{
			$spenvs = $ARGV[$i];
			$spenvs =~ s/^\+//g; # 去除参数标识符'-'之后进行处理
			if ( $spenvs eq "" )
			{
				print "Invalid Env '$ARGV[$i]' !\n";
				exit 1; # 处理后的关键字仍然为空则退出整个程序
			}
			if ( !open(ENV,$workpath."env.ini") )
			{
				print "Failed Open File '$workpath"."env.ini' !\n";
				exit 1; # 找不到环境变量配置文件则退出整个程序
			}
			@envfile = <ENV>;
			close(ENV);
			my @part = split(/,+/,$spenvs); # 分隔输入的编译过程关键字 , 逐个校验
			shift(@part) if ( $part[0] eq "" );
			for( my $j = 0; $j < @part ; $j++ ) # 判断输入的关键字是否正确
			{
				my @found = grep(/^$part[$j]\s+/i,@envfile);
				if ( @found == 0 )
				{
					print "Unknow Env '$part[$j]' in '$ARGV[$i]' !\n";
					exit 1; # 处理后的关键字仍然为空则退出整个程序
				}
			}
			undef(@envfile);
			undef(@part);
			undef(@found);
			undef($j);
		}
		else
		{
			print "ReDefine Env '$ARGV[$i]' after '$spenvs' !\n";
			exit 1; # 重定义参数 , 则退出
		}
	}
	elsif ( $version eq "" ) # 第一个参数为版本号
	{
		$version = $ARGV[$i];
	}
	elsif ( $modulelist eq "" ) # 第二个参数为模块列表
	{
		$modulelist = $ARGV[$i];
	}
	elsif (( $spspecRP eq "" ) && ( $ARGV[$i] =~ /^[\\\/]+/ )) # 第三个参数为指定发布路径
	{
		$spspecRP = $ARGV[$i];
	}
	else
	{
		print "Redundant ARGVmeters '$ARGV[$i]' !\n";
		exit 1; # 参数数量不正确则退出整个程序
	}
}
undef($i);
if ( $version eq "" )
{
	print "Lost 'Version' ARGVmeters in '@ARGV' !\n";
	exit 1; # 缺少关键参数
}
if ( $modulelist eq "" )
{
	print "Lost 'ModuleList' ARGVmeters in '@ARGV' !\n";
	exit 1; # 缺少关键参数
}
# =====================================================================================================================
# 关键环境变量名"AUTOBUILD_PATH"的值代表本脚本所在路径,由配置管理员手动设置
$place = "Get_AutoBuild_ENV";
print "\n$module - $place......\n";
$autobuildpath = $ENV{'AUTOBUILD_PATH'};
if ( -e $autobuildpath && chdir($autobuildpath) )
{
	push (@INC,$autobuildpath);
	$workpath = $autobuildpath;
}
else
{
	print "Warnning : OS ENV 'AUTOBUILD_PATH' Not Exit or Incorrectly Config !\n\n";
}
require ($workpath."common.pl");
if ( $spoptions eq "" )
{
	if ( $OS && $ISM )
	{
		&printerror(1,"Linux SourceCode is mounted , Skip Update View !\n");
		$spoptions = "bcrn";
	}
	else
	{
		$spoptions = "ubcrn";
	}
}
# =====================================================================================================================
# 如果编译 , 则设置编译环境变量
if ( $spoptions =~ /b/ )
{
	$place = "Get_Build_Env";
	print "\n$module - $place......\n";
	if ( $spenvs eq "" )
	{
		$spenvs = $defenv;
	}
	else
	{
		$spenvs = $spenvs.",".$defenv if ( $spenvs =~ /^,*GROUPS[\d\._]*,*$/i );
	}
}
else
{
	$spenvs = "-" if ( $spoptions !~ /u/ );
}
# 如果发布 , 则处理发布路径
if ( $spoptions =~ /r/ )
{
	# 获取最终的发布路径
	if ( $spspecRP eq "" ) # 当用户没有指定发布路径 , 根据发布规则在发布路径后加日前文件夹/日前时间文件夹以区分每次发布
	{
		my $p = &revisepath(0,"$builddate-SP/$builddatetime");
		$RP = $ReleaseP.$p;
	}
	else
	{
		$RP = &revisepath(1,$spspecRP);
	}
	&printerror(1,"Build_Single : ReleasePlace : $RP\n");
}
undef($spspecRP);
# =====================================================================================================================
@spmodules = &getvalue(",",$modulelist);
# 在调用预处理之前 , 写入allmodules.log , 用于写入BUILDINFO,AllReadme(如果被调用)和邮件发送
&writelog($verworkP."allmodules.log","@spmodules");
undef($modulelist);
# 编译过程开始之前的预处理
&preprocess($spoptions);
# 编译过程开始
$place = "Module_Process";
print "\n$module - $place......\n";
&module($spoptions,$spenvs,@spmodules);
undef($spenvs);
undef(@spmodules);
# 编译过程之后的续处理
&afterprocess($spoptions);
undef($spoptions);
# =====================================================================================================================
# 关闭编译错误信息文件
close(BUILDERROR);
print "\n================================= END =================================\n";
#&command("pause"); # chenhuiren 090610 
# End