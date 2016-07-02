/*****************************************************************************
   模块名      : H323Adapter
   文件名      : H323Adapter.h
   相关文件    : H323Adapter.cpp
   文件实现功能: 给上层应用提供API函数和回调函数原型
   作者        : 邓辉
   版本        : V3.0  Copyright(C) 2003 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2003/07/25  3.0         邓  辉      创建
   2004/07/19  3.1         魏治兵      代码优化
******************************************************************************/

#ifndef _H323ADAPTER_H
#define _H323ADAPTER_H

#include "osp.h"
//add by yy for ipv6
#include "rvaddress.h"
//#define RV_H323_TRANSPORT_ADDRESS RV_H323_TRANSPORT_ADDRESS_ANY
//end
#include "cm.h"
#include <time.h>
#include <math.h>


#ifdef WIN32
#define KDVCALLBACK CALLBACK
#else
#define KDVCALLBACK
#endif

#ifndef IN
#define IN
#endif//!IN
#ifndef OUT
#define OUT
#endif//!OUT

#ifdef _LINUX_
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#ifndef min
#define min(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif

#endif//!_LINUX_

///////////////////////////////// KDV H323 协议版本号 ///////////////////////////////
// 用于 TH323CONFIG.SetVersionId
//

#define KDVH323_VERID      (u8*)"8"

#define LEN_KDVH323_VERID  strlen((s8*)KDVH323_VERID)
//
////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////// 调试函数 ///////////////////////////////
API void kdv323adptver();
API void kdv323adpthelp();
API void pne();
API void set323trace( IN u8 byLevel);
API void setstacklog( IN s32 nLevel = 0, IN BOOL bFileLogOn = FALSE);
API void kdv323help();
API void radvaltree();//[20120619]
API void radallnode( IN BOOL bJustRootNode = TRUE );//[20120918]

///////////////////////////////// 长度定义 ///////////////////////////////
#define LEN_TERID			(u16)128				//终端名字最大长度
#define LEN_GUID			(u8)16					//GIUD长度
#define LEN_H245_CONFID		(u16)32					//H245会议ID 
#define LEN_CONFNAME		(u16)256				//h323-id
#define LEN_NETADDR			sizeof(TNETADDR)		//网路地址长度
#define LEN_MTNUM_PERCONF	(u16)193				//每个会议中的最大呼叫数
#define LEN_OID				(u16)16					//OID长度
#define LEN_CALLADDR		sizeof(TAliasAddr)		//呼叫地址长度
#define LEN_CHANPARAM		sizeof(TCHANPARAM)		//信道参数长度
#define LEN_TERINFO			sizeof(TTERINFO)		//终端信息长度
#define LEN_CAP				sizeof(TCapSet)			//能力参数长度
#define LEN_H243PWD			(u16)32                 //密码长度
#define LEN_ALIAS			(u16)64				    //最大别名长度h323-id
#define LEN_DHKEY_MAX		(u16)256                //dhkey的最大长度
#define LEN_DH_X			(u16)8                  //dh中的随机数长度
#define LEN_DHKEY			(u16)64                 //dhkey的长度
#define LEN_H235KEY_MAX		(u16)64					//(1-65535)  
#define LEN_DISPLAY_MAX		(u16)82                 //显示信息的最大长度
#define LEN_DISPLAY_MIN		(u16)2                  //显示信息的最小长度
#define MAX_CONF_NUM		(u16)32                 //最大会议数. Jacky Wei, changed from 32 to 24 to solve cmEvFacility lost issue(Bug00156267)
#define ANNEXQ_DYNAMIC_PT	(u16)100				//h.281动态载荷
#define LEN_USERINFO_MAX    (u8)131					//UserInfor的最大长度
#define LEN_USERINFO_MIN	(u16)2                  //UserInfor的最小长度
#define MAX_CAPTABLE_SIZE	(u16)64					//max 256
#define MAX_ALTCAP_NUM      MAX_CAPTABLE_SIZE		//max 256
#define MAX_SIMUCAP_NUM     (u16)16					//max 256 (Codian为13)
#define MAX_DESCAP_NUM      (u16)5					//max 256 (抓包发现都是1，很少有2以上)
#define MAX_H264CAP_NUM     (u8)10                  //H.264能力个数
#define MAXBITRATE_H261     (u16)19200				//单位100bps
#define MAXBITRATE_H263     (u32)192400				//单位100bps
#define CALLPORT			(u16)1720				//呼叫端口
#define RASPORT				(u16)1719				//RAS端口 
#define MAXALIASNUM			(u16)128				//最大别名数
#define MAXSessionDescript	(u8)128                 //支持的最大SessionDescript
#define MAX_ProductIDSize	(u16)256                //产品信息的最大长度
#define MAX_VersionIdSize	(u16)256                //版本信息的最大长度
#define MAX_LEN_CONFNAME    (u16)64					//会议名字最大长度
#define PRODUCTSTRING_LESS_SIZE        (u16)32      //产品信息的长度,用于获取对方的产品信息
#define VERSIONSTRING_LESS_SIZE        (u16)32      //版本信息的长度,用于获取对方的版本信息
#define LEN_NONSTANDARD_MAX (u16)2048               //setup消息中非标数据的最大长度
#define LEN_DES_KEY         (u8)8                   //DES的密钥长度
#define LEN_AES_KEY         (u8)16                  //AES的密钥长度
#define MAX_CALL_NUM        (u16)256
#define MIN_CALL_NUM        (u16)10
#define MAX_CHANNEL_NUM     (u16)20
#define MIN_CHANNEL_NUM     (u16)12
#define RequestMsg_BGN		(u16)101
#define RequestMsg_END		(u16)300
#define ResponseMsg_BGN		(u16)301
#define ResponseMsg_END		(u16)500
#define CommandMsg_BGN		(u16)501
#define CommandMsg_END		(u16)700
#define IndicationMsg_BGN	(u16)701
#define IndicationMsg_END	(u16)900
#define LEN_256				(u16)256
#define LEN_1				(u16)1
#define LEN_128				(u16)128
#define REG_NAME_LEN		(u16)64
#define REG_PWD_LEN			(u16)64
#define VALID_LEN_ALIAS      (u16)16   //有效别名它度  
//add by yj for ipv6
#define IPV6_NAME_LEN		(u16)64  //IPv6地址名长度
#define INVALID_NODEID      RV_PVT_INVALID_NODEID
#define TYPE_NONE           RV_ADDRESS_TYPE_NONE         //表示为未定义地址
#define TYPE_IPV4           RV_ADDRESS_TYPE_IPV4         //表示支持IPv4地址
#define TYPE_IPV6           RV_ADDRESS_TYPE_IPV6         //表示支持IPv6地址
//#define TYPE_SCTP           RV_NET_SCTP         //表示同事支持IPv4、IPv6地址
//end
       


#define FLAG_H460FEATURESIGNALL			18			//460.18
#define FLAG_H460MEDIAFWSIGNALL			19			//460.19     

#define FLAG_H460SUPPORTMULTIPLEXDMEDIA	1			//支持端口复用
#define FLAG_H460MEDIATRAVESALSERVER	2			//本端为460服务器

///////////////////////////////// 消息类型 ///////////////////////////////

//呼叫消息
enum H225CallMsgType 
{
	h_call_invalidInterface = 0,
    h_call_make,		            //发起呼叫	      [outgoing]: TCALLPARAM
	h_call_incoming,		        //呼叫到来		  [incoming]: TCALLPARAM + VendorProductId		  	  	
	h_call_answer,		            //接受(拒绝)呼叫  [outgoing]: TNODEINFO
	h_call_drop,                    //结束呼叫		  [outgoing]: 1 byte,reason
	h_call_connected,               //呼叫建立		  [incoming]: TNODEINFO	
	h_call_disconnected,            //呼叫断开		  [incoming]: 1 byte,reason	
	h_call_facility,			    //呼叫扩展处理	  [incoming]&[outgoing]: TFACILITYINFO   
	h_call_h460make,		        //发起h460呼叫	  [outgoing]: TH460CALLPARAM
	//add by daiqing 20100723 for 460 kdv
	h_call_h460CallerMake,			//公网终端收到facility发起呼叫 [outgoing]: TCALLPARAM
	//end
    //Jacky Wei add h_call_alerting event for KDV1000
    h_call_alerting,                //Useless except for KDV1000 processes
};

//RAS消息
enum H225RasMsgType 
{
	h_ras_invalidInterface = 0,
	h_ras_GRQ,						//reserve
	h_ras_GCF,						//reserve
	h_ras_GRJ,						//reserve
	h_ras_RRQ,                      //[incoming]&[outgoing]:[TRRQINFO]
	h_ras_RCF,						//[incoming]&[outgoing]:[TRCFINFO]    
	h_ras_RRJ,                      //[incoming]&[outgoing]:[int:reason]
	h_ras_LWRRQ,                    //[incoming]&[outgoing]:[TRRQINFO]
	h_ras_ARQ,						//[incoming]&[outgoing]:[HCALL][TARQINFO]
	h_ras_ACF,                      //[incoming]&[outgoing]:[HCALL][TACFINFO]
	h_ras_ARJ,                      //[incoming]&[outgoing]:[HCALL][int:reason]
	h_ras_DRQ,                      //[incoming]&[outgoing]:[HCALL][TDRQINFO]
	h_ras_DCF,						//[incoming]&[outgoing]:[HCALL][TDCFINFO]
	h_ras_DRJ,						//[incoming]&[outgoing]:[HCALL][int:reason]
	h_ras_URQ,						//[incoming]&[outgoing]:[TURQINFO]
	h_ras_UCF,						//[incoming]&[outgoing]:[TUCFINFO]
	h_ras_URJ,						//[incoming]&[outgoing]:[int:reason]
	h_ras_IRQ,                      //[incoming]&[outgoing]:[HCALL][TIRQINFO]
	h_ras_IRQTimeOut,               //[incoming]:           [HCALL]
	h_ras_IRR,						//[incoming]&[outgoing]:[HCALL][TIRRINFO]
	h_ras_BRQ,						//[incoming]&[outgoing]:[HCALL][TBRQINFO] 
	h_ras_BCF,                      //[incoming]&[outgoing]:[HCALL][TBCFINFO]
	h_ras_BRJ,						//[incoming]&[outgoing]:[HCALL][int:reason]
	h_ras_LRQ,						//[incoming]&[outgoing]:[TLRQINFO]
	h_ras_LCF,                      //[incoming]&[outgoing]:[TLCFINFO]
	h_ras_LRJ,                      //[incoming]&[outgoing]:[int:reason]

	h_ras_registration,				//自动RAS修改注册信息重新向GK注册	[outgoing]:TRRQINFO
	h_ras_unregistration,			//自动RAS向GK注销				    [outgoing]:NULL
	h_ras_online,					//自动RAS在GK注册成功			    [incoming]:NULL
	h_ras_offline,					//自动RAS未在GK注册			        [incoming]:NULL

	h_ras_NSM,                      //非标消息
	//add by daiqing 20100720 for 460
	h_ras_SCI,                      //[incoming]&[outgoing]:[TSCIINFO]
	h_ras_SCR,                      //[incoming]&[outgoing]:[s32]

	h_ras_manual_NSM,				//手动RAS 非标消息 [20121018]
	
};

//逻辑通道消息
enum H245ChanMsgType 
{
	h_chan_invalidInterface = 0,
    h_chan_open,						//[incoming]&[outgoing]: [NULL]&[TCHANPARAM] 打开逻辑通道
	h_chan_openAck,						//[incoming]           : [TCHANPARAM]        同意打开逻辑通道
	h_chan_openReject,					//[outgoing]           : [emlogChannelRejectCause]     拒绝打开逻辑通道
	h_chan_connected,					//[incoming]           : NULL                信道已建立
	h_chan_close,						//[outgoing][incoming] : NULL                关闭逻辑通道	

	h_chan_parameter,					//[incoming]	       : [TCHANPARAM]        得到逻辑信道媒体参数和数据信道地址
	h_chan_remoteRtpAddress,			//[incoming]           : [TNETADDR]          得到对端RTP地址 
	h_chan_remoteRtcpAddress,			//[incoming]           : [TNETADDR]          得到对端RTCP地址
	h_chan_dynamicpayloadtype,			//[incoming]           : [u8]                得到通道的动态载荷类型    
	h_chan_VideoFreezePicture,			//[incoming]&[outgoing]: [NULL]&[NULL]       图像冻结命令
	h_chan_videofastupdatePIC,			//[incoming]&[outgoing]: [NULL]&[NULL]       图像更新命令
	h_chan_videofastupdateGOB,			//[incoming]&[outgoing]: [TVFUGOB]&[TVFUGOB] GOB更新命令
	h_chan_videofastupdateMB,			//[incoming]&[outgoing]: [TVFUMB]&[TVFUMB]   MB更新命令
	h_chan_VideoSendSyncEveryGOB,		//[incoming]&[outgoing]: [NULL]&[NULL]       use sync for every GOB as defined in H.263
	h_chan_VideoSendSyncEveryGOBCancel, //[incoming]&[outgoing]: [NULL]&[NULL]       cancel use sync for every GOB as defined in H.263
    h_chan_SwitchReceiveMediaOn,        //[incoming]&[outgoing]: [NULL]&[NULL]       logical channel should be used for receive
	h_chan_SwitchReceiveMediaOff,       //[incoming]&[outgoing]: [NULL]&[NULL]       logical channel should not be used for receive
	h_chan_tsto,						//[incoming]&[outgoing]: [TTSTO]             temporal spatial tradeoff operation.
	h_chan_flowControl,					//[incoming]&[outgoing]: [u16:rate(kbps)]    码流控制命令

	h_chan_on,							//[incoming]&[outgoing]: [NULL]&[NULL]       激活逻辑通道，    开始接收数据    
	h_chan_off,							//[incoming]&[outgoing]: [NULL]&[NULL]       冻结逻辑通道，    停止接收数据

	h_chan_MediaLoopRequest,			//[incoming]: [HCHAN]
	h_chan_MediaLoopConfirm,			//[incoming]: [HCHAN]
	h_chan_MediaLoopReject,				//[incoming]: [HCHAN]
	h_chan_MediaLoopOff,				//[incoming]: [HCALL]
	h_chan_EncryptSyn,                  //[incoming]: [TEncryptSync]
	h_chan_EncryptionUpdateRequest,     //[incoming]: [HCHAN]
	h_chan_EncryptionUpdateCommand,     //[incoming]: [HCHAN][TEncryptSync]

    h_chan_flowControlIndication,   	//[incoming]&[outgoing]: [u16:rate(kbps)]    码流控制指示
	h_chan_keepAliveAddress,			//[incoming]           : [TNETADDR]          得到对端keepAlive地址added by wangxiaoyi for h460 keepalivechannel
	h_chan_newchan,							  //[incoming]           : [HCHAN]          OnNewChan的消息
    
};


/*
 H245 message
 0. h_ctrl_h245TransportConnected表示h245链路的建立。在收到该条消息前不得发送任何H245消息。
 1. 能力集(capset)交互和主从决定(MSD)在h_ctrl_h245TransportConnected后发出。上层必须发出这两个消息。
 2. 双方能力集交互和主从决定都完成后，才会收到h_ctrl_h245Established消息。
 3. 主从决定判断由协议栈处理。只有在本端还没有发送主从决定消息时，协议栈才会报告对方的主从决定request，即h_ctrl_masterSlaveDetermine
 4. 打开通道消息必须在收到h_ctrl_h245Established后才能发送。
*/


//会议控制消息
enum H245CtrlMsgType 
{
	h_ctrl_invlaidInterface = 0,
	h_ctrl_h245TransportConnected,          //[incoming]	       :[NULL]	       h245通道刚刚建立, 此时可以开始能力交换和主从确定
	h_ctrl_h245Established,                 //[incoming]		   :[NULL]		   能力交换和主从确定完成,可以开始打开逻辑通道，收到CancelMultipointConference 也会发此消息 
	h_ctrl_masterSlave,						//[incoming]		   :[u8:result]	   主从决定ack, [incoming]--result表示本端主从 
	h_ctrl_masterSlaveDetermine,            //[outgoing]&[incoming]:[s32[2]]       主从决定request, 4byte(terminal Type， 50－240) + (optional)4 bytes(random number 0-0xffffff), 
	h_ctrl_capNotifyExt,                    //[incoming]&[outgoing]:[TCapSet]&[TCapSet]  对端能力指示
	h_ctrl_capResponse,						//[incoming]&[outgoing]:[u8: result]         能力集响应

	/************************h.245 Standard**************************/
	h_ctrl_MultipointConference,            //开始多点会议指示  MC->ter, Master->slave,   [incoming]&[outgoing]: NULL
	h_ctrl_CancelMultipointConference,      //取消多点会议指示  MC->ter, Master->slave,   [outgoing]: NULL

	h_ctrl_communicationModeCommand,        //会议模式命令	    MC->ter,   [incoming]&[outgoing]: [TCOMMODEINFO[]],目前不使用
	
	h_ctrl_enterH243Password,				//MC询问会议密码    MC->ter,   [incoming]&[outgoing]: NULL
	h_ctrl_passwordResponse,				//终端回答密码	    ter->MC,   [incoming]&[outgoing]: [TPassRsp]
	h_ctrl_terminalNumberAssign,            //分配终端编号	    MC->ter,   [incoming]&[outgoing]: [TTERLABEL]
	h_ctrl_terminalListRequest,             //请求终端列表	    ter->MC,   [incoming]&[outgoing]: NULL			
	h_ctrl_terminalListResponse,            //回答终端列表	    MC->ter,   [incoming]&[outgoing]: [TTERLABEL[]]
	h_ctrl_requestTerminalID,               //ter询问终端名	    ter->MC,   [incoming]&[outgoing]: [TTERLABEL]
	h_ctrl_mCTerminalIDResponse,            //MC回答终端名	    MC->ter,   [incoming]&[outgoing]: [TTERINFO]
	h_ctrl_enterH243TerminalID,				//MC询问终端名	    MC->ter,   [incoming]&[outgoing]: NULL
	h_ctrl_terminalIDResponse,              //ter回答终端名	    ter->MC,   [incoming]&[outgoing]: [TTERINFO]
	h_ctrl_requestAllTerminalIDs,           //ter请求所有终端名 ter->MC,   [incoming]&[outgoing]: NULL
	h_ctrl_requestAllTerminalIDsResponse,   //回答所有终端名字	MC->ter,   [incoming]&[outgoing]: [TTERINFO[]]
	h_ctrl_terminalJoinedConference,        //终端加入会议		MC->ter,   [incoming]&[outgoing]: [TTERLABEL]
	h_ctrl_terminalLeftConference,          //终端退出会议		MC->ter,   [incoming]&[outgoing]: [TTERLABEL]
	h_ctrl_dropTerminal,                    //强制终端退出		chair->MC, [incoming]&[outgoing]: [TTERLABEL]
	h_ctrl_terminalDropReject,              //强制终端退出失败	MC->chair, [incoming]&[outgoing]: NULL	
	h_ctrl_dropConference,                  //结束会议 chair->MC,MC->ter,  [incoming]&[outgoing]: NULL

	h_ctrl_requestForFloor,                 //申请发言			ter->MC,   [incoming]&[outgoing]: NULL
	h_ctrl_floorRequested,                  //发言申请			MC->chair, [incoming]&[outgoing]: [TTERLABEL]
	h_ctrl_makeTerminalBroadcaster,         //广播终端			chair->MC, [incoming]&[outgoing]: [TTERLABEL+u8:CtrlMode]
	h_ctrl_makeTerminalBroadcasterResponse, //广播应答			MC->chair, [incoming]&[outgoing]: [u8:CtrlResult]
	h_ctrl_cancelMakeTerminalBroadcaster,   //取消广播			chair->MC, [incoming]&[outgoing]: NULL
	h_ctrl_seenByAll,                       //广播源指示		MC->ter,   [incoming]&[outgoing]: NULL
	h_ctrl_cancelSeenByAll,                 //广播源被取消指示	MC->ter,   [incoming]&[outgoing]: NULL
	h_ctrl_seenByAtLeastOneOther,           //告知被其他终端选看  MC->ter, [incoming]&[outgoing]: NULL
	h_ctrl_cancelSeenByAtLeastOneOther,     //告知被其他终端停看  MC->ter, [incoming]&[outgoing]: NULL
	h_ctrl_sendThisSource,                  //选看终端			chair->MC, [incoming]&[outgoing]: [TTERLABEL+u8:CtrlMode]
	h_ctrl_sendThisSourceResponse,          //选看应答			MC->chair, [incoming]&[outgoing]: [u8:CtrlResult]
	h_ctrl_cancelSendThisSource,            //取消选看			chair->MC, [incoming]&[outgoing]: NULL
	h_ctrl_terminalYouAreSeeing,            //正在观看的视频源	MC->ter,   [incoming]&[outgoing]: [TTERLABEL]

	h_ctrl_makeMeChair,                     //申请主席			ter->MC,   [incoming]&[outgoing]: NULL
	h_ctrl_cancelMakeMeChair,               //释放主席			chair->MC, [incoming]&[outgoing]: NULL
	h_ctrl_makeMeChairResponse,             //主席申请/释放应答	MC->ter,   [incoming]&[outgoing]: [u8:CtrlResult]
	h_ctrl_withdrawChairToken,              //主席撤销指示		MC->ter,   [incoming]&[outgoing]: NULL
	h_ctrl_requestChairTokenOwner,          //询问当前主席		ter->MC,   [incoming]&[outgoing]: NULL
	h_ctrl_chairTokenOwnerResponse,         //回答当前主席		MC->ter,   [incoming]&[outgoing]: [TTERINFO]
	h_ctrl_mcLocationIndication,			//含有MC的实体地址	MC->ter,   [incoming]&[outgoing]: [TNETADDR]
    h_ctrl_roundTripDelay,					//检查对端是否Alive ter->MC,MC->ter, [incoming]&[outgoing]: [s32:nMaxDelay]
    
    h_ctrl_H239Message,                     //发送H239消息      ter->MC,MC->ter, [incoming]&[outgoing]: [TH239INFO]
    h_ctrl_userInput,                       //发送UserInput消息            [incoming]&[outgoing]: [TUserInputInfo]

    h_ctrl_roundTripDelay_request,          //h_ctrl_roundTripDelay请求的附加消息(防火墙内部使用，用于区分请求和应答)

	h_ctrl_customFecc,						//FECC消息（使用H245进行FECC消息的传递，主要用于pxy）

    h_ctrl_multiplexCapability,             //multiplexCapability消息,  [incoming]&[outgoing]: [TMultiplexCapability]
	
	h_ctrl_masterSlaveResponse,             //[outgoing]                           主从决定response，用于收到对方masterslaveresquest后，手动回复对方ack
	h_ctrl_OutgoingMultipointConference,	//[outgoing]		转发h_ctrl_MultipointConference消息，不产生回报


	h_ctrl_standardend              = 100,
	
	h_ctrl_mcuvccommonnonstandstart = 101,  //MCU产品自定义的消息 12 Januaray 2011
	h_ctrl_chairTransferReq,				//主席终端转移令牌请求	chair->MC, TTERLABEL
	h_ctrl_chairInviteTerminalReq,			//主席邀请终端请求		chair->MC, TTERADDR
	h_ctrl_terStatusReq,					//终端状态请求			MC->ter,   NULL
	h_ctrl_pollParamReq,					//查询轮询参数			MC->ter,   NULL	
	h_ctrl_whoViewThisTerReq,				//询问MCU A终端被谁选看	ter->MC,   TTERLABEL(A)
	h_ctrl_confInfoReq,						//询问会议信息			ter->MC,   NULL
	h_ctrl_delayConfTimeReq,				//延长会议时间			chair->MC, u16, minutes
	h_ctrl_letTerViewMeReq,					//强制选看请求			ter->MC,   TTERLABEL(被强制选看者) + 1BYTE(DataType) 
	h_ctrl_multicastTerReq,					//组播终端请求			ter->MC,   TTERLABEL(目的终端)	
	h_ctrl_destterStatusReq,                //源终端请求目的终端状态     ter->MC,TTERLABEL(目的终端)

	h_ctrl_whoViewThisTerRsp,				//对应whoViewThisTerReq	MC->ter, TTERLABEL(被选看终端) + 1Byte(选看终端个数) + TTERLABEL数组
	h_ctrl_pollParamRsp,					//轮询参数应答			ter->MC,  
	                                        //类型(1Byte,PollType) + 模式(1Byte,DataType) + 当前状态(1Byte, PollStatus)
											//+ 当前轮询到的终端(TTERLABEL，+ 时间间隔(u16,s). 如果没有则全填0)
											//终端个数(1Byte) + 列表(TTERLABEL数组 + 时间间隔(1Byte,s))

											//类型(1Byte,PollType) + 模式(1Byte,DataType) + 时间间隔(1Byte,s) + 终端个数(1Byte) + 列表(TTERLABEL数组)
	h_ctrl_confInfoRsp,						//回答会议信息			MC->ter, 透明转发给MTC
	h_ctrl_delayConfTimeRsp,				//延长会议时间应答		1BYTE,granted/denied
	h_ctrl_letTerViewMeRsp,					//强制选看应答			MC->ter,   1BYTE(granted/denied)
	
	h_ctrl_startVACReq,						//开始语音激励请求 chair->MC, NULL
	h_ctrl_stopVACReq,						//停止语音激励请求 chair->MC, NULL
	h_ctrl_startVACRsp,						//开始语音激励请求响应 MC->ter, 1BYTE(granted/denied)
	h_ctrl_stopVACRsp,						//停止语音激励请求响应 MC->ter,1BYTE(granted/denied)

	h_ctrl_startDiscussReq,					//开始讨论 chair->MC, 1byte(同时参加讨论(混音)的成员个数)
	h_ctrl_stopDiscussReq,					//停止讨论 chair->MC, NULL
	h_ctrl_startDiscussRsp,					//开始讨论请求响应 MC->ter,1BYTE(granted/denied)
	h_ctrl_stopDiscussRsp,					//停止讨论请求响应 MC->ter,1BYTE(granted/denied)
	
	h_ctrl_startVMPReq,						//开始视频复合 chair->MC, struct TVMPParam
	h_ctrl_stopVMPReq,						//停止视频复合 chair->MC, NULL
	h_ctrl_changeVMPParamReq,				//改变视频复合参数 chair->MC, struct TVMPParam
	h_ctrl_getVMPParamReq,					//得到视频复合参数 chair->MC, NULL
	h_ctrl_startVMPRsp,						//开始视频复合请求响应 MC->ter,1BYTE(granted/denied)
	h_ctrl_stopVMPRsp,						//停止视频复合请求响应 MC->ter,1BYTE(granted/denied)
	h_ctrl_changeVMPParamRsp,				//改变视频复合参数请求响应 MC->ter,1BYTE(granted/denied)
	h_ctrl_getVMPParamRsp,					//得到视频复合参数请求响应 MC->ter,struct TVMPParam
	
	h_ctrl_multicastTerRsp,					//组播终端应答   MC->ter,  TTERLABEL(目的终端) + u32(ip,如果失败ip为0) 
											//                       + u16(音频端口号)     + u16(视频端口号)
											//                       + u8(音频真实载荷值)  + u8(视频真实载荷值)
											//                       + u8(音频加密类型)    + u8(视频加密类型) -- emEncryptTypeNone等
											//                       + TEncryptSync(音频加密信息) + TEncryptSync(视频加密信息)
	
	h_ctrl_destterStatusRsp,                //请求目的终端状态的响应   MC->ter,TTERLABEL(目的终端) + _TTERSTATUS

	h_ctrl_makeTerQuietCmd,					//远端静音命令			ter->MC,MC->ter,  TTERLABEL(dst) + 1BYTE(turn_on/turn_off)
	h_ctrl_makeTerMuteCmd,					//远端哑音命令			ter->MC,MC->ter,  TTERLABEL(dst) + 1BYTE(turn_on/turn_off)
	h_ctrl_startPollCmd,					//开始轮询命令			MC->ter,    
											//类型(1Byte,PollType) + 模式(1Byte,DataType) + 终端个数(1Byte) + 列表(TTERLABEL数组 + 时间间隔(1Byte,s))
	h_ctrl_stopPollCmd,		   				//停止轮询命令			MC->ter,	NULL
	h_ctrl_pausePollCmd,					//暂停轮询				MC->ter,	NULL
	h_ctrl_resumePollCmd,					//恢复轮询				MC->ter,	NULL
											//						action(1Byte,对于普通操作:turn_on(开始)/turn_off(停止);对于预置命令:后面跟1-4表示预置位)
	h_ctrl_stopViewMeCmd,					//停止强制选看			ter->MC
	h_ctrl_stopMulticastTer,				//停止组播终端			ter->MC,	TTERLABEL
	
    h_ctrl_matrixCfgCmd,                    //为兼容3.6终端保留, 矩阵配置命令			MC->ter,	1Byte(MT_AUDIO/MT_VIDEO/MT_AV) + _TMATRIXPORT(Output) + _TMATRIXPORT(Input)
	
    h_ctrl_chairSelectViewCmd,				//主席选看				ter->MC,    <m,t> + 1Byte(type_audio/type_video/type_av)	

	h_ctrl_chairInviteFailedInd,			//主席邀请终端失败		MC->chair, TTERADDR
	h_ctrl_confWillEndInd,					//会议即将结束指示		MC->chair, u16 minutes, 表示还有多久结束
	h_ctrl_delayConfTimeInd,                //延长会议时间指示      MC->ter, u16 分钟
	h_ctrl_terApplyChairInd,				//有终端申Ind,					//终端状态指示			ter->MC,   _TTERSTATUS ( response h_ctrl_terStatusReq)	
	h_ctrl_terStatusInd,					//终端状态指示			ter->MC,   _TTERSTATUS ( response h_ctrl_terStatusReq)	
	h_ctrl_pollParamInd,                    //轮询状态指示          类型(1Byte,PollType) + 模式(1Byte,DataType) + 当前状态(1Byte, PollStatus) 
	                                        //                      + 当前轮询到的终端(TTERLABEL，+ 时间间隔(u16,s). 如果没有则全填0)
	h_ctrl_smsInd,							//短消息指示			ter->MC,MC->ter,TTERLABEL(src) + TTERLABEL(dst) + (1BYTE:滚动次数)+字符串('\0'结束)	
	h_ctrl_broadcasterTokenOwnerInd,		//新广播源通告			MC->ter, TTERLABEL

	h_ctrl_matrixSchemeLoadCmd,             //为兼容3.6终端保留, 矩阵方案载入命令      MC->ter,用指定方案设置当前配置 TMATRIXSCHEME结构
	h_ctrl_matrixSchemeSaveCmd,             //为兼容3.6终端保留, 矩阵方案保存命令      MC->ter,保存当前配置为指定方案 TMATRIXSCHEME结构
	h_ctrl_defaultmatrixInfoCmd,            //为兼容3.6终端保留, 矩阵默认配置信息命令  MC->ter,1Byte(MT_AUDIO/MT_VIDEO/MT_AV)
	h_ctrl_matrixInfoInd,                   //为兼容3.6终端保留, 当前矩阵配置信息指示	ter->MC,1Byte(MT_AUDIO/MT_VIDEO/MT_AV) + _TMATRIXINFO(如果是AV则为数组,A+V)
	
    h_ctrl_termSourceNameInd,               //终端视频源名称指示    MC->ter,消息体是字符串，'\0'结束
	
    h_ctrl_matrixSchemeInd,                 //为兼容3.6终端保留, 所有矩阵方案指示      ter->MC,TMATRIXSCHEME结构的数组，共8套方案
	
    h_ctrl_feccCmd,							//远遥命令				ter->MC,MC->ter,  TTERLABEL(dst) + Command(1Byte,命令类型,FeccCommand) + 
	
    h_ctrl_matrixInfoReq,                   //为兼容3.6终端保留, 请求矩阵当前配置信息	MC->ter,1Byte(MT_AUDIO/MT_VIDEO/MT_AV)
	h_ctrl_matrixSchemeReq,                 //为兼容3.6终端保留, 所有矩阵方案请求 MC->ter,1Byte(MT_AUDIO/MT_VIDEO/MT_AV)
	
    h_ctrl_termVideoSrcCmd,					//选择终端视频源		MC->ter, 1BYTE(视频源索引0:S端子 1~16:C端子)

	h_ctrl_chimeInReq,						//终端申请插话				ter->MC,		NULL
	h_ctrl_makeTerChimeInCmd,				//主席指定终端插话			ter->MC,		TTERLABEL
	h_ctrl_chimeInInd,						//终端插话状态				MC->ter			1字节（0: 不在插话, 1: 在插话）
	h_ctrl_otherTerChimeInInd,				//其它终端申请插话指示		MC->chair		TTERLABEL

    // mcu
    h_ctrl_McuMcuAddMtReq,					//MCU邀请下级mcu上的终端       Mcu->Mcu  TAddMtInfo数组
    h_ctrl_McuMcuAddMtRsp,                  //MCU邀请下级mcu上的终端应答   Mcu->Mcu  1BYTE (granted/denied)
    
    h_ctrl_McuMcuCallMtCmd,                 //MCU呼叫下级mcu上的终端       Mcu->Mcu   TMt
    h_ctrl_McuMcuCallAlertingInd,           //正在呼叫通报                 Mcu->Mcu   TMt
    h_ctrl_McuMcuMtJoinedInd,               //新终端加入通报               Mcu->Mcu   TMt
    
    h_ctrl_McuMcuDropMtCmd,                 //mcu挂断下级mcu上的终端       Mcu->Mcu   TMt
    h_ctrl_McuMcuDelMtCmd,                  //mcu删除下级mcu上的终端       Mcu->Mcu   TMt
    h_ctrl_McuMcuMtDisconnectedInd,         //终端挂断通知                 Mcu->Mcu   TMt
    h_ctrl_McuMcuMtDeletedInd,              //终端离会通告                 Mcu->Mcu   TMt
    
    h_ctrl_McuMcuGetMtListReq,              //Mcu之间请求对方终端列表      Mcu->Mcu   Null
    h_ctrl_McuMcuGetMtListRsp,              //Mcu之间请求对方终端列表应答  Mcu->Mcu   TMt+TmtAlias数组
    h_ctrl_McuMcuConfMtInfoInd,             //终端信息通知                 Mcu->Mcu   TConfMtInfo + TMtEx数组
    
    h_ctrl_McMcuSetReverseChannelCmd,       //指定回传通道,消息体TMt       Mcu->Mcu   Tmt
    h_ctrl_McuMcuNewReverseChannelInd,      //新回传通道通知               Mcu->Mcu   Tmt
    h_ctrl_McuMcuAudioInfoReq,              //请求音频信息                 Mcu->Mcu   Null
    h_ctrl_McuMcuAudioInfoRsp,              //请求音频信息响应             Mcu->Mcu   TMt数组
    h_ctrl_McuMcuVideoInfoReq,              //请求视频信息                 Mcu->Mcu   Null
    h_ctrl_McuMcuVideoInfoRsp,              //请求视频信息响应             Mcu->Mcu   TMt数组
    
    h_ctrl_McuMcuConfMtStatusInd,           //终端状态上报                 Mcu->Mcu   TMcu+TMtStatus数组
    h_ctrl_McuMcuFeccCmd,                   //摄像头远遥                   Mcu->Mcu   TMt + 1byte：远遥操作类型 + 1 byte: 远遥参数
    h_ctrl_McuMcuSendMsgInd,                //终端短消息通知               Mcu->Mcu   消息体：TMt(终端号为0广播到所有终端)+TROLLMSG(有效长度)
    
    h_ctrl_McuMcuNewSpeakerInd,             //上级改变发言人通知           Mcu->Mcu   消息体：TMt+1byte(媒体模式:视频或音频或两种)
    
    //add by jason 2005-10-11
    h_ctrl_SimpleConfInfoInd,               //mcu通知终端更新会议信息      MC->ter			1字节（0: 不在插话, 1: 在插话）
    
    h_ctrl_terBandWidthInd,                 //mt接收发送带宽通知           ter->MC      u16(接收带宽（包括音频和视频）,网络序) + u16(发送带宽（包括音频和视频）,网络序)
    h_ctrl_terBitrateInfoReq,               //mcu请求终端码率              MC->ter      消息体：无
    h_ctrl_terBitrateInfoRsp,               //mt对终端码率应答             ter->MC      消息体：_TERBITRATE

    // 4.0外置矩阵
	h_ctrl_matrixExGetInfoCmd,              //获取终端外置矩阵类型

    h_ctrl_matrixExInfoInd,                 //终端外置矩阵类型指示
	                                        //    +u8 矩阵输入端口数 0表示没有矩阵

    h_ctrl_matrixExSetPortCmd,              //设置外置矩阵连接端口号
	                                        //    +u8  (1-64 )

    h_ctrl_matrixExGetPortReq,              //请求获取外置矩阵连接端口

    h_ctrl_matrixExPortInd,                 //外置矩阵连接端口指示
	                                        //    +u8  (1-64 )

    h_ctrl_matrixExSetPortNameCmd,          //设置外置矩阵连接端口名
	                                        //    +u8  (1-64 )
										    //    +s8[MAXLEN_MATRIXPORTNAME] 0结尾的字符串 

    h_ctrl_matrixExPortNameInd,             //外置矩阵端口名指示
	                                        //    +u8  (1-64 )
										    //    +s8[MAXLEN_MATRIXPORTNAME] 0结尾的字符串

    h_ctrl_matrixExGetAllPortNameCmd,       //请求获取外置矩阵的所有端口名

    h_ctrl_matrixExAllPortNameInd,          //请求获取外置矩阵的所有端口名
	                                        //    +s8[][MAXLEN_MATRIXPORTNAME]
									        //    索引0 表示输出端口名 其余位输入端口米安
    //4.0 内置矩阵
    h_ctrl_innerMatrixGetAllSchemesCmd,     /*****************<< 获取矩阵方案所有内容请求  >>**************	
                                             *[消息体]		
                                             * 无   
                                             */                              
                                                                                  
    h_ctrl_innerMatrixGetAllSchemesInd,     /***************<< 获取矩阵方案所有内容指示  >>****************	
                                             *[消息体]			                  
                                             *    + u8( 方案数 ) + ITInnerMatrixScheme[ ]
                                             */ 

    h_ctrl_innerMatrixGetOneSchemeCmd,      /**************<< 获取矩阵方案一个内容请求  >>*****************	
                                             *[消息体]		
                                             *    + u8 ( 0 为默认方案 )       方案索引从( 0 - 6 )
                                             */  
                                                
                                                
    h_ctrl_innerMatrixGetOneSchemeInd,      /***************<< 获取矩阵方案一个内容指示  >>****************	
                                             *[消息体]			 
                                             *    ITInnerMatrixScheme
                                             */  
                                                
    h_ctrl_innerMatrixSaveSchemeCmd,        /***********************<< 保存方案  >>************************	
                                             *[消息体]			 
                                             *    +  ITInnerMatrixScheme
                                             */ 
                                                
    h_ctrl_innerMatrixSaveSchemeInd,        /*********************<< 保存方案指示  >>**********************	
                                             *[消息体]			 
                                             *    +  ITInnerMatrixScheme
                                             */ 
                                                
    h_ctrl_innerMatrixSetCurSchemeCmd,      /*****************<< 调用指定方案为当前方案  >>****************	
                                             *[消息体]			 
                                             *	  +  u8  方案索引
                                             */ 
                                                
    h_ctrl_innerMatrixSetCurSchemeInd,      /**************<< 调用指定方案为当前方案指示 >>****************	
                                             *[消息体]			 
                                             *	  +  u8  方案索引
                                             */ 
                                                
    h_ctrl_innerMatrixCurSchemeCmd,         /********************<< 获取当前方案请求  >>*******************	
                                             *[消息体]			 
                                             *  无  
                                             */                                                 
                                                
    h_ctrl_innerMatrixCurSchemeInd,         /*****************<< 获取当前方案请求指示  >>******************	
                                             *[消息体]			 
                                             *    +  u8  方案索引
                                             */
    //扩展视频源
  
	h_ctrl_getVideoSourceInfoCmd,           /*******************<< 获取终端视频源信息  >>******************	
                                             *[消息体]			 
                                             *    NULL
                                             */   

    h_ctrl_allVideoSourceInfoInd,           /******************<< 终端全部视频源指示  >>*******************	
                                             *[消息体]			 
                                             *    +  tagITVideoSourceInfo[]
                                             */   
												 
    h_ctrl_setVideoSourceInfoCmd,           /*******************<< 设置终端视频源信息  >>******************	
                                             *[消息体]			 
                                             *    +  tagITVideoSourceInfo
                                             */   

    h_ctrl_VideoSourceInfoInd,              /******************<< 终端视频源信息指示  >>*******************	
                                             *[消息体]			 
                                             *    +  tagITVideoSourceInfo
                                             */
			
    h_ctrl_selectExVideoSourceCmd,          /********************<< 扩展视频源选择  >>*********************	
                                             *[消息体]			 
                                             *    +  u8     EX_VIDEOSOURCE_BASE 基址 
                                             *              即EX_VIDEOSOURCE_BASE代表扩展视频源的1号端口
                                             */
	
    h_ctrl_ViewBroadcastingSrcCmd,          /*主席终端强制广播, 消息体: u8 (opr_start-开始，opr_stop－取消)*/
    
    h_ctrl_SatDConfAddrInd,                 /*******************<< 卫星分散会议接收地址通知 >>*************
                                             * Mcu-->ter
                                             *[消息体](均网络序)
                                             *    +   u32   第一路视频地址
                                             *    +   u16   第一路视频端口
                                             *    +   u32   第一路音频地址
                                             *    +   u16   第一路音频端口
                                             *    +   u32   第二路视频地址
                                             *    +   u16   第二路视频端口
                                             */

	 h_ctrl_videoFormatChangeCmd,            /*********************<< 视频分辨率调整  >>********************
											 * Mcu-->ter
											 *[消息体]
											 *
											 *    +   u8  通道类型   LOGCHL_VIDEO      (默认主视频通道)
											 *    +   u8  分辨率类型 VIDEO_FORMAT_CIF, VIDEO_FORMAT_QCIF, VIDEO_FORMAT_SQCIF_* 等
											 */

    

    h_ctrl_setAudioVolumeCmd,               /*********************<<  终端音量调节   >>********************
                                             * Mcu-->ter
                                             *[消息体]
                                             *
                                             *    +   u8  音量类型(NOTE: VOLUME_TYPE_OUT or VOLUME_TYPE_IN)
                                             *    +   u8  调整后的音量(NOTE: vx linux 0～32; PCMT: 0～255)
                                             */
    
    h_ctrl_someOfferingNtf,                 /********************<<  终端通知通用消息 >>*******************
                                             * Mcu-->ter
                                             *[消息体]
                                             *
                                             *    Mcu2MtNtfMsgBodyType
                                             */ 
                                             
    h_ctrl_videoFrameRateChangeCmd,         /********************<<  (第一路)视频编码帧率调整 >>***********
                                             * Mc-->ter
                                             *[消息体]
                                             *
                                             *    +   u8  编码帧率
                                             */

	h_ctrl_removeMixMemberCmd,				/********************<<  移除定制混音中的终端 (由主席触发有效) >>*******************
											 * Mt-->Mc
											 *[消息体]
											 *
											 *    +   TMt 数组
											 * 
                                             */
	h_ctrl_discussParamInd,					/*******************<< 混音信息通知>>***************************
											 * Mt-->Mc
											 * [消息体]
											 * + TDiscussParam
											 *
											 */
	h_ctrl_videoFormatRecoverCmd,			/******************<<  终端分辨率恢复命令   >>****not in use now**********
											 * ter-->Mt
											 * [消息体]  无
											 */
	h_ctrl_mtSelectVmpReq,					/*******************<< 终端（仅限主席MT）选看画面合成请求 >>**************
											 * ter --> MC
											 * [消息体] u8	1表start,0表stop
											 */
	h_ctrl_mtSelectVmpRsp,					/*******************<< 终端（仅限主席MT）选看画面合成应答 >>**************
											 * MC --> ter
											 * [消息体] u8  ok/fail(1表ok，0表fail)
											 */	
    h_ctrl_mtSpeakStatusNtf,                /*******************<< 终端 申请发言中的终端              >>**************
											 * MC --> ter
											 * [消息体] emSpeakerStatus
                                             *
                                                             emWaiting       = 1,        //等待同意；mcu不会发送此状态，终端在发起申请 到 成为发言人通知 到 之前，自动跳入该状态
                                                             emAgreed        = 2,        //发言人；
                                                             emDenid         = 3,        //申请被拒绝；
                                                             enCanceled      = 4,        //被取消发言；
                                             */	

    h_ctrl_confSpeakModeNtf,                 /*******************<< 终端 所有与会终端                 >>**************
											 * MC --> ter
											 * [消息体] emConfSpeakMode
                                                            emNormal            = 0,        //普通发言模式：主席指定/会控指定/一般的终端申请
                                                            emAnswerInstantly   = 1         //抢答模式
                                             */	

    h_ctrl_applycancelspeakerReq,               /*******************<< 发言人终端 －> MCU                 >>**************
											 * ter --> MC
											 * [消息体] u8:0-取消发言 1-申请发言
                                             */	

    h_ctrl_applycancelspeakerRsp,               /*******************<< MCU －>  发言人终端                >>**************
											 * MC --> ter
											 * [消息体] u8:0-取消发言 1-申请发言
														u8 result: granted/denied
                                             */	

	h_ctrl_videoFpsChangeCmd,               /*********************<< 视频帧率调整  >>********************
											 * Mcu-->ter
											 *[消息体]
										 	 *
										 	 *    +   u8  通道类型   LOGCHL_VIDEO      (默认主视频通道)
											 *    +   u8  帧率值 
											 */
	h_ctrl_videoAliasCmd,               /*********************<< mcu获取终端视频源别名  >>********************
										 * Mcu-->ter
										 *[消息体] 空
										 *
										 *    
										 *    
										 */
	h_ctrl_videoAliasInd,               /*********************<< 终端视频源别名指示  >>********************
										* ter-->Mcu
										*[消息体]
										*  u8 byVidNum（视频源数量）+ u8 byVidIdx（视频源端口索引）+   
										*   u8 byAliaseLen（别名长度）+ s8 *pchAlias（别名）+ 
										*  u8 byVidIdx（视频源端口索引）+ u8 byAliaseLen（别名长度）+  
										*   s8 *pchAlias（别名）+ ...
										*    
										*    
										*/
	h_ctrl_ChgSecVidSendCmd			= 101 + 134,/*********************<< 控制终端双流发送命令 >>********************
												* Mcu-->ter
												*[消息体] 
												*u8 0—停止双流  1—发起双流
												*    
												*    
												*/
//[20120613]
	h_ctrl_transparentMsgNtf,           /*********************<< 界面、终端透传消息  >>********************
										* ter<-->Mcu
										*[消息体]
										*     根据消息类型不同，消息体内容不同
										*      
										*/  
										
	h_ctrl_CodingFormNtf,                /*********************<< 在h245建立后马上向对端发送该非标消息通告对端本端的编码方式  >>********************
										 * ter<-->Mcu
										 *[消息体]
									     * u8(编码方式)
										 *      
										 */
    h_ctrl_nonstandardVrsDef,           /***************<<非标自定义》******************
                                         * ter<-->Mcu
                                         *[消息体]
                                         *     根据消息类型不同，消息体内容不同
                                         *     上层两端业务协商消息的子类型和消息内容
                                         *
						                 */

	h_ctrl_mcuvccommonnonstandend   = 299,


	//自定义消息开始
	h_ctrl_nonstandstart			= 300,	//
	
	h_ctrl_quietRequest				= 301,  //远端静音请求,				[outgoing]: [TTERLABEL]
	h_ctrl_quietResponse			= 302,  //远端静音响应				[incoming]: [TTERLABEL+u8:CtrlResult]

	h_ctrl_cancelQuietRequest		= 303,  //取消远端静音请求			[outgoing]: [TTERLABEL]
	h_ctrl_cancelQuietResponse		= 304,  //取消远端静音响应			[incoming]: [TTERLABEL+u8:CtrlResult]

	h_ctrl_muteRequest				= 305,  //远端闭音请求,				[outgoing]: [TTERLABEL]
	h_ctrl_muteResponse 			= 306,  //远端闭音响应				[incoming]: [TTERLABEL+u8:CtrlResult]

	h_ctrl_cancelMuteRequest		= 307,  //取消远端闭音请求			[outgoing]: [TTERLABEL]
	h_ctrl_cancelMuteResponse		= 308,  //取消远端闭音响应			[incoming]: [TTERLABEL+u8:CtrlResult]

	h_ctrl_setVACReqequest			= 309,  //设置声控切换(语音激励)	[outgoing]: [u8+DW] 0：设置声控切换1：取消声控切换+1 DW 声控门限(0~100之间的一个值),可以没有声控门限，这时使用缺省的声控门限
	h_ctrl_setVACResponse			= 310,  //设置声控切换结果			[incoming]: [u8:CtrlResult](返回是对设置操作的结果，不是表明当前声控切换的状态。例如，取消声控切换的结果为成功，表示取消声控切换的设置成功。)

	h_ctrl_startDiscussRequest		= 311,  //开始自由讨论				[outgoing]: NULL
	h_ctrl_startDiscussResponse		= 312,  //自由讨论结果				[incoming]: [u8:CtrlResult]终端收到这条结果,所有会场开音,全部取消静音
	
	h_ctrl_setMultiPicRequest		= 313,  //设置多画面				[outgoing]: TMultiPic
	h_ctrl_setMultiPicResponse		= 314,  //

	h_ctrl_notInConfTerminalListRequest = 315,//请求所有未入会会场名	[outgoing]: NULL
	h_ctrl_notInConfTerminalListResponse= 316,//所有未入会会场名应答	[incoming]: [TTERINFO[]]

	h_ctrl_callTerminal				= 317,  //呼叫离线会场				[outgoing]: [TTERLABEL]
	h_ctrl_disconnectTerminal		= 318,  //挂断终端					[outgoing]: [TTERLABEL]

	h_ctrl_selectSpeakingUserRequest= 319,	//点名发言					[outgoing]: [TTERLABEL]
	h_ctrl_selectSpeakingUserResponse=320,  //点名发言结果				[incoming]: [TTERLABEL+u8:CtrlResult]
	
	h_ctrl_setPicSwitch				 =321,	//单,多画面切换				[outgoing]: [u8] 0:单画面;1:多画面
    
    h_ctrl_capabilitiesInd           =322,  //对通能力指示              [incoming]: [TCAPINFO]

    h_ctrl_serialPortData            =323,  //串口数据                  [incoming]&[outgoing]: [TCOMDATA]


	h_ctrl_setChanMultiCastAddr		 =400,  //设置通道组播地址			[incoming]&[outgoing]: [TCHANMULTICAST]

	h_ctrl_serialNumber      		 =401,  //序列号    				[incoming]&[outgoing]: [u8[]] 
    h_ctrl_startDataConfRequest      =402,  //请求开始数据会议          NULL

    h_ctrl_startDataConfResponse     =403,  //应答开始数据会议          [incoming]&[outgoing]: u8: result
	h_ctrl_askVcsOccupyPermit	     =500,	//
	
	h_ctrl_askVcsOccupyPermitResp    =501,  //

	h_ctrl_VersionIdInd              =600, // 消息体为u8[LEN_256]
	
	h_ctrl_KdvpExtraConfNSNInfo      =601, // 超过192个终端列表用这个非标 [incoming]&[outgoing]

    h_ctrl_end
};

//非标参数类型
enum NonstandardType 
{
    emNonstandardTypeInvalidType = 0,
    emNonstandardTypeRequest,
    emNonstandardTypeResponse,
    emNonstandardTypeCommand,
    emNonstandardTypeIndication
};

//add by yj for QUERY [20121212]
//当前平台所支持的列表查询能力
enum emPlatformCap
{
	KdvPlatformCapStart	= 0,
	KdvPlatformCapTerLIst,					//在线终端列表
	KdvPlatformCapConfList,					//会议列表
	KdvPlatformCapCreateConfwithTerlist,	//召集会商能力
	KdvPlatformCapJoinCofByConfList,		//参加会商能力
	KdvPlatformCapGlobalAddrBook,			//全局地址簿
	KdvPlatformCapVideoRecordBroadcast,		//录播能力
	KdvPlatformCapEnd,
};

typedef struct tagPlatformCap
{
	u32 m_dwCap;
	
	void Clear(){  m_dwCap = 0; } 

	BOOL SetAllCap( u32 dwCap )
	{
		if ( dwCap <= (0x01<< KdvPlatformCapStart)  || dwCap >= (0x01<< KdvPlatformCapEnd) )
			return FALSE;
		m_dwCap = htonl(dwCap);
		return TRUE;
	}
	
	u32 GetAllCap()
	{
		return ntohl( m_dwCap );
	}

	BOOL AddCap( emPlatformCap emCap )
	{	
		if ( emCap <= KdvPlatformCapStart  || emCap >= KdvPlatformCapEnd )
			return FALSE;
		m_dwCap = ntohl(m_dwCap);
		m_dwCap = ( m_dwCap | (0x01<< emCap ) );
		m_dwCap = htonl(m_dwCap);
		return TRUE;
	}

	BOOL RemoveCap( emPlatformCap emCap )
	{
		if ( emCap <= KdvPlatformCapStart  || emCap >= KdvPlatformCapEnd )
			return FALSE;
		m_dwCap = ntohl(m_dwCap);
		m_dwCap = ( m_dwCap & ~(0x01<< emCap ) );
		m_dwCap = htonl(m_dwCap);
		
		return TRUE;
	}

	BOOL IsSupport(emPlatformCap emCap )
	{
		if ( emCap <= KdvPlatformCapStart  || emCap >= KdvPlatformCapEnd )
			return FALSE;
		m_dwCap = ntohl(m_dwCap);
        if (((s32)m_dwCap & (0x01<<emCap)) ==  (0x01<<emCap))
		{
			m_dwCap = htonl(m_dwCap);
			return TRUE;
		}
		else
		{
			m_dwCap = htonl(m_dwCap);
			return FALSE;
		}
	}
}TPlatformCap;
//end [20121212]

///////////////////////////////// 返回值定义 ///////////////////////////////

//API函数执行结果
enum ActResult 
{
    act_err = -1,
	act_ok
};

//add by yj for callmake error return [20120618]
enum CallMAkeErrReason 
{
    err_unknow				= -1,	//未知原因
	err_full_call			= -2,	//呼叫已满
	err_set_param			= -3,	//适配层设置参数出错(包含地址)
	err_null_ptr			= -4,	//指针为NULL或指向的buffer大小不匹配	
	err_set_address			= -5,	//呼叫时协议栈内部判断地址设置有误	
	err_already_setup		= -6,	//已经呼叫过	
	err_autoRas_PrepareARQ	= -7,	//发送ARQ失败
	err_autoRas_Register	= -8,   //注册失败
	err_Q931_Connect		= -9,	//Q931呼叫失败
	err_iptype_notmatch		= -10,	//呼叫源地址和目的地址类型不一致(多宿IP下)
};
//end [20120618]

enum callBackResult 
{
    cb_err = -1,
    cb_ok
};
///////////////////////////////// 常量定义 ///////////////////////////////

// IP地址类型
enum TransportAddrType 
{
	addr_uca,		//unicast
	addr_mca,		//multicast
    addr_err = 0xff
};

// 呼叫地址类型
enum CallAddrType
{
    cat_err = -1,
	cat_ip  = 1, 	//IP呼叫
	cat_alias	   	//别名呼叫
};

#ifndef CM_H
typedef enum
{
	cmCreate,
	cmJoin,
	cmInvite,
	cmCapabilityNegotiation,
	cmCallIndependentSupplementaryService,
	cmLastCG
}cmConferenceGoalType;

typedef enum
{
    cmCallTypeP2P,
	cmCallTypeOne2N,
	cmCallTypeN2One,
	cmCallTypeN2Nw
}cmCallType;

typedef enum
{
    cmEndpointTypeTerminal,
	cmEndpointTypeGateway,
	cmEndpointTypeMCU,
	cmEndpointTypeGK,
	cmEndpointTypeUndefined,
	cmEndpointTypeSET
}cmEndpointType;

typedef enum
{
    cmRASDisengageReasonForcedDrop,
	cmRASDisengageReasonNormalDrop,
	cmRASDisengageReasonUndefinedReason
}cmRASDisengageReason;

typedef enum
{
    cmRASUnregReasonReregistrationRequired,
	cmRASUnregReasonTtlExpired,
	cmRASUnregReasonSecurityDenial,
	cmRASUnregReasonUndefinedReason,
	cmRASUnregReasonMaintenance
}cmRASUnregReason;

typedef enum
{
    cmRASReasonResourceUnavailable,             /* GRJ, RRJ, ARJ, LRJ - gatekeeper resources exhausted */
    cmRASReasonInsufficientResources,           /* BRJ */
    cmRASReasonInvalidRevision,                 /* GRJ, RRJ, BRJ */
    cmRASReasonInvalidCallSignalAddress,        /* RRJ */
    cmRASReasonInvalidRASAddress,               /* RRJ - supplied address is invalid */
    cmRASReasonInvalidTerminalType,             /* RRJ */
    cmRASReasonInvalidPermission,               /* ARJ - permission has expired */
                                                /* BRJ - true permission violation */
                                                /* LRJ - exclusion by administrator or feature */
    cmRASReasonInvalidConferenceID,             /* BRJ - possible revision */
    cmRASReasonInvalidEndpointID,               /* ARJ */
    cmRASReasonCallerNotRegistered,             /* ARJ */
    cmRASReasonCalledPartyNotRegistered,        /* ARJ - can't translate address */
    cmRASReasonDiscoveryRequired,               /* RRJ - registration permission has aged */
    cmRASReasonDuplicateAlias,                  /* RRJ - alias registered to another endpoint */
    cmRASReasonTransportNotSupported,           /* RRJ - one or more of the transports */
    cmRASReasonCallInProgress,                  /* URJ */
    cmRASReasonRouteCallToGatekeeper,           /* ARJ */
    cmRASReasonRequestToDropOther,              /* DRJ - can't request drop for others */
    cmRASReasonNotRegistered,                   /* DRJ, LRJ, INAK - not registered with gatekeeper */
    cmRASReasonUndefined,                       /* GRJ, RRJ, URJ, ARJ, BRJ, LRJ, INAK */
    cmRASReasonTerminalExcluded,                /* GRJ - permission failure, not a resource failure */
    cmRASReasonNotBound,                        /* BRJ - discovery permission has aged */
    cmRASReasonNotCurrentlyRegistered,          /* URJ */
    cmRASReasonRequestDenied,                   /* ARJ - no bandwidrg available */
                                                /* LRJ - cannot find location */
    cmRASReasonLocationNotFound,                /* LRJ - cannot find location */
    cmRASReasonSecurityDenial,                  /* GRJ, RRJ, URJ, ARJ, BRJ, LRJ, DRJ, INAK */
    cmRASReasonTransportQOSNotSupported,        /* RRJ */
    cmRASResourceUnavailable,                   /* Same as cmRASReasonResourceUnavailable */
    cmRASReasonInvalidAlias,                    /* RRJ - alias not consistent with gatekeeper rules */
    cmRASReasonPermissionDenied,                /* URJ - requesting user not allowed to unregister specified user */
    cmRASReasonQOSControlNotSupported,          /* ARJ */
    cmRASReasonIncompleteAddress,               /* ARJ, LRJ */
    cmRASReasonFullRegistrationRequired,        /* RRJ - registration permission has expired */
    cmRASReasonRouteCallToSCN,                  /* ARJ, LRJ */
    cmRASReasonAliasesInconsistent,             /* ARJ, LRJ - multiple aliases in request identify distinct people */
    cmRASReasonAdditiveRegistrationNotSupported,/* RRJ */
    cmRASReasonInvalidTerminalAliases,          /* RRJ */
    cmRASReasonExceedsCallCapacity,             /* ARJ - destination does not have the capacity for this call */
    cmRASReasonCollectDestination,              /* ARJ */
    cmRASReasonCollectPIN,                      /* ARJ */
    cmRASReasonGenericData,                     /* GRJ, RRJ, ARJ, LRJ */
    cmRASReasonNeededFeatureNotSupported,       /* GRJ, RRJ, ARJ, LRJ */
    cmRASReasonUnknownMessageResponse,          /* XRS message was received for the request */
    cmRASReasonHopCountExceeded                 /* LRJ */
}cmRASReason;

typedef struct
{
    int timestamp;      /* -1 if optional; */
    int expirationTime; /* -1 if optional;*/
    int logicalChannelNumber;
}cmUserInputSignalRtpStruct;

typedef struct
{
    char signalType;
    int duration;
    cmUserInputSignalRtpStruct cmUserInputSignalRtp;/* if logicalchannel == 0 -optional*/
}cmUserInputSignalStruct;
#endif//CM_H没有包含cm.h

#ifndef CMCTRL_H
typedef enum 
{
    cmCapReceive=1,
    cmCapTransmit=2,
    cmCapReceiveAndTransmit=3
} cmCapDirection;
#endif//CMCTRL_H 没有包含cm.h

//呼叫断开原因
enum CallDisconnectReason 
{
	reason_busy = 1,
	reason_normal,
	reason_rejected,
	reason_unreachable,
	reason_local,
	reason_unknown,

	reason_nobandwidth,                  //ARQ拒绝或没有带宽
	reason_nopermission,                 //被叫网守拒绝
	reason_unreachablegatekeeper,         //网守不可达for arq
	reason_mcuoccupy,
	reason_reconnect,
	reason_confholding,
	reason_hascascaded                 
};

//Facility Reason
enum FacilityReason 
{
	routeToGK,				
	callForward,				
	routeToMC,
	confListChoice,
	startH245,
	unknown
};

//主从决定结果
enum MasterSlaveResult 
{
	msdse_master,		//本端为master
	msdse_slave,		//本端为slave
    msdse_err = 0xff    
};

//能力交换结果
enum CapExchangeResult 
{
	cese_accept,		//对端接受本端能力
	cese_reject,		//对端拒绝本端能力
    cese_err = 0xff
};

//会议操作:makeTerminalBroadcaster和sendThisSource的操作方式
enum CtrlMode 
{
	mode_invalid = -1,
	mode_command,
	mode_request
};

//会议操作:请求-应答方式返回结果
enum CtrlResult 
{
	ctrl_err = -1,
	ctrl_granted,		//成功
	ctrl_denied			//拒绝
};

//别名类型
enum AliasType
{
	type_aliastype,
	type_e164 = 0x1,
	type_h323ID,
	type_email,
	type_epID,
	type_GKID,
	type_partyNumber,
	type_others
};

//数据类型
enum  DataType 
{
	type_none = 0,
	type_audio,
	type_video,
	type_data,
};

#ifndef MCUCASC_ADAPTER_COMMON
#define MCUCASC_ADAPTER_COMMON

//媒体类型
//guzh [12/14/2007] 整理、范围确定
//guzh [03/27/2008] 由于历史原因，a_typeEnd、v_typeEnd等范围不准确，请勿在外面使用
//请直接使用 IsAudioType, IsAudioType, IsDataType等接口
enum  PayloadType 
{
    emPayloadTypeBegin = 0,
        //音频格式
        a_typeBegin,
        a_g711a,
        a_g711u,
        a_g722,
        a_g7231,
        a_g728,
        a_g729,
        a_mp3,
        
        a_g723,
        a_g7221,//代替a_g7221c
        
        // guzh
        a_mpegaaclc = 11,
        a_mpegaacld = 12,
		a_g719,//ruiyigen 20091012
        
        // 保持新老版本兼容
        v_mpeg4 = 17,    
        v_h262 = 18,
        
        a_typeGeneric = 48,   // 内部使用
        
        a_typeEnd = 49,
        
        //视频格式
        v_typeBegin = 50,	
        v_h261,
        v_h263,
        v_h263plus,
        v_h264,
        //v_mpeg4,
        //v_h262,
        v_generic = 97,
        v_extended = 98,
        v_typeEnd = 99,
        
        //数据格式
        d_typeBegin = 100,	
        d_t120,
        d_h224,
        d_typeEnd = 149,
        
        t_h235 = 150, //内部使用
        t_mmcu,       //级联通道
        t_none,       //此通道不支持, 仅用于级联
        t_nonStandard,
        t_huawei,

		//add for huawei nostandrd [20130427]
		t_huawei_5,
		t_huawei_6,
		t_huawei_7,
		//end [20130427]

		//add for ZTE nostandrd [20130507]
		t_ZTE_8,	//audio
		t_ZTE_10,	//audio
		t_ZTE_11,	//audio
		t_ZTE_13,	//video 且未设入同时能力中
		t_ZTE_21,	//video
        //end [[20130507]
    emPayloadTypeEnd = 255
};

#define PayloardIsAudioType( paylord )  \
( (a_typeBegin < paylord && paylord < a_typeEnd && paylord != v_mpeg4 && paylord != v_h262) )

#define PayloardIsVideoType( paylord )  \
( ( (v_typeBegin < paylord && paylord < v_typeEnd) || paylord == v_mpeg4 || paylord == v_h262) )

#define PayloardIsDataType( paylord )  \
( (d_typeBegin < paylord && paylord < d_typeEnd) )



//分辨率
typedef enum
{
	emResolutionBegin,
	    emSQCIF,
	    emQCIF,
	    emCIF,
	    emCIF2,
	    emCIF4,    // 576SD: stands for D1: 720*576
	    emCIF16,
		    
	    em352240,
	    em704480,  // 480SD

	    em640480,
	    em800600,
	    em1024768,

	    emAUTO,
    emResolutionEnd
}emResolution;

// High Definition Resolutions(1080i is i-mode, others are p-mode)
typedef enum
{    
    emW4CIF = 14,   // Wide4CIF 16:9 1024*576
    emHD720,        // 720   1280*720
    emSXGA,         // SXGA  1280*1024
    emUXGA,         // UXGA  1600*1200
    emHD1080,       // 1080  1920*1080	
	
	em1280x800,
	em1280x768
}emResolutionHD;
#endif

//呼叫相关的参数
enum CallParamType
{
	cpt_ConferenceGoalType,					//会议的目标类型 cmConferenceGoalType
	cpt_CallType,							//呼叫类型       cmCallType
	cpt_callId,								//呼叫ID		 char[16]
	cpt_callCrv,							//呼叫CRV		 int
	cpt_confId								//会议ID		 char[16]
};

//终端类型
typedef enum
{
	emEndpointTypeUnknown = 0,
	emEndpointTypeMT	  = 0x01,
	emEndpointTypeMCU	  = 0x02,
	emEndpointTypeGK      = 0x04
}emEndpointType;

//终端类型值
typedef enum
{
	emMsMT			= 50,
	emMsMT_MC		= 70,
	emMsMCU			= 190,
	emMsActiveMC	= 240,
}emMsTerminalType;

//设备厂商标识信息
enum VendorProductId
{
	vendor_MICROSOFT_NETMEETING = 1,
	vendor_KEDA,
	vendor_HUAWEI,
	vendor_POLYCOM,
    vendor_POLYCOM_MCU,
	vendor_TANDBERG,
	vendor_RADVISION,
	vendor_VCON,
	vendor_SONY,
	vendor_VTEL,
	vendor_ZTE,
	vendor_ZTE_MCU,
    vendor_CODIAN,
	vendor_AETHRA,
	//[20120613]
	vendor_CHAORAN,
	//end
	vendor_UNKNOWN,
	vendor_END
};

/*载荷类型的内部转换:
|<--------------加密和H239的载荷转换---------- >|<低字节为上层使用的类型>|
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+  0  +  0  +  0  + 0  +  1  +  1  +  1  +  1  +  payloadType           +             
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                       + H239+ AES + DES +NoEnc+   
*/
//加密类型
typedef enum
{
    emEncryptTypeNone = 1,//00000001
    emEncryptTypeDES  = 2,//00000010
    emEncryptTypeAES  = 4,//00000100

    emEncryptTypeH239 = 8,//00001000

}emEncryptType;

//H323 Annex Q
/*   
 *   h.281 encoded in host bit order:
 *   +-------+---------+--------+---------+--------+---------+--------+---------+
 *   +   P   | R=1/L=0 |    T   | U=1/D=0 |    Z   | I=1/O=0 |    F   | I=1/O=0 |
 *   +-------+---------+--------+---------+--------+---------+--------+---------+
 */
enum feccAction
{
	actionInvalid				= 0,  
	actionStart					= 0x01,  //开始
	actionContinue				= 0x02,  //继续
	actionStop					= 0x03,  //停止
	actionSelectVideoSource		= 0x04,  //选择视频源
	actionVideoSourceSwitched	= 0x05,  //切换视频源
	actionStoreAsPreset			= 0x06,  //存储预置位
	actionActivatePreset		= 0x07,  //恢复到预置位
	actionAutoFocus				= 0x11,  //自动调焦
};

//PTFZ codes, parameters for action Start, continue and stop.
enum feccPTFZRequest
{
	requestInvalid				= 0,
	requestPanLeft				= 0x80,	//向左移动
	requestPanRight				= 0xc0,	//向右移动
	requestTiltUp				= 0x30,	//向上移动
	requestTiltDown				= 0x20,	//向下移动
	requestFocusIn				= 0x03,	//近焦
	requestFocusOut				= 0x02,	//远焦
	
	requestZoomIn				= 0x0c,	//视野变小
	requestZoomOut				= 0x08,	//视野变大

	requestBrightnessUp			= 0x01,	//亮度加(nonstandard)
	requestBrightnessDown		= 0x04  //亮度减(nonstandard)
};

typedef enum
{
	annexQNotInited				= -20,
	annexQBindError				= -15,
	annexQParamError			= -14,
	annexQLockError				= -12,	
	annexQCodingError			= -10,
	annexQSendError				= -9,	
	annexQGeneralError			= -5,
	annexQOk					= 1
}annexQResult;

//for tanderg
enum feccVideoSourceNumber
{
    VideoSourceDefault               = 0,    //当前视频源
    VideoSourceMainCamera            = 1,    //主视频源
    VideoSourceAuxCamera             = 2,    //辅助视频源
    VideoSourceDocCamera             = 3,    //文件展示台
    VideoSourceAuxDocCamera          = 4,    //辅助文件展示台，对于泰德为vcr
    VideoSourceVideoPlaybackSource   = 5,    //vcr，对于泰德为PC
};

//H239
typedef enum
{
    emH239InfoTypeInvalid		        = 0,

    emH239FlowControlReleaseRequest     = 1,
    emH239FlowControlReleaseResponse    = 2,
    emH239PresentationTokenRequest      = 3,
    emH239PresentationTokenResponse     = 4,
    emH239PresentationTokenRelease      = 5,
    emH239PresentationTokenIndicateOwner= 6,

    emH239InfoTypeEnd
}emH239InfoType;

typedef enum
{
	emStyle2p  = 0x07,
	emStyle3p  = 0x08,
	emStyle4p  = 0x00,
	emStyle6p  = 0x02,
	emStyle8p  = 0x04,
	emStyle9p  = 0x01,
	emStyle13p = 0x06,
	emStyle13  = 0x05,
	emStyle16p = 0x03,
	emStyleEnd
}emPicStyle;

typedef enum
{
	emChanUnkown = 0,
	emChanVideo1 = 1,
	emChanAudio	 = 2,
	emChanVideo2 = 3
}emChanID;

typedef enum
{
    emComUnkown  = 0,
    emComRS232   = 1,
    emComRS422   = 2,
    emComRS485   = 3,
    emComEnd
    
}emComType;

// guzh [9/25/2007] 前向纠错类型
typedef enum
{
    emFECNone   = 0x00,
    emFECRaid5  = 0x01,
    emFECRaid6  = 0x02,
	emFECNonStd    = 0x04 //公司自己的NsmFec add by shigubin 20130228
}emFECType;

typedef enum
{
    emNALNode            =  0x00,       // invalid
    emNALSignal          =  0x01,       // single
	emNALNoneInterleaved =  0x02,       // not interleaved
	emNALInterleaved     =  0x04        // interleaved
}emH264NALMode;


typedef enum
{
    emLogChanRejectUnspecified,
    emLogChanRejectUnsuitableReverseParameters,
    emLogChanRejectDataTypeNotSupported,
    emLogChanRejectDataTypeNotAvailable,
    emLogChanRejectUnknownDataType,
    emLogChanRejectDataTypeALCombinationNotSupported,
    emLogChanRejectMulticastChannelNotAllowed,
    emLogChanRejectInsuffientBandwdith,
    emLogChanRejectSeparateStackEstablishmentFailed,
    emLogChanRejectInvalidSessionID,
    emLogChanRejectMasterSlaveConflict,
    emLogChanRejectWaitForCommunicationMode,
    emLogChanRejectInvalidDependentChannel,
    emLogChanRejectReplacementForRejected
}emlogChannelRejectCause;

//支持媒体穿越
typedef enum
{
	emNotSupportNATFWTraversal,			//不支持460穿越
	emSupportNATFWTraversal,			//支持460
	emSupportTransmitMulti,			    //支持端口复用
	emSupportMediaTraversal				//本端为460服务器
}emNATFWTraversal;

// 开关常量
enum AdpSwitchConstant
{
	Adp_turn_on,		//开启
	Adp_turn_off		//关闭
};

// 轮询类型
enum AdpPollType
{
	Adp_poll_none,		//没有轮询
	Adp_poll_vcb,		//轮流广播
	Adp_poll_vcs		//轮流选看
};

enum AdpPollStatus
{
	Adp_poll_status_none,    //未轮询
	Adp_poll_status_normal,  //轮询进行中
	Adp_poll_status_pause    //轮询暂停
};

//操作类型宏定义
enum AdpOprType
{
    Adp_opr_start,
	Adp_opr_stop
};

//MCU发向终端的自定义通知消息 消息体号定义
enum AdpMcu2MtNtfMsgType
{
    emAdpNoneNtf           = 0,
	emAdpBePolledNextNtf   = 1           //即将被轮询到 通知
};

///////////////////////////////// 参数结构 ////////////////////////////////
//userInput结构
typedef struct tagUserInputInfo 
{
    s8  m_chSignalType;     //FROM ("0123456789#*ABCD!")
    s32 m_nDuration;        //超时值，单位：milliseconds

    tagUserInputInfo()
    {
        m_chSignalType = '\0';
        m_nDuration = 300;
    }

    void SetSignalType(s8 signalType)
    {
        m_chSignalType = signalType;
    }

    s8 GetSignalType()
    {
        return m_chSignalType;
    }

    void SetDuration(s32 duration)
    {
        m_nDuration = duration;
    }

    u16 GetDuration()
    {
        return m_nDuration;
    }
}TUserInputInfo,*PTUserInputInfo;
typedef struct tagH460Features
{
	BOOL m_bPeerSupportH460;
	BOOL m_bPeerMediaTraversalServer;
	BOOL m_bPeerSupportTransMultiMedia;
	tagH460Features()
	{
		m_bPeerSupportH460 = FALSE;
		m_bPeerMediaTraversalServer = FALSE;
		m_bPeerSupportTransMultiMedia = FALSE;
	}
}TH460Features,*PTH460Features;


//别名地址
typedef struct tagALIASADDR
{
	AliasType  m_emType;
	s8		   m_achAlias[LEN_ALIAS+2];			//h323-Id 别名后两位为空

	tagALIASADDR()
	{
		Clear();
	}

	void Clear()
	{
		m_emType = type_aliastype;
		memset( m_achAlias, 0, sizeof(m_achAlias) );
	}

	//设置别名
	BOOL SetAliasAddr( AliasType aliasType, const char* pAlias )
	{
		if ( aliasType <= type_aliastype || aliasType >= type_others ||
			 pAlias == NULL ) 
			return FALSE;
		
		memset( m_achAlias, 0, sizeof(m_achAlias) );

		m_emType = aliasType;

		u16 nLen = min( strlen(pAlias), LEN_ALIAS );
		strncpy( m_achAlias, pAlias, nLen );
		m_achAlias[nLen] = '\0';

		return TRUE;
	}	
	//获得type
	AliasType GetAliasType()
	{
		return m_emType;
	}
	//获得别名
	s8* GetAliasName()  
	{
		return m_achAlias;	
	}
}TALIASADDR,*PTALIASADDR;

//关键配置参数定义
typedef struct tagConfigs
{
	u32				m_dwSystem_maxCalls;				    //最大呼叫数
	u32				m_dwSystem_maxChannels;					//每个呼叫打开的逻辑通道数
	
	emEndpointType  m_emTerminalType;						//终端类型如(emMt emMcu)
	BOOL			m_bRas_manualRAS;						//是否手动RAS操作
	BOOL			m_bRas_manualRegistration;				//是否手动RAS注册
	s8				m_achRas_defaultGkIp[4];				//缺省gk地址
	u16				m_wRas_defaultGkPort;					//缺省gk呼叫端口
	u16				m_wRas_defaultlocalPort;				//缺省本地RAS端口
	s8				m_ach_UserName[REG_NAME_LEN];			//注册帐号(E164)
	s8				m_ach_Password[REG_PWD_LEN];			//注册密钥
	
	s32				m_nProductIdSize;
	u8				m_abyProductId[LEN_256];				//产品号
	s32				m_nVerIdSize;
	u8				m_abyVersionId[LEN_256];				//版本号
	TALIASADDR		m_atAlias[type_others];					//别名数组
		
	u16				m_wQ931_CallingPort;					//h225呼叫端口
	emMsTerminalType  m_emMsTerminalType;					//主从决定终端类型   
	BOOL			m_bH245_masterSlaveManualOperation;		//是否手工进行主从确定
	BOOL			m_bH245_capabilitiesManualOperation;	//是否进行手工能力交换

    s32             m_nPortFrom;                            //端口范围
    s32             m_nPortTo;                              //
	
	u32				m_dwStackIp;							//协议栈运行IP，网络序;

	//add by yj for ipv6
	u8          m_byIPType;                                 // 支持的 IP地址类型
	u8          m_dwStackIp_ipv6[IPV6_NAME_LEN];			//协议栈运行IP(ipv6), 非网络序;
	u8			m_achRas_defaultGkIp_ipv6[IPV6_NAME_LEN];	//缺省gk地址(IPv6), 非网络序
//	u8          m_dwStackIp_ipv6[16];			            //协议栈运行IP(ipv6)，网络序;
	s16         m_swScopeId;                                 //获取或设置 IPv6 地址范围标识符

	//end
	
	tagConfigs()
	{
		Clear();
	}

	void Clear()
	{
		m_dwSystem_maxCalls = MAX_CALL_NUM;
		m_dwSystem_maxChannels = MIN_CHANNEL_NUM;
		m_bRas_manualRAS = TRUE;
		m_bRas_manualRegistration = TRUE;
		m_wRas_defaultGkPort = RASPORT;
		m_wRas_defaultlocalPort = RASPORT;
		m_wQ931_CallingPort = CALLPORT;
		m_bH245_masterSlaveManualOperation = TRUE;
		m_bH245_capabilitiesManualOperation = TRUE;

		memset(m_atAlias, 0, sizeof(m_atAlias));
		memset(m_ach_UserName, 0, sizeof(m_ach_UserName));
		memset(m_ach_Password, 0, sizeof(m_ach_Password));
		m_emTerminalType  = emEndpointTypeMT;
		m_emMsTerminalType = emMsMT;
		m_nProductIdSize  = 0;
		m_nVerIdSize      = 0;
		memset(m_abyProductId, 0, sizeof(m_abyProductId));
		memset(m_abyVersionId, 0, sizeof(m_abyVersionId));
		m_achRas_defaultGkIp[0] = 127;
		m_achRas_defaultGkIp[1] = 0;
		m_achRas_defaultGkIp[2] = 0;
		m_achRas_defaultGkIp[3] = 1;

        m_nPortFrom = 0;
        m_nPortTo   = 0;

		m_dwStackIp = 0;

		//add by yj for ipv6
		m_byIPType = TYPE_NONE;   
		memset(m_dwStackIp_ipv6, 0, sizeof(m_dwStackIp_ipv6));
		m_swScopeId = -1;
		//end
       

	};

	//设置最大呼叫数、最大逻辑通道数
	void SetMaxNum( u32 dwMaxCalls, u32 dwMaxChannels )
	{
        m_dwSystem_maxCalls = (dwMaxCalls>MIN_CALL_NUM && dwMaxCalls<MAX_CALL_NUM)?dwMaxCalls:MAX_CALL_NUM;
        m_dwSystem_maxChannels = (dwMaxChannels>MIN_CHANNEL_NUM && dwMaxChannels<MAX_CHANNEL_NUM)?dwMaxChannels:MIN_CHANNEL_NUM;
	}
	//获取最大呼叫数
	u32 GetMaxCalls()
	{
		return m_dwSystem_maxCalls;
	}
	//获取最大通道数
	u32 GetMaxChannels()
	{
		return m_dwSystem_maxChannels;
	}
	//设置是否使用手动RAS
	void SetMannualRAS( BOOL bManualRAS, BOOL bManualReg )
	{
		m_bRas_manualRAS = bManualRAS;
		m_bRas_manualRegistration = bManualReg;
	}
	//获取是否手动RAS
	BOOL IsManualRAS()
	{
		return m_bRas_manualRAS;
	}
	//获取是否手动注册
	BOOL IsManualReg()
	{
		return m_bRas_manualRegistration;
	}
	//设置终端类型、主从决定终端类型
	void SetTerminalType( emEndpointType emEpType, emMsTerminalType emMSTType )
	{
		m_emTerminalType = emEpType;
		m_emMsTerminalType = emMSTType;
	}
	//获取终端类型
	emEndpointType GetEndpointType()
	{
		return m_emTerminalType;
	}
	//获取主从决定的终端类型
	emMsTerminalType GetMsTerminalType()
	{
		return m_emMsTerminalType;
	}
	//设置产品号
	void SetProductId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyProductId, 0, sizeof(m_abyProductId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nProductIdSize = nReal;
        if(nLen >= LEN_1)
		{
			memcpy(m_abyProductId, pbyId, nReal);
		}
	}
	//获取产品号长度
	s32 GetProductIdSize()
	{
		return m_nProductIdSize;
	}
	//获取产品号
	s32 GetProductId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nProductIdSize); 
		memcpy(pbyId, m_abyProductId, nReal);
		return nReal;
	}
	//设置版本号	
	void SetVersionId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyVersionId, 0, sizeof(m_abyVersionId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nVerIdSize = nReal;
        if(nLen >= LEN_1)
		{
			memcpy(m_abyVersionId, pbyId, nReal);
		}
	}
	//获取版本号长度
	s32 GetVersionIdSize()
	{
		return m_nVerIdSize;
	}
	//获取版本号
	s32 GetVersionId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nVerIdSize); 
		memcpy(pbyId, m_abyVersionId, nReal);
		return nReal;
	}
	//设置用户密码
	void SetUserInfo(s8* pszUserName, s8* pszPassword)
	{
		if (pszUserName == NULL)
		{
			return;
		}
		memset(m_ach_UserName, 0, sizeof(m_ach_UserName));
		u16 nameLen = min( strlen(pszUserName), REG_NAME_LEN);
		strncpy(m_ach_UserName, pszUserName, nameLen);
		m_ach_UserName[nameLen] = '\0';

		if (pszPassword)
		{
			memset(m_ach_Password, 0, sizeof(m_ach_Password));
			u16 pwdLen = min(strlen(pszPassword), REG_PWD_LEN);
			strncpy(m_ach_Password, pszPassword, pwdLen);
			m_ach_Password[pwdLen] = '\0';
		}

	}
	//取用户名
	s8* GetUserName()
	{
		return m_ach_UserName;
	}
	//取密码
	s8* GetPassword()
	{
		return m_ach_Password;
	}

	//设置缺省GK地址
	void SetDefGKIP(u32 dwGkIP, u16  wGkPort = RASPORT)
	{
		s8 *p = (s8 *)(&dwGkIP);
		memset(m_achRas_defaultGkIp, 0, sizeof(m_achRas_defaultGkIp));
		for(s32 nIndex=0; nIndex<4; nIndex++)
        {
            m_achRas_defaultGkIp[nIndex] = *(p+nIndex);			
        }
		m_wRas_defaultGkPort = wGkPort;
		m_byIPType = TYPE_IPV4;
	}
	//获取缺省GK地址
	u32 GetDefGKIP()
	{
		u32 dwGkIP = 0;
		s8 *p = (s8 *)(&dwGkIP);
		for(s32 nIndex=0; nIndex<4; nIndex++)
        {
			*(p+nIndex) = m_achRas_defaultGkIp[nIndex];
        }
		return dwGkIP;
	}
	//获取缺省GK端口
	u16 GetDefGKPort()
	{
		return m_wRas_defaultGkPort;
	}

	//设置别名
	void SetAliasAddr( TALIASADDR &tAliasAddr, u32 dwAliasNo )
	{
		if ( dwAliasNo >= type_others )
		{
			return;
		}
		m_atAlias[dwAliasNo] = tAliasAddr;
	}
	//获取别名类型
	PTALIASADDR GetAliasAddr( u32 dwAliasNo )
	{
		if ( dwAliasNo >= type_others )
		{
			return NULL;
		}
		return &(m_atAlias[dwAliasNo]);
	}

	//设置h225呼叫端口
	void SetH225CallingPort( u16 wCallPort = CALLPORT )
	{
		m_wQ931_CallingPort = wCallPort;
	}
	//获取h225呼叫端口
	u16 GetH225CallingPort()
	{
		return m_wQ931_CallingPort;
	}
    //设置本地RAS端口
    void SetLocalRASPort( u16 wRASPort = RASPORT )
    {
        m_wRas_defaultlocalPort = wRASPort;
    }
    //获取本地RAS端口
    u16 GetLocalRASPort()
    {
        return m_wRas_defaultlocalPort;
    }
	//设置是否手动MSD，CSE
	void SetH245IsManual( BOOL bMsdManual, BOOL bCseManual )
	{
		m_bH245_masterSlaveManualOperation  = bMsdManual;
		m_bH245_capabilitiesManualOperation = bCseManual;
	}
	//获取MSD是否手动操作
	BOOL IsMasterSlaveManual()
	{
		return m_bH245_masterSlaveManualOperation;
	}
	//获取CSE是否手动操作
	BOOL IsCapabilitiesManual()
	{
		return m_bH245_capabilitiesManualOperation;
	}
    //设置端口范围
    void SetPortRange(s32 portFrom, s32 portTo)
    {
        m_nPortFrom = portFrom;
        m_nPortTo   = portTo;
    }
    //得到起始端口号
    s32 GetFirstPort()
    {
        return m_nPortFrom;
    }
    //得到最后一个端口号
    s32 GetLastPort()
    {
        return m_nPortTo;
    }

//add by yj for ipv6
	//设置ip地址类型
	u8 GetStackIP_Type( )
	{
		return	m_byIPType;
	}
//	void SetStackIP_Type(u8 byIPType)
//	{
//		m_byIPType = byIPType;
//	}
	//设置ScopeId(ipv6)值
	s16 GetStackIP_Ipv6_ScopeId( )
	{
		return	m_swScopeId;
	}
//	void SetStackIP_Ipv6_ScopeId(s16 swScopeId)
//	{
//		m_swScopeId = swScopeId;
//	}

	//设置协议栈运行IP(ipv6)
	void SetStackIP_Ipv6(u8 *dwIP_ipv6,s16 swScopeId)
	{
		memcpy(m_dwStackIp_ipv6, dwIP_ipv6, sizeof(m_dwStackIp_ipv6));	
		m_swScopeId = swScopeId;
        m_byIPType = TYPE_IPV6;
	}
	
	u8* GetStackIP_Ipv6()
	{
		return m_dwStackIp_ipv6;
		
	}
//end

	//设置协议栈运行IP
	void SetStackIP(u32 dwIP)
	{
		m_dwStackIp = dwIP;
        m_byIPType = TYPE_IPV4;
	}
	u32	GetStackIP()
	{
		return m_dwStackIp;
	}
}TH323CONFIG,*PH323TH323CONFIG; 

//h323stack additional config
typedef struct tagAdditionalConfig
{
	u32 m_dwNATAddress;
	
	//设置NAT地址
	void SetNATAddress(u32 dwNATAddress)
	{
		m_dwNATAddress = dwNATAddress;
	}
	//得到NAT地址
	u32 GetNATAddress()
	{
		return m_dwNATAddress;
	}
}TAdditionalConfig;

//transport地址
typedef struct tagTransportAddress
{
    //add by yj for ipv6
	u8          m_byIPType;                                 //支持的 IP地址类型
	u8          m_dwIP_ipv6[IPV6_NAME_LEN];			        //协议栈运行IP(ipv6)，非网络序;
//	s16         m_swScopeId;                                //获取或设置 IPv6 地址范围标识符
    //end
	u32 m_dwIP;
	u16 m_wPort;
	u8	m_byType;

	tagTransportAddress()
	{
		Clear();
	}

	void Clear()
	{
		m_dwIP	 = 0;
		m_wPort  = 0;
		m_byType = addr_uca;
		//add by yj for ipv6
		m_byIPType = TYPE_NONE;   
	    memset(m_dwIP_ipv6, 0, sizeof(m_dwIP_ipv6));
//		m_swScopeId = -1;
        //end
	}

    void host2net(BOOL bhton)
    {
        if (bhton)
        {
            m_dwIP  = htonl(m_dwIP);
            m_wPort = htons(m_wPort);
        }
        else
        {
            m_dwIP  = ntohl(m_dwIP);
            m_wPort = ntohs(m_wPort);
        }
    }

	//设置transport地址
	void SetNetAddress(u32 dwIPAddr, u16 wIPPort, u8 byAddrType=addr_uca)
	{
		m_dwIP   = dwIPAddr;
		m_wPort  = wIPPort;
		m_byType = byAddrType;
		m_byIPType = TYPE_IPV4;
	}
	//add by yj for ipv6
	//设置ip地址类型
	void SetIPAddr_Type(u8 byIPType)
	{
		m_byIPType = byIPType;
	}
	u8 GetIPAddr_Type( )
	{
		return	m_byIPType;
	}
	//void SetNetAddress_Ipv6(u8* dwIPAddr_ipv6, u16 wIPPort, s16 swScopeId, u8 byAddrType=addr_uca)
	void SetNetAddress_Ipv6(u8* dwIPAddr_ipv6, u16 wIPPort, u8 byAddrType=addr_uca)
	{
		memcpy(m_dwIP_ipv6, dwIPAddr_ipv6, sizeof(m_dwIP_ipv6));
		m_wPort  = wIPPort;
		m_byType = byAddrType;
		m_byIPType = TYPE_IPV6;
	//	m_swScopeId = swScopeId;
	}
// 	void SetIPAddr_Ipv6_ScopeId(s16 swScopeId)
// 	{
// 		m_swScopeId = swScopeId;
// 	}
	u8 *GetIPAddr_Ipv6()
	{
		return m_dwIP_ipv6;
	}
//	s16 GetIPAddr_Ipv6_ScopeId( )
//	{
//		return	m_swScopeId;
//	}
	//end

	//获取IP地址
	u32 GetIPAddr()
	{
		return m_dwIP;
	}
	//获取端口
	u16 GetIPPort()
	{
		return m_wPort;
	}
	//获取类型
	u8 GetNetAddrType()
	{
		return m_byType;
	}
}TNETADDR,*PTNETADDR;

//呼叫地址
typedef struct tagCallAddr
{
private:
	CallAddrType    m_eType;
	TNETADDR		m_tIPAddr;					
	TALIASADDR		m_tAliasAddr;
public:
	tagCallAddr()
	{
		Clear();
	}

	void Clear()
	{
		m_tIPAddr.Clear();
		m_tAliasAddr.Clear();
	}
	
	//设置CallAddrType
	void SetCallAddrType(CallAddrType emAddrType)
	{
		m_eType = emAddrType;
	}
	//获得CallAddrType
	CallAddrType GetCallAddrType()
	{
		return m_eType;
	}

	//设置IP地址和端口号
	void SetIPAddr(u32 dwIp,u16 wPort = 0)
	{
		m_tIPAddr.m_dwIP  = dwIp;
		m_tIPAddr.m_wPort = htons(wPort);
	}	
	//获得IP地址
	u32 GetIPAddr()  
	{	
		return m_tIPAddr.m_dwIP;	
	}

	//获得端口号
	u16 GetIPPort()  
	{	
		return ntohs( m_tIPAddr.m_wPort );	
	}
	//设置Alias地址
	void SetAliasAddr( AliasType emAliasType, const s8* sAlias )
	{
		memset( &m_tAliasAddr, 0, sizeof(m_tAliasAddr) );
		m_tAliasAddr.m_emType = emAliasType;

		if ( sAlias != NULL ) 
		{
			u16 wLen = min( strlen(sAlias), LEN_ALIAS );
			strncpy( m_tAliasAddr.m_achAlias, sAlias, wLen );
			m_tAliasAddr.m_achAlias[wLen] = '\0';
		}
	}	
	//获得TALIASADDR的type
	AliasType GetAliasType()
	{
		return m_tAliasAddr.m_emType;
	}
	//获得TALIASADDR的别名
	s8* GetAliasName()  
	{
		return m_tAliasAddr.m_achAlias;	
	}	
}TCALLADDR,*PTCALLADDR;

//呼叫时的别名地址
typedef struct tagAliasAddr
{
	CallAddrType    m_emType;		//呼叫类型
	TNETADDR		m_tNetAddr;		//IP呼叫时使用		
	TALIASADDR		m_tAlias;		//别名呼叫使用

	tagAliasAddr()
	{
		m_emType = cat_err;
	}

	void Clear()
	{
		m_emType = cat_err;
		m_tNetAddr.Clear();
		m_tAlias.Clear();
	}

	//设置为IP地址呼叫
	void SetIPCallAddr( TNETADDR *pTNetAddr )
	{
		if ( pTNetAddr != NULL )
		{
			m_emType = cat_ip;
			memcpy( &m_tNetAddr, pTNetAddr, sizeof(m_tNetAddr) );
		}		
	}

	//add by yj for ipv6
	//获取ip地址类型
	u8 GetIPAddr_Type( )
	{
		if (m_emType == cat_ip) 
		{
		return	m_tNetAddr.GetIPAddr_Type();
		}
		return TYPE_NONE;
	}
	//获取呼叫的IP(ipv6)地址
	u8* GetIPAddr_Ipv6()
	{
		if (m_emType == cat_ip) 
		{
			if (m_tNetAddr.m_byIPType == TYPE_IPV6)
			{
				return m_tNetAddr.GetIPAddr_Ipv6();
			}
		}
		return 0;
	}
//	s16 GetIPAddr_Ipv6_ScopeId()
//	{
//		if (m_emType == cat_ip) 
//		{
//			if (m_tNetAddr.m_byIPType == TYPE_IPV6)
//			{
//				return m_tNetAddr.GetIPAddr_Ipv6_ScopeId();
//			}
//		}
//		return -1;
//	}

	//end

	//获取呼叫的IP地址
	u32 GetIPAddr()
	{
		if (m_emType == cat_ip) 
		{
			//add by yj
			if (m_tNetAddr.m_byIPType == TYPE_IPV4)
			{
				return m_tNetAddr.GetIPAddr();
			}
		    //end
		}
		return 0;
	}

	//获取呼叫的端口
	u16 GetIPPort()
	{
		if (m_emType == cat_ip) 
		{
			return m_tNetAddr.GetIPPort();
		}
		return 0;
	}
	//设置为别名地址呼叫
	void SetIPCallAddr( TALIASADDR *pAliasAddr )
	{
		if ( pAliasAddr != NULL )
		{
			m_emType = cat_alias;
			memcpy( &m_tAlias, pAliasAddr, sizeof(m_tAlias) );
		}		
	}
	//获取别名类型
	AliasType GetAliasType()
	{
		if (m_emType == cat_alias)
		{
			return m_tAlias.GetAliasType();
		}
		return type_aliastype;
	}
	//获取别名
	s8* GetAliasName()
	{
		if (m_emType == cat_alias)
		{
			return m_tAlias.GetAliasName();
		}
		return NULL;
	}
	//获取呼叫类型
	CallAddrType GetCallAddrType()
	{
		return m_emType;
	}
}TAliasAddr,*PTAliasAddr;



//connect消息到来时返回给上层的结构
typedef struct tagNodeInfo 
{
    emEndpointType  m_emEndpointType;   
	s32				m_nVenderid;					//厂商id
	s8				m_chName[LEN_DISPLAY_MAX+1];	//结点名称
	BOOL			m_bIsEncrypt;					//是否支持加密

    //2007.05.30 
	s32				m_nProductIdSize;
	u8				m_abyProductId[LEN_256];				//产品号
	s32				m_nVerIdSize;
	u8				m_abyVersionId[LEN_256];				//版本号
    
	u32             m_dwCallRate;						  //呼叫速率	(kbps)

	emNATFWTraversal	  m_bySupportMediaTraversal;
	TH460Features		  m_tH460Features;
	
	TAliasAddr			  m_atCallingAliasAddr[type_others+1]; //主叫别名地址(别名地址+IP)
	TAliasAddr			  m_atCalledAliasAddr[type_others+1]; //被叫别名地址(别名地址+IP)

    tagNodeInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_emEndpointType = emEndpointTypeMT;
		m_nVenderid = vendor_KEDA;
   		memset( m_chName, 0, sizeof(m_chName) ); 		
		
		m_bIsEncrypt = FALSE;

		m_nProductIdSize = 0;
		m_nVerIdSize = 0;
        memset(m_abyProductId, 0, sizeof(m_abyProductId));
        memset(m_abyVersionId, 0, sizeof(m_abyVersionId));
		m_bySupportMediaTraversal = emNotSupportNATFWTraversal;
		memset( &m_tH460Features, 0, sizeof(m_tH460Features) );
	}

	//设置终端类型
	void SetEndpointType(emEndpointType emEpType)
	{
		m_emEndpointType = emEpType;
	}
	//获取终端类型
	emEndpointType GetEndpointType()
	{
		return m_emEndpointType;
	}
	//设置厂商ID
	void SetVenderId( s32 nVenderId)
	{
		m_nVenderid = nVenderId;
	}
	//获取厂商ID
	s32 GetVenderId()
	{
		return m_nVenderid;
	}
	//设置display name
	void SetName(s8 *pchName, s32 nLen)
	{
		if (pchName == NULL || nLen <= 0) 
			return;
		s32 realLen = min(nLen,LEN_DISPLAY_MAX);
		strncpy( m_chName, pchName, realLen );
		m_chName[realLen] = '\0';
	}
	//获取display name
	s8* GetName()
	{
		return m_chName;
	}
	//设置是否加密
	void SetIsEncrypt( BOOL bIsEncrypt )
	{
		m_bIsEncrypt = bIsEncrypt;
	}
	//获取是否加密
	BOOL IsEncrypt()
	{
		return m_bIsEncrypt;
	}

    	//设置产品号
	void SetProductId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyProductId, 0, sizeof(m_abyProductId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nProductIdSize = nReal;
        if(nLen >= LEN_1)
		{
			memcpy(m_abyProductId, pbyId, nReal);
		}
	}
	//获取产品号长度
	s32 GetProductIdSize()
	{
		return m_nProductIdSize;
	}
	//获取产品号
	s32 GetProductId(u8 *pbyId, s32 nLen)
	{
		if( pbyId == NULL || nLen <= 0 || m_nProductIdSize <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nProductIdSize); 
		memcpy(pbyId, m_abyProductId, nReal);
		return nReal;
	}
	//设置版本号	
	void SetVersionId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyVersionId, 0, sizeof(m_abyVersionId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nVerIdSize = nReal;
        if(nLen >= LEN_1)
		{
			memcpy(m_abyVersionId, pbyId, nReal);
		}
	}
	//获取版本号长度
	s32 GetVersionIdSize()
	{
		return m_nVerIdSize;
	}
	//获取版本号
	s32 GetVersionId(u8 *pbyId, s32 nLen)
	{
		if( pbyId == NULL || nLen <= 0 || m_nVerIdSize <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nVerIdSize); 
		memcpy(pbyId, m_abyVersionId, nReal);
		return nReal;
	}

	void SetSupportMediaTraversal(emNATFWTraversal emNet)
	{
		m_bySupportMediaTraversal = emNet;
	}
	emNATFWTraversal GetSupportMediaTraversal()
	{
		return m_bySupportMediaTraversal;
	}
	void SetH460features( const TH460Features &th460features )
	{
		memcpy( &m_tH460Features, &th460features, sizeof(TH460Features) );
	}
	void GetH460features( TH460Features &th460features )
	{
		memcpy( &th460features, &m_tH460Features, sizeof(TH460Features) );
	}


	//设置主叫别名地址	作为源端的信息
	void SetCallingAliasaddr( TAliasAddr &tAliasAddr, u8 addrNo )
	{
		if ( addrNo < type_others+1  )
		{
			m_atCallingAliasAddr[addrNo] = tAliasAddr;
		}
	}
	//获取主叫别名地址
	PTAliasAddr GetCallingAliasaddr( u8 addrNo )
	{
		if ( addrNo < type_others+1  )
		{
			return &(m_atCallingAliasAddr[addrNo]);
		}
		return NULL;
	}
	
	//设置被叫别名地址	作为目的端的信息
	void SetCalledAliasAddr( TAliasAddr &tAliasAddr, u8 byAddrNo )
	{
		if ( byAddrNo < type_others+1  )
		{
			m_atCalledAliasAddr[byAddrNo] = tAliasAddr;
		}		
	}
	//获取被叫别名地址
	PTAliasAddr GetCalledAliasAddr( u8 byAddrNo )
	{
		if ( byAddrNo < type_others+1  )
		{
			return &(m_atCalledAliasAddr[byAddrNo]);
		}
		return NULL;
	}


}TNODEINFO,*PTNODEINFO;



//会议信息
typedef struct tagConfInfo
{
	u8          m_abyConfId[LEN_GUID];
	TAliasAddr  m_tConfAlias;
	
	tagConfInfo()
	{
		memset( m_abyConfId, 0, sizeof(m_abyConfId) );
	}

	
	void Clear()
	{
		memset( m_abyConfId, 0, sizeof(m_abyConfId) );
		m_tConfAlias.Clear();
	}

	//设置会议ID
	void SetConfId( u8 *pbyConfId, u32 dwLen )
	{
		if ( pbyConfId == NULL )
			return;

		u32 realLen = min(dwLen, LEN_GUID);
		memcpy( m_abyConfId, pbyConfId, realLen );
		m_abyConfId[realLen] = '\0';
	}
	//获取会议ID
	u8* GetConfId()
	{
		return m_abyConfId;
	}
	
	//设置TAliasAddr
	void SetAliasAddr( TAliasAddr &tAliasAddr)
	{
		m_tConfAlias = tAliasAddr;
	}
	//获取TAliasAddr
	TAliasAddr& GetAliasAddr()
	{
		return m_tConfAlias;
	}
}TCONFINFO,*PTCONFINFO;

//终端编号参数
typedef struct tagTerminalLabel 
{
	u8 m_byMcuNo;
	u8 m_byTerNo;

	tagTerminalLabel()
	{
		Clear();		
	}

	void Clear()
	{
		m_byMcuNo = 0xff;
		m_byTerNo = 0xff;
	}

	bool operator ==(tagTerminalLabel& tLabel)
	{
		if (m_byMcuNo == tLabel.m_byMcuNo && m_byTerNo == tLabel.m_byTerNo)
			return true;
		else
			return false;
	}

	bool operator !=(tagTerminalLabel& tLabel)
	{
		if (m_byMcuNo != tLabel.m_byMcuNo || m_byTerNo != tLabel.m_byTerNo)
			return true;
		else
			return false;
	}
    //有效性检查
	bool IsValid()
	{
		return ( m_byMcuNo < LEN_MTNUM_PERCONF && m_byTerNo < LEN_MTNUM_PERCONF);
	}
    
    //设置TerminalLabel
	BOOL SetTerminalLabel( u8 byMcuNo, u8 byTerNo )
	{
		if( byMcuNo < LEN_MTNUM_PERCONF && byTerNo < LEN_MTNUM_PERCONF )
		{
			m_byMcuNo = byMcuNo;
			m_byTerNo = byTerNo;
            return TRUE;
		}
        return FALSE;
	}
    //获取TerminalLabel
	void GetTerminalLabel( u8 *pMcuNo, u8 *pTerNo )
	{
		*pMcuNo = m_byMcuNo;
		*pTerNo = m_byTerNo;
	}
    //获取mcu编号
	u8 GetMcuNo()
	{
		return m_byMcuNo;
	}
    //获取终端编号
	u8 GetTerNo()
	{
		return  m_byTerNo;
	}
}TTERLABEL,*PTTERLABEL;

/*add by jason 2004-08-09*/
typedef struct
{
    unsigned char m_abyMacAddress[6];
    unsigned char m_abyTime[8];
    BOOL m_bClock_seq_change;
} uCID;

typedef struct
{
	u32		m_dwStartTime;
	u32		m_dwStartMilliTime;
	u32		m_dwOldMilliTime;
	u8		m_abyLastTime[8];
	BOOL	m_bNotStart;
	BOOL	m_bGenerated;
	uCID	m_tUCID;
	u8  	m_abyCid[16];
	u8  	m_abyOldCid[16];
	u16     m_wClock_seq;
} cidLocalStorage;

u8* GetGUID(void);
//

typedef struct tagMultiPic
{
	u8  m_byMcuNo;
	u8  m_byStyle;//emPicStyle
	TTERLABEL m_tTer[16];
	
	tagMultiPic()
	{
		Clear();
	}

	void Clear()
	{
		m_byMcuNo = 0;
		m_byStyle = 0;
		memset(m_tTer, 0, sizeof(m_tTer));//Huawei默认都为0
	}
	
}TMultiPic, *PTMultiPic;


typedef struct tagChanMultiCast
{
	u8  m_byChanID;    //1视频，2音频，3第二路视频
	u32 m_dwIP;        //组播地址
	u16 m_wPort;       //组播端口
	//add by yj for ipv6
	u8          m_byIPType;                                 //支持的 IP地址类型
	u8          m_dwIP_ipv6[IPV6_NAME_LEN];			         //组播地址ipv6，非网络序;
	s16         m_swScopeId;                                //获取或设置 IPv6 地址范围标识符
	//end

    tagChanMultiCast()
    {
        Clear();
    }

    void Clear()
    {
        m_byChanID = emChanUnkown;
        m_dwIP = 0;
        m_wPort = 0;
		//add by yj for ipv6
		m_byIPType = TYPE_NONE;   
	    memset(m_dwIP_ipv6, 0, sizeof(m_dwIP_ipv6));
		m_swScopeId = -1;
        //end
    }

    void host2net(BOOL bhton)
    {
        if (bhton)
        {
            m_dwIP  = htonl(m_dwIP);
            m_wPort = htons(m_wPort);
        }
        else
        {
            m_dwIP  = ntohl(m_dwIP);
            m_wPort = ntohs(m_wPort);
        }
    }

    void SetChanID(u8 byChan)
    {
        m_byChanID = byChan;
    }
	
    u8 GetChanID()
    {
        return m_byChanID;
    }

    //add by yj for ipv6
	//设置ip地址类型
	void SetChanAddr_Type (u8 byIPType)
	{
		m_byIPType = byIPType;
	}
	u8 GetChanAddr_Type( )
	{
		return	m_byIPType;
	}
	//设置ScopeId(ipv6)值
	s16 GetChanAddr_Ipv6_ScopeId( )
	{
		return	m_swScopeId;
	}
	void SetChanAddr_Ipv6_ScopeId(s16 swScopeId)
	{
		m_swScopeId = swScopeId;
	}
	//设置协议栈运行IP(ipv6)
	void SetChanAddr_Ipv6(u8 *dwIP_ipv6)
	{
		memcpy(m_dwIP_ipv6, dwIP_ipv6, sizeof(m_dwIP_ipv6));	
	}
	u8 *GetChanAddr_Ipv6()
	{
		return m_dwIP_ipv6;
	}
	

	//end

    void SetChanAddr(u32 ip, u16 port)
    {
        m_dwIP = ip;
        m_wPort = port;
    }

    u32 GetChanIp()
    {
        return m_dwIP;
    }

    u16 GeChantPort()
    {
        return m_wPort;
    }
    
}TCHANMULTICAST, *PTCHANMULTICAST;


/*
多点会议中厂商能力指示

*/
typedef struct tagCapInfo
{   
    s32  m_nVendor;                     //厂商ID
    BOOL m_bIsMCU;                      //FALSE:MT, TRUE:MCU
    BOOL m_bMix;                        //混音
    BOOL m_bVAC;                        //语音激励
    BOOL m_bCustomVMP;                  //定制画面合成
    BOOL m_bAutoVMP;                    //自动画面合成
    BOOL m_bEndConf;                    //结束会议
    BOOL m_bInvateMt;                   //邀请终端
    BOOL m_bDropMt;                     //删除终端
    BOOL m_bSelSpeaker;                 //指定/取消发言人
    BOOL m_bSelChair;                   //指定/放弃主席
    BOOL m_bSelSource;                  //选看终端
    BOOL m_bFECC;                       //远程摄像头遥控
    BOOL m_bQuiet;                      //远端静音
    BOOL m_bMute;                       //远端哑音
    BOOL m_bConfReq;                    //会议申请处理
    BOOL m_bOnlineList;                 //在线终端列表
    BOOL m_bOfflineList;                //离线终端列表
    BOOL m_bSelSpeakingUser;            //点名
    BOOL m_bPicSwitch;                  //单多画面切换
    BOOL m_bMustSeeSpeaker;             //强制广播
    //...                                
                            

    tagCapInfo()
    {
        Clear();
    }

    void Clear()
    {
        m_nVendor = vendor_UNKNOWN;
        m_bIsMCU = FALSE;
        m_bMix = FALSE;
        m_bCustomVMP = FALSE;
        m_bAutoVMP = FALSE;
        m_bVAC = FALSE;
        m_bEndConf = FALSE;
        m_bInvateMt = FALSE;
        m_bDropMt = FALSE;
        m_bSelSource = FALSE;
        m_bSelSpeaker = FALSE;
        m_bSelChair = FALSE;
        m_bFECC = FALSE;
        m_bQuiet = FALSE;
        m_bMute = FALSE;
        m_bOnlineList = FALSE;
        m_bOfflineList = FALSE;
        m_bConfReq = FALSE;
        m_bSelSpeakingUser = FALSE;
        m_bPicSwitch = FALSE;
        m_bMustSeeSpeaker = FALSE;
    }

}TCAPINFO, *PTCAPINFO;

/*
该功能主要用于在会议中A终端从串口读到的数据 通过网络传输给对端 
消息内容
  1.数据类型    //初步用 1 -RS232串口数据  2 =RS422 3=RS485 
  2.数据长度
  3.数据
*/
typedef struct tagCOMData
{
    emComType  m_emComType;
    s32 m_nDataLen;
    s8  m_achData[128];
    
public:
    tagCOMData()
    {
        Clear();
    }

    void Clear()
    {
        m_emComType = emComUnkown;
        memset(m_achData, 0, sizeof(m_achData));
        m_nDataLen = 0;
    }

    void host2net(BOOL bhton)
    {
        s32 nType = m_emComType;
        if (bhton)
        {
            m_emComType  = (emComType)htonl(nType);
            m_nDataLen   = htonl(m_nDataLen);
        }
        else
        {
            m_emComType  = (emComType)ntohl(nType);
            m_nDataLen   = ntohl(m_nDataLen);
        }
    }

    void SetComType(emComType emType)
    {
        m_emComType = emType;
    }

    emComType GetComType()
    {
        return m_emComType;
    }

    void SetComData(s8 *pData, s32 nLen)
    {
        if (NULL == pData || nLen < 0)
        {
            return;
        }

        m_nDataLen = min(nLen, sizeof(m_achData));
        memset(m_achData, 0, sizeof(m_achData));
        memcpy(m_achData, pData, m_nDataLen);
        m_achData[m_nDataLen] = '\0';
    }

    s32 GetComDataLen()
    {
        return m_nDataLen;
    }

    s8* GetComData()
    {
        return m_achData;
    }

}TCOMDATA, *PTCOMDATA;


//视频能力集(不表示H264的能力) 
/*********************关于帧率的设置******************
*1表示29.97, 2表示29.97/2, 3表示29.97/3,.....
*H261：cif(1-4) qcif(1-4)
*H263: sqcif,qcif,cif,4cif,16cif  (1-32)
*MP2/MP4: cif, 2cif, 4cif 1-30
*****************************************************/
typedef struct tagVideoCap
{
	cmCapDirection  m_emCapDirection;		//transmit or receive
	u16             m_wMaxBitRate;						//单位kbps
	u8              m_abyResolution[emResolutionEnd];	//帧率  	
public:
	tagVideoCap()
	{
		Clear();
	}
	
	void Clear()
	{
		m_emCapDirection = cmCapReceive;
		m_wMaxBitRate = 0;
		memset(m_abyResolution, 0, sizeof(m_abyResolution));        
	}

    void host2net(BOOL bhton)
    {
        s32 nCapDirection = m_emCapDirection;
        if (bhton)
        {
            m_emCapDirection = (cmCapDirection)htonl(nCapDirection);
            m_wMaxBitRate    = htons(m_wMaxBitRate);
        }
        else
        {
            m_emCapDirection = (cmCapDirection)ntohl(nCapDirection);
            m_wMaxBitRate    = ntohs(m_wMaxBitRate);
        }
    }

	//单位kbps
	void SetBitRate(u16 wBitRate)
	{
		m_wMaxBitRate = wBitRate;		
	}
	u16 GetBitRate() const
	{
        return m_wMaxBitRate;
	}

	void SetCapDirection(cmCapDirection emDirect)
	{
		m_emCapDirection = emDirect;
	}
	cmCapDirection GetCapDirection() const
	{
		return m_emCapDirection;
	}

	/*
	 *function: 设置支持的分辨率、帧率
	 *param:	emRes 表示分辨率 byMPI表示帧率,参见结构头说明
     *return:   TRUE or FALSE 
	 */
	BOOL SetResolution(emResolution emRes, u8 byMPI)
	{
		if(emRes <= emResolutionBegin || emRes >= emResolutionEnd)
		{
			return FALSE;
		}
		m_abyResolution[emRes] = byMPI;
	    return TRUE;
	}
    /*
	 *function: 得到支持的分辨率、帧率
	 *param:	emRes 表示分辨率 byMPI表示帧率,参见结构头说明
     *return:   帧率(0 表示不支持改分辨率)  
	 */
	u8 GetResolution(emResolution emRes) const
	{
		if(emRes <= emResolutionBegin || emRes >= emResolutionEnd)
		{
			return 0;
		}
		return m_abyResolution[emRes];
	}
	
	void MergeVideoCap( const tagVideoCap &tVideoCap )
	{
		if (m_wMaxBitRate != 0)
		{
			m_wMaxBitRate = min( m_wMaxBitRate, tVideoCap.m_wMaxBitRate );
		}
		else
		{
			m_wMaxBitRate = tVideoCap.m_wMaxBitRate;
		}
		m_emCapDirection = tVideoCap.m_emCapDirection;
		for ( int nIndex = emResolutionBegin +1; nIndex < emResolutionEnd; ++nIndex )
		{
			if ( m_abyResolution[nIndex] == 0 )
			{
				m_abyResolution[nIndex] = tVideoCap.m_abyResolution[nIndex];
			}
		}
	}
}TVideoCap;

//H.264视频能力集
/*********************关于帧率的设置******************
*1表示1fps, 60代表60fps，最大允许帧率可以达到理论值255
*****************************************************/
typedef struct tagH264VideoCap
{
public:
    // 协议 
    enum  emH264ProfileMask
    {
        emProfileBaseline   = 64,
        emProfileMain       = 32,
        emProfileExtend     = 16,
        emProfileHigh       = 8
    };

    tagH264VideoCap()
    {
        Clear();
    }
    
    void Clear()
    {
        memset(this, 0, sizeof(tagH264VideoCap));
        m_emCapDirection = cmCapReceive;
        m_wMaxBitRate = 0;
        SetSupportProfile(emProfileBaseline);
        m_byLevel = 36;  //level 1.3 
		m_nParaFlag = 0;
        m_nMaxBRandCPB = 0;
    }
    
    void host2net(BOOL bhton)
    {
        s32 nCapDirection = m_emCapDirection;
        if (bhton)
        {
            m_emCapDirection = (cmCapDirection)htonl(nCapDirection);
            m_wMaxBitRate    = htons(m_wMaxBitRate);
            m_nMaxMBPS       = htonl(m_nMaxMBPS);
            m_nMaxFS         = htonl(m_nMaxFS);
            m_nMaxDPB        = htonl(m_nMaxDPB);
            m_nMaxBRandCPB   = htonl(m_nMaxBRandCPB);
            m_nMaxStaticMBPS = htonl(m_nMaxStaticMBPS);
            m_nParaFlag      = htonl(m_nParaFlag);
        }
        else
        {
            m_emCapDirection = (cmCapDirection)ntohl(nCapDirection);
            m_wMaxBitRate    = ntohs(m_wMaxBitRate);
            m_nMaxMBPS       = ntohl(m_nMaxMBPS);
            m_nMaxFS         = ntohl(m_nMaxFS);
            m_nMaxDPB        = ntohl(m_nMaxDPB);
            m_nMaxBRandCPB   = ntohl(m_nMaxBRandCPB);
            m_nMaxStaticMBPS = ntohl(m_nMaxStaticMBPS);
            m_nParaFlag      = ntohl(m_nParaFlag);
        }
    }        

    // 传输方向性
    void SetCapDirection(cmCapDirection emDirect)
    {
        m_emCapDirection = emDirect;
    }
    cmCapDirection GetCapDirection() const
    {
        return m_emCapDirection;
	}

    // 通道码率
    void SetBitRate(u16 wBitrate)
    {
        m_wMaxBitRate = wBitrate;
    }
    
    u16  GetBitRate() const
    {
        return m_wMaxBitRate;
    }

    // 协议: 支持 emH264ProfileMask 各个参数和它们的逻辑与
    void SetSupportProfile(u8 byProfile)
    {
        m_byProfile = byProfile;
// 		if(m_byProfile < 64)
// 			m_byProfile = 64;
    }

    BOOL IsSupportProfile(u8 byReqProfile) const
    {
        return ((m_byProfile&byReqProfile) == byReqProfile);
    }

    u8   GetProfileValue() const
    {
        return m_byProfile;
    }
    
    // 能力集操作

	/*
	 *function: 获取特定分辨率下的帧率
	 *param:	u8 byRes                    分辨率：emResolution 或者 emResolutionHD
     *return:   u8 帧率 frame/s
	 */
    inline u8   GetResolution(u8 byRes) const;
    inline u8   GetResolution(u16 wWidth, u16 wHeight) const;

	/*
	 *function: 特定分辨率下，特定帧率(对于场模式，需要上层转化为帧率传入)是否支持
                至于对端是否支持场格式，要业务根据 IsSupportProfile 自行判断 
	 *param:	u8 byRes                    分辨率：emResolution 或者 emResolutionHD
                u8 byFps                    帧率frame/s
     *return:   TRUE/FALSE
	 */
    inline BOOL IsSupport(u8 byRes, u8 byFps) const;
    inline BOOL IsSupport(u16 wWidth, u16 wHeight, u8 byFps) const;

	/*
	 *function: 设置支持某种分辨率和帧率(对于场模式，需要上层转化为帧率传入)。多种分辨率可以叠加设置
	 *param:	u8 byRes                    分辨率：emResolution 或者 emResolutionHD
                u8 byFps                    帧率frame/s
	 */
    inline void  SetResolution(u8 byRes, u8 byFps) ;
    inline void  SetResolution(u16 wWidth, u16 wHeight, u8 byFps) ;

    // 内部使用函数
    void  SetMaxMBPS(s32 nMax)
    {
        m_nMaxMBPS = nMax;
    }
    s32   GetMaxMBPS() const
    {
        return m_nMaxMBPS;
    }
    void  SetMaxFS(s32 nMax)
    {
        m_nMaxFS = nMax;
    }
    s32   GetMaxFS() const
    {
        return m_nMaxFS;
    }
	void  SetMaxBRandCPB(s32 nMaxBRandCPB)
	{
		m_nMaxBRandCPB = nMaxBRandCPB;
	}
	s32   GetMaxBRandCPB() const
	{
		return m_nMaxBRandCPB;
	}

    //  Level，外部调用无效
    void SetLevel(u8 byLevel)
    {
        m_byLevel = byLevel;
    }
    u8 GetLevel() const
    {
        return m_byLevel;
    }

	s32 GetParaFlag()
	{
		return m_nParaFlag;
	}
    void SetParaFlag(s32 nParaFlag)
    {
        m_nParaFlag = nParaFlag;
    }

	void SetMaxStaticMBPs( s32 nStaticMBPs )
	{
		m_nMaxStaticMBPS = nStaticMBPs;
	}
	s32 GetMaxStaticMBPs() const
	{
		return m_nMaxStaticMBPS;
	}

    // toolkit: Res enum to width*height
    inline void  Res2WxH(u8 byRes, u16 &wWidth, u16 &wHeight) const;

	BOOL operator <= (const tagH264VideoCap& tH264VideoCap)
	{
		if((m_nMaxMBPS <= tH264VideoCap.GetMaxMBPS()) && (m_nMaxFS <= tH264VideoCap.GetMaxFS()) && ( m_byProfile | tH264VideoCap.GetProfileValue() ) == tH264VideoCap.GetProfileValue() )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	void operator = (const tagH264VideoCap& tH264VideoCap)
	{
		memcpy(this, (void*)&tH264VideoCap, sizeof(tH264VideoCap));
	}

	BOOL operator == (const tagH264VideoCap& tH264VideoCap)
	{
        if(memcmp(this, (void*)&tH264VideoCap, sizeof(tH264VideoCap)) == 0)
		    	return TRUE;

		return FALSE;
	}

protected:
    cmCapDirection  m_emCapDirection;		            //transmit or receive
    u16             m_wMaxBitRate;						//单位kbps

    // H.264 参数定义，参见 ITU H.241文档
    u8              m_byProfile;
    u8              m_byLevel;
    // 以下2个字段，取 Custom 与Level值默认的 max(单位均为宏块)
    s32             m_nMaxMBPS;
    s32             m_nMaxFS;
    // 以下保存的是绝对 Custom 的字段（暂时不支持设置）
    s32             m_nMaxDPB;
    s32             m_nMaxBRandCPB;
    s32             m_nMaxStaticMBPS;

	// 以下是标志字段,如果参数偏大相应位置位
	s32             m_nParaFlag;
}TH264VideoCap;

// sif/cif 以下不支持
inline void TH264VideoCap::Res2WxH(u8 byRes, u16 &wWidth, u16 &wHeight) const
{
    switch (byRes)
    {
    case emCIF:
        wWidth = 352;   wHeight = 288;   break;
    case emCIF2:
        wWidth = 576;   wHeight = 352;   break;
    case emCIF4:
        wWidth = 704;   wHeight = 576;   break;
    case em352240:
        wWidth = 352;   wHeight = 240;   break;
    case em704480:
        wWidth = 704;   wHeight = 480;   break;
    case em640480:
        wWidth = 640;   wHeight = 480;   break;
    case em800600:
        wWidth = 800;   wHeight = 600;   break;
    case em1024768:
        wWidth = 1024;  wHeight = 768;   break;
    case emW4CIF:
        wWidth = 1024;  wHeight = 576;   break;
    case emHD720:
        wWidth = 1280;  wHeight = 720;   break;
    case emSXGA:
        wWidth = 1280;  wHeight = 1024;  break;
    case emUXGA:
        wWidth = 1600;  wHeight = 1200;  break;
    case emHD1080:
        wWidth = 1920;  wHeight = 1080;  break;
	case em1280x768:
		wWidth = 1280;  wHeight = 768;   break;
	case em1280x800:
		wWidth = 1280;  wHeight = 800;   break;
    default:
        wWidth = 352;   wHeight = 288;   break;     // default as cif
    }

    return;
}
inline u8 TH264VideoCap::GetResolution(u8 byRes) const
{
    u16 wWidth = 0;
    u16 wHeight = 0;
    Res2WxH(byRes, wWidth, wHeight);

    return GetResolution(wWidth, wHeight);
}

#define BYTE_CUTTER(n)  ( n>0xFF ? 0xFF : ( (n==11 || n==12 || n==14 || n==24 || n==29 || n==59) ? n+1 : n )  ) 
inline u8 TH264VideoCap::GetResolution(u16 wWidth, u16 wHeight) const
{    
    if (wWidth*wHeight > (m_nMaxFS)<<8)
    {
        return 0;
    }

	int nFps = 0;
	s32 nMaxFs = (wWidth*wHeight)>>8;;
	if ( ((nMaxFs>>8)<<8) < nMaxFs )
	{
		nMaxFs = (nMaxFs>>8) + 1;
		nMaxFs = nMaxFs << 8;
    }
	double dabs = (nMaxFs) - (m_nMaxFS);
	if ( m_nMaxStaticMBPS != 0 && fabs( dabs ) <= 2.0 )
	{
		double dMaxMBPS = m_nMaxMBPS;//GetMaxMBPS();
		double dMaxFS = m_nMaxFS;//GetMaxFS();
		double dMaxStaticMBPS = m_nMaxStaticMBPS;//GetMaxStaticMBPs();
		s32 nNewMaxMBPS = (s32)(1.0 / ( ( ( 4.0 / dMaxFS ) / dMaxMBPS ) + ( ( ( dMaxFS - 4.0 ) / dMaxFS ) / dMaxStaticMBPS ) ));
		nFps = (nNewMaxMBPS<<8) / (wWidth*wHeight);
	} 
	else
	{
		nFps = (m_nMaxMBPS<<8) / (wWidth*wHeight);
	}

    //return BYTE_CUTTER(nFps);
	if( nFps > 0xFF )
		nFps = 0xFF;
	return nFps;

}
#undef BYTE_CUTTER

inline BOOL TH264VideoCap::IsSupport(u8 byRes, u8 byFps) const
{
    return GetResolution(byRes) >= byFps;
}

inline BOOL TH264VideoCap::IsSupport(u16 wWidth, u16 wHeight, u8 byFps) const
{
    return GetResolution(wWidth, wHeight) >= byFps;
}

inline void TH264VideoCap::SetResolution(u8 byRes, u8 byFps)
{
    u16 wWidth = 0;
    u16 wHeight = 0;
    Res2WxH(byRes, wWidth, wHeight);
    
    SetResolution(wWidth, wHeight, byFps);
}
inline void TH264VideoCap::SetResolution(u16 wWidth, u16 wHeight, u8 byFps)
{
	//                       1  1.1 1.2 1.3 2.1 2.2 3   3.1 3.2  4  4.2   5   5.1
	const u8 abyLevel[13] = {15, 22, 29, 36, 50, 57, 64, 71, 78, 85, 99, 106, 113};

    const s32 MaxMBPSAndMaxFrame[13][2] = {{1485,99},{3000,396},{6000,396},{11880,396},{19800,792},{20250,1620},{40500,1620},
											{108000,3600},{216000,5120},{245760,8192},{522240,8704},{589824,22080},{983040,36864}};

    s32 nNewMaxFs = (wWidth*wHeight)>>8;
    if (nNewMaxFs > m_nMaxFS)
    {
        m_nMaxFS = nNewMaxFs;
        if ( ((m_nMaxFS>>8)<<8) < m_nMaxFS )
        {
            m_nMaxFS = (m_nMaxFS>>8) + 1;
			m_nMaxFS = m_nMaxFS << 8;
        }
    }
    s32 nNewMaxMBPS = nNewMaxFs*byFps;
    if (nNewMaxMBPS > m_nMaxMBPS)
    {
        m_nMaxMBPS = nNewMaxMBPS;
        if ( (m_nMaxMBPS/500*500) < m_nMaxMBPS )
        {
            m_nMaxMBPS = m_nMaxMBPS/500 + 1;
			m_nMaxMBPS = m_nMaxMBPS * 500;
        }
    }

	for(s32 i=0; i<13; i++)
	{
		if(m_nMaxMBPS >= MaxMBPSAndMaxFrame[i][0] && m_nMaxFS >= MaxMBPSAndMaxFrame[i][1])
		{
			m_byLevel = abyLevel[i];
			m_nParaFlag = 0;
			if(m_nMaxMBPS > MaxMBPSAndMaxFrame[i][0])
				m_nParaFlag = 1;
			if(m_nMaxFS > MaxMBPSAndMaxFrame[i][1])
				m_nParaFlag = m_nParaFlag | 2;
		}
		else
		{
			break;
		}
	}

    // cif*30以下不支持
// 	if(m_byLevel < 36)
// 	{
// 		m_byLevel = 36;
// 		m_nParaFlag = 0;
// 		if(m_nMaxMBPS > MaxMBPSAndMaxFrame[3][0])
// 		{
// 			m_nParaFlag = 1;
// 		}
// 		else
// 		{
// 			m_nMaxMBPS = MaxMBPSAndMaxFrame[3][0];
// 			if ( (m_nMaxMBPS/500*500) < m_nMaxMBPS )
// 			{
// 				m_nMaxMBPS = m_nMaxMBPS/500 + 1;
// 				m_nMaxMBPS = m_nMaxMBPS * 500;
// 			}
// 		}
// 		if(m_nMaxFS > MaxMBPSAndMaxFrame[3][1])
// 		{
// 			m_nParaFlag = m_nParaFlag | 2;
// 		}
// 		else
// 		{
// 			m_nMaxFS = MaxMBPSAndMaxFrame[3][1];
// 			if ( ((m_nMaxFS>>8)<<8) < m_nMaxFS )
// 			{
// 				m_nMaxFS = (m_nMaxFS>>8) + 1;
// 				m_nMaxFS = m_nMaxFS << 8;
// 			}
// 		}
// 	}
}

//音频能力集，不包括G.7231
typedef struct tagAudioCap
{
	cmCapDirection  m_emCapDirection; //transmit or receive
	u8              m_byPackTime;     //每一RTP包的最大时长,单位ms(1-256); 
public:
	tagAudioCap()
	{
		Clear();
	}

	void Clear()
	{
		m_emCapDirection = cmCapReceive;
		m_byPackTime = 60;
	};

    void host2net(BOOL bhton)
    {
        s32 nCapDirection = m_emCapDirection;
        if (bhton)
        {
            m_emCapDirection = (cmCapDirection)htonl(nCapDirection);
        }
        else
        {
            m_emCapDirection = (cmCapDirection)ntohl(nCapDirection);
        }
    }

	void SetCapDirection(cmCapDirection emDirect)
	{
		m_emCapDirection = emDirect;
	}
	cmCapDirection GetCapDirection()
	{
		return m_emCapDirection;
	}
    //设置打包时间
	void SetPackTime(u8 byPackTime)
	{
		m_byPackTime = byPackTime;
	}
    //获取打包时间
	u8  GetPackTime()
	{
		return m_byPackTime;
	}	
}TAudioCap;

//G7231的能力集
typedef struct tagG7231Cap
{
	cmCapDirection  m_emCapDirection;		//transmit or receive
	u8				m_byMaxFrameNum;		//单包最大的帧数
	BOOL			m_bSilenceSuppression;	//是否静音抑制
public:
	tagG7231Cap()
	{
		Clear();
	}

	void Clear()
	{
		m_emCapDirection = cmCapReceive;
		m_byMaxFrameNum  = 1;			    //目前我们只支持单帧
		m_bSilenceSuppression = TRUE;	    //我们默认支持静音抑制
	}

    void host2net(BOOL bhton)
    {
        s32 nCapDirection = m_emCapDirection;
        if (bhton)
        {
            m_emCapDirection        = (cmCapDirection)htonl(nCapDirection);
            m_bSilenceSuppression   = htonl(m_bSilenceSuppression);
        }
        else
        {
            m_emCapDirection        = (cmCapDirection)ntohl(nCapDirection);
            m_bSilenceSuppression   = ntohl(m_bSilenceSuppression);
        }
    }

	void SetCapDirection(cmCapDirection emDirect)
	{
		m_emCapDirection = emDirect;
	}
	cmCapDirection GetCapDirection()
	{
		return m_emCapDirection;
	}

    //设置是否支持静音
    void SetIsSilenceSuppression(BOOL bSilenceSuppression)
    {
        m_bSilenceSuppression = bSilenceSuppression;
    }
    //是否支持静音
	BOOL IsSilenceSuppression()
	{
		return m_bSilenceSuppression;
	}
    
    //设置每包的最大帧数
	void SetMaxFrameNumPerPack(u8 byMaxFrameNum)
	{
		m_byMaxFrameNum = byMaxFrameNum;
	}
    //获取每包的最大帧数
	u8 GetMaxFrameNumPerPack()
	{
		return m_byMaxFrameNum;
	}
}TG7231Cap;

//MPEG AAC能力集
typedef struct tagAACCap
{
    enum emAACSampleFreq                    //sampling frequency
    {
        emFs96000 = 0,
        emFs88200 = 1,
        emFs64000 = 2,
        emFs48000 = 3,
        emFs44100 = 4,
        emFs32000 = 5,
        emFs24000 = 6,
        emFs22050 = 7,
        emFs16000 = 8,
        emFs12000 = 9,
        emFs11025 = 10,
        emFs8000  = 11
    };
    
    enum emAACChnlCfg                       //channel config
    {
        emChnlCust   = 0,
        emChnl1      = 1,                   //单声道
        emChnl2      = 2,                   //双声道
        emChnl3      = 4,
        emChnl4      = 8,
        emChnl5      = 16,
        emChnl5dot1  = 32,                   //5.1声道
        emChnl7dot1  = 64                    //7.1声道
    };

	cmCapDirection  m_emCapDirection;		//transmit or receive    
    emAACSampleFreq m_emSampleFreq;
    emAACChnlCfg    m_emChnlCfg;
    u16             m_wBitrate;
    u8              m_byMaxFrameNum;

public:
    tagAACCap()
    {
        Clear();
    }
    
    void Clear()
    {
        m_emCapDirection = cmCapReceive;
        m_emSampleFreq = emFs32000;
        m_emChnlCfg = emChnlCust;
        m_byMaxFrameNum  = 1;			    //目前我们只支持单帧
        m_wBitrate = 96;                    //kbps
    }
    
    void host2net(BOOL bhton)
    {
        s32 nCapDirection = m_emCapDirection;
        if (bhton)
        {
            m_emCapDirection        = (cmCapDirection)htonl(nCapDirection);
            m_emSampleFreq          = (emAACSampleFreq)htonl(m_emSampleFreq);
            m_emChnlCfg             = (emAACChnlCfg)htonl(m_emChnlCfg);
            m_wBitrate              = htons(m_wBitrate);
        }
        else
        {
            m_emCapDirection        = (cmCapDirection)ntohl(nCapDirection);
            m_emSampleFreq          = (emAACSampleFreq)ntohl(m_emSampleFreq);
            m_emChnlCfg             = (emAACChnlCfg)ntohl(m_emChnlCfg);
            m_wBitrate              = ntohs(m_wBitrate);
        }
    }
    
    void SetCapDirection(cmCapDirection emDirect)
    {
        m_emCapDirection = emDirect;
    }
    cmCapDirection GetCapDirection() const
    {
        return m_emCapDirection;
	}

    void SetSampleFreq(emAACSampleFreq emFs)
    {
        m_emSampleFreq= emFs;
    }

    emAACSampleFreq GetSampleFreq() const
    {
        return m_emSampleFreq;
    }

    void SetChnl(emAACChnlCfg emChnl)
    {
        m_emChnlCfg = emChnl;
    }

    emAACChnlCfg GetChnl() const
    {
        return m_emChnlCfg;
    }

    void SetBitrate(u16 wBitrate) 
    {
        m_wBitrate = wBitrate;
    }

    u16 GetBitrate() const
    {
        return m_wBitrate;
    }
}TAACCap;

//数据能力集
typedef struct tagDataCap
{
	cmCapDirection  m_emCapDirection;				//transmit or receive
	u32             m_dwMaxBitRate;					//单位kbps; 
public:
	tagDataCap()
	{
		Clear();
	}

	void Clear()
	{
		m_dwMaxBitRate = 64;
		m_emCapDirection = cmCapReceiveAndTransmit;//数据信道
	}

    void host2net(BOOL bhton)
    {
        s32 nCapDirection = m_emCapDirection;
        if (bhton)
        {
            m_emCapDirection = (cmCapDirection)htonl(nCapDirection);
            m_dwMaxBitRate   = htonl(m_dwMaxBitRate);
        }
        else
        {
            m_emCapDirection = (cmCapDirection)ntohl(nCapDirection);
            m_dwMaxBitRate   = ntohl(m_dwMaxBitRate);
        }
    }

	void SetCapDirection(cmCapDirection emDirect)
	{
		m_emCapDirection = emDirect;
	}
	cmCapDirection GetCapDirection()
	{
		return m_emCapDirection;
	}

	//单位kbps
	void SetBitRate(u32 dwBitRate)
	{
		m_dwMaxBitRate = dwBitRate;		
	}
	u32 GetBitRate()
	{
        return m_dwMaxBitRate;
	}		
}TDataCap;

//发起H460呼叫参数
typedef struct tagH460CallParam 
{
	cmTransportAddress   m_tCallSignalAddr;
	u8                   m_abyCallId[LEN_GUID];      //CallIdentifier

	tagH460CallParam()
	{
		memset(&m_tCallSignalAddr, 0, sizeof(cmTransportAddress));
		memset(m_abyCallId, 0, LEN_GUID);
	}
}TH460CALLPARAM, *PTH460CALLPARAM;
//发起呼叫参数
typedef struct tagCallParam 
{
	cmConferenceGoalType  m_emConfGoalType;  				  //会议的目标类型
	cmCallType            m_emCallType;						  //呼叫类型 
	emEndpointType        m_emEndpointType;					  //终端类型  表示呼入终端的类型
	u32					  m_dwCallRate;						  //呼叫速率	(kbps)
    TAliasAddr			  m_tCalledAddr;					  //被叫地址。 发送时,可设置为别名地址，接收时为被叫信令地址
    TAliasAddr			  m_tCallingAddr;					  //主叫地址   主叫信令地址
	TAliasAddr			  m_atCallingAliasAddr[type_others+1];//主叫别名地址(别名地址+IP) ，缺省采用配置文件的设置
	TAliasAddr			  m_atCalledAliasAddr[type_others+1]; //被叫别名地址(别名地址+IP)
	u8					  m_abyConfId[LEN_GUID];		      //会议号,必须16 octet
	u8					  m_abyCallId[LEN_GUID];  
	s8					  m_achDisplay[LEN_DISPLAY_MAX+1];	  //主叫信息

    //UserInfo字段暂不使用
    s32					  m_nUUSize; 
	u8					  m_abyUserInfo[LEN_USERINFO_MAX];	  //UserInfo消息长度：2-131字节

	BOOL				  m_bIsEncrypt;						  //是否加密

    //代替原来的UserInfo字段
	s32             m_nNonStandLen;
	u8              m_abyNonStandData[LEN_NONSTANDARD_MAX];
    

    //2007.05.30 
	s32				      m_nProductIdSize;
	u8				      m_abyProductId[LEN_256];				//产品号
	s32				      m_nVerIdSize;
	u8				      m_abyVersionId[LEN_256];				//版本号

	emNATFWTraversal	  m_bySupportMediaTraversal;
	TH460Features		  m_tH460Features;
    
	tagCallParam()
	{
		Clear();	
		CreateConfId();
	}

	void Clear()
	{
		m_emConfGoalType = cmCreate;
		m_emCallType     = cmCallTypeP2P; 
		m_emEndpointType = emEndpointTypeUnknown;
		m_dwCallRate = 0;
		memset(m_atCallingAliasAddr,0,sizeof(m_atCallingAliasAddr));
		memset(m_atCalledAliasAddr,0,sizeof(m_atCalledAliasAddr));
		memset(&m_tCalledAddr, 0, sizeof(m_tCalledAddr));
		memset(&m_tCallingAddr, 0, sizeof(m_tCallingAddr));
		m_tCalledAddr.m_tNetAddr.m_wPort = CALLPORT;
		memset( m_abyConfId, 0, sizeof(m_abyConfId));		
		memset( m_abyCallId, 0, sizeof(m_abyCallId));
		memset( m_achDisplay,0, sizeof(m_achDisplay));
		memset( m_abyUserInfo,0,sizeof(m_abyUserInfo));
		m_nUUSize = 0;
		
		m_bIsEncrypt = FALSE;

		m_nProductIdSize = 0;
		m_nVerIdSize = 0;
        memset(m_abyProductId, 0, sizeof(m_abyProductId));
        memset(m_abyVersionId, 0, sizeof(m_abyVersionId));
		m_bySupportMediaTraversal = emNotSupportNATFWTraversal;
		memset( &m_tH460Features, 0, sizeof(m_tH460Features) );

		m_nNonStandLen = 0;
		memset( m_abyNonStandData, 0, LEN_NONSTANDARD_MAX );
	}

    //设置终端类型
    void SetEndpointType(emEndpointType emType)
    {
        m_emEndpointType = emType;
    }
    //得到终端类型
    emEndpointType GetEndpointType()
    {
        return m_emEndpointType;
    }
    

    //设置最大速率
    void SetCallRate( u32 dwCallRate )
    {
        m_dwCallRate = dwCallRate;
    }
    //得到最大速率
    u32 GetCallRate()
    {
        return m_dwCallRate;
    }
	//设置被叫地址
	void SetCalledAddr(u32 dwIp,u16 wPort = CALLPORT)
	{
		TNETADDR addr;
		addr.SetNetAddress( dwIp, wPort==0 ? CALLPORT : wPort );
		addr.SetIPAddr_Type( TYPE_IPV4 );
		m_tCalledAddr.SetIPCallAddr( &addr );
	}

//add by yj for ipv6
	//设置被叫地址(ipv6)
//	void SetCalledAddr_Ipv6(u8* dwIp_ipv6, s16 swScopeId,u16 wPort = CALLPORT)
	void SetCalledAddr_Ipv6(u8* dwIp_ipv6,u16 wPort = CALLPORT)
	{
		TNETADDR addr;
		addr.SetNetAddress_Ipv6( dwIp_ipv6, wPort==0 ? CALLPORT : wPort);
		addr.SetIPAddr_Type( TYPE_IPV6 );
		m_tCalledAddr.SetIPCallAddr( &addr );
	}
	//获得被叫IP(ipv6)
	u8* GetCalledIp_Ipv6()
	{
		return m_tCalledAddr.GetIPAddr_Ipv6(); 
	}
//	s16 GetCalledIp_Ipv6_ScopeId()
//	{
//		return m_tCalledAddr.GetIPAddr_Ipv6_ScopeId();
//	}
// TAliasAddr& GetCalledIp()
// {
//    return m_tCalledAddr;
// }
// TAliasAddr& GetCallingIp()
// {
// 	return m_tCallingAddr;
// }
//end
	//获得被叫IP
	u32 GetCalledIp()
	{
		return m_tCalledAddr.GetIPAddr(); 
	}	
	//获得被叫端口
	u16 GetCalledPort() 
	{ 
		return m_tCalledAddr.GetIPPort(); 
	}

	//设置被叫地址
	void SetCalledAddr(AliasType eType,const s8* sAlias)
	{
		TALIASADDR aliasAddr;
		aliasAddr.SetAliasAddr( eType, sAlias );
		m_tCalledAddr.SetIPCallAddr( &aliasAddr );
	}
    //得到被叫地址
    TAliasAddr& GetCalledAddr()
    {
        return m_tCalledAddr;
    }

	//设置主叫地址
	void SetCallingAddr( u32 dwIp,u16 wPort = 0 )
	{
		TNETADDR addr;
        //Jacky Wei Replace port 0 with default CALLPORT for Bug00144938(Intercommunication problem with HUAWEI EP)
		//addr.SetNetAddress( dwIp, wPort==CALLPORT? 0 : wPort);
        addr.SetNetAddress(dwIp, wPort == 0 ? CALLPORT : wPort);
		addr.SetIPAddr_Type( TYPE_IPV4 );
		m_tCallingAddr.SetIPCallAddr( &addr );
	}
	//获得主叫IP
	u32 GetCallingIp()
	{
		return m_tCallingAddr.GetIPAddr();	
	}	
	//获得主叫端口
	u16 GetCallingPort()
	{
		return m_tCallingAddr.GetIPPort();	
	}
    //设置主叫地址
    void SetCallingAddr(AliasType eType,const char* sAlias)
    {
        TALIASADDR aliasAddr;
        aliasAddr.SetAliasAddr( eType, sAlias );
        m_tCallingAddr.SetIPCallAddr( &aliasAddr );
    }

	//add by yj for ipv6
	//设置主叫地址(ipv6)
//	void SetCallingAddr_Ipv6( u8* dwIp_ipv6,s16 swScopeId,u16 wPort = 0 )
	void SetCallingAddr_Ipv6( u8* dwIp_ipv6,u16 wPort = 0 )
	{
		TNETADDR addr;
        //Jacky Wei Replace port 0 with default CALLPORT for Bug00144938(Intercommunication problem with HUAWEI EP)
        //addr.SetNetAddress_Ipv6( dwIp_ipv6, wPort == 0 ? CALLPORT : wPort);
		addr.SetNetAddress_Ipv6(dwIp_ipv6, wPort == 0 ? CALLPORT : wPort);
		addr.SetIPAddr_Type( TYPE_IPV6 );
		m_tCallingAddr.SetIPCallAddr( &addr );
	}
	//获得主叫IP(ipv6)
	u8* GetCallingIp_Ipv6()
	{
		return m_tCallingAddr.GetIPAddr_Ipv6(); 
	}
//	s16 GetCallingIp_Ipv6_ScopeId()
//	{
//		return m_tCalledAddr.GetIPAddr_Ipv6_ScopeId();
//	}
	//end

    //得到主叫地址
    TAliasAddr& GetCallingAddr()
    {
        return m_tCallingAddr;
    }   

	//设置主叫别名地址	作为源端的信息
	void SetCallingAliasaddr( TAliasAddr &tAliasAddr, u8 addrNo )
	{
		if ( addrNo < type_others+1  )
		{
			m_atCallingAliasAddr[addrNo] = tAliasAddr;
		}
	}
	//获取主叫别名地址
	PTAliasAddr GetCallingAliasaddr( u8 addrNo )
	{
		if ( addrNo < type_others+1  )
		{
			return &(m_atCallingAliasAddr[addrNo]);
		}
		return NULL;
	}
	
	//设置被叫别名地址	作为目的端的信息
	void SetCalledAliasAddr( TAliasAddr &tAliasAddr, u8 byAddrNo )
	{
		if ( byAddrNo < type_others+1  )
		{
			m_atCalledAliasAddr[byAddrNo] = tAliasAddr;
		}		
	}
	//获取被叫别名地址
	PTAliasAddr GetCalledAliasAddr( u8 byAddrNo )
	{
		if ( byAddrNo < type_others+1  )
		{
			return &(m_atCalledAliasAddr[byAddrNo]);
		}
		return NULL;
	}

	//设置会议号
	void SetConfId(const u8* pchConfId,u8 byLen)
	{
		if( pchConfId == NULL || byLen <= 0 )
		{
			return;
		}

		s32 nReal = min(max(LEN_1, byLen), LEN_GUID);
		memcpy(m_abyConfId, pchConfId, nReal);
	}
	//会议号长度
	s32 GetConfIdSize()
	{
		return LEN_GUID;
	};
	//获取会议号
	s32 GetConfId(u8* pbyConfId, s32 nLen)
	{
		if( pbyConfId == NULL || nLen <= 0 )
		{
			return 0;
		}

		s32 nReal = min(max(LEN_1, nLen), LEN_GUID);
		memcpy( pbyConfId, m_abyConfId, nReal );
		return nReal;
	}

	//生成一个会议号
	void CreateConfId()
	{
		memcpy(m_abyConfId, GetGUID(), LEN_GUID);
	}
	
	//生成一个Callidentifier
	void CreateCallId()
	{
		memcpy(m_abyCallId, GetGUID(), LEN_GUID);
	}
	//设置呼叫ID
	void SetCallId(const u8* pbyCallId, u8 byLen)
	{
		if( pbyCallId == NULL || byLen <= 0 )
		{
			return;
		}
		CreateCallId();
		s32 nReal = min(max(LEN_1, byLen), LEN_GUID);
		memcpy(m_abyCallId, pbyCallId, nReal);
	}
	//获取呼叫ID的长度
	s32 GetCallIdSize()
	{
		return LEN_GUID;
	};
	//获取呼叫ID
	s32 GetCallId(u8* pbyCallId, s32 nLen)
	{
		if(pbyCallId == NULL|| nLen <= 0 )
		{
			return 0;
		}
		
		s32 nReal = min(max(LEN_1, nLen), LEN_GUID);
		memcpy(pbyCallId, m_abyCallId, nReal);
		return nReal;
	}

	//设置显示信息
	void SetDisplayInfo(const s8* pchInfo)
	{
		if (pchInfo == NULL|| strlen(pchInfo) < LEN_DISPLAY_MIN)
		{
			return ;
		}
		strncpy( m_achDisplay, pchInfo, LEN_DISPLAY_MAX );
		m_achDisplay[LEN_DISPLAY_MAX] = 0;
	}
	//获得显示信息
	s8* GetDisplayInfo()
	{
		return m_achDisplay;
	}
	
	//设置UserInfo
	void SetUserInfo(const u8* pchUserInfo, u8 byLen)
	{
		if(pchUserInfo == NULL || byLen < LEN_USERINFO_MIN)
		{
			return ;
		}
	    u8 length = min(LEN_USERINFO_MAX, byLen);
		memcpy(m_abyUserInfo,pchUserInfo,length);
		m_nUUSize = length;
	}	
	//获得UserInfo长度
	s32 GetUserInfoSize()
	{
		return m_nUUSize;
	}
	//获取UserInfo
	s32 GetUserInfo(u8* pbyUserInfo, u8 byLen)
	{
		if(pbyUserInfo == NULL || byLen < 1)
		{
			return 0;
		}
        s32 nReal = min(byLen, m_nUUSize);
		memcpy(pbyUserInfo, m_abyUserInfo, nReal);
		return nReal;
	}	
	//设置加密
	void SetIsEncrypt( BOOL bIsEncrypt )
	{
		m_bIsEncrypt = bIsEncrypt;
	}
	//是否加密
	BOOL IsEncrypt()
	{
		return m_bIsEncrypt;
	}

	//对指针赋值，不能保存， 小心！！！！！！！！
	BOOL SetNonStandData(u8 *pBuf, s32 nLen)
	{
		if(nLen < 0 ||nLen > LEN_NONSTANDARD_MAX)
		{
			return  FALSE;
		}
		memcpy( m_abyNonStandData, pBuf, nLen );
		m_nNonStandLen = nLen;
	    return TRUE;
	}
    //获取指针，不能保存小心！！！！！！！！
	BOOL GetNonStandData(u8 **ppBuf, s32 *pnLen)
	{
		if(ppBuf == NULL|| pnLen == NULL)
		{
			return FALSE;
		}
		*ppBuf = m_abyNonStandData;
		*pnLen = m_nNonStandLen;
		return TRUE;
	}

	//设置产品号
	void SetProductId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyProductId, 0, sizeof(m_abyProductId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nProductIdSize = nReal;
        if(nLen >= LEN_1)
		{
			memcpy(m_abyProductId, pbyId, nReal);
		}
	}
	//获取产品号长度
	s32 GetProductIdSize()
	{
		return m_nProductIdSize;
	}
	//获取产品号
	s32 GetProductId(u8 *pbyId, s32 nLen)
	{
		if( pbyId == NULL || nLen <= 0 || m_nProductIdSize <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nProductIdSize); 
		memcpy(pbyId, m_abyProductId, nReal);
		return nReal;
	}
	//设置版本号	
	void SetVersionId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyVersionId, 0, sizeof(m_abyVersionId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nVerIdSize = nReal;
        if(nLen >= LEN_1)
		{
			memcpy(m_abyVersionId, pbyId, nReal);
		}
	}
	//获取版本号长度
	s32 GetVersionIdSize()
	{
		return m_nVerIdSize;
	}
	//获取版本号
	s32 GetVersionId(u8 *pbyId, s32 nLen)
	{
		if( pbyId == NULL || nLen <= 0 || m_nVerIdSize <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nVerIdSize); 
		memcpy(pbyId, m_abyVersionId, nReal);
		return nReal;
	}

	void SetSupportMediaTraversal(emNATFWTraversal emNet)
	{
		m_bySupportMediaTraversal = emNet;
	}
	emNATFWTraversal GetSupportMediaTraversal()
	{
		return m_bySupportMediaTraversal;
	}
	void SetH460features( const TH460Features &th460features )
	{
		memcpy( &m_tH460Features, &th460features, sizeof(TH460Features) );
	}
	void GetH460features( TH460Features &th460features )
	{
		memcpy( &th460features, &m_tH460Features, sizeof(TH460Features) );
	}

}TCALLPARAM,*PTCALLPARAM;


//会议列表
typedef struct tagConferences
{
	u8		   m_byNum;
	TCONFINFO  m_atConfInfo[MAX_CONF_NUM];
	
	tagConferences():m_byNum(0){}
	
	void Clear()
	{
		m_byNum = 0;
		memset( m_atConfInfo, 0, sizeof(m_atConfInfo) );
	}
	
	//设置会议数目
	void SetConfNum( u8 num)
	{
		m_byNum = num;
	}
	//获取会议数目
	u8 GetConfNum()
	{
		return m_byNum;
	}

	//设置会议信息
	void SetConfInfo( TCONFINFO &tConfInfo, u8 byConfNo )
	{
		if ( byConfNo >= MAX_CONF_NUM ) 
			return ;		
		m_atConfInfo[byConfNo] = tConfInfo;
	}
	//获取会议信息
	PTCONFINFO GetConfInfo( u8 byConfNo )
	{
		if ( byConfNo >= MAX_CONF_NUM ) 
			return  NULL;
		return &(m_atConfInfo[byConfNo]);
	}
}TCONFERENCES,*PTCONFERENCES;

//begin with added by wangxiaoyi for facility with non-strandard conflist message on 5 August 2010
typedef struct tagFacilityConfList_NSM
{
protected:
	u8   m_abyConfId[LEN_GUID+1];	//会议ID
	u16  m_wListIdx;				// 在TCONFERENCES结构中的存放位置索引，网络序

	//新增非标内容
	s8	 m_sConfE164[LEN_ALIAS+1];	// 会议E164号码
	u8	 m_byMtNum;					// 入会终端数
	u16  m_wRemainTime;				// 会议剩余时间, 单位是分钟
	u32  m_dwReserved1;
	u32  m_dwIsContinue;
	BOOL32         m_bConfListNonStandard;// whether contains non-standard information or not when facilityreason is conferenceListChoice

	////////
public:
	tagFacilityConfList_NSM()
	{
		Clear();
	}
	
	void Clear()
	{
		memset(m_abyConfId,0,sizeof(m_abyConfId));
		SetListIdx(0);
		SetConfE164( NULL, 0 );
		SetMtNum( 0 );
		SetRemainTime( 0 );
		SetConfListNonStandard( FALSE );
	}
	//设置会议ID
	void SetConfId( u8 *pbyConfId, u32 dwLen )
	{
		if ( pbyConfId == NULL )
			return;
		
		u32 realLen = min(dwLen, LEN_GUID);
		memcpy( m_abyConfId, pbyConfId, realLen );
		m_abyConfId[realLen] = '\0';
	}
	//获取会议ID
	u8* GetConfId()
	{
		return m_abyConfId;
	}
	//设置存放位置索引
	void SetListIdx( u16 wListIdx )
	{
		m_wListIdx = htons(wListIdx);
	}
	//获取存放位置索引
	u16 GetListIdx()
	{
		return ntohs(m_wListIdx);
	}
	// 设置会议E164号码
	void SetConfE164( s8 * psConfE164, u8 byLen )
	{
		memset( m_sConfE164, 0, sizeof(m_sConfE164) );
		if ( NULL == psConfE164 || 0 == byLen )
		{
			return;
		}
		u8 byRealLen = (u8)min( byLen, LEN_ALIAS );
		memcpy( m_sConfE164, psConfE164, byRealLen );
		return;
	}
	// 获取会议E164号码
	s8 * GetConfE164( void )
	{
		return m_sConfE164;
	}
	// 设置入会终端数
	void SetMtNum( u8 byMtNum )
	{
		m_byMtNum = byMtNum;
	}
	// 获取入会终端数
	u8   GetMtNum( void )
	{
		return m_byMtNum;
	}
	// 设置会议剩余时间
	void SetRemainTime( u16 wRemainTime )
	{
		m_wRemainTime = htons(wRemainTime);
	}
	// 获取会议剩余时间
	u16  GetRemainTime( void )
	{
		return ntohs(m_wRemainTime);
	}
	BOOL32 IsIdleConf(void)
	{
		return ( GetMtNum() == 0 ) ? TRUE : FALSE;
	}

    void SetContinueFlag( BOOL32 bIsContinue )
    {
        m_dwIsContinue = bIsContinue ? htonl(1) : htonl(0);
    }
    BOOL32 IsContinue( void )
    {
        return ( ntohl(m_dwIsContinue) == 1 ) ? TRUE : FALSE;
    }
	void SetConfListNonStandard( BOOL32 bConfListNonStandard )
	{
		m_bConfListNonStandard = htonl(bConfListNonStandard);
	}
	BOOL32 IsConfListNonStandard()
	{
		return ntohl(m_bConfListNonStandard);
	}
	void SetReserved1( u32 dwReserved )
	{
		m_dwReserved1 = htonl( dwReserved );
	}
	u32 GetReserved1( void )
	{
		return ntohl( m_dwReserved1 );
	} 
}
TFacilityConfList_NSM,*PTFaclilityConfList_NSM;
//end with added by wangxiaoyi for facility with non-strandard conflist message on 5 August 2010
//Facility参数
typedef struct tagFacilityUUIE
{
	u8			  m_byReason;					//FacilityReason
	TAliasAddr	  m_tAlternativeAddrAlias;
	TCONFERENCES  m_tConferences;

	u8			  m_abyCallId[LEN_GUID]; 
	BOOL          m_bExistCallId;
	
	//begin with added by wangxiaoyi for facility with non-strandard conflist message on 5 August 2010

	TFacilityConfList_NSM  m_atConfList_NSM[MAX_CONF_NUM];

	//end with added by wangxiaoyi for facility with non-strandard conflist message on 5 August 2010

	tagFacilityUUIE()
	{
		Clear();
	}

	void Clear()
	{
		m_byReason = unknown;
		m_tAlternativeAddrAlias.Clear();
		m_tConferences.Clear();
		m_bExistCallId = FALSE;
		memset(m_atConfList_NSM,0,MAX_CONF_NUM * sizeof(TFacilityConfList_NSM));
	}

	//设置TAliasAddr
	void SetAliasAddr( TAliasAddr &tAliasAddr )
	{
		m_tAlternativeAddrAlias = tAliasAddr;
	}
	//获取TAliasAddr
	TAliasAddr& GetAliasAddr()
	{
		return m_tAlternativeAddrAlias;
	}

	//设置TCONFERENCES
	void SetConferences( TCONFERENCES &tConf )
	{
		m_tConferences = tConf;
	}
	//获取TCONFERENCES
	TCONFERENCES& GetConferences()
	{
		return m_tConferences;
	}

	//获取呼叫ID
	s32 GetCallId(u8* pbyCallId, s32 nLen)
	{
		if(pbyCallId == NULL|| nLen <= 0 )
		{
			return 0;
		}
		
		s32 nReal = min(max(LEN_1, nLen), LEN_GUID);
		memcpy(pbyCallId, m_abyCallId, nReal);
		return nReal;
	}
	
	//设置呼叫ID
	void SetCallId(const u8* pbyCallId,u8 byLen)
	{
		if( pbyCallId == NULL || byLen <= 0 )
		{
			return;
		}
		m_bExistCallId = TRUE;
		
		s32 nReal = min(max(LEN_1, byLen), LEN_GUID);
		memcpy(m_abyCallId, pbyCallId, nReal);
	}

    void SetContinueFlag( BOOL32 bIsContinue )
    {
        m_atConfList_NSM[MAX_CONF_NUM-1].SetContinueFlag( bIsContinue );
    }
    BOOL32 IsContinue( void )
    {
        return m_atConfList_NSM[MAX_CONF_NUM-1].IsContinue();
    }

}TFACILITYINFO,*PTFACILITYINFO;

//加密同步结构  
typedef struct tagEncryptSync
{
	s32			  m_nSynchFlag;			         //shall be the Dynamic PayloadType for H.323
	u8			  m_abyH235Key[LEN_H235KEY_MAX]; //H.235 encoded value(1-65535)
	s32           m_nH235KeyLen;

    tagEncryptSync()
	{
		Reset();
	}

    void Reset()
	{
		m_nSynchFlag  = 0;
		m_nH235KeyLen = 0;
	}

    void host2net(BOOL bhton)
    {
        if (bhton)
        {
            m_nSynchFlag  = htonl(m_nSynchFlag);
            m_nH235KeyLen = htonl(m_nH235KeyLen);
        }
        else
        {
            m_nSynchFlag  = ntohl(m_nSynchFlag);
            m_nH235KeyLen = ntohl(m_nH235KeyLen);
        }
    }
    
    //设置SynchFlag
	void SetSynchFlag(s32 nFlag)
	{ 
		m_nSynchFlag = nFlag; 
	}
    //获取SynchFlag
	s32  GetSynchFlag() 
	{
		return m_nSynchFlag; 
	}

    //设置H235Key
	s32 SetH235Key(u8 *pBuf, s32 nLen)
	{
		m_nH235KeyLen = min(max(0, nLen), LEN_H235KEY_MAX);
		memcpy(m_abyH235Key, pBuf, m_nH235KeyLen);
		return m_nH235KeyLen;
	}	
    //获取H235Key的长度
	s32 GetH235KeyLen() 
	{
		return m_nH235KeyLen; 
	}
    //获取H235Key
	u8* GetH235Key()
	{
		return m_abyH235Key;
	}
}TEncryptSync;

//打开信道参数
typedef struct tagChannelParam 
{
	TNETADDR	  m_tRtp;				//rtp地址 ,incoming chan使用
	TNETADDR	  m_tRtcp;				//rtcp地址,incoming and outgoing chan使用
	u16			  m_wPayloadType;		//媒体通道类型	

	TVideoCap	  m_tVideoCap;
	TAudioCap	  m_tAudioCap;
	TDataCap	  m_tDataCap;
	TG7231Cap	  m_tG7231Cap;

	BOOL		  m_bFlowToZero;		//是否要求发送端在信道建立时不发送码流
	TTERLABEL	  m_tSrcTer;
	TTERLABEL	  m_tDstTer;

	u8			  m_byDynamicPT;		//动态载荷类型值(96~127)
	emEncryptType m_emEncryptType;		//加密类型
	TEncryptSync  m_tEncryptSync;       //master需设置此结构         

    //for H239
    BOOL          m_bIsH239DStream;     //在打开视频通道时指明是否H239协议的双流

    //for RSVP
    BOOL          m_bSupportRSVP;       //是否支持RSVP，双方都支持RSVP则设为TRUE
    
    // guzh [9/28/2007] 
    u8            m_byFECType;          //前向纠错类型。音视频通道可填

    // guzh [11/28/2007] 
    TH264VideoCap m_tH264Cap;           //H.264能力视频能力，与m_tVideoCap互斥，只需填写一个即可
    // guzh [2008/06/06]
    TAACCap       m_tAacCap;            //AAC LC/LD 音频能力
	
	emH264NALMode m_emH264NALMode;
	
		TNETADDR	  m_tKeepAliveAddr;		//460 server's rtp sending address for keep-alive channel added by wangxiaoyi 29 July 2010
	u16			  m_byKeepAlivePayload;	//打洞包载荷类型(0~127)
	u32			  m_dwKeepAliveInterval; //打洞包发送时间间隔

    void host2net(BOOL bhton)
    {
        m_tRtp.host2net(bhton);
        m_tRtcp.host2net(bhton);
		m_tKeepAliveAddr.host2net(bhton);//added by wangxiaoyi for h460 keepalivechannel
        m_wPayloadType = htons(m_wPayloadType);
        m_tVideoCap.host2net(bhton);
        m_tH264Cap.host2net(bhton);
        m_tAudioCap.host2net(bhton);
        m_tDataCap.host2net(bhton);
        m_tG7231Cap.host2net(bhton);
        m_tAacCap.host2net(bhton);
        m_bFlowToZero = htonl(m_bFlowToZero);
        // m_tSrcTer 
        // m_tDstTer
        // m_byDynamicPT
        m_emEncryptType = (emEncryptType)htonl(m_emEncryptType);
        m_tEncryptSync.host2net(bhton);
        m_bIsH239DStream = htonl(m_bIsH239DStream);
        m_bSupportRSVP = htonl(m_bSupportRSVP);

        // m_byFECType

		m_emH264NALMode  = (emH264NALMode)htonl((u32)m_emH264NALMode);

		m_byKeepAlivePayload = htons(m_byKeepAlivePayload);
		m_dwKeepAliveInterval = htonl(m_dwKeepAliveInterval);
    }

public:
	tagChannelParam()
	{
		Reset();
	};

	void Reset()
	{
		m_tRtp.Clear();
		m_tRtcp.Clear();
		m_tKeepAliveAddr.Clear();//added by wangxiaoyi for h460 keepalivechannel
		m_tSrcTer.Clear();
		m_tDstTer.Clear();
        m_wPayloadType = emPayloadTypeBegin;
		m_byDynamicPT   = 0;  
		m_bFlowToZero   = FALSE; 
		m_emEncryptType = emEncryptTypeNone;
		m_tEncryptSync.Reset();

        m_bIsH239DStream  = FALSE;
        m_bSupportRSVP    = FALSE;
    
        SetFECType(emFECNone);     
		m_emH264NALMode = emNALNode;

		m_byKeepAlivePayload = 0;
		m_dwKeepAliveInterval = 0;
	}

    //设置是否支持RSVP
    void SetSupportRSVP(BOOL bSupportRSVP) 
    {	
        m_bSupportRSVP = bSupportRSVP; 
    }
    BOOL IsSupportRSVP() 
    {
        return m_bSupportRSVP;
    }

    //设置FEC类型
    void SetFECType(u8 emType)
    {
        m_byFECType = emType;
    }
    u8 GetFECType() const
    {
        return m_byFECType;
    }
	
    //设置RTP参数
	void SetRtp(TNETADDR& rtp) 
	{ 
		m_tRtp = rtp; 
	}
    //得到RTP参数
	TNETADDR& GetRtp() 
	{
		return m_tRtp; 
	};
	//设置KeepAliveAddr参数added by wangxiaoyi for h460 keepalivechannel
	void SetKeepAliveAddr(TNETADDR& keepaliveaddr) 
	{ 
		m_tKeepAliveAddr = keepaliveaddr; 
	}
    //得到KeepAliveAddr参数added by wangxiaoyi for h460 keepalivechannel
	TNETADDR& GetKeepAliveAddr() 
	{
		return m_tKeepAliveAddr; 
	};

    //设置RTCP参数
	void SetRtcp(TNETADDR& rtcp) 
	{
		m_tRtcp = rtcp; 
	}
    //得到RTCP参数
	TNETADDR& GetRtcp() 
	{
		return m_tRtcp; 
	}

    //设置FlowToZero
	void SetIsFlowToZero(BOOL bFlowToZero) 
	{	
		m_bFlowToZero = bFlowToZero; 
	}
	BOOL IsFlowToZero() 
	{
		return m_bFlowToZero;
	}
	
    //设置加密类型
    void SetEncryptType(emEncryptType emType)
	{
		m_emEncryptType = emType; 
	}
    //得到加密类型
	emEncryptType GetEncryptType()
	{
		return m_emEncryptType;
	}

    //设置源端TTERLABEL
	void SetSrcTerLabel( TTERLABEL& tTer) 
	{ 
		m_tSrcTer = tTer;
	}
    //得到源端TTERLABEL
    TTERLABEL& GetSrcTerLabel() 
	{ 
		return m_tSrcTer; 
	}

    //设置目的端TTERLABEL
	void SetDstTerLabel( TTERLABEL& tTer) 
	{ 
		m_tDstTer = tTer;
	}
    //得到目的端TTERLABEL
    TTERLABEL& GetDstTerLabel() 
	{ 
		return m_tDstTer; 
	}

    //设置动态载荷类型
	BOOL SetDynamicPayloadParam( u8 byDynamicPT )
	{
		if ( byDynamicPT>=96 && byDynamicPT<=127 ) 
		{
			m_byDynamicPT = byDynamicPT;
			return TRUE;
		}
		return FALSE;
	}
	u8 GetDynamicPayloadParam()
	{
	    return m_byDynamicPT;
	} 
    //是否动态载荷类型 
    BOOL IsDynamicPayload()	
    { 
        return (m_byDynamicPT >= 96)&&(m_byDynamicPT <= 127); 
    }

    //得到载荷类型
    u16 GetPayloadType()
    {
        return m_wPayloadType; 
    }

    //设置视频参数，不包括H.264
	BOOL SetPayloadParam(u16 wPayloadType, TVideoCap &tVideoCap)
	{
		if ( !PayloardIsVideoType(wPayloadType) || wPayloadType == v_h264 )
		{
            // guzh [11/28/2007] critical error!
            OspPrintf(1, 0, "[H323Adp][TCHANPARAM::SetPayloadParam] Payload param(%d) error! TVideoCap\n", wPayloadType);
			return FALSE;
		}
		m_tVideoCap = tVideoCap;
		m_wPayloadType = wPayloadType;
		return TRUE;
	}
    //得到视频参数
    BOOL GetPayloadParam(u16 wPayloadType, TVideoCap *ptVideoCap)
    {
        if( !PayloardIsVideoType(wPayloadType) || wPayloadType == v_h264 || ptVideoCap == NULL )
        {
            // guzh [11/28/2007] critical error!
            OspPrintf(1, 0, "[H323Adp][TCHANPARAM::GetPayloadParam] Payload param(%d) error! TVideoCap\n", wPayloadType);
            return FALSE;
        }
        *ptVideoCap = m_tVideoCap;
        return TRUE;
    }
    //设置H.264视频参数
    BOOL SetPayloadParam(u16 wPayloadType, TH264VideoCap &tVideoCap)
    {
        if ( wPayloadType != v_h264 )
        {
			OspPrintf(1, 0, "[H323Adp][TCHANPARAM::SetPayloadParam] Payload param(%d) error! TH264VideoCap\n", wPayloadType);
			return FALSE;
        }
        m_tH264Cap = tVideoCap;
        m_wPayloadType = wPayloadType;
        return TRUE;
    }
    //得到H.264视频参数
    BOOL GetPayloadParam(u16 wPayloadType, TH264VideoCap *ptVideoCap)
    {        
        if( wPayloadType != v_h264 || ptVideoCap == NULL )
        {
			OspPrintf(1, 0, "[H323Adp][TCHANPARAM::GetPayloadParam] Payload param(%d) error! TH264VideoCap\n", wPayloadType);
            return FALSE;
        }
        *ptVideoCap = m_tH264Cap;
        return TRUE;
    }

    //设置音频参数，不包括G7231
	BOOL SetPayloadParam(u16 wPayloadType, TAudioCap &tAudioCap)
	{
		if ( !PayloardIsAudioType(wPayloadType) || 
             wPayloadType == a_g7231 ||
             wPayloadType == a_mpegaaclc || 
             wPayloadType == a_mpegaacld )
		{
            OspPrintf(1, 0, "[H323Adp][TCHANPARAM::SetPayloadParam] Payload param(%d) error! TAudioCap\n", wPayloadType);
			return FALSE;
		}
		m_tAudioCap = tAudioCap;
		m_wPayloadType = wPayloadType;
		return TRUE;
	}
    //得到音频参数，不包括G7231/AAC
    BOOL GetPayloadParam(u16 wPayloadType, TAudioCap *ptAudioCap)
    {
        if (!PayloardIsAudioType(wPayloadType) || 
            wPayloadType == a_g7231 || 
            wPayloadType == a_mpegaaclc || 
            wPayloadType == a_mpegaacld || 
            ptAudioCap == NULL) 
        {
            OspPrintf(1, 0, "[H323Adp][TCHANPARAM::GetPayloadParam] Payload param(%d) error! TAudioCap\n", wPayloadType);
            return FALSE;
        }
        *ptAudioCap = m_tAudioCap;
        return TRUE;
    }

    //设置G7231参数
    BOOL SetPayloadParam(u16 wPayloadType, TG7231Cap &tG7231Cap)
    {
        if(wPayloadType != a_g7231)
        {
			OspPrintf(1, 0, "[H323Adp][TCHANPARAM::SetPayloadParam] Payload param(%d) error! TG7231Cap\n", wPayloadType);
            return FALSE;
        }
        m_tG7231Cap = tG7231Cap;
        m_wPayloadType = wPayloadType;
        return TRUE;
    }
    //得到G7231参数
    BOOL GetPayloadParam(u16 wPayloadType, TG7231Cap *ptG7231Cap)
    {
        if(wPayloadType  != a_g7231|| ptG7231Cap == NULL )
        {
			OspPrintf(1, 0, "[H323Adp][TCHANPARAM::GetPayloadParam] Payload param(%d) error! TG7231Cap\n", wPayloadType);
            return FALSE;
        }
        *ptG7231Cap = m_tG7231Cap;
        return TRUE;
    }
    //设置AAC参数
    BOOL SetPayloadParam(u16 wPayloadType, TAACCap &tAacCap)
    {
        if (wPayloadType != a_mpegaaclc && wPayloadType  != a_mpegaacld)
        {
			OspPrintf(1, 0, "[H323Adp][TCHANPARAM::SetPayloadParam] Payload param(%d) error! TAACCap\n", wPayloadType);
            return FALSE;
        }
        m_tAacCap = tAacCap;
        m_wPayloadType = wPayloadType;
        return TRUE;
    }
    //得到AAC参数
    BOOL GetPayloadParam(u16 wPayloadType, TAACCap *ptAacCap)
    {
        if((wPayloadType != a_mpegaaclc && wPayloadType  != a_mpegaacld) || ptAacCap == NULL )
        {
			OspPrintf(1, 0, "[H323Adp][TCHANPARAM::GetPayloadParam] Payload param(%d) error! TAACCap\n", wPayloadType);
            return FALSE;
        }
        *ptAacCap = m_tAacCap;
        return TRUE;
    }

    //设置数据参数
	BOOL SetPayloadParam(u16 wPayloadType, TDataCap &tDataCap)
	{
		if (!PayloardIsDataType(wPayloadType))
		{
			OspPrintf(1, 0, "[H323Adp][TCHANPARAM::SetPayloadParam] Payload param(%d) error! TDataCap\n", wPayloadType);
            return FALSE;
		}
		m_tDataCap = tDataCap;
		m_wPayloadType = wPayloadType;
		return TRUE;
	}
    //得到数据参数
    BOOL GetPayloadParam(u16 wPayloadType, TDataCap *ptDataCap)
    {
        if(!PayloardIsDataType(wPayloadType) || ptDataCap == NULL)
        {
			OspPrintf(1, 0, "[H323Adp][TCHANPARAM::GetPayloadParam] Payload param(%d) error! TDataCap\n", wPayloadType);
            return FALSE;
        }
        *ptDataCap = m_tDataCap;
        return TRUE;
    }

    //设置加密同步信息
	void SetEncryptSync( TEncryptSync &tSync)
	{ 
		m_tEncryptSync = tSync; 
	}
    //得到加密同步信息
	TEncryptSync &GetEncryptSync() 
	{ 
		return m_tEncryptSync; 
	} 

	void SetH264NALMode(emH264NALMode emNALMode)
	{
		m_emH264NALMode = emNALMode;
	}
	
	emH264NALMode GetH264NALMode()
	{
		return m_emH264NALMode;
	}

	void SetKeepAlivePayload(u16 payload)
	{
		m_byKeepAlivePayload = payload;
	}
	u16 GetKeepAlivePayload()
	{
		return m_byKeepAlivePayload;
	}

	void SetKeepAliveInterval(u32 time)
	{
		m_dwKeepAliveInterval = time;
	}
	u32 GetKeepAliveInterval()
	{
		return m_dwKeepAliveInterval;
	}
	
}TCHANPARAM,*PTCHANPARAM;

typedef struct tagKeepAliveInfo
{
	TNETADDR	m_tKeepAliveNet;
	u16			m_wKeepAlivePayload;
	u32			m_dwKeepAliveInterval;

	tagKeepAliveInfo()
	{
		Clear();
	}
	void Clear()
	{
		m_tKeepAliveNet.Clear();
		m_wKeepAlivePayload = 0;
		m_dwKeepAliveInterval = 0;
	}
}TKeepAliveInfo, *PTKeepAliveInfo;
//终端信息参数
typedef struct tagTerminalInformation 
{
	TTERLABEL	m_tTerLabel;
	s8			m_achTerID[LEN_TERID + 1];

	tagTerminalInformation()
	{
		Clear();
	}

	void Clear()
	{
		m_tTerLabel.Clear();
        memset( m_achTerID, 0, sizeof(m_achTerID) );        
	}	
	
	//设置终端编号
	void SetLabel(const PTTERLABEL ptLabel)
	{
		if (ptLabel != NULL)
		{
			memcpy( &m_tTerLabel, ptLabel, sizeof(m_tTerLabel) );
		}		
	}
	void SetLabel(u8 byMcuNo, u8 byTerNo)
	{
		if (byMcuNo < 193 && byTerNo <193)
		{
			m_tTerLabel.SetTerminalLabel(byMcuNo, byTerNo);
		}
	}
	//获得终端编号
	TTERLABEL& GetTerlabel()
	{
		return m_tTerLabel;
	}
	void GetLabel(u8& byMcuNo, u8& byTerNo)
	{
		m_tTerLabel.GetTerminalLabel( &byMcuNo, &byTerNo );
	}
	//获得Mcu号
	u8 GetMcuNo() 
	{ 
		return m_tTerLabel.GetMcuNo();	
	}
	//获得终端号
	u8 GetTerNo() 
	{ 
		return m_tTerLabel.GetTerNo();	
	}

	//设置终端名字
	BOOL SetName(const s8* pchName)
	{
		if (pchName != NULL)
		{
			strncpy( m_achTerID, pchName, LEN_TERID + 1 );
			m_achTerID[LEN_TERID] = '\0';
			return TRUE;
		}
		return FALSE;
	}
	//获得终端名字
	s8* GetName()
	{
		return m_achTerID;
	}

	void operator = (tagTerminalInformation& tTerInfo)
	{
		m_tTerLabel.SetTerminalLabel( tTerInfo.m_tTerLabel.GetMcuNo(), 
									  tTerInfo.m_tTerLabel.GetTerNo() );

		strncpy( m_achTerID,tTerInfo.GetName(), LEN_TERID + 1);
	}

	bool operator != (tagTerminalInformation& tTerInfo)
	{
		if ( m_tTerLabel.GetMcuNo() == tTerInfo.m_tTerLabel.GetMcuNo() && 
			 m_tTerLabel.GetTerNo() == tTerInfo.m_tTerLabel.GetTerNo() &&
			 strcmp( m_achTerID, tTerInfo.m_achTerID ) == 0 )
		{
			return false;
		}
		return true;
	}

	bool operator == (tagTerminalInformation& tTerInfo)
	{
		if ( m_tTerLabel.GetMcuNo() == tTerInfo.m_tTerLabel.GetMcuNo() && 
			 m_tTerLabel.GetTerNo() == tTerInfo.m_tTerLabel.GetTerNo() &&
			 strcmp( m_achTerID, tTerInfo.m_achTerID ) == 0 )
		{
			return true;
		}
		return false;
	}
}TTERINFO,*PTTERINFO;

//通讯模式参数
typedef struct tagCommunicationModeCommandInfo
{
	u8				m_bySessionID;
	u8				m_byAssociatedSessionID;
	TTERLABEL		m_tTerLabel;							//如果为<255,255>表示所有终端
	u8				m_byDataType;							//type_audio/type_video/type_data
	u16				m_wPayloadType;							//媒体体类型
	TNETADDR		m_tRtp;									//rtp地址		
	TNETADDR		m_tRtcp;								//rtcp地址
	TTERLABEL		m_tDestination;							//目的终端,<0,0>表示无效
	u8				m_bySessionDescriptLen;
	s8				m_achSessionDescript[MAXSessionDescript+1];
					
	tagCommunicationModeCommandInfo()
	{
		Clear();
	}
	
	void Clear()
	{
		m_bySessionID = 0;
		m_byAssociatedSessionID = 0;
		m_tTerLabel.Clear();
		m_byDataType = 0;
		m_wPayloadType = 0;
		m_tRtp.Clear();
		m_tRtcp.Clear();
		m_tDestination.Clear();
		m_bySessionDescriptLen = 0;
		memset( m_achSessionDescript, 0, sizeof(m_achSessionDescript) );
	}

    //设置SessionID
	void SetSessionID( u8 bySessionID)
	{
		m_bySessionID = bySessionID;
	}
    //得到SessionID
	u8 GetSessionID()
	{
		return m_bySessionID;
	}

    //设置AssociatedSessionID
	void SetAssociatedSessionID( u8 byAssociatedSessionID )
	{
		m_byAssociatedSessionID = byAssociatedSessionID;
	}
    //得到AssociatedSessionID
	u8 GetAssociatedSessionID()
	{
		return m_byAssociatedSessionID;
	}

    //设置TTERLABEL
	void SetTerLabel( TTERLABEL &tTerLabel)
	{
		m_tTerLabel = tTerLabel;
	}
    //得到TTERLABEL
	TTERLABEL& GetTerLabel()
	{
		return m_tTerLabel;
	}

    //设置数据类型
	void SetDataType( u8 byDataType)
	{
		m_byDataType = byDataType;
	}
    //得到数据类型
	u8 GetDataType()
	{
		return m_byDataType;
	}

    //设置媒体类型
	void SetPayloadType( u16 wPayloadType)
	{
		m_wPayloadType = wPayloadType;
	}
    //得到媒体类型
	u16 GetPayloadType()
	{
		return m_wPayloadType;
	}

    //设置RTP地址
	void SetRTPAddr( TNETADDR &tRtp )
	{
		m_tRtp = tRtp;
	}
    //得到RTP地址
	TNETADDR& GetRTPAddr()
	{
		return m_tRtp;
	}

    //设置RTCP地址
	void SetRTCPAddr( TNETADDR &tRtcp )
	{
		m_tRtcp = tRtcp;
	}
    //得到RTCP地址
	TNETADDR& GetRTCPAddr()
	{
		return m_tRtcp;
	}

    //设置目的端TTERLABEL
	void SetDestTerLabel( TTERLABEL &tTerLabel)
	{
		m_tDestination = tTerLabel;
	}
    //得到目的端TTERLABEL
	TTERLABEL& GetDestTerLabel()
	{
		return m_tDestination;
	} 
	
    //设置SessionDescript
	void SetSessionDescript( s8 *pSessionDescript, u8 byLen )
	{
		m_bySessionDescriptLen = min( byLen, MAXSessionDescript );
		strncpy( m_achSessionDescript, pSessionDescript, m_bySessionDescriptLen );
		m_achSessionDescript[m_bySessionDescriptLen] = '\0';
	}
    //得到SessionDescript
	s8* GetSessionDescript()
	{
		return m_achSessionDescript;
	}
    //得到SessionDescript长度
	u16 GetSessionDescriptLen()
	{
		return m_bySessionDescriptLen;
	}
}TCOMMODEINFO,*PTCOMMODEINFO;

u16 GetEType( IN u16 wType, IN u8 byEBits);
u16 GetH239Type( IN u16 wType );
BOOL IsH239Type( IN u16 wType );
/************************************
* 能力集结构
* 目前的视频支持:h261,h263,h264,mpeg4,h263+,h262
* 目前的音频支持:G711u(64k),G711a(64k),G722(64k),G7231,G728,G729,G7221C,MP3,MP2AAC,MP4AAC.
* 
* guzh [12/17/2007] 调整结构
* guzh [04/03/2008] 调整结构
 *************************************/
typedef struct tagCapSet
{
private:
	u16 m_awCapTable[MAX_CAPTABLE_SIZE]; //PayloadType array
	u16 m_aaawCapDesc[MAX_DESCAP_NUM][MAX_SIMUCAP_NUM][MAX_ALTCAP_NUM]; //PayloadType array
    //Video
	TVideoCap       m_tH261Cap;
	TVideoCap       m_tH263Cap;
	TVideoCap       m_tH263plusCap;
	TVideoCap       m_tMpeg4Cap;
    TVideoCap       m_tH262Cap;
    //Audio
	TAudioCap       m_tG711u;   
	TAudioCap       m_tG711a;   
	TAudioCap       m_tG722;    
	TAudioCap       m_tG728;	   
	TAudioCap       m_tG729;
	TAudioCap       m_tMP3;
    TAudioCap       m_tG7221c;
    TAACCap         m_tMpegAacLc;
    TAACCap         m_tMpegAacLd;		
	TG7231Cap       m_tG7231;
    //Data
	TDataCap        m_tT120;
	TDataCap        m_tH224;

    /*载荷类型的内部转换:
    |<--------------加密和H239的载荷转换---------- >|<低字节为上层使用的类型>|
    ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    +  0  +  0  +  0  +  0  +  1  +  1  +  1  +  1  +  payloadType           +             
    ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            + H239+ AES + DES +NoEnc+   
    */
	u8              m_byEncryptBits;	        //enum emEncryptType
    BOOL            m_bIsSupportH239;         //支持H.239的类型
    BOOL            m_bSupportRSVP;           //是否支持RSVP功能
	BOOL            m_bSupportActiveH263Plus; //是否支持H.263+
    u8              m_byFECBits;              //前向纠错

    //guzh [2008/04/03] H.264 能力
    TH264VideoCap   m_atH264Cap[MAX_H264CAP_NUM];
	u8              m_awH264ToSubId[MAX_CAPTABLE_SIZE]; //H264 payload id 到 sub id 映射表
	u8              m_bySubId;
	emH264NALMode   m_emH264NALMode;

	TAudioCap       m_tG719;   //ruiyigen 20091012 Add At Rear
	
public:
	tagCapSet()
	{
		Clear();
	}

	void Clear()
	{
		memset( m_awCapTable, 0, sizeof(m_awCapTable) ); 
		memset( m_aaawCapDesc, 0xFF, sizeof(m_aaawCapDesc) ); 
		memset(m_awH264ToSubId, 0xFF, sizeof(m_awH264ToSubId));

		m_tH261Cap.Clear();
		m_tH263Cap.Clear();
		m_tH263plusCap.Clear();
		for (u8 byLoop = 0; byLoop < MAX_H264CAP_NUM; byLoop++)
        {
            m_atH264Cap[byLoop].Clear();
        }
		m_tMpeg4Cap.Clear();
        m_tH262Cap.Clear();
	
		m_tG711u.Clear();   
		m_tG711a.Clear();   
		m_tG722.Clear();    
		m_tG728.Clear();	   
		m_tG729.Clear();
		m_tMP3.Clear();
        m_tG7221c.Clear();
		m_tG719.Clear();
        m_tMpegAacLc.Clear();
        m_tMpegAacLd.Clear();
		
		m_tG7231.Clear();
		
		m_tT120.Clear();
		m_tH224.Clear();

        SetEncryptBits(emEncryptTypeNone);

        m_bIsSupportH239        = FALSE;
        m_bSupportRSVP          = FALSE;
		m_bSupportActiveH263Plus= FALSE;

        SetFECType((u8)emFECNone);

		m_bySubId               = 0;
		m_emH264NALMode         = emNALSignal;
	}

    void host2net(BOOL bhton)
    {
        for(s32 nIndex = 0; nIndex<MAX_CAPTABLE_SIZE; nIndex++)
        {
            if (m_awCapTable[nIndex] == 0)
                break;
            
            if (bhton) 
            {
                m_awCapTable[nIndex] = htons(m_awCapTable[nIndex]);
            }
            else
            {
                m_awCapTable[nIndex] = ntohs(m_awCapTable[nIndex]);
            }
        }
        
        s32 nDesCap = 0;
        s32 nSimuCap = 0;
        s32 nAltCap = 0;
        for(nDesCap = 0; nDesCap < MAX_DESCAP_NUM; nDesCap++)
        {
            for(nSimuCap = 0; nSimuCap < MAX_SIMUCAP_NUM; nSimuCap++)
            {
                for(nAltCap = 0; nAltCap < MAX_ALTCAP_NUM; nAltCap++)
                {
                    if (bhton)
                    {
                        m_aaawCapDesc[nDesCap][nSimuCap][nAltCap] 
                            = htons(m_aaawCapDesc[nDesCap][nSimuCap][nAltCap]);
                    }
                    else
                    {
                        m_aaawCapDesc[nDesCap][nSimuCap][nAltCap] 
                            = ntohs(m_aaawCapDesc[nDesCap][nSimuCap][nAltCap]);
                    }
                }
            }
        }

        m_tH261Cap.host2net(bhton);
        m_tH263Cap.host2net(bhton);
        m_tH263plusCap.host2net(bhton);
        for (u8 byLoop = 0; byLoop < MAX_H264CAP_NUM; byLoop++)
        {
            m_atH264Cap[byLoop].host2net(bhton);
        }

        m_tMpeg4Cap.host2net(bhton);
        m_tH262Cap.host2net(bhton);

        m_tG711u.host2net(bhton);
        m_tG711a.host2net(bhton);
        m_tG722.host2net(bhton);
        m_tG728.host2net(bhton);
        m_tG729.host2net(bhton);
        m_tMP3.host2net(bhton);
        m_tG7221c.host2net(bhton);  
		m_tG719.host2net(bhton);
        m_tG7231.host2net(bhton);
        m_tMpegAacLc.host2net(bhton);
        m_tMpegAacLd.host2net(bhton);

        m_tT120.host2net(bhton);
        m_tH224.host2net(bhton);

        m_bIsSupportH239 = htonl(m_bIsSupportH239);
        m_bSupportRSVP   = htonl(m_bSupportRSVP);    
		m_emH264NALMode  = (emH264NALMode)htonl((u32)m_emH264NALMode);
    }

    //设置支持RSVP功能
    void SetSupportRSVP(BOOL bSupportRSVP)
    {
        m_bSupportRSVP = bSupportRSVP;
    }
    //是否支持RSVP
    BOOL IsSupportRSVP()
    {
        return m_bSupportRSVP;
    }

	//设置支持RSVP功能
    void SetSupportActiveH263(BOOL bSupport)
    {
        m_bSupportActiveH263Plus = bSupport;
    }
    //是否支持RSVP
    BOOL IsSupportActiveH263()
    {
        return m_bSupportActiveH263Plus;
    }
    //设置前向纠错类型
    void SetFECType(u8 emType)
    {
        m_byFECBits = emType;
    }
    //获取前向纠错类型
    u8 GetFECType() const
    {
        return m_byFECBits;
    }
    /*上层暂时不使用*/
    void SetSupportH239(BOOL bSupporth239)
    {
        m_bIsSupportH239 = bSupporth239;
    }
    BOOL IsSupportH239()
    {
        return m_bIsSupportH239;
    }
    //
    //设置能力集。如果是H.264，则需额外传入SetH264VideoCap时对应的数组下标
	BOOL SetCapDesc(u8 byCapIndex, u8 bySimuIndex, u8 byAltIndex, u16 wPayload, u8 byH264SubId = 0)
	{
		u16 wRealPt = wPayload&0xff;

		if( byCapIndex >= MAX_DESCAP_NUM || bySimuIndex >= MAX_SIMUCAP_NUM
			|| byAltIndex >= MAX_ALTCAP_NUM )
		{
			return FALSE;
		}

        if ( wRealPt == v_h264 && byH264SubId >= MAX_H264CAP_NUM )
        {
            return FALSE;
        }

		u16 byCapTableId = AddCapToTable(wPayload, byH264SubId);
		m_aaawCapDesc[byCapIndex][bySimuIndex][byAltIndex] = byCapTableId;
		if((wPayload&0xff) == v_h264)
		{
			m_awH264ToSubId[byCapTableId] = byH264SubId;
		}

        return TRUE;
	}
    //得到能力级    
	u16 GetCapDescId(u8 byCapIndex, u8 bySimuIndex, u8 byAltIndex)
	{
		if(byCapIndex >= MAX_DESCAP_NUM || bySimuIndex >= MAX_SIMUCAP_NUM
			|| byAltIndex >= MAX_ALTCAP_NUM )
		{
            //guzh 2008/06/02
			return 0xFFFF;
		}
		return m_aaawCapDesc[byCapIndex][bySimuIndex][byAltIndex];		
	}
    
    //同时能力集的判断,pwPayload全部能力集,nNum能力集个数,ptH264Cap全部H264能力[in/out]
	BOOL IsExistsimultaneous(u16 *pwPayload, s32 nNum, TH264VideoCap* ptH264Cap=NULL, TAACCap *ptAacCap = NULL )
	{
		//如果H264对TH264按能力排序
		TH264VideoCap tH264;
		s32 nH264Num=0;
		s32 anH264Input[MAX_H264CAP_NUM];
        s32 i;
		if(ptH264Cap != NULL)
		{
			memset(anH264Input, 0xFF, MAX_H264CAP_NUM);
			for(i=0; i<nNum; i++)
			{
				if((pwPayload[i]&0xff) == v_h264)
				{
					nH264Num++;
				}
			}

// 			for(i=0; i<nH264Num; i++)
// 			{
// 				anH264Input[i] = i;
// 			}
// 
// 			for(i=0; i<nH264Num-1; i++)
// 			{
// 				for(s32 j=i+1; j<nH264Num; j++)
// 				{
// 					if(ptH264Cap[j] <= ptH264Cap[i])
// 					{
// 						tH264 = ptH264Cap[j];
// 						ptH264Cap[j] = ptH264Cap[i];
// 						ptH264Cap[i] = tH264;
// 
// 						temp = anH264Input[j];
//                         anH264Input[j] = anH264Input[i];
// 						anH264Input[i] = temp;
// 					}
// 				}
// 			}
		}
		
        //////////////////////////////////////////////////////////////////////////
        //Jacky Wei Print all inner the capabilities
        /*
        s32  nCapIndex  = 0, nSimuIndex=0, nAltIndex=0;
        OspPrintf(1, 0, "Inner capabilities:");   
        for (nCapIndex = 0; nCapIndex < MAX_DESCAP_NUM; nCapIndex++)
        {
            for(nSimuIndex=0; nSimuIndex < MAX_SIMUCAP_NUM; nSimuIndex++)
            {
                for(nAltIndex = 0; nAltIndex < MAX_ALTCAP_NUM; nAltIndex++)
                {
                    u16 wCapTableId = GetCapDescId(nCapIndex, nSimuIndex, nAltIndex);
                    if ( wCapTableId == 0xFFFF )
                        break;
                    u16 wPayloadType = m_awCapTable[wCapTableId];
                    if(wPayloadType == 0) 
                        break;
                    OspPrintf(1, 0, "    Payload:%d, (%d:%d:%d)\n", wPayloadType, nCapIndex, nSimuIndex, nAltIndex);
                }
            }
        }
        */
        //////////////////////////////////////////////////////////////////////////

        u16 awInterPayload[MAX_SIMUCAP_NUM]; //保存外部载荷类型的数组
        memset(awInterPayload, 0, sizeof(awInterPayload));
        for(s32 nInterIndex = 0; nInterIndex < nNum; nInterIndex++)
        {
            awInterPayload[nInterIndex] = pwPayload[nInterIndex];
        }

        BOOL bRes = FALSE;
        for(s32 nH263Num = 0; nH263Num < 3; nH263Num++)
        {
            bRes = IsInterExistsimultaneous(awInterPayload, nNum, ptH264Cap, nH264Num, ptAacCap );
			//IsInterExistsimultaneous查表有bug，没有239时可能先把第二路格式在第一路里找到
			//所以需要反过来再查找一次以确保最终结果 ruiyigen 091126
// 			if ( !bRes )
// 			{
// 				u16 awInterPayload2[MAX_SIMUCAP_NUM] = { 0 };
// 				int n = 0;
// 				for ( n = 0; n < nNum; n++ )
// 				{
// 					awInterPayload2[n] = m_awInterPayload[nNum - n - 1];					
// 				}
// 				bRes = IsInterExistsimultaneous(awInterPayload2, nNum, ptH264Cap, nH264Num);
// 			}
// 
// 				if(ptH264Cap != NULL)
// 				{
// 					TH264VideoCap atH264VideoCap[MAX_H264CAP_NUM];
// 					s32 i;
// 					for(i=0; i<nH264Num; i++)
// 					{
// 						atH264VideoCap[anH264Input[i]] = ptH264Cap[i];
// 					}
// 
// 					for(i=0; i<nH264Num; i++)
// 					{
// 						 ptH264Cap[i] = atH264VideoCap[i];
// 					}
// 				}
			if ( bRes == TRUE )
            {
				break;
            }
            
            //如果判断h263能力集失败，再判断h263plus能力集
            s32 nIndex = 0;
            //assuming that we can match a H.263 or H.263+ video capability
            //bRes = TRUE;

            for(nIndex = 0; nIndex < nNum; nIndex++) 
            {
                if(awInterPayload[nIndex] == v_h263)
                {
                    awInterPayload[nIndex] = v_h263plus;
                    break;
                }
                else if(awInterPayload[nIndex] == GetH239Type(v_h263))
                {
                    awInterPayload[nIndex] = GetH239Type(v_h263plus);
                    break;
                }

                //////////////////////////////////////////////////////////////////////////
                /*
                else if(m_awInterPayload[nIndex] == GetH239Type(v_h263plus)
                    || m_awInterPayload[nIndex] == GetEType(v_h263plus,emEncryptTypeAES)
                    || m_awInterPayload[nIndex] == GetEType(v_h263plus,emEncryptTypeDES)
                    || m_awInterPayload[nIndex] == GetEType(v_h263plus,emEncryptTypeAES|emEncryptTypeDES))
                {
                    break;
                }
                */
                //////////////////////////////////////////////////////////////////////////

                else if(awInterPayload[nIndex] == GetEType(v_h263,emEncryptTypeAES))
                {
                    awInterPayload[nIndex] = GetEType(v_h263plus,emEncryptTypeAES);
                    break;
                }
                else if(awInterPayload[nIndex] == GetEType(v_h263,emEncryptTypeDES))
                {
                    awInterPayload[nIndex] = GetEType(v_h263plus,emEncryptTypeDES);
                    break;
                }
                else if(awInterPayload[nIndex] == GetEType(v_h263,emEncryptTypeAES|emEncryptTypeDES))
                {
                    awInterPayload[nIndex] = GetEType(v_h263plus,emEncryptTypeAES|emEncryptTypeDES);
                    break;
                }
                else if(awInterPayload[nIndex] == GetH239Type(GetEType(v_h263,emEncryptTypeAES)))
                {
                    awInterPayload[nIndex] = GetH239Type(GetEType(v_h263plus,emEncryptTypeAES));
                    break;
                }
                else if(awInterPayload[nIndex] == GetH239Type(GetEType(v_h263,emEncryptTypeDES)))
                {
                    awInterPayload[nIndex] = GetH239Type(GetEType(v_h263plus,emEncryptTypeDES));
                    break;
                }
                else if(awInterPayload[nIndex] == GetH239Type(GetEType(v_h263,emEncryptTypeAES|emEncryptTypeDES)))
                {
                    awInterPayload[nIndex] = GetH239Type(GetEType(v_h263plus,emEncryptTypeAES|emEncryptTypeDES));
                    break;
                }
                else
                {
                    bRes = FALSE;
                }
            }

            if (nIndex == nNum) //没有H263能力
                return bRes;
        }

        return bRes;
	}
	
	//得到能力表
	u16 GetCapFromTable(u8 byCapIndex)
	{
		if(byCapIndex >= MAX_CAPTABLE_SIZE)
			return 0;
		return m_awCapTable[byCapIndex];
	};
	void SetCapToTable(u8 byCapIndex, u16 wPayload)
	{
		if(byCapIndex >= MAX_CAPTABLE_SIZE)
			return;
		m_awCapTable[byCapIndex] = wPayload;
	}
    //设置视频能力集.不包括H.264
	BOOL SetVideoCap(TVideoCap &tVideoCap, u16 wPayload)
	{
		switch(wPayload)
		{
		case v_h261:	 m_tH261Cap		= tVideoCap; break;
		case v_h263:	 m_tH263Cap		= tVideoCap; break;
		case v_h263plus: m_tH263plusCap = tVideoCap; break;		
		case v_mpeg4:	 m_tMpeg4Cap	= tVideoCap; break;
        case v_h262:	 m_tH262Cap	    = tVideoCap; break;
            
        case v_h264:
            OspPrintf(1, 0, "[H323Adp][TCapSet::SetVideoCap] Function depreciated!\n");
            //no return
		default: 
            return FALSE;
		}
		return TRUE;
	}
    //得到视频能力集.不包括H.264
    TVideoCap* GetVideoCap(u16 wPayload)
    {
        TVideoCap *ptVideoCap = NULL;
        switch(wPayload)
        {
        case v_h261:	 ptVideoCap = &m_tH261Cap;		break;
        case v_h263:	 ptVideoCap = &m_tH263Cap;		break;
        case v_h263plus: ptVideoCap = &m_tH263plusCap;  break;        
        case v_mpeg4:	 ptVideoCap = &m_tMpeg4Cap;		break;
        case v_h262:	 ptVideoCap = &m_tH262Cap;		break;
        
        case v_h264:
             OspPrintf(1, 0, "[H323Adp][TCapSet::GetVideoCap] Function depreciated!\n");
             //no retuurn
        default:break;
        }
        return ptVideoCap;
    }
     //设置H.264视频能力集,返回值是byH264SubId,调SetCapDesc时用
    u8 SetH264VideoCap(TH264VideoCap &tVideoCap)
    {       
        if (m_bySubId >= MAX_H264CAP_NUM)
        {
            return FALSE;
        }
		u8 byResult = m_bySubId;
		if(FindH264VideoCap(tVideoCap, byResult) == FALSE)
		{
			m_atH264Cap[m_bySubId++]= tVideoCap;
		}

		return byResult;
    }
    //得到H.264视频能力集
    TH264VideoCap* GetH264VideoCap(u8 bySubId)
    {
        if (bySubId >= MAX_H264CAP_NUM)
        {
            return NULL;
        }
        return &(m_atH264Cap[bySubId]);
    }
    //得到H264个数
	u8 GetH264Num()
	{
		return m_bySubId;
	}

	//不包含G7231/AAC
	TAudioCap* GetAudioCap(u16 wPayload)
	{
		TAudioCap *ptAudioCap = NULL;
		switch(wPayload)
		{
		case a_g711u: ptAudioCap = &m_tG711u; break;
		case a_g711a: ptAudioCap = &m_tG711a; break;
		case a_g722:  ptAudioCap = &m_tG722;  break;
		case a_g728:  ptAudioCap = &m_tG728;  break;
		case a_g729:  ptAudioCap = &m_tG729;  break;
		case a_mp3:   ptAudioCap = &m_tMP3;   break;
        case a_g7221: ptAudioCap = &m_tG7221c;break;
		case a_g719:  ptAudioCap = &m_tG719;  break;//ruiyigen 20091012
		default:break;
		}
		return ptAudioCap;
	}
	//不包含G7231/AAC
	BOOL SetAudioCap(TAudioCap &tAudioCap, u16 wPayload)
	{
		switch(wPayload)
		{
		case a_g711u: m_tG711u = tAudioCap; break;
		case a_g711a: m_tG711a = tAudioCap; break;
		case a_g722:  m_tG722  = tAudioCap; break;
		case a_g728:  m_tG728  = tAudioCap; break;
		case a_g729:  m_tG729  = tAudioCap; break;
        case a_mp3:   m_tMP3   = tAudioCap; break;
        case a_g7221: m_tG7221c= tAudioCap; break;
		case a_g719:  m_tG719 = tAudioCap; break; //ruiyigen 20091012
		default: return FALSE; break;
		}
		return TRUE;
	}
	//G7231
	TG7231Cap* GetG7231Cap()
	{	
		return &m_tG7231;
	}
	//G7231
	BOOL SetG7231Cap(TG7231Cap &tG7231Cap)
	{
		m_tG7231 = tG7231Cap;
		return TRUE;
	}
    //AAC
    TAACCap* GetAACCap(u16 wPayload)
    {
        TAACCap *ptAudioCap = NULL;
        switch(wPayload)
        {
        case a_mpegaaclc: ptAudioCap = &m_tMpegAacLc;break;
        case a_mpegaacld: ptAudioCap = &m_tMpegAacLd;break;
        default:break;
        }
        return ptAudioCap;
    }
    //AAC
    BOOL SetAACCap(TAACCap &tAudioCap, u16 wPayload)
    {
        switch(wPayload)
        {
        case a_mpegaaclc: 
			{
				//m_tMpegAacLc = tAudioCap; break;
				m_tMpegAacLc.SetCapDirection( tAudioCap.GetCapDirection() );
				m_tMpegAacLc.SetChnl( static_cast<tagAACCap::emAACChnlCfg>(m_tMpegAacLc.GetChnl() | tAudioCap.GetChnl()) );
				break;
			}
        case a_mpegaacld: 
			{
				//m_tMpegAacLd = tAudioCap; break;
				m_tMpegAacLc.SetCapDirection( tAudioCap.GetCapDirection() );
				m_tMpegAacLd.SetChnl( static_cast<tagAACCap::emAACChnlCfg>(m_tMpegAacLd.GetChnl() | tAudioCap.GetChnl()) );
				break;
			}
            
        default: return FALSE; break;
        }
        return TRUE;
	}

	TDataCap* GetDataCap(u16 wPayload)
	{
		TDataCap *ptDataCap = NULL;
		switch(wPayload)
		{
		case d_t120: ptDataCap = &m_tT120; break;
		case d_h224: ptDataCap = &m_tH224; break;
		default:break;
		}
		return ptDataCap;
	}
	BOOL SetDataCap(TDataCap &tDataCap, u16 wPayload)
	{
		switch(wPayload)
		{
		case d_t120: m_tT120 = tDataCap; break;
		case d_h224: m_tH224 = tDataCap; break;
		default: return FALSE; break;
		}
		return TRUE;
	}

	/*仅用于发送能力集时使用,表示同时支持的加密能力.
	 *如byEncryptBit=emEncryptTypeDES|emEncryptTypeAES;
	 */
	void SetEncryptBits( u8 byEncryptBits)
	{
		m_byEncryptBits = byEncryptBits;
	}
	u8 GetEncryptBits()
	{
		return m_byEncryptBits;
	}

	u8 GetSubId(u8 byCapIdex)
	{
		return m_awH264ToSubId[byCapIdex];
	}

	void SetH264NALMode(emH264NALMode emNALMode)
	{
		m_emH264NALMode = emNALMode;
	}

	emH264NALMode GetH264NALMode()
	{
		return m_emH264NALMode;
	}

	BOOL IsH264CapALessEqualB( const TH264VideoCap & tH264A, const TH264VideoCap & tH264B )
	{
		double dAMaxMBPS = .0;
		double dBMaxMBPS = .0;
		if ( tH264A.GetMaxStaticMBPs() != 0 )
		{
			double dAMaxMBPS = tH264A.GetMaxMBPS();
			double dAMaxFS = tH264A.GetMaxFS();
			double dAMaxStaticMBPS = tH264A.GetMaxStaticMBPs();
			dAMaxMBPS = 1.0 / ( ( ( 4.0 / dAMaxFS ) / dAMaxMBPS ) + ( ( ( dAMaxFS - 4.0 ) / dAMaxFS ) / dAMaxStaticMBPS ) );
		}
		else
		{
			dAMaxMBPS = tH264A.GetMaxMBPS();
		}

		if ( tH264B.GetMaxStaticMBPs() != 0 )
		{
			double dBMaxMBPS = tH264B.GetMaxMBPS();
			double dBMaxFS = tH264B.GetMaxFS();
			double dBMaxStaticMBPS = tH264B.GetMaxStaticMBPs();
			dBMaxMBPS = 1.0 / ( ( ( 4.0 / dBMaxFS ) / dBMaxMBPS ) + ( ( ( dBMaxFS - 4.0 ) / dBMaxFS ) / dBMaxStaticMBPS ) );
		}
		else
		{
			dBMaxMBPS = tH264B.GetMaxMBPS();
		}

		if( ( dAMaxMBPS <= dBMaxMBPS ) && ( tH264A.GetMaxFS() <= tH264B.GetMaxFS() ) && ( !tH264A.GetProfileValue()/*如果tH264是空的,profile为零*/ || tH264A.GetProfileValue() >= tH264B.GetProfileValue() ) )
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
/*====================================================================
  函 数 名： FindH264PeerBestSupport
  功    能： 返回某一分辨率下对端能力集所支持的最大帧率的TH264VideoCap结构体
  算法实现： 
  全局变量： 
  参    数： u16 *pwPayload     payload数组
             s32 nNum           payload数组大小
			 OUT BOOL &bStatic  返回的TH264VideoCap结构体中是否包含staticMaxBPS
			 s32 nFirstPtIndex    第一路视频在payload数组中的索引，若第一路非h264为-1
			 s32 nSecondPtIndex   第二路视频在payload数组中的索引，若无第二路为-1
			 TH264VideoCap* ptH264Cap 能力集数组
  返 回 值： BOOL 匹配成功TRUE 失败FALSE
  备注：1、参数TH264VideoCap* ptH264Cap是INOUT类型，但是本算法中返回的ptH264Cap数组仅最后一个索引值有效，
        因为只有在第一路成功发起的条件下才会尝试发送第二路，故数组ptH264Cap[2]中的ptH264Cap[0]的值已
		固定，是否改变不予参考，ptH264Cap[1]表示第二路匹配成功的值
		2、本函数仅对H264能力集支持
		3、建议在调用IsExistsimultaneous()失败的情况下调用本函数
  --------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  20100830      1.0		    王晓意                   创建
======================================================================*/
	BOOL FindH264PeerBestSupport( u16 *pwPayload, s32 nNum, OUT BOOL &bStatic, s32 nFirstPtIndex = -1, s32 nSecondPtIndex = -1, INOUT TH264VideoCap* ptH264Cap = NULL )
	{
		TH264VideoCap tH264;
		s32 nH264Num=0;
//		s32 anH264Input[MAX_H264CAP_NUM];
		bStatic = FALSE;
		if ( -1 == nFirstPtIndex && -1 == nSecondPtIndex )
		{
			return FALSE;
		}
		if ( NULL == ptH264Cap )
		{
			return FALSE;
		}

		if ( nFirstPtIndex != -1 )
		{
			++nH264Num;
		}
		if ( nSecondPtIndex != -1 )
		{
			++nH264Num;
		}
		

        u16 m_awInterPayload[MAX_SIMUCAP_NUM]; //保存外部载荷类型的数组
        for(s32 nInterIndex = 0; nInterIndex < nNum; nInterIndex++)
        {
            m_awInterPayload[nInterIndex] = pwPayload[nInterIndex];
        }

		///////////////////////////////////////////////////////////////////
		BOOL bUsedFlag[MAX_DESCAP_NUM][MAX_SIMUCAP_NUM];
		BOOL ab239Flag[MAX_DESCAP_NUM][MAX_SIMUCAP_NUM];
		
		BOOL abH264GoodFlag[MAX_H264CAP_NUM];
		memset(abH264GoodFlag, 0, sizeof(abH264GoodFlag));

		BOOL abH264BadFlag[MAX_H264CAP_NUM];
		memset(abH264BadFlag, 0, sizeof(abH264BadFlag));

		u8   anH264Index[MAX_H264CAP_NUM];
		
		TH264VideoCap atBadH264VideoCap[MAX_H264CAP_NUM];
		memset( atBadH264VideoCap, 0, sizeof( atBadH264VideoCap ) );
		
		TH264VideoCap atGoodH264VideoCap[MAX_H264CAP_NUM];
		memset( atGoodH264VideoCap, 0, sizeof( atGoodH264VideoCap ) );

		TH264VideoCap atEqualH264VideoCap[MAX_H264CAP_NUM];
		memset( atEqualH264VideoCap, 0, sizeof( atEqualH264VideoCap ) );
		
        s32  nCount = nNum;//用于计数
        s32 nCapIndex=0, nSimuIndex=0, nAltIndex=0;
		s32 nIndex = 0;
        for(nCapIndex = 0; nCapIndex < MAX_DESCAP_NUM; nCapIndex++)
        {
            nCount = nNum; //恢复初始值
			nIndex = 0;
			memset(anH264Index, 0xFF, MAX_H264CAP_NUM);
			
			memset(bUsedFlag, 0, sizeof(bUsedFlag));
			memset( ab239Flag, 0, sizeof( ab239Flag ) );
            for(nSimuIndex=0; nSimuIndex < MAX_SIMUCAP_NUM; nSimuIndex++)
            {
				if ( bUsedFlag[nCapIndex][nSimuIndex] )
				{
					continue;
				}
				s32  nPtNum = 1;//根据当前的主辅视频来选择相应的Pt
				BOOL bSingle = FALSE;
				if ( 2 == nH264Num )
				{
					if ( 0 == nIndex )
					{
						nPtNum = nFirstPtIndex;
					}
					else
					{
						nPtNum = nSecondPtIndex;
					}
				}
				else if ( 1 == nH264Num )
				{
					bSingle = TRUE;
					if ( nFirstPtIndex != -1 )
					{
						nPtNum = nFirstPtIndex;
					}
					else if ( nSecondPtIndex != -1 )
					{
						nPtNum = nSecondPtIndex;
					}
				}
				if ( -1 == nFirstPtIndex && -1 == nSecondPtIndex )
				{
					return FALSE;
				}
				s32 nTemp = 0;
				for ( ; nTemp < MAX_ALTCAP_NUM; ++nTemp )
				{
					u16 wCapTableId = GetCapDescId(nCapIndex,nSimuIndex,nAltIndex);

                    if ( wCapTableId == 0xFFFF )
                        break;
					u16 wPayloadType = m_awCapTable[wCapTableId];
                    if(wPayloadType == 0) 
                        break;
					if( IsH239Type( wPayloadType ) )
					{
						ab239Flag[nCapIndex][nSimuIndex] = TRUE;
					}
				}
                for(nAltIndex = 0; nAltIndex < MAX_ALTCAP_NUM; nAltIndex++)
                {
                    u16 wCapTableId = GetCapDescId(nCapIndex,nSimuIndex,nAltIndex);
    
                    if ( wCapTableId == 0xFFFF )
                        break;
					u16 wPayloadType = m_awCapTable[wCapTableId];
                    if(wPayloadType == 0) 
                        break;
                    BOOL t1 = IsH239Type( wPayloadType );
					BOOL t2 = IsH239Type( pwPayload[nPtNum] );
                    //先判断是否都是H239类型
                    if ( IsH239Type( wPayloadType ) != IsH239Type( pwPayload[nPtNum] ) )
                        continue;
					
					if ( ab239Flag[nCapIndex][nSimuIndex] && nFirstPtIndex == nPtNum )//若当前是第一路，而simuindex是第二路则跳过
					{
						break;
					}
                    //高字节包含，低字节相等
                    if ( (((~(wPayloadType&0xff00))&(pwPayload[nPtNum]&0xff00)) == 0)
                        && ((wPayloadType&0xff) == (pwPayload[nPtNum]&0xff)) )
                    {
						if((wPayloadType&0xff) == v_h264 && ptH264Cap!=NULL)
						{
                            u8 bySubId=m_awH264ToSubId[wCapTableId];
							if(bySubId == 0xFF)
								break;
							//判断能力是否支持
							
							if( IsH264CapALessEqualB( ptH264Cap[nIndex], m_atH264Cap[bySubId] ) )
							{
								
								bUsedFlag[nCapIndex][nSimuIndex] = TRUE;
								nSimuIndex = -1;//重新搜索	
								abH264GoodFlag[nIndex] = TRUE;
								
								if ( IsH264CapALessEqualB( atGoodH264VideoCap[nIndex], m_atH264Cap[bySubId] ) )
								{
									atGoodH264VideoCap[nIndex] = m_atH264Cap[bySubId];
								}
							}
							else////////////////////.	
							{
								if( ptH264Cap[nIndex].GetMaxFS() <= m_atH264Cap[bySubId].GetMaxFS() )
								{
									if ( IsH264CapALessEqualB( atBadH264VideoCap[nIndex], m_atH264Cap[bySubId] ) )
									{
										abH264BadFlag[nIndex] = TRUE;
										atBadH264VideoCap[nIndex] = m_atH264Cap[bySubId];
									}
									if ( ptH264Cap[nIndex].GetMaxFS() == m_atH264Cap[bySubId].GetMaxFS() 
									    && IsH264CapALessEqualB(atEqualH264VideoCap[nIndex], m_atH264Cap[bySubId]) )
									{
										atEqualH264VideoCap[nIndex] = m_atH264Cap[bySubId];
									}
								}
							}//////////////////////
						}
                    }
                }
				if ( -1 == nSimuIndex )
				{
					--nCount;
					++nIndex;
					if ( 0 == nCount || bSingle || 2 == nIndex )
					{
						break;
					}
				}
            }
        }
		BOOL bGoodFind = FALSE;
		BOOL bBadFind  = FALSE;
		if ( 2 == nH264Num )
		{
			bGoodFind = abH264GoodFlag[1];
			bBadFind  = abH264BadFlag[1];
		}
		else
		{
			bGoodFind = abH264GoodFlag[0];
			bBadFind  = abH264BadFlag[0];
		}
		if ( !bGoodFind && !bBadFind )
		{
			return FALSE;
		}
		for ( int j = nH264Num - 1; j >= 0; --j )
		{
			if ( IsH264CapALessEqualB( atGoodH264VideoCap[j], atBadH264VideoCap[j] ) )
			{
				if ( ptH264Cap[j].GetMaxFS() == atEqualH264VideoCap[j].GetMaxFS() )
				{
					ptH264Cap[j] = atEqualH264VideoCap[j];
				}
				else
				{
					ptH264Cap[j] = atBadH264VideoCap[j];
				}
				
			}
			else
			{
				ptH264Cap[j] = atGoodH264VideoCap[j];
			}

			if ( ptH264Cap[j].GetMaxStaticMBPs() != 0 )
			{
				bStatic = TRUE;
			}
			break;
		}
		return TRUE;
		////////////////////////////////////////////////////////////////////////
	}

private:
	//往能力表添加能力项。如果为 H.264则需要传入TH264VideoCap数组下标，返回值为payload在captable中的索引值
	u16 AddCapToTable(u16 wPayload, u8 byH264SubId)
	{
		for(s32 i = 0; i < MAX_CAPTABLE_SIZE; i++)
		{
            u8 byH264Count = 0;
            // 检查是否重复
			if(m_awCapTable[i] == wPayload)
            {
                if ((wPayload&0xff) != v_h264) 
                {
				    return i;
                }
				if(byH264SubId == m_awH264ToSubId[i])
				{
					return i;
				}
                byH264Count ++;
                if (byH264Count >= MAX_H264CAP_NUM)
                {
                    return 0xFFFF;
                }
            }
			
			if(m_awCapTable[i] == 0)
			{
				m_awCapTable[i] = wPayload;
				return i;
			}
		}
		return 0xFFFF;
	}
    
    BOOL IsInterExistsimultaneous( u16 *pwPayload, s32 nNum, TH264VideoCap* ptH264Cap=NULL, s32 nH264Num=0, TAACCap* ptAacCap = NULL )
    {
		//add by shigubin for DEBUG
		if( ptAacCap || ptH264Cap )
		{
			OspPrintf( TRUE, FALSE, "IsInterExistsimultaneous Payload IN Total caps: %d, (H264 caps: %d):\n", nNum, nH264Num);
			u8 h264Index = 0;
			for( u8 payloadNum = 0; payloadNum < nNum; payloadNum++ )
			{
				OspPrintf( TRUE,FALSE, "	No%d. payload %d\n", payloadNum+1, pwPayload[payloadNum] );
				switch( pwPayload[payloadNum]&0xff )
				{
				case a_mpegaaclc:
				case a_mpegaacld:
					{
						if( ptAacCap )
							OspPrintf( TRUE,FALSE, "		AAC  ChannelNum: %d   Sample: %d	   Bitrate: %d\n", (u8)ptAacCap->GetChnl(), (u8)ptAacCap->GetSampleFreq(), ptAacCap->GetBitrate() );
					}
					break;
				case v_h264:
					{
						if( (h264Index == nH264Num) || (!ptH264Cap) )
							break;
						TH264VideoCap tH264 = ptH264Cap[h264Index++];
						{
							OspPrintf( TRUE, FALSE, "		H264  Profile: %d Level: %d MBPS: %d FS: %d StatciMBPS: %d Bitrate: %d\n",
								tH264.GetProfileValue(), tH264.GetLevel(), tH264.GetMaxMBPS(), tH264.GetMaxFS(), tH264.GetMaxStaticMBPs(), tH264.GetBitRate() );
						}
					}
					break;
				default:
					break;
				}
			}
		}
		//add end 

        //BOOL bUsedFlag[MAX_DESCAP_NUM][MAX_SIMUCAP_NUM][MAX_ALTCAP_NUM] = {{0},{0},{0}};
		BOOL bUsedFlag  [MAX_DESCAP_NUM][MAX_SIMUCAP_NUM];
		BOOL ab239Flag  [MAX_DESCAP_NUM][MAX_SIMUCAP_NUM];
        BOOL bH264Flag  [MAX_H264CAP_NUM];
        u8   anH264Index[MAX_H264CAP_NUM];
        s32 nCount = nNum;//用于计数
		s32 nH264Count = 0;
        s32 nCapIndex=0, nSimuIndex=0, nAltIndex=0;
        for(nCapIndex = 0; nCapIndex < MAX_DESCAP_NUM; nCapIndex++)
        {
            nCount = 0; //恢复初始值
			memset(anH264Index, 0xFF, MAX_H264CAP_NUM);
			memset(bH264Flag, 0, sizeof(bH264Flag));
			memset(bUsedFlag, 0, sizeof(bUsedFlag));
			memset(ab239Flag, 0, sizeof(ab239Flag));
			nH264Count = 0;
            for(nSimuIndex=0; nSimuIndex < MAX_SIMUCAP_NUM; nSimuIndex++)
            {
                for(nAltIndex = 0; nAltIndex < MAX_ALTCAP_NUM; nAltIndex++)
                {
                    u16 wCapTableId = GetCapDescId(nCapIndex,nSimuIndex,nAltIndex);
                    //guzh 2008/06/02
                    if ( wCapTableId == 0xFFFF )
                        break;
					u16 wPayloadType = m_awCapTable[wCapTableId];
                    if (wPayloadType == 0)
                        break;

                    //先判断是否都是H239类型
                    if ( IsH239Type( wPayloadType ) != IsH239Type( pwPayload[nCount] ) )
                        continue;

                    //高字节包含，低字节相等
                    if ( (((~(wPayloadType&0xff00))&(pwPayload[nCount]&0xff00)) == 0)
                        && ((wPayloadType&0xff) == (pwPayload[nCount]&0xff)))
                    {
                        if (bUsedFlag[nCapIndex][nSimuIndex] == FALSE)
                        {
                            if ((wPayloadType&0xff) == v_h264 && ptH264Cap!=NULL)
                            {
                                u8 bySubId=m_awH264ToSubId[wCapTableId];
                                if (bySubId == 0xFF)
                                    break;
                                //判断能力是否支持
                                //for(s32 i=nH264Num-1; i>=0; i--)
                                //{
                                if (nH264Count < nH264Num && bH264Flag[nH264Count] == FALSE && ptH264Cap[nH264Count] <= m_atH264Cap[bySubId])
                                {
                                    bH264Flag[nH264Count] = TRUE;
                                    ++nCount;
                                    bUsedFlag[nCapIndex][nSimuIndex] = TRUE;
                                    nSimuIndex = -1;//重新搜索
                                    
                                    anH264Index[nH264Count] = bySubId;
                                    ++nH264Count;
                                    break;
                                }
                                //}
                            }
                            else
                            {
                                //二进制位表示通道 add 2011 12 28
                                if (( (wPayloadType&0xff) == a_mpegaaclc || (wPayloadType&0xff) == a_mpegaacld ))
                                {
                                    TAACCap *ptLocAacCap = GetAACCap( (wPayloadType&0xff) );
                                    if( wPayloadType&0xff == a_mpegaaclc && ptAacCap != NULL )
                                    {
                                        if( ((~(ptLocAacCap->GetChnl()&0x7f)) & (ptAacCap->GetChnl()&0x7f)) != 0 )
                                            break;
                                    }
                                    else if( wPayloadType&0xff == a_mpegaacld && ptAacCap != NULL )
                                    {
                                        if( ((~(ptLocAacCap->GetChnl()&0x7f)) & (ptAacCap->GetChnl()&0x7f)) != 0 )
                                            break;
                                    }
                                }

                                OspPrintf(1, 0, "       [adapter][DEBUG] IsInterExistsimultaneous, Round:%d, payload matched: wPayloadType=%d!\r\n",nCapIndex, wPayloadType);

                                ++nCount;
                                bUsedFlag[nCapIndex][nSimuIndex] = TRUE;
                                nSimuIndex = -1;//重新搜索
                                break;
                            }
                        } //fi (bUsedFlag[nCapIndex][nSimuIndex] == FALSE)
                    } //fi payload type高字节包含，低字节相等
                } // for (nAltIndex = 0; nAltIndex < MAX_ALTCAP_NUM; nAltIndex++)

                //OspPrintf(1, 0, "   [adapter][DEBUG] IsInterExistsimultaneous, Round:%d, matched:%d/%d, matched h264 cap: %d!\r\n", nCapIndex, nCount, nNum, nH264Num);

                if (nCount == nNum)
                {
					for (s32 i=0; i<nH264Num; i++)
					{
						ptH264Cap[i] = m_atH264Cap[anH264Index[i]];
						OspPrintf(1, 0, "   MaxMBPS is %d MaxFS is %d\r\n", ptH264Cap[i].GetMaxMBPS(), ptH264Cap[i].GetMaxFS());
					}
                    return TRUE;
                }
            } // for(nSimuIndex=0; nSimuIndex < MAX_SIMUCAP_NUM; nSimuIndex++)
        } // for(nCapIndex = 0; nCapIndex < MAX_DESCAP_NUM; nCapIndex++)

        return FALSE;
    }

    BOOL FindH264VideoCap(TH264VideoCap& tH264VideoCap, u8& bySubId)
	{
		for (u8 i=0; i<m_bySubId; i++)
		{
			if (m_atH264Cap[i] == tH264VideoCap)
			{
				bySubId = i;
				return TRUE;
			}
		}
		return FALSE;
	}
}TCapSet;


//temporal spatial tradeoff
typedef struct tagTSTO
{
	u8  m_chIsCommand;     //命令还是指示
    u8  m_chTradeoffValue; //The trade off value, between 0(high spatial resolution) to 31(a high frame rate)

    tagTSTO()
	{
		Clear();
	}

	void Clear()
	{
		m_chIsCommand = 0;
		m_chTradeoffValue = 0;
	}

	void SetIsCommand(BOOL bCommand)
	{
		m_chIsCommand = bCommand ? 1:0;
	}
	BOOL IsCommand()
	{
		return m_chIsCommand>0;
	}
	//set The trade off value, between 0 to 31.
	void SetTradeoffValue(u8 byTradeoffValue)
	{
		if( byTradeoffValue > 31 )
		{
           return ;
		}
        m_chTradeoffValue = byTradeoffValue;
	}

	s8 GetTradeoffValue()
	{
		return m_chTradeoffValue;
	}
}TTSTO;


//update GOB struct
typedef struct tagVFUGOB
{
	s32 m_nFirstGOB;
	s32 m_nNumOfGOB;

	tagVFUGOB()
	{
		Clear();
	}

	void Clear()
	{
		m_nFirstGOB = 0;
		m_nNumOfGOB = 0;
	}

    void host2net(BOOL bhton)
    {
        if (bhton)
        {
            m_nFirstGOB = htonl(m_nFirstGOB);
            m_nNumOfGOB = htonl(m_nNumOfGOB);
        }
        else
        {
            m_nFirstGOB = ntohl(m_nFirstGOB);
            m_nNumOfGOB = ntohl(m_nNumOfGOB);
        }
    }

    void SetFirstGOB(s32 nFirstGOB)
	{
		m_nFirstGOB = nFirstGOB;
	}
	s32 GetFirstGOB()
	{
		return m_nFirstGOB;
	}
	void SetNumOfGOB(s32 nNumOfGOB)
	{
		//add by yj
		//m_nNumOfGOB = m_nNumOfGOB;
		m_nNumOfGOB = nNumOfGOB;
		//end
	}
	s32 GetNumOfGOB()
	{
		return m_nNumOfGOB;
	}

}TVFUGOB;

//update MB struct
typedef struct tagVFUMB
{
	s32 m_nFirstGOB;
	s32 m_nFirstMB;
	s32 m_nNumOfMB;

	tagVFUMB()
	{
		Clear();
	}

	void Clear()
	{
		m_nFirstGOB = 0;
		m_nFirstMB	= 0;
		m_nNumOfMB	= 0;
	}

    void host2net(BOOL bhton)
    {
        if (bhton)
        {
            m_nFirstGOB = htonl(m_nFirstGOB);
            m_nFirstMB  = htonl(m_nFirstMB);
            m_nNumOfMB  = htonl(m_nNumOfMB);
        }
        else
        {
            m_nFirstGOB = ntohl(m_nFirstGOB);
            m_nFirstMB  = ntohl(m_nFirstMB);
            m_nNumOfMB  = ntohl(m_nNumOfMB);
        }
    }

    void SetFirstGOB(s32 nFirstGOB)
	{
		m_nFirstGOB = nFirstGOB;
	}
	s32 GetFirstGOB()
	{
		return m_nFirstGOB;
	}

	void SetFirstMB(s32 nFirstMB)
	{
		m_nFirstMB = nFirstMB;
	}
	s32 GetFirstMB()
	{
		return m_nFirstMB;
	}

	void SetNumOfMB(s32 nNumOfMB)
	{
		//add by yj
		//m_nNumOfMB = m_nNumOfMB;
		m_nNumOfMB = nNumOfMB;
		//end
	}
	s32 GetNumOfMB()
	{
		return m_nNumOfMB;
	}
	
}TVFUMB;

typedef struct tagPassRsp
{
	TTERLABEL m_tTer;
	s32       m_nPassLen;
	u8        m_abyPassword[LEN_H243PWD+1]; 

	tagPassRsp()
	{
		Clear();
	}

	void Clear()
	{
		memset( &m_tTer, 0, sizeof(m_tTer) );
		m_nPassLen = 0;
		memset( m_abyPassword, 0, sizeof(m_abyPassword) );
	}

    void host2net(BOOL bhton)
    {
        if (bhton)
        {
            m_nPassLen  = htonl(m_nPassLen);
        }
        else
        {
            m_nPassLen  = ntohl(m_nPassLen);
        }
    }

	//设置TTERLABEL
	void SetTerLabel( TTERLABEL &tTerLabel )
	{
		m_tTer = tTerLabel;
	}
	//获取TTERLABEL
	TTERLABEL& GetTerlabel()
	{
		return m_tTer;
	}
	
	//设置Password
	void SetPassword( u8 *pPassword, u16 wLen )
	{
		if ( pPassword != NULL && wLen < LEN_H243PWD+1 ) 
		{
			memset( m_abyPassword, 0, sizeof(m_abyPassword) );
			memcpy( m_abyPassword, pPassword, wLen );
			m_abyPassword[wLen] = '\0';
            m_nPassLen = wLen;
		}
	}
	//获取Password
	u16 GetPassword( u8 *pbyPassword, u16 &wLen )
	{
		if ( pbyPassword != NULL ) 
		{
			u16 realLen = min( wLen, LEN_H243PWD );
			memcpy( pbyPassword, m_abyPassword, realLen );
			return realLen;
		}
	}	
}TPassRsp;

//H221非标参数
typedef struct tagH221NonStandard
{
	u8		m_byT35CountryCode;
	u8		m_byT35Extension;
	u16		m_wManufacturerCode;

	void Clear()
	{
		m_byT35CountryCode	 = 0;
		m_byT35Extension	 = 0;
		m_wManufacturerCode  = 0;
	}

	void SetH221NonStandard( u8 byCountryCode, u8 byExtension, u16 wManufacturerCode)
	{
		m_byT35CountryCode	= byCountryCode;
		m_byT35Extension	= byExtension;
		m_wManufacturerCode = wManufacturerCode;
	}
	u8 GetT35CountryCode()
	{
		return m_byT35CountryCode;
	}
	u8 GetT35Extension()
	{
		return m_byT35Extension;
	}
	u16 GetManufacturerCode()
	{
		return m_wManufacturerCode;
	}
}TH221NONSTANDARD,*PTH221NONSTANDARD;

//厂商标识信息
typedef struct tagVendorInfo
{
	TH221NONSTANDARD	m_tVendor;
	s8					m_achProductId[MAX_ProductIDSize+1];
	s8					m_achVersionId[MAX_VersionIdSize+1];

	tagVendorInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_tVendor.Clear();
		memset( m_achProductId, 0, sizeof(m_achProductId) );
		memset( m_achVersionId, 0, sizeof(m_achVersionId) );
	}

	void SetH221NonStandard( TH221NONSTANDARD &tVendor)
	{
		m_tVendor = tVendor;
	}
	TH221NONSTANDARD& GetH221NonStandard()
	{
		return m_tVendor;
	}

	void SetVendorID( s8 *pProductId, s8 *pVersionId )
	{
		strncpy( m_achProductId, pProductId, min(strlen(pProductId), MAX_ProductIDSize));
		strncpy( m_achVersionId, pVersionId, min(strlen(pVersionId), MAX_VersionIdSize));		
	}
	s8* GetProductID()
	{
		return m_achProductId;
	}
	s8* GetVersionID()
	{
		return m_achVersionId;
	}
}TVENDORINFO,*PTVENDORINFO;


//MultiplexCapability
typedef struct tagMultiplexCapability
{
	emH264NALMode m_emH264NALMode;

	tagMultiplexCapability()
	{
		Clear();
	}
	
	void Clear()
	{
		m_emH264NALMode = emNALNode;
	}
	
	void SetH264NALMode( emH264NALMode emNALMode)
	{
		m_emH264NALMode = emNALMode;
	}
	emH264NALMode& GetH264NALMode()
	{
		return m_emH264NALMode;
	}

}TMultiplexCapability,*PTMultiplexCapability;

//RAS消息结构
//RRQ
typedef struct tagRRQInfo
{
	TNETADDR		m_tCallAddr;			//呼叫信令地址
	TNETADDR		m_tLocalRASAddr;		//本地RAS地址
	cmEndpointType	m_eTerminalType;		//终端类型,GK用
	TALIASADDR		m_atAlias[MAXALIASNUM];	//向GK注册时的多个别名
	s32				m_nTimetoLive;			//timetolive，单位：秒	(optional)
	BOOL			m_bKeepAlive;			//轻量级注册标志位
	TALIASADDR		m_tEndpointID;			//终端ID,GK用			(optional)
	TNETADDR		m_tGkAddr;				//GK地址
	s8				m_ach_UserName[REG_NAME_LEN];			//注册帐号(E164)
	s8				m_ach_Password[REG_PWD_LEN];			//注册密钥
	TALIASADDR		m_tGKID;				//GK ID,GK用
	BOOL            m_bSigTraversal;        //signalling traversal
 	BOOL			m_bRequireGKPermison;

	s32				m_nProductIdSize;
	u8				m_abyProductId[LEN_256];				//产品号
	s32				m_nVerIdSize;
	u8				m_abyVersionId[LEN_256];				//版本号

	tagRRQInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_tCallAddr.Clear();
		m_tLocalRASAddr.Clear();
		m_eTerminalType = cmEndpointTypeTerminal;
		memset( m_atAlias,0,sizeof(m_atAlias));
		m_nTimetoLive = 0;
		m_tGkAddr.Clear();
		memset(&m_tEndpointID,0,sizeof(TALIASADDR));
		memset(m_ach_UserName, 0, sizeof(m_ach_UserName));
		memset(m_ach_Password, 0, sizeof(m_ach_Password));
		m_bKeepAlive = FALSE;
		m_tCallAddr.SetNetAddress( 0, CALLPORT );
		m_tLocalRASAddr.SetNetAddress( 0, RASPORT );
		m_tGkAddr.SetNetAddress( 0, RASPORT );
		memset(&m_tGKID,0,sizeof(TALIASADDR));
		m_bSigTraversal = FALSE;
		m_bRequireGKPermison = FALSE;
		m_nProductIdSize = 0;
		m_nVerIdSize = 0;
		memset(m_abyProductId , 0 , LEN_256);
		memset(m_abyVersionId , 0 , LEN_256);
	}	
	
    //设置呼叫信令地址
    void SetCallAddr( TNETADDR &tCallAddr )
    {
        m_tCallAddr = tCallAddr;
    }
    //得到呼叫信令地址
    TNETADDR& GetCallAddr()
    {
        return m_tCallAddr;
    }

    //设置本地RAS地址
    void SetLocalRASAddr( TNETADDR &tLocalRASAddr )
    {
        m_tLocalRASAddr = tLocalRASAddr;
    }
    //得到本地RAS地址
    TNETADDR& GetLocalRASAddr()
    {
        return m_tLocalRASAddr;
    }

    //设置终端类型
    void SetTerminalType( cmEndpointType emTerminalType )
    {
        m_eTerminalType = emTerminalType;
    }
    //得到终端类型
    cmEndpointType GetTerminalType()
    {
        return m_eTerminalType;
    }

    //设置注册别名
    void SetRRQAlias( TALIASADDR &tAlias, u8 byAliasNo )
    {
        if ( byAliasNo >= MAXALIASNUM )
            return;
        
        m_atAlias[byAliasNo] = tAlias;
    }
    //得到注册的别名
    TALIASADDR& GetRRQAlias( u8 byAliasNo )
    {
        if ( byAliasNo < MAXALIASNUM ) 
            return m_atAlias[byAliasNo];
        else
            return m_atAlias[0];        
    }

    //设置TimetoLive值
    void SetTimetoLive( s32 nTimetoLive )
    {
        m_nTimetoLive = nTimetoLive;
    }
    //得到TimetoLive值
    s32 GetTimetoLive()
    {
        return m_nTimetoLive;
    }

    //设置是否轻量级注册
    void SetKeepAlive( BOOL bKeepAlive )
    {
        m_bKeepAlive = bKeepAlive;
    }
    //判断是否轻量级注册
    BOOL IsKeepAlive()
    {
        return m_bKeepAlive;
    }

    //设置终端ID
    void SetEndpointID( TALIASADDR &tEndpointID )
    {
        m_tEndpointID = tEndpointID;
    }
    //得到终端ID
    TALIASADDR& GetEndpointID()
    {
        return m_tEndpointID;
    }
	//设置用户密码
	void SetUserInfo(s8* pszUserName, s8* pszPassword)
	{
		if (pszUserName == NULL)
		{
			return;
		}
		memset(m_ach_UserName, 0, sizeof(m_ach_UserName));
		u16 nameLen = min( strlen(pszUserName), REG_NAME_LEN);
		strncpy(m_ach_UserName, pszUserName, nameLen);
		m_ach_UserName[nameLen] = '\0';
		
		if (pszPassword != NULL)
		{
			memset(m_ach_Password, 0, sizeof(m_ach_Password));
			u16 pwdLen = min(strlen(pszPassword), REG_PWD_LEN);
			strncpy(m_ach_Password, pszPassword, pwdLen);
			m_ach_Password[pwdLen] = '\0';
		}
		
	}
	//取用户名
	s8* GetUserName()
	{
		return m_ach_UserName;
	}
	//取密码
	s8* GetPassword()
	{
		return m_ach_Password;
	}
    //设置GK地址
    void SetGKAddr( TNETADDR &tGkAddr )
    {
        m_tGkAddr = tGkAddr;
    }
    //得到GK地址
    TNETADDR& GetGKAddr()
    {
        return m_tGkAddr;
    }
    //设置GK ID
    void SetGKID( TALIASADDR &tGKID )
    {
        m_tGKID = tGKID;
    }
    //得到GK ID
    TALIASADDR& GetGKID()
    {
        return m_tGKID;
    }
	//设置产品号
	void SetProductId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyProductId, 0, sizeof(m_abyProductId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nProductIdSize = nReal;
        if(nLen >= LEN_1)
		{
			memcpy(m_abyProductId, pbyId, nReal);
		}
	}
	//获取产品号长度
	s32 GetProductIdSize()
	{
		return m_nProductIdSize;
	}
	//获取产品号
	s32 GetProductId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nProductIdSize); 
		memcpy(pbyId, m_abyProductId, nReal);
		return nReal;
	}
	//设置版本号	
	void SetVersionId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return ;
		}
		memset( m_abyVersionId, 0, sizeof(m_abyVersionId) );
		s32 nReal = min(max(LEN_1, nLen), LEN_256);
		m_nVerIdSize = nReal;
        if(nLen >= LEN_1)
		{
			memcpy(m_abyVersionId, pbyId, nReal);
		}
	}
	//获取版本号长度
	s32 GetVersionIdSize()
	{
		return m_nVerIdSize;
	}
	//获取版本号
	s32 GetVersionId(u8 *pbyId, s32 nLen)
	{
		if(pbyId == NULL|| nLen <= 0 )
		{
			return 0;
		}
		s32 nReal = min(nLen, m_nVerIdSize); 
		memcpy(pbyId, m_abyVersionId, nReal);
		return nReal;
	}

}TRRQINFO, *PTRRQINFO;

//RCF
typedef struct tagRCFInfo
{
	TNETADDR		m_tCallSignalAddr;		//GK返回的呼叫信令地址
	TALIASADDR		m_atAlias[MAXALIASNUM];	//GK返回的多个别名			(optional)
	TALIASADDR		m_tGKID;				//GK ID,GK用
	TALIASADDR		m_tEndpointID;			//终端ID,GK用
	s32				m_nTimetoLive;			//timetolive 单位：秒		(optional)
	BOOL			m_bWillRespondToIRR;	//是否响应IRR消息,GK用
	BOOL            m_bIsKDVTSGK;           //是否是KDVTS的GK
	BOOL            m_bSigTraversal;        //signalling traversal
	BOOL            m_bIfAccess;
	tagRCFInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_tCallSignalAddr.Clear();
		memset(m_atAlias,0,sizeof(m_atAlias));
		memset(&m_tGKID,0,sizeof(TALIASADDR));
		memset(&m_tEndpointID,0,sizeof(TALIASADDR));
		m_tCallSignalAddr.SetNetAddress( 0, CALLPORT );
		m_nTimetoLive = 0;
		m_bWillRespondToIRR = FALSE;
		m_bIsKDVTSGK = FALSE;
		m_bSigTraversal = FALSE;
		m_bIfAccess = FALSE;
	}

    //设置GK返回的呼叫信令地址
    void SetCallSignalAddr( TNETADDR &tCallSignalAddr )
    {
        m_tCallSignalAddr = tCallSignalAddr;
    }
    //得到GK返回的呼叫信令地址
    TNETADDR& GetCallSignalAddr()
    {
        return m_tCallSignalAddr;
    }
    
    //设置GK返回的多个别名
    void SetRCFAlias( TALIASADDR &tAlias, u8 byAliasNo )
    {
        if ( byAliasNo >= MAXALIASNUM )
            return;
        
        m_atAlias[byAliasNo] = tAlias;
    }
    //得到GK返回的多个别名
    TALIASADDR& GetRCFAlias( u8 byAliasNo )
    {
        if ( byAliasNo < MAXALIASNUM ) 
            return m_atAlias[byAliasNo];
        else
            return m_atAlias[0];
    }

    //设置GK ID
    void SetGKID( TALIASADDR &tGKID )
    {
        m_tGKID = tGKID;
    }
    //得到GK ID
    TALIASADDR& GetGKID()
    {
        return m_tGKID;
    }
    
    //设置终端ID
    void SetEndpointID( TALIASADDR &tEndpointID )
    {
        m_tEndpointID = tEndpointID;
    }
    //得到终端ID
    TALIASADDR& GetEndpointID()
    {
        return m_tEndpointID;
    }

    //设置TimetoLive值
    void SetTimetoLive( s32 nTimetoLive )
    {
        m_nTimetoLive = nTimetoLive;
    }
    //得到TimetoLive值
    s32 GetTimetoLive()
    {
        return m_nTimetoLive;
    }

    //设置是否响应IRR消息
    void SetWillRespondToIRR( BOOL bKeepAlive )
    {
        m_bWillRespondToIRR = bKeepAlive;
    }
    //判断是否响应IRR消息
    BOOL IsWillRespondToIRR()
    {
        return m_bWillRespondToIRR;
    }
	void SetIsKDVTSGK(BOOL bIsKDVGK)
	{
		m_bIsKDVTSGK = bIsKDVGK;
	}
	BOOL GetIsKDVTSGK()
	{
		return m_bIsKDVTSGK;
    }
}TRCFINFO, *PTRCFINFO;

//ARQ
typedef struct tagARQInfo
{
	cmCallType		m_eCallType;			    //呼叫类型,GK用
	cmCallModelType m_eCallModel;			    //GK呼叫模式,默认值为直接呼叫	(optional)
	TALIASADDR		m_tEndpointID;			    //终端ID,GK用
	TALIASADDR		m_tDestAlias;			    //目的端别名
	TNETADDR		m_tCalledAddr;			    //目的端地址
	TALIASADDR		m_atSrcAlias[MAXALIASNUM];  //源端别名
	TNETADDR		m_tCallAddr;			    //源端地址,GK用					(optional)
	s32				m_nBandWidth;			    //单位：kbps
	s32				m_nCRV;					    //呼叫参考值,GK用
	s8  			m_achConferenceID[LEN_GUID];//会议ID,GK用
	BOOL			m_bAnswerCall;			    //是否被叫,GK用
	s8  			m_achCallID[LEN_GUID];	    //呼叫ID,GK用
	TALIASADDR		m_tGKID;				    //GK ID,GK用					(optional)
	
	TNETADDR		m_tGkAddr;				    //GK地址
	
	tagARQInfo()
	{
		Clear();
	}

	void Clear()
	{
		memset(&m_tEndpointID,0,sizeof(TALIASADDR));
		memset(&m_tGKID,0,sizeof(TALIASADDR));
		memset(m_achCallID,0,sizeof(m_achCallID));
		memset(m_achConferenceID,0,sizeof(m_achConferenceID));
		memset(&m_tDestAlias,0,sizeof(TALIASADDR));
		memset(m_atSrcAlias,0,sizeof(m_atSrcAlias));
		m_tCalledAddr.Clear();
		m_tCallAddr.Clear();		
		m_tGkAddr.Clear();
		m_eCallType = cmCallTypeP2P;
		m_eCallModel = cmCallModelTypeDirect;
		m_nBandWidth = 2000;
		m_nCRV = 0;
		m_bAnswerCall = FALSE;
		m_tCalledAddr.SetNetAddress( 0, CALLPORT );
		m_tGkAddr.SetNetAddress( 0, RASPORT );
	}

    //设置呼叫类型
    void SetCallType( cmCallType emCallType )
    {
        m_eCallType = emCallType;
    }
    //得到呼叫类型
    cmCallType GetCallType()
    { 
        return m_eCallType;
    }

    //设置呼叫模式
    void SetCallModelType( cmCallModelType emCallModel )
    {
        m_eCallModel = emCallModel;
    }
    //得到呼叫模式
    cmCallModelType GetCallModelType()
    { 
        return m_eCallModel;
    }

    //设置终端ID
    void SetEndpointID( TALIASADDR &tEndpointID )
    {
        m_tEndpointID = tEndpointID;
    }
    //得到终端ID
    TALIASADDR& GetEndpointID()
    {
        return m_tEndpointID;
    }

    //设置目的端别名
    void SetDestAlias( TALIASADDR &tDestAlias )
    {
        m_tDestAlias = tDestAlias;
    }
    //得到目的端别名
    TALIASADDR& GetDestAlias()
    {
        return m_tDestAlias;
    }

    //设置目的端地址
    void SetCalledAddr( TNETADDR &tCalledAddr )
    {
        m_tCalledAddr = tCalledAddr;
    }
    //得到目的端地址
    TNETADDR& GetCalledAddr()
    {
        return m_tCalledAddr;
    }

    //设置源端别名
    void SetARQSrcAlias( TALIASADDR &tAlias, u8 byAliasNo )
    {
        if ( byAliasNo >= MAXALIASNUM )
            return;
        
        m_atSrcAlias[byAliasNo] = tAlias;
    }
    //得到源端别名
    TALIASADDR& GetARQSrcAlias( u8 byAliasNo )
    {
        if ( byAliasNo < MAXALIASNUM ) 
            return m_atSrcAlias[byAliasNo];
        else
            return m_atSrcAlias[0];
    }

    //设置源端地址
    void SetCallAddr( TNETADDR &tCallAddr )
    {
        m_tCallAddr = tCallAddr;
    }
    //得到源端地址
    TNETADDR& GetCallAddr()
    {
        return m_tCallAddr;
    }

    //设置带宽
    void SetBandWidth( s32 nBandWidth )
    {
        m_nBandWidth = nBandWidth;
    }
    //得到带宽
    s32 GetBandWidth()
    {
        return m_nBandWidth;
    }

    //设置呼叫参考值
    void SetCRV( s32 nCRV )
    {
        m_nCRV = nCRV;
    }
    //得到呼叫参考值
    s32 GetCRV()
    {
        return m_nCRV;
    }

    //设置会议ID
    void SetConferenceID( s8 *pchConferenceID, u8 byLen )
    {
        if ( pchConferenceID == NULL )
            return;
        
        memset( m_achConferenceID, 0, sizeof(m_achConferenceID) );
        u8 byRealLen = min( byLen, LEN_GUID );
        strncpy( m_achConferenceID, pchConferenceID, byRealLen );
        m_achConferenceID[byRealLen] = '\0';
    }
    //得到会议ID
    s8* GetConferenceID()
    {
        return m_achConferenceID;
    }

    //设置是否被叫
    void SetAnswerCall( BOOL bAnswerCall )
    {
        m_bAnswerCall = bAnswerCall;
    }
    //判断是否被叫
    BOOL IsAnswerCall()
    {
        return m_bAnswerCall;
    }

    //设置呼叫ID
    void SetCallID( s8 *pchCallID, u8 byLen )
    {
        if ( pchCallID == NULL )
            return;
        
        memset( m_achCallID, 0, sizeof(m_achCallID) );
        u8 byRealLen = min( byLen, LEN_GUID );
        strncpy( m_achCallID, pchCallID, byRealLen );
        m_achCallID[byRealLen] = '\0';
    }
    //得到呼叫ID
    s8* GetCallID()
    {
        return m_achCallID;
    }

    //设置GK ID
    void SetGKID( TALIASADDR &tGKID )
    {
        m_tGKID = tGKID;
    }
    //得到GK ID
    TALIASADDR& GetGKID()
    {
        return m_tGKID;
    }

    //设置GK地址
    void SetGKAddr( TNETADDR &tGKAddr )
    {
        m_tGkAddr = tGKAddr;
    }
    //得到GK地址
    TNETADDR& GetGKAddr()
    {
        return m_tGkAddr;
    }    
}TARQINFO, *PTARQINFO;

//ACF
typedef struct tagACFInfo
{
	s32				m_nBandWidth;			    //单位：kbps
	cmCallModelType m_eCallModel;			    //GK呼叫模式,默认值为直接呼叫
	TNETADDR		m_tCalledAddr;			    //目的端地址
	s32				m_nIRRFrequency;		    //IRR的发送频率，单位：秒	(optional)
	BOOL			m_bWillResponseIRR;		    //响应IRR消息
	
	tagACFInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_tCalledAddr.Clear();
		m_nBandWidth = 4000;
		m_eCallModel = cmCallModelTypeDirect;
		m_nIRRFrequency = 0;
		m_bWillResponseIRR = FALSE;
		m_tCalledAddr.SetNetAddress( 0, CALLPORT );
	}

    //设置带宽
    void SetBandWidth( s32 nBandWidth )
    {
        m_nBandWidth = nBandWidth;
    }
    //得到带宽
    s32 GetBandWidth()
    {
        return m_nBandWidth;
    }

    //设置呼叫模式
    void SetCallModelType( cmCallModelType emCallModel )
    {
        m_eCallModel = emCallModel;
    }
    //得到呼叫模式
    cmCallModelType GetCallModelType()
    { 
        return m_eCallModel;
    }

    //设置目的端地址
    void SetCalledAddr( TNETADDR &tCalledAddr )
    {
        m_tCalledAddr = tCalledAddr;
    }
    //得到目的端地址
    TNETADDR& GetCalledAddr()
    {
        return m_tCalledAddr;
    }

    //设置IRR的发送频率
    void SetIRRFrequency( s32 nIRRFrequency )
    {
        m_nIRRFrequency = nIRRFrequency;
    }
    //得到IRR的发送频率
    s32 GetIRRFrequency()
    {
        return m_nIRRFrequency;
    }

    //设置是否响应IRR消息
    void SetWillResponseIRR( BOOL bWillResponseIRR )
    {
        m_bWillResponseIRR = bWillResponseIRR;
    }
    //判断是否响应IRR消息
    BOOL IsWillResponseIRR()
    {
        return m_bWillResponseIRR;
    }
}TACFINFO, *PTACFINFO;

//DRQ
typedef struct tagDRQInfo
{
	TALIASADDR		m_tEndpointID;			    //终端ID,GK用
	s8  			m_achConferenceID[LEN_GUID];//会议ID,GK用
	s32				m_nCRV;					    //呼叫参考值,GK用
	cmRASDisengageReason m_eReason;			    //DRQ的原因		
	s8  			m_achCallID[LEN_GUID];	    //呼叫ID,GK用
	TALIASADDR		m_tGKID;				    //GK ID,GK用				(optional)
	BOOL			m_bAnsweredCall;		    //是否为被叫端,GK用

	TNETADDR		m_tDestAddr;			    //DRQ的接受端地址
	
	tagDRQInfo()
	{
		Clear();
	}

	void Clear()
	{
		memset(&m_tEndpointID,0,sizeof(TALIASADDR));
		memset(m_achConferenceID,0,sizeof(m_achConferenceID));
		memset(m_achCallID,0,sizeof(m_achCallID));
		memset(&m_tGKID,0,sizeof(TALIASADDR));
		m_tDestAddr.Clear();
		m_bAnsweredCall = FALSE;
		m_eReason = (cmRASDisengageReason)0;
		m_nCRV = 0;
		m_tDestAddr.SetNetAddress( 0, RASPORT );
	}

    //设置终端ID
    void SetEndpointID( TALIASADDR &tEndpointID )
    {
        m_tEndpointID = tEndpointID;
    }
    //得到终端ID
    TALIASADDR& GetEndpointID()
    {
        return m_tEndpointID;
    }

    //设置会议ID
    void SetConferenceID( s8 *pchConferenceID, u8 byLen )
    {
        if ( pchConferenceID == NULL )
            return;
        
        memset( m_achConferenceID, 0, sizeof(m_achConferenceID) );
        u8 byRealLen = min( byLen, LEN_GUID );
        strncpy( m_achConferenceID, pchConferenceID, byRealLen );
        m_achConferenceID[byRealLen] = '\0';
    }
    //得到会议ID
    s8* GetConferenceID()
    {
        return m_achConferenceID;
    }

    //设置呼叫参考值
    void SetCRV( s32 nCRV )
    {
        m_nCRV = nCRV;
    }
    //得到呼叫参考值
    s32 GetCRV()
    {
        return m_nCRV;
    }

    //设置DRQ的原因
    void SetDRQReason( cmRASDisengageReason emDRQReason )
    {
        m_eReason = emDRQReason;
    }
    //得到DRQ的原因
    cmRASDisengageReason GetDRQReason()
    { 
        return m_eReason;
    }

    //设置呼叫ID
    void SetCallID( s8 *pchCallID, u8 byLen )
    {
        if ( pchCallID == NULL )
            return;
        
        memset( m_achCallID, 0, sizeof(m_achCallID) );
        u8 byRealLen = min( byLen, LEN_GUID );
        strncpy( m_achCallID, pchCallID, byRealLen );
        m_achCallID[byRealLen] = '\0';
    }
    //得到呼叫ID
    s8* GetCallID()
    {
        return m_achCallID;
    }
    
    //设置GK ID
    void SetGKID( TALIASADDR &tGKID )
    {
        m_tGKID = tGKID;
    }
    //得到GK ID
    TALIASADDR& GetGKID()
    {
        return m_tGKID;
    }

    //设置是否被叫
    void SetAnsweredCall( BOOL bAnsweredCall )
    {
        m_bAnsweredCall = bAnsweredCall;
    }
    //判断是否被叫
    BOOL IsAnsweredCall()
    {
        return m_bAnsweredCall;
    }
    
    //设置DRQ的接受端地址
    void SetDestAddr( TNETADDR &tDestAddr )
    {
        m_tDestAddr = tDestAddr;
    }
    //得到DRQ的接受端地址
    TNETADDR& GetDestAddr()
    {
        return m_tDestAddr;
    }
}TDRQINFO, *PTDRQINFO;

//URQ
typedef struct tagURQInfo
{
	TNETADDR		m_tCallAddr;			//源端呼叫信令地址
	TALIASADDR		m_atAlias[MAXALIASNUM];	//向GK注册时的多个别名		(optional)
	TALIASADDR		m_tEndpointID;			//终端ID					(optional)
	TALIASADDR		m_tGKID;				//GK ID						(optional)
	cmRASUnregReason m_eReason;				//URQ的原因					(optional)
	
	TNETADDR		m_tDestAddr;			//URQ的接受端地址
	
	tagURQInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_tCallAddr.Clear();		
		memset(m_atAlias,0,sizeof(m_atAlias));		
		memset(&m_tEndpointID,0,sizeof(TALIASADDR));	
		memset(&m_tGKID,0,sizeof(TALIASADDR));
		m_tDestAddr.Clear();
		m_eReason = cmRASUnregReasonUndefinedReason;
		m_tCallAddr.SetNetAddress( 0, CALLPORT );
		m_tDestAddr.SetNetAddress( 0, RASPORT );
	}

    //设置源端呼叫信令地址
    void SetCallAddr( TNETADDR &tCallAddr )
    {
        m_tCallAddr = tCallAddr;
    }
    //得到源端呼叫信令地址
    TNETADDR& GetCallAddr()
    {
        return m_tCallAddr;
    }

    //设置向GK注册时的多个别名
    void SetURQAlias( TALIASADDR &tAlias, u8 byAliasNo )
    {
        if ( byAliasNo >= MAXALIASNUM )
            return;
        
        m_atAlias[byAliasNo] = tAlias;
    }
    //得到向GK注册时的多个别名
    TALIASADDR& GetURQAlias( u8 byAliasNo )
    {
        if ( byAliasNo < MAXALIASNUM ) 
            return m_atAlias[byAliasNo];
        else
            return m_atAlias[0];
    }

    //设置终端ID
    void SetEndpointID( TALIASADDR &tEndpointID )
    {
        m_tEndpointID = tEndpointID;
    }
    //得到终端ID
    TALIASADDR& GetEndpointID()
    {
        return m_tEndpointID;
    }

    //设置GK ID
    void SetGKID( TALIASADDR &tGKID )
    {
        m_tGKID = tGKID;
    }
    //得到GK ID
    TALIASADDR& GetGKID()
    {
        return m_tGKID;
    }

    //设置URQ的原因
    void SetURQReason( cmRASUnregReason emURQReason )
    {
        m_eReason = emURQReason;
    }
    //得到URQ的原因
    cmRASUnregReason GetURQReason()
    { 
        return m_eReason;
    }

    //设置URQ的接受端地址
    void SetDestAddr( TNETADDR &tDestAddr )
    {
        m_tDestAddr = tDestAddr;
    }
    //得到URQ的接受端地址
    TNETADDR& GetDestAddr()
    {
        return m_tDestAddr;
    }
}TURQINFO, *PTURQINFO;

//BRQ
typedef struct tagBRQInfo
{
	TALIASADDR		m_tEndpointID;			    //终端ID,GK用
	s8			    m_achConferenceID[LEN_GUID];//会议ID,GK用
	s32				m_nCRV;					    //呼叫参考值,GK用
	cmCallType		m_eCallType;			    //呼叫类型,GK用，默认为点对点	(optional)
	s32				m_nBandWidth;			    //单位：kbps
	
	s8			    m_achCallID[LEN_GUID];	    //呼叫ID,GK用
	TALIASADDR		m_tGKID;				    //GK ID,GK用					(optional)
	BOOL			m_bAnsweredCall;		    //是否为被叫端,GK用

	TNETADDR		m_tDestAddr;			    //BRQ的接受端地址
	
	tagBRQInfo()
	{
		Clear();
	}

	void Clear()
	{
		memset(&m_tEndpointID,0,sizeof(TALIASADDR));
		memset(&m_achConferenceID,0,sizeof(m_achConferenceID));
		memset(&m_achCallID,0,sizeof(m_achCallID));
		memset(&m_tGKID,0,sizeof(TALIASADDR));
		m_tDestAddr.Clear();
		m_eCallType = cmCallTypeP2P;
		m_tDestAddr.SetNetAddress( 0, RASPORT );
		m_nBandWidth = 0;
		m_nCRV = 0;
		m_bAnsweredCall = FALSE;
	}

    //设置终端ID
    void SetEndpointID( TALIASADDR &tEndpointID )
    {
        m_tEndpointID = tEndpointID;
    }
    //得到终端ID
    TALIASADDR& GetEndpointID()
    {
        return m_tEndpointID;
    }

    //设置会议ID
    void SetConferenceID( s8 *pchConferenceID, u8 byLen )
    {
        if ( pchConferenceID == NULL )
            return;
        
        memset( m_achConferenceID, 0, sizeof(m_achConferenceID) );
        u8 byRealLen = min( byLen, LEN_GUID );
        strncpy( m_achConferenceID, pchConferenceID, byRealLen );
        m_achConferenceID[byRealLen] = '\0';
    }
    //得到会议ID
    s8* GetConferenceID()
    {
        return m_achConferenceID;
    }

    //设置呼叫参考值
    void SetCRV( s32 nCRV )
    {
        m_nCRV = nCRV;
    }
    //得到呼叫参考值
    s32 GetCRV()
    {
        return m_nCRV;
    }

    //设置呼叫类型
    void SetCallType( cmCallType emCallType )
    {
        m_eCallType = emCallType;
    }
    //得到呼叫类型
    cmCallType GetCallType()
    { 
        return m_eCallType;
    }

    //设置带宽
    void SetBandWidth( s32 nBandWidth )
    {
        m_nBandWidth = nBandWidth;
    }
    //得到带宽
    s32 GetBandWidth()
    {
        return m_nBandWidth;
    }

    //设置呼叫ID
    void SetCallID( s8 *pchCallID, u8 byLen )
    {
        if ( pchCallID == NULL )
            return;
        
        memset( m_achCallID, 0, sizeof(m_achCallID) );
        u8 byRealLen = min( byLen, LEN_GUID );
        strncpy( m_achCallID, pchCallID, byRealLen );
        m_achCallID[byRealLen] = '\0';
    }
    //得到呼叫ID
    s8* GetCallID()
    {
        return m_achCallID;
    }

    //设置GK ID
    void SetGKID( TALIASADDR &tGKID )
    {
        m_tGKID = tGKID;
    }
    //得到GK ID
    TALIASADDR& GetGKID()
    {
        return m_tGKID;
    }

    //设置是否被叫
    void SetAnsweredCall( BOOL bAnsweredCall )
    {
        m_bAnsweredCall = bAnsweredCall;
    }
    //判断是否被叫
    BOOL IsAnsweredCall()
    {
        return m_bAnsweredCall;
    }

    //设置BRQ的接受端地址
    void SetDestAddr( TNETADDR &tDestAddr )
    {
        m_tDestAddr = tDestAddr;
    }
    //得到BRQ的接受端地址
    TNETADDR& GetDestAddr()
    {
        return m_tDestAddr;
    }
}TBRQINFO, *PTBRQINFO;

//BCF
typedef struct tagBCFInfo
{
	s32				m_nBandWidth;			//单位：kbps
	tagBCFInfo()
	{
		m_nBandWidth = 0;
	}

    //设置带宽
    void SetBandWidth( s32 nBandWidth )
    {
        m_nBandWidth = nBandWidth;
    }
    //得到带宽
    s32 GetBandWidth()
    {
        return m_nBandWidth;
    }
}TBCFINFO, *PTBCFINFO;

//LRQ
typedef struct tagLRQInfo
{
	TALIASADDR		m_tEndpointID;			//终端ID,GK用		(optional)
	TALIASADDR		m_tDestAlias;			//目的端别名
	TNETADDR		m_tReplyAddr;			//发送端地址	
	TALIASADDR		m_tGKID;				//GK ID,GK用		(optional)

	TNETADDR		m_tResponseAddr;		//接受端地址

	tagLRQInfo()
	{
		Clear();
	}

	void Clear()
	{
		memset(&m_tEndpointID,0,sizeof(TALIASADDR));	
		memset(&m_tDestAlias,0,sizeof(TALIASADDR));			
		m_tReplyAddr.Clear();
		memset(&m_tGKID,0,sizeof(TALIASADDR));
		m_tResponseAddr.Clear();
		m_tReplyAddr.SetNetAddress( 0, RASPORT );
		m_tResponseAddr.SetNetAddress( 0, RASPORT );
	}

    //设置终端ID
    void SetEndpointID( TALIASADDR &tEndpointID )
    {
        m_tEndpointID = tEndpointID;
    }
    //得到终端ID
    TALIASADDR& GetEndpointID()
    {
        return m_tEndpointID;
    }

    //设置目的端别名
    void SetDestAlias( TALIASADDR &tDestAlias )
    {
        m_tDestAlias = tDestAlias;
    }
    //得到目的端别名
    TALIASADDR& GetDestAlias()
    {
        return m_tDestAlias;
    }

    //设置发送端地址
    void SetReplyAddr( TNETADDR &tReplyAddr )
    {
        m_tReplyAddr = tReplyAddr;
    }
    //得到发送端地址
    TNETADDR& GetReplyAddr()
    {
        return m_tReplyAddr;
    }
    
    //设置GK ID
    void SetGKID( TALIASADDR &tGKID )
    {
        m_tGKID = tGKID;
    }
    //得到GK ID
    TALIASADDR& GetGKID()
    {
        return m_tGKID;
    }

    //设置接受端地址
    void SetResponseAddr( TNETADDR &tResponseAddr )
    {
        m_tResponseAddr = tResponseAddr;
    }
    //得到接受端地址
    TNETADDR& GetResponseAddr()
    {
        return m_tResponseAddr;
    }
}TLRQINFO, *PTLRQINFO;

//LCF
typedef struct tagLCFInfo
{
	TNETADDR		m_tCallAddr;			//呼叫信令地址
	TNETADDR		m_tRASAddr;				//RAS地址
	cmEndpointType	m_nDestinationType;		//终端类型			(optional)

	tagLCFInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_tCallAddr.Clear();		
		m_tRASAddr.Clear();
		m_tCallAddr.SetNetAddress( 0, CALLPORT );
		m_tRASAddr.SetNetAddress( 0, RASPORT );
		m_nDestinationType = cmEndpointTypeTerminal;
	}

    //设置呼叫信令地址
    void SetCallAddr( TNETADDR &tCallAddr )
    {
        m_tCallAddr = tCallAddr;
    }
    //得到呼叫信令地址
    TNETADDR& GetCallAddr()
    {
        return m_tCallAddr;
    }
    
    //设置RAS地址
    void SetRASAddr( TNETADDR &tRASAddr )
    {
        m_tRASAddr = tRASAddr;
    }
    //得到RAS地址
    TNETADDR& GetRASAddr()
    {
        return m_tRASAddr;
    }
    
    //设置终端类型
    void SetDestinationType( cmEndpointType emDestinationType )
    {
        m_nDestinationType = emDestinationType;
    }
    //得到终端类型
    cmEndpointType GetDestinationType()
    {
        return m_nDestinationType;
    }
}TLCFINFO, *PTLCFINFO;

//IRQ
typedef struct tagIRQInfo
{	
	s32				m_nCRV;					//呼叫参考值
	s8  			m_achCallID[LEN_GUID];	//呼叫ID

	HCALL			m_hsCall;				//呼叫句柄	
	TNETADDR		m_tPeerRasAddr;			//对端RAS地址

	tagIRQInfo()
	{
		Clear();
	}

	void Clear()
	{
		memset( m_achCallID, 0, sizeof(m_achCallID));
		m_tPeerRasAddr.Clear();
		m_nCRV = 0;
		m_hsCall = NULL;
		m_tPeerRasAddr.SetNetAddress( 0, RASPORT );
	}

    //设置呼叫参考值
    void SetCRV( s32 nCRV )
    {
        m_nCRV = nCRV;
    }
    //得到呼叫参考值
    s32 GetCRV()
    {
        return m_nCRV;
    }

    //设置呼叫ID
    void SetCallID( s8 *pchCallID, u8 byLen )
    {
        if ( pchCallID == NULL )
            return;
        
        memset( m_achCallID, 0, sizeof(m_achCallID) );
        u8 byRealLen = min( byLen, LEN_GUID );
        strncpy( m_achCallID, pchCallID, byRealLen );
        m_achCallID[byRealLen] = '\0';
    }
    //得到呼叫ID
    s8* GetCallID()
    {
        return m_achCallID;
    }

    //设置呼叫句柄
    void SetHCALL( HCALL hsCall )
    {
        if ( hsCall != NULL )
            m_hsCall = hsCall;
    }
    //得到呼叫句柄
    HCALL GetHCALL()
    {
        return m_hsCall;
    }

    //设置对端RAS地址
    void SetPeerRasAddr( TNETADDR &tPeerRasAddr )
    {
        m_tPeerRasAddr = tPeerRasAddr;
    }
    //得到对端RAS地址
    TNETADDR& GetPeerRasAddr()
    {
        return m_tPeerRasAddr;
    }
}TIRQINFO, *PTIRQINFO;

//IRR
typedef struct tagIRRInfo
{
	cmEndpointType	m_eEndpointType;		    //终端类型,GK用
	TALIASADDR		m_tEndpointID;			    //终端ID,GK用
	TNETADDR		m_tRASAddr;				    //本地RAS地址
	TNETADDR		m_tCallAddr;			    //呼叫信令地址
	TALIASADDR		m_atAlias[MAXALIASNUM];	    //向GK注册的多个别名	(optional)	
	//perCallInfo
	s32				m_nCRV;					    //呼叫参考值,GK用
	s8				m_achConferenceID[LEN_GUID];//会议ID,GK用
	BOOL			m_bOriginator;			    //是否为主叫,GK用

	s8				m_achCallID[LEN_GUID];	    //呼叫ID,GK用
	BOOL			m_bNeedResponse;		    //是否响应IRR消息
	BOOL			m_bIsUnsolicited;		    //默认为TRUE：主动发IRR；FALSE：响应IRQ
											
	TNETADDR		m_tGKAddr;				    //GK地址
	

	tagIRRInfo()
	{
		Clear();
	}

	void Clear()
	{
		memset(&m_tEndpointID,0,sizeof(TALIASADDR));	
		m_tRASAddr.Clear();			
		m_tCallAddr.Clear();		
		memset(m_atAlias,0,sizeof(m_atAlias));
		m_tGKAddr.Clear();			
		
		m_bNeedResponse  = FALSE;
		m_bIsUnsolicited = TRUE;
		m_tRASAddr.SetNetAddress( 0, RASPORT );
		m_tCallAddr.SetNetAddress( 0, CALLPORT );
		m_tGKAddr.SetNetAddress( 0, RASPORT );
	}

    //设置终端类型
    void SetEndpointType( cmEndpointType emEndpointType )
    {
        m_eEndpointType = emEndpointType;
    }
    //得到终端类型
    cmEndpointType GetEndpointType()
    {
        return m_eEndpointType;
    }

    //设置终端ID
    void SetEndpointID( TALIASADDR &tEndpointID )
    {
        m_tEndpointID = tEndpointID;
    }
    //得到终端ID
    TALIASADDR& GetEndpointID()
    {
        return m_tEndpointID;
    }

    //设置RAS地址
    void SetRASAddr( TNETADDR &tRASAddr )
    {
        m_tRASAddr = tRASAddr;
    }
    //得到RAS地址
    TNETADDR& GetRASAddr()
    {
        return m_tRASAddr;
    }

    //设置呼叫信令地址
    void SetCallAddr( TNETADDR &tCallAddr )
    {
        m_tCallAddr = tCallAddr;
    }
    //得到呼叫信令地址
    TNETADDR& GetCallAddr()
    {
        return m_tCallAddr;
    }

    //设置注册别名
    void SetIRRAlias( TALIASADDR &tAlias, u8 byAliasNo )
    {
        if ( byAliasNo >= MAXALIASNUM )
            return;
        
        m_atAlias[byAliasNo] = tAlias;
    }
    //得到注册的别名
    TALIASADDR& GetIRRAlias( u8 byAliasNo )
    {
        if ( byAliasNo < MAXALIASNUM ) 
            return m_atAlias[byAliasNo];
        else
            return m_atAlias[0];
    }

    //设置呼叫参考值
    void SetCRV( s32 nCRV )
    {
        m_nCRV = nCRV;
    }
    //得到呼叫参考值
    s32 GetCRV()
    {
        return m_nCRV;
    }

    //设置会议ID
    void SetConferenceID( s8 *pchConferenceID, u8 byLen )
    {
        if ( pchConferenceID == NULL )
            return;
        
        memset( m_achConferenceID, 0, sizeof(m_achConferenceID) );
        u8 byRealLen = min( byLen, LEN_GUID );
        strncpy( m_achConferenceID, pchConferenceID, byRealLen );
        m_achConferenceID[byRealLen] = '\0';
    }
    //得到会议ID
    s8* GetConferenceID()
    {
        return m_achConferenceID;
    }
    
    //设置是否主叫
    void SetOriginator( BOOL bOriginator )
    {
        m_bOriginator = bOriginator;
    }
    //判断是否主叫
    BOOL IsOriginator()
    {
        return m_bOriginator;
    }

    //设置呼叫ID
    void SetCallID( s8 *pchCallID, u8 byLen )
    {
        if ( pchCallID == NULL )
            return;
        
        memset( m_achCallID, 0, sizeof(m_achCallID) );
        u8 byRealLen = min( byLen, LEN_GUID );
        strncpy( m_achCallID, pchCallID, byRealLen );
        m_achCallID[byRealLen] = '\0';
    }
    //得到呼叫ID
    s8* GetCallID()
    {
        return m_achCallID;
    }
    
    //设置是否响应IRR消息
    void SetNeedResponse( BOOL bNeedResponse )
    {
        m_bNeedResponse = bNeedResponse;
    }
    //判断是否响应IRR消息
    BOOL IsNeedResponse()
    {
        return m_bNeedResponse;
    }

    //设置是否Unsolicited消息
    void SetUnsolicited( BOOL bIsUnsolicited )
    {
        m_bIsUnsolicited = bIsUnsolicited;
    }
    //判断是否Unsolicited消息
    BOOL IsUnsolicited()
    {
        return m_bIsUnsolicited;
    }
    
    //设置GK地址
    void SetGKAddr( TNETADDR &tGkAddr )
    {
        m_tGKAddr = tGkAddr;
    }
    //得到GK地址
    TNETADDR& GetGKAddr()
    {
        return m_tGKAddr;
    }
    
}TIRRINFO, *PTIRRINFO;

//GRQ
typedef struct tagGRQInfo
{	
	TNETADDR		m_tRASAddr;				    //终端RAS地址
	cmEndpointType	m_eEndpointType;		    //终端类型
	TALIASADDR		m_tGKID;				    //GK ID,GK用

	tagGRQInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_tRASAddr.Clear();			
		memset(&m_tGKID,0,sizeof(TALIASADDR));	
		m_tRASAddr.SetNetAddress( 0, RASPORT );
		m_eEndpointType = cmEndpointTypeTerminal;
	}
}TGRQINFO, *PTGRQINFO;

//GCF
typedef struct tagGCFInfo
{	
	TALIASADDR		m_tGKID;				    //GK ID,GK用
	TNETADDR		m_tRASAddr;				    //GK的RAS地址

	tagGCFInfo()
	{
		Clear();
	}

	void Clear()
	{
		memset(&m_tGKID,0,sizeof(TALIASADDR));	
		m_tRASAddr.Clear();		
		m_tRASAddr.SetNetAddress( 0, RASPORT );
	}
}TGCFINFO, *PTGCFINFO;

//add by daiqing 20100720 for 460
//ServiceControlSession
typedef struct tagServiceControlSession
{
	u8       m_bySessionId;
    u8       m_byReason;
}TServiceControlSession, *PTServiceControlSession;

//SCI
typedef struct tagSCIInfo
{
	TServiceControlSession m_tServiceControlSession;
    TNETADDR        m_tCallSignallingAddress;   //信令呼叫地址
	u8	            m_abyCallId[LEN_GUID];      //CallIdentifier
	
	tagSCIInfo()
	{
		Clear();
	}
	
	void Clear()
	{
		memset(&m_tServiceControlSession, 0, sizeof(m_tServiceControlSession));
		m_tCallSignallingAddress.Clear();
		memset(m_abyCallId, 0, LEN_GUID);
	}
	
}TSCIINFO, *PTSCIINFO;
//end


//SCR
typedef struct tagSCRINFO
{
	u16             m_wRequestSeqNum;           //请求序列编号
	TALIASADDR      m_tEndpointID;
	s8	            m_abyCallId[LEN_GUID];      //CallIdentifier
}TSCRINFO;

//H323 Annex Q
typedef struct
{
	u8 m_byAction;			  //start, continue, stop, video source, preset
	
	union 
	{
		u8 m_byRequest;		  //for PTFZ operations
		u8 m_bySourceNo;      //for VIDEO SOURCE operations
		u8 m_byPresetNo;      //for PRESET operations
	}arguments;
	
	u8 m_byTimeout;			  //for start action only ,unit 50ms, 0表示800ms	
}TFeccStruct, *PTFeccStruct;

typedef struct 
{
	TFeccStruct m_tFeccStruct;
	TTERLABEL   m_tSrcTer;
	TTERLABEL   m_tDstTer;
}TH245Fecc;

//H239 Information
typedef struct tagH239Info
{
    emH239InfoType  m_emH239InfoType;
    TTERLABEL       m_tTerLabel;
    s32             m_nChannelId;
    
    /*在emH239FlowControlReleaseResponse
      和emH239PresentationTokenResponse中使用
      TRUE表示同意，FALSE表示拒绝*/
    BOOL            m_bIsResponseAck;   
    
    /*通道码率，单位：100bit/s，范围：1～19200
      在emH239FlowControlReleaseRequest
      和emH239FlowControlReleaseResponse消息中使用      
    */
    u32             m_dwBitRate;          
    
    /*
      MT取1～127之间的随机值;
      MCU可以设为0，但是有具体含义
    */
    u8              m_bySymmetryBreaking; 
    
    tagH239Info()
    {
        Clear();
    }

    void Clear()
    {
        m_emH239InfoType     = emH239InfoTypeInvalid;
        m_bIsResponseAck     = FALSE;
        
        m_dwBitRate          = 0;
        m_bySymmetryBreaking = 0;
        m_nChannelId         = 0;
        
        m_tTerLabel.Clear();
    }

    void host2net(BOOL bhton)
    {
        s32 nH239InfoType = m_emH239InfoType;
        if (bhton)
        {
            m_emH239InfoType = (emH239InfoType)htonl(nH239InfoType);
            m_nChannelId     = htonl(m_nChannelId);
            m_bIsResponseAck = htonl(m_bIsResponseAck);
            m_dwBitRate      = htonl(m_dwBitRate);
        }
        else
        {
            m_emH239InfoType = (emH239InfoType)ntohl(nH239InfoType);
            m_nChannelId     = ntohl(m_nChannelId);
            m_bIsResponseAck = ntohl(m_bIsResponseAck);
            m_dwBitRate      = ntohl(m_dwBitRate);
        }
    }

    BOOL SetH239InfoType(emH239InfoType nH239InfoType)
    {
        if (nH239InfoType>emH239InfoTypeInvalid
            && nH239InfoType<emH239InfoTypeEnd)
        {
            m_emH239InfoType = nH239InfoType;
            return TRUE;
        }
        return FALSE;
    }
    emH239InfoType GetH239InfoType()
    {
        return m_emH239InfoType;
    }
    
    void SetTerLabel(TTERLABEL &tTerlabel)
    {
        m_tTerLabel = tTerlabel;
    }

    TTERLABEL &GetTerlabel()
    {
        return m_tTerLabel;
    }


    void SetSymmetryBreaking(u8 bySymmetryBreaking)
    {
        m_bySymmetryBreaking = bySymmetryBreaking;
    }
    u8 GetSymmetryBreaking()
    {
        return m_bySymmetryBreaking;
    }
    
    void SetResponseAck(BOOL bIsResponseAck)
    {
        m_bIsResponseAck = bIsResponseAck;
    }
    BOOL IsResponseAck()
    {
        return m_bIsResponseAck;
    }
    
    void SetChannelId(HCHAN hsChan)
    {
        m_nChannelId = cmGetLCNFromHchan(hsChan);
    }
    s32 GetChannelId()
    {
        return m_nChannelId;
    }
    void SetChannelId(s32 nChannelId)
    {
        m_nChannelId = nChannelId;
    }
    
}TH239INFO,*PTH239INFO;
//add by zhanghb 100316 
//修改需和kdvpts保持一致
//#define KDV_NSM_MAX_LEN        (s32)1300  //暂时1300字节
#define KDV_NSM_MAX_LEN        (s32)(1024*10)  //暂时1300字节
//非标消息错误号
#define KDVP_NSM_SUCCESS          (u32)0
#define KDVP_NSM_SER_DB_ERR       (u32)1

//非标消息号定义
//add by yj for QUERY [20121212]
// #define KDVP_NSM_START_ID         (u32)0
// #define KDVP_NSM_GETMTLIST_REQ    (u32)(KDVP_NSM_START_ID + 1) //MT 向GK请求在线终端列表
// #define KDVP_NSM_GETMTLIST_RSP    (u32)(KDVP_NSM_START_ID + 2) //回复
typedef enum
{
	KDVP_NSM_START_ID		= 0,
	KDVP_NSM_GETMTLIST_REQ,									//MT 向GK请求在线终端列表
	KDVP_NSM_GETMTLIST_RSP,									//会商平台回复KDVP_NSM_GETMTLIST_REQ

	KDVP_NSM_NEW_BEGIN		= (KDVP_NSM_START_ID + 10),
	KDVP_NSM_PALTFORMCAP_NOTIFY,							//GK告知h323实体平台能力
	KDVP_NSM_GETMTLIST_RSP2,								//会议平台回复KDVP_NSM_GETMTLIST_REQ
	KDVP_NSM_GETMTLIST_REJECT,								//4.2GK回复KDVP_NSM_GETMTLIST_REQ
	
	//MT向GK申请地址簿消息
	KDVP_NSM_ADDRBOOK_ENTRY_REQ,
	KDVP_NSM_ADDRBOOK_ENTRY_REJECT,
	KDVP_NSM_ADDRBOOK_ENTRY_RSP,
	
	KDVP_NSM_ADDRBOOK_GROUP_REQ,
	KDVP_NSM_ADDRBOOK_GROUP_REJECT,
	KDVP_NSM_ADDRBOOK_GROUP_RSP,
	
	KDVP_NSM_ADDRBOOK_GROUPLEVEL_REQ,
	KDVP_NSM_ADDRBOOK_GROUPLEVEL_REJECT,
	KDVP_NSM_ADDRBOOK_GROUPLEVEL_RSP,
	
	//终端注册 MT<-- >MTCarrier
	KDVP_NSM__REG_REQ,										
	KDVP_NSM_REG_ACK,
	KDVP_NSM_REG_NACK,
	KDVP_NSM__ROUNDTRIP_TIMER,
	
	//0.1 开启录像 MT<-->GK
	KDVP_NSM_STARTP2PREC_REQ,
	KDVP_NSM_STARTP2PREC_REJECT,
	KDVP_NSM_STARTP2PREC_RSP,
	
	//0.2 终止录像 双向的
	KDVP_NSM_STOPP2PREC_CMD,
	KDVP_NSM_P2PRECSTATUS_NTY,
	KDVP_NSM_P2PFASTUPDATE_CMD,
	KDVP_NSM_P2P_COMMON_MSG,

	KDVP_NSM_END_ID           =200,
}emKDVPNsmType;
//end [20121212]
  
#ifdef WIN32
#pragma comment( lib, "ws2_32.lib" ) 
#pragma pack( push )
#pragma pack( 1 )
#define window( x )	x
#define PACKED 
#else
#include <netinet/in.h>
#define window( x )
#define PACKED __attribute__((__packed__))	// 取消编译器的优化对齐
#endif

typedef struct tagNSMInfo
{
	u32          m_dwNSMID;               //消息号
    u32          m_dwErrorCode;           //错误码 回复消息时才有用
    u32          m_dwSequenceNumber;      //序列号 暂时未用
    u32          m_dwContentLen;          //消息体内容长度
    u32          m_dwStructNum;           //消息体的结构体数目
    BOOL         m_bWillContinue;         //是否有后续的消息，用于消息体长度太长的情况
	TALIASADDR   m_tMTAlias;              //终端别名目前只填E164号
	u8           m_bybuf[KDV_NSM_MAX_LEN];//消息体

    tagNSMInfo()
    {
        Clear();
    }
	
    void Clear()
    {
        m_dwNSMID            = 0;
        m_dwErrorCode        = KDVP_NSM_SUCCESS;       
        m_dwSequenceNumber   = 0;
        m_dwContentLen       = 0;
        m_dwStructNum        = 0;
		m_bWillContinue      = FALSE;
        
        memset(m_bybuf,0,KDV_NSM_MAX_LEN);
    }
	//设置别名
	void SetAliasAddr( TALIASADDR tAliasAddr )
	{		
		m_tMTAlias.SetAliasAddr(tAliasAddr.GetAliasType(),tAliasAddr.GetAliasName());
		m_tMTAlias.m_emType = (AliasType)htonl(tAliasAddr.m_emType);
	}
	//获取别名类型
    void GetAliasAddr( TALIASADDR &tAliasAddr)
	{
		tAliasAddr.m_emType = (AliasType)ntohl(m_tMTAlias.m_emType);
		tAliasAddr.SetAliasAddr(tAliasAddr.GetAliasType(),m_tMTAlias.GetAliasName());
	}
	void SetNSMID(u32 dwNSMID)
	{ 
		m_dwNSMID = htonl(dwNSMID);
	}
	u32 GetNSMID()
	{
		return ntohl(m_dwNSMID);
	}
	void SetErrorCode(u32 dwErrorCode)
	{
		m_dwErrorCode = htonl(dwErrorCode);
	}
	u32 GetErrorCode()
	{
		return ntohl(m_dwErrorCode);
	}
	void SetSQN(u32 dwSQN)
	{
		m_dwSequenceNumber = htonl(dwSQN);
	}
	u32 GetSQN()
	{
		return ntohl(m_dwSequenceNumber);
	}
	void SetStructNum(u32 dwMsgNum)
	{
		m_dwStructNum = htonl(dwMsgNum);
	}
	u32 GetStructNum()
	{
		return ntohl(m_dwStructNum);
	}
    void SetWillContinue(BOOL bWillContinue)
    {
        m_bWillContinue = htonl(bWillContinue);
    }
    BOOL IsWillContinue()
    {
        return ntohl(m_bWillContinue);
    }
    void SetContentLen(u32 dwContentLen)
    {
        m_dwContentLen = htonl(dwContentLen);
    }
    u32 GetContentLen()
    {
        return ntohl(m_dwContentLen);
    }
	u8* GetMsgData()
	{
		return (u8*)m_bybuf;
	}
	BOOL SetMsgData(u8 * pbuf,u32 dwMsgLen)
	{
		if (pbuf == NULL || dwMsgLen > KDV_NSM_MAX_LEN)
		{
			return FALSE;
		}
		//memcpy(m_bybuf,0, KDV_NSM_MAX_LEN);
		memset( m_bybuf, 0, KDV_NSM_MAX_LEN );
		memcpy(m_bybuf,pbuf,dwMsgLen);
		return TRUE;
	}
}
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
TNSMInfo,*PTNSMInfo;

typedef struct tagNSM_MTList_RSP_BodyHead
{
private:
    cmEndpointType	m_emEndpointType;       //终端类型
    TNETADDR        m_tRasAddr;             //RAS地址
    u16				m_wCallAddrNum;         //呼叫信令地址的个数    
    u16				m_wAliasNum;            //别名个数
    u32             m_dwAliasBufLen;        //压缩后的别名buf长度
	
public:
	
    void SetTerminalType( cmEndpointType emEndpointType )
    {
        m_emEndpointType = (cmEndpointType)htonl(emEndpointType);
    }
    cmEndpointType GetTerminalType()
    {
        return (cmEndpointType)ntohl(m_emEndpointType);
    }
	
    void SetAliasNum( u16 wAliasNum )
    {
        m_wAliasNum = htons(wAliasNum);
    }
    u16 GetAliasNum()
    {
        return ntohs(m_wAliasNum);
    }
    
    void SetAliasBufLen( u32 dwAliasBufLen )
    {
        m_dwAliasBufLen = htonl(dwAliasBufLen);
    }
    u32 GetAliasBufLen()
    {
        return ntohl(m_dwAliasBufLen);
    }
	
    void SetRASAddr( TNETADDR &tRASAddr )
    {
        m_tRasAddr = tRASAddr;
    }
    TNETADDR* GetRASAddr()
    {
        return &m_tRasAddr;
    }
	
    void SetCallAddrNum( u16 wCallAddrNum )
    {
        m_wCallAddrNum = htons(wCallAddrNum);
    }
    u16 GetCallAddrNum()
    {
        return ntohs(m_wCallAddrNum);
    }
}
#if defined(_VXWORKS_) || defined(_LINUX_)
__attribute__ ( (packed) ) 
#endif // _VXWORKS_
TNSM_MTList_RSP_BodyHead,*PTNSM_MTList_RSP_BodyHead;

#ifdef WIN32
#pragma pack( pop )
#endif

//恢复别名结构
BOOL NSM_DecodeAlias(IN u8 *pbyAliasBuf, IN u16 wAliasNum, IN u32 dwAliasBufLen, OUT TALIASADDR* atAlias, IN u16 wMaxAliasNum);
/***********************mmcu define**************************/

enum H323MCUMsgType
{
    CASCADE_MSGTYPE_BEGINNING,
        
        H_CASCADE_REGUNREG_REQ,   //[IN]&[OUT]:[TRegUnRegReq]	register request
        H_CASCADE_REGUNREG_RSP,   //[IN]&[OUT]:[TRegUnRegRsp]	register response
        
        H_CASCADE_NEWROSTER_NTF,  //[IN]&[OUT]:[TRosterList]  roster notify
        
        H_CASCADE_PARTLIST_REQ,   //[IN]&[OUT]:[TReq]         request part list 
        H_CASCADE_PARTLIST_RSP,   //[IN]&[OUT]:[TPartListRsp] reponse part list (dwpid ==0 表示对方的MCU)
        
        H_CASCADE_INVITEPART_REQ, //[IN]&[OUT]:[TInvitePartReq]  invite part request
        H_CASCADE_INVITEPART_RSP, //[IN]&[OUT]:[Tsp]             invite part response
        H_CASCADE_NEWPART_NTF,    //[IN]&[OUT]:[TPart]           new part notify
        
        H_CASCADE_CALL_ALERTING_NTF, //[IN]&[OUT]:[TCallNtf]
        
        H_CASCADE_REINVITEPART_REQ, //[IN]&[OUT]:[TReInviteReq]    reinvite part request
        H_CASCADE_REINVITEPART_RSP, //[IN]&[OUT]:[Tsp]        reinvite part response
        
        H_CASCADE_DISCONNPART_REQ, //[IN]&[OUT]:[TPartReq]     disconnect part request
        H_CASCADE_DISCONNPART_RSP, //[IN]&[OUT]:[Tsp]         disconnect part response
        H_CASCADE_DISCONNPART_NTF, //[IN]&[OUT]:[TDiscPartNF] disconnect part notify
        
        
        H_CASCADE_DELETEPART_REQ, //[IN]&[OUT]:[TPartReq]		delete part request
        H_CASCADE_DELETEPART_RSP, //[IN]&[OUT]:[Tsp]			delete part response
        H_CASCADE_DELETEPART_NTF, //[IN]&[OUT]:[TPartNtf]   delete part notify (whether or not local)
        
        H_CASCADE_SETIN_REQ,      //[IN]&[OUT]:[TSetInReq]    request video in
        H_CASCADE_SETIN_RSP,      //[IN]&[OUT]:[Tsp]          response msg
        
        H_CASCADE_SETOUT_REQ,     //[IN]&[OUT]:[TSetOutReq]//视频输出到自己的请求(视频流向:MCU->MT)
        H_CASCADE_SETOUT_RSP,     //[IN]&[OUT]:[Tsp]//视频输出到自己的应答(视频流向:MCU->MT)
        
        H_CASCADE_OUTPUT_NTF,     //[IN]&[OUT]:[TPartOutputNtf]      video output notify (dwpid ==0 表示对方的MCU)
        
        H_CASCADE_NEWSPEAKER_NTF, //[IN]&[OUT]:[TNewSpeakerNtf]      newSpeaker output notify (dwpid ==0 表示对方的MCU)
        
        H_CASCADE_AUDIOINFO_REQ,  //[IN]&[OUT]:[TConfVideInfoReq]         audio info request 
        H_CASCADE_AUDIOINFO_RSP,  //[IN]&[OUT]:[TConfAudioInfo]    audio info response
        
        H_CASCADE_VIDEOINFO_REQ,  //[IN]&[OUT]:[pReq]          conference videoinfo request 
        H_CASCADE_VIDEOINFO_RSP,  //[IN]&[OUT]:[TConfVideoInfo] conference videoinfo response
        
        
        H_CASCADE_CONFVIEW_CHG_NTF,//[IN]&[OUT]:[TConfViewChgNtf] conference view format(layout geometry)change notify
        
        H_CASCADE_PARTMEDIACHAN_REQ,   //[IN]&[OUT]:[TPartMediaChanReq] 请求会议成员打开/关闭音/视频/数据通道
        H_CASCADE_PARTMEDIACHAN_RSP,   //[IN]&[OUT]:[TRsp]    response
        H_CASCADE_PARTMEDIACHAN_NTF,   //[IN]&[OUT]:[TPartMediaChanNtf] 会议成员的音/视频/数据通道打开/关闭的通知

        H_CASCADE_GETMCUSERVICELIST_REQ, //[IN]&[OUT]:[TReq] 获取MCU上的服务模板列表的请求消息
		H_CASCADE_GETMCUSERVICELIST_RSP,//[IN]&[OUT]:[暂时未定义]  response

		H_CASCADE_GETMUCCONFLIST_REQ,//[IN]&[OUT]:[TMCUConfListReq] 获取MCU上当前召开的所有会议的列表
		H_CASCADE_GETMUCCONFLIST_RSP,//[IN]&[OUT]:[暂时未定义]
  
        H_CASCADE_GETCONFGIDBYNAME_REQ,//[IN]&[OUT]:[TGetConfIDByNameReq] 通过会议名字获得会议ID的请求消息
        H_CASCADE_GETCONFGIDBYNAME_RSP,//[IN]&[OUT]:[暂时未定义]
		
        H_CASCADE_GET_CONFPROFILE_REQ,//[IN]&[OUT]:[TConfProfileReq] 获取MCU上当前召开的所有会议的profile
		
        H_CASCADE_GET_CONFPROFILE_RSP,//[IN]&[OUT]:[暂时未定义]

        H_CASCADE_NONSTANDARD_REQ,//[IN]&[OUT]:[TNonStandardReq] 非标(自定义)请求消息
        H_CASCADE_NONSTANDARD_RSP,//[IN]&[OUT]:[TNonStandardRsp] 非标(自定义)response消息
		H_CASCADE_NONSTANDARD_NTF,//[IN]&[OUT]:[TNonStandardMsg] 非标(自定义)notify消息
		//
        H_CASCADE_NOT_SUPPORTED,
		//目前我们还不支持,但是radmuc支持的消息类型
        H_CASCADE_VA_REQ,       //[IN]&[OUT]:[TVAReq]
        H_CASCADE_VA_RSP,       //[IN]&[OUT]:[TRsp]

        H_CASCADE_RELEASECONTROL_OF_CONF_REQ,//[IN]&[OUT]:[]
        H_CASCADE_RELEASECONTROL_OF_CONF_RSP,//[IN]&[OUT]:[]

        H_CASCADE_TAKECONTROL_OF_CONF_REQ,//[IN]&[OUT]:[]
        H_CASCADE_TAKECONTROL_OF_CONF_RSP,//[IN]&[OUT]:[]

        H_CASCADE_ALLPART_SETOUT_REQ,//[IN]&[OUT]:[]
        H_CASCADE_ALLPART_SETOUT_RSP,//[IN]&[OUT]:[]
        
        H_CASCADE_CREATE_CONF_REQ,//[IN]&[OUT]:[]
        H_CASCADE_CREATE_CONF_RSP,//[IN]&[OUT]:[]
        
        H_CASCADE_TERMINATE_CONF_REQ,//[IN]&[OUT]:[]
        H_CASCADE_TERMINATE_CONF_RSP,//[IN]&[OUT]:[]

        H_CASCADE_UNDEFINED_REQ,//[IN]&[OUT]:[]
        H_CASCADE_UNDEFINED_RSP,//[IN]&[OUT]:[]
        H_CASCADE_UNDEFINED_NTF,//[IN]&[OUT]:[]

        //add 2007.04.28
        H_CASCADE_SET_LAYOUT_AUTOSWITCH_REQUEST,//[IN]&[OUT]:TSetLayoutAutoswitchReq
        H_CASCADE_SET_LAYOUT_AUTOSWITCH_RESPONSE,
        
        CASCADE_MSGTYPE_ENDING
};

#ifndef H323CASCADE_H_

#define CASCADE_PORT		    (s32)3337
#define CASCADE_USERNAME_MAXLEN     (s32)32
#define CASCADE_USERPASS_MAXLEN     (s32)32 
#define CASCADE_NONSTANDARDMSG_MAXLEN (u32)1536
#define PART_NUM			     (s32)192//
#define STR_LEN                  (s32)16
#define CONFID_BYTELEN                   (s32)12// 
#define PID_POS                          (s32)4//
//#define MAX_VIEWS_PER_CONF               (s32)2
#define MAX_VIEWS_PER_CONF               (s32)4
#define MAX_GEOMETRYS_PER_VIEW           (s32)5
#define MAX_SUBFRAMES_PER_GEOMETRY       (s32)16
#define MAX_VIDEOSCHEMES_PER_VIEW        (s32)4
#define MAX_MIXERS_PER_CONF              (s32)16

#define MCU_PID                          (u32)0XFFFFFFFF
#define CACSCONF_ID                      (u32)0XFFFFFFFF
#define EMPTY_MT_PID                     (u32)0x0


#define E_TOO_MANY_ITEMS                 (s32)-1
#define E_ALREADY_EXISTED                (s32)-2
#define E_INVALID_PARAMETER              (s32)-3
#define E_NOT_EXISTED                    (s32)-4
#define E_SUBFRAMES_CONTRIBUTERS_UNMATCH (s32)-5
//#define S_RIGHT                             (s32)1
#define S_REPLACED                       (s32)2
#define S_RIGHT                          (s32)1

enum emConnState
{
	    emConnStateUnknown,                 //该MT,MCU的连接状态未知，表明它从未
		emConnStateConnected,               //"connected"
		emConnStateDisconnected,            //"disconnected"
		emConnStateConnecting               //"alerting"
};

enum emEPType
{
    	emEPTypeUndefined,                  //不明类型
		emEPTypeMCU,                        //mcu,但是它已一个普通终端的身份参加级联会议或是会议
		emEPTypeTerminal,                   //普通终端
		emEPTypeCascConf                    //mcu,并且参加了级联会议
};



//time config struct
typedef struct tagTimeSpec
{
    s8 day;
    s8 hour;
    s8 minute;
    s8 second;
    s16 year;
    s8 month;
}TTimeSpec,*PTTimeSpec;

//标准请求头,他是所有请求的基类
typedef struct tagRequest
{
    s32  m_nReqID;//流水号/系列号
	s8   m_aszUserName[CASCADE_USERNAME_MAXLEN];//帐号
	s8   m_aszUserPass[CASCADE_USERPASS_MAXLEN];//密码
	//设置帐号和密码
	inline void SetUserNameAndPass(const s8 * aszUserName, const s8 *aszUserPass)
	{
		strncpy(m_aszUserName, aszUserName, CASCADE_USERNAME_MAXLEN-1);
		m_aszUserName[CASCADE_USERNAME_MAXLEN-1] = '\0';
		strncpy(m_aszUserPass, aszUserPass, CASCADE_USERPASS_MAXLEN-1);
		m_aszUserPass[CASCADE_USERPASS_MAXLEN-1] = '\0';
	}
}TReq;


//非标级联消息的基类
typedef struct tagNonStandardMsg
{
	s32 m_nMsgLen;//消息长度
	u8	m_abyMsgBuf[CASCADE_NONSTANDARDMSG_MAXLEN];//消息体
}TNonStandardMsg;

//非标(自定义)请求消息
typedef struct tagNonStandardReq: public TReq, public TNonStandardMsg{}TNonStandardReq;

//response消息的返回值字段
enum emReturnValue
{
    emReturnValue_Ok = 0, //request消息已得到正确的处理
    emReturnValue_Error,//一般性错误，错误原因不明
    emReturnValue_PartNotFound,//会议组中不存在请求消息中PID指示的会议成员
    emReturnValue_ConfNotFound,//请求消息中confID指明的会议组不存在
    emReturnValue_ChannelNotFound,//会议成员的音频/视频通道不存在
    emReturnValue_NoPermission,//用户的权限不够，请求消息不被处理
    emReturnValue_XmlBadParams,//请求消息中字段名或是字段的值类型/范围错误
    emReturnValue_TimeOut,//请求消息处理超时
    emReturnValue_NoResources,//mcu没有足够的资源来满足请求消息对音视频资源的要求
    emReturnValue_Invalid//请求无效,不会被处理
};
typedef struct tagResponse           //Request消息的回应消息(response消息)
{                           
    s32    m_nReqID;//流水号/系列号,它匹配request中的流水号/系列号
    emReturnValue m_emReturnVal;//request处理结果指示符
    tagResponse()
    {
        m_emReturnVal = emReturnValue_Ok;
        m_nReqID =((s32)time(NULL))%((s32)0x7fffffff);
    }
}TRsp;

//非标(自定义)response消息
typedef struct tagNonStandardRsp: public TRsp, public TNonStandardMsg{}TNonStandardRsp;


//通知类型消息(notification)的基类
typedef struct tagNotification
{
    s32    m_nCookie;//cookie值
}TNtf;

//获取MCU上的服务列表的请求消息,它向对端mcu请求此mcu上所有已经定义的服务模板的信息
#define TMCUServiceListReq TReq

//获取MCU上当前召开的所有会议相关信息的列表
typedef struct tagMCUConfListReq: public TReq
{
	s32 m_nSnapShot;//MCU快照值
}TMCUConfListReq;

//通过会议名字获得会议ID的请求消息
typedef struct tagGetConfIDByNameReq:public TReq
{
	s8 m_aszConfName[STR_LEN];//会议名称
	s8 m_aszConfPass[STR_LEN];//会议密码,mcu管理者的密码也同样有效
}TGetConfIDByNameReq;

//请求进行会议级联的注册注销请求
typedef struct tagRegUnReg: public TReq     //向对端mcu注册注销的请求消息
{
    BOOL32 m_bReg;               //true:注册;false 注销
}TRegUnRegReq;

//请求进行会议级联的注册/注销的回应消息(成功 m_emReturnVal为true，否则)
typedef struct tagRegUnRegRsp :public TRsp 
{
	BOOL32 m_bReg;  //true:注册;false 注销            
}TRegUnRegRsp;

//
typedef struct tagPartID           //会议参加者的ID,唯一的表示级连MCU会议中一个参与者
                                  //其中m_dwPID＝0xffffffff表示级连子会议;
{
    u32         m_dwPID;          // part id
    BOOL32      m_bLocal;         // true:会议参加者直接与本地MCU相连 
                                 // false:会议参加者直接与对端MCU相连
    tagPartID ()
    {
        m_dwPID = 0;
        m_bLocal =FALSE;
    }
    inline BOOL32 IsSamePart(tagPartID tPeerPart)
    {
        if(m_dwPID == tPeerPart.m_dwPID && m_bLocal == tPeerPart.m_bLocal)
        {
            return TRUE;
        }
        return FALSE;
    }
    inline BOOL32 IsSamePart(u32 dwPID, BOOL32 bLocal)
    {
        if(m_dwPID == dwPID && m_bLocal == bLocal)
        {
            return TRUE;
        }
        return FALSE;
    }
    inline u32    GetPid() { return m_dwPID ;}
    inline void   SetPid(u32 dwPID) { m_dwPID = dwPID;}
    inline BOOL32 IsLocalPart(){ return m_bLocal;}
    inline BOOL32 IsRemotePart(){ return !m_bLocal;}
    inline BOOL32 IsEmptyPart(){ return m_dwPID == EMPTY_MT_PID;}
    inline void   SetEmpty(){ m_dwPID = EMPTY_MT_PID; m_bLocal = FALSE; }
    inline void   SetIsLocalPart(BOOL32 bLocal = TRUE) { m_bLocal = bLocal; }
    //
    inline BOOL32 IsLocalKdvMCU(){  return (m_dwPID  == CACSCONF_ID && m_bLocal);}
    inline BOOL32 IsRemoteRadMcu(){ return (m_dwPID == CACSCONF_ID && !m_bLocal);}

    inline void SetPartGid(u32 dwPid, BOOL32 bLocal)
    {
        m_dwPID  = dwPid;
        m_bLocal = bLocal;
    }
    
    inline void GetPartGid(u32 *pdwPid, BOOL32 *pbLocal)
    {
        if(pdwPid)
        {
            *pdwPid = m_dwPID;
        }
        if(pbLocal)
        {
            *pbLocal = m_bLocal;
        }
        return;
    }
}TPartID;

//获取MCU上当前召开的所有会议的profile
//我们暂时未定义它的响应消息的结构
typedef struct tagConfProfileReq:public TReq
{
    s32 m_nSnapShot;//
}TConfProfileReq;

//
typedef struct tagPartReq: public TPartID, public TReq{}TPartReq;

//新发言人(Floor)通知
typedef struct tagNewSpeakerNtf:public TPartID,public TNtf
{
    s32 m_nMixerID;  //Mixer ID
    tagNewSpeakerNtf(){memset(this, 0, sizeof(*this)); }
}TNewSpeakerNtf,*PTNewSpeakerNtf;

//
typedef enum 
{
        emMediaTypeNone,    //unknown,invalid
        emMediaTypeVideo,   //video
        emMediaTypeAudio,   //audio
        emMediaTypeData     //data
}emMediaType;               //媒体类型

typedef enum 
{
    emDirectionNone,       //unknown,invalid
    emDirectionIn,        //从终端到MCU
    emDirectionOut       //从MCU到终端
}emMuteDirection;          //某一类媒体流的流向

typedef struct tagPartMediaChan:public TPartID
{
    emMediaType      m_emMediaType;        //媒体类型
    emMuteDirection  m_emMuteDirection;    //媒体流流动方向
    BOOL32           m_bMute;              //true:切断该方向的流动，false:打开该方向的媒体流动
    tagPartMediaChan(){memset(this, 0, sizeof(*this)); }
}TPartMediaChan; 

//会议成员音/视/数据通道打开/关闭通知
typedef struct tagPartMediaChanNtf:public TPartMediaChan,public TNtf{}TPartMediaChanNtf;

//会议成员音/视/数据通道打开/关闭请求
typedef struct tagPartMediaChanReq:public TPartMediaChan,public TReq{}TPartMediaChanReq;

// 一个从MCU到终端的视频输出规范的定义
typedef struct tagViewOutInfo:public TPartID 
{    
    s32 m_nOutViewID;         //LayoutID,输出的会议视图ID
    s32 m_nOutVideoSchemeID;  //VideoSchemeID,视频输出方案ID
}TViewOutInfo,*PTViewOutInfo;

//请求MCU输出视频流到会议成员
typedef struct tagSetOutReq :public TReq
{
    s32              m_dwPartCount;           //请求MCU输出视频到自己的终端数目
    TViewOutInfo     m_tPartOutInfo[PART_NUM];//视频输出规范的列表，每一个列表元素定义了一个终端的视频输出请求

private:
    
     BOOL32 FindPartViewOutItem(u32 dwPartId, BOOL32 bLocal, s32 &nPartIndex)
     {
         for(nPartIndex =0; nPartIndex < m_dwPartCount; nPartIndex++)
         {
             if(m_tPartOutInfo[nPartIndex].IsEmptyPart())
             {
                 return FALSE;
             }
             if(m_tPartOutInfo[nPartIndex].IsSamePart(dwPartId, bLocal))
             {
                 return TRUE;
             }
         }
         return FALSE;
     }

public: 
    tagSetOutReq(){memset(this, 0, sizeof(*this)); }

    inline s32 GetPartCount(){ return m_dwPartCount;}

    s32 AddNewPartViewOutItem(u32 dwPartId, BOOL32 bLocal, s32 nOutViewID, s32 m_nOutVideoSchemeID)
    {
             s32 nPartIndex = 0;
             if( FindPartViewOutItem(dwPartId, bLocal, nPartIndex))
             {
                 return E_ALREADY_EXISTED;
             }
             if(nPartIndex >= PART_NUM)
             {
                 return E_TOO_MANY_ITEMS;
             }
             m_tPartOutInfo[nPartIndex].m_nOutViewID = nOutViewID;
             m_tPartOutInfo[nPartIndex].m_nOutVideoSchemeID = m_nOutVideoSchemeID;
             m_tPartOutInfo[nPartIndex].SetPartGid(dwPartId, bLocal);
             m_dwPartCount++;
             return S_RIGHT;
    }
    ////////////////
    s32 DelPartViewOutItem(u32 dwPartId, BOOL32 bLocal)
    {
        s32 nPartIndex = 0;
        if(! FindPartViewOutItem(dwPartId, bLocal, nPartIndex))
        {
            return E_NOT_EXISTED;
        }
        for(; nPartIndex < m_dwPartCount-1; nPartIndex++)
        {
            memcpy(&(m_tPartOutInfo[nPartIndex]), 
                   &(m_tPartOutInfo[nPartIndex + 1]), sizeof(TViewOutInfo));
        }
        m_dwPartCount--;
        return S_RIGHT;
    }
    
    s32 GetPartViewOutItem(u32 dwPartId, BOOL32 bLocal,s32 &nViewId, s32 &nVideoSchemeId)
    {
        s32 nPartIndex = 0;
        if(! FindPartViewOutItem(dwPartId, bLocal, nPartIndex))
        {
            return E_NOT_EXISTED;
        }
        nViewId        = m_tPartOutInfo[nPartIndex].m_nOutViewID;
        nVideoSchemeId = m_tPartOutInfo[nPartIndex].m_nOutVideoSchemeID;
        return S_RIGHT;
    }

    s32 GetPartViewOutItem(u32 dwPartId, BOOL32 bLocal,TViewOutInfo &tPartViewOut)
    {
        s32 nPartIndex = 0;
        if(! FindPartViewOutItem(dwPartId, bLocal, nPartIndex))
        {
            return E_NOT_EXISTED;
        }
        tPartViewOut.m_nOutViewID        = m_tPartOutInfo[nPartIndex].m_nOutViewID;
        tPartViewOut.m_nOutVideoSchemeID = m_tPartOutInfo[nPartIndex].m_nOutVideoSchemeID;
        tPartViewOut.m_dwPID             = m_tPartOutInfo[nPartIndex].m_dwPID;
        tPartViewOut.m_bLocal            = m_tPartOutInfo[nPartIndex].m_bLocal;
        return S_RIGHT;
    }

    s32 SetPartViewOutItem(u32 dwPartId, BOOL32 bLocal,s32 nViewId, s32 nVideoSchemeId)
    {
        s32 nPartIndex = 0;
        if(! FindPartViewOutItem(dwPartId, bLocal, nPartIndex))
        {
            return E_NOT_EXISTED;
        }
        m_tPartOutInfo[nPartIndex].m_nOutViewID        = nViewId;
        m_tPartOutInfo[nPartIndex].m_nOutVideoSchemeID = nVideoSchemeId;
        return S_RIGHT;
    }

    s32 SetPartViewOutItem(u32 dwPartId, BOOL32 bLocal,TViewOutInfo &tPartViewOut)
    {
        s32 nPartIndex = 0;
        if(! FindPartViewOutItem(dwPartId, bLocal, nPartIndex))
        {
            return E_NOT_EXISTED;
        }
        m_tPartOutInfo[nPartIndex].m_nOutViewID        = tPartViewOut.m_nOutViewID;
        m_tPartOutInfo[nPartIndex].m_nOutVideoSchemeID = tPartViewOut.m_nOutVideoSchemeID;
    }

    inline TViewOutInfo * GetItemByIndex(s32 nIndex)
    { return (nIndex >= 0 && nIndex < m_dwPartCount) ? &(m_tPartOutInfo[nIndex]) : NULL; }

}TSetOutReq,*PTSetOutReq;

//MCU输出视频流到会议成员的通知
typedef struct tagPartOutputNtf:public TPartID,public TNtf
{
    s32 m_nViewID; //终端输出到的会议视图ID
    inline tagPartOutputNtf()
    {
        m_nViewID = m_dwPID = 0;
        m_bLocal = FALSE;
    }
}TPartOutputNtf, *PTPartOutPutNtf;


//会议花名册的一项,表示一个与会成员的基本信息
typedef struct tagTRosterItem:public TPartID
{
    emConnState  m_emConnState;    //连接状态
}TRosterItem, *PTRosterItem;

//会议花名册通知
typedef struct tagRosterList:public TNtf
{
    s32          m_nCount;               //花名册记录条数目
    TRosterItem  m_atRoster[PART_NUM];   //记录条列表
private: 
    BOOL32 FindRosterItem(u32 dwPartId, BOOL32 bLocal, s32 &nIndex)
    {
        for(nIndex =0; nIndex < m_nCount; nIndex++)
        {
            if(m_atRoster[nIndex].IsEmptyPart())
            {
                return FALSE;
            }
            if(m_atRoster[nIndex].IsSamePart(dwPartId, bLocal))
            {
                return TRUE;
            }
        }
        return FALSE;
    }

public:

    inline tagRosterList(){ memset(this, 0, sizeof(*this));}

    BOOL32 AddRosterItem(u32 dwPartId, BOOL32 bLocal, emConnState emState)
    {
        s32  nRosterIndex = 0;
        if( FindRosterItem(dwPartId, bLocal, nRosterIndex))
        {
            return E_ALREADY_EXISTED;
        }

        if( nRosterIndex >= PART_NUM)
        {
            return E_TOO_MANY_ITEMS;
        }
        m_atRoster[nRosterIndex].m_dwPID = dwPartId;
        m_atRoster[nRosterIndex].m_bLocal = bLocal;
        m_atRoster[nRosterIndex].m_emConnState = emState;
        m_nCount ++;
        return S_RIGHT;
    }
    
    BOOL32 DelRosterItem(u32 dwPartId, BOOL32 bLocal, emConnState emState)
    {
        s32  nRosterIndex = 0;
        if(! FindRosterItem(dwPartId, bLocal, nRosterIndex))
        {
            return E_NOT_EXISTED;
        }
        for(; nRosterIndex < m_nCount-1; nRosterIndex++)
        {
            memcpy(&(m_atRoster[nRosterIndex]), 
                &(m_atRoster[nRosterIndex + 1]), sizeof(TRosterItem));
        }
        m_nCount--;
        return S_RIGHT;
    } 
    
    inline TRosterItem * GetItemByIndex(s32 nIndex)
    { return (nIndex >= 0 && nIndex < m_nCount) ? &(m_atRoster[nIndex]) : NULL; }

}TRosterList, *PTRosterList;

//会议视图子窗口的位置标志
typedef struct 
{
    s32  m_nViewID;//会议视图ID
    u8   m_bySubframeIndex;//子窗口在会议视图中位置(按照从左到右从上到下的顺序从0开始计数)
}TViewPos;

//(voice activate)语音激活请求,把当前说话人/或者音量最大的会议成员的视频放置在视图的那个子窗口中
typedef struct tagVAReq:public TReq, public TViewPos{}TVAReq;

//会议成员的视频输出输入信息
typedef struct tagPartVideoInfo
{
    s32       m_nViewCount;                    //终端(会议成员)的视频总共输出到的视图数目
    TViewPos  m_anViewPos[MAX_VIEWS_PER_CONF]; //终端在哪些视图里
    s32       m_nOutputViewID;                 //终端接收哪个视图
    s32       m_nOutVideoSchemeID;             //输入到终端的视频方案ID(当一个会议支持两种会议格式)                        
    
private:
    BOOL32 FindViewPos(s32 nViewID, u8 bySubframeIndex, s32 &nIndex)
    {
        for(nIndex =0; nIndex < m_nViewCount; nIndex++)
        {
            if(m_anViewPos[nIndex].m_nViewID == 0)
            {
                return FALSE;
            }
            if(m_anViewPos[nIndex].m_nViewID == nViewID && m_anViewPos[nIndex].m_bySubframeIndex ==bySubframeIndex)
            {
                return TRUE;
            }
        }
        return FALSE;
    }

public:
    tagPartVideoInfo()
    {
        memset(this,0,sizeof(*this));
    }
    BOOL32 AddViewPos(s32 nViewID,u8 bySubframeIndex)
    {
        s32  nVPosIndex = 0;
        if(nViewID == 0)
        {
            return S_RIGHT;
        }
        if(FindViewPos(nViewID, bySubframeIndex, nVPosIndex))
        {
            return E_ALREADY_EXISTED;
        }
        
        if( nVPosIndex >= MAX_VIEWS_PER_CONF)
        {
            return E_TOO_MANY_ITEMS;
        }
        
        m_anViewPos[nVPosIndex].m_nViewID         = nViewID;
        m_anViewPos[nVPosIndex].m_bySubframeIndex = bySubframeIndex;

        m_nViewCount ++;
        return S_RIGHT;
    }
   
    BOOL32 DelViewPos(s32 nViewID,u8 bySubframeIndex)
    {
        s32  nVPosIndex = 0;
        if(! FindViewPos(nViewID, bySubframeIndex, nVPosIndex))
        {
            return E_NOT_EXISTED;
        }
        for(; nVPosIndex < m_nViewCount-1; nVPosIndex++)
        {
            memcpy(&(m_anViewPos[nVPosIndex]), 
                &(m_anViewPos[nVPosIndex + 1]), sizeof(TViewPos));
        }
        m_nViewCount--;
        return S_RIGHT;
    }

    BOOL32 IsViewPosExist(s32 nViewID, u8 bySubframeIndex)
    {
        s32  nVPosIndex = 0;
        if(! FindViewPos(nViewID, bySubframeIndex, nVPosIndex))
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }

    inline void SetVideoSpec(s32 nNewLID, s32 nNewSchemeID, s32 *nOldLID = NULL, s32 *nOldSchemeID = NULL)
    {
        if(nOldLID)
        {
            *nOldLID = m_nOutputViewID;
        }
        if(nOldSchemeID)
        {
            *nOldSchemeID = m_nOutVideoSchemeID;
        }
        m_nOutputViewID        = nNewLID;
        m_nOutVideoSchemeID = nNewSchemeID;
        return;
    }

    inline void GetVideoSpec(s32 *pnLID, s32 *pnSchemeID)
    {
        if(pnLID)
        {
            *pnLID = m_nOutputViewID;
        }
        if(pnSchemeID)
        {
            *pnSchemeID = m_nOutVideoSchemeID;
        }
    }

}TPartVideoInfo,*PTPartVideoInfo;


/*
 *与会者的视频，音频输入输出的统计信息，包括（带宽，发送/接受的包数目，乱序包数目，
 抖动(jitter)缓冲等,目前没有具体定义
*/
typedef struct tagPartAVStatis
{
  u32 m_dwTotalInBandWidth;
  u32 m_dwTotalOutBandWidth;
  u32 m_dwUnused_AudioIn;
  u32 m_dwUnused_AudioOut;
  u32 m_dwUnused_VideoIn;
  u32 m_dwUnused_VideoOut;
  // Currently We Define the bandwith with fixed value
  // because we did not decide to get extra info about part
  // Once we change to get the extra info, we should rewrite this code here.
  tagPartAVStatis()
  {
      m_dwTotalInBandWidth = 437720;
      m_dwTotalOutBandWidth = 437720; 
      m_dwUnused_AudioIn  = 0;
      m_dwUnused_AudioOut = 0;
      m_dwUnused_VideoIn  = 0;
      m_dwUnused_VideoOut = 0;
      
  }
}TPartAVStatis;


/*
 *与会者的信息
*/
typedef struct tagPart:public TPartID
{
    s8			m_aszPartName[STR_LEN];     //part name
    s8			m_aszDialStr[STR_LEN];      //dial string
    u32			m_dwIP;            //part ip
    emEPType	m_emEPType;        //part type
    s8          m_aszEPDesc[STR_LEN];       //part des(product id)
    PayloadType m_emVideoIn;       //video in format (H263、G711.....None)
    PayloadType m_emVideoOut;      //.....
    PayloadType m_emVideo2In;      //.....
    PayloadType m_emVideo2Out;     //..... 
    PayloadType m_emAudioIn;       //.....
    PayloadType m_emAudioOut;      //..... 
    BOOL32      m_bDataMeeting;    //data meeting  
    BOOL32      m_bVideoMuteIn;    //whether or not video in mute(no camera)
    BOOL32      m_bVideoMuteOut;   //....(no dispaly)
    BOOL32      m_bAudioMuteIn;    //....(no mic)
	BOOL32      m_bAudioMuteOut;   //....(no audio player)
    BOOL32      m_bInvited;        //whether or not is Invited
    BOOL32      m_bMaster;         //whether or not Master ternial
    TTimeSpec   m_tStartTime;      //call start time
    emConnState m_emConnState;     //connect state
    BOOL32      m_bFECCEnable;     //whether or not enable FECC
    TPartVideoInfo m_tPartVideoInfo;   //视频输出输入信息
    TPartAVStatis  m_tAVStatis;    //视频音频输入输出统计信息

    tagPart()
    {
        memset(this, 0, sizeof(*this));
    }
    inline void SetVideoAttr(PayloadType emVideoIn, PayloadType emVideoOut, 
                                PayloadType emVideo2In, PayloadType emVideo2Out, 
                                BOOL32 bVideoMuteIn, BOOL32 bVideoMuteOut)
    {
         m_emVideoIn      = emVideoIn;      
         m_emVideoOut     = emVideoOut; 
         m_emVideo2In      = emVideo2In;      
         m_emVideo2Out     = emVideo2Out; 
         m_bVideoMuteIn   = bVideoMuteIn;    
         m_bVideoMuteOut  = bVideoMuteOut;   
    }

    inline void GetVideoAttr(PayloadType *pemVideoIn, PayloadType *pemVideoOut, 
                                PayloadType *pemVideo2In, PayloadType *pemVideo2Out,
                                BOOL32 *pbVideoMuteIn, BOOL32 *pbVideoMuteOut)
    {
        if(pemVideoIn)
        { 
            *pemVideoIn = m_emVideoIn;
        }
        if(pemVideoOut)
        { 
            *pemVideoOut = m_emVideoOut; 
        }
        if(pemVideo2In)
        { 
            *pemVideo2In = m_emVideo2In;
        }
        if(pemVideo2Out)
        { 
            *pemVideo2Out = m_emVideo2Out; 
        }
        if(pbVideoMuteIn)
        { 
            *pbVideoMuteIn = m_bVideoMuteIn;
        }
        if(pbVideoMuteOut)
        { 
            *pbVideoMuteOut = m_bVideoMuteOut;
        }
        return;
    }
    
    inline void SetViewInAndOutAttr(TPartVideoInfo &tPartVideoInfo)
    {
        memcpy(&m_tPartVideoInfo, &tPartVideoInfo,sizeof(TPartVideoInfo));
    }

    inline TPartVideoInfo* GetViewInAndOutAttr()
    {
        return &m_tPartVideoInfo;
    }
    
    inline void SetAudioAttr(PayloadType emAudioIn, PayloadType emAudioOut, BOOL32 bAudioMuteIn, BOOL32 bAudioMuteOut)
    {
        m_emAudioIn      = emAudioIn;      
        m_emAudioOut     = emAudioOut;     
        m_bAudioMuteIn   = bAudioMuteIn;    
        m_bAudioMuteOut  = bAudioMuteOut;   
    }

    inline void GetAudioAttr(PayloadType *pemAudioIn, PayloadType *pemAudioOut, BOOL32 *pbAudioMuteIn, BOOL32 *pbAudioMuteOut)
    {
        if(pemAudioIn)
        { 
            *pemAudioIn = m_emAudioIn;
        }
        if(pemAudioOut)
        { 
            *pemAudioOut = m_emAudioOut; 
        }
        if(pbAudioMuteIn)
        { 
            *pbAudioMuteIn = m_bAudioMuteIn;
        }
        if(pbAudioMuteOut)
        { 
            *pbAudioMuteOut = m_bAudioMuteOut;
        }
        return;
    }

    inline void SetNameAttr(const s8 *pszName, s32 nNameLen, const s8 *pszDescr, s32 nDescrLen)
    {
        // assert(pszName != NULL);
        s32 nStrLen = (nNameLen >= STR_LEN ? STR_LEN -1 : nNameLen );
        strncpy(m_aszPartName, pszName, nStrLen);
        m_aszPartName[nStrLen] = '\0';

        if(pszDescr != NULL && nDescrLen > 0)
        {
            nStrLen = (nDescrLen >= STR_LEN ? STR_LEN -1 : nDescrLen );
            strncpy(m_aszDialStr, pszDescr, nStrLen);
            m_aszDialStr[nStrLen] = '\0';
        }
        else
        {
            m_aszDialStr[0] = '\0';
        }
    }

    inline void GetNameAttr(s8 *pszName, s32 nNameLen, s8 *pszDescr, s32 nDescrLen)
    {
        if(pszName != NULL && nNameLen > 0)
        {
            s32 nStrLen = (nNameLen >= STR_LEN ? STR_LEN -1 : nNameLen );
            strncpy(pszName, m_aszPartName, nStrLen);
            pszName[nStrLen] = '\0';  
        }

        if(pszDescr != NULL && nDescrLen > 0)
        {
            s32 nStrLen = (nDescrLen >= STR_LEN ? STR_LEN -1 : nDescrLen);
            strncpy(pszDescr, m_aszDialStr, nStrLen);     
            pszDescr[nStrLen] = '\0';
        }
    }

    inline void SetDialStr(const s8 *pszDialStr, s32 nLen)
    {
       if(pszDialStr !=NULL && nLen >0)
       {
           s32 nStrLen = (nLen >= STR_LEN ? STR_LEN -1 : nLen);
           strncpy(m_aszDialStr, pszDialStr, nStrLen);
           m_aszDialStr[nStrLen] = '\0';
       }
    }

    inline s8 * GetDialStr() { return this->m_aszDialStr; }

    inline void SetEntityAttr(emConnState emState, emEPType emEntityType, BOOL32 bFECCEnable, BOOL32 bMaster, BOOL32 bInvited)
    {
        m_emConnState    = emState;  
        m_emEPType       = emEntityType;
        m_bFECCEnable    = bFECCEnable;     
        m_bMaster        = bMaster;       
        m_bInvited       = bInvited;       
    }
    inline BOOL32 IsFECCEnable(){ return m_bFECCEnable;}
    inline BOOL32 IsMaster() { return m_bMaster;}
    inline BOOL32 IsInvited() { return m_bInvited;}
    inline emConnState GetConnState() { return m_emConnState;}
    inline void   SetConnState(emConnState emState) { m_emConnState = emState;}
    inline void   StopAsVideoSrc() { m_tPartVideoInfo.m_nViewCount = 0;}
    inline BOOL32 IsVideoSrc() { return (m_tPartVideoInfo.m_nViewCount > 0);}
    inline u32    GetTotalBandWith() { return m_tAVStatis.m_dwTotalInBandWidth + m_tAVStatis.m_dwTotalOutBandWidth;}

    inline void GetEntityAttr(emConnState *pemState, BOOL32 *pbFECCEnable, BOOL32 *pbMaster, BOOL32 *pbInvited)
    {
        if(pemState)
        { 
            *pemState = m_emConnState;
        }
        if(pbFECCEnable)
        { 
            *pbFECCEnable = m_bFECCEnable; 
        }
        if(pbMaster)
        { 
            *pbMaster = m_bMaster;
        }
        if(pbInvited)
        { 
            *pbInvited = m_bInvited;
        }
        return;
    }
    inline void GetVideoScheme(s32 *pnViewId, s32 *pnVideoSchemeId)
    {
        m_tPartVideoInfo.GetVideoSpec(pnViewId,pnVideoSchemeId);
    }
    inline void SetVideoScheme(s32 nViewId, s32 nVideoSchemeId)
    {
        m_tPartVideoInfo.SetVideoSpec(nViewId, nVideoSchemeId, NULL, NULL);
    }
}TPart,*PTPart;

//级连会议与会者的信息
typedef struct tagConfPartInfo
{
    s32    m_dwPartCount;       //fact part count
    s32    m_nSnapShot;         //会议的快照值
        //The first Element Denotes MCU, The Record series from second Denote Terminals 
    TPart  m_atParts[PART_NUM]; // part list 
 private:
     BOOL32 FindPart(u32 dwPartId, BOOL32 bLocal, s32 &nIndex)
     {
         for(nIndex =0; nIndex < m_dwPartCount; nIndex++)
         {
             if(m_atParts[nIndex].IsEmptyPart())
             {
                 return FALSE;
             }
             if(m_atParts[nIndex].IsSamePart(dwPartId, bLocal))
             {
                 return TRUE;
             }
         }
         return FALSE;
     }
     
 public:
     void Clear()
     {
         memset(this, 0, sizeof(*this));
     }
     tagConfPartInfo()
     {
         memset(this, 0, sizeof(*this));
     }
     s32 AddPart(TPart &tPart, BOOL32 bReplace = TRUE)
     {
         s32  nPartIndex = 0;
         if(FindPart(tPart.m_dwPID, tPart.m_bLocal, nPartIndex))
         {
             if(bReplace)
             {
                 memcpy(&m_atParts[nPartIndex], &tPart, sizeof(TPart));
                 return S_RIGHT;
             }
             else
             {
                 return E_ALREADY_EXISTED;
             }
         }
         if( nPartIndex >= PART_NUM)
         {
             return E_TOO_MANY_ITEMS;
         }
         
         memcpy(&m_atParts[nPartIndex], &tPart, sizeof(TPart));
         m_dwPartCount ++;
         return S_RIGHT;
     }

    s32 DelPart(u32 dwPartId, BOOL32 bLocal)
     {
         s32 nPartIndex = 0;
         if(! FindPart(dwPartId, bLocal, nPartIndex))
         {
             return E_NOT_EXISTED;
         }
         for(; nPartIndex < m_dwPartCount-1; nPartIndex++)
         {
             memcpy(&(m_atParts[nPartIndex]), 
                 &(m_atParts[nPartIndex + 1]), sizeof(TPart));
         }
         m_dwPartCount--;
         return S_RIGHT;
     }
    
     s32 Disconnect(u32 dwPartId, BOOL32 bLocal, u32 dwNewPartId)
     {
         s32 nPartIndex = 0;
         if(! FindPart(dwPartId, bLocal, nPartIndex))
         {
             return E_NOT_EXISTED;
         }
         if(m_atParts[nPartIndex].GetConnState() == emConnStateDisconnected)
         {
             return E_ALREADY_EXISTED;
         }
         m_atParts[nPartIndex].m_dwPID = dwNewPartId;
         m_atParts[nPartIndex].SetConnState(emConnStateDisconnected);
         memset(&(m_atParts[nPartIndex].m_tPartVideoInfo), 0, sizeof(TPartVideoInfo));
         
         return S_RIGHT;
     }
    
     TPart *GetPart(u32 dwPartId, BOOL32 bLocal)
     {
         s32 nPartIndex = 0;
         if(! FindPart(dwPartId, bLocal, nPartIndex))
         {
             return NULL;
         }
         return &(m_atParts[nPartIndex]);
     }

     inline TPart *GetLocalMCU()
     {
         return GetPart(CACSCONF_ID, TRUE);
     }

     inline TPart *GetPeerMCU()
     {
         return GetPart(CACSCONF_ID, FALSE);
     }

     BOOL32 IsPartExist(u32 dwPartId, BOOL32 bLocal)
     {
         s32 nPartIndex = 0;
         if(! FindPart(dwPartId, bLocal, nPartIndex))
         {
             return FALSE;
         }
         return TRUE;
     }
     
     inline TPart *GetItemByIndex(s32 nPosFrom)
     {
         return (nPosFrom >= 0 && m_dwPartCount > nPosFrom ? &m_atParts[nPosFrom] : NULL);
     }
     
}TConfPartInfo;

//对方MCU级连子会议的回应列表
typedef struct tagPartListRsp :public TConfPartInfo,public TRsp
{
   BOOL32 m_bLastPack;			//whether or not last pack，used only while receiving
   public:
    tagPartListRsp() { m_emReturnVal = emReturnValue_Ok; m_bLastPack = TRUE;}
    void Clear()
    { 
        memset(this, 0, sizeof(*this));
        m_emReturnVal = emReturnValue_Ok; 
        m_bLastPack = TRUE;
    }
    void BuildToPartListRsp(TConfPartInfo &tConfPart)
    {
        s32 nMemSize = 0;
        nMemSize = tConfPart.m_dwPartCount * sizeof(TPart);
        memcpy(&m_atParts[0], &tConfPart.m_atParts[0], nMemSize);
        m_nSnapShot = tConfPart.m_nSnapShot;
        m_dwPartCount = tConfPart.m_dwPartCount ;
        m_bLastPack = TRUE;
        m_emReturnVal = emReturnValue_Ok;
    }
    
    BOOL32 ExtractConfPartList(TConfPartInfo &tConfPart)
    {
        s32 nIndex      = 0;
        while( nIndex < m_dwPartCount)
        {
            tConfPart.AddPart(m_atParts[nIndex]);
            nIndex ++;
        }
        if(m_bLastPack)        
        {
            tConfPart.m_nSnapShot = m_nSnapShot;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
}TPartListRsp,*PTPartListRsp;

//PartList消息收发头结构，后面跟m_dwPartCount个TPart结构
typedef struct tagPartListMsg: public TRsp
{
    s32    m_dwPartCount;       //fact part count
    s32    m_nSnapShot;         //会议的快照值
    BOOL32 m_bLastPack;         //whether or not last pack，used only while receiving
} TPartListMsg,*PTPartListMsg;


//邀请终端加入会议的参数
typedef struct tagInvitePart
{
    s8   m_aszDialStr[STR_LEN];   //dial string ("123"、"100.1.1.1"、"h323id")
    u32  m_dwCallRate;                //call rate (kbps)
    
    //optional,advanced invitation parameter
    TPartVideoInfo m_tPartVideoInfo;
    
    tagInvitePart()
    {
        memset(this, 0, sizeof(*this));
    }
    
    void SetBasicInviteeParam(const s8 *pszDialStr, s32 nStrLen, u32 dwCallRate)
    {
        if(pszDialStr != NULL && nStrLen >0)
        {
            nStrLen = (nStrLen >= STR_LEN ? STR_LEN -1 : nStrLen );
            strncpy(m_aszDialStr, pszDialStr, nStrLen);
            m_aszDialStr[nStrLen] = '\0';           
        }
        m_dwCallRate = dwCallRate;
    }

    void GetBasicInviteeParam(s8 *pszDialStr, s32 nStrLen, u32 *pdwCallRate)
    {
        if(pszDialStr != NULL && nStrLen >0)
        {
            nStrLen = (nStrLen >= STR_LEN ? STR_LEN -1 : nStrLen );
            strncpy(pszDialStr, m_aszDialStr, nStrLen);
            pszDialStr[nStrLen] = '\0';
        }
        if(pdwCallRate)
        {
            *pdwCallRate = m_dwCallRate;
        }
        return;
    }

    inline void SetInviteeSetInAndOutAttr(TPartVideoInfo &tPartVideoInfo)
    {
        memcpy(&m_tPartVideoInfo, &tPartVideoInfo, sizeof(TPartVideoInfo));
    }

    inline TPartVideoInfo GetInviteeSetInAndOutAttr()
    {
       return m_tPartVideoInfo;
    }
  
}TInvitePart,*PTInvitePart;


/*
  邀请对象加入级连会议的消息体  
*/
typedef struct tagInvitePartReq: public TReq
{
	s32          m_nCount;           //同时受要求的对象的数目
    TInvitePart  m_atPart[PART_NUM]; //受邀请对象信息
public:
     tagInvitePartReq()
     {
        memset(this, 0, sizeof(*this));
     }

     inline BOOL32 AddInvitee(TInvitePart &tInvitee)
     {
         if(m_nCount >= PART_NUM)
         {
             return FALSE;
         }
         memcpy(&m_atPart[m_nCount], &tInvitee,sizeof(TInvitePart));
         m_nCount++;
         return TRUE;
     }

     inline TInvitePart *GetItemByIndex(s32 nPos)
     { return (nPos >= 0 && nPos < m_nCount) ? &(m_atPart[nPos]) : NULL; }

}TInvitePartReq;

//重新要求已经断开的终端加入会议
typedef struct tagReinvitePartReq: public TPartID, public TReq{}TReInvitePartReq;

//MCU_MCU_CALL_ALERTING_NTF 消息体
typedef struct tagCallNtf : public TPartID, public TNtf
{
	s8   m_aszDialStr[STR_LEN];   //dial string ("123"、"100.1.1.1"、"h323id")
   
    tagCallNtf()
    {
        memset(this, 0, sizeof(*this));
    }
    
    void inline GetPartCallingParam(s32 *pdwPartId, BOOL32 *pbLocal,s8 *pszDialStr, s32 nStrLen)
    {
        if(pdwPartId)
        {
            *pdwPartId = m_dwPID;
        }
        if(pbLocal)
        {
            *pbLocal = m_bLocal;
        }
        if(pszDialStr != NULL && nStrLen >0)
        {
            nStrLen = (nStrLen >= STR_LEN ? STR_LEN -1 : nStrLen );
            strncpy(pszDialStr, m_aszDialStr, nStrLen);
            pszDialStr[nStrLen] = '\0';
        }
    }

    void inline SetPartCallingParam(u32 dwPartId, BOOL32 bLocal,s8 *pszDialStr, s32 nStrLen)
    {
        m_dwPID  = dwPartId;
        m_bLocal = bLocal;
        if(pszDialStr != NULL && nStrLen >0)
        {
            nStrLen = (nStrLen >= STR_LEN ? STR_LEN -1 : nStrLen );
            strncpy(m_aszDialStr, pszDialStr, nStrLen);
            m_aszDialStr[nStrLen] = '\0';
        }
    }
}TCallNtf;

//请求终端输出图像到会议的某个视图
typedef struct tagSetInReq:public TPartID, public TReq
{
    s32 m_nViewID;          //图像输出到达的会议视图
    u8  m_bySubFrameIndex;  //会议视图中的子图像(subframe)索引
    tagSetInReq()
    {
        memset(this, 0, sizeof(*this));
    }
    
    void SetPartSetInParam(u32 dwPartId, BOOL32 bLocal, s32 nViewID, u8 bySubFrameIndex)
    {
        m_dwPID =  dwPartId;
        m_bLocal = bLocal;
        m_nViewID = nViewID;
        m_bySubFrameIndex = bySubFrameIndex;
    }

    void GetPartSetInParam(s32 *pdwPartId, BOOL32 *pbLocal, s32 *pnViewID, u8 *pbySubFrameIndex)
    {
        if(pdwPartId)
        {
            *pdwPartId = m_dwPID;
        }
        if(pbLocal)
        {
            *pbLocal = m_bLocal;
        }
        if(pnViewID)
        {
            *pnViewID = m_nViewID;
        }
        if(pbySubFrameIndex)
        {
            *pbySubFrameIndex = m_bySubFrameIndex;
        }
    }
    
}TSetInReq;

/*
 *与会者从级连会议中断开的通告，
*/
typedef struct tagDiscPartNF : public TPartID, public TNtf
{
    u32 m_dwNewPID;   //new part id
    tagDiscPartNF()
    {
        m_dwNewPID = 0;
    }
}TDiscPartNF,*PTDiscPartNF;

//混音器信息，可能会有多个speaker，这里只是制定一个
typedef struct tagAudioMixerInfo
{
    s32    m_nMixerID;             //mixer ID
    u32    m_dwSpeakerPID;         //speaker Pid
    BOOL32 m_bLocal;               //romote or local?
    tagAudioMixerInfo()
    {
        m_nMixerID = m_dwSpeakerPID = 0;
        m_bLocal = FALSE;
    }
    inline BOOL32 IsSpeakerExist(u32 dwPartId, BOOL32 bLocal)
    {
        return (m_dwSpeakerPID == dwPartId && m_bLocal == bLocal);
    }
}TAudioMixerInfo, *PTAudioMixerInfo;

//MCU的混音器的信息
typedef struct tagConfAudioInfo:public TRsp
{
    u8 m_byMixerCount;                                 //混音器数目
    u8 m_byDefaultMixerIndex;                          //缺省混音器，以索引表示,它也是当前正在播音的混音器
    s32 m_nSnapShot;                                    //会议快照值
    TAudioMixerInfo m_tMixerList[MAX_MIXERS_PER_CONF]; //混音器列表

 private:
     BOOL32 FindSpeaker(u32 dwPartId, BOOL32 bLocal, s32 &nMixerIndex)
     {
         for(nMixerIndex =0; nMixerIndex < m_byMixerCount; nMixerIndex++)
         {
             if(m_tMixerList[nMixerIndex].m_nMixerID == 0 || m_tMixerList[nMixerIndex].m_dwSpeakerPID == 0)
             {
                 return FALSE;
             }
             if(m_tMixerList[nMixerIndex].IsSpeakerExist(dwPartId, bLocal))
             {
                 return TRUE;
             }
         }
         return FALSE;
     }
 public:
     tagConfAudioInfo()
     {
         memset(this, 0, sizeof(*this));
         m_emReturnVal = emReturnValue_Ok;
         m_nSnapShot = 0;
     }

     void Clear(void)
     {
         memset(this, 0, sizeof(*this));
         m_emReturnVal = emReturnValue_Ok;
         m_nSnapShot = 0;
     }
     
     BOOL32 AddNewSpeaker(u32 dwPartId, BOOL32 bLocal, s32 nMixerId)
     {
         s32 nMixerIndex = 0;
         if(FindSpeaker(dwPartId, bLocal, nMixerIndex))
         {
             m_tMixerList[nMixerIndex].m_nMixerID = nMixerId;
             return FALSE;
         }
         if(nMixerIndex >= MAX_MIXERS_PER_CONF)
         {
             return FALSE;
         }

         m_tMixerList[nMixerIndex].m_dwSpeakerPID = dwPartId;
         m_tMixerList[nMixerIndex].m_bLocal       = bLocal;
         m_tMixerList[nMixerIndex].m_nMixerID     = nMixerId;

         m_byMixerCount++;
         return TRUE;   
     }
     
     BOOL32 DelSpeaker(u32 dwPartId, BOOL32 bLocal)
     {
         s32 nMixerIndex = 0;
         if(! FindSpeaker(dwPartId, bLocal, nMixerIndex))
         {
             return E_NOT_EXISTED;
         }
         
         if(IsCurrentSpeaker(dwPartId, bLocal))
         {
             m_byDefaultMixerIndex = 0;
         }

         for(; nMixerIndex < m_byMixerCount-1; nMixerIndex++)
         {
             memcpy(&(m_tMixerList[nMixerIndex]), 
                 &(m_tMixerList[nMixerIndex + 1]), sizeof(TAudioMixerInfo));
         }

         m_byMixerCount--;
         return S_RIGHT;        
     }
     
     inline void GetCurrentSpeaker(u32 &dwPartId, BOOL32 &bLocal)
     {
         dwPartId = m_tMixerList[m_byDefaultMixerIndex].m_dwSpeakerPID;
         bLocal   = m_tMixerList[m_byDefaultMixerIndex].m_bLocal;
     }

     s32 SetCurrentSpeaker(u32 dwPartId, BOOL32 bLocal)
     {
         s32 nMixerIndex = 0;
         if(FindSpeaker(dwPartId, bLocal, nMixerIndex))
         {
             m_byDefaultMixerIndex = nMixerIndex;
             return S_RIGHT;
         }
         s32 nNewMixerId = (nMixerIndex + 1) * 10;
         AddNewSpeaker(dwPartId, bLocal, nNewMixerId);
         m_byDefaultMixerIndex = nMixerIndex;
         return nNewMixerId;
     }

     inline BOOL32 IsCurrentSpeaker(u32 dwPartId, BOOL32 bLocal)
     {
         return (m_tMixerList[m_byDefaultMixerIndex].m_dwSpeakerPID == dwPartId
                && m_tMixerList[m_byDefaultMixerIndex].m_bLocal == bLocal);
     }
}TConfAudioInfo;



//会议视图几何布局变更通知
typedef struct tagConfViewChgNtf: public TNtf
{
    s32                   m_nViewID;                                 //发生几何布局变化的会议视图ID
    u8                    m_dwSubFrameCount;                           //变化后的几何布局划分成的子图像数目(subframe)
}TConfViewChgNtf;

//MCU的视频输出方案
typedef struct tagOutPutVideoSchemeInfo
{
    s32           m_nOutputVideoSchemeID;   //视频输出方案ID
    u32           m_dwMaxRate;              //max rate (kbps)
    u32           m_dwMinRate;              //min rate (kbps)
    BOOL32        m_bCanUpdateRate;         // whether or not enable update rate
    s32           m_nFrameRate;             // frame rate (30、25)
    PayloadType   m_emPayLoad;              // payload    (h261,h263,etc)
    tagOutPutVideoSchemeInfo()
    {
        memset(this, 0, sizeof(*this));
    }
}TOutPutVideoSchemeInfo,*PTOutPutVideoSchemeInfo;


//一个会议视图的定义，它包括一系列的几何布局，一系列的视图视频源(由pid来标志)的列表，
//以及可选的输出该会议视图的几个视频输出方案(rad mcu 中最多四个).
typedef struct tagViewInfo
{
    s32                m_nViewId;                                           //会议视图 ID
    emResolution       m_emRes;                                             //会议视图图像的分辨率
    u8                 m_byCurrentGeometryIndex;                            //会议视图当前正在使用(输出)的几何布局,以在几何布局列表中的索引来标明
    u8                 m_byGeometryCount;                                   //几何布局的数目
    u8                 m_abyGeometryInfo[MAX_GEOMETRYS_PER_VIEW];            //几何布局列表
    u8                 m_byContributePIDCount;                              //会议视图的视频源的数目
    TPartID            m_atContributePIDs[MAX_SUBFRAMES_PER_GEOMETRY];         //视频源列表
    u8                 m_byVideoSchemeCount;                                //可选的视频输出方案数目
    TOutPutVideoSchemeInfo    m_atVideoSchemeList[MAX_VIDEOSCHEMES_PER_VIEW];     //视频输出方案列表
  
    //add 2007.04.28
    BOOL32             m_bAutoSwitchStatus;
    BOOL32             m_bDynamic;
    s32                m_nAutoSwitchTime;
    s32                m_nVideoRole;
    

    inline tagViewInfo()
    {
        memset(this, 0, sizeof(*this));
    }
    
    inline void SetViewResolution(emResolution emRes)
    {
        m_emRes = emRes;
    }
    
    inline emResolution GetViewResolution()
    {
        return m_emRes;
    }
    
    inline void GetCurrentContributerList(TPartID **pptContributerParts, s32 &nArrayLen, s32 *pnCurrentSubframes)
    {
        if(pptContributerParts)
        {
           *pptContributerParts = m_atContributePIDs;
            nArrayLen = m_byContributePIDCount;
        }
        if(pnCurrentSubframes)
        {
           *pnCurrentSubframes = m_abyGeometryInfo[m_byCurrentGeometryIndex];
        }
        return;
    }

    inline BOOL32 SetCurrentContributerList(TPartID *ptContributerParts, s32 nArrayLen)
    {
        if(ptContributerParts != NULL && nArrayLen > 0)
        {
            s32 nSubFrameCount = 0;
            nSubFrameCount =(nArrayLen <= MAX_SUBFRAMES_PER_GEOMETRY ?
                                     nArrayLen : MAX_SUBFRAMES_PER_GEOMETRY);
            s32 nIndex = 0;
            while(nIndex < MAX_SUBFRAMES_PER_GEOMETRY && m_abyGeometryInfo[nIndex] != 0 && m_abyGeometryInfo[nIndex] != nSubFrameCount)
            {
                nIndex ++;
            }
            if(nIndex >= MAX_SUBFRAMES_PER_GEOMETRY)
            {
                return FALSE;
            }
            if(m_abyGeometryInfo[nIndex] == 0)
            {
                m_byContributePIDCount ++;
                m_byGeometryCount ++;
                m_abyGeometryInfo[nIndex] = nSubFrameCount;  
            }
            m_byCurrentGeometryIndex = nIndex;
            memcpy(m_atContributePIDs, ptContributerParts, sizeof(TPartID)*nSubFrameCount);
        }
        else
        {
            m_byGeometryCount        = 1;
            m_abyGeometryInfo[0]     = 1;
            m_byCurrentGeometryIndex = 0;
            m_byContributePIDCount   = 0;
        }
        return TRUE;
    }

    inline void SetGeometryList(u8 *pbyGeometryList, s32 nItemCount, s32 nIndexCurrentGeometry = 0)
    {
        s32 nGeometryCount = 0;
        nGeometryCount = (nItemCount < MAX_GEOMETRYS_PER_VIEW ?
                         nItemCount : MAX_GEOMETRYS_PER_VIEW);
        memcpy(m_abyGeometryInfo, pbyGeometryList,nGeometryCount * sizeof(u8));
        m_byGeometryCount = nGeometryCount;
        m_byCurrentGeometryIndex = ((m_byGeometryCount -1 > nIndexCurrentGeometry) ?
                                    nIndexCurrentGeometry : m_byGeometryCount -1);
        return;
    }
    
    inline void GetGeometryList(u8 **ppbyGeometryList, s32 *pnItemCount, s32 *pnIndexCurrentGeometry = NULL)
    {
        if(ppbyGeometryList && pnItemCount) 
        {
            *ppbyGeometryList = m_abyGeometryInfo;
            *pnItemCount      = m_byGeometryCount;
        }
        if(pnIndexCurrentGeometry)
        {
            *pnIndexCurrentGeometry = m_byCurrentGeometryIndex;
        }
        return;
    }
    
    inline void SetVideoSchemeList(TOutPutVideoSchemeInfo *ptSchemeList, s32 nItemCount)
    {
        this->m_byVideoSchemeCount = nItemCount;
        memcpy(this->m_atVideoSchemeList, ptSchemeList, nItemCount * sizeof(TOutPutVideoSchemeInfo));
    }

    inline void GetVideoSchemeList(TOutPutVideoSchemeInfo **pptSchemeList, s32 &nItemCount)
    {
        *pptSchemeList = this->m_atVideoSchemeList;
        nItemCount = this->m_byVideoSchemeCount;
        return;
    }
    
    inline s32 GetViewId() { return m_nViewId;}
    inline void SetViewId(s32 nViewId) { m_nViewId = nViewId;}

}TView, *PTView;

//
typedef struct tagConfVideInfoReq: public TReq
{
    u8          m_bFullInfo;    //是否请求发送几何布局视图
    tagConfVideInfoReq(){ m_bFullInfo = TRUE; }
}TConfVideInfoReq;
//级连会议在单个MCU上的视频输入输出信息
typedef struct tagConfVideoInfo:public TRsp
{
    u8          m_byConfViewCount;                              //会议视图的数目
    //默认使用的会议视图,如果与该MCU直连的终端或是MCU在连接建立时没有指定会议视图信息，
    //则默认观看该会议视图
    u8          m_byDefaultViewIndex;                            //默认会议视图索引
    u8          m_bFullInfo;                                     //是否发送几何布局视图
    s32         m_nSnapShot;                                     //会议快照值
    TView       m_atViewInfo[MAX_VIEWS_PER_CONF]; //会议视图列表
    
    private:
        BOOL32 FindView(s32 nViewId, s32 &nViewIndex)
        {
            for(nViewIndex =0; nViewIndex < m_byConfViewCount; nViewIndex++)
            {
                if(m_atViewInfo[nViewIndex].m_nViewId == 0)
                {
                    return FALSE;
                }
                if(m_atViewInfo[nViewIndex].m_nViewId == nViewId)
                {
                    return TRUE;
                }
            }
            return FALSE;
        }
        
    public:

        inline tagConfVideoInfo()
        {
            m_byConfViewCount = m_byDefaultViewIndex = 0;
            m_emReturnVal = emReturnValue_Ok;
            m_bFullInfo = TRUE;
        }
        inline void Clear(void)
        {
            memset(this, 0, sizeof(*this));
            m_byConfViewCount = m_byDefaultViewIndex = 0;
            m_emReturnVal = emReturnValue_Ok;
        }
        s32 AddView(TView &tView, BOOL32 bDefaultView = FALSE, BOOL32 bReplace = TRUE)
        {
            s32 nViewIndex = 0;
            if( FindView(tView.GetViewId(), nViewIndex))
            {
                if(bReplace)
                {
                    memcpy(&m_atViewInfo[nViewIndex], &tView, sizeof(TView));
                    if(bDefaultView)
                    {
                        m_byDefaultViewIndex = (u8)nViewIndex;
                    }
                    return S_REPLACED;
                }
                else
                {
                    if(bDefaultView)
                    {
                        m_byDefaultViewIndex = (u8)nViewIndex;
                    }
                    return E_ALREADY_EXISTED;
                }
            }

            if(nViewIndex >= MAX_VIEWS_PER_CONF)
            {
                return E_TOO_MANY_ITEMS;
            }

            memcpy(&m_atViewInfo[nViewIndex], &tView, sizeof(TView));
            if(bDefaultView)
            {
               m_byDefaultViewIndex = (u8)nViewIndex;
            }

            m_byConfViewCount++;
            return S_RIGHT;  
        }
        
        s32 DelView(s32 nViewId)
        {
            s32 nViewIndex = 0;
            if(! FindView(nViewId, nViewIndex))
            {
                return E_NOT_EXISTED;
            }
            if(nViewIndex == m_byDefaultViewIndex)
            {
                m_byDefaultViewIndex = 0;
            }
            memmove(&m_atViewInfo[nViewIndex], &m_atViewInfo[nViewIndex +1],
                   sizeof(TView) * (m_byConfViewCount - nViewIndex - 1));
            
            m_byConfViewCount--;
            return S_RIGHT;
        }
        
        s32 GetView(s32 nViewId, TView **pptView)
        {
            s32 nViewIndex = 0;
            if(! FindView(nViewId, nViewIndex))
            {
                *pptView = NULL;
                 return E_NOT_EXISTED;
            }
            if(pptView)
            {
                *pptView = &m_atViewInfo[nViewIndex];
                return nViewIndex;
            }
            return FALSE;
        }
        
        BOOL32 IsViewExist(s32 nViewId)
        {
            s32 nViewIndex = 0;
            if(! FindView(nViewId, nViewIndex))
            {
                return FALSE;
            }
            return TRUE;
        }
        inline TView* GetItemByIndex(s32 nIndex)
        {
           return nIndex >= 0 && nIndex < m_byConfViewCount ?
               &m_atViewInfo[nIndex] : NULL;
        }
        
        inline s32 GetDefaultViewId()
        { 
            return m_atViewInfo[m_byDefaultViewIndex].GetViewId();
        }

        void DelPartRelationship(s32 nPId, BOOL32 bLocal)
        {
            s32 nIndex            = 0;
            TView *ptView         = NULL;
            TPartID *ptPartId     = NULL;
            s32     nContributors = 0;
            while(ptView = GetItemByIndex(nIndex))
            {
                ptView->GetCurrentContributerList(&ptPartId, nContributors, NULL);
                while(nContributors-- > 0)
                {
                    if((ptPartId + nContributors)->IsSamePart(nPId, bLocal))
                    {
                        //will 
                        (ptPartId + nContributors)->SetEmpty();
                    }
                }           
                nIndex ++;
            }
        }
}TConfVideoInfo;


//add 2007.04.28
typedef struct tagAutoswitchReq: public TReq     //
{
  
    s32         m_nLid;
    s32         m_nAutoSwitchTime;
    s32         m_nAutoSwitchLevel;
    BOOL32      m_bAutoSwitchOn;

public: 
    tagAutoswitchReq()
    {
        Clear();
    }


    void Clear()
    {
        memset(this, 0, sizeof(tagAutoswitchReq));
    }
    
    void SetLid(s32 nLid)
    {
        m_nLid = nLid;
    }

    s32  GetLid()
    {
        return m_nLid;
    }

    void SetAutoSwitchTime(s32 nTime)
    {
        m_nAutoSwitchTime = nTime;
    }

    s32 GetAutoSwitchTime()
    {
        return m_nAutoSwitchTime;
    }

    void SetAutoSwitchLevel(s32 nLevel)
    {
        m_nAutoSwitchLevel = nLevel;
    }

    s32 GetAutoSwitchLevel()
    {
        return m_nAutoSwitchLevel;
    }
    
    void SetAutoSwitchOn(BOOL32 bOn)
    {
        m_bAutoSwitchOn = bOn;
    }

    BOOL32 GetAutoSwitchOn()
    {
        return m_bAutoSwitchOn;
    }

}TAutoswitchReq, *PTAutoswitchReq;

typedef struct tagAutoswitchRsp: public TRsp{}TAutoswitchRsp, PTAutoswitchRsp;

#endif //H323CASCADE_H_ // [8/13/2010 xliang] 


#ifdef WIN32
#pragma pack(1)
#define PACKED 
#endif // WIN32


// 终端地址信息

// 地址类型
#define		ADDR_TYPE_IP		1
#define		ADDR_TYPE_ALIAS		2
// // 
// 别名类型
#define		ALIAS_H323ID		1
#define		ALIAS_E164			2
#define		ALIAS_EMAIL			3

typedef struct tagTerAddr
{
public :
	u8 GetAddrType( void ) { return byAddrType; }
	void SetAddrType( u8 byType ) { byAddrType = byType; }
	
	void SetIpAddr( u32 ip, u16 port )
	{
		dwIp = ip;
		wPort = htons( port );
	}
	u32 GetIpAddr( void ) { return dwIp; }
	u16 GetIpPort( void ) { return ntohs( wPort ); }

	//add by yj for ipv6
	void SetIpAddr_Ipv6( u8* ipv6, u16 port, s16 scopeID)
	{
		memcpy(dwIp_ipv6, ipv6, sizeof(dwIp_ipv6));
		wPort = htons( port );
        swScopeId = scopeID;
	}
	u8* GetIpAddr_Ipv6( void ) { return dwIp_ipv6; }
	s16 GetScopeId( void ) { return swScopeId; }
	//end

	u8 GetAliasType( void ) { return byAliasType; }
	s8* GetAliasName( void ) { return achAlias; }
	BOOL SetAlias( u8 byType, const s8* pchName, u8 byNameLen ) 
	{ 
		if ( pchName == NULL || byNameLen == 0 )
			return FALSE;
		
		byAliasType = byType; 
		memset( achAlias, 0, sizeof( achAlias ) );
		u8 byLen = min( byNameLen, (u8)VALID_LEN_ALIAS );
		memcpy( achAlias, pchName, byLen );
		achAlias[byLen] = '\0';
		return TRUE;
	}
	
	tagTerAddr() 
	{
		memset( this, 0, sizeof( tagTerAddr ) );
	}
	
	void Reset( void )
	{
		memset( this, 0, sizeof( tagTerAddr ) );
	}
	
private :
	u8	byAddrType;		// 地址类型
	u32 dwIp;			// IP(网络序)
	u16	wPort;			// 端口号
	u8	byAliasType;	// 别名类型
	s8 achAlias[ VALID_LEN_ALIAS + 1 ];

	//add by yj for ipv6
    u8	byAddrIpType;		// IP地址类型
    u8 dwIp_ipv6[IPV6_NAME_LEN];			// IPv6地址(非网络序)
	s16 swScopeId;
	//end
}
#if defined(_VXWORKS_) || (defined(_LINUX_) && !defined(_EQUATOR_))
__attribute__ ((packed)) 
#endif
TTERADDRESS,*PTTERADDRESS;

#ifdef WIN32
#pragma pack()
#endif




//初始化级联库
BOOL32 MMcuStackInit(u16 wListenPort,BOOL32 bSockInit = FALSE);
//解初始化级联库
BOOL32 MMcuStackClean(BOOL32 bSockClean = FALSE);

//新的会议级联成功通知
#define 	H_CASCADE_NEW_CALL_NTF   (s32)CASCADE_MSGTYPE_ENDING  
//会议级联断开通知
#define     H_CASCADE_DROP_CALL_NTF  (s32)(H_CASCADE_NEW_CALL_NTF+1)
API void mmcusetlog(u8 level);
API void mmcuhelp();


///////////////////////////////// Outgoing Interface ////////////////////////////////

/*====================================================================
函数名      : NotifyNewCallT
功能        : 交换呼叫句柄
算法实现    : 无
引用全局变量: 无
输入参数说明: hsCall   - 协议栈呼叫句柄
    		  lphaCall - 应用程序呼叫句柄
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (KDVCALLBACK* NotifyNewCallT)(HCALL hsCall, LPHAPPCALL lphaCall);
//add by daiqing 20100723 for 460 kdv
/*====================================================================
函数名      : NotifyNew460CallT
功能        : 交换呼叫句柄
算法实现    : 无
引用全局变量: 无
输入参数说明: hsCall   - 协议栈呼叫句柄
    		  lphaCall - 应用程序呼叫句柄
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (KDVCALLBACK* NotifyNew460CallT)(HCALL hsCall, LPHAPPCALL lphaCall , const void* pBuf , u16 nBufLen );
//end
/*====================================================================
函数名      : NotifyNewChanT
功能        : 交换信道句柄
算法实现    : 无
引用全局变量: 无
输入参数说明: haCall - 应用程序呼叫句柄
			  hsCall - 协议栈呼叫句柄
			  hsChan - 协议栈信道句柄
			  lphaChan - 应用程序信道句柄
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (KDVCALLBACK* NotifyNewChanT)(HAPPCALL haCall,HCALL hsCall,HCHAN hsChan,
										  LPHAPPCHAN lphaChan);

/*====================================================================
函数名      : NotifyCallCtrlMsgT
功能        : 呼叫控制通知函数
算法实现    : 无
引用全局变量: 无
输入参数说明: haCall  - 应用程序呼叫句柄
			  hsCall  - 协议栈呼叫句柄
			  msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (KDVCALLBACK* NotifyCallCtrlMsgT)(HAPPCALL haCall,HCALL hsCall,u16 msgType,
											  const void* pBuf,u16 nBufLen);

/*====================================================================
函数名      : NotifyChanCtrlMsgT
功能        : 信道控制通知函数
算法实现    : 无
引用全局变量: 无
输入参数说明: haCall  - 应用程序呼叫句柄
              hsCall  - 协议栈呼叫句柄
              haChan  - 应用程序信道句柄
			  hsChan  - 协议栈信道句柄
			  msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (KDVCALLBACK* NotifyChanCtrlMsgT)(HAPPCALL haCall,HCALL hsCall,HAPPCHAN haChan,HCHAN hsChan,
											  u16 msgType,const void* pBuf,u16 nBufLen);

/*====================================================================
函数名      : NotifyConfCtrlMsgT
功能        : 会议控制通知函数
算法实现    : 无
引用全局变量: 无
输入参数说明: haCall  - 应用程序呼叫句柄
			  hsCall  - 协议栈呼叫句柄
			  msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (KDVCALLBACK* NotifyConfCtrlMsgT)(HAPPCALL haCall, HCALL hsCall,u16 msgType,const void* pBuf,
											  u16 nBufLen);

/*====================================================================
函数名      : NotifyRasCtrlMsgT
功能        : RAS信息通知函数
算法实现    : 无
引用全局变量: 无
输入参数说明: haCall  - 应用程序呼叫句柄
              hsCall  - 协议栈呼叫句柄
              haRas   - 应用程序RAS句柄
			  hsRas   - 协议栈RAS句柄
			  msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (KDVCALLBACK* NotifyRasCtrlMsgT)(HAPPCALL haCall,HCALL hsCall,HAPPRAS haRas, HRAS hsRas,u16 msgType,
											 const void* pBuf,u16 nBufLen);

/*====================================================================
函数名      : NotifyFeccMsgT
功能        : H.224, H.281功能回调函数
算法实现    : 无
引用全局变量: 无
输入参数说明: hsCall     - 协议栈呼叫句柄
              hsChan     - 协议栈信道句柄
			  feccStruct - 远遥命令(TFeccStruct)
			  terSrc     - 源端终端列表
			  terDst     - 源端终端列表
返回值说明  : 成功 - cb_ok
              失败 - cb_err
====================================================================*/
typedef s32 (KDVCALLBACK *NotifyFeccMsgT)(HCALL hsCall, HCHAN hsChan, 
										  TFeccStruct feccStruct,
										  TTERLABEL terSrc, TTERLABEL terDst);

//MMcu 会议级联控制消息回调函数
typedef int (KDVCALLBACK *NotifyMMcuMsgT)(HAPPCALL haCall,HCALL hsCall,HAPPCHAN haChan,HCHAN hsChan,
                                          u16 msgType,const void* pBuf,u16 nBufLen);


//回调函数指针结构
typedef struct 
{
    NotifyNewCallT		fpNewCall;
    NotifyNewChanT		fpNewChan;
    NotifyCallCtrlMsgT  fpCallCtrlMsg;
    NotifyChanCtrlMsgT  fpChanCtrlMsg;
    NotifyConfCtrlMsgT	fpConfCtrlMsg;
    NotifyRasCtrlMsgT	fpRasCtrlMsg;
    NotifyFeccMsgT      fpFeccMsg;  
    NotifyMMcuMsgT      fpMMcuMsg;    
	//add by daiqing 20100723 for 460 kdv
	NotifyNew460CallT   fpNew460Call;
	//end
}H323EVENT,*LPH323EVENT;

///////////////////////////////// Incoming Interface ////////////////////////////////

/*====================================================================
函	 数   名: kdvInitH323Adapter
功	      能: 初始化H323适配模块
算 法 实  现: 
引用全局变量: g_hApp
输入参数说明: sConfigFile(IN), 协议栈配置文件名
              keyParams(INOUT), 协议栈关键配置参数:
	        1> 从配置文件初始化失败时，根据这些参数初始化协议栈
			2> 从配置文件初始化成功时，从配置文件读取一些参数到该结构中，以备后用
			  
返   回   值: 成功 - &g_hApp
			  失败 - NULL		
====================================================================*/
void*  kdvInitH323Adapter(IN s8* sConfigFile, IN TH323CONFIG *keyParams = NULL);

/*====================================================================
函	 数   名: kdvSetStackAdditionalConfig
功	      能: 设置协议栈配置
算 法 实  现: 
引用全局变量: 
输入参数说明: ptStackConfig   - 协议栈配置
返   回   值: 成功 - act_ok
失败 - act_err
====================================================================*/
s32 kdvSetStackAdditionalConfig(IN TAdditionalConfig* ptStackConfig);

/*====================================================================
函	 数   名: kdvCloseH323Adapter
功	      能: 关闭H323适配模块
算 法 实  现: 
引用全局变量: 无
输入参数说明: 无
返   回   值: 无
====================================================================*/
void kdvCloseH323Adapter();

/*====================================================================
函	 数   名: kdvSetAppCallBack
功	      能: 注册应用回调函数
算 法 实  现: 
引用全局变量: 无
输入参数说明: lpEvent - 回调函数结构指针
返   回   值: 成功 - act_ok
			  失败 - act_err
====================================================================*/
s32  kdvSetAppCallBack(IN LPH323EVENT lpEvent);

/*====================================================================
函	 数   名: kdvCheckMessage
功	      能: 检查协议栈是否收到H.323消息
算 法 实  现: 
引用全局变量: 无
输入参数说明: 
返   回   值: 
====================================================================*/
void kdvCheckMessage();

/*====================================================================
函	 数   名: kdvCreateNewCall
功	      能: 创建新的呼叫，主叫在发送呼叫消息前调用,必须先创建再呼叫
算 法 实  现: 
引用全局变量: 无
输 入 参  数: haCall -   应用程序呼叫句柄
输 出 参  数: lphsCall - 协议栈呼叫句柄
返   回   值: 成功 - act_ok
              失败 - act_err
====================================================================*/
s32 kdvCreateNewCall(IN HAPPCALL haCall, OUT LPHCALL lphsCall, IN bool bSync = true);

/*====================================================================
函	 数   名: kdvCreateNewChan
功	      能: 创建新的呼叫，主叫在发送信道消息前调用,必须先创建再操作
算 法 实  现: 
引用全局变量: 无
输 入 参  数: hsCall -   协议栈呼叫句柄
			  haChan -   应用程序信道句柄
输 出 参  数: lphsCall - 协议栈信道句柄
返   回   值: 成功 - act_ok
              失败 - act_err
====================================================================*/
s32 kdvCreateNewChan(IN HCALL hsCall, IN HAPPCHAN haChan, 
                     OUT LPHCHAN lphsChan, IN bool bSync = true);

/*====================================================================
函	 数   名: kdvCreateRASStartTransaction
功	      能: 创建RAS的Transaction，必须先创建再操作
算 法 实  现: 
引用全局变量: 无
输 入 参  数: haRas       -   应用程序RAS句柄
              transaction -   RAS事件类型
              destAddress -   对于终端是GK地址，对于GK是终端地址
              hsCall      -   协议栈呼叫句柄
输 出 参  数: lphsRas -  协议栈RAS句柄
返   回   值: 成功 - act_ok
失败 - act_err
====================================================================*/
s32 kdvCreateRASStartTransaction(IN  HAPPRAS          haRas,
                                 OUT LPHRAS           lphsRas,
                                 IN  cmRASTransaction transaction,
                                 IN  TNETADDR*        destAddress,
                                 IN  HCALL            hsCall,
                                 IN  bool             bSync = true);

/*====================================================================
函	 数   名: kdvSendRasCtrlMsg
功	      能: 发送RAS控制消息
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsCall  - 呼叫句柄
              hsRas   - RAS句柄
              msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返   回   值: 成功 - act_ok
              失败 - act_err
====================================================================*/
s32 kdvSendRasCtrlMsg(IN HCALL hsCall, IN HRAS hsRas, IN u16 msgType,
                      IN const void* pBuf, IN u16 nBufLen, IN bool bSync = true);

/*====================================================================
函	 数   名: kdvSendCallCtrlMsg
功	      能: 发送呼叫控制消息
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsCall  - 呼叫句柄
              msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返   回   值: 成功 - act_ok
              失败 - act_err
====================================================================*/
s32 kdvSendCallCtrlMsg(IN HCALL hsCall,IN u16 msgType,
                       IN const void* pBuf, IN u16 nBufLen, IN bool bSync = true);

/*====================================================================
函	 数   名: kdvSendChanCtrlMsg
功	      能: 发送信道控制消息
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsChan  - 信道句柄
              msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返   回   值: 成功 - act_ok
              失败 - act_err
====================================================================*/
s32 kdvSendChanCtrlMsg(IN HCHAN hsChan, IN u16 msgType,
                       IN const void* pBuf,IN u16 nBufLen, IN bool bSync = true);

/*====================================================================
函	 数   名: kdvSendConfCtrlMsg
功	      能: 发送会议控制消息
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsCall  - 呼叫句柄
              msgType - 消息类型
			  pBuf    - 参数缓冲区
			  nBufLen - 参数长度
返   回   值: 成功 - act_ok
              失败 - act_err
====================================================================*/
s32 kdvSendConfCtrlMsg(IN HCALL hsCall, IN u16 msgType,
                       IN const void* pBuf, IN u16 nBufLen, IN bool bSync = true);

/*====================================================================
函	 数   名: kdvCheckValid
功	      能: 检查呼叫句柄的合法性
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsCall	- 呼叫句柄
输出参数说明：
返   回   值: FALSE - 无效
              TRUE  - 有效
====================================================================*/
BOOL kdvCheckValid(IN HCALL hsCall);

/*====================================================================
函	 数   名: kdvCheckValid
功	      能: 检查通道句柄的合法性
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsChan    - 通道句柄
输出参数说明：
返   回   值: FALSE - 无效
              TRUE  - 有效
====================================================================*/
BOOL kdvCheckValid(IN HCHAN hsChan);

/*====================================================================
函	 数   名: kdvCallSetHandle
功	      能: 设置hsCall的应用层句柄
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsCall - 协议栈呼叫句柄
              haCall - 应用句柄
			  bSync  - 是否同步
返   回   值: 成功 - act_ok
              失败 - act_err
====================================================================*/
s32 kdvCallSetHandle(IN HCALL hsCall, IN HAPPCALL haCall, IN bool bSync = true);

/*====================================================================
函	 数   名: kdvCallGetHandle
功	      能: 获得应用句柄
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsCall - 协议栈呼叫句柄              
			  bSync  - 是否同步
输出参数说明: haCall - 应用句柄
返   回   值: 成功 - act_ok
              失败 - act_err
====================================================================*/
s32 kdvCallGetHandle(IN HCALL hsCall, OUT HAPPCALL* haCall, IN bool bSync = true);

/*====================================================================
函	 数   名: kdvRasUpdateRegAliasList
功	      能: 更新本端别名
算 法 实  现: 
引用全局变量: g_hApp
输入参数说明: alias  - 别名数组
			  size	 - 别名个数              
返   回   值: 成功   - act_ok
              失败   - act_err
====================================================================*/
s32 kdvRasUpdateRegAliasList(IN cmAlias alias[], IN s32 size);

/*====================================================================
函	 数   名: kdvSetGKRASAddress
功	      能: 设置gk地址
算 法 实  现: 
引用全局变量: 
输入参数说明: hApp - HAPP
              tr   - GK地址              
返   回   值: 成功 - act_ok
              失败 - act_err
====================================================================*/
s32 kdvSetGKRASAddress(IN HAPP hApp, IN cmTransportAddress tr, bool bSync = true );

/*====================================================================
函	 数   名: kdvGetCallParam
功	      能: 获得呼叫参数
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsCall	- 呼叫句柄
			  cpt		- 呼叫参数类型
			  bSync		- 是否同步
输出参数说明：pBuf		- 参数值
			  nBufLen	- 参数长度
返   回   值: 成功 - act_ok
              失败 - act_err
====================================================================*/
s32 kdvGetCallParam(IN HCALL hsCall, IN CallParamType cpt, 
                    OUT void* pBuf, OUT u16 nBufLen, IN bool bSync = true);

/*====================================================================
函	 数   名: kdvIsMasterCall
功	      能: 检查是否主叫
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsCall    - 呼叫句柄
输出参数说明：
返   回   值: FALSE - 被叫
              TRUE  - 主叫
====================================================================*/
BOOL kdvIsMasterCall(IN HCALL hsCall);

/*====================================================================
函	 数   名: kdvIsMasterTerminal
功	      能: 检查主从决定的结果是否master
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsCall    - 呼叫句柄
输出参数说明：
返   回   值: FALSE - slave
              TRUE  - master
====================================================================*/
BOOL kdvIsMasterTerminal(IN HCALL hsCall);

/*====================================================================
函	 数   名: kdvGetLocalCallIPAddress
功	      能: 获取本端呼叫的真实IP
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsCall    - 呼叫句柄
输出参数说明：
返   回   值: 网络序的IP地址
====================================================================*/
#if ( RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_ANY)
ipAddress kdvGetLocalCallIPAddress(IN HCALL hsCall);
#elif ( RV_H323_TRANSPORT_ADDRESS == RV_H323_TRANSPORT_ADDRESS_OLD)
u32 kdvGetLocalCallIPAddress(HCALL hsCall);
#endif


/*====================================================================
函	 数   名: GetEType
功	      能: 普通载荷类型转换成带加密信息的载荷类型
算 法 实  现: 
引用全局变量: 无
输入参数说明: wType	    - 待转换的载荷类型
              byEBits   - 按位的加密位，如 emEncryptTypeDES|emEncryptTypeAES   
返   回   值: 转换后的载荷类型
====================================================================*/
inline u16 GetEType( IN u16 wType, IN u8 byEBits) 
{
	u16 wTmp = byEBits;
	wTmp = ((wTmp<<8)&0xff00);
    wTmp |= wType;
	return wTmp;
};

inline u16 GetH239Type( IN u16 wType ) 
{
    return GetEType(wType, emEncryptTypeH239);
};

inline BOOL IsH239Type( IN u16 wType )
{  
    return ((wType>>8) & emEncryptTypeH239);
}

/*====================================================================
函	 数   名: IsAudioType
功	      能: 判断是否音频类型
算 法 实  现: 
引用全局变量: 无
输入参数说明: wPayLoadType	 - 载荷类型
返   回   值: 成功 - 大于0
              失败 - 小于0
====================================================================*/
inline BOOL IsAudioType(IN u16 wPayLoadType)
{
	return PayloardIsAudioType(wPayLoadType); 
}

/*====================================================================
函	 数   名: IsVideoType
功	      能: 判断是否视频类型
算 法 实  现: 
引用全局变量: 无
输入参数说明: wPayLoadType	 - 载荷类型
返   回   值: 成功 - 大于0
              失败 - 小于0
====================================================================*/
inline BOOL IsVideoType(IN u16 wPayLoadType)
{
	return PayloardIsVideoType(wPayLoadType);
}

/*====================================================================
函	 数   名: IsDataType
功	      能: 判断是否数据类型
算 法 实  现: 
引用全局变量: 无
输入参数说明: wPayLoadType	 - 载荷类型
返   回   值: 成功 - 大于0
              失败 - 小于0
====================================================================*/
inline BOOL IsDataType(IN u16 wPayLoadType)
{
	return PayloardIsDataType(wPayLoadType);
}


/////////////////////////////H323 Annex Q/////////////////////////////////

/*====================================================================
函	 数   名: kdvFeccBind
功	      能: 多点会议中绑定远遥对象，点对点呼叫时无需绑定
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsChan	 - h224通道句柄
              localTer   - 本地终端列表(TTERLABEL)
			  remoteTer  - 远端终端列表(TTERLABEL)
返   回   值: 成功 - 大于0
              失败 - 小于0
====================================================================*/
 s32 kdvFeccBind(IN HCHAN hsChan, IN TTERLABEL localTer, 
                 IN TTERLABEL remoteTer, IN bool bSync = true);

/*====================================================================
函	 数   名: kdvSendFeccMsg
功	      能: 通过h224通道发送远遥命令
算 法 实  现: 
引用全局变量: 无
输入参数说明: hsChan	 - h224通道句柄
			  feccStruct - 远遥命令(TFeccStruct)
返   回   值: 成功 - 大于0
              失败 - 小于0
====================================================================*/
s32 kdvSendFeccMsg(IN HCHAN hsChan, IN TFeccStruct feccStruct, IN bool bSync = true);


/* 输出有关协议栈的函数，并增加信号量保护 */
s32 kdvCmCallGetParam(IN HCALL hsCall, IN cmCallParam param, IN s32 index,
                      INOUT s32* value, OUT s8* svalue, IN bool bSync = true);

s32 kdvCmRASStartTransaction(IN  HAPP hApp, IN  HAPPRAS haRas, OUT LPHRAS lphsRas,
                             IN  cmRASTransaction transaction, IN cmRASTransport* destAddress,
                             IN  HCALL hsCall, IN  bool bSync = true);

s32 kdvCmRASSetParam(IN HRAS hsRas, IN cmRASTrStage trStage, IN cmRASParam param,
                     IN s32 index, IN s32 value, IN char* svalue, IN bool bSync = true);

s32 kdvCmRASGetParam(IN HRAS hsRas, IN  cmRASTrStage trStage, IN  cmRASParam param,
                     IN s32 index, IN OUT s32* value, /* value or length */
                     IN s8* svalue, IN bool bSync = true);

s32 kdvCmRASRequest(IN  HRAS hsRas, IN  bool bSync = true);

s32 kdvCmRASClose(IN  HRAS hsRas, IN bool bSync = true);

/*====================================================================
函	 数   名: kdvRASSetCallAddress
功	      能: 设置注册呼叫地址
算 法 实  现: 
引用全局变量: 
输入参数说明: ta   - 呼叫信令地址
返   回   值: 成功 - act_ok
              失败 - act_err
====================================================================*/
s32 kdvRASSetCallAddress(IN HAPP hApp, IN cmTransportAddress tr);

/*====================================================================
函	 数   名: kdvSetlocalRASAddress
功	      能: 设置注册RAS地址
算 法 实  现: 
引用全局变量: 
输入参数说明: ta   - 本地RAS地址
返   回   值: 成功 - act_ok
              失败 - act_err
====================================================================*/
s32 kdvSetlocalRASAddress(IN HAPP hApp, IN cmTransportAddress tr);


const s8* GetCmAliasType(AliasType dwAliasType);
const s8* GetCmAliasType(cmAliasType dwAliasType);
const s8* GetCmPayloadString(u16 wSubMsgType);
const s8* GetCmConfMsgString(u16 wSubMsgType);
const s8* GetCmChanMsgString(u16 wSubMsgType);
const s8* GetCmCallMsgString(u16 wPayload);
const s8* GetCmRasMsgString(u16 wSubMsgType);

/*====================================================================
函	 数   名: kdvUpdateGatekeeperID
功	      能: 更新GatekeeperID
算 法 实  现: 
引用全局变量: 
输入参数说明: tGKID   - gatekeeperid
返   回   值: 成功 - act_ok
失败 - act_err
====================================================================*/
s32 kdvUpdateGatekeeperID(IN HAPP hApp, IN cmAlias tGKID);

//用于加密
BOOL GetDesDHKey(HCALL hsCall, u8 **pBuf, s32 *pnBits);
BOOL GetAesDHKey(HCALL hsCall, u8 **pBuf, s32 *pnBits);


//发送会议级联控制消息
int kdvSendMMcuCtrlMsg(HCHAN hsChan,u16 msgType,const void* pBuf,u16 nBufLen,bool bSync = true);

/**说明***********************************************************************
*
1.关于双向逻辑通道(单独协议通道)(I incoming, O out )
> 逻辑通道冲突时， master 终端拒绝incoming channel
> I 方，方主动tcp connect时，ack消息不需带单独协议通道监听地址
> O 方，当ack带有单独协议通道监听地址，且I没有主动tcp connect时,发起tcp connect
2.级联通道
>h_chan_open,h_chan_openAck 只需填写payload, m_tRtp.ip.
*
******************************************************************************/

/************************** DNS Trans *****************************************/
typedef enum
{
    DNS_SUCCESS = 0,
    DNS_TIMEOUT,
    DNS_NOTFOUND,
    DNS_FAILED,
} dnsResult;

typedef s32 (*DnsCallBackT)(s32 nUserID, dnsResult eResult, s32 nIpNum, u32* adwIP);

API int kdvDnsRequest(s8* pchName, u32 dwDnsServer, u32 dwUserID, 
                      DnsCallBackT ptDnsCallbackFunc, IN  BOOL32 bSync);

/******************************************************************************/

//设置IRR信息，用于不依赖HCALL的场合，测试用。
s32 kdvSetRasIRR(HRAS hsRas, TIRRINFO* ptIRRInfo, bool bSync);

//消息体的主机序和网络序的转换
s32 kdvH323Adapterhton(u16 msgType, void* pBuf, u16 nBufLen, BOOL bhton);

//设置H323TOS值
API s32 kdvSetH323TOS(IN s32 nTOS, IN bool bSync = true);
//获取H323TOS值
API s32 kdvGetH323TOS();

//设置远遥TOS值, 函数体在kdvdatanet库中定义
API s32 kdvSetDataTOS(s32 nTOS);
API s32 kdvGetDataTOS();

API s8* GetPeerVersionID(HCALL hsCall);
API s8* GetPeerProductID(HCALL hsCall);
API BOOL IsPeerSupportNewG7221c(HCALL hsCall);  //对端是否支持正确字节序的G7221C音频码流

void KdvSetIsH460GW(BOOL32 bIsGw);

int KdvSendTpktPinhole(HCALL hsCall, cmTpktChanHandleType type, BOOL bSync = TRUE);
void KdvSetH460Config(BOOL bSupportH460, BOOL bMediaTraversalServer, BOOL bSupportTransMultiMedia);
void KdvSetCallApplicationHandle(HCALL hsCall, void* haCall);

// 设置本地是否为UTF8编码
API void kdvSetLocalIsUTF8(BOOL32 bIsUtf8);


//debug print
API void kdv323help();
API void set323trace(u8 byLevel);
API void setstacklog(s32 nLevel, BOOL bFileLogOn);
API void setfecctype(u8 byType);
API void seth323print(s32 nType, u8 byDir);
API void print323call();
API void print323config();
API void pne();

API void radvaltree();//[20120619]
API void radallnode( BOOL bJustRootNode );//[20120918]
// API void printrasstatus();
// API void printcallstatus();
// API void printtransstatus();
// API void printpoolstatus();


#endif //_H323ADAPTER_H
