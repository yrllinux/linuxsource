/*****************************************************************************
   模块名      : 
   文件名      : agtcomm.h
   相关文件    : 
   文件实现功能: MCU MT 代理宏定义
   作者        : 
   版本        : V4.0  Copyright( C) 2006-2008 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人        修改内容
   2006/11/16  4.0         张宝卿        代码整理
******************************************************************************/
#if !defined( _AGENT_COMMON_H_ )
#define _AGENT_COMMON_H_ 

#include "kdvsys.h"

//本配置版本号的定义随版本发布情况更新
#define MCUAGT_SYS_CFGVER               "20080701-4.0R5"

/*MT的硬件类型*/
enum  
{
	TYPE_PC		= 1,
	TYPE_EMBED	= 2
};

/*终端类型*/
enum
{
	TYPE_RECV_AND_SND,
	TYPE_RECV_ONLY,
	TYPE_SND_ONLY
};

/*终端地位*/
enum
{
	POSITION_MASTER		=1,
	POSITION_SLARE		=2,
	POSITION_AUDIT		=3
};

/*解码器解码方式*/
enum
{
	MODE_SOFTWARE_DECODE		=1,
	MODE_HARDWARE_DECODE		=2,
	MODE_SOFTWARE_AND_HARDWARE	=3
};

/*编码器采集方式*/
enum
{
	MODE_SOFTWARE		=1,
	MODE_HARDWARE		=2
};


/*软件采集数据格式*/
enum
{
	FORMAT_RGB		=1,
	FORMAT_Yuv422	=2
};

/*视频播放是否使用Overlay*/
enum
{
	OVERLAY_ENABLE	=1,
	OVERLAY_DISABLE =2,
};

/*回声抵消参数*/
enum
{
	AEC_ENABLE		=1,
	AEC_DISABLE		=2
};

/*任务状态*/
enum
{
	TASK_NORMAL		=0,
	TASK_BUSY		=1
};

/*视频源状态*/
enum
{
	VIDEOSOURCE_NORMAL		=0,
	VIDEOSOURCE_ABNORMAL    =1,
};

/*同步源状态*/
enum
{
	SYNCSOURCE_NORMAL		=0,
	SYNCSOURCE_ABNORMAL     =1,
};

/*同步源工作状态*/
enum
{
	SYNC_FREERUN	= 1,
	SYNC_TRACE_2M	= 2,
};

enum
{
	APP_TYPE_CONF  =1,
	APP_TYPE_APPLY =2
};

#define SYNCMODE_LIBERAL						(u8)1   /* 自由振荡模式 */
#define SYNCMODE_EXTERNAL						(u8)2   /* 外部时钟源模式 */

#define MAX_BOARD_COUNT							60
#define RESET_BRD								2       /* nms reboot board */

#define MAX_BRD_NUM								16
#define MCU_SLOT_NUM                        	17      /* mcu槽数目 */

#define CONF_STATE_START		            	1       /*会议开始*/
#define CONF_STATE_STOP			            	2       /*会议结束*/

#define CONF_ADD_TERMINAL		            	1       /*会议增加终端*/
#define CONF_DEL_TERMINAL		            	2       /*会议删除终端*/

// 单板状态(代理内部)
#define BOARD_STATUS_INLINE                 	(u8)0   /* 0：正常 */
#define BOARD_STATUS_DISCONNECT             	(u8)1   /* 1：断链 */
#define BOARD_STATUS_CFGCONFLICT            	(u8)2   /* 2：配置冲突 */
#define BOARD_STATUS_UNKNOW                 	(u8)3   /* 3：单板未知(初始状态) */

#define KDV_READ_COMMUNITYSTRING            	(LPCSTR)"public"
#define KDV_WRITE_COMMUNITYSTRING           	(LPCSTR)"kdv123"

/*MPC 板状态*/
#define  MCU_STATE_RUNNING                  	(u8)1   /* 运行状态 */
#define  MCU_STATE_REBOOT                   	(u8)2   /* 重起状态 */
#define  MCU_STATE_STANDY                   	(u8)3   /* 待机状态 */
#define  MPC_COLD_START                     	(u8)4   /* Mpc 板冷启动 */
#define  MPC_POWER_OFF                      	(u8)5   /* Mpc 板关机 */
#define  MCU_STATE_UNKNOW                   	(u8)10  /* 未知状态  */

#define NMS_TEST_START                      	(u8)1   /* test */
#define NMS_TEST_STOP                       	(u8)2

/* 网管或会控一次最大上传文件数 */
#define MAXNUM_FILE_UPDATE						(u8)16	

//由于BoardWrapper.h与brddrvlib.h存在命名冲突，所以在这里单独定义，但需要与BoardWrapper.h中保持一致
#define DSL8000_BRD_HDSC                    	0x14     /* HDSC板 */

/*------------------------------------------------------------- 
/*define max table items
*-------------------------------------------------------------*/
#define MAX_mcunetTrapRcvEntry_NUM          	10
#define MAX_mcunetPeriEqpEntry_NUM          	16
#define MAX_mcunetSubMcuEntry_NUM           	32
#define MAX_mcunetMp4SubMcuEntry_NUM        	32
#define MAX_mcunetSubMtEntry_NUM            	128
#define MAX_mcunetMp4SubMtEntry_NUM         	128
#define MAX_mcueqpDtiEntry_NUM              	16
#define MAX_mcueqpE1Entry_NUM               	MAX_mcueqpDtiEntry_NUM * 8
#define MAX_mcueqpTsEntry_NUM               	MAX_mcueqpE1Entry_NUM * 32
#define MAX_mcueqpNscuEntry_NUM             	2
#define MAX_mcueqpSwitchEntry_NUM           	16

/*------------------------------------------------------------- 
/* 定义机箱风扇转速门限值，大于2000转为正常，小于2000产生告警
*-------------------------------------------------------------*/
#define MIN_MCU8000ABOXFAN_SPEED                2000

/*------------------------------------------------------------- 
/* 定义MCU8000A机箱风扇个数
*-------------------------------------------------------------*/
#define MCU8000ABOXFAN_NUM                      8    

/*------------------------------------------------------------- 
*VCS组常量 
*-------------------------------------------------------------*/
#define SECTION_mcuVCS                       (const s8*)"mcuVCS"
#define KEY_mcuVCSSoftName                   (const s8*)"mcuVCSSoftName"

/*------------------------------------------------------------- 
*系统组常量 
*-------------------------------------------------------------*/
#define SECTION_mcuSystem                       (const s8*)"mcuSystem"
#define KEY_mcusysConfigVersion                 (const s8*)"mcusysConfigVersion"
#define KEY_mcuIsConfiged                       (const s8*)"mcuIsConfiged"

/*------------------------------------------------------------- 
*本地信息常量 
*-------------------------------------------------------------*/
#define SECTION_mcuLocalInfo	                (const s8*)"mcuLocalInfo"
#define KEY_mucId				                (const s8*)"mcuId"
#define KEY_mcuAlias			                (const s8*)"mcuAlias"
#define KEY_mcuE164Number		                (const s8*)"mcuE164Number"
#define KEY_mcuMmcuNeedBAS		                (const s8*)"mcuMmcuNeedBAS"
#define KEY_mcuCheckMtLinkTimes                 (const s8*)"mcuCheckLinkTimes"
#define Key_mcuCheckLinkTime                    (const s8*)"mcuCheckLinkTime"
#define Key_mcuRefreshListTime                  (const s8*)"mcuRefreshListTime"
#define Key_mcuAudioRefreshTime                 (const s8*)"mcuAudioRefreshTime"
#define Key_mcuVideoRefreshTime                 (const s8*)"mcuVideoRefreshTime"
#define Key_mcuIsSaveBand                       (const s8*)"mcuIsSaveBand"
#define Key_mcuIsNPlusMode                      (const s8*)"mcuIsNPlusMode"
#define Key_mcuIsNPlusBackupMode                (const s8*)"mcuIsNPlusBackupMode"
#define Key_mcuNPlusMcuIp                       (const s8*)"mcuNPlusMcuIp"
#define Key_mcuNPlusRtdTime                     (const s8*)"mcuNPlusRtdTime"
#define Key_mcuNPlusRtdNum                      (const s8*)"mcuNPlusRtdNum"
#define Key_mcuIsNPlusRollBack                  (const s8*)"mcuIsNPlusRollBack"
#define Key_mcuMsSynTime						(const s8*)"mcuMsSynTime"
#define key_mcuIsHoldDefaultConf				(const s8*)"mcuIsHoldDefaultConf"
#define key_mcuIsShowMMcuMtList					(const s8*)"mcuIsShowMMcuMtList"
#define key_mcuMaxMcsOnGoingConfNum				(const s8*)"mcuMaxMcsOnGoingConfNum"
#define key_mcuAdminLevel						(const s8*)"mcuAdminLevel"
#define key_mcuLocalConfNameShowType			(const s8*)"mcuLocalConfNameShowType"
#define key_mcuIsMMcuSpeaker					(const s8*)"mcuIsMMcuSpeaker"

/*------------------------------------------------------------- 
*级连时缺省的终端列表常量 
*-------------------------------------------------------------*/
#define SECTION_mcuTerminalIpTable		        (const s8*)"mcuTerminalIpTable"
#define FIELD_mcuTerminalIpEntIpAddr	        (const s8*)"mcuTerminalIpEntIpAddr"


/*------------------------------------------------------------- 
*网络组常量 
*-------------------------------------------------------------*/
#define SECTION_mcuNetwork			            (const s8*)"mcuNetwork"
#define KEY_mcunetGKIpAddr			            (const s8*)"mcunetGKIpAddr"
#define KEY_mcunetGKCharge                      (const s8*)"mcunetGKCharge"
#define KEY_mcunetRRQMtadpIp                    (const s8*)"mcunetRRQMtadpIp"
#define KEY_mcunetMulticastIpAddr	            (const s8*)"mcunetMulticastIpAddr"
#define KEY_mcunetMulticastPort		            (const s8*)"mcunetMulticastPort"
#define KEY_mcunetRecvStartPort		            (const s8*)"mcunetRecvStartPort"
#define KEY_mcunet225245StartPort               (const s8*)"mcunet225245StartPort"
#define KEY_mcunet225245MtNum                   (const s8*)"mcunet225245MtNum"
#define KEY_mcunetMTUSize						(const s8*)"mcunetMTUSize"
#define KEY_mcunetMTUSetupMode					(const s8*)"mcunetMTUSetupMode"
#define KEY_mcunetUseMPCTransData               (const s8*)"mcunetUseMPCTransData"
#define KEY_mcunetUseMPCStack	                (const s8*)"mcunetUseMPCStack"
	
/*------------------------------------------------------------- 
*GK/Proxy配置表常量		 
*-------------------------------------------------------------*/
#define SECTION_mcuGkProxyCfg					(const s8*)"mcuGkProxyCfg"
#define KEY_mcuGkUsed							(const s8*)"GkUsed"
#define KEY_mcuProxyUsed						(const s8*)"ProxyUsed"
#define KEY_mcuGkIpAddr							(const s8*)"GkIpAddr"
#define KEY_mcuProxyIpAddr						(const s8*)"ProxyIpAddr"
#define	KEY_mcuProxyPort						(const s8*)"ProxyPort"


/*------------------------------------------------------------- 
*prs time span表常量		 
*-------------------------------------------------------------*/
#define SECTION_mcuPrsTimeSpan					(const s8*)"mcuPrsTimeSpan"
#define KEY_mcuPrsSpan1							(const s8*)"span1"
#define KEY_mcuPrsSpan2							(const s8*)"span2"
#define KEY_mcuPrsSpan3							(const s8*)"span3"
#define KEY_mcuPrsSpan4							(const s8*)"span4"

/*------------------------------------------------------------- 
*TRAP服务器表常量 
*-------------------------------------------------------------*/
#define SECTION_mcunetTrapRcvTable              (const s8*)"mcunetTrapRcvTable"
#define FIELD_mcunetTrapRcvEntIpAddr            (const s8*)"mcunetTrapRcvEntIpAddr"
#define FIELD_mcunetTrapRcvEntRCommunity        (const s8*)"mcunetTrapRcvRCommunity"
#define FIELD_mcunetTrapRevEntWCommunity        (const s8*)"mcunetTrapRcvWCommunity"
#define FIELD_mcunetTrapGetSetPort              (const s8*)"mcunetTrapRcvGetSetPort"   
#define FIELD_mcunetTrapSendTrapPort            (const s8*)"mcunetTrapRcvSendTrapPort"

/*------------------------------------------------------------- 
*单板表常量 
*-------------------------------------------------------------*/
#define SECTION_mcueqpBoardConfig		        (const s8*)"mcueqpBoardConfig"
#define FIELD_mcueqpBoardId				        (const s8*)"mcueqpBoardId"
#define FIELD_mcueqpBoardLayer			        (const s8*)"mcueqpBoardLayer"
#define FIELD_mcueqpBoardSlot			        (const s8*)"mcueqpBoardSlot"
#define FIELD_mcueqpBoardType			        (const s8*)"mcueqpBoardType"
#define FIELD_mcueqpBoardIpAddr			        (const s8*)"mcueqpBoardIpAddr"
#define FIELD_mcueqpBoardPortChoice             (const s8*)"mcueqpBoardPortChoice"
#define FIELD_mcueqpBoardCastChoice             (const s8*)"mcueqpBoardCastChoice"

// 网同步参数 
#define SECTION_mcueqpNetSync				    (const s8*)"mcueqpNetSync"
#define KEY_mcueqpNetSyncMode				    (const s8*)"mcueqpNetSyncMode"
#define KEY_mcueqpNetSyncDTSlot				    (const s8*)"mcueqpNetSyncDTSlot"
#define KEY_mcueqpNetSyncE1Index			    (const s8*)"mcueqpNetSyncE1Index"
#define KEY_mcueqpNetSyncSource				    (const s8*)"mcueqpNetSyncSource"
#define KEY_mcueqpNetSyncClock				    (const s8*)"mcueqpNetSyncClock"
	

// 混音器表常量 
#define SECTION_mcueqpMixerTable			    (const s8*)"mcueqpMixerTable"
#define FIELD_mcueqpMixerEntId				    (const s8*)"mcueqpMixerEntId"
#define FIELD_mcueqpMixerEntPort			    (const s8*)"mcueqpMixerEntPort"
#define FIELD_mcueqpMixerEntSwitchBrdId		    (const s8*)"mcueqpMixerEntSwitchBrdId"
#define FIELD_mcueqpMixerEntAlias			    (const s8*)"mcueqpMixerEntAlias"
#define FIELD_mcueqpMixerEntRunningBrdId	    (const s8*)"mcueqpMixerEntRunningBrdId"
#define FIELD_mcueqpMixerEntMixStartPort	    (const s8*)"mcueqpMixerEntMixStartPort"
#define FIELD_mcueqpMixerEntMaxMixGroupNum      (const s8*)"mcueqpMixerEntMaxMixGroupNum"
#define FIELD_mcueqpMixerEntMaxChannelInGrp     (const s8*)"mcueqpMixerEntMaxChannelInGrp"
#define FIELD_mcueqpMixerEntMapId			    (const s8*)"mcueqpMixerEntMapId"
#define FIELD_mcueqpMixerEntMixSendRedundancy	(const s8*)"mcueqpMixerEntMixSendRedundancy"

// 数字会议服务器参数常量 
#define SECTION_mcueqpDataConfServer		    (const s8*)"mcueqpDataConfServer"
#define KEY_mcueqpDcsIp						    (const s8*)"mcueqpDcsIp"
#define KEY_mcueqpDcsId                         (const s8*)"mcueqpDcsId"


// 放、录象机表常量 
#define SECTION_mcueqpRecorderTable				(const s8*)"mcueqpRecorderTable"
#define FIELD_mcueqpRecorderEntId				(const s8*)"mcueqpRecorderEntId"
#define FIELD_mcueqpRecorderEntPort				(const s8*)"mcueqpRecorderEntPort"
#define FIELD_mcueqpRecorderEntSwitchBrdId		(const s8*)"mcueqpRecorderEntSwitchBrdId"
#define FIELD_mcueqpRecorderEntAlias			(const s8*)"mcueqpRecorderEntAlias"
#define FIELD_mcueqpRecorderEntIpAddr			(const s8*)"mcueqpRecorderEntIpAddr"
#define FIELD_mcueqpRecorderEntRecvStartPort	(const s8*)"mcueqpRecorderEntRecvStartPort"
#define FIELD_mcueqpRecorderEntRecChannelNum	(const s8*)"mcueqpRecorderEntRecChannelNum"
#define FIELD_mcueqpRecorderEntPlayChannelNum	(const s8*)"mcueqpRecorderEntPlayChannelNum"


// Vrs新录播表常量 
#define SECTION_mcueqpVrsRecorderTable			(const s8*)"mcueqpVrsRecorderTable"
#define FIELD_mcueqpVrsRecorderEntId			(const s8*)"mcueqpVrsRecorderEntId"
#define FIELD_mcueqpVrsRecorderEntAlias			(const s8*)"mcueqpVrsRecorderEntAlias"
#define FIELD_mcueqpVrsRecorderEntCallAliasType	(const s8*)"mcueqpVrsRecorderEntCallAliasType"
#define FIELD_mcueqpVrsRecorderEntIpAddr		(const s8*)"mcueqpVrsRecorderEntIpAddr"
#define FIELD_mcueqpVrsRecorderEntE164Number	(const s8*)"mcueqpVrsRecorderEntE164Number"


// 电视墙表常量 
#define SECTION_mcueqpTVWallTable			    (const s8*)"mcueqpTVWallTable"
#define FIELD_mcueqpTVWallEntId				    (const s8*)"mcueqpTVWallEntId"
#define FIELD_mcueqpTVWallEntAlias			    (const s8*)"mcueqpTVWallEntAlias"
#define FIELD_mcueqpTVWallEntRunningBrdId       (const s8*)"mcueqpTVWallEntRunningBrdId"
#define FIELD_mcueqpTVWallEntRecvStartPort      (const s8*)"mcueqpTVWallEntRecvStartPort"
#define FIELD_mcueqpTVWallEntDivStyle		    (const s8*)"mcueqpTVWallEntDivStyle"
#define FIELD_mcueqpTVWallEntDivNum			    (const s8*)"mcueqpTVWallEntDivNum"
#define FIELD_mcueqpTVWallEntMapId			    (const s8*)"mcueqpTVWallEntMapId"

// 码流适配器表常量 
#define SECTION_mcueqpBasTable			        (const s8*)"mcueqpBasTable"
#define SECTION_mcueqpBasHDTable			    (const s8*)"mcueqpBasHDTable"
#define FIELD_mcueqpBasEntId			        (const s8*)"mcueqpBasEntId"
#define FIELD_mcueqpBasEntPort			        (const s8*)"mcueqpBasEntPort"
#define FIELD_mcueqpBasEntSwitchBrdId	        (const s8*)"mcueqpBasEntSwitchBrdId"
#define FIELD_mcueqpBasEntAlias			        (const s8*)"mcueqpBasEntAlias"
#define FIELD_mcueqpBasEntRunningBrdId	        (const s8*)"mcueqpBasEntRunningBrdId"
#define FIELD_mcueqpBasEntRecvStartPort	        (const s8*)"mcueqpBasEntRecvStartPort"
#define FIELD_mcueqpBasEntMaxAdpChannel         (const s8*)"mcueqpBasEntMaxAdpChannel"
#define FIELD_mcueqpBasEntMapId			        (const s8*)"mcueqpBasEntMapId"
#define FIELD_mcueqpBasEntIpAddr                (const s8*)"mcueqpBasEntIpAddr"

// 图像合成器表常量 
#define SECTION_mcueqpVMPTable			        (const s8*)"mcueqpVMPTable"
#define FIELD_mcueqpVMPEntId			        (const s8*)"mcueqpVMPEntId"
#define FIELD_mcueqpVMPEntPort			        (const s8*)"mcueqpVMPEntPort"
#define FIELD_mcueqpVMPEntSwitchBrdId	        (const s8*)"mcueqpVMPEntSwitchBrdId"
#define FIELD_mcueqpVMPEntAlias			        (const s8*)"mcueqpVMPEntAlias"
#define FIELD_mcueqpVMPEntRunningBrdId	        (const s8*)"mcueqpVMPEntRunningBrdId"
#define FIELD_mcueqpVMPEntRecvStartPort	        (const s8*)"mcueqpVMPEntRecvStartPort"
#define FIELD_mcueqpVMPEntMapId			        (const s8*)"mcueqpVMPEntMapId"
#define FIELD_mcueqpVMPEntEncNumber		        (const s8*)"mcueqpVMPEntEncNumber"

// 图像合成器附属设置
#define SECTION_mcuVmpAttachTable               (const s8*)"mcuVmpAttachTable"
#define FIELD_mcueqpVmpAttachIndex              (const s8*)"mcueqpVmpAttachIndex"
#define FIELD_mcueqpBlackGroudColor             (const s8*)"mcueqpBlackGroudColor"
#define FIELD_mcueqpFrameColer                  (const s8*)"mcueqpFrameColor"
#define FIELD_mcueqpSpeakerFrameColor           (const s8*)"mcueqpSpeakerFrameColor"
#define FIELD_mcueqpChairColor                  (const s8*)"mcueqpChairColor"


// 复合电视墙表常量 
#define SECTION_mcueqpMpwTable			        (const s8*)"mcueqpMpwTable"
#define FIELD_mcueqpMpwEntId			        (const s8*)"mcueqpMpwEntId"
#define FIELD_mcueqpMpwEntPort			        (const s8*)"mcueqpMpwEntPort"
#define FIELD_mcueqpMpwEntSwitchBrdId	        (const s8*)"mcueqpMpwEntSwitchBrdId"
#define FIELD_mcueqpMpwEntAlias			        (const s8*)"mcueqpMpwEntAlias"
#define FIELD_mcueqpMpwEntRunningBrdId	        (const s8*)"mcueqpMpwEntRunningBrdId"
#define FIELD_mcueqpMpwEntRecvStartPort	        (const s8*)"mcueqpMpwEntRecvStartPort"
#define FIELD_mcueqpMpwEntMapId			        (const s8*)"mcueqpMpwEntMapId"

// 包重传器表常量 
#define SECTION_mcueqpPrsTable			        (const s8*)"mcueqpPrsTable"
#define FIELD_mcueqpPrsId				        (const s8*)"mcueqpPrsId"
#define FIELD_mcueqpPrsMCUStartPort		        (const s8*)"mcueqpPrsMCUStartPort"
#define FIELD_mcueqpPrsSwitchBrdId		        (const s8*)"mcueqpPrsSwitchBrdId"
#define FIELD_mcueqpPrsAlias			        (const s8*)"mcueqpPrsAlias"
#define FIELD_mcueqpPrsRunningBrdId		        (const s8*)"mcueqpPrsRunningBrdId"
#define FIELD_mcueqpPrsStartPort		        (const s8*)"mcueqpPrsStartPort"
#define FIELD_mcueqpPrsFirstTimeSpan	        (const s8*)"mcueqpPrsFirstTimeSpan"
#define FIELD_mcueqpPrsSecondTimeSpan	        (const s8*)"mcueqpPrsSecondTimeSpan"
#define FIELD_mcueqpPrsThirdTimeSpan            (const s8*)"mcueqpPrsThirdTimeSpan"
#define FIELD_mcueqpPrsRejectTimeSpan	        (const s8*)"mcueqpPrsRejectTimeSpan"

//4.6版本 新加 jlb
//hdu
#define SECTION_mcueqpHduTable			        (const s8*)"mcueqpHduTable"
#define FIELD_mcueqpHduEntId				    (const s8*)"mcueqpHduEntId"
#define FIELD_mcueqpHduEntAlias					(const s8*)"mcueqpHduEntAlias"
#define FIELD_mcueqpHduEntRunningBrdId			(const s8*)"mcueqpHduEntRunningBrdId"
#define FIELD_mcueqpHduEntRecvStartPort			(const s8*)"mcueqpHduEntRecvStartPort"
#define FIELD_mcueqpHduEntDivStyle				(const s8*)"mcueqpHduEntDivStyle"
#define FIELD_mcueqpHduEntDivNum			    (const s8*)"mcueqpHduEntDivNum"
#define FIELD_mcueqpHduEntMapId					(const s8*)"mcueqpHduEntMapId"
#define FIELD_mcueqpHduEntOutModeType1			(const s8*)"mcueqpHduEntOutModeType1"
#define FIELD_mcueqpHduEntOutPortType1			(const s8*)"mcueqpHduEntOutPortType1"
#define FIELD_mcueqpHduEntOutModeType2			(const s8*)"mcueqpHduEntOutModeType2"
#define FIELD_mcueqpHduEntOutPortType2			(const s8*)"mcueqpHduEntOutPortType2"
#define FIELD_mcueqpHduEntZoomRate1			    (const s8*)"mcueqpHduEntZoomRate1"
#define FIELD_mcueqpHduEntZoomRate2			    (const s8*)"mcueqpHduEntZoomRate2"



//svmp
#define SECTION_mcueqpSvmpTable			        (const s8*)"mcueqpSvmpTable"
#define FIELD_mcueqpSvmpEntId			        (const s8*)"mcueqpSvmpEntId"
#define FIELD_mcueqpSvmpEntPort			        (const s8*)"mcueqpSvmpEntPort"
#define FIELD_mcueqpSvmpEntSwitchBrdId	        (const s8*)"mcueqpSvmpEntSwitchBrdId"
#define FIELD_mcueqpSvmpEntAlias			    (const s8*)"mcueqpSvmpPEntAlias"
#define FIELD_mcueqpSvmpEntRunningBrdId	        (const s8*)"mcueqpSvmpEntRunningBrdId"
#define FIELD_mcueqpSvmpEntRecvStartPort	    (const s8*)"mcueqpSvmpEntRecvStartPort"
#define FIELD_mcueqpSvmpEntMapId			    (const s8*)"mcueqpSvmpEntMapId"

//dvmp
#define SECTION_mcueqpDvmpTable			        (const s8*)"mcueqpDvmpTable"
#define FIELD_mcueqpDvmpEntId			        (const s8*)"mcueqpDvmpEntId"
#define FIELD_mcueqpDvmpEntPort			        (const s8*)"mcueqpDvmpEntPort"
#define FIELD_mcueqpDvmpEntSwitchBrdId	        (const s8*)"mcueqpDvmpEntSwitchBrdId"
#define FIELD_mcueqpDvmpEntAlias			    (const s8*)"mcueqpDvmpPEntAlias"
#define FIELD_mcueqpDvmpEntRunningBrdId	        (const s8*)"mcueqpDvmpEntRunningBrdId"
#define FIELD_mcueqpDvmpEntRecvStartPort	    (const s8*)"mcueqpDvmpEntRecvStartPort"
#define FIELD_mcueqpDvmpEntMapId			    (const s8*)"mcueqpDvmpEntMapId"

//MpuBas
#define SECTION_mcueqpMpuBasTable			    (const s8*)"mcueqpMpuBasTable"
#define SECTION_mcueqpMpuBasHDTable			    (const s8*)"mcueqpMpuBasHDTable"
#define FIELD_mcueqpMpuBasEntId			        (const s8*)"mcueqpMpuBasEntId"
#define FIELD_mcueqpMpuBasEntPort			    (const s8*)"mcueqpMpuBasEntPort"
#define FIELD_mcueqpMpuBasEntSwitchBrdId	    (const s8*)"mcueqpMpuBasEntSwitchBrdId"
#define FIELD_mcueqpMpuBasEntAlias			    (const s8*)"mcueqpMpuBasEntAlias"
#define FIELD_mcueqpMpuBasEntRunningBrdId	    (const s8*)"mcueqpMpuBasEntRunningBrdId"
#define FIELD_mcueqpMpuBasEntRecvStartPort	    (const s8*)"mcueqpMpuBasEntRecvStartPort"
#define FIELD_mcueqpMpuBasEntMaxAdpChannel      (const s8*)"mcueqpMpuBasEntMaxAdpChannel"
#define FIELD_mcueqpMpuBasEntMapId			    (const s8*)"mcueqpMpuBasEntMapId"
#define FIELD_mcueqpMpuBasEntIpAddr             (const s8*)"mcueqpMpuBasEntIpAddr"

//Ebap
#define SECTION_mcueqpEbapTable			        (const s8*)"mcueqpEbapTable"
#define SECTION_mcueqpEbapHDTable			    (const s8*)"mcueqpEbapHDTable"
#define FIELD_mcueqpEbapEntId			        (const s8*)"mcueqpEbapEntId"
#define FIELD_mcueqpEbapEntPort			        (const s8*)"mcueqpEbapEntPort"
#define FIELD_mcueqpEbapEntSwitchBrdId	        (const s8*)"mcueqpEbapEntSwitchBrdId"
#define FIELD_mcueqpEbapEntAlias			    (const s8*)"mcueqpEbapEntAlias"
#define FIELD_mcueqpEbapEntRunningBrdId	        (const s8*)"mcueqpEbapEntRunningBrdId"
#define FIELD_mcueqpEbapEntRecvStartPort	    (const s8*)"mcueqpEbapEntRecvStartPort"
#define FIELD_mcueqpEbapEntMaxAdpChannel        (const s8*)"mcueqpEbapEntMaxAdpChannel"
#define FIELD_mcueqpEbapEntMapId			    (const s8*)"mcueqpEbapntMapId"
#define FIELD_mcueqpEbapEntIpAddr               (const s8*)"mcueqpEbapEntIpAddr"

//Evpu
#define SECTION_mcueqpEvpuTable			        (const s8*)"mcueqpEvpuTable"
#define FIELD_mcueqpEvpuEntId			        (const s8*)"mcueqpEvpuEntId"
#define FIELD_mcueqpEvpuEntPort			        (const s8*)"mcueqpEvpuEntPort"
#define FIELD_mcueqpEvpuEntSwitchBrdId	        (const s8*)"mcueqpEvpuEntSwitchBrdId"
#define FIELD_mcueqpEvpuEntAlias			    (const s8*)"mcueqpEvpuPEntAlias"
#define FIELD_mcueqpEvpuEntRunningBrdId	        (const s8*)"mcueqpEvpuEntRunningBrdId"
#define FIELD_mcueqpEvpuEntRecvStartPort	    (const s8*)"mcueqpEvpuEntRecvStartPort"
#define FIELD_mcueqpEvpuEntMapId			    (const s8*)"mcueqpEvpuEntMapId"

//Hdu Scheme
#define SECTION_mcuHduSchemeTable			    (const s8*)"mcuHduSchemeTable"
#define SECTION_mcuHduSchemeChnTable1           (const s8*)"HduSchemeChnTable1"
#define SECTION_mcuHduSchemeChnTable2           (const s8*)"HduSchemeChnTable2"
#define SECTION_mcuHduSchemeChnTable3           (const s8*)"HduSchemeChnTable3"
#define SECTION_mcuHduSchemeChnTable4           (const s8*)"HduSchemeChnTable4"
#define SECTION_mcuHduSchemeChnTable5           (const s8*)"HduSchemeChnTable5"
#define SECTION_mcuHduSchemeChnTable6           (const s8*)"HduSchemeChnTable6"
#define SECTION_mcuHduSchemeChnTable7           (const s8*)"HduSchemeChnTable7"
#define SECTION_mcuHduSchemeChnTable8           (const s8*)"HduSchemeChnTable8"
#define SECTION_mcuHduSchemeChnTable9           (const s8*)"HduSchemeChnTable9"
#define SECTION_mcuHduSchemeChnTable10          (const s8*)"HduSchemeChnTable10"
#define SECTION_mcuHduSchemeChnTable11          (const s8*)"HduSchemeChnTable11"
#define SECTION_mcuHduSchemeChnTable12          (const s8*)"HduSchemeChnTable12"
#define SECTION_mcuHduSchemeChnTable13          (const s8*)"HduSchemeChnTable13"
#define SECTION_mcuHduSchemeChnTable14          (const s8*)"HduSchemeChnTable14"
#define SECTION_mcuHduSchemeChnTable15          (const s8*)"HduSchemeChnTable15"
#define SECTION_mcuHduSchemeChnTable16          (const s8*)"HduSchemeChnTable16"


// Qos
#define SECTION_mcuQosInfo                      (const s8*)"mcuQosConfigure"
#define KEY_mcuQosType                          (const s8*)"mcuQosType"
#define KEY_mcuAudioLevel                       (const s8*)"mcuAudioLevel"
#define KEY_mcuVideoLevel                       (const s8*)"mcuVideoLevel"
#define KEY_mcuDataLevel                        (const s8*)"mcuDataLevel"
#define KEY_mcuSignalLevel                      (const s8*)"mcuSignalLevel"
#define KEY_mcuIpServiceType                    (const s8*)"mcuIpServiceType"

// DSC板在MCU8000B下的特殊配置处理
#define SECTION_DscModule                       (const s8*)"DSCModule"
#define KEY_DscInnerIp                          (const s8*)"DscInnerIp"
#define KEY_DscMp                               (const s8*)"Mp"
#define KEY_DscMtAdp                            (const s8*)"MtAdp"
#define KEY_DscGk                               (const s8*)"Gk"
#define KEY_DscProxy                            (const s8*)"Proxy"
#define KEY_DscDcs                              (const s8*)"Dcs"
#define KEY_NetType								(const s8*)"NetType"
#define KEY_LanMtProxyIpPos						(const s8*)"LanMtProxyIpPos"
#define KEY_CallAddrNum							(const s8*)"CallAddrNum"
#define ENTRY_CallAddr							(const s8*)"CallAddr"
#define KEY_MCSAccessAddrNum					(const s8*)"MCSAccessAddrNum"
#define ENTRY_MCSAccessAddr						(const s8*)"MCSAccessAddr"

// 外设扩展配置
#define SECTION_EqpExCfgInfo                    (const s8*)"EqpExCfgInfo"
#define KEY_HduIdleChlShowMode                  (const s8*)"HduIdleChlShowMode"
#define KEY_VmpIdleChlShowMode                  (const s8*)"VmpIdleChlShowMode"
#define KEY_HduShowPictureIndx                  (const s8*)"HduShowPictureIndx"
#define KEY_VmpShowPictureIndx                  (const s8*)"VmpShowPictureIndx"
#define KEY_VmpIsDisplayMmbAlias				(const s8*)"VmpIsDisplayMmbAlias"
// VCS
#define VCS_DEFAULTSOFTNAME                     (const s8*)"科达可视指挥调度系统" 







#define ALARM_RESTART							1000L       /*设备重启动告警*/ //新老告警规则都使用

//定义一条新告警时 不要有子标识符 ，如果有子标识符请增加一条告警宏（宏的值这个和告警知识库一一对应）
/////////////////////////////////////////////////老告警规则//////////////////////////////////////////////////////////////////
////////////////////////////////////mpc板子、非8000A所有告警mcu ///////////////////////////////////////////
/*软件告警部分*/
#define ALARM_MCU_MEMORYERROR					1001L       /*内存错误*/
#define ALARM_MCU_FILESYSTEMERROR				1002L       /*文件系统出错*/
#define ALARM_MCU_TASK_BUSY						1101L       /*任务忙 网管5.0未用*/

/*以下是硬件告警部分*/
/*网同步部分*/
#define ALARM_MCU_SYNCSRC_ABNORMAL				2101L       /*同步源异常*/
#define ALARM_MCU_SYNCSRC_SPARE_ABNORMAL		2102L       /*备用同步源异常*/
#define ALARM_MCU_SYNCSRC_BOTH_ABNORMAL			2103L       /*主备用同步源同时异常 网管5.0未用*/
/*电源相关*/
#define ALARM_MCU_POWER_ABNORMAL				2201L       /*电源异常*/
#define ALARM_MCU_POWER_FAN_ABNORMAL			2202L       /*电源风扇异常*/

/*模块相关*/
#define ALARM_MCU_MODULE_OFFLINE				2301L       /*模块离线 网管5.0未用*/
#define ALARM_MCU_MODULE_CONFLICT				2302L       /*模块冲突 网管5.0未用*/
#define ALARM_MCU_MODULE_ERROR					2303L       /*模块错误 网管5.0未用*/

/*单板相关*/
#define ALARM_MCU_BOARD_DISCONNECT				2401L       /*单板异常*/
#define ALARM_MCU_BOARD_CFGCONFLICT				2402L       /*单板配置冲突*/
#define ALARM_MCU_BOARD_FAN_STOP				2410L       /*单板风扇异常 网管5.0未用*/


/*机箱风扇相关 miaoqingsong add */
#define ALARM_MCU_BOX_FAN_ABNORMAL              2411L       /*机箱风扇异常*/
/*MPC板内存使用率相关 miaoqingsong add */
#define ALARM_MCU_MPC_MEMORY_ABNORMAL           2412L       /*MPC板Memory使用率超过85%*/
/*MPC板Cpu使用率相关 miaoqingsong add */
#define ALARM_MCU_MPC_CPU_ABNORMAL              2413L       /*MPC板Cpu使用率超过85%*/
/*电源板温度相关 miaoqingsong add */
#define ALARM_MCU_POWER_TEMP_ABNORMAL           2414L       /*电源板温度异常*/
/*MPC板温度相关 miaoqingsong add */
#define ALARM_MCU_MPC_TEMP_ABNORMAL             2415L       /*MPC板温度过高*/        
//												2416L		/*2416 已用*/
#define ALARM_MCU_CPU_FAN_ABNORMAL				2417L		/*CPU风扇异常*/
#define ALARM_MCU_CPU_TEMP_ABNORMAL				2418L		/*CPU温度过高*/

/*E1相关*/
#define ALARM_MCU_E1_RUA1						2501L       /*E1全1*/
#define ALARM_MCU_E1_RLOS						2502L       /*E1失同步*/
#define ALARM_MCU_E1_RRA						2503L       /*E1远端告警*/
#define ALARM_MCU_E1_LOC						2504L       /*E1失载波*/
#define ALARM_MCU_E1_RMF						2505L       /*CRC复帧错误*/
#define ALARM_MCU_E1_RCMF						2506L       /*随路复帧错误*/
/////////////////////////////////////////////////老告警规则end//////////////////////////////////////////////////////////////////


///////////////////////////////////////// 新单板告警规则 begin  /////////////////////////////////
//// 先报以下的告警码，然后通过其他字段获取板子类型和层槽号/////////////////////////////////////////////
////为了兼容除"老告警规则"之外全部用这种方式告警/////////////////////////////////////////
//告警后的详细字符串标识规格  层（字节0）槽（字节1）类型（字节2）其他（字节3-n）

//机箱 （层需要上报）
#define ALARM_MCU_FRM_POWER_ABNORMAL				3000L//电源板异常                    2201
#define ALARM_MCU_FRM_FAN_ABNORMAL					3001L//机箱风扇异常                  2411
#define ALARM_MCU_FRM_LEFTPOWER_TEMP_ABNORMAL		3002L//左电源板温度告警              2414
#define ALARM_MCU_FRM_RIGHTPOWER_TEMP_ABNORMAL		3003L//右电源板温度告警              2416
#define ALARM_MCU_FRM_POWERFAN_ABNORMAL				3004L//电源风扇异常					 2202

//板子 （层、槽、类型需要上报）
#define ALARM_MCU_BRD_ACCESSMEMERROR				3010L//设备故障，内存访问错误        1001
#define ALARM_MCU_BRD_FILESYSTEMERROR				3011L//文件系统错误                  1002
#define ALARM_MCU_BRD_SYNCSRC_ABNORMAL				3012L//同步源异常出错                2101
#define ALARM_MCU_BRD_SYNCSRC_SPARE_ABNORMAL		3013L//备板同步源异常出错            2102
#define ALARM_MCU_BRD_BOARD_DISCONNECT				3014L//单板未注册	                 2401
#define ALARM_MCU_BRD_BOARD_CFGCONFLICT				3015L//单板配置冲突                  2402
#define ALARM_MCU_BRD_CPU_ABNORMAL					3016L//CPU使用率高于指定阀值告警     2413
#define ALARM_MCU_BRD_MEMORY_ABNORMAL				3017L//内存使用超过指定阀值告警      2412
#define ALARM_MCU_BRD_TEMP_ABNORMAL					3018L//板子温度过高                  2415

//E1	（层、槽、类型需要上报）
#define ALARM_MCU_BRD_E1_RUA1						3030L//E1全1                         2501
#define ALARM_MCU_BRD_E1_RLOS						3031L//E1失同步                      2502
#define ALARM_MCU_BRD_E1_RRA						3032L//E1远端告警                    2503
#define ALARM_MCU_BRD_E1_LOC						3033L//E1失载波                      2504
#define ALARM_MCU_BRD_E1_RMF						3034L//E1 CRC复帧错误                2505
#define ALARM_MCU_BRD_E1_RCMF						3035L//E1 随路复帧错误               2506

//#define ALARM_MCU_BRD_END							3059L//板子的最大告警值 3059-3010+1=50

#define ALARM_MCU_BASE							20000L  //mcu告警码起始
#define ALARM_ONEBRD_RANGE						50L		//每个板子有50告警号
//板子新告警码范围
#define ALARM_MCU_RANGE_START					ALARM_MCU_BASE + ALARM_ONEBRD_RANGE*0   + 3010 //23010
#define ALARM_MCU_RANGE_END						ALARM_MCU_BASE + ALARM_ONEBRD_RANGE*255 + 3059 //32750 + 3059 

/*sdh 以下4001-4018 网管5.0未用 miaoqingsong*/
#define ALARM_MCU_SDH_LOS						4001L       /*光信号丢失*/
#define ALARM_MCU_SDH_LOF						4002L       /*帧丢失*/
#define ALARM_MCU_SDH_OOF						4003L       /*帧失步*/
#define ALARM_MCU_SDH_LOM						4004L       /*复帧丢失*/
#define ALARM_MCU_SDH_LOP						4005L       /*管理单元指针丢失*/
#define ALARM_MCU_SDH_MS_RDI					4006L       /*复用段远端缺陷指示*/
#define ALARM_MCU_SDH_MS_AIS					4007L       /*复用段报警指示*/
#define ALARM_MCU_SDH_HP_RDI					4008L       /*高阶通道远端缺陷指示*/
#define ALARM_MCU_SDH_HP_AIS					4009L       /*高阶通道报警指示*/
#define ALARM_MCU_SDH_RS_TIM					4010L       /*再生段通道踪迹字节不匹配*/
#define ALARM_MCU_SDH_HP_TIM					4011L       /*高阶通道踪迹字节不匹配*/
#define ALARM_MCU_SDH_HP_UNEQ					4012L       /*高阶通道信号标记字节未装载*/
#define ALARM_MCU_SDH_HP_PSLM					4013L       /*高阶通道信号标记字节不匹配*/
#define ALARM_MCU_SDH_TU_LOP					4014L       /*支路单元指针丢失*/
#define ALARM_MCU_SDH_LP_RDI					4015L       /*低阶通道远端缺陷指示*/
#define ALARM_MCU_SDH_LP_AIS					4016L       /*低阶通道报警指示*/
#define ALARM_MCU_SDH_LP_TIM					4017L       /*低阶通道踪迹字节不匹配*/
#define ALARM_MCU_SDH_LP_PSLM					4018L       /*低阶通道信号标记字节不匹配*/



//#define ALARM_MCU_BASE							20000L  //mcu告警码起始
//#define ALARM_ONEBRD_RANGE						50L

//#define ALARM_MCU_RANGE_END						ALARM_MCU_BASE + ALARM_ONEBRD_RANGE*255 //32750
///////////////////////////////////////// 新单板告警规则 end  /////////////////////////////////







////////////////////////////////////////	终端告警码		//////////////////////////////////
/* MT告警码常量定义 编码按照告警对象类型分类*/
#define ALARM_MT_BASE							10000L

/*软件告警部分*/
/*MT终端系统内存相关 miaoqingsong add*/
#define ALARM_MT_MEMORYERROR                    (ALARM_MT_BASE+1001L)   /*内存错误*/
#define ALARM_MT_FILESYSTEMERROR                (ALARM_MT_BASE+1002L)   /*文件系统出错*/

/*MT终端系统CPU相关 miaoqingsong add*/
#define ALARM_MT_CPUERROR						(ALARM_MT_BASE+1003L)   /*cpu出错*/
#define ALARM_MT_TASK_BUSY				        (ALARM_MT_BASE+1101L)   /*任务忙*/

/*以下是硬件告警部分*/

/*E1相关*/
#define ALARM_MT_E1_RUA1                        (ALARM_MT_BASE+2001L)   /*E1全1*/
#define ALARM_MT_E1_RLOS                        (ALARM_MT_BASE+2002L)   /*E1失同步*/
#define ALARM_MT_E1_RRA                         (ALARM_MT_BASE+2003L)   /*E1远端告警*/
#define ALARM_MT_E1_LOC                         (ALARM_MT_BASE+2004L)   /*E1失载波*/

/*MAP相关*/
#define ALARM_MT_MAP_DIE				        (ALARM_MT_BASE+2101L)   /*MAP死机   未用*/

/*CCI相关*/
#define ALARM_MT_CCI_QUEUE_FULL			        (ALARM_MT_BASE+2201L)   /*CCI队列满  未用*/

/*码流相关*/
/*网管5.0改变：终端接收丢包网络丢包均采用2301L告警码 miaoqingsong*/
#define ALARM_MT_NO_BIT_STREAM			        (ALARM_MT_BASE+2301L)   /*接收无码流*/
#define ALARM_MT_SEND_LOST_FRAME		        (ALARM_MT_BASE+2302L)   /*发送丢包   未用*/
//(请定义时从 ALARM_MT_BASE+2305L开始  因为ALARM_MT_BASE+2301+2代表接收丢包大于%5  ALARM_MT_BASE+2301+3代表接收丢包大于%8 )

/*视频源相关*/
#define ALARM_MT_VIDEOSOURCE_ERROR		        (ALARM_MT_BASE+2403L)   /*无视频源*/

/*以太网相关*/
#define ALARM_MT_ETHERNET_DISCONNECT	        (ALARM_MT_BASE+2501L)   /*以太网断开 网管5.0未用*/

/*风扇相关*/
#define ALARM_MT_FAN_SPEED_LOW			        (ALARM_MT_BASE+2601L)   /*风扇转速低*/

/*V35相关 以下2701L-2705L网管5.0未用 miaoqingsong*/
#define ALARM_MT_V35_CD_DOWN			        (ALARM_MT_BASE+2701L)   /*v35载波检测失败    未用*/
#define ALARM_MT_V35_CTS_DOWN			        (ALARM_MT_BASE+2702L)   /*v35清除发送失败    未用*/
#define ALARM_MT_V35_RTS_DOWN			        (ALARM_MT_BASE+2703L)   /*v35请求发送失败    未用*/
#define ALARM_MT_V35_DTR_DOWN			        (ALARM_MT_BASE+2704L)   /*v35数据终端未就绪  未用*/
#define ALARM_MT_V35_DSR_DOWN			        (ALARM_MT_BASE+2705L)	/*v35数据未准备好    未用*/

/*安全相关*/
#define ALARM_MT_TEMP_HIGH					    (ALARM_MT_BASE+2801L)   /*高清终端温度过高*/
#define ALARM_MT_VOLT_ERROR		                (ALARM_MT_BASE+2802L)	/*高清终端电压告警*/




////////////////////////////////////////	Cps告警码		//////////////////////////////////
#define ALARM_CPS_BASE							13000L
#define ALARM_CPS_CPURATE						(ALARM_CPS_BASE + 1)   //CPU过高
#define ALARM_CPS_AUTHUSBSTATE					(ALARM_CPS_BASE + 2)   //USB认证


////////////////////////////////////////	Mps告警码		//////////////////////////////////
#define ALARM_MPS_BASE							13100L
#define ALARM_MPS_CPURATE						(ALARM_MPS_BASE + 1)   //CPU过高


////////////////////////////////////////	Aps告警码		//////////////////////////////////
#define ALARM_APS_BASE							13200L
#define ALARM_APS_CPURATE						(ALARM_APS_BASE + 1)   //CPU过高

////////////////////////////////////////	Caps告警码		//////////////////////////////////
#define ALARM_CAPS_BASE							13300L
#define ALARM_CAPS_CPURATE						(ALARM_CAPS_BASE + 1)   //CPU过高


/* MCU告警对象类型 */
#define ALARMOBJ_MCU						    (u8)1
#define ALARMOBJ_MCU_BOARD					    (u8)11
#define ALARMOBJ_MCU_LINK					    (u8)12
#define ALARMOBJ_MCU_TASK					    (u8)13
#define ALARMOBJ_MCU_SDH					    (u8)14
#define ALARMOBJ_MCU_POWER					    (u8)15
#define ALARMOBJ_MCU_MODULE					    (u8)16
#define ALARMOBJ_MCU_SOFTWARE				    (u8)17
#define ALARMOBJ_MCU_BRD_FAN				    (u8)18
#define ALARMOBJ_MCU_BOX_FAN                    (u8)19              // miaoqingsong add
#define ALARMOBJ_MCU_MPC_CPU                    (u8)20              // miaoqingsong add 
#define ALARMOBJ_MCU_MPC_MEMORY                 (u8)21              // miaoqingsong add
#define ALARMOBJ_MCU_POWER_TEMP                 (u8)22              // miaoqingsong add
#define ALARMOBJ_MCU_MPC_TEMP                   (u8)23              // miaoqingsong add
#define ALARMOBJ_MCU_CPU_FAN                    (u8)24
#define ALARMOBJ_MCU_CPU_TEMP                   (u8)25

/* MT告警对象类型 */
#define ALARMOBJ_MT_BASE				        (u8)128
#define ALARMOBJ_MT                             (u8)(ALARMOBJ_MT_BASE + 1)
#define ALARMOBJ_MT_E1                          (u8)(ALARMOBJ_MT_BASE + 11)
#define ALARMOBJ_MT_MAP					        (u8)(ALARMOBJ_MT_BASE + 12)
#define ALARMOBJ_MT_CCI					        (u8)(ALARMOBJ_MT_BASE + 13)
#define ALARMOBJ_MT_STREAM				        (u8)(ALARMOBJ_MT_BASE + 14)
#define ALARMOBJ_MT_ETHERNET			        (u8)(ALARMOBJ_MT_BASE + 15)
#define ALARMOBJ_MT_TASK				        (u8)(ALARMOBJ_MT_BASE + 16)
#define ALARMOBJ_MT_FAN					        (u8)(ALARMOBJ_MT_BASE + 17)
#define ALARMOBJ_MT_V35					        (u8)(ALARMOBJ_MT_BASE + 18)
#define ALARMOBJ_MT_TEMP			            (u8)(ALARMOBJ_MT_BASE + 20)   
#define ALARMOBJ_MT_VOLT			            (u8)(ALARMOBJ_MT_BASE + 21)

/*MPC板内存和CPU异常告警使用*/
#define  MEMORY_MCU_OBJECT              (u8)1         // MCU设备MPC板内存告警对象标识，处理内存异常告警时使用 mqs 2010/12/22
#define  CPU_MCU_OBJECT                 (u8)2         // MCU设备MPC板CPU告警对象标识，处理CPU异常告警使用 mqs 2010/12/22
#define  TEMP_MCU_OBJECT                (u8)3         // MCU设备MPC板温度异常告警对象标识，处理MPC2板温度过高异常告警使用 mqs 2011/01/13

/*会商告警对象类型*/  //对应NODE_XXXPFMALARMOBJTYPE
#define ALARMOBJTYPE_CONSULTATION_BASE				(u8)160
#define ALARMOBJTYPE_CPS							(u8)(ALARMOBJ_CONSULTATION_BASE + 0)
#define ALARMOBJTYPE_MPS							(u8)(ALARMOBJ_CONSULTATION_BASE + 1)
#define ALARMOBJTYPE_APS							(u8)(ALARMOBJ_CONSULTATION_BASE + 2)
#define ALARMOBJTYPE_CAPS							(u8)(ALARMOBJ_CONSULTATION_BASE + 3)

//外设使用率统计buffer长度
const u8 AGENT_MAX_EQPNODEBUFF_LEN  = 128;

enum Pfm_eTypeKind
{
	Pfm_eType_MtNum = 0,
	Pfm_eType_ConfNum,
	Pfm_eType_Eqp,
	Pfm_eType_Auth
};

/*性能改变通知*/
struct TPfmNotify
{
	u8 m_byPfmType; //Pfm_eTypeKind
	u8 m_byEqpType; //如果有外设
	u16 m_wValue;
}

#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*会议开始或结束时通知AGENT的结构*/
struct TConfNotify
{
	u8 m_abyConfId[16];
	u8 m_abyConfName[32];   /*会议名*/
	u8 m_byConfMtNum;       /*与会终端数目*/
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*会议中增加删除终端时通知AGENT的结构*/
struct TMtNotify
{
	u8 m_abyConfId[16];
	u8 m_abyMtAlias[128];
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

    
/*定义业务向代理即时上报的E1或DTI状态改变结构*/
struct CDevStatus
{
	u8 dti;					/*0-32*/
	u8 e1;					/*0-7*/
	u8 status;				/*对于E1/DTI状态消息，分别表示E1/DTI的状态：DTI时： 0 可用, 1 异常
							    e1时:	bit0 E1失同步		bit1 E1失载波 
									    bit2 E1远端告警		bit3 E1全1*/
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


/*单板向MPC报告告警时使用的结构*/
struct TBoardAlarmMsgInfo
{
	u16	wEventId;
	u8   abyAlarmContent[5];
};


/*单板状态*/
struct TBrdStatus
{
	u8	byLayer;	        /*层号*/
	u8	bySlot;		        /*槽号*/
	u8	byType;		        /*板类型*/
	u8	byStatus;	        /*状态，0:正常  1:异常  2:配置冲突 */
    u8  byOsType;           /*操作系统类型*/
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TBrdFanStatus
{
	u8  byLayer;
	u8  bySlot;
	u8  byType;
	u8  byFanId;
	u8  byStatus;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*链路状态*/
struct TLinkStatus
{
	u8	byLayer;	        /*层号*/
	u8	bySlot;		        /*槽号*/
	u8	byType;		        /*板类型*/
	u8	byE1Num;            /*E1号*/
	u8	byStatus;	        /*状态  bit0 E1失同步     bit1 E1失载波       bit2 E1远端告警	
						            bit3  E1全1       bit4 CRC复帧错误    bit5 随路复帧错误*/
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

struct TPowerStatus
{
	u8 bySlot;              /*电源槽*/
	u8 byType;              /*类型(5v 48v)*/
	u8 byStatus;            /*状态*/
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

enum
{
	POWER_LEFT = 1,
	POWER_RIGHT = 2
};

enum 
{
	POWER_5V =1,
	POWER_48V = 2
};

enum 
{
	FAN_LEFT  = 1,
	FAN_RIGHT = 2
};

struct TPowerFanStatus
{
	u8 bySlot;              /*电源槽*/
	u8 byFanPos;            /**/
	u8 byStatus;            /*状态*/
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [2010/12/14 miaoqingsong add]  机箱风扇位置结构体
struct TBoxFanStatus
{
	u8 bySlot;              /*电源槽 从右至左分别为：0 1 2 3 */
	u8 byFanPos;            /*1 2*/ 
	u8 byStatus;            /*状态*/
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

// [2010/12/15 miaoqingsong add] MPC板Cpu状态信息结构体
struct TMPCCpuStatus
{
	u8 byCpuAllocRate;    /*MPC板Cpu占用率 0 - 100 */
	u8 byCpuStatus;       /*状态*/
};

// [2010/12/15 miaoqingsong add] MPC板内存状态信息结构体
struct TMPCMemoryStatus
{
	u8 byMemoryAllocRate;   /*MPC板内存使用率 0 - 100 */
	u8 byMemoryStatus;      /*状态*/
};

// [2011/01/13 miaoqingsong add] MPC2板温度状态信息结构体
struct TMPC2TempStatus
{
	u32 dwMpc2Temp;        /*MPC2板实时温度值 0 - 100 */
	u8  byMpc2TempStatus;  /*状态*/
};

// [2010/12/15 miaoqingsong add] 电源板温度信息结构体
struct TPowerBrdTempStatus
{
	u8 bySlot;                   /*电源槽 左电源:  POWER_LEFT(1); 右电源: POWER_RIGHT(2)*/
	u8 byPowerBrdTempStatus;     /*电源板温度状态*/
};

struct TModuleStatus
{
	u8 byLayer;             /*层号*/
	u8 bySlot;              /*槽号*/
	u8 byType;              /*板类型*/
	u8 byModuleId;          /*模块号*/
	u8 byStatus;            /*状态  bit0 模块离线  bit1  模块冲突 bit2 模块错误*/  
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*MCU的内存和文件系统状态报告使用该结构*/
struct TSoftwareStatus
{
	u8 byLayer;             /*层号*/
	u8 bySlot;              /*槽号*/
	u8 byType;              /*板类型*/
	u8 byStatus;            /*状态*/
};

/*MCU的任务状态报告使用该结构*/
struct TMcuTaskStatus
{
	u8 byLayer;             /*层号*/
	u8 bySlot;              /*槽号*/
	u8 byType;              /*板类型*/
	u16  wAppId;	        /*APP号*/
	u8 byStatus;            /*状态*/
};

/*SDH的状态*/
struct TSDHStatus
{
	u32	dwStatus;/*状态  bit0  光信号丢失                    bit1  帧丢失           
					         bit2  帧失步	                     bit3  复帧丢失    
							 bit4  管理单元指针丢失              bit5  复用段远端缺陷指示
							 bit6  复用段报警指示                bit7  高阶通道远端缺陷指示 
							 bit8  高阶通道报警指示              bit9  再生段通道踪迹字节不匹配 
							 bit10 高阶通道踪迹字节不匹配        bit11 高阶通道信号标记字节未装载 
							 bit12 高阶通道信号标记字节不匹配    bit13 支路单元指针丢失  
							 bit14 低阶通道远端缺陷指示          bit15 低阶通道报警指示  
							 bit16 低阶通道踪迹字节不匹配        bit17 低阶通道信号标记字节不匹配   */
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*定义业务向代理上报的同步源状态改变结构*/
struct CSyncSourceAlarm
{
	u8 masterstatus;		/*0: ok, 1: fail*/
	u8 slavestatus;			/*0: ok, 1: fail*/
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*定义业务向代理即时上报的MT状态改变结构*/
struct CMtConnectStatus
{
	u8 mt;					/*1-127*/
	u8 status;				/*对于MT状态消息，分别表示MT的状态：0 可用, 1 异常*/
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


/*定义业务向代理上报的当前同步源改变上报结构*/
struct CCurrentMasterSync
{
	u8 dti;
	u8 e1;
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*定义业务向代理上报的设备连接信息结构*/
struct CEqpConnStatus
{
	u8 eqptype;             /*设备类型*/
	u8 eqpid;	            /*设备编号*/
	u8 status;	            /*设备状态 1 在线 2不在线 3不知道*/	 	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*定义业务向代理上报的任务状态结构*/
struct CTaskStatus
{
	u16 appid;	            /*APP号*/
	u8  status;	            /*APP状态 0 正常 1任务忙*/	 	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*定义业务向代理上报的MAP状态结构*/
struct CMapStatus
{
	u8 mapid;	            /*MAP号*/
	u8 status;	            /*MAP状态 0 正常 1死机*/
	u16 wDiedCount;         /*该MAP死机次数*/
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*定义业务向代理上报的CCI状态结构*/
struct CCCIStatus
{
	u8 mapid;	            /*MAP号*/
	u8 updown;              /*0 上行 1 下行*/
	u8 queueid;             /*队列号*/
	u8 status;	            /*MAP队列状态  0 正常  1 满	*/ 	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*定义业务向代理上报的以太网状态结构*/
struct CEthernetCardStatus
{
	u8 cardid;	            /*以太网卡号*/
	u8 status;	            /*以太网卡状态 0 正常 1网线断开*/ 	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*定义业务向代理上报的媒体流状态结构*/
struct CMediaStreamStatus
{
	u8 mediatype;	        /*媒体类型*/
	u8 status;		        /*流状态 bit0 接收无码流 bit1 发送丟包*/ 	 	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

/*定义业务向代理上报的风扇状态结构*/
struct CFanStatus
{
	u8 fanid;	            /*风扇ID号*/
	u8 status;		        /*风扇状态 0 正常 1 异常*/ 	 	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

#endif /*_AGENT_COMMON_H_*/
