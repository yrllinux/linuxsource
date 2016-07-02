#!usr/bin/perl
#
# Perl Source File -- Created with SAPIEN Technologies PrimalScript 3.1
#
# NAME: module build process
#
# AUTHOR: chenhuiren , xuping ,pengjing
# DATE  : 
#
# PURPOSE: 
#
# =====================================================================================================================
print "\n=======================================================================\n";
print "\n                            INT BUILD START                            \n";
print "\n=======================================================================\n";
# 校验脚本输入参数
$place = "Validate_Script_Para";
print "\n$module - $place......\n";
# def 3
for ( my $i = 0 ; $i < @ARGV; $i++ )
{
	if ( $ARGV[$i] =~ /^-/ ) # "-"标识options
	{
		if ( $intoptions eq "" )
		{
			$intoptions = $ARGV[$i];
			$intoptions =~ s/^-//g; # 去除参数标识符'-'之后进行处理
			if ( $intoptions eq "" )
			{
				print "Invalid Option '$ARGV[$i]' !\n";
				exit 1; # 处理后的关键字仍然为空则退出整个程序
			}
			$intoptions = lc($intoptions); # 将参数转化为小写
			if (( $intoptions ne "a" ) && ( $intoptions ne "s" ))
			{
				print "Invalid Option '$ARGV[$i]' !\n";
				exit 1; # 处理后的关键字仍然为空则退出整个程序
			}
		}
		else
		{
			print "ReDefine Options '$ARGV[$i]' after '$intoptions' !\n";
			exit 1; # 参数数量不正确则退出整个程序
		}
	}
	elsif ( $version eq "" ) # 第一个参数为版本号
	{
		$version = $ARGV[$i];
	}
	elsif (( $intspecRP eq "" ) && ( $ARGV[$i] =~ /^[\\\/]+/ )) # 第三个参数为指定发布路径
	{
		$intspecRP = $ARGV[$i];
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
if (( $intoptions eq "a" ) && ( $intspecRP ne "" ))
{
	print "Not Match options '$intoptions' and specRP '$intspecRP' in '@ARGV' !\n";
	exit 1; # 缺少关键参数
}
require ("common.pl");





# =====================================================================================================================
# 获取模块列表
$place = "Get_Module_List";
print "\n$module - $place......\n";
if ( !open (MODULES,$workpath."$version.ini") )
{
	&printerror(0,"Failed Open File '$workpath$version".".ini' !\n");
	&command("pause");
	exit 1;
}
@modulefile = <MODULES>;
close(MODULES);
chomp(@modulefile);
if ( $OS )
{
	@modulelines = &getline(2,"l",@modulefile);
}
else
{
	@modulelines = &getline(2,"w",@modulefile);
}
undef(@modulefile);
if ( @modulelines == 0 )
{
	&printerror(0,"Failed Found Modules Group in '$workpath$version".".ini' !\n");
	&command("pause");
	exit 1; # 找不到版本编译模块则退出整个程序
}
# def 3
foreach $moduleline (@modulelines)
{
	my @part = &getvalue(" ",$moduleline); # 0操作系统 1小组负责人邮件 2环境关键字列表 3模块列表
	if ( @part < 4 )
	{
		&printerror(0,"Wrong Format in '$moduleline' !\n");
		&command("pause");
		exit 1; # 模块列表格式错误则退出整个程序
	}
	$part[1] = $CMOM if ( $part[1] =~ /^-*$/ );
	$part[2] = $defenv if ( $part[2] =~ /^-*$/ );
	push(@groupowner,$part[1]);
	push(@envkeylist,$part[2]);
	push(@modulelist,$part[3]);
}
undef($moduleline);
undef(@part);
undef(@modulelines);
# 将全部模块写入allmodules.log文件
for ( my $i = 0 ; $i < @modulelist ; $i++ )
{
	my @part = &getvalue(",",$modulelist[$i]);
	&writelog($verworkP."allmodules.log","@part"." ");
}
undef(@part);
undef($i);
# =====================================================================================================================
if ( $OS && $ISM )
{
	&printerror(1,"Linux SourceCode is mounted , Skip Mkbl Setcs and Update View !\n");
}
else
{
	# 获取基线 , 更新源码
	$place = "Access_SnapshotView_Path";
	print "\n$module - $place......\n";
	if ( !chdir($SnapviewP) ) # 进入静态视图编译路径 , 如果无法进入 , 则报错 , 且退出
	{
		&printerror(0,"Failed Access Snapshot View Path '$SnapviewP' !\n");
		&command("pause");
		exit 1;
	}
	# 打基线 , 置推荐基线 , 获取推荐基线 , 过程中如有错误，则报错 , 但不退出
	$place = "Make_Baseline";
	print "\n$module - $place......\n";
	&cleanerror;
	####&command("mkbl");  #hanjian 20120810 svn don't need clearcase command
###	if ( !&geterror )
###	{
###		$place = "Change_Stream";
###		print "\n$module - $place......\n";
###		&cleanerror;
###		&command("chstream");
###		if ( !&geterror )
###		{
###			$place = "Get_BaseLine";
###			print "\n$module - $place......\n";
###			&cleanerror;
###			$baseline = &command("lsstream"); # 取静态视图对应的推荐基线的列表
###			if ( !&geterror )
###			{
###				$baseline =~ s/\s/\n/g; # 字符串中所有的空格都用回车替换
###				&writelog($verworkP."baseline.log",$baseline."\n");
###				print $baseline;
###			}
###			else
###			{
###				&printerror(0,"@errors");
###			}
###			undef($baseline);
###		}
###		else
###		{
###			&printerror(0,"@errors");
###		}
###	}
###	else
###	{
###		&printerror(0,"@errors");
###	}
	# 视图同步和更新源码之前 , Windows操作系统上需要设置环境变量
	if ( !$OS )
	{
		$place = "Set_Update_Env";
		print "\n$module - $place......\n";
		&getenv("GROUPS");
	}
	# 视图同步和更新源码 , 过程中如有错误 , 则报错 , 且退出
	###$place = "Synchronize_View";
	$place = "Update all of the code:";
	
	print "\n$module - $place......\n";
	&cleanerror;
	###&command("setcs");
	###&command("setcs"); # chenhuiren 090610 #hanjian20120810 svn don't need clearcase cpmmand
	if ( &geterror )
	{
		&printerror(0,"@errors");
		&command("pause");
		exit 1;
	}
	$place = "Update_All";
	print "\n$module - $place......\n";
	&cleanerror;
	$SnapviewP1 = $SnapviewP;
	chop($SnapviewP1);
	print "$SnapviewP1 !!!!!!!!!!!******\n";
	&command("update",$SnapviewP1,$verworkP."all-code_".$builddatetime."_update.log");
	undef($SnapviewP1);

	if ( &geterror )
	{
		&printerror(0,"@errors");
		&command("pause");
		exit 1;
	}
	# 发布update LOG ??
	# 回到工作路径
	$place = "Backto_Script_Path";
	&printerror(0,"Warnning:Failed Back to Work Path '$workpath' !\n") if ( !chdir($workpath) );
	# =====================================================================================================================
	# 去除源码只读属性
	$place = "DisReadonly_SourceCode";
	print "\n$module - $place......\n";
	&cleanerror;
	&command("disreadonly",$SnapviewP); # 调用更新源码命令并输出错误信息到error.log中
	&printerror(0,"@errors") if ( &geterror );
}
# =====================================================================================================================



# 编译开始之前的预处理
&preprocess("b");

#======================================================================================================================
# 整体编译之前删除编译信息文件夹中，所有旧的编译信息
# add by hanjian 20120824
 #&deletecompileinfo;

#======================================================================================================================

# 编译开始 , 仅编译
$place = "Module_Process_Compile";
print "\n$module - $place......\n";
for ( my $i = 0 ; $i < @modulelist ; $i++ )
{
	my @intmodules = &getvalue(",",$modulelist[$i]);
	&module("b",$envkeylist[$i],@intmodules);
}
undef(@intmodules);
undef($i);
# 编译之后的续处理 , 放在校验 , 发布 , 通知全部结束后做续处理
# =====================================================================================================================
# 校验 , 发布 , 通知之前的预处理
&preprocess("crni");
# 处理windows和linux协同编译时的发布位置
$RP = &revisepath(1,$ReleaseP.$builddatetime); # 根据发布规则在发布路径后加日前文件夹/日前时间文件夹以区分每次发布
my $sharefile = $ReleaseP."share.txt";
my $D = 0; # 如果share.txt已经存在,默认删除该文件
my $C = 0; # 如果share.txt已经存在,默认不创建新的share文件
if ( $intoptions eq "a" )
{
	$place = "Build_Together";
	print "\n$module - $place......\n";
	print "\nSearching Share...\n";
	if ( -e $sharefile && open (SHARE,$sharefile) )
	{
		print "\nGetting Share...\n";
		my $line = <SHARE>;
		close(SHARE);
		$D = 1; # Share文件已经存在,因此需要删除该文件
		my @value = &getvalue(":",$line);
		if ( @value == 2 )
		{
			my @OSstr = ("Windows","Linux");
			my $str = $OSstr[!$OS];
			my $str1 = $OSstr[$OS];
			if ( $value[0] =~ /^$str$/i ) # 必须当share.txt中的操作系统标识与当前操作系统相反时,才确认其值可能有效.
			{
				if ( $WF && ( $value[0] =~ /^Linux$/i ))
				{
					&printerror(0,"Wrong OS of '$value[1]' in '$sharefile' when Windows Build First !\n");
				}
				else
				{
					if ( $value[1] =~ /^\d{8}-\d{4}$/ ) # 日期符合yyyymmdd-hhmm格式
					{
						use Time::Local;
						my $dis = &difftime($builddatetime,$value[1]);
						$dis = abs($dis);
						$RP = &revisepath(1,$ReleaseP.$value[1]) if ( $dis <= $TS ); # 协同编译 , 获取共享文件中的共享发布路径
						no Time::Local;
					}
					else
					{
						&printerror(0,"Wrong Format Datetime of '$value[1]' in '$sharefile' !\n");
					}
				}
			}
			elsif ( $value[0] =~ /^$str1$/i )
			{
				if ( $WF && $OS )
				{
					&printerror(1,"$place : Don't Create Share when Windows Build First !\n");
				}
				else
				{
					$C = 1; # 同操作系统下编译多次,需要重新创建share文件
				}
			}
			else
			{
				&printerror(0,"Wrong Format OS of '$value[0]' in '$sharefile' !\n");
			}
		}
		else
		{
			&printerror(0,"Wrong Format of '$line' in '$sharefile' !\n");
		}
	}
	elsif ( !-e $sharefile ) # 如果share文件不存在 , 则创建该文件提供给协同编译共享
	{
		if ( $WF && $OS )
		{
			&printerror(1,"$place : Don't Create Share when Windows Build First !\n");
		}
		else
		{
			$C = 1;
		}
	}
	else # share文件存在但无法打开
	{
		$D = 1;
	}
}
elsif ( $intoptions eq "s" )
{
	$place = "Build_Single";
	print "\n$module - $place......\n";
	$RP = &revisepath(1,$intspecRP) if ( $intspecRP ne "" );
	if ( -e $sharefile )
	{
		$D = 1; # 如果share文件已经存在 , 则删除该文件
	}
}
if ( $D )
{
	print "\nDeleting Share...\n";
	&cleanerror;
	&command("del","f",$sharefile); # 获取到共享发布路径后即删除共享文件
	if ( &geterror )
	{
		&printerror(0,"Failed Del ShareFile '$sharefile' !\n");
		&command("pause");
		exit 1; # 找不到版本编译模块则退出整个程序
	}
	else
	{
		&printerror(1,"$place : Successfully Delete ShareFile '$sharefile' !\n");
	}
}
if ( $C )
{
	print "\nCreating Share...\n";
	if ( open (SHARE,">$sharefile") )
	{
		print SHARE "$OSstring : $builddatetime";
		close(SHARE);
		&printerror(1,"$place : Successfully Create ShareFile '$sharefile' !\n");
	}
	else # 创建share文件失败
	{
		&printerror(0,"Failed Create ShareFile '$sharefile' !\n");
	}
}
undef($D);
undef($C);
undef($sharefile);
undef($intoptions);
undef($intspecRP);
&printerror(1,"\n$place : ReleasePlace : $RP\n");
# =====================================================================================================================
# 校验 , 发布 , 通知
$place = "Module_Process_Check_Release_Notify";
print "\n$module - $place......\n";
for ( my $i = 0 ; $i < @modulelist ; $i++ )
{
	my @intmodules = &getvalue(",",$modulelist[$i]);
	$gownerM = $groupowner[$i];
	&module("crni","-",@intmodules);
}
undef(@intmodules);
undef($i);
# 校验 , 发布 , 通知之后的续处理
&afterprocess("bcrni");
# =====================================================================================================================
# 关闭编译错误信息文件
close(BUILDERROR);
print "\n================================= END =================================\n";
sub difftime
{
	my ($s,$d) = @_;
	my $sd = 1441;
	if (( $s !~ /^\d{8}-\d{4}$/ ) || ( $d !~ /^\d{8}-\d{4}$/ ))
	{
		&printerror(0,"Wrong Format of '$s' or '$d' !\n");
	}
	else
	{
		my @tm = ("",$s,$s,$s,$s,$s); # 按照($sec,$min,$hour,$mday,$mon,$year)推入
		$tm[1] =~ s/(\d{4})(\d{2})(\d{2})-(\d{2})(\d{2})/$5/g; # 获取'min'
		$tm[2] =~ s/(\d{4})(\d{2})(\d{2})-(\d{2})(\d{2})/$4/g; # 获取'hour'
		$tm[3] =~ s/(\d{4})(\d{2})(\d{2})-(\d{2})(\d{2})/$3/g; # 获取'mday'
		$tm[4] =~ s/(\d{4})(\d{2})(\d{2})-(\d{2})(\d{2})/$2/g; # 获取'mon'
		$tm[4] = $tm[4]-1; # timelocal的月必须在"0~11"范围中
		$tm[5] =~ s/(\d{4})(\d{2})(\d{2})-(\d{2})(\d{2})/$1/g; # 获取'year'
		my $st = timelocal(@tm);
		my @tm = ("",$d,$d,$d,$d,$d); # 按照($sec,$min,$hour,$mday,$mon,$year)推入
		$tm[1] =~ s/(\d{4})(\d{2})(\d{2})-(\d{2})(\d{2})/$5/g; # 获取'min'
		$tm[2] =~ s/(\d{4})(\d{2})(\d{2})-(\d{2})(\d{2})/$4/g; # 获取'hour'
		$tm[3] =~ s/(\d{4})(\d{2})(\d{2})-(\d{2})(\d{2})/$3/g; # 获取'mday'
		$tm[4] =~ s/(\d{4})(\d{2})(\d{2})-(\d{2})(\d{2})/$2/g; # 获取'mon'
		$tm[4] = $tm[4]-1; # timelocal的月必须在"0~11"范围中
		$tm[5] =~ s/(\d{4})(\d{2})(\d{2})-(\d{2})(\d{2})/$1/g; # 获取'year'
		my $dt = timelocal(@tm);
		$sd = $st-$dt;
	}
	return $sd;
}
# End
