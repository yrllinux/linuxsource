              
/*****************************************************************************
   模块名      : 新方案业务
   文件名      : evmcumt.h
   相关文件    : 
   文件实现功能: 新方案MCU和终端之间接口消息定义
                 *_REQ: 需应答请求
				 *_ACK, *_NACK: 请求之答复
				 *_CMD: 不需应答命令
				 *_NOTIF: 状态通知

-----------------------------------------------------------------------------
   修改记录:

注：为保证程序正确运行，所有成功和失败应答比原消息号分别加1，2

******************************************************************************/
#ifndef _EV_MCUMT_H_
#define _EV_MCUMT_H_

#include "osp.h"
#include "eventid.h"

const u16 EV_MCUMT_STD_INTERVAL  = 400;
const u16 EV_MCUMCU_STD_INTERVAL = 100;
const u16 EV_MCUMT_KDV_INTERVAL  = 400;
const u16 EV_MTADP_INTERVAL      = 50;
const u16 EV_MCUMCU_KDV_INTERVAL = 20;  //  pengjie[9/28/2009] 自定义级联多回传消息间隔
const u16 EV_MCUMT_SAT_INTERVAL  = 30;

const u16 EV_MCUMT_STD_BGN = EV_MCUMT_BGN + 1;
const u16 EV_MCUMT_STD_END = EV_MCUMT_BGN + EV_MCUMT_STD_INTERVAL;

const u16 EV_MCUMCU_STD_BGN = EV_MCUMT_STD_END  + 1;
const u16 EV_MCUMCU_STD_END = EV_MCUMCU_STD_BGN + EV_MCUMCU_STD_INTERVAL;

const u16 EV_MCUMT_KDV_BGN = EV_MCUMCU_STD_END + 1;
const u16 EV_MCUMT_KDV_END = EV_MCUMCU_STD_END + EV_MCUMT_KDV_INTERVAL;

const u16 EV_MTADP_BGN = EV_MCUMT_KDV_END + 1; // 自定义 MTADP内部维护, 区间长度150   
const u16 EV_MTADP_END = EV_MCUMT_KDV_END + EV_MTADP_INTERVAL; // 自定义 MTADP内部维护, 区间长度150 

//  pengjie[9/28/2009] 自定义级联多回传消息 长度20
const u16 EV_MCUMCU_KDV_BGN = EV_MTADP_END + 1;
const u16 EV_MCUMCU_KDV_END = EV_MTADP_END + EV_MCUMCU_KDV_INTERVAL;

const u16 EV_MCUMT_SAT_BGN = EV_MCUMCU_KDV_END + 1;
const u16 EV_MCUMT_SAT_END = EV_MCUMT_SAT_BGN + EV_MCUMT_SAT_INTERVAL;  // MCU和mtadp卫星协议会话交互，区间长度30

/************************************************************************/
/* 标准 创建、撤销会议                                                  */
/************************************************************************/

//终端发给MCU的创建会议请求，MT->MCU，
//TMtAlias(终端H323Id)+TMTAlias(终端e164Id)+TMtAlias(终端ip)+TMtAlias(本MCU的会议名)+
//+byType(type_MT, type_MCU)+byEncrypt(1-加密,0-不加密)+TConfInfo
OSPEVENT( MT_MCU_CREATECONF_REQ,		EV_MCUMT_STD_BGN + 1 );
//MCU发给MT的创建会议应答，MCU->MT 消息体:byEncrypt(1-加密,0-不加密)+TCapSupport(会议能力)+TMtAlias(本MCU的会议名)
OSPEVENT( MCU_MT_CREATECONF_ACK,		EV_MCUMT_STD_BGN + 2 );
//MCU拒绝MT的创建会议请求，无消息体
OSPEVENT( MCU_MT_CREATECONF_NACK,		EV_MCUMT_STD_BGN + 3 );

// guzh [6/19/2007] 
//下级MCU发给本级MCU的呼叫请求通知，MT->MCU，
//TMtAlias(终端H323Id)+TMTAlias(终端e164Id)+TMtAlias(终端ip)+TMtAlias(本MCU的会议名)+
//+byType(type_MT, type_MCU)+byEncrypt(1-加密,0-不加密)+TConfInfo
OSPEVENT( MT_MCU_LOWERCALLEDIN_NTF,		EV_MCUMT_STD_BGN + 5);

//主席终端发给MCU的中止会议请求，MT->MCU，消息体NULL
OSPEVENT( MT_MCU_DROPCONF_CMD,			EV_MCUMT_STD_BGN + 8 );
/************************************************************************/
/* 标准 入会、离会                                                      */
/************************************************************************/

//终端发给MCU的加入会议的请求，MT->MCU，消息体为结构TMtAlias(终端别名) + TMtAlias（会议别名）
OSPEVENT( MT_MCU_MTJOINCONF_REQ,		EV_MCUMT_STD_BGN + 10 );
//终端发给MCU的加入会议的请求成功应答，MT->MCU，无消息体
OSPEVENT( MCU_MT_MTJOINCONF_ACK,		EV_MCUMT_STD_BGN + 11 );
//终端发给MCU的加入会议的请求失败应答，MT->MCU，无消息体
OSPEVENT( MCU_MT_MTJOINCONF_NACK,	    EV_MCUMT_STD_BGN + 12 );



//MCU邀请终端加入指定会议请求，MCU->MT (MCU->SMCU)，
//消息体为结构TMt+TMtAlias(终端别名)+TMtAlias(会议名)+byEncrypt(1-加密,0-不加密)
//u16(网络序,呼叫带宽kbps)+TCapSupport(会议能力)+u16(低字节:调度级别(u8)+高字节:调度类型(1-命令型 0-请求型))
OSPEVENT( MCU_MT_INVITEMT_REQ,		    EV_MCUMT_STD_BGN + 18 );
//MT接受应答，MT->MCU，消息体为结构TMtAlias(H323Id) + TMtAlias(ip) + 1byte (内含远端实体类型,为: TYPE_MCU或TYPE_MT)+
//byEncrypt(1-加密,0-不加密)
OSPEVENT( MT_MCU_INVITEMT_ACK,		    EV_MCUMT_STD_BGN + 19 );
//MT拒绝应答，MT->MCU，消息体为结构TMtAlias + 1byte (VC原因)
//                                 (若原因为MTLEFT_REASON_BYSYEXT + u8(所在会议的调度级别) + 会议的323ID)   原因参见错误码
OSPEVENT( MT_MCU_INVITEMT_NACK,		    EV_MCUMT_STD_BGN + 20 );


//MCU强制终端退出会议命令，MCU->MT (MCU->SMCU)，无消息体
OSPEVENT( MCU_MT_DELMT_CMD,			    EV_MCUMT_STD_BGN + 21 );

//主席终端发给MCU的强制终端退出请求，MT->MCU，消息体为结构TMt
OSPEVENT( MT_MCU_DELMT_REQ,				EV_MCUMT_STD_BGN + 25 );
//MCU回的成功应答，MCU->MT，无消息体
OSPEVENT( MCU_MT_DELMT_ACK,				EV_MCUMT_STD_BGN + 26 );
//MCU回的失败应答，MCU->MT，无消息体
OSPEVENT( MCU_MT_DELMT_NACK,			EV_MCUMT_STD_BGN + 27 );

//终端加入会议通知，MCU->MT (MCU->SMCU)，消息体为结构TMt
OSPEVENT( MCU_MT_MTJOINED_NOTIF,		EV_MCUMT_STD_BGN + 30 );
//终端离开会议通知，MCU->MT (MCU->SMCU)，消息体为结构TMt
OSPEVENT( MCU_MT_MTLEFT_NOTIF,			EV_MCUMT_STD_BGN + 32 );
//会议增加受邀终端通知，MCU->MT，消息体为结构TMt
/************************************************************************/
/* 标准 认证操作                                                        */
/************************************************************************/
//请求终端输入密码，消息体为NULL
OSPEVENT(MCU_MT_ENTERPASSWORD_REQ,      EV_MCUMT_STD_BGN + 35 ); //enterH243Password
//终端回应密码, u8 数组,表示实际输入的密码(长度不定)
OSPEVENT(MT_MCU_ENTERPASSWORD_ACK,      EV_MCUMT_STD_BGN + 36 );
OSPEVENT(MT_MCU_ENTERPASSWORD_NACK,     EV_MCUMT_STD_BGN + 37 );

//MCU同步终端适配层该会议的会议密码，用于合并级联请求的认证校验，消息体为会议密码数组，长度MAXLEN_PWD
OSPEVENT(MCU_MT_CONFPWD_NTF,		    EV_MCUMT_STD_BGN + 39 );

/************************************************************************/
/* 标准 主席操作                                                        */
/************************************************************************/

//终端发给MCU的申请主席请求，MT->MCU，消息体为NULL
OSPEVENT( MT_MCU_APPLYCHAIRMAN_REQ,		EV_MCUMT_STD_BGN + 40 );
//终端发给MCU的申请主席请求成功应答，MCU->MT，无消息体
OSPEVENT( MCU_MT_APPLYCHAIRMAN_ACK,		EV_MCUMT_STD_BGN + 41 ); 
//终端发给MCU的申请主席请求失败应答，MCU->MT，无消息体
OSPEVENT( MCU_MT_APPLYCHAIRMAN_NACK,	EV_MCUMT_STD_BGN + 42 );

//MCU发给终端的取消主席请求，MCU->MT，无消息体
OSPEVENT( MCU_MT_CANCELCHAIRMAN_NOTIF,	EV_MCUMT_STD_BGN + 45 );

//主席终端发送给MCU的取消主席的请求，MT->MCU, 消息体NULL
OSPEVENT( MT_MCU_CANCELCHAIRMAN_REQ,	EV_MCUMT_STD_BGN + 50 );//cancelMakeMeChair
//MCU->MT, 消息体NULL
OSPEVENT( MCU_MT_CANCELCHAIRMAN_ACK,	EV_MCUMT_STD_BGN + 51 );//withdrawChairToken
OSPEVENT( MCU_MT_CANCELCHAIRMAN_NACK,	EV_MCUMT_STD_BGN + 52 );


//查询具有主席权限的终端ID，消息体NULL
OSPEVENT( MT_MCU_GETCHAIRMAN_REQ,       EV_MCUMT_STD_BGN + 55 );
//查询主席ID应答，消息体TMt + TMtAlias
OSPEVENT( MCU_MT_GETCHAIRMAN_ACK,       EV_MCUMT_STD_BGN + 56 );
OSPEVENT( MCU_MT_GETCHAIRMAN_NACK,      EV_MCUMT_STD_BGN + 57 );

/************************************************************************/
/* 标准 发言操作                                                        */
/************************************************************************/

//主席终端发给MCU的指定发言人请求，MT->MCU，消息体为空
OSPEVENT( MT_MCU_SPECSPEAKER_REQ,		EV_MCUMT_STD_BGN + 60 );//makeTerminalBroadcaster
//MCU回的成功应答，MCU->MT，无消息体
OSPEVENT( MCU_MT_SPECSPEAKER_ACK,		EV_MCUMT_STD_BGN + 61 );
//MCU回的失败应答，MCU->MT，无消息体
OSPEVENT( MCU_MT_SPECSPEAKER_NACK,		EV_MCUMT_STD_BGN + 62 );

//MCU发给终端的指定发言通知，MCU->MT，消息体为结构TMt，代表发言人
OSPEVENT( MCU_MT_SPECSPEAKER_NOTIF,	    EV_MCUMT_STD_BGN + 65 ); //seenByAll
//MCU发给终端的取消发言请求，MCU->MT，消息体为结构TMt
OSPEVENT( MCU_MT_CANCELSPEAKER_NOTIF,	EV_MCUMT_STD_BGN + 67 ); //cancelSeenByAll
//主席终端指定发言人，MT->MCU，TMt
OSPEVENT( MT_MCU_SPECSPEAKER_CMD,		EV_MCUMT_STD_BGN + 69 );//makeTerminalBroadcaster
//主席终端取消发言人，MT->MCU，NULL
OSPEVENT( MT_MCU_CANCELSPEAKER_CMD,		EV_MCUMT_STD_BGN + 71 );//cancelMakeTerminalBroadcaster NULL

//终端发给MCU的申请发言请求，MT->MCU，消息体NULL
//OSPEVENT( MT_MCU_APPLYSPEAKER_CMD,		EV_MCUMT_STD_BGN + 136 ); 
OSPEVENT( MT_MCU_APPLYSPEAKER_NOTIF,	EV_MCUMT_STD_BGN + 75 ); //requestForFloor ind
//通知主席终端申请发言请求，MCU->MT，消息体为结构TMt
OSPEVENT( MCU_MT_APPLYSPEAKER_NOTIF,	EV_MCUMT_STD_BGN + 77 ); // floorRequested

/************************************************************************/
/* 标准 选看广播                                                        */
/************************************************************************/

//主席终端发给MCU的选看终端命令，MT->MCU，消息体为结构TSwitchInfo
OSPEVENT( MT_MCU_STARTSELMT_CMD,		EV_MCUMT_STD_BGN + 90 );//sendThisSource
//主席终端发给MCU的停止选看终端命令，MT->MCU，消息体为结构TMt, 选看终端
OSPEVENT( MT_MCU_STOPSELMT_CMD,			EV_MCUMT_STD_BGN + 92 );//cancelSendThisSource

//主席终端发给MCU的选看终端请求，MT->MCU，消息体为结构TSwitchInfo
OSPEVENT( MT_MCU_STARTSELMT_REQ,		EV_MCUMT_STD_BGN + 94 ); //sendThisSource
//消息体NULL
OSPEVENT( MCU_MT_STARTSELMT_ACK,		EV_MCUMT_STD_BGN + 95 );
OSPEVENT( MCU_MT_STARTSELMT_NACK,		EV_MCUMT_STD_BGN + 96 );

//MCU告知终端当前视频源， 消息体TMt
OSPEVENT( MCU_MT_YOUARESEEING_NOTIF,	EV_MCUMT_STD_BGN + 97 );

//MCU通知终端被选看， 消息体TMt
OSPEVENT( MCU_MT_SEENBYOTHER_NOTIF,		EV_MCUMT_STD_BGN + 98 );
//MCU通知终端被取消选看， 消息体TMt
OSPEVENT( MCU_MT_CANCELSEENBYOTHER_NOTIF,		EV_MCUMT_STD_BGN + 99 );
/************************************************************************/
/* 标准 逻辑信道                                                        */
/************************************************************************/
//MCU发给MT的打开逻辑通道请求，MCU->MT (MCU->SMCU)，消息体TLogicalChannel
OSPEVENT( MCU_MT_OPENLOGICCHNNL_REQ,	EV_MCUMT_STD_BGN + 100 );
//终端发给MCU的成功应答，MT->MCU，消息体为结构TLogicalChannel
OSPEVENT( MT_MCU_OPENLOGICCHNNL_ACK,	EV_MCUMT_STD_BGN + 101 );
//终端发给MCU的失败应答，MT->MCU，消息体为TLogicalChannel
OSPEVENT( MT_MCU_OPENLOGICCHNNL_NACK,	EV_MCUMT_STD_BGN + 102 );

//MT发给MCU的打开逻辑通道请求，MT->MCU，消息体为结构TLogicalChannel
OSPEVENT( MT_MCU_OPENLOGICCHNNL_REQ,	EV_MCUMT_STD_BGN + 105 );
//MCU发给终端的成功应答，MCU->MT (MCU->SMCU)，消息体为结构TLogicalChannel
OSPEVENT( MCU_MT_OPENLOGICCHNNL_ACK,	EV_MCUMT_STD_BGN + 106 );
//MCU发给终端的失败应答，MCU->MT (MCU->SMCU)，消息体为结构TLogicalChannel
OSPEVENT( MCU_MT_OPENLOGICCHNNL_NACK,	EV_MCUMT_STD_BGN + 107 );
//协议栈回调给MtAdp转给MCU的逻辑通道成功建立的通知，MT(stack)->MCU(SMCU->MCU)，消息体为结构TLogicChannel
OSPEVENT( MT_MCU_LOGICCHNNLOPENED_NTF,  EV_MCUMT_STD_BGN + 104 );

//MCU发给终端的ChannelOn消息,消息体无
OSPEVENT( MCU_MT_CHANNELON_CMD,	        EV_MCUMT_STD_BGN + 108 );
//MCU发给终端的ChannelOff消息,消息体无
OSPEVENT( MCU_MT_CHANNELOFF_CMD,        EV_MCUMT_STD_BGN + 109 );

//终端开始环回申请，消息体：TLogicalChannel
OSPEVENT( MT_MCU_MEDIALOOPON_REQ,		EV_MCUMT_STD_BGN + 111 );
//消息体：TLogicalChannel
OSPEVENT( MCU_MT_MEDIALOOPON_ACK,		EV_MCUMT_STD_BGN + 112 );
//消息体：TLogicalChannel
OSPEVENT( MCU_MT_MEDIALOOPON_NACK,		EV_MCUMT_STD_BGN + 113 );
//终端结束环回命令，消息体：TLogicalChannel
OSPEVENT( MT_MCU_MEDIALOOPOFF_CMD,		EV_MCUMT_STD_BGN + 115 );

//MCU发给终端冻结图像命令，MCU->MT，无消息体
OSPEVENT( MCU_MT_FREEZEPIC_CMD,			EV_MCUMT_STD_BGN + 116 );
OSPEVENT( MT_MCU_FREEZEPIC_CMD,			EV_MCUMT_STD_BGN + 118 );
//MCU发给终端快速更新图像命令，MCU->MT (MCU->SMCU)，消息体u8 byMode(MODE_VIDEO,MODE_SECVIDEO );
OSPEVENT( MCU_MT_FASTUPDATEPIC_CMD,		EV_MCUMT_STD_BGN + 120 );
OSPEVENT( MT_MCU_FASTUPDATEPIC_CMD,		EV_MCUMT_STD_BGN + 122 );

//码流控制命令, TLogicChannel
OSPEVENT( MT_MCU_FLOWCONTROL_CMD,		EV_MCUMT_STD_BGN + 124 );
OSPEVENT( MCU_MT_FLOWCONTROL_CMD,		EV_MCUMT_STD_BGN + 126 );

//MCU发给MT的关闭逻辑通道请求，MCU->MT (MCU->SMCU)，消息体为结构TLogicalChannel
OSPEVENT( MCU_MT_CLOSELOGICCHNNL_CMD,   EV_MCUMT_STD_BGN + 128 );

//MT发给MCU的关闭逻辑通道请求，MT->MCU，
//消息体为结构TLogicalChannel+(u8)byOut是否输出
OSPEVENT( MT_MCU_CLOSELOGICCHNNL_NOTIF, EV_MCUMT_STD_BGN + 130 );

//码流控制指示, TLogicChannel
OSPEVENT( MT_MCU_FLOWCONTROLINDICATION_NOTIF,	EV_MCUMT_STD_BGN + 132 );
OSPEVENT( MCU_MT_FLOWCONTROLINDICATION_NOTIF,	EV_MCUMT_STD_BGN + 134 );

/************************************************************************/
/* 标准 环路维护                                                        */
/************************************************************************/

//MCU发给MT的RoundTripDelay请求，MCU->MT (MCU->SMCU)，无消息体
OSPEVENT( MCU_MT_ROUNDTRIPDELAY_REQ,	EV_MCUMT_STD_BGN + 135 );
//终端发给MCU的成功应答，MT->MCU，无消息体
OSPEVENT( MT_MCU_ROUNDTRIPDELAY_ACK,	EV_MCUMT_STD_BGN + 136 );
//MT发给MCU的RoundTripDelay请求，MCU->MT，无消息体
OSPEVENT( MT_MCU_ROUNDTRIPDELAY_REQ,	EV_MCUMT_STD_BGN + 139 );
//MCU发给终端的成功应答，MT->MCU，无消息体
OSPEVENT( MCU_MT_ROUNDTRIPDELAY_ACK,	EV_MCUMT_STD_BGN + 140 );


/************************************************************************/
/* 标准 其他                                                            */
/************************************************************************/

//MCU发给终端的能力集通知，MCU->MT (MCU->SMCU)，消息体为结构TCapSupport+u8(bySupportH263+)+u8(bySendMSDetermine)+TCapSupportEx
OSPEVENT( MCU_MT_CAPBILITYSET_NOTIF,	EV_MCUMT_STD_BGN + 142 );
//终端发给MCU的能力集通知，MT->MCU，消息体为结构TMultiCapSupport
OSPEVENT( MT_MCU_CAPBILITYSET_NOTIF,	EV_MCUMT_STD_BGN + 144 );
//终端适配发给业务的主从确定结果的通知，消息体u8(确定结果), 1为master，其他为slave
OSPEVENT( MT_MCU_MSD_NOTIF,				EV_MCUMT_STD_BGN + 146 );


//终端发给MCU的会议与会终端列表查询请求，MT->MCU，无消息体
OSPEVENT( MT_MCU_JOINEDMTLIST_REQ,		EV_MCUMT_STD_BGN + 150 );
//MCU返回成功结果，MCU->MT，消息体为TMt数组
OSPEVENT( MCU_MT_JOINEDMTLIST_ACK,		EV_MCUMT_STD_BGN + 151 );
//MCU返回失败结果，MCU->MT，消息体
OSPEVENT( MCU_MT_JOINEDMTLIST_NACK,		EV_MCUMT_STD_BGN + 152 );

//终端发给MCU的会议与会终端列表及其ID查询请求，MT->MCU，无消息体
OSPEVENT( MT_MCU_JOINEDMTLISTID_REQ,	EV_MCUMT_STD_BGN + 160 );
//MCU返回成功结果，MCU->MT，消息体为(TMt + TMtAlias)数组的压缩形式
OSPEVENT( MCU_MT_JOINEDMTLISTID_ACK,	EV_MCUMT_STD_BGN + 161 );
OSPEVENT( MCU_MT_JOINEDMTLISTID_NACK,	EV_MCUMT_STD_BGN + 162 );

//查询特定终端别名，消息体TMt
OSPEVENT( MT_MCU_GETMTALIAS_REQ,        EV_MCUMT_STD_BGN + 165 );
//查询特定终端别名应答，消息体TMt+TMtAlias
OSPEVENT( MCU_MT_GETMTALIAS_ACK,        EV_MCUMT_STD_BGN + 166 );
OSPEVENT( MCU_MT_GETMTALIAS_NACK,       EV_MCUMT_STD_BGN + 167 );
//终端汇报别名, TMtAlias
OSPEVENT( MT_MCU_MTALIAS_NOTIF,         EV_MCUMT_STD_BGN + 170 );


/************************************************************************/
/* 标准 摄像机控制                                                      */
/************************************************************************/

//MCU发给终端的镜头控制命令

//发给终端摄像机镜头运动命令，MCU->MT (MCU->SMCU)，消息体为结构TMt+1字节，
OSPEVENT( MCU_MT_MTCAMERA_CTRL_CMD,		    EV_MCUMT_STD_BGN + 175 );
//发给终端摄像机镜头停止运动命令，MCU->MT (MCU->SMCU)，消息体为结构TMt+1字节，
OSPEVENT( MCU_MT_MTCAMERA_CTRL_STOP,	    EV_MCUMT_STD_BGN + 177 );
//发给终端摄像机遥控器使能命令，MCU->MT (MCU->SMCU)，消息体为结构TMt+1字节，
OSPEVENT( MCU_MT_MTCAMERA_RCENABLE_CMD,	    EV_MCUMT_STD_BGN + 179 );
//发给终端摄像机要求将当前位置信息存入指定位置，MCU->MT，消息体为结构TMt+1字节，
//1～4：预设位置号。
OSPEVENT( MCU_MT_MTCAMERA_SAVETOPOS_CMD,	EV_MCUMT_STD_BGN + 181 );
//发给终端摄像机要求调整到指定位置，MCU->MT (MCU->SMCU)，消息体为结构TMt+1字节，
//1～4：预设位置号。
OSPEVENT( MCU_MT_MTCAMERA_MOVETOPOS_CMD,	EV_MCUMT_STD_BGN + 183 );
//发给终端的切换视频源命令，MCU->MT (MCU->SMCU)，消息体为结构TMt+1字节，
OSPEVENT( MCU_MT_VIDEOSOURCESWITCHED_CMD,	EV_MCUMT_STD_BGN + 184 );
//发给终端的选择视频源命令，MCU->MT (MCU->SMCU)，消息体为结构TMt+1字节，
OSPEVENT( MCU_MT_SETMTVIDSRC_CMD,	EV_MCUMT_STD_BGN + 182 );


//主席终端发的终端镜头控制命令

//发给终端摄像机镜头运动命令，MT->MCU，消息体为结构TEqp+1字节(参见摄像头控制参数定义)
OSPEVENT( MT_MCU_MTCAMERA_CTRL_CMD,			EV_MCUMT_STD_BGN + 185 );
//发给终端摄像机镜头停止运动运动命令，MT->MCU，消息体为结构TEqp+1字节(参见摄像头控制参数定义)
OSPEVENT( MT_MCU_MTCAMERA_CTRL_STOP,		EV_MCUMT_STD_BGN + 187 );
//发给终端摄像机遥控器使能命令，MT->MCU，消息体为结构TEqp+1字节，
OSPEVENT( MT_MCU_MTCAMERA_RCENABLE_CMD,		EV_MCUMT_STD_BGN + 189 );
//发给终端摄像机要求将当前位置信息存入指定位置，MT->MCU，消息体为结构TEqp+1字节，
//1～4：预设位置号。
OSPEVENT( MT_MCU_MTCAMERA_SAVETOPOS_CMD,	EV_MCUMT_STD_BGN + 191 );
//发给终端摄像机要求调整到指定位置，MT->MCU，消息体为结构TEqp+1字节，
//1～4：预设位置号。
OSPEVENT( MT_MCU_MTCAMERA_MOVETOPOS_CMD,	EV_MCUMT_STD_BGN + 193 );

OSPEVENT( MT_MCU_VIDEOSOURCESWITCHED_CMD,	EV_MCUMT_STD_BGN + 194 );
OSPEVENT( MT_MCU_SELECTVIDEOSOURCE_CMD,	    EV_MCUMT_STD_BGN + 195 );


/************************************************************************/
/* 标准 网守操作                                                        */
/************************************************************************/

//注册请求,消息体: TMtAlias+TMtAlias(若为mcu注册，则为323id；其他null)+TTransportAddr[](协议适配板地址)
OSPEVENT( MCU_MT_REGISTERGK_REQ,		    EV_MCUMT_STD_BGN + 200 );
//注册确认, NULL
OSPEVENT( MT_MCU_REGISTERGK_ACK,		    EV_MCUMT_STD_BGN + 201 );
//注册拒绝, NULL
OSPEVENT( MT_MCU_REGISTERGK_NACK,		    EV_MCUMT_STD_BGN + 202 );


//注销请求, 消息体: TMtAlias
OSPEVENT( MCU_MT_UNREGISTERGK_REQ,		    EV_MCUMT_STD_BGN + 210 );
//注销确认, NULL
OSPEVENT( MT_MCU_UNREGISTERGK_ACK,	  	    EV_MCUMT_STD_BGN + 211 );
//注销拒绝, NULL
OSPEVENT( MT_MCU_UNREGISTERGK_NACK,		    EV_MCUMT_STD_BGN + 212 );

//更新其他适配板的MCU及会议实体的GK注册信息的通知, 消息体: TMtAlias+u8(byConfIdx)+u8(byRegState)
OSPEVENT( MCU_MT_UPDATE_REGGKSTATUS_NTF,    EV_MCUMT_STD_BGN + 213 );

//更新其他适配板的GK上注册的GatekeeperID/EndpointID信息的通知, 消息体: TH323EPGKIDAlias(m_tGKID)+TH323EPGKIDAlias(m_tEPID)
OSPEVENT( MT_MCU_UPDATE_GKANDEPID_NTF,	    EV_MCUMT_STD_BGN + 215 );

//更新其他适配板的GK上注册的GatekeeperID/EndpointID信息的通知, 消息体: TH323EPGKIDAlias(m_tGKID)+TH323EPGKIDAlias(m_tEPID)
OSPEVENT( MCU_MT_UPDATE_GKANDEPID_NTF,	    EV_MCUMT_STD_BGN + 216 );

//强制注销 主MCU上所有注册GK的 实体，N+1备份用。消息体：TRASInfo
OSPEVENT( MCU_MT_UNREGGK_NPLUS_CMD,         EV_MCUMT_STD_BGN + 217 );

//通知 备份MCU 恢复其本来的E164号。消息体：NULL
OSPEVENT( MCU_MT_RESTORE_MCUE164_NTF,       EV_MCUMT_STD_BGN + 218 );

/************************************************************************/
/* H239 令牌操作                                                        */
/************************************************************************/

//终端给MCU的 获取H239令牌 权限请求
OSPEVENT( MT_MCU_GETH239TOKEN_REQ,	  	    EV_MCUMT_STD_BGN + 220 );
//MCU给终端的 获取H239令牌权限请求的 回应通知
OSPEVENT( MCU_MT_GETH239TOKEN_ACK,	        EV_MCUMT_STD_BGN + 221 );
//MCU给终端的 获取H239令牌权限请求的 回应通知
OSPEVENT( MCU_MT_GETH239TOKEN_NACK,	        EV_MCUMT_STD_BGN + 222 );

//终端给MCU的 拥有 H239令牌 权限通知
OSPEVENT( MT_MCU_OWNH239TOKEN_NOTIF,	    EV_MCUMT_STD_BGN + 223 );
//终端给MCU的 释放 H239令牌 权限通知
OSPEVENT( MT_MCU_RELEASEH239TOKEN_NOTIF,	EV_MCUMT_STD_BGN + 224 );

//MCU给终端的 释放H239令牌 权限命令 - 
//实际为请求操作，一般用于释放对方令牌,一般认为默认成功且不作response回应处理
OSPEVENT( MCU_MT_RELEASEH239TOKEN_CMD,	  	EV_MCUMT_STD_BGN + 225 );
//MCU给终端的 拥有 H239令牌 权限通知
OSPEVENT( MCU_MT_OWNH239TOKEN_NOTIF,	    EV_MCUMT_STD_BGN + 226 );

//MCU发给中兴终端或mcu的申请主席请求(为适应远遥的支持)，MCU->ZTE_MT, 消息体: NULL
OSPEVENT( MCU_MT_APPLYCHAIRMAN_CMD,			EV_MCUMT_STD_BGN + 228 );
//mcu发送取消主席请求，消息体：无
OSPEVENT( MCU_MT_CANCELCHAIRMAN_CMD,		EV_MCUMT_STD_BGN + 229 );


/************************************************************************/
/* 会议计费操作                                                         */
/************************************************************************/
//主适配向GK建链消息(内部用)
OSPEVENT( MTADP_GK_CONNECT,                 EV_MCUMT_STD_BGN + 230 );

//主适配向GK注册消息 (内部用)
OSPEVENT( MTADP_GK_REGISTER,                EV_MCUMT_STD_BGN + 231 );

//计费链路建立完成上报MCU：消息体 NULL
OSPEVENT( MT_MCU_CHARGE_REGGK_NOTIF,        EV_MCUMT_STD_BGN + 232 );

//MCU发给主适配的开始计费请求, 消息体 TConfChargeInfo
OSPEVENT( MCU_MT_CONF_STARTCHARGE_REQ,      EV_MCUMT_STD_BGN + 234 );
//主适配返回给MCU的开始计费的同意应答, 消息体 TAcctSessionId
OSPEVENT( MT_MCU_CONF_STARTCHARGE_ACK,      EV_MCUMT_STD_BGN + 235 );
//主适配返回给MCU的开始计费的拒绝应答, 消息体 NULL 
OSPEVENT( MT_MCU_CONF_STARTCHARGE_NACK,     EV_MCUMT_STD_BGN + 236 );

//MCU发给主适配的结束计费请求, 消息体 NULL
OSPEVENT( MCU_MT_CONF_STOPCHARGE_REQ,       EV_MCUMT_STD_BGN + 237 );
//主适配返回给MCU的结束计费的同意应答, 消息体 NULL
OSPEVENT( MT_MCU_CONF_STOPCHARGE_ACK,       EV_MCUMT_STD_BGN + 238 );
//主适配返回给MCU的结束计费的拒绝应答, 消息体 NULL
OSPEVENT( MT_MCU_CONF_STOPCHARGE_NACK,      EV_MCUMT_STD_BGN + 239 );

//主适配发给MCU的会议计费异常通知，消息体 NULL
OSPEVENT( MT_MCU_CONF_CHARGEEXP_NOTIF,		EV_MCUMT_STD_BGN + 240 );

//主适配发起MCU的停止计费命令，消息体: TConfChargeInfo + TAcctSessionId
OSPEVENT( MCU_MT_CONF_STOPCHARGE_CMD,       EV_MCUMT_STD_BGN + 241  );

/*  针对Polycom的MCU打开双流通道增加 H239 处理  */

//KEDAMCU向Polycom的MCU发起H239令牌请求
OSPEVENT( MCU_POLY_GETH239TOKEN_REQ,			EV_MCUMT_STD_BGN + 245 );
//PolyMCU 获取令牌的回应
OSPEVENT( POLY_MCU_GETH239TOKEN_ACK,			EV_MCUMT_STD_BGN + 246 );
//PolyMCU 获取令牌的回应
OSPEVENT( POLY_MCU_GETH239TOKEN_NACK,			EV_MCUMT_STD_BGN + 247 );

//下列消息 针对polymcu作下级时，暂未使用
//KEDAMCU 给PolyMCU 的拥有H239令牌的权限通知
OSPEVENT( MCU_POLY_OWNH239TOKEN_NOTIF,			EV_MCUMT_STD_BGN + 248 );
//PolyMCU 给KEDAMCU 的释放H239令牌的权限通知
OSPEVENT( POLY_MCU_RELEASEH239TOKEN_NOTIF,		EV_MCUMT_STD_BGN + 249 );
//KEDAMCU 给PolyMCU 释放H239令牌权限的命令
OSPEVENT( MCU_POLY_RELEASEH239TOKEN_CMD,		EV_MCUMT_STD_BGN + 252 );

//PolyMCU给KEDAMCU的释放H239令牌 命令
OSPEVENT( POLY_MCU_RELEASEH239TOKEN_CMD,		EV_MCUMT_STD_BGN + 250 );
//PolyMCU给KEDAMCU的 拥有H239令牌的 权限通知
OSPEVENT( POLY_MCU_OWNH239TOKEN_NOTIF,			EV_MCUMT_STD_BGN + 251 );

/************************************************************************/
/* MCU <=> MMCU    (标准＋自定义)                                       */
/************************************************************************/

//mmcu asks password
OSPEVENT(MMCU_MCU_ENTERPASSWORD_REQ,        EV_MCUMCU_STD_BGN + 1 );
OSPEVENT(MCU_MMCU_ENTERPASSWORD_ACK,        EV_MCUMCU_STD_BGN + 2 );
OSPEVENT(MCU_MMCU_ENTERPASSWORD_NACK,       EV_MCUMCU_STD_BGN + 3 );

//mcu ask another mcu for mtlist
// 头部添加表示是否要发给MMcu [pengguofeng 7/10/2013]
//content: bySend2MMcu + byMcuNum + byMcuNum * byMcuMtId
OSPEVENT( MCU_MTADP_GETMTLIST_CMD,          EV_MCUMCU_STD_BGN + 4);
//content: bySend2MMcu + byMcuMtId + byMtNum + [ byMtId + wNameLen + pName ] * byMtNum + byMcuMtId_x + byMtNum_x + ...
OSPEVENT( MTADP_MCU_GETMTLIST_NOTIF,        EV_MCUMCU_STD_BGN + 7);
// mcu向子mcu获取终端别名信息，目前用于下级终端在vmp中别名显示(别名扩容)。
//content:byEqpid + byMcuId + byMtNum + byMtNum*[ byMtId + byChlNum + byChlNum*[byChlIdx]]
OSPEVENT( MCU_MTADP_GETSMCUMTALIAS_CMD,     EV_MCUMCU_STD_BGN + 8);
//content:byEqpid + byMcuId + byMtNum + byMtNum*[ byMtId + byChlNum + byChlNum*[byChlIdx] + byNameLen + pName]
OSPEVENT( MTADP_MCU_GETSMCUMTALIAS_NOTIF,   EV_MCUMCU_STD_BGN + 9);


/************************************************************************/
/* MCU <=> MMCU  3.6版本的标准级连消息                                  */
/************************************************************************/


////////////////增加终端部分//////////////////////////

//本级mcu在下级mcu上增加终端，消息体为TMcuMcuReq+TMCU+TAddMtInfo数组
OSPEVENT(MCU_MCU_INVITEMT_REQ,			    EV_MCUMCU_STD_BGN + 10 );
OSPEVENT(MCU_MCU_INVITEMT_ACK,			    EV_MCUMCU_STD_BGN + 11 );
OSPEVENT(MCU_MCU_INVITEMT_NACK,			    EV_MCUMCU_STD_BGN + 12 );
//对方MCU增加新终端通知, 消息体TMcuMcuMtInfo
OSPEVENT(MCU_MCU_NEWMT_NOTIF,			    EV_MCUMCU_STD_BGN + 13 );

//本级mcu在下级mcu上呼叫终端，消息体为TMcuMcuReq+TMt
OSPEVENT(MCU_MCU_REINVITEMT_REQ,			EV_MCUMCU_STD_BGN + 14 );
OSPEVENT(MCU_MCU_REINVITEMT_ACK,			EV_MCUMCU_STD_BGN + 15 );
OSPEVENT(MCU_MCU_REINVITEMT_NACK,			EV_MCUMCU_STD_BGN + 16 );

//mcu<->mcu挂断终端请求，消息体为TMcuMcuReq+TMt
OSPEVENT(MCU_MCU_DROPMT_REQ,				EV_MCUMCU_STD_BGN + 17 );
OSPEVENT(MCU_MCU_DROPMT_ACK,				EV_MCUMCU_STD_BGN + 18 );
OSPEVENT(MCU_MCU_DROPMT_NACK,				EV_MCUMCU_STD_BGN + 19 );

//mcu<->mcu挂断终端通知，消息体为TMt
OSPEVENT(MCU_MCU_DROPMT_NOTIF,				EV_MCUMCU_STD_BGN + 20 );

//正在呼叫通报, 消息体为TMt+TMtAlias
OSPEVENT(MCU_MCU_CALLALERTING_NOTIF, EV_MCUMCU_STD_BGN + 21 );

//mcu<->mcu删除终端请求，消息体为TMcuMcuReq+TMt
OSPEVENT(MCU_MCU_DELMT_REQ,					EV_MCUMCU_STD_BGN + 22 );
OSPEVENT(MCU_MCU_DELMT_ACK,					EV_MCUMCU_STD_BGN + 23 );
OSPEVENT(MCU_MCU_DELMT_NACK,				EV_MCUMCU_STD_BGN + 24 );

//mcu<->mcu删除终端通知，消息体为TMt
OSPEVENT(MCU_MCU_DELMT_NOTIF,				EV_MCUMCU_STD_BGN + 25 );

//mcu-mcu请求设置媒体通道开关, 消息体:TMcuMcuReq+TMtMediaChanStatus
OSPEVENT(MCU_MCU_SETMTCHAN_REQ,				EV_MCUMCU_STD_BGN + 26 );
OSPEVENT(MCU_MCU_SETMTCHAN_ACK,				EV_MCUMCU_STD_BGN + 27 );
OSPEVENT(MCU_MCU_SETMTCHAN_NACK,			EV_MCUMCU_STD_BGN + 28 );

//mcu-mcu媒体通道开关状态通知, 消息体:TMtMediaChanStatus
OSPEVENT(MCU_MCU_SETMTCHAN_NOTIF,			EV_MCUMCU_STD_BGN + 29 );

//mcu-mcu指定终端图像进入view,消息体:TMcuMcuReq+TSetInParam+  Optional-H320(bySwitchMode) 
OSPEVENT(MCU_MCU_SETIN_REQ,					EV_MCUMCU_STD_BGN + 30 );
OSPEVENT(MCU_MCU_SETIN_ACK,					EV_MCUMCU_STD_BGN + 31 );
OSPEVENT(MCU_MCU_SETIN_NACK,				EV_MCUMCU_STD_BGN + 32 );

//mcu-mcu指定view输入到终端, 消息体:TMcuMcuReq+TSetOutParam
OSPEVENT(MCU_MCU_SETOUT_REQ,				EV_MCUMCU_STD_BGN + 33 );
OSPEVENT(MCU_MCU_SETOUT_ACK,				EV_MCUMCU_STD_BGN + 34 );
OSPEVENT(MCU_MCU_SETOUT_NACK,				EV_MCUMCU_STD_BGN + 35 );

//mcu-mcu指定view输入到终端通知, 消息体:TSetOutParam
OSPEVENT(MCU_MCU_SETOUT_NOTIF,				EV_MCUMCU_STD_BGN + 36 );

//RadMcu->KedaMcu 自动轮询请求，消息体：TRadMMcuAutoSwitchReq
OSPEVENT(MCU_MCU_AUTOSWITCH_REQ,            EV_MCUMCU_STD_BGN + 37 );
//KedaMcu->RadMcu 自动轮询应答，消息体：NULL
OSPEVENT(MCU_MCU_AUTOSWITCH_ACK,            EV_MCUMCU_STD_BGN + 38 );
OSPEVENT(MCU_MCU_AUTOSWITCH_NACK,           EV_MCUMCU_STD_BGN + 39 );


////////////////摄像头远遥信令///////////////////////

//跨MCU进行摄像头远遥, 消息体TMt + 1byte：远遥操作类型 + 1 byte: 远遥参数
OSPEVENT( MCU_MCU_FECC_CMD,                 EV_MCUMCU_STD_BGN + 40 );


//至终端短消息通知，MCU->MT 消息体：TMt(终端号为0广播到所有终端)+TROLLMSG(有效长度)
OSPEVENT( MCU_MCU_SENDMSG_NOTIF,			EV_MCUMCU_STD_BGN + 42 );


//MCU级联通道打开后的注册消息,消息体:无
OSPEVENT(MCU_MCU_REGISTER_NOTIF,			EV_MCUMCU_STD_BGN + 44 );

//mcu合并级连打开后的花名册通知,消息体TConfMtInfo
OSPEVENT(MCU_MCU_ROSTER_NOTIF,			    EV_MCUMCU_STD_BGN + 45 );

//lukunpeng 2010/07/23 由于61 - 63已经在广西公安中被MCU_MCU_APPLYSPEAKER_REQ占用，此处消息需要调整
// [pengjie 2010/4/23] 级联调下级终端帧率，消息体:Tmt(要调的终端) + u8(是恢复还是调整) + u8(信道类型) + u8(调整的帧率值)
OSPEVENT(MCU_MCU_ADJMTFPS_REQ,	            EV_MCUMCU_STD_BGN + 46 );
OSPEVENT(MCU_MCU_ADJMTFPS_ACK,	            EV_MCUMCU_STD_BGN + 47 );
OSPEVENT(MCU_MCU_ADJMTFPS_NACK,	            EV_MCUMCU_STD_BGN + 48 );


//请求对方MCU的终端列表,消息体TMcuMcuReq
OSPEVENT(MCU_MCU_MTLIST_REQ,			    EV_MCUMCU_STD_BGN + 50 );
//MCU的终端列表请求成功响应,消息体u8(1-最后一包,0-中间包)+TMcuMcuMtInfo[]
OSPEVENT(MCU_MCU_MTLIST_ACK,			    EV_MCUMCU_STD_BGN + 51 );
//MCU的终端列表请求失败响应,消息体无
OSPEVENT(MCU_MCU_MTLIST_NACK,			    EV_MCUMCU_STD_BGN + 52 );

//请求对方MCU的视频信息, 消息体TMcuMcuReq
OSPEVENT(MCU_MCU_VIDEOINFO_REQ,			    EV_MCUMCU_STD_BGN + 55 );
//请求对方MCU的视频信息的成功应答,消息体：TCConfViewInfo
OSPEVENT(MCU_MCU_VIDEOINFO_ACK,			    EV_MCUMCU_STD_BGN + 56 );
//请求对方MCU的视频信息的失败应答,消息体无
OSPEVENT(MCU_MCU_VIDEOINFO_NACK,			EV_MCUMCU_STD_BGN + 57 );

//mcu<->会议视图发生变化,消息体:TCConfViewChangeNtf
OSPEVENT(MCU_MCU_CONFVIEWCHG_NOTIF,	        EV_MCUMCU_STD_BGN + 60 );

//请求申请成为上级的发言人，消息体：TMMcuReqInfo＋TMt
OSPEVENT(MCU_MCU_APPLYSPEAKER_REQ,          EV_MCUMCU_STD_BGN + 61 );
OSPEVENT(MCU_MCU_APPLYSPEAKER_ACK,          EV_MCUMCU_STD_BGN + 62 );
OSPEVENT(MCU_MCU_APPLYSPEAKER_NACK,         EV_MCUMCU_STD_BGN + 63 );

//请求申请成为上级的发言人，消息体：TMt
OSPEVENT(MCU_MCU_APPLYSPEAKER_NOTIF,        EV_MCUMCU_STD_BGN + 64 );

//请求对方MCU的视频信息
OSPEVENT(MCU_MCU_AUDIOINFO_REQ,			    EV_MCUMCU_STD_BGN + 65 );
//请求对方MCU的视频信息的成功应答, 消息体:TCConfAudioInfo
OSPEVENT(MCU_MCU_AUDIOINFO_ACK,			    EV_MCUMCU_STD_BGN + 66 );
//请求对方MCU的视频信息的失败应答,消息体无
OSPEVENT(MCU_MCU_AUDIOINFO_NACK,			EV_MCUMCU_STD_BGN + 67 );

//二级级联：跨级发言状态通知。消息体：TMt ＋ u8 byMode ( emAgreed )
OSPEVENT( MCU_MCU_SPEAKSTATUS_NTF,			EV_MCUMCU_STD_BGN + 68 );
OSPEVENT( MCU_MCU_SPEAKERMODE_NOTIFY,		EV_MCUMCU_STD_BGN + 69 );


/*******************************************************************/
/*nostandard message                                               */
/*******************************************************************/
//请求对方MCU开始混音, 消息体:TMcu+1byte(同时参加讨论(混音)的成员个数)+TMt数组(指定成员混音时才有)
OSPEVENT(MCU_MCU_STARTMIXER_CMD,			EV_MCUMCU_STD_BGN + 70 );
//请求对方MCU开始混音的失败应答,消息体：TMcu+1byte(同时参加讨论(混音)的成员个数)
OSPEVENT(MCU_MCU_STARTMIXER_NOTIF,			EV_MCUMCU_STD_BGN + 73 );

//请求对方MCU停止混音, 消息体:TMcu
//OSPEVENT(MCU_MCU_STOPMIXER_REQ,			EV_MCUMCU_STD_BGN + 75 );
//请求对方MCU开始混音的成功应答, 消息体:TMcu
//OSPEVENT(MCU_MCU_STOPMIXER_ACK,			EV_MCUMCU_STD_BGN + 76 );
//请求对方MCU开始混音的失败应答,消息体TMcu
//OSPEVENT(MCU_MCU_STOPMIXER_NACK,			EV_MCUMCU_STD_BGN + 77 );

//请求对方MCU停止混音, 消息体:TMcu
OSPEVENT(MCU_MCU_STOPMIXER_CMD,				EV_MCUMCU_STD_BGN + 75 );
//请求对方MCU开始混音的失败应答,消息体TMcu
OSPEVENT(MCU_MCU_STOPMIXER_NOTIF,			EV_MCUMCU_STD_BGN + 78 );

//请求对方MCU混音参数, 消息体:TMcu
OSPEVENT(MCU_MCU_GETMIXERPARAM_REQ,			EV_MCUMCU_STD_BGN + 80 );
//请求对方MCU混音参数的成功应答, 消息体TMcu+TDiscussParam
OSPEVENT(MCU_MCU_GETMIXERPARAM_ACK,			EV_MCUMCU_STD_BGN + 81 );
//请求对方MCU混音参数的失败应答,消息体TMcu
OSPEVENT(MCU_MCU_GETMIXERPARAM_NACK,		EV_MCUMCU_STD_BGN + 82 );
//请求对方MCU混音参数通知,消息体TMcu+TDiscussParam
OSPEVENT(MCU_MCU_MIXERPARAM_NOTIF,			EV_MCUMCU_STD_BGN + 83 );

//增加混音成员, 消息体：TMcu+TMt数组
OSPEVENT( MCU_MCU_ADDMIXMEMBER_CMD,		    EV_MCUMCU_STD_BGN + 85 );
//移除混音成员, 消息体：TMcu+TMt数组
OSPEVENT( MCU_MCU_REMOVEMIXMEMBER_CMD,	    EV_MCUMCU_STD_BGN + 87 );

//请求锁定对方MCU, 消息体:TMcu+byLocdMode(CONF_LOCKMODE_NONE,CONF_LOCKMODE_LOCK..)
OSPEVENT(MCU_MCU_LOCK_REQ,			        EV_MCUMCU_STD_BGN + 89 );
//请求锁定对方MCU成功应答, 消息体TMcu
OSPEVENT(MCU_MCU_LOCK_ACK,			        EV_MCUMCU_STD_BGN + 90 );
//请求锁定对方MCU失败应答,消息体TMcu
OSPEVENT(MCU_MCU_LOCK_NACK,			        EV_MCUMCU_STD_BGN + 91 );

//请求对方MCU某一终端状态, 消息体:TMcu
OSPEVENT(MCU_MCU_MTSTATUS_CMD,				EV_MCUMCU_STD_BGN + 92 );
//通知对方MCU自己某一所在直连终端状态,消息体TMcu
OSPEVENT(MCU_MCU_MTSTATUS_NOTIF,			EV_MCUMCU_STD_BGN + 95 );

//请求对方MCU调整其下直连终端的视频分辨率，消息体:
OSPEVENT(MCU_MCU_ADJMTRES_REQ,				EV_MCUMCU_STD_BGN + 96 );
OSPEVENT(MCU_MCU_ADJMTRES_ACK,				EV_MCUMCU_STD_BGN + 97 );
OSPEVENT(MCU_MCU_ADJMTRES_NACK,				EV_MCUMCU_STD_BGN + 98 );

////[5/4/2011 zhushengze]VCS控制发言人发双流
OSPEVENT(MCU_MCU_CHANGEMTSECVIDSEND_CMD,    EV_MCUMCU_STD_BGN + 99);
//[2/23/2012 zhushengze]界面、终端消息透传
OSPEVENT(MCU_MCU_TRANSPARENTMSG_NOTIFY,     EV_MCUMCU_STD_BGN + 100);

/************************************************************************/
/* 自定义 级联多回传消息                                                */
/************************************************************************/
//mcu是否支持多回传能力互探消息,u32 回传带宽
OSPEVENT(MCU_MCU_MULTSPYCAP_NOTIF,	    	 EV_MCUMCU_KDV_BGN + 1 );
  
//上级mcu告诉下级mcu将要进行级联多回传（setin 前的准备），
//下级mcu将据此判断自己是否还能回传（根据上下级总带宽及终端回传码率）
//消息应包括需要回传的终端，回传的mode(AUDIO，VIDEO，BOTH)，回传的媒体格式、码率、帧率
OSPEVENT(MCU_MCU_PRESETIN_REQ,	    	     EV_MCUMCU_KDV_BGN + 2 );
//上级将根据SMCU给的ACK/NACK, 判断其能否有足够带宽来收容一个spy,消息体
//包括下级mcu建立这个回传通道需要的带宽，下级终端是否支持调分辨率的标志
OSPEVENT(MCU_MCU_PRESETIN_ACK,	    	     EV_MCUMCU_KDV_BGN + 3 );
OSPEVENT(MCU_MCU_PRESETIN_NACK,	    	     EV_MCUMCU_KDV_BGN + 4 );

//通知下级mcu，上级可以接受其回传码流的带宽
OSPEVENT(MCU_MCU_SPYCHNNL_NOTIF,	    	 EV_MCUMCU_KDV_BGN + 5 );

//通知下级mcu，上级mcu带宽不足，不能进行级联多回传
OSPEVENT(MCU_MCU_REJECTSPY_NOTIF,	    	 EV_MCUMCU_KDV_BGN + 6 );

//上级mcu发给下级的释放一路音、视频回传通道的CMD，回收回传通道
OSPEVENT(MCU_MCU_BANISHSPY_CMD,	    	     EV_MCUMCU_KDV_BGN + 7 );

// 下级mcu发给上级的释放一路音、视频回传通道的通知，上级收到后释放自己相应的音、视频回传通道资源
//lukunpeng 2010/06/10 由于现在是由上级管理下级带宽占用情况，故不需要再通过下级通知上级
//OSPEVENT(MCU_MCU_BANISHSPY_NOTIF,	    	 EV_MCUMCU_KDV_BGN + 8 );

// 级联多回传关键帧请求消息，消息体为：u8 byMode + Tmt  (音视频模式 + 音视频源)
OSPEVENT(MCU_MCU_SPYFASTUPDATEPIC_CMD,	     EV_MCUMCU_KDV_BGN + 9 );

//非标消息区没有空间了，在级联多回传中添加
//跨级取消发言人的非标消息，消息体：TMt
OSPEVENT(MCU_MCU_CANCELMESPEAKER_REQ,       EV_MCUMCU_KDV_BGN + 10 );
OSPEVENT(MCU_MCU_CANCELMESPEAKER_ACK,       EV_MCUMCU_KDV_BGN + 11 );
OSPEVENT(MCU_MCU_CANCELMESPEAKER_NACK,      EV_MCUMCU_KDV_BGN + 12 );

//级联静音哑音非标消息，消息体：TMt + u8(byMuteOpenFlag 开关启用：1 取消：0) + u8(byMuteType 静音：1 哑音：0)
OSPEVENT(MCU_MCU_MTAUDMUTE_REQ,       EV_MCUMCU_KDV_BGN + 13 );
OSPEVENT(MCU_MCU_MTAUDMUTE_ACK,       EV_MCUMCU_KDV_BGN + 14 );
OSPEVENT(MCU_MCU_MTAUDMUTE_NACK,      EV_MCUMCU_KDV_BGN + 15 );

//级联丢包重传RTCP信息告知消息，消息体：
OSPEVENT(MCU_MCU_MTEXTINFO_NOTIF,     EV_MCUMCU_KDV_BGN+16);

//级联下级建交换到上级MCU失败的告知消息，消息体：
OSPEVENT(MCU_MCU_SWITCHTOMCUFAIL_NOTIF,     EV_MCUMCU_KDV_BGN+17);

//级联下级调码率命令，消息体：TMsgHeadMsg + TMt + u8 bIsRecover + u16 wBitRate
OSPEVENT(MCU_MCU_ADJMTBITRATE_CMD,     EV_MCUMCU_KDV_BGN + 18);
// End

/************************************************************************/
/* 自定义 模块注册消息                                                  */
/************************************************************************/

//终端适配模块发给MCU的OSP连接
OSPEVENT(MTADP_MCU_CONNECT,			        EV_MCUMT_KDV_BGN + 1 );

//终端适配模块内部连接mcu的定时器ID － 为防止event id冲突而统一定义在此处
OSPEVENT(MTADP_MCU_CONNECT2,		        EV_MCUMT_KDV_BGN + 2 );
//终端适配模块内部注册mcu的定时器ID－ 为防止event id冲突而统一定义在此处
OSPEVENT(MTADP_MCU_REGISTER_REQ2,	        EV_MCUMT_KDV_BGN + 4 );

// MCU 通知会话 重联MPCB 相应实例, zgc, 2007/04/30 
OSPEVENT(MCU_MTADP_RECONNECTB_CMD,			EV_MCUMT_KDV_BGN + 3 );

//终端适配模块发给MCU的注册请求
OSPEVENT(MTADP_MCU_REGISTER_REQ,	        EV_MCUMT_KDV_BGN + 5 );
//MCU发给终端适配模块的注册响应
OSPEVENT(MCU_MTADP_REGISTER_ACK,	        EV_MCUMT_KDV_BGN + 6 );
OSPEVENT(MCU_MTADP_REGISTER_NACK,	        EV_MCUMT_KDV_BGN + 7 );

//终端呼叫失败通知
OSPEVENT(MT_MCU_CALLMTFAILURE_NOTIFY,	    EV_MCUMT_KDV_BGN + 8 );

/************************************************************************/
/* 自定义 MCU在线更改GK地址命令		                                            */
/************************************************************************/
//MCU在线更改GK地址, 消息体: 无
OSPEVENT( MCU_MTADP_GKADDR_UPDATE_CMD,		EV_MCUMT_KDV_BGN + 9 );

//主席终端强制广播，消息体：1－开始，0－停止
OSPEVENT( MT_MCU_VIEWBRAODCASTINGSRC_CMD,   EV_MCUMT_KDV_BGN + 10 );

/************************************************************************/
/* 自定义 会议延长或结束通告                                            */
/************************************************************************/
//消息体为u16 分钟
OSPEVENT( MT_MCU_DELAYCONF_REQ,			    EV_MCUMT_KDV_BGN + 12 );
OSPEVENT( MCU_MT_DELAYCONF_ACK,			    EV_MCUMT_KDV_BGN + 13 );
OSPEVENT( MCU_MT_DELAYCONF_NACK,		    EV_MCUMT_KDV_BGN + 14 );
//消息体为u16 分钟
OSPEVENT( MCU_MT_DELAYCONF_NOTIF,		    EV_MCUMT_KDV_BGN + 15 );
OSPEVENT( MCU_MT_CONFWILLEND_NOTIF, 	    EV_MCUMT_KDV_BGN + 17 );

//适配查询mcu状态，消息体：无
OSPEVENT( MTADP_MCU_GETMSSTATUS_REQ,        EV_MCUMT_KDV_BGN + 18 );
//消息体：TMcuMsStatus
OSPEVENT( MCU_MTADP_GETMSSTATUS_ACK,        EV_MCUMT_KDV_BGN + 19 );

/************************************************************************/
/* 自定义 查询通告                                                      */
/************************************************************************/

//终端发给MCU的查询会议中某个终端状态请求，MT->MCU，消息体为结构TMt
OSPEVENT( MT_MCU_GETMTSTATUS_REQ,		    EV_MCUMT_KDV_BGN + 20 );
//MCU回的成功应答，MCU->MT，消息体：TMtStatus
OSPEVENT( MCU_MT_GETMTSTATUS_ACK,		    EV_MCUMT_KDV_BGN + 21 );
//MCU回的失败应答，MCU->MT，消息体为结构TMt
OSPEVENT( MCU_MT_GETMTSTATUS_NACK,		    EV_MCUMT_KDV_BGN + 22 );

//MCU发给终端查询状态请求，MCU->MT，无消息体
OSPEVENT( MCU_MT_GETMTSTATUS_REQ,	        EV_MCUMT_KDV_BGN + 25 );
//MT接受应答，MT->MCU，消息体为结构TMtStatus
OSPEVENT( MT_MCU_GETMTSTATUS_ACK,	        EV_MCUMT_KDV_BGN + 26 );
//MT拒绝应答，MT->MCU，消息体为结构TMt
OSPEVENT( MT_MCU_GETMTSTATUS_NACK,	        EV_MCUMT_KDV_BGN + 27 );
//MT状态改变时主动发给MCU状态消息，消息体为结构TMtStatus
OSPEVENT( MT_MCU_MTSTATUS_NOTIF,	        EV_MCUMT_KDV_BGN + 30 );


//MCU发给终端查询终端版本信息请求，MCU->MT,无消息体
OSPEVENT( MCU_MT_GETMTVERID_REQ,            EV_MCUMT_KDV_BGN + 31 );
//MT接受应答，MT->MCU，消息体 TMtExt2
OSPEVENT( MT_MCU_GETMTVERID_ACK,            EV_MCUMT_KDV_BGN + 32 );
//MT拒绝接受应答，MT->MCU, NULL
OSPEVENT( MT_MCU_GETMTVERID_NACK,           EV_MCUMT_KDV_BGN + 33 );

//终端发给MCU的查询会议完整信息请求，MT->MCU
OSPEVENT( MT_MCU_GETCONFINFO_REQ,		    EV_MCUMT_KDV_BGN + 35 );
//MCU回的成功应答，MCU->MT，消息体为结构TConfFullInfo
OSPEVENT( MCU_MT_GETCONFINFO_ACK,		    EV_MCUMT_KDV_BGN + 36 );
//MCU回的失败应答，MCU->MT，消息体为结构TMt
OSPEVENT( MCU_MT_GETCONFINFO_NACK,		    EV_MCUMT_KDV_BGN + 37 );

//会议信息通知，MCU->MT，消息体为结构TConfFullInfo
OSPEVENT( MCU_MT_CONF_NOTIF,			    EV_MCUMT_KDV_BGN + 40 );

//简化通知终端使用的会议信息通知，MCU->MT，消息体为结构TSimConfInfo 2005-10-11
OSPEVENT( MCU_MT_SIMPLECONF_NOTIF,	        EV_MCUMT_KDV_BGN + 41 );

//和终端建立连接成功通知，1 BYTE， 厂商编号 + u8 + s8*(versionId) + u8 + s8*(ProductId)
OSPEVENT( MT_MCU_MTCONNECTED_NOTIF,	        EV_MCUMT_KDV_BGN + 42 );
//终端成功入会通知, 1byte(1-master,0-slave)
OSPEVENT( MT_MCU_MTJOINCONF_NOTIF,	        EV_MCUMT_KDV_BGN + 44 );

//终端类型通知（320接入时的额外通知）, 1byte(MT_TYPE_MT、TYPE_MCU)
OSPEVENT( MT_MCU_MTTYPE_NOTIF,	            EV_MCUMT_KDV_BGN + 45 );

//和终端连接断开通知，1 Byte. 断链原因
//param: emDisconnectReason
OSPEVENT( MT_MCU_MTDISCONNECTED_NOTIF,	    EV_MCUMT_KDV_BGN + 46 );

/************************************************************************/
/* 自定义 	主席操作                                                    */
/************************************************************************/
//MCU通知终端发言人改变,消息体: TMt(新发言人)
OSPEVENT( MCU_MT_CHANGESPEAKER_NOTIF,		EV_MCUMT_KDV_BGN + 48 );
//MCU通知终端主席改变,消息体: TMt(新主席)
OSPEVENT( MCU_MT_CHANGECHAIRMAN_NOTIF,		EV_MCUMT_KDV_BGN + 50 );

//主席终端发给MCU的指定主席请求，MT->MCU，消息体为结构TMt，代表新主席
OSPEVENT( MT_MCU_SPECCHAIRMAN_REQ,		    EV_MCUMT_KDV_BGN + 55 );
//MCU回的成功应答，MCU->MT，无消息体
OSPEVENT( MCU_MT_SPECCHAIRMAN_ACK,		    EV_MCUMT_KDV_BGN + 56 );
//MCU回的失败应答，MCU->MT，无消息体
OSPEVENT( MCU_MT_SPECCHAIRMAN_NACK,		    EV_MCUMT_KDV_BGN + 57 );

//通知主席终端申请主席请求，MCU->MT，消息体为结构TMt
OSPEVENT( MCU_MT_APPLYCHAIRMAN_NOTIF,	    EV_MCUMT_KDV_BGN + 59 );

//MCU发给终端的指定主席通知，MCU->MT，无消息体
OSPEVENT( MCU_MT_SPECCHAIRMAN_NOTIF,	    EV_MCUMT_KDV_BGN + 61 );
/************************************************************************/
/* 自定义 终端选看相关                                                  */
/************************************************************************/

//终端发给MCU的其它终端选看自己的请求，MT->MCU，消息体为结构TSwitchInfo
OSPEVENT( MT_MCU_STARTMTSELME_REQ,		    EV_MCUMT_KDV_BGN + 65 );
//MCU发给终端的其它终端选看自己的请求应答，MCU->MT，消息体为结构TSwitchInfo
OSPEVENT( MCU_MT_STARTMTSELME_ACK,		    EV_MCUMT_KDV_BGN + 66 );
//MCU发给终端的其它终端选看自己的拒绝，MCU->MT，消息体为结构TSwitchInfo
OSPEVENT( MCU_MT_STARTMTSELME_NACK,		    EV_MCUMT_KDV_BGN + 67 );
//终端发给MCU的停止其它终端选看自己的通知，MT->MCU，消息体为结构TSwitchInfo
OSPEVENT( MT_MCU_STOPMTSELME_CMD,	        EV_MCUMT_KDV_BGN + 70 );

//终端发给MCU的请求MCU组播其它终端的请求，MT->MCU，消息体为结构TMt(被组播的终端)
OSPEVENT( MT_MCU_STARTBROADCASTMT_REQ,	    EV_MCUMT_KDV_BGN + 75 );

//MCU发给终端的组播其它终端的响应，MCU->Mt，消息体为
//TMt(被组播的终端)+TTransportAddr(视频流组播地址)+TTransportAddr(音频流组播地址)
//				   +TMediaEncrypt(视频加密字串)   +TDoublePayload(视频动态载荷)
//				   +TMediaEncrypt(音频加密字串)   +TDoublePayload(音频动态载荷)
OSPEVENT( MCU_MT_STARTBROADCASTMT_ACK,	    EV_MCUMT_KDV_BGN + 76 );

//MCU发给终端的组播其它终端的响应，MCU->Mt, 消息体为TMt(被组播的终端)
OSPEVENT( MCU_MT_STARTBROADCASTMT_NACK,	    EV_MCUMT_KDV_BGN + 77 );

//终端发给MCU的停止MCU组播其它终端的命令，MT->MCU，消息体为结构TMt(其它终端)
OSPEVENT( MT_MCU_STOPBROADCASTMT_CMD,	    EV_MCUMT_KDV_BGN + 80 );

//终端发给MCU的查询其它终端选看情况的请求，MT->MCU，消息体为结构TMt(其它终端)
OSPEVENT( MT_MCU_GETMTSELSTUTS_REQ,		    EV_MCUMT_KDV_BGN + 85 );
//MCU发给终端的查询其它终端选看情况的应答，MCU->MT，消息体为结构TMt(其它终端)+TMt(选看终端)
//TMt(选看终端) 中全0表示未选看
OSPEVENT( MCU_MT_GETMTSELSTUTS_ACK,		    EV_MCUMT_KDV_BGN + 86 );
//MCU发给终端的查询其它终端选看情况的的拒绝，MCU->MT，消息体为结构TMt(其它终端)+TMt(全0)
OSPEVENT( MCU_MT_GETMTSELSTUTS_NACK,	    EV_MCUMT_KDV_BGN + 87 );
//终端呼叫失败通知(因HDI接入能力满所致)
OSPEVENT(MT_MCU_CALLFAIL_HDIFULL_NOTIF,		EV_MCUMT_KDV_BGN + 88 );

/************************************************************************/
/* 自定义 终端视频源别名显示                                            */
/************************************************************************/
// 获取终端视频源别名，消息体: 空
OSPEVENT( MCU_MT_GETMTVIDEOALIAS_CMD,		EV_MCUMT_KDV_BGN + 89 );
// 上报终端视频源别名，消息体: s8 achVidALias[MT_MAXNUM_VIDSOURCE][MT_MAX_PORTNAME_LEN]
OSPEVENT( MT_MCU_MTVIDEOALIAS_NOTIF,		EV_MCUMT_KDV_BGN + 90 );


/************************************************************************/
/* 自定义 设置静音（远端静音）                                          */
/************************************************************************/

//主席终端命令MCU进行终端静音设置，MT->MCU，消息体为结构TMt+1字节，
//1：MUTE，0：NOTMUTE
OSPEVENT( MT_MCU_MTMUTE_CMD,			    EV_MCUMT_KDV_BGN + 100 );

//MCU发给终端某一会议实例对终端静音设置，MCU->MT，消息体为结构TMt+1字节，
//1：MUTE，0：NOTMUTE
OSPEVENT( MCU_MT_MTMUTE_CMD,			    EV_MCUMT_KDV_BGN + 105 );

/************************************************************************/
/* 自定义 设置哑音（近端静音）                                          */
/************************************************************************/

//主席终端命令MCU进行终端哑音设置，MT->MCU，消息体为结构TMt+1字节，
//1：MUTE，2：NOTMUTE
OSPEVENT( MT_MCU_MTDUMB_CMD,			    EV_MCUMT_KDV_BGN + 110 );

//MCU发给终端某一会议实例对终端进行哑音设置，MCU->MT，消息体为结构TMt+1字节，
//1：MUTE，2：NOTMUTE
OSPEVENT( MCU_MT_MTDUMB_CMD,			    EV_MCUMT_KDV_BGN + 115 );

//发给终端的选择视频源命令，MCU->MT (MCU->SMCU)，消息体为结构TMt+1字节，
//OSPEVENT( MCU_MT_SETMTVIDSRC_CMD,	EV_MCUMT_KDV_BGN + 120 );

/************************************************************************/
/*  自定义 语音激励控制                                                 */
/************************************************************************/

//启用语音激励控制发言

   //主席请求MCU开始语音激励控制发言, 消息体：  无
   OSPEVENT( MT_MCU_STARTVAC_REQ ,          EV_MCUMT_KDV_BGN + 130 );
   //MCU同意主席开始语音激励控制发言的请求, 消息体：  无
   OSPEVENT( MCU_MT_STARTVAC_ACK,           EV_MCUMT_KDV_BGN + 131 );
   //MCU拒绝主席开始语音激励控制发言的请求, 消息体：  无
   OSPEVENT( MCU_MT_STARTVAC_NACK,          EV_MCUMT_KDV_BGN + 132 );
   //MCU给主席开始语音激励控制发言的通知, 消息体：  无
   OSPEVENT( MCU_MT_STARTVAC_NOTIF,         EV_MCUMT_KDV_BGN + 135 );

//取消语音激励控制发言
   
   //主席请求MCU停止语音激励控制发言, 消息体：  无
   OSPEVENT( MT_MCU_STOPVAC_REQ ,           EV_MCUMT_KDV_BGN + 140 );
   //MCU同意主席停止语音激励控制发言的请求, 消息体：  无
   OSPEVENT( MCU_MT_STOPVAC_ACK,            EV_MCUMT_KDV_BGN + 141 );
   //MCU拒绝主席停止语音激励控制发言的请求, 消息体：  无
   OSPEVENT( MCU_MT_STOPVAC_NACK,           EV_MCUMT_KDV_BGN + 142 );
   //MCU给主席停止语音激励控制发言的通知, 消息体：  无
   OSPEVENT( MCU_MT_STOPVAC_NOTIF,          EV_MCUMT_KDV_BGN + 150 );

/************************************************************************/
/*  自定义 会议讨论                                                     */
/************************************************************************/

//开始讨论
   
    //开始会议讨论请求, 消息体：  1byte(同时参加讨论(混音)的成员个数) + 如果开启定制混音则追加TMt数组
	OSPEVENT( MT_MCU_STARTDISCUSS_REQ,		EV_MCUMT_KDV_BGN + 160 );
	//MCU同意开始会议讨论, 消息体：  1byte(同时参加讨论(混音)的成员个数)
	OSPEVENT( MCU_MT_STARTDISCUSS_ACK,		EV_MCUMT_KDV_BGN + 161 );
	//MCU拒绝开始会议讨论, 消息体：  1byte(同时参加讨论(混音)的成员个数)
	OSPEVENT( MCU_MT_STARTDISCUSS_NACK,		EV_MCUMT_KDV_BGN + 162 );
	//MCU开始会议讨论通知, 消息体：  1byte(同时参加讨论(混音)的成员个数)
	OSPEVENT( MCU_MT_STARTDISCUSS_NOTIF,	EV_MCUMT_KDV_BGN + 165 );

//结束讨论

	//主席请求MCU结束会议讨论, 消息体：  无
	OSPEVENT( MT_MCU_STOPDISCUSS_REQ,       EV_MCUMT_KDV_BGN + 167 );
	//MCU同意主席结束会议讨论的请求, 消息体： 无
	OSPEVENT( MCU_MT_STOPDISCUSS_ACK,       EV_MCUMT_KDV_BGN + 168 );
	//MCU拒绝主席结束会议讨论的请求, 消息体：  无
	OSPEVENT( MCU_MT_STOPDISCUSS_NACK,      EV_MCUMT_KDV_BGN + 169 );
	//MCU给主席结束会议讨论的通知, 消息体：  无
	OSPEVENT( MCU_MT_STOPDISCUSS_NOTIF,	    EV_MCUMT_KDV_BGN + 175 );

/************************************************************************/
/*  自定义 视频复合                                                     */
/************************************************************************/
//开始视频复合

	//开始视频复合请求, 消息体：  TVMPParam
	OSPEVENT( MT_MCU_STARTVMP_REQ,          EV_MCUMT_KDV_BGN + 180 );
	//MCU同意视频复合请求, 消息体：  TVMPParam
	OSPEVENT( MCU_MT_STARTVMP_ACK,          EV_MCUMT_KDV_BGN + 181 );
	//MCU不同意视频复合请求, 消息体：  无
	OSPEVENT( MCU_MT_STARTVMP_NACK,         EV_MCUMT_KDV_BGN + 182 );
	//MCU开始视频复合的通知, 消息体：  TVMPParam
	OSPEVENT( MCU_MT_STARTVMP_NOTIF,        EV_MCUMT_KDV_BGN + 185 );

//停止视频复合	

	//结束视频复合请求, 消息体：  
	OSPEVENT( MT_MCU_STOPVMP_REQ,		    EV_MCUMT_KDV_BGN + 187 );
	//MCU同意视频结束复合请求, 消息体：  无
	OSPEVENT( MCU_MT_STOPVMP_ACK,		    EV_MCUMT_KDV_BGN + 188 );
	//MCU不同意结束视频复合请求, 消息体：  无
	OSPEVENT( MCU_MT_STOPVMP_NACK,		    EV_MCUMT_KDV_BGN +  189 );
	//MCU结束视频复合的通知, 消息体：  无
	OSPEVENT( MCU_MT_STOPVMP_NOTIF,		    EV_MCUMT_KDV_BGN +  191 );
	
//改变视频复合参数

	//主席请求MCU改变视频复合参数, 消息体：  TVMPParam
	OSPEVENT( MT_MCU_CHANGEVMPPARAM_REQ,	EV_MCUMT_KDV_BGN + 193 );
	//MCU同意主席的改变视频复合参数请求, 消息体： TVMPParam
	OSPEVENT( MCU_MT_CHANGEVMPPARAM_ACK,	EV_MCUMT_KDV_BGN + 194 );
	//MCU拒绝主席的改变视频复合参数请求, 消息体：  无
	OSPEVENT( MCU_MT_CHANGEVMPPARAM_NACK,	EV_MCUMT_KDV_BGN + 195 );

//查询视频复合参数	

	//查询视频复合成员请求, 消息体：  无 
	OSPEVENT( MT_MCU_GETVMPPARAM_REQ,	    EV_MCUMT_KDV_BGN + 197 );
	//MCU同意查询视频复合成员, 消息体：  TVMPParam
	OSPEVENT( MCU_MT_GETVMPPARAM_ACK,	    EV_MCUMT_KDV_BGN + 198 );
	//MCU不同意查询视频复合成员, 消息体：  无
	OSPEVENT( MCU_MT_GETVMPPARAM_NACK,	    EV_MCUMT_KDV_BGN + 199 );
	//MCU给主席的视频复合参数通知, 消息体：  TVMPParam
	OSPEVENT( MCU_MT_VMPPARAM_NOTIF,	    EV_MCUMT_KDV_BGN + 201 );

//开始广播复合视频流
	//主席请求MCU广播视频复合码流, 消息体无  
	OSPEVENT( MT_MCU_STARTVMPBRDST_REQ,		EV_MCUMT_KDV_BGN + 210 );
	//MCU同意广播视频复合码流, 消息体无
	OSPEVENT( MCU_MT_STARTVMPBRDST_ACK,		EV_MCUMT_KDV_BGN + 211 );
	//MCU拒绝广播视频复合码流, 消息体无
	OSPEVENT( MCU_MT_STARTVMPBRDST_NACK,	EV_MCUMT_KDV_BGN + 212 );

//停止广播复合视频流
	//主席请求MCU停止广播视频复合码流, 消息体无  
	OSPEVENT( MT_MCU_STOPVMPBRDST_REQ,		EV_MCUMT_KDV_BGN + 215 );
	//MCU同意停止广播视频复合码流, 消息体无
	OSPEVENT( MCU_MT_STOPVMPBRDST_ACK,		EV_MCUMT_KDV_BGN + 216 );
	//MCU拒绝停止广播视频复合码流, 消息体无
	OSPEVENT( MCU_MT_STOPVMPBRDST_NACK,	    EV_MCUMT_KDV_BGN + 217 );
	//主席选看VMP 请求， 消息体： 无
	OSPEVENT( MT_MCU_STARTSWITCHVMPMT_REQ,	EV_MCUMT_KDV_BGN + 218 );
	//主席选看VMP应答, 消息体： 无
	OSPEVENT( MCU_MT_STARTSWITCHVMPMT_ACK,  EV_MCUMT_KDV_BGN + 219 );
	OSPEVENT( MCU_MT_STARTSWITCHVMPMT_NACK, EV_MCUMT_KDV_BGN + 220 );

/************************************************************************/
/*  自定义  终端轮询选看                                                */
/************************************************************************/

//终端轮询选看命令

   //MCU命令该终端开始轮询选看, 消息体：  TMt +TPollInfo + TMtPollParam数组
   OSPEVENT( MCU_MT_STARTMTPOLL_CMD,        EV_MCUMT_KDV_BGN + 221 ); 
   //MCU命令该终端停止轮询选看, 消息体：  TMt
   OSPEVENT( MCU_MT_STOPMTPOLL_CMD,         EV_MCUMT_KDV_BGN + 223 );
   //MCU命令该终端暂停轮询选看, 消息体：  TMt
   OSPEVENT( MCU_MT_PAUSEMTPOLL_CMD,        EV_MCUMT_KDV_BGN + 225 );
   //MCU命令该终端继续轮询选看, 消息体：  TMt
   OSPEVENT( MCU_MT_RESUMEMTPOLL_CMD,       EV_MCUMT_KDV_BGN + 227 );

//查询终端轮询选看参数

   //MCU向终端查询轮询参数, 消息体：  TMt 
   OSPEVENT( MCU_MT_GETMTPOLLPARAM_REQ,     EV_MCUMT_KDV_BGN + 230 );
   //Mt应答MCU查询终端轮询参数, 消息体：  TMt + TPollInfo + TMtPollParam数组
   OSPEVENT( MT_MCU_GETMTPOLLPARAM_ACK,     EV_MCUMT_KDV_BGN + 231 ); 
   //MT拒绝MCU查询终端轮询参数, 消息体：  TMt
   OSPEVENT( MT_MCU_GETMTPOLLPARAM_NACK,    EV_MCUMT_KDV_BGN + 232 );
   //MT通知MCU当前的终端轮询状态, 消息体：  TMt + TPollInfo + TMtPollParam数组
   OSPEVENT( MT_MCU_POLLMTSTATE_NOTIF,      EV_MCUMT_KDV_BGN + 235 );


/************************************************************************/
/* 自定义 内置矩阵(视频)                                                    */
/************************************************************************/
  
//所有矩阵方案请求  MC->ter, 消息体：无
OSPEVENT( MCU_MT_MATRIX_GETALLSCHEMES_CMD,  EV_MCUMT_KDV_BGN + 237 );
//所有矩阵方案信息通知，消息体为：u8( 方案数 ) + ITInnerMatrixScheme[]
OSPEVENT( MT_MCU_MATRIX_ALLSCHEMES_NOTIF,   EV_MCUMT_KDV_BGN + 240 );

//单个矩阵方案请求  MC->ter, 消息体：u8 ( 0 为默认方案 )       方案索引从( 0 - 6 )
OSPEVENT( MCU_MT_MATRIX_GETONESCHEME_CMD,   EV_MCUMT_KDV_BGN + 241 );
//消息体：ITInnerMatrixScheme
OSPEVENT( MT_MCU_MATRIX_ONESCHEME_NOTIF,    EV_MCUMT_KDV_BGN + 245 );

//保存矩阵方案命令,消息体:ITInnerMatrixScheme
OSPEVENT( MCU_MT_MATRIX_SAVESCHEME_CMD,		EV_MCUMT_KDV_BGN + 246 );
//消息体：ITInnerMatrixScheme
OSPEVENT( MT_MCU_MATRIX_SAVESCHEME_NOTIF,   EV_MCUMT_KDV_BGN + 249 );

//指定当前方案,消息体：u8  方案索引
OSPEVENT( MCU_MT_MATRIX_SETCURSCHEME_CMD,   EV_MCUMT_KDV_BGN + 250 );
//消息体：u8  方案索引
OSPEVENT( MT_MCU_MATRIX_SETCURSCHEME_NOTIF, EV_MCUMT_KDV_BGN + 253 );

//获取当前方案,消息体：无
OSPEVENT( MCU_MT_MATRIX_GETCURSCHEME_CMD,   EV_MCUMT_KDV_BGN + 254 );
//消息体：u8  方案索引
OSPEVENT( MT_MCU_MATRIX_CURSCHEME_NOTIF,    EV_MCUMT_KDV_BGN + 257 );

/************************************************************************/
/* 外置矩阵                                                             */
/************************************************************************/

//获取终端外置矩阵类型，消息体：无
OSPEVENT( MCU_MT_EXMATRIX_GETINFO_CMD,      EV_MCUMT_KDV_BGN + 260 );
//终端外置矩阵类型指示, 消息体：u8 （矩阵输入端口数 0表示没有矩阵）
OSPEVENT( MT_MCU_EXMATRIXINFO_NOTIFY,       EV_MCUMT_KDV_BGN + 263 );

//设置外置矩阵连接端口号,消息体：u8  (1-64 )
OSPEVENT( MCU_MT_EXMATRIX_SETPORT_CMD,      EV_MCUMT_KDV_BGN + 264 );

//请求获取外置矩阵连接端口,消息体：无
OSPEVENT( MCU_MT_EXMATRIX_GETPORT_REQ,      EV_MCUMT_KDV_BGN + 268 );
//消息体：无
OSPEVENT( MT_MCU_EXMATRIX_GETPORT_ACK,      EV_MCUMT_KDV_BGN + 269 );
//消息体：无
OSPEVENT( MT_MCU_EXMATRIX_GETPORT_NACK,     EV_MCUMT_KDV_BGN + 270 );
//消息体：u8  (1-64 )
OSPEVENT( MT_MCU_EXMATRIX_GETPORT_NOTIF,    EV_MCUMT_KDV_BGN + 271 );

//设置外置矩阵连接端口名,消息体：u8  (1-64 )+s8[MAXLEN_MATRIXPORTNAME] 0结尾的字符串 
OSPEVENT( MCU_MT_EXMATRIX_SETPORTNAME_CMD,  EV_MCUMT_KDV_BGN + 272 );
//外置矩阵端口名指示, 消息体：u8  (1-64 )+s8[MAXLEN_MATRIXPORTNAME] 0结尾的字符串 
OSPEVENT( MT_MCU_EXMATRIX_PORTNAME_NOTIF,   EV_MCUMT_KDV_BGN + 275 );

//请求获取外置矩阵的所有端口名,消息体：无
OSPEVENT( MCU_MT_EXMATRIX_GETALLPORTNAME_CMD,   EV_MCUMT_KDV_BGN + 276 );
//外置矩阵的所有端口名指示，消息体：s8[][MAXLEN_MATRIXPORTNAME] 索引0 表示输出端口名 其余为输入端口，（字符串数组）
OSPEVENT( MT_MCU_EXMATRIX_ALLPORTNAME_NOTIF,    EV_MCUMT_KDV_BGN + 279 );


/************************************************************************/
/* 自定义 其他消息                                                      */
/************************************************************************/
//主席终端发给MCU的邀请终端请求，MT->MCU，消息体为结构TMcu+TAddMtInfo

OSPEVENT( MT_MCU_ADDMT_REQ,				    EV_MCUMT_KDV_BGN + 280 );
//MCU回的成功应答，MCU->MT，消息体为结构TMtStatus
OSPEVENT( MCU_MT_ADDMT_ACK,				    EV_MCUMT_KDV_BGN + 281 );
//MCU回的失败应答，MCU->MT，无消息体
OSPEVENT( MCU_MT_ADDMT_NACK,			    EV_MCUMT_KDV_BGN + 282 );

//至终端短消息通知，MCU->MT 消息体：TMt(终端号为0广播到所有终端)+TROLLMSG(有效长度)
OSPEVENT( MCU_MT_SENDMSG_NOTIF,			    EV_MCUMT_KDV_BGN + 285 );

//终端至终端短消息命令，MT->MCU
//消息体：2 byte(终端数,网络序,值为N,0为广播到所有终端) + N个TMt +CRollMsg(有效长度)
OSPEVENT( MT_MCU_SENDMSG_CMD,			    EV_MCUMT_KDV_BGN + 287 );

//终端至会议控制台短消息命令，MT->MCU，消息体为结构TMt+CRollMsg（有效长度）
OSPEVENT( MT_MCU_SENDMCMSG_CMD,			    EV_MCUMT_KDV_BGN + 289 );

//终端视频源名称指示    MC->ter,消息体是字符串，'\0'结束, 最大长度1024字节
OSPEVENT(MCU_MT_MTSOURCENAME_NOTIF,	        EV_MCUMT_KDV_BGN + 292 );
//主席移除混音终端  MT->MCU, TMt
OSPEVENT(MT_MCU_REMOVEMIXMEMBER_CMD,		EV_MCUMT_KDV_BGN + 295 );
//主席增加混音终端，MT->MCU，TMt
OSPEVENT( MT_MCU_ADDMIXMEMBER_CMD,		    EV_MCUMT_KDV_BGN + 300 );
//终端发给MCU的申请参加混音请求，MT->MCU，消息体NULL
OSPEVENT( MT_MCU_APPLYMIX_NOTIF,	        EV_MCUMT_KDV_BGN + 305 ); 
//通知主席终端其它终端申请参加混音请求，MCU->MT，消息体为结构TMt
OSPEVENT( MCU_MT_MTAPPLYMIX_NOTIF,	        EV_MCUMT_KDV_BGN + 310 ); 
//通知终端已成功参加混音，MCU->MT，消息体: 0-不在插话 1-在插话
OSPEVENT( MCU_MT_MTADDMIX_NOTIF,	        EV_MCUMT_KDV_BGN + 315 );
//通知终端定制混音状态, MCU->MT, 消息体：TMixParam	// [11/25/2010 xliang] add for T3, R3-full mt
OSPEVENT( MCU_MT_DISCUSSPARAM_NOTIF,        EV_MCUMT_KDV_BGN + 316 );
//设置qos，消息体：TMcuQosCfgInfo
OSPEVENT(MCU_MT_SETQOS_CMD,                 EV_MCUMT_KDV_BGN + 320 );

//更新协议适配板配置信息,消息体：TMtAdpCfg
OSPEVENT(MCU_MT_MTADPCFG_CMD,               EV_MCUMT_KDV_BGN + 325 );

//获取终端码率
OSPEVENT(MCU_MT_GETBITRATEINFO_REQ,         EV_MCUMT_KDV_BGN + 330 );
//消息体：无
OSPEVENT(MT_MCU_GETBITRATEINFO_ACK,         EV_MCUMT_KDV_BGN + 331 );
OSPEVENT(MT_MCU_GETBITRATEINFO_NACK,        EV_MCUMT_KDV_BGN + 332 );
//消息体：TMtBitrate
OSPEVENT(MT_MCU_GETBITRATEINFO_NOTIF,       EV_MCUMT_KDV_BGN + 333 );

//终端接收带宽通知, 消息体：u16接收带宽 + u16发送带宽（包括音频和视频）
OSPEVENT(MT_MCU_BANDWIDTH_NOTIF,            EV_MCUMT_KDV_BGN + 334 );

//终端扩展视频源
//获取终端视频源信息,消息体：无
OSPEVENT( MCU_MT_GETVIDEOSOURCEINFO_CMD,    EV_MCUMT_KDV_BGN + 340 );
//终端全部视频源指示，消息体：tagITVideoSourceInfo[]
OSPEVENT( MT_MCU_ALLVIDEOSOURCEINFO_NOTIF,  EV_MCUMT_KDV_BGN + 343 );

//设置终端视频源信息，消息体：tagITVideoSourceInfo
OSPEVENT( MCU_MT_SETVIDEOSOURCEINFO_CMD,    EV_MCUMT_KDV_BGN + 344 );
//终端视频源信息指示,消息体：tagITVideoSourceInfo
OSPEVENT( MT_MCU_VIDEOSOURCEINFO_NOTIF,     EV_MCUMT_KDV_BGN + 347 );

//发给终端的选择扩展视频源命令, 消息体：u8
OSPEVENT( MCU_MT_SELECTEXVIDEOSRC_CMD,      EV_MCUMT_KDV_BGN + 348 );

//发给终端的视频分辨率改变的命令(H264会议VMP时发送)，消息体：u8(视频分辨率) + u8 通道类型(LOGCHL_VIDEO)
//OSPEVENT( MCU_MT_VIDEOFORMATCHANGE_CMD,     EV_MCUMT_KDV_BGN + 349 );
//xliang [080731] 以上消息名做如下更改
//发给终端的视频分辨率改变的命令,消息体：u8 通道类型 + u8 信道类型(H264,MPEG4等) + u8 分辨率类型 + u8 帧率
OSPEVENT( MCU_MT_VIDEOPARAMCHANGE_CMD,     EV_MCUMT_KDV_BGN + 349 );
// xliang [1/6/2009] 发给终端的恢复视频分辨率命令,消息体：无
OSPEVENT( MCU_MT_VIDEOPARAMRECOVER_CMD,		EV_MCUMT_KDV_BGN + 350); 
// [pengjie 2010/4/22] 发给终端的视频帧率改变的命令,消息体：u8 通道类型  + u8 帧率
OSPEVENT( MCU_MT_VIDEOFPSCHANGE_CMD,		EV_MCUMT_KDV_BGN + 351);

//[5/4/2011 zhushengze]VCS控制发言人发双流
OSPEVENT(MCU_MT_CHANGEMTSECVIDSEND_CMD,         EV_MCUMT_KDV_BGN + 352 );

//[2/23/2012 zhushengze]界面、终端消息透传
OSPEVENT(MCU_MT_TRANSPARENTMSG_NOTIFY,          EV_MCUMT_KDV_BGN + 353 );
OSPEVENT(MT_MCU_TRANSPARENTMSG_NOTIFY,          EV_MCUMT_KDV_BGN + 354 );

//卫星会议终端接收地址变化命令，消息体：u8(LOGCHL_VIDEO,...) + TTransportAddr(终端接收码流地址)
OSPEVENT( MCU_MT_SATDCONFCHGADDR_CMD,       EV_MCUMT_KDV_BGN + 356 );

//MUC->MT卫星会议组播地址通知，消息体：u32 第一路视频地址 +   u16   第一路视频端口
//                                     u32 第一路音频地址 +   u16   第一路音频端口
//                                     u32 第二路视频地址 +   u16   第二路视频端口
// 各个地址端口均为网络序
//OSPEVENT( MCU_MT_SATDCONFADDR_NOTIF,        EV_MCUMT_KDV_BGN + 363 );

//发给终端的调节终端音量的命令，消息体：u8(音量类型 VOLUME_TYPE_IN ...) + u8(调整后的音量)
OSPEVENT( MCU_MT_SETMTVOLUME_CMD,           EV_MCUMT_KDV_BGN + 357 );

//发给终端自定义消息 通用通知，消息体：Mcu2MtNtfMsgType
OSPEVENT( MCU_MT_SOMEOFFERRING_NOTIF,       EV_MCUMT_KDV_BGN + 358 );

// 抢占请求是否可释放该终端    消息体: s8[]请求抢占的会议名称
OSPEVENT( MT_MCU_RELEASEMT_REQ,             EV_MCUMT_KDV_BGN + 359 );
OSPEVENT( MCU_MT_RELEASEMT_ACK,             EV_MCUMT_KDV_BGN + 360 );
OSPEVENT( MCU_MT_RELEASEMT_NACK,            EV_MCUMT_KDV_BGN + 361 );

//上级mcu的会议名称在下级的显示类型(mcu名称+会议名称或者会议名称) 消息体:u8(显示类型)
OSPEVENT( MCU_MT_MMCUCONFNAMESHOWTYPE_CMD,	EV_MCUMT_KDV_BGN + 362 );

//mcu的行政级别 消息体:u8(行政级别)
OSPEVENT( MCU_MT_CHANGEADMINLEVEL_CMD,	EV_MCUMT_KDV_BGN + 363 );

// 通知终端当前的发言状态，消息体：u32 / emMtSpeakerStatus
OSPEVENT( MCU_MT_MTSPEAKSTATUS_NTF,         EV_MCUMT_KDV_BGN + 364 );

// 通知所有终端会议当前的发言状态，消息体：u32 / emConfSpeakMode
OSPEVENT( MCU_MT_CONFSPEAKMODE_NTF,         EV_MCUMT_KDV_BGN + 365 );

// 终端取消自己发言人申请，消息体：NULL
OSPEVENT( MT_MCU_APPLYCANCELSPEAKER_REQ,       EV_MCUMT_KDV_BGN + 367 );
OSPEVENT( MCU_MT_APPLYCANCELSPEAKER_ACK,       EV_MCUMT_KDV_BGN + 368 );
OSPEVENT( MCU_MT_APPLYCANCELSPEAKER_NACK,      EV_MCUMT_KDV_BGN + 369 );

/************************************************************************/
/* mt调试消息	                                                        */
/************************************************************************/
OSPEVENT( MCU_MT_DBG_FLOWCTRLCMD,			   EV_MCUMT_KDV_BGN + 380 );


/************************************************************************/
/* 协议栈回调消息                                                       */
/************************************************************************/
OSPEVENT( EV_RAD_RAS_NOTIF,             EV_MTADP_BGN );
OSPEVENT( EV_RAD_NEWCALL_NOTIF,         EV_MTADP_BGN + 1 );
OSPEVENT( EV_RAD_NEWCHAN_NOTIF,         EV_MTADP_BGN + 2 );
OSPEVENT( EV_RAD_CALLCTRL_NOTIF,        EV_MTADP_BGN + 3 );
OSPEVENT( EV_RAD_CHANCTRL_NOTIF,        EV_MTADP_BGN + 4 );
OSPEVENT( EV_RAD_CONFCTRL_NOTIF,        EV_MTADP_BGN + 5 );
OSPEVENT( EV_RAD_FECCCTRL_NOTIF,        EV_MTADP_BGN + 6 ); 
OSPEVENT( EV_RAD_MMCUCTRL_NOTIF,        EV_MTADP_BGN + 7 ); 

/************************************************************************/
/* MTADP内部消息                                                        */
/************************************************************************/
OSPEVENT( WAIT_PASSWORD_TIMEOUT,		EV_MTADP_BGN + 9 ); //wait for password response
OSPEVENT( WAIT_RESPONSE_TIMEOUT,		EV_MTADP_BGN + 10 ); //wait for response of calling or message

OSPEVENT( TIMER_ROUNDTRIPDELAY_REQ,	    EV_MTADP_BGN + 11 );  //环路维护请求时钟
//OSPEVENT( WAIT_ROUNDTRIPDELAY_RSP_TIMEOUT,     EV_MTADP_BGN + 12 ); //link maintenance timeout
OSPEVENT( TIMER_REGGK_REQ,			    EV_MTADP_BGN + 13 ); //注册GK定时器
OSPEVENT( TIMER_POLL_WATCHDOG,          EV_MTADP_BGN + 14 ); 
OSPEVENT( TIMER_IRR,					EV_MTADP_BGN + 15 ); //IRR定时器
OSPEVENT( TIMER_VIDEOINFO,			    EV_MTADP_BGN + 16 ); //mcu级联时的请求视频信息定时器
OSPEVENT( TIMER_AUDIOINFO,              EV_MTADP_BGN + 17 ); //mcu级联时的请求音频信息定时器
OSPEVENT( TIMER_MTLIST,                 EV_MTADP_BGN + 18 ); //mcu级联时的请求终端列表定时器
OSPEVENT( CLEAR_INSTANCE,			    EV_MTADP_BGN + 19 ); //清除实例命令
OSPEVENT( TIMER_ROUNDTRIPDELAY_START,   EV_MTADP_BGN + 20 );
OSPEVENT( TIMER_GETMSSTATUS_REQ,        EV_MTADP_BGN + 21 ); //等待获取主备状态定时器
OSPEVENT( TIMER_H323_POLL_EVENT,        EV_MTADP_BGN + 22 );
OSPEVENT( TIMER_CONFCHARGE_STATUS_NTF,	EV_MTADP_BGN + 23 ); //主适配注册GK后, 会议计费状态激活定时器
OSPEVENT( CALLOUT_NEXT_NOTIFY,          EV_MTADP_BGN + 24 ); //分批呼叫策略呼叫下一个MT通知

//h320 MTAPD 内部消息
OSPEVENT( EV_H320STACK_STATE_NOTIF,     EV_MTADP_BGN + 25 ); //H320协议栈状态回调通知
OSPEVENT( EV_H320STACK_CMD_NOTIF,       EV_MTADP_BGN + 26 ); //h320协议栈cmd回调通知
OSPEVENT( EV_H320STACK_FECC_NOTIF,      EV_MTADP_BGN + 27 ); //h320协议栈远遥回调通知
OSPEVENT( EV_MTADP_SHOWSWITCH,          EV_MTADP_BGN + 28 ); //show switch info
OSPEVENT( EV_MTADP_SETSTACKLOG,         EV_MTADP_BGN + 29 ); //set stack log print level
OSPEVENT( EV_MTADP_VIDLOOPBACK,         EV_MTADP_BGN + 30 ); //video loop back no bch
OSPEVENT( EV_MTADP_BCHVIDLOOPBACK,      EV_MTADP_BGN + 31 ); //video loop back bch
OSPEVENT( TIMER_INVITEMT,               EV_MTADP_BGN + 32 ); //邀请终端超时定时器
OSPEVENT( TIMER_LSDTOKEN_RELEASE,       EV_MTADP_BGN + 33 ); //是否释放远遥令牌定时器

//H323 MTADP 内部消息(接上)
OSPEVENT( TIMER_URQ_RSP,                EV_MTADP_BGN + 34 ); //URQ发起后的响应等待定时
//RAS消息超时校验（防止GK备份的导致的状态等待，归于上层控制）
OSPEVENT( TIMER_RAS_RSP,                EV_MTADP_BGN + 35 );
// [11/21/2011 liuxu] 定时将下级的MtList发给McuVc
OSPEVENT( TIMER_SEND_SMCU_MTLIST,       EV_MTADP_BGN + 36 );

//H225信令交互完毕设置该TIMER，在Timer超时前H245呼叫信令应该开始交互(Mtadp收到MCU_MT_CAPBILITYSET_NOTIF信令).超时ClearInst.
//该TIMER主要解决场景：主组呼大量终端进行中，重启主，备此时还未接替主，导致H225信令交互完成后，备不会与Mtadp交互，完成
//H245建链的过程。导致终端未上线。而在Mtadp内部，视作该交互进行到H225完成，尚未完成所有交互。此时待备接替成主，再呼叫该终端
//Mtadp侧视作该终端已经呼出，只是未完成交互，不再向终端发消息，导致终端永远无法上线。[9/6/2012 chendaiwei]
//OSPEVENT( TIMER_H245_CAPBILITY_NOTIFY_OVERTIME, EV_MTADP_BGN + 37 );

// vrs支持，开启录像放像获取列表，都需要设Timer等待回复，若超时，则需挂断终端
OSPEVENT( TIMER_VRS_REC_RSP,       EV_MTADP_BGN + 38 );


/************************************************************************/
/*  私有消息（用于测试）                                                */
/************************************************************************/

OSPEVENT(MCU_MTADP_PRIVATE_MSG,			EV_MTADP_BGN + 40 ); 

#define EXIT_INSTANCE                   0  //body: null
#define ENABLE_OUTPUT_REDIRECTED        1  //body: u8, debug level
#define DISABLE_OUTPUT_REDIRECTED       2  //body: null
#define CLEAR_ALL				        3  //body: null


OSPEVENT( MTADP_MCU_PRIVATE_MSG,		EV_MTADP_BGN + 41 ); 

#define MTADP_OUTPUT                    10
#define CALL_STATISTICS_NOTIF           11  //u32[8]

//用于h320MtAdp测试的消息
OSPEVENT( MCU_H320MTADP_PRIVATE_MSG,    EV_MTADP_BGN + 42 );
OSPEVENT( H320MTADP_MCU_PRIVATE_MSG,    EV_MTADP_BGN + 43 );
OSPEVENT( TIMER_STARTAFTERSTOPSMIX,     EV_MTADP_BGN + 44 );
#define CHAN_SEND_STATE                 20 // 通道码流发送状态(body：u8)
#define CHAN_OPENCLOSE_STATE            21 // 通道是否关闭状态(body：u8)



/************************************************************************/
/* MCU卫星会议消息定义                                                  */
/************************************************************************/

OSPEVENT( MT_MCU_REG_REQ,               EV_MCUMT_SAT_BGN + 1);
OSPEVENT( MCU_MT_REG_ACK,               EV_MCUMT_SAT_BGN + 2);
OSPEVENT( MCU_MT_REG_NACK,              EV_MCUMT_SAT_BGN + 3);
OSPEVENT( MT_MCU_READY_NOTIF,           EV_MCUMT_SAT_BGN + 4);

OSPEVENT( MCU_MTCONNECTED_NOTIF,        EV_MCUMT_SAT_BGN + 6);

OSPEVENT( MT_MCU_APPLYJOINCONF_REQ,     EV_MCUMT_SAT_BGN + 8);
OSPEVENT( MCU_MT_APPLYJOINCONF_ACK,     EV_MCUMT_SAT_BGN + 9);
OSPEVENT( MCU_MT_APPLYJOINCONF_NACK,    EV_MCUMT_SAT_BGN + 10);

OSPEVENT( MCU_MT_MTTOPO_NOTIF,          EV_MCUMT_SAT_BGN + 12);

OSPEVENT( MCU_MTDISCONNECTED_NOTIF,     EV_MCUMT_SAT_BGN + 14);

#endif /* _EV_MCUMT_H_ */
