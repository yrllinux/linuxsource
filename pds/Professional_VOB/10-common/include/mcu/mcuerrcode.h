/*****************************************************************************
   模块名      : MCU子系统错误码定义
   文件名      : mcuerrcode.h
   相关文件    : 
   文件实现功能: MCU错误定义
   作者        : 李屹
   版本        : V4.0  Copyright(C) 2001-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/08/27  0.9         李屹        创建
   2003/02/28  1.0         Qzj         整理错误码
   2007/11/06  4.0         顾振华      重新整理错误码，归类，预留空间
   ******************************************************************************/


/**********************************************************
   业务MCU错误号定义（20001-22000）

   分为如下部分：

   1. 一般性错误：如超时、忙、权限 链路错误——20001-20100
   2. 会议/模板创建/修改等——20100-20200
   3. MCU并发能力控制——20301-20400
   4. 会议操作——20401-21000
        细分为：终端呼叫/主席/发言——20401-20500
                选看媒体——20501-20520
                其他会议操作——20521-20600
                外设操作——20801-21000
   5. 主备/N+1备份专用——21401-21500
   6. GK/会议计费专用——21501-21600
   7. MCU配置界面化——21601-21700

   0. 目前留空部分：21021-20300; 20601-20800; 
                    21001-21400; 21701-22000

**********************************************************/


#ifndef _ERR_MCU_H_
#define _ERR_MCU_H_

#include "errorid.h"

#define DefineError(err, num )   \
    err = num,
    
#define DefineErrorEnd  \
    ERR_MCU_CODENDED = ERR_MCU_END
        
enum emMcuErrorCode
{




/* 一般性错误（20001-20100） */

//MCU等待应答超时
DefineError( ERR_MCU_TIMEROUT				   , ERR_MCU_BGN + 0)

//MCU正在初始化
DefineError( ERR_MCU_INITIALIZING			   , ERR_MCU_BGN + 1)

//MCU正忙
DefineError( ERR_MCU_BUSY        			   , ERR_MCU_BGN + 2)

//MCU正在等待前一条命令的处理
DefineError( ERR_MCU_WAITINGSTATE			   , ERR_MCU_BGN + 3)

//权限不够，不能执行此操作
DefineError( ERR_MCU_OPERATION_NOPERMIT        , ERR_MCU_BGN + 4)

//地址簿正在初始化
DefineError( ERR_MCU_ADDRBOOKINIT              , ERR_MCU_BGN + 5)

//该外设已登记
DefineError( ERR_MCU_PERIEQP_REGISTERED		   , ERR_MCU_BGN + 15)

//建链节点通信版本与MCU不一致（错误码：20017，注意兼容老版本，不能更改）
DefineError( ERR_MCU_VER_UNMATCH               , ERR_MCU_BGN + 16)

//无空闲外设管理能力
DefineError(ERR_MCU_NOIDLEEQPSSN               , ERR_MCU_BGN + 21)

//指定外设不在线
DefineError( ERR_MCU_EQPOFFLINE				   , ERR_MCU_BGN + 50)

// 指定的外设不合法
DefineError( ERR_MCU_WRONGEQP				   , ERR_MCU_BGN + 51)

// 尝试连接的外设非法
DefineError( ERR_MCU_REGEQP_INVALID			   , ERR_MCU_BGN + 52)

// 4.6 jlb
// 得到mt的逻辑通道失败
DefineError( ERR_MCU_GETMTLOGCHN_FAIL          , ERR_MCU_BGN + 53 )

// hdu选看的外设不存在
DefineError( ERR_MCU_MCUPERI_NOTEXIST          , ERR_MCU_BGN + 54 )

// 电视墙轮询中不能选看VMP或终端
DefineError( ERR_MCU_NOTSWITCHVMPORMT          , ERR_MCU_BGN + 55 )

// 非同一会议，不能改变音量大小或设置静音
DefineError( ERR_MCU_HDUCHANGEVOLUME           , ERR_MCU_BGN + 56 )

// hdu批量轮询前，有通道被占用，需释放
DefineError( ERR_MCU_HDUBATCHPOLL_CHNLINUSE    , ERR_MCU_BGN + 57 )

// hdu批量轮询时，会议没有终端
DefineError( ERR_MCU_HDUBATCHPOLL_NONEMT       , ERR_MCU_BGN + 58 )

// 当前已经开始网络抓包
DefineError( ERR_MCU_STARTNETCAP_ALREADYSTART       , ERR_MCU_BGN + 59 )

// 当前并没开始网络抓包
DefineError( ERR_MCU_STOPNETCAP_NOTSTARTYET       , ERR_MCU_BGN + 60 )



/* 创建会议或模板失败 (20101-20200) */

//MCU无法为此会议分配资源，请稍候再创建新会议
DefineError( ERR_MCU_CONFOVERFLOW			   , ERR_MCU_BGN + 100)

//创建会议失败,已达最大会议数
DefineError( ERR_MCU_CONFNUM_EXCEED			   , ERR_MCU_BGN + 101)

//创建模板失败，没有可用的模版空间
DefineError( ERR_MCU_TEMPLATE_NOFREEROOM       , ERR_MCU_BGN + 102)

//需要的接入或转发板未连接，请稍候再创建新会议
DefineError( ERR_MCU_CREATECONF_MCS_NOMPMTADP  , ERR_MCU_BGN + 103)

//会议名已存在
DefineError( ERR_MCU_CONFNAME_REPEAT		   , ERR_MCU_BGN + 104)

//会议号已存在
DefineError( ERR_MCU_CONFIDREPEAT              , ERR_MCU_BGN + 105)

//该E164号码已存在
DefineError( ERR_MCU_CONFE164_REPEAT		   , ERR_MCU_BGN + 106)

//创建会议指定模版未找到
DefineError( ERR_MCU_TEMPLATE_NOTEXIST         , ERR_MCU_BGN + 107)

//指定终端是您的上级MCU，呼叫暂时被拒绝（您可能会被重新呼入）
DefineError( ERR_MCU_CALLMMCUFAIL              , ERR_MCU_BGN + 108)

//没有空闲的适配器，创建双格式媒体会议或者预留适配器会议失败
DefineError( ERR_MCU_NOIDLEADAPTER  		   , ERR_MCU_BGN + 110)

//没有空闲的混音器,混音会议创建失败
DefineError( ERR_MCU_NOIDLEMIXER    	       , ERR_MCU_BGN + 111)

//没有空闲的丢包重传器,丢包重传会议创建失败
DefineError( ERR_MCU_NOIDLEPRS    	           , ERR_MCU_BGN + 112)

//会议加密属性与是否设置加密不一致,会议创建失败
DefineError( ERR_MCU_SETENCRYPT    	           , ERR_MCU_BGN + 113)

//没有空闲的电视墙,含电视墙的会议创建失败
DefineError( ERR_MCU_NOIDLETVWALL  	           , ERR_MCU_BGN + 114)

//指定的录像机不在线，创建自动录像会议失败
DefineError( ERR_MCU_CONFSTARTREC_MAYFAIL      , ERR_MCU_BGN + 115)

//没有空闲的dcs，创建数据会议失败
DefineError( ERR_MCU_NOIDLEDCS                 , ERR_MCU_BGN + 116)

//没有指定终端列表，无终端自动结束的会议创建失败
DefineError( ERR_MCU_NOMTINCONF                , ERR_MCU_BGN + 117)

//会议结束时间应该在当前时间之后
DefineError( ERR_MCU_STARTTIME_WRONG		   , ERR_MCU_BGN + 118)

//会议码率不支持
DefineError( ERR_MCU_CONFBITRATE      		   , ERR_MCU_BGN + 119)

//指定的会议音频或视频格式不支持
DefineError( ERR_MCU_CONFFORMAT				   , ERR_MCU_BGN + 120)

//会议组播地址非法
DefineError( ERR_MCU_MCASTADDR_INVALID         , ERR_MCU_BGN + 121)

//会议组播地址已经被占用
DefineError( ERR_MCU_MCASTADDR_USED            , ERR_MCU_BGN + 122)

//分散会议组播地址非法
DefineError( ERR_MCU_DCASTADDR_INVALID         , ERR_MCU_BGN + 123)

//分散会议不能是双速双格式
DefineError( ERR_MCU_DCAST_NOADP               , ERR_MCU_BGN + 124)

//分散会议组播地址已经被占用
DefineError( ERR_MCU_DCASTADDR_USED            , ERR_MCU_BGN + 125)

//分散会议超过最大回传通道数
DefineError( ERR_MCU_DCAST_OVERCHNNLNUM        , ERR_MCU_BGN + 126)

//该会议不能进行低速组播
DefineError( ERR_MCU_MCLOWNOSUPPORT            , ERR_MCU_BGN + 127)

//该会议不能进行低速录像
DefineError( ERR_MCU_RECLOWNOSUPPORT           , ERR_MCU_BGN + 128)

//没有足够权限保存缺省会议
DefineError( ERR_MCU_SAVEDEFCONF_NOPERMIT      , ERR_MCU_BGN + 150)

//会议号为空，同时给出终端可选择的会议列表(错误号不可修改)
DefineError( ERR_MCU_NULLCID                   , ERR_MCU_BGN + 160)

//呼叫下级MCU失败，该MCU正在召开其它会议
DefineError( ERR_MCU_CALLMCUERROR_CONFISHOLDING, ERR_MCU_BGN + 161)

//上级MCU呼叫被拒绝，若要接受上级邀请，请先结束当前会议
DefineError( ERR_MCU_MMCUCALLREFUSE			   , ERR_MCU_BGN + 162)

//已达到会议数上限，无法创建其他会议
DefineError( ERR_MCU_CREATECONFERROR_CONFISHOLDING, ERR_MCU_BGN + 163)

//当前会议不支持保存缺省会议
DefineError( ERR_MCU_CANNOTSAVEDEFAULTCONF, ERR_MCU_BGN + 164)

//呼叫下级MCU失败，该MCU已经被其它高级别MCU呼叫
DefineError( ERR_MCU_CASADEBYOTHERHIGHLEVELMCU, ERR_MCU_BGN + 165)

//当前时间已过周期性预约会议最后开启时间
DefineError( ERR_MCU_CURRENT_LATER_DURAENDTIME, ERR_MCU_BGN + 166)

//周期性预约会议最早开启时间晚于最后开启时间
DefineError( ERR_MCU_DURASTART_LATER_DURAENDTIME, ERR_MCU_BGN + 167)

//周期性预约会议设置的开启时间和有效时间冲突
DefineError( ERR_MCU_CIRCLESCHEDULECONF_TIME_WRONG, ERR_MCU_BGN + 168)

//周期时间到，当前预约会议自动结束
DefineError( ERR_MCU_CIRCLESCHEDULECONF_WILLNOT_START, ERR_MCU_BGN + 169)

/* MCU 并发能力限制的错误码 (20301-20400) */

//超过MCU最大接入终端能力
DefineError( ERR_MCU_OVERMAXCONNMT				 , ERR_MCU_BGN + 300)

//转发板负载能力已满
DefineError( ERR_MCU_MPNETTRFCOVERRUN            , ERR_MCU_BGN + 301)

// 语音激励时不能进行画面合成
DefineError( ERR_MCU_VACNOVMP                    , ERR_MCU_BGN + 302)

// 画面合成时不能进行语音激励
DefineError( ERR_MCU_VMPNOVAC                    , ERR_MCU_BGN + 303)

// 当前会议格式不支持画面合成
DefineError( ERR_MCU_VMPNOSUPPORT                , ERR_MCU_BGN + 304)

// 会议同时进行混音时无法进行指定风格的画面合成
DefineError( ERR_MCU_VMPNOSUPPORTONMIX           , ERR_MCU_BGN + 305)

// 会议正在启用适配时无法进行指定风格的混音
DefineError( ERR_MCU_MIXNOSUPPORTONBAS           , ERR_MCU_BGN + 306)

// 会议正在画面合成时无法进行指定风格的混音
DefineError( ERR_MCU_MIXNOSUPPORTONVMP           , ERR_MCU_BGN + 307)

// 会议终端数超过最大混音能力
DefineError( ERR_MCU_MIXNOSUPPORT                , ERR_MCU_BGN + 308)

// 会议终端数超过最大语音激励能力
DefineError( ERR_MCU_VACNOSUPPORT                , ERR_MCU_BGN + 309)

// 会议正在启用适配时无法对该数量终端进行语音激励
DefineError( ERR_MCU_VACNOSUPPORTONBAS           , ERR_MCU_BGN + 310)

// 会议正在对该数量终端进行语音激励时无法开启适配
DefineError( ERR_MCU_BASNOSUPPORTONVAC           , ERR_MCU_BGN + 311)

// 会议正在对该数量终端进行混音时无法开启适配
DefineError( ERR_MCU_BASNOSUPPORTONMIX           , ERR_MCU_BGN + 312)

// 无法支持更多的终端数量同时进行会议讨论，讨论已经自动停止或切换为定制混音
DefineError( ERR_MCU_DISCUSSAUTOCHANGE2SPECMIX   , ERR_MCU_BGN + 313)

//超过混音器最大混音通道数
DefineError( ERR_MCU_OVERMAXMIXERCHNNL           , ERR_MCU_BGN + 314)

//没有空闲的混音器,开始会议讨论失败
DefineError( ERR_MCU_NOIDLEMIXER_INCONF          , ERR_MCU_BGN + 315)

//语音激励与定制混音互斥
DefineError( ERR_MCU_VACMUTEXSPECMIX             , ERR_MCU_BGN + 316)

//级联定制混音和本地语音激励互斥
//DefineError( ERR_MCU_CASSPECMIXMUTEXVAC          , ERR_MCU_BGN + 317)

//强制发言人时不允许放像
DefineError( ERR_MUSTSEESPEAKER_NOTPLAY          , ERR_MCU_BGN + 318)

//下级VAC被级联定制混音冲掉 提示
DefineError( ERR_MCU_MMCUSPECMIX_VAC             , ERR_MCU_BGN + 319)

//强制广播时不允许开启混音
DefineError( ERR_MUSTSEESPEAKER_NOTMIXING        , ERR_MCU_BGN + 320)


//会议当前有空闲画面合成器，但不支持所需风格。(无法进行画面合成或召开会议)
DefineError( ERR_MCU_ALLIDLEVMP_NO_SUPPORT       , ERR_MCU_BGN + 330)

//会议当前使用的画面合成器不支持所需风格，但本MCU还有其他画面合成器支持所需风格。如果确实需要本风格的画面合成，重新开启该画面合成风格的画面合成即可。
DefineError( ERR_MCU_OTHERVMP_SUPPORT            , ERR_MCU_BGN + 331)

//指定的画面合成风格不被支持
DefineError( ERR_INVALID_VMPSTYLE                , ERR_MCU_BGN + 332)

//设有空闲的画面合成器，不能进行画面合成
DefineError( ERR_MCU_NOIDLEVMP			         , ERR_MCU_BGN + 333)

//当前的空闲mau/mpu不足，可能会导致稍后的召开会议失败
DefineError( ERR_MCU_NOENOUGH_HDBAS_TEMP         , ERR_MCU_BGN + 334)

//没有空闲的mau/mpu，不能召开会议
DefineError( ERR_MCU_NOENOUGH_HDBAS_CONF         , ERR_MCU_BGN + 335)

//尝试占用mau/mpu失败
DefineError( ERR_MCU_OCUPPYHDBAS                 , ERR_MCU_BGN + 336)

//VMP不支持主席跟随
DefineError( ERR_VMPUNSUPPORTCHAIRMAN			 , ERR_MCU_BGN + 337)

//没有空闲的mpu，不能召开会议
DefineError( ERR_MCU_NOENOUGH_MPU_CONF           , ERR_MCU_BGN + 338)

//尝试占用MPU失败
DefineError( ERR_MCU_OCUPPYMPU                   , ERR_MCU_BGN + 339)

//会议已开始轮询，不能开始语音激励
DefineError( ERR_MCU_CONFSTARTPOLLVACMODE        , ERR_MCU_BGN + 340)

//会议已处于已处于讨论状态，不能进行语音激励控制
DefineError( ERR_MCU_CONFINVACMODE_BYDISCUSSC    , ERR_MCU_BGN + 341)

//会议已处于已处于语音激励控制，不能进行讨论
DefineError( ERR_MCU_CONFINDISCUSSCMODE_BYVAC    , ERR_MCU_BGN + 342)

//会议处于语音激励方式，不能取消发言人
DefineError( ERR_MCU_VACNOSPEAKERCANCEL          , ERR_MCU_BGN + 343)

//会议处于讨论方式，不能指定发言人
DefineError( ERR_MCU_DISCUSSNOSPEAKER            , ERR_MCU_BGN + 344)

//会议处于语音激励方式，不能指定发言人
DefineError( ERR_MCU_VACNOSPEAKERSPEC            , ERR_MCU_BGN + 345)

//会议处于强制广播发言人模式，不能选看终端
DefineError( ERR_MCU_MUSTSEESPEAKER              , ERR_MCU_BGN + 346)
 
//两次发言人切换间隔小于最小时间间隔，不能指定发言人
DefineError( ERR_MCU_LESSPEAKERHOLDTIME          , ERR_MCU_BGN + 347)

//当前的定制混音状态被冲掉 提示
DefineError( ERR_MCU_SPECMIX_CANCEL_NTF          , ERR_MCU_BGN + 348)

//当前的智能混音状态被冲掉 提示
DefineError( ERR_MCU_AUTOMIX_CANCEL_NTF          , ERR_MCU_BGN + 349)

//tianzhiyong手动选择的混音器未通过验证 提示
DefineError( ERR_MCU_SELETMIX_INVALIDE_NTF       , ERR_MCU_BGN + 350)

// [10/21/2010 liuxu][添加] 找不到空闲的bas通道,建立录像选看适配通道失败
DefineError( ERR_MCU_GETIDLELBASCHN_FAILED       , ERR_MCU_BGN + 351)

//[2013/03/21 nizhijun] AALC的单双声道会议，必须界面勾选适配录像，否则开启录像失败
DefineError( ERR_MCU_AACLC_CONF_NEED_RECADPAT      , ERR_MCU_BGN + 352)

//没有空闲的音频适配器，不能召开会议
DefineError( ERR_MCU_NOENOUGH_AUDBAS_CONF         , ERR_MCU_BGN + 353)

//分配的适配通道不可用
DefineError( ERR_BASCHANNEL_INVALID              , ERR_MCU_BGN + 370)

//分散会议不能执行本操作
DefineError( ERR_MCU_DCAST_NOOP                  , ERR_MCU_BGN + 380)

//接入能力满，导致MT呼入mcu失败
DefineError( ERR_MCU_MTCALLFAIL_FULL			 , ERR_MCU_BGN + 381)

//HD接入能力满（仅用于内嵌接入）
DefineError( ERR_MCU_CALLHDMTFAIL				 , ERR_MCU_BGN + 382)

//为外设预留的转发能力满，建议重新分配转发板
DefineError( ERR_MCU_MPRESERVEDBANDWIDTH_FULL    , ERR_MCU_BGN + 383)

//PCMT 接入能力满
DefineError( ERR_MCU_CALLPCMTFAIL				 , ERR_MCU_BGN + 384)

//mcu license 终端授权数满
DefineError( ERR_MCU_LICENSECALLMT_FULL			 , ERR_MCU_BGN + 385)

//[liu lijiu][2010/10/28]USBKEY授权数据验证失败
DefineError( ERR_MCU_USBKEYLICENSEDATA_INVALIDATION			 , ERR_MCU_BGN + 386)

//语音接入能力满
DefineError( ERR_MCU_CALLAUDMTFAIL			      , ERR_MCU_BGN + 387)

//AES加密会议拒绝非加密终端入会
DefineError( ERR_MCU_CONF_REJECT_NOENCYPT_MT ,  ERR_MCU_BGN + 388)


/* 会议操作错误（20401-21000） */

//呼叫终端/申请入会[20401-20430]

//指定终端不可及，可能不在线
DefineError( ERR_MCU_MTUNREACHABLE			    , ERR_MCU_BGN + 400)

//指定终端拒绝加入会议
DefineError( ERR_MCU_MTREJECT				    , ERR_MCU_BGN + 401)

//指定终端已在会议中
DefineError( ERR_MCU_ADDEDMT_INCONF			    , ERR_MCU_BGN + 402)

//非主席终端不能执行此项操作
DefineError( ERR_MCU_INVALID_OPER			    , ERR_MCU_BGN + 403)

//呼叫的终端已经与会
DefineError( ERR_MCU_CALLEDMT_JOINEDCONF	    , ERR_MCU_BGN + 404)

//呼叫的终端没有可用的呼叫地址信息
DefineError( ERR_MCU_CALLEDMT_NOADDRINFO	    , ERR_MCU_BGN + 405)

//不能级联下级MCU会议，密码错误
DefineError( ERR_MCU_INVALIDPWD                 , ERR_MCU_BGN + 406)

//上级MCU没有应答 
DefineError( ERR_MCU_MMCUNORESPONSE             , ERR_MCU_BGN + 407) 

//不能呼叫会议本身
DefineError( ERR_MCU_NOTCALL_CONFITSELF	        , ERR_MCU_BGN + 408)

//指定终端忙，可能在另一会议中
DefineError( ERR_MCU_MTBUSY					    , ERR_MCU_BGN + 409)

//终端Round trip delay 超时,保持链路失败
DefineError( ERR_MCU_MTRTDFAIL				    , ERR_MCU_BGN + 410)

//此会议为非开放会议，需主席批准
DefineError( ERR_MCU_NOT_OPENCONF			    , ERR_MCU_BGN + 411)

//已将申请提交主席
DefineError( ERR_MCU_CHAIRMANEXIST			    , ERR_MCU_BGN + 412)

//会议中已有上级MCU
DefineError( ERR_MCU_HAS_MMCU	                , ERR_MCU_BGN + 413)

//指定终端已经在回传通道中
DefineError( ERR_MCU_BACKCHNNL_HAS_SAMEMT                , ERR_MCU_BGN + 414)


//呼叫的终端和MCU的类型不匹配
DefineError( ERR_MCU_CALLEDMT_TYPENOTMATCH		, ERR_MCU_BGN + 420)

//终端不在可调度资源的范围类
DefineError( ERR_MCU_MT_NOTINVC                 , ERR_MCU_BGN + 421)


//终端操作/指定主席/发言人[20431-20500]

//指定终端是只接收终端
DefineError( ERR_MCU_RCVONLYMT			        , ERR_MCU_BGN + 430)

//被指定终端已经是会议主席
DefineError( ERR_MCU_SAMECHAIRMAN			    , ERR_MCU_BGN + 431)

//会议处于无主席模式,不能指定主席
DefineError( ERR_MCU_NOCHAIRMANMODE			    , ERR_MCU_BGN + 432)

//会议中无主席
DefineError( ERR_MCU_NOCHAIRMAN				    , ERR_MCU_BGN + 433)

//指定终端未与会
DefineError( ERR_MCU_MT_NOTINCONF			    , ERR_MCU_BGN + 434)

//指定的MCU未与会
DefineError( ERR_MCU_THISMCUNOTJOIN             , ERR_MCU_BGN + 435)

//指定终端已经是发言终端
DefineError( ERR_MCU_SAMESPEAKER			    , ERR_MCU_BGN + 436)

//会议中已有发言人
DefineError( ERR_MCU_HASSPEAKER				    , ERR_MCU_BGN + 437)

//会议中无发言人
DefineError( ERR_MCU_NOSPEAKER				    , ERR_MCU_BGN + 438)

//会议未?读?
DefineError( ERR_MCU_CONFNOTCASCADE             , ERR_MCU_BGN + 439)

//此操作只能针对本地终端
DefineError( ERR_MCU_OPRONLYLOCALMT             , ERR_MCU_BGN + 440)

//指定的下级MCU不存在,锁定操作失败
DefineError( ERR_LOCKSMCU_NOEXIT                , ERR_MCU_BGN + 441)

// 终端版本不支持版本号获取
DefineError( ERR_MCU_MT_NOVERINFO               , ERR_MCU_BGN + 442)

// 终端的媒体能力不支持指定操作
DefineError( ERR_MT_MEDIACAPABILITY             , ERR_MCU_BGN + 443)


//Media Source Select[20501-20510]

//源与目的的媒体类型不同，不能选看
DefineError( ERR_MCU_NOTSAMEMEDIATYPE           , ERR_MCU_BGN + 500)

//需码率适配终端不得选看
DefineError( ERR_MCU_DSTMTNEEDADAPT	    	    , ERR_MCU_BGN + 501)

//混音时不得选择音频模式选看
DefineError( ERR_MCU_SELAUDIO_INMIXING    	    , ERR_MCU_BGN + 502)

//混音时选看音视频模式，音频选看将失败
DefineError( ERR_MCU_SELBOTH_INMIXING           , ERR_MCU_BGN + 503)

//选看终端无法接收或是选看目标不在发送，无法进行指定选看
DefineError( ERR_MCU_SRCISRECVONLY  	        , ERR_MCU_BGN + 504)

//会控不能选看高清终端
DefineError( ERR_MCU_MCSSELMT_HD                , ERR_MCU_BGN + 505)

//音频选看被拒绝, 选看模式改变
DefineError( ERR_MCU_SELMODECHANGE_AUDIOFAIL    , ERR_MCU_BGN + 506)
//视频选看被拒绝, 选看模式改变
DefineError( ERR_MCU_SELMODECHANGE_VIDEOFAIL    , ERR_MCU_BGN + 507)
//视频适配资源不足，选看失败
DefineError( ERR_MCU_SELVIDBASISNOTENOUGH          , ERR_MCU_BGN + 508)
//音频适配资源不足，选看失败
DefineError( ERR_MCU_SELAUDBASISNOTENOUGH          , ERR_MCU_BGN + 509)

//会议控制[20521-20600]

//会议控制权被其它会控独享
DefineError( ERR_MCU_CTRLBYOTHER			    , ERR_MCU_BGN + 520)

//无效的会议保护方式
DefineError( ERR_MCU_INVALID_CONFLOCKMODE	    , ERR_MCU_BGN + 521)

//无效的终端呼叫策略
DefineError( ERR_MCU_INVALID_CALLMODE	        , ERR_MCU_BGN + 522)

// 轮询停止，可能是因为所有待轮询终端没有视频源或不在线或者次数已完成
DefineError( ERR_MCU_POLLING_NOMTORVIDSRC       , ERR_MCU_BGN + 523)

// 指定轮询位置失败，因为指定位置终端不在轮询列表中
DefineError( ERR_MCU_SPECPOLLPOS_MTNOTINTABLE   , ERR_MCU_BGN + 524)

// 指定轮询位置失败，因为指定终端不在会议中
DefineError( ERR_MCU_SPECPOLLPOS_MTNOTINCONF	, ERR_MCU_BGN + 525)

// 指定轮询位置失败，因为会议不在轮询
DefineError( ERR_MCU_SPECPOLLPOS_CONFNOTPOLL	, ERR_MCU_BGN + 526)

// 修改轮询列表失败，当前轮询终端不能被删除
DefineError( ERR_MCU_DELCURPOLLMT_FAIL   	    , ERR_MCU_BGN + 527)


// 错误的点名类型
DefineError( ERR_MCU_ROLLCALL_MODE              , ERR_MCU_BGN + 528)

// 错误的点名操作消息长度
DefineError( ERR_MCU_ROLLCALL_MSGLEN            , ERR_MCU_BGN + 529)

// 下级终端不能作点名人
DefineError( ERR_MCU_ROLLCALL_CALLER_SMCUMT     , ERR_MCU_BGN + 530)

// 点名人不能为空
DefineError( ERR_MCU_ROLLCALL_CALLERNULL        , ERR_MCU_BGN + 531)

// 被点名人不能为空
DefineError( ERR_MCU_ROLLCALL_CALLEENULL        , ERR_MCU_BGN + 532)

// 强制广播不能点名
DefineError( ERR_MCU_ROLLCALL_MUSTSEESPEAKER    , ERR_MCU_BGN + 533)

// 画面合成风格及成员变更提示
DefineError( ERR_MCU_ROLLCALL_VMPSTYLE          , ERR_MCU_BGN + 534)

// 语音激励停止提示
DefineError( ERR_MCU_ROLLCALL_VACING            , ERR_MCU_BGN + 535)

// 定制混音成员变更提示
DefineError( ERR_MCU_ROLLCALL_SPECMIXING        , ERR_MCU_BGN + 536)

// 会议讨论停止提示
DefineError( ERR_MCU_ROLLCALL_AUTOMIXING        , ERR_MCU_BGN + 537)

// 停止画面合成提示
DefineError( ERR_MCU_ROLLCALL_VMPING            , ERR_MCU_BGN + 538)

// 非相关选看停止提示
DefineError( ERR_MCU_ROLLCALL_SEL               , ERR_MCU_BGN + 539)

// 会议轮询的时候拒绝点名
DefineError( ERR_MCU_ROLLCALL_POLL              , ERR_MCU_BGN + 540)

// 点名人掉线，点名停止
DefineError( ERR_MCU_ROLLCALL_CALLERLEFT        , ERR_MCU_BGN + 541)

// 被点名人掉线，点名停止
DefineError( ERR_MCU_ROLLCALL_CALLEELEFT        , ERR_MCU_BGN + 542)

// 混音器掉线，点名停止
DefineError( ERR_MCU_ROLLCALL_MIXERLEFT         , ERR_MCU_BGN + 543)

// VMP掉线，点名停止
DefineError( ERR_MCU_ROLLCALL_VMPLEFT           , ERR_MCU_BGN + 544)

// H264-D1会议不允许外厂商终端加入VMP
DefineError( ERR_MCU_NONKEDAMT_JOINVMP          , ERR_MCU_BGN + 545)

// 没有空闲的混音器，停止或拒绝点名
DefineError( ERR_MCU_ROLLCALL_NOMIXER           , ERR_MCU_BGN + 546)

// 没有空闲的VMP， 停止或拒绝点名
DefineError( ERR_MCU_ROLLCALL_NOVMP            , ERR_MCU_BGN + 547)

// VMP能力不足，停止或拒绝点名
DefineError( ERR_MCU_ROLLCALL_VMPABILITY       , ERR_MCU_BGN + 548)

// VMP能力不足，切换合成风格
DefineError( ERR_MCU_ROLLCALL_STYLECHANGE      , ERR_MCU_BGN + 549)

// 卫星会议相关的错误码

// 申请卫星频率失败
DefineError( ERR_MCU_NOTGETFREQUENCE           , ERR_MCU_BGN + 550)

// 网管拒绝了所有的终端
DefineError( ERR_MCU_NMSNOPARTICIPANT          , ERR_MCU_BGN + 551)

// 网管拒绝了会议
DefineError( ERR_MCU_NMSNOTAGREE               , ERR_MCU_BGN + 552)

// 预约会议开启超时
DefineError( ERR_MCU_SCHEDTIMEROUT             , ERR_MCU_BGN + 553)

// 网管响应超时
DefineError( ERR_MCU_NMSTIMEROUT               , ERR_MCU_BGN + 554)

// 网管响应 拒绝
DefineError( ERR_MCU_NMSNACK                   , ERR_MCU_BGN + 555)



// 上级抢断本地的轮询回传 提示信息
DefineError( ERR_MCU_CASPOLL_STOP              , ERR_MCU_BGN + 560)

// 主席轮询时不允许主席选看画面合成、主席选看以及切换主席操作
DefineError( ERR_MCU_SPECCHAIREMAN_NACK        , ERR_MCU_BGN + 561)

// 级联会议中，主席为下级MCU时不允许开启主席轮询选看
DefineError( ERR_MCU_CHAIMANPOLL_NACK          , ERR_MCU_BGN + 562)

//上次点名操作还没完成
DefineError( ERR_MCU_LASTROLLCALL_NOTFINISH          , ERR_MCU_BGN + 563)

//会议外设操作[20801-21000]

//当前正在进行会议录像
DefineError( ERR_MCU_CONFRECORDING			    , ERR_MCU_BGN + 800)

//当前不在进行会议录像
DefineError( ERR_MCU_CONFNOTRECORDING		    , ERR_MCU_BGN + 801)

//当前未被暂停会议录像
DefineError( ERR_MCU_CONFNOTRECPAUSE			, ERR_MCU_BGN + 802)

//当前正在进行会议放像
DefineError( ERR_MCU_CONFPLAYING				, ERR_MCU_BGN + 803)

//当前不在进行会议放像
DefineError( ERR_MCU_CONFNOTPLAYING			    , ERR_MCU_BGN + 804)

//当前未被暂停会议放像
DefineError( ERR_MCU_CONFNOTPLAYPAUSE		    , ERR_MCU_BGN + 805)

//指定的终端处于录像状态
DefineError( ERR_MCU_MTRECORDING			    , ERR_MCU_BGN + 806)

//指定的终端未处于录像状态
DefineError( ERR_MCU_MTNOTRECORDING			    , ERR_MCU_BGN + 807)

//指定的终端未处于录像暂停状态
DefineError( ERR_MCU_MTNOTRECPAUSE			    , ERR_MCU_BGN + 808)


//当前正在进行会议混音
DefineError( ERR_MCU_CONFMIXING				    , ERR_MCU_BGN + 810)

//当前不在进行会议混音
DefineError( ERR_MCU_CONFNOTMIXING			    , ERR_MCU_BGN + 811)

//当前未被暂停会议混音
DefineError( ERR_MCU_CONFNOTMIXPAUSE		    , ERR_MCU_BGN + 812)

//指定终端不是混音组成员
DefineError( ERR_MCU_MTNOTMIXMEMBER			    , ERR_MCU_BGN + 813)


//会议已处于语音激励控制发言状态,不能再次开始
DefineError( ERR_MCU_CONFINVACMODE  		    , ERR_MCU_BGN + 815)

//会议未处于语音激励控制发言状态,不能停止
DefineError( ERR_MCU_CONFNOTINVACMODE  		    , ERR_MCU_BGN + 816)

//会议已处于讨论状态,不能再次开始
DefineError( ERR_MCU_CONFINDISCUSSCMODE  	    , ERR_MCU_BGN + 817)

//会议未处于讨论状态,不能停止
DefineError( ERR_MCU_CONFNOTINDISCUSSCMODE  	, ERR_MCU_BGN + 818)


//有画面合成模板时不能设为动态分屏
DefineError( ERR_MCU_DYNAMCIVMPWITHMODULE       , ERR_MCU_BGN + 820)     

//画面合成已开始
DefineError( ERR_MCU_VMPRESTART  			    , ERR_MCU_BGN + 821)

//画面合成尚未开始,不能进行此项操作
DefineError( ERR_MCU_VMPNOTSTART  			    , ERR_MCU_BGN + 822)

//画面合成未广播码流
DefineError( ERR_MCU_VMPNOTBRDST                , ERR_MCU_BGN + 823)

//动态分屏时必需是自动合成
DefineError( ERR_MCU_DYNAMICMUSTBYAUTO          , ERR_MCU_BGN + 824)           

//VMP成员重复
DefineError( ERR_MCU_REPEATEDVMPMEMBER			, ERR_MCU_BGN + 825)

//有多个通道被设成发言人跟随
DefineError( ERR_MCU_REPEATEDVMPSPEAKER			, ERR_MCU_BGN + 826)

//有多个通道被设成轮询跟随
DefineError( ERR_MCU_REPEATEDVMPPOLL			, ERR_MCU_BGN + 827)

//会议带音频轮询时，画面合成器不支持轮询跟随和发言人跟随同时存在
DefineError( ERR_AUDIOPOLL_CONFLICTVMPFOLLOW	,	ERR_MCU_BGN + 828)

//VMP正忙
DefineError( ERR_VMPBUSY						,   ERR_MCU_BGN + 829)

//老版VMP不支持该功能
DefineError( ERR_FUNCTIONNOTSUPPORTBYOLDVMP		,	ERR_MCU_BGN + 830)

//下级mcu不支持多回传,其终端只能占用一个通道
DefineError( ERR_MCUNOMULTISPYONLYUSEONEVMPCHANNEL,	ERR_MCU_BGN + 831)

//当前通道解码能力不匹配，请更换通道尝试
DefineError( ERR_HDU_CHNDECABILITYNOTMATCHED  ,   ERR_MCU_BGN + 832)

//8000A前适配通道不足,终端无法进画面合成
DefineError( ERR_VMP_NO_VIP_VMPCHNNL  ,   ERR_MCU_BGN + 833)

// Vmp vicp资源不足
DefineError( ERR_VMP_VICP_NOT_ENOUGH		, ERR_MCU_BGN + 834)

//VMP中Other成员或G400IPC重复
DefineError( ERR_MCU_REPEATEDVMPNONEKEDAMEMBER	, ERR_MCU_BGN + 835)

//VMP中双流跟随通道重复
DefineError( ERR_MCU_REPEATEDVMPDSTREAM	, ERR_MCU_BGN + 836)

//VMP中MMCU成员重复
DefineError( ERR_MCU_REPEATEDVMPMMCUMEMBER	, ERR_MCU_BGN + 837)

//VMP中发言人抢占前适配，导致其它通道无图像，给提示
DefineError( ERR_MCU_UNABLEADJUSTRESMTOUTVMPADPCHL	, ERR_MCU_BGN + 838)

/*N+1备份错误(21401-21500)*/

//不能创建模版、会议
DefineError( ERR_MCU_NPLUS_CREATETEMPLATE        , ERR_MCU_BGN + 1400)  

//不能编辑用户
DefineError( ERR_MCU_NPLUS_EDITUSER              , ERR_MCU_BGN + 1401)  
 
//不支持手动回滚（用户选择了自动回滚）
DefineError( ERR_MCU_NPLUS_AUTOROLLBACK          , ERR_MCU_BGN + 1402)

//需要回滚的MCU未连接
DefineError( ERR_MCU_NPLUS_BAKCLIENT_NOTREG      , ERR_MCU_BGN + 1403)

//会议在备板进行的画面合成无法在主板恢复：主板没有画面合成器
DefineError( ERR_MCU_NPLUS_MASTER_VMP_NONE       , ERR_MCU_BGN + 1404)

//N+1备份恢复或是回滚的会议，会议当前有空闲画面合成器，但不支持所需风格，
//画面合成将无法恢复。可能备份端的画面合成能力大于本主端导致。(map数,8000B... etc)
DefineError( ERR_MCU_ALLIDLEVMP_NO_SUPPORT_NPLUS , ERR_MCU_BGN + 1405)


/*END OF N+1备份错误. */


/* GK/会议计费错误(21501-21600) */

//GK资源耗尽GRJ, RRJ, ARJ, LRJ
DefineError( ERR_MCU_RAS_RESOURCE_UNAVAILABLE 	 , ERR_MCU_BGN + 1500)

//带宽资源不够BRJ
DefineError( ERR_MCU_RAS_INSUFFICIENT_RESOURCE	 , ERR_MCU_BGN + 1503)

//呼叫被GK拒绝
DefineError( ERR_MCU_RAS_GK_REJECT       	     , ERR_MCU_BGN + 1504)

//无效呼叫信令地址RRJ
DefineError( ERR_MCU_RAS_INVALID_CALLADDR		 , ERR_MCU_BGN + 1505)

//别名已经被其他网络实体注册RRJ
DefineError( ERR_MCU_RAS_DUPLICATE_ALIAS	     , ERR_MCU_BGN + 1507)

//对方忙
DefineError( ERR_MCU_RAS_MTISBUSY  			     , ERR_MCU_BGN + 1508)

//不明原因
DefineError( ERR_MCU_RAS_UNDEFINED				 , ERR_MCU_BGN + 1509)

//正常挂断
DefineError( ERR_MCU_RAS_NORMALDROP				 , ERR_MCU_BGN + 1510)

//GK请求超时
DefineError( ERR_MCU_RAS_TIMEOUT				 , ERR_MCU_BGN + 1512)

//注册Gk失败（虽然非RAS失败，放在这里）
DefineError( ERR_MCU_RAS_GKUNREG				 , ERR_MCU_BGN + 1520)

//GK内部错误
DefineError( ERR_MCU_RAS_INTERNALERR 			 , ERR_MCU_BGN + 1521)

//GK用户名不存在
DefineError( ERR_MCU_GK_USRNAME                  , ERR_MCU_BGN + 1530)

//GK密码不正确
DefineError( ERR_MCU_GK_USRPWD                   , ERR_MCU_BGN + 1531)

//GK数据库错误
DefineError( ERR_MCU_GK_DB                       , ERR_MCU_BGN + 1532)

//GK会议不存在
DefineError( ERR_MCU_GK_CONF                     , ERR_MCU_BGN + 1533)

//请求开始计费超时
DefineError( ERR_MCU_GK_STARTCHARGE_TIMEOUT      , ERR_MCU_BGN + 1534)

//请求结束计费超时
DefineError( ERR_MCU_GK_STOPCHARGE_TIMEOUT       , ERR_MCU_BGN + 1535)

//其他未明确的GK错误
DefineError( ERR_MCU_GK_UNKNOW                   , ERR_MCU_BGN + 1536)

//GK不存在，可能导致模板创会失败
DefineError( ERR_MCU_GK_UNEXIST_CREATEMODULE     , ERR_MCU_BGN + 1537)

//GK不存在，创会失败
DefineError( ERR_MCU_GK_UNEXIST_CREATECONF       , ERR_MCU_BGN + 1538)

//GK计费服务器注册失败
DefineError( ERR_MCU_GK_REGFAILED                , ERR_MCU_BGN + 1539)

//会议计费异常
DefineError( ERR_MCU_GK_CONFCHARGE_EXCEPTION 	 , ERR_MCU_BGN + 1540)

//GK计费链路断
DefineError( ERR_MCU_GK_DISCONNECT		         , ERR_MCU_BGN + 1541)

//GK未配置Radius服务器，召开计费会议失败
DefineError( ERR_MCU_GK_WITHOUT_RADIUS           , ERR_MCU_BGN + 1542)

//GK操作Radius服务器失败
DefineError( ERR_MCU_GK_OP_RADIUS_FAILED         , ERR_MCU_BGN + 1543)

//会议的计费帐号已经被使用
DefineError( ERR_MCU_GK_ACCT_IN_USE              , ERR_MCU_BGN + 1544)

//配置了GK, 但没有启用计费功能, 可能导致模板创会失败
DefineError( ERR_MCU_GK_NOCHARGE_CREATEMODULE    , ERR_MCU_BGN + 1545)

//配置了GK, 但没有启用计费功能，创会失败
DefineError( ERR_MCU_GK_NOCHARGE_CREATECONF      , ERR_MCU_BGN + 1546)

//计费sessionId为空，停止计费失败
DefineError( ERR_MCU_GK_SSNID_NULL               , ERR_MCU_BGN + 1547)

//GK计费服务能力满
DefineError( ERR_MCU_GK_INST_FULL                , ERR_MCU_BGN + 1548)

/*END OF GK/会议计费错误*/


/* mcu 配置界面化错误定义 21601-21700 */

//修改配置时单板参数非法
DefineError( ERR_MCU_CFG_INVALID_BRD             , ERR_MCU_BGN + 1600)
//混音器
DefineError( ERR_MCU_CFG_INVALID_MIXER           , ERR_MCU_BGN + 1601)
//录像机
DefineError( ERR_MCU_CFG_INVALID_REC             , ERR_MCU_BGN + 1602)
//vmp
DefineError( ERR_MCU_CFG_INVALID_VMP             , ERR_MCU_BGN + 1603)
//电视墙
DefineError( ERR_MCU_CFG_INVALID_TVWALL          , ERR_MCU_BGN + 1604)
//码流适配
DefineError( ERR_MCU_CFG_INVALID_BAS             , ERR_MCU_BGN + 1605)
//丢包重传器
DefineError( ERR_MCU_CFG_INVALID_PRS             , ERR_MCU_BGN + 1606)
//多画面电视墙
DefineError( ERR_MCU_CFG_INVALID_MTVWALL         , ERR_MCU_BGN + 1607)
//mcu eqp 参数非法
DefineError( ERR_MCU_CFG_INVALID_MCUEQP          , ERR_MCU_BGN + 1608)  //调整错误号 1610->1608     
//mcu general cfg参数非法
DefineError( ERR_MCU_CFG_INVALID_MCUGEN          , ERR_MCU_BGN + 1609)  //调整错误号 1611->1609
//mcu rrq mtadp ipaddr 参数非法
DefineError( ERR_MCU_CFG_INVALID_RRQMTADPIP      , ERR_MCU_BGN + 1610)  //调整错误号 1612->1610
//高清bas
DefineError( ERR_MCU_CFG_INVALID_BASHD           , ERR_MCU_BGN + 1611)  //调整错误号 1613->1611
//4.6新加
//HDU
DefineError( ERR_MCU_CFG_INVALID_HDU             , ERR_MCU_BGN + 1612)
//SVMP
DefineError( ERR_MCU_CFG_INVALID_SVMP            , ERR_MCU_BGN + 1613)
//DVMP
DefineError( ERR_MCU_CFG_INVALID_DVMP            , ERR_MCU_BGN + 1614)
//MPUBAS
DefineError( ERR_MCU_CFG_INVALID_MPUBAS          , ERR_MCU_BGN + 1615)
//EBAP
DefineError( ERR_MCU_CFG_INVALID_EBAP            , ERR_MCU_BGN + 1616)
//EVPU
DefineError( ERR_MCU_CFG_INVALID_EVPU            , ERR_MCU_BGN + 1617)

//写配置文件失败
//单板
DefineError( ERR_MCU_CFG_WRITE_BRD               , ERR_MCU_BGN + 1620)
//混音器
DefineError( ERR_MCU_CFG_WRITE_MIXER             , ERR_MCU_BGN + 1621)
//录像机
DefineError( ERR_MCU_CFG_WRITE_REC               , ERR_MCU_BGN + 1622)
//vmp
DefineError( ERR_MCU_CFG_WRITE_VMP               , ERR_MCU_BGN + 1623)
//电视墙
DefineError( ERR_MCU_CFG_WRITE_TVWALL            , ERR_MCU_BGN + 1624)
//码流适配
DefineError( ERR_MCU_CFG_WRITE_BAS               , ERR_MCU_BGN + 1625)
//丢包重传器
DefineError( ERR_MCU_CFG_WRITE_PRS               , ERR_MCU_BGN + 1626)
//多画面电视墙
DefineError( ERR_MCU_CFG_WRITE_MTVWALL           , ERR_MCU_BGN + 1627)

//mcu eqp
DefineError( ERR_MCU_CFG_WRITE_MCUEQP            , ERR_MCU_BGN + 1628)
//画面合成风格
DefineError( ERR_MCU_CFG_WRITE_VMPSTYLE          , ERR_MCU_BGN + 1629)
//mcu本端信息
DefineError( ERR_MCU_CFG_WRITE_LOCALINFO         , ERR_MCU_BGN + 1630)
//mcu网络信息
DefineError( ERR_MCU_CFG_WRITE_NET               , ERR_MCU_BGN + 1631)
//qos
DefineError( ERR_MCU_CFG_WRITE_QOS               , ERR_MCU_BGN + 1632)
//网同步
DefineError( ERR_MCU_CFG_WRITE_NETSYN            , ERR_MCU_BGN + 1633)
//dcs配置
DefineError( ERR_MCU_CFG_WRITE_DCS               , ERR_MCU_BGN + 1634)
//MCU配置标识
DefineError( ERR_MCU_CFG_WRITE_ISCONFIGED        , ERR_MCU_BGN + 1635)
//dsc配置
DefineError( ERR_MCU_CFG_WRITE_DSC			  	 , ERR_MCU_BGN + 1636)
//LoginInfo配置
DefineError( ERR_MCU_CFG_WRITE_LOGININFO		 , ERR_MCU_BGN + 1637)
//高清bas
DefineError( ERR_MCU_CFG_WRITE_BASHD             , ERR_MCU_BGN + 1638)
//4.6新加版本 jlb
//hdu
DefineError( ERR_MCU_CFG_WRITE_HDU               , ERR_MCU_BGN + 1639)
//SVMP 
DefineError( ERR_MCU_CFG_WRITE_SVMP              , ERR_MCU_BGN + 1640)
//DVMP
DefineError( ERR_MCU_CFG_WRITE_DVMP              , ERR_MCU_BGN + 1641)
//MPUBAS
DefineError( ERR_MCU_CFG_WRITE_MPUBAS            , ERR_MCU_BGN + 1642)
//EBAP
DefineError( ERR_MCU_CFG_WRITE_EBAP              , ERR_MCU_BGN + 1643)
//EVPU
DefineError( ERR_MCU_CFG_WRITE_EVPU              , ERR_MCU_BGN + 1644)

//修改配置的行为发生冲突
DefineError( ERR_MCU_CFG_CONFLICT_WITHOTHERMCS	 , ERR_MCU_BGN + 1649)

//读写文件未知错误
DefineError( ERR_MCU_CFG_FILEERR                 , ERR_MCU_BGN + 1650)

//Hdu预案数过大错误定义
DefineError( ERR_MCU_CFG_HDUSCHEME_NUM_OVER      , ERR_MCU_BGN + 1651)

//读hdu配置失败
DefineError( ERR_MCU_CFG_READ_HDU                , ERR_MCU_BGN + 1652)

//预案正在被使用，不能修改
DefineError( ERR_MCU_CFG_CHANGEHDUSCHEME         , ERR_MCU_BGN + 1653)

//有空预案，不能添加
DefineError( ERR_MCU_CFG_HDUSCHEMEVOID           , ERR_MCU_BGN + 1654)

//保存外设扩展信息错误
DefineError( ERR_MCU_CFG_EQPEXCFGINFO            , ERR_MCU_BGN + 1655)

//8000E 保存GK，proxy配置信息错误
DefineError( ERR_MCU_CFG_GKPROXYCFGINFO          , ERR_MCU_BGN + 1656)

//8000E 保存Prs time span 配置信息错误
DefineError( ERR_MCU_CFG_PrsTimeSpanCfgInfo      , ERR_MCU_BGN + 1657)

//发送短消息繁忙
DefineError( ERR_MCU_SMSBUSY					 , ERR_MCU_BGN + 1658)

//多运营网络配置错误
DefineError( ERR_MCU_CFG_MULTIMANUNET			 , ERR_MCU_BGN + 1659)

// [2013/03/11 chenbing] 添加错误号 
//当前HDU通道不支持多画面
DefineError( ERR_MCU_HDUCHN_NOMULTIMODE			 , ERR_MCU_BGN + 1660)
//当前HDU不支持2个通道同时开启多画面
DefineError( ERR_MCU_HDUCHN_MULTIMODE_TOOMORE	 , ERR_MCU_BGN + 1661)
// 切换多画面输入参数错误 
DefineError( ERR_MCU_CHGMODE_INPUTPARAM_ERROR	 , ERR_MCU_BGN + 1662)
// 由于资源不足，画面合成失败 
DefineError( ERR_MCU_RESOURCES_NOT_ENOUGH		 , ERR_MCU_BGN + 1663)

//VCS业务相关错误(21701-)
//VCS调度席未配置电视墙或者配置了电视墙但均不在线
DefineError( ERR_MCU_VCS_NOUSABLETW               , ERR_MCU_BGN + 1700)
//VCS调度席未配置画面合成器或者配置了但不可用
DefineError( ERR_MCU_VCS_NOUSABLEVMP              , ERR_MCU_BGN + 1701)
// VCS当前会议模式正是要求改变的会议模式
DefineError( ERR_MCU_VCS_NOMODECHG                , ERR_MCU_BGN + 1702)
// 请求调度的资源非可调度资源
DefineError( ERR_MCU_VCS_NOVCSOURCE               , ERR_MCU_BGN + 1703)
// 模式已更改，操作被取消
DefineError( ERR_MCU_VCS_CANCEL                   , ERR_MCU_BGN + 1704)
// 该调度席正在被调度，不可修改或删除
DefineError( ERR_MCU_TEMPLATE_INVC                , ERR_MCU_BGN + 1705)
// 选看音频失败，选看者不接收音频或者被选看者不发送音频码流
DefineError( ERR_MCU_VCS_NOVCMTAUD                , ERR_MCU_BGN + 1706)
// 选看视频失败，选看者不接收视频或者被选看者不发送视频码流
DefineError( ERR_MCU_VCS_NOVCMTVID                , ERR_MCU_BGN + 1707)
// 选看音视频失败，选看者不接收音视频或者被选看者不发送音视频码流
DefineError( ERR_MCU_VCS_NOVCMTAV                 , ERR_MCU_BGN + 1708)
// 未检测到可用的混音器资源，请检查外设资源情况，进入多方调度远端点无声音
DefineError( ERR_MCU_VCS_NOMIXER                  , ERR_MCU_BGN + 1709)
// 本地终端掉线，不允许进行任何调度操作，且调度席恢复到最初状态
DefineError( ERR_MCU_VCS_LOCMTOFFLINE             , ERR_MCU_BGN + 1710)
// 本地终端能力不足，无法接收图像
DefineError( ERR_MCU_VCS_LOCNOACCEPTVIDEO         , ERR_MCU_BGN + 1711)
// 当前调度终端能力不足，无法接收图像
DefineError( ERR_MCU_VCS_REMNOACCEPTVIDEO         , ERR_MCU_BGN + 1712)
// 本地终端能力不足，无法接收声音
DefineError( ERR_MCU_VCS_LOCNOACCEPTAUDIO         , ERR_MCU_BGN + 1713)
// 当前调度终端能力不足，无法接收声音
DefineError( ERR_MCU_VCS_REMNOACCEPTAUDIO         , ERR_MCU_BGN + 1714)
// 正在调度终端
DefineError( ERR_MCU_VCS_VCMTING                  , ERR_MCU_BGN + 1715)
// 调度终端超时
DefineError( ERR_MCU_VCS_VCMTOVERTIME             , ERR_MCU_BGN + 1716)
// 超过了可同时进行的组呼数
DefineError( ERR_MCU_VCS_OVERGROUPNUM             , ERR_MCU_BGN + 1717)
// 主席轮询已开启
DefineError( ERR_MCU_VCS_CHAIRPOLLING             , ERR_MCU_BGN + 1718)
// 当前调度席允许的临时呼叫数已满，请结束调度清除
DefineError( ERR_MCU_VCS_OVERADDMTNUM             , ERR_MCU_BGN + 1719)

//当前操作员不具备操作该会议的权限
DefineError( ERR_MCU_VCS_NOTALLOWOPR              , ERR_MCU_BGN + 1720)
//MCS和VCS不能同时登陆
DefineError( ERR_MCU_VCSMCS_LOGONLYONE            , ERR_MCU_BGN + 1721)
//超过MCU允许接入的VCS数量
DefineError( ERR_MCU_VCSMCS_LOGVCSNUMOVER         , ERR_MCU_BGN + 1722)
//VCS软件名修改失败
DefineError( ERR_MCU_VCS_WRITESOTTNAME            , ERR_MCU_BGN + 1723)
//不可同时召开VCS和MCS会议
DefineError( ERR_MCU_VCSMCS_ONETYPECONF           , ERR_MCU_BGN + 1724)
//当前调度席已进入级联模式，正在被上级调度，本级无法调度
DefineError( ERR_MCU_VCS_MMCUINVC                 , ERR_MCU_BGN + 1725)
//**调度席正在调度中，无法进入级联模式，本级无法调度
DefineError( ERR_MCU_VCS_SMCUINVC                 , ERR_MCU_BGN + 1726)
//未检测到可用的混音器资源，请检查外设资源情况，进入该模式均听发言人
DefineError( ERR_MCU_VCS_NOMIXSPEAKER             , ERR_MCU_BGN + 1727)
//指定终端忙，在会议中,是否进行强拆
DefineError( ERR_MCU_VCS_MTBUSY					  , ERR_MCU_BGN + 1728)

//VCS没有多余回传通道
DefineError( ERR_MCU_VCS_NOUSABLEBACKCHNNL               , ERR_MCU_BGN + 1729)
//vcs要修改的预案名或者新的预案名已经存在
DefineError( ERR_MCU_VCS_PLANNAMEALREADYEXIST            , ERR_MCU_BGN + 1730)
//vcs预案名长度不正确
DefineError( ERR_MCU_VCS_PLANNAMELENNOTVALID	         , ERR_MCU_BGN + 1731)
//vcs预案名不存在
DefineError( ERR_MCU_VCS_PLANNAMENOTEXIST		         , ERR_MCU_BGN + 1732)
//vcs电视墙正在调度中,请稍候
DefineError( ERR_MCU_VCS_PLANISOPERATING		         , ERR_MCU_BGN + 1733)
//vcs预案正在修改,请稍候
DefineError( ERR_MCU_VCS_PLANISMODIFYING		         , ERR_MCU_BGN + 1734)
//vcs预案数量已满,无法添加
DefineError( ERR_MCU_VCS_PLANNUMOVERFLOW		         , ERR_MCU_BGN + 1735)
//vcs预案已经被其它VCS锁定
DefineError( ERR_MCU_VCS_PLANHASLOCKED			         , ERR_MCU_BGN + 1736)
//该调度席正在被他人修改,请稍后
DefineError( ERR_MCU_TEMPLATE_INEDITING                  , ERR_MCU_BGN + 1737)

//会议不支持多回传
DefineError( ERR_MCU_CONFNOTSUPPORTMULTISPY				 , ERR_MCU_BGN + 1738)
//会议回传带宽已满
DefineError( ERR_MCU_CONFSNDBANDWIDTHISFULL				 , ERR_MCU_BGN + 1739)
//级联适配器不足
DefineError( ERR_MCU_CASDBASISNOTENOUGH					 , ERR_MCU_BGN + 1740)

//下级mcu不支持多级联版本，本操作无效
DefineError( ERR_MCU_OPRINVALID_NOTSUPPORTMULTICASCADE   , ERR_MCU_BGN + 1741)

//回传带宽资源不足，无法完全满足所有调度应用
DefineError( ERR_MCU_VCS_SMCUSPYBWISNOTENOUGH			 , ERR_MCU_BGN + 1742)

//下级会议回传带宽满,无法回传终端
DefineError( ERR_MCU_SPYMTSTATE_WAITFREE				 , ERR_MCU_BGN + 1743)

//有终端正在发双流，不能设置第二广播源
DefineError( ERR_MCU_SECSPEAKER_DSNOTNULL				 , ERR_MCU_BGN + 1744)
//有第二广播源，不能发双流
DefineError( ERR_MCU_SECSPEAKER_SECSPEAKERNOTNULL		 , ERR_MCU_BGN + 1745)
//MCS直接创会或终端创会等（非模板创会）不支持预案
DefineError( ERR_MCS_CONFWITHOUTTEMPLATE_NOT_SUPPORT_PLAN, ERR_MCU_BGN + 1746)

//升级程序错误,您可以尝试以下操作:先升级到V4.7.1版本,然后再升级到您需要的版本!
DefineError( ERR_MCU_UPDATEBOARD_PIDCHECKERR			 , ERR_MCU_BGN + 1747)

//升级程序错误,文件校验失败!
DefineError( ERR_MCU_UPDATEBOARD_FILECHECKERR			 , ERR_MCU_BGN + 1748)

/* 用户管理模块扩展的错误码 19900-20000 */

DefineError( UM_EX_START                         , 19900 )
// 用户组满
DefineError( UM_EX_USRGRPFULL                    , UM_EX_START + 1 )
// 组名已存在
DefineError( UM_EX_USRGRPDUP                     , UM_EX_START + 2 )
// 组不存在
DefineError( UM_EX_USRGRPNOEXIST                 , UM_EX_START + 3 )
// 组ID已存在
DefineError( UM_EX_USRGRPIDDUP                   , UM_EX_START + 4 )
// 组ID非法
DefineError( UM_EX_USRGRPIDINV                   , UM_EX_START + 5 )


DefineErrorEnd
};

#undef DefineError
#undef DefineErrorEnd


#endif /* _ERR_MCU_H_ */


