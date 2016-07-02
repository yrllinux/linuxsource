/*=============================================================================
模   块   名: 会议控制台业务功能库
文   件   名: mcsconst.h
相 关  文 件: 无
文件实现功能: mcslib中常量定义
作        者: 王昊
版        本: V4.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-------------------------------------------------------------------------------
修改记录:
日      期  版本    修改人      修改内容
2005/04/26  4.0     王昊        创建
=============================================================================*/

#ifndef _MCSCONST_20050426_H_
#define _MCSCONST_20050426_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "errorid.h"
#include "eventid.h"

#define AUTO_INC_VERSION		"4,4,3,2688"

//  录像文件
#define MAX_FILE_NUM                    (s32)1000      //支持最大文件数

#define MCSLIB_OK                       (u16)0//调用成功
#define MCSLIB_BASE_ERROR               (u16)ERR_MC_BGN//错误码基准值
#define MCSLIB_ERROR_OSP_ALREADY_INIT   (u16)(ERR_MC_BGN + 1)//Osp 已经初始化
#define MCSLIB_ERROR_OSP_INIT           (u16)(ERR_MC_BGN + 2)//osp 初始化失败
#define MCSLIB_ERROR_OSP_NO_INIT        (u16)(ERR_MC_BGN + 3)//osp 没有初始化
#define MCSLIB_ERROR_CREATE_APP		    (u16)(ERR_MC_BGN + 4)//创建应用失败  
#define MCSLIB_NO_MEMORY                (u16)(ERR_MC_BGN + 5)//分配内存出错
#define MCSLIB_INVALID_WINDOW           (u16)(ERR_MC_BGN + 6)//无效的窗口句柄
#define MCSLIB_NOT_IN_IDLE_STATE        (u16)(ERR_MC_BGN + 7)//会控已经连接上MCU
#define MCSLIB_NOT_REGISTER_WINDOW      (u16)(ERR_MC_BGN + 8)//会控没有注册窗口
#define MCSLIB_MUST_WAIT                (u16)(ERR_MC_BGN + 9)//会控正在处理前一请求
#define MCSLIB_INVALID_PARAM            (u16)(ERR_MC_BGN + 10)//无效的参数
#define MCSLIB_FAIL_CONNECT_MCU         (u16)(ERR_MC_BGN + 11)//连接MCU失败
#define MCSLIB_FAIL_POSTMSG_TO_MCU      (u16)(ERR_MC_BGN + 12)//向MCU发送消息失败
#define MCSLIB_FAIL_ALREADY_CREATE      (u16)(ERR_MC_BGN + 13)//对象已经创建
#define MCSLIB_FAIL_REG_SESSION		    (u16)(ERR_MC_BGN + 14)//注册MCU对象失败
#define MCSLIB_FAIL_UNREG_SESSION       (u16)(ERR_MC_BGN + 15)//注销MCU对象失败
#define MCSLIB_NOT_IN_NORMAL_STATE      (u16)(ERR_MC_BGN + 16)//会控还没连接上MCU
#define MCSLIB_MAXNUM_MCU               (u16)(ERR_MC_BGN + 17)//已连接最大数目MCU

//  Mt子类型, 本级MCU
#define MT_TYPE_LOCALMCU    (u8)90

//  索引号错误
#define INDEX_ERR           -1

//MCU等待回复(秒为单位)
#define DELAY_TIME_DEFAULT      (u16)6

// CP936即GBK编码 by zjl
#define CP936			936

//  会议级联类型
typedef enum enumCascadeType
{
    emSingleConf        = 0,    //  非级联会议
    emTwoLayerMMcuConf  = 1,    //  两级级联, 本级MCU作上级
    emTwoLayerSMcuConf  = 2,    //  两级级联, 本级MCU作下级
    emThreeLayerConf    = 3     //  三级级联会议
} EMCascadeType;

//  连接MCU会议数据初始化
typedef enum enumConfInitValue
{
    emLockInfo          = 0,        //  本级MCU锁定信息
    emAllMtStatus       = 1,        //  所有终端状态, 包含其它级MCU
    emAllMtAlias        = 2,        //  所有终端别名, 包含其它级MCU
    emAllMcuMediaSrc    = 3,        //  所有下级MCU媒体源
    emAllSMcuLockStatus = 4,        //  所有下级MCU锁定状态
    emAllSMcuMixParam   = 5,        //  所有下级MCU混音参数
    emAllMtVideoSrcAlias= 6,         //  所有终端视频源别名获取
    emAllInfo           = 7,        //  所有信息

} EMConfInitValue;

//  语言种类
typedef enum enumLangID
{
    emEnglish               = 0,        //  英语
    emChinese               = 1,        //  中文
    emOther                 = 3         //  其他(默认英文)
} EMLangID;

//  实例号
#define INSID_MCSLIB            (u16)1      //  mcslib起始实例号
#define INSID_MCSLIB_TESTER     (u16)1      //  mcslib单元测试tester实例号
#define INSID_MCSLIB_MCU        (u16)1      //  mcslib单元测试模拟MCU实例号
#define INSID_ADDRBOOK          (u16)1      //  地址簿
#define INSID_UM                (u16)1      //  用户管理

//  mcslib 状态
#define MCSLIB_STATE_IDLE       (u16)100
#define MCSLIB_STATE_NORMAL     (u16)101

//  MCS等待MCU回复时间，OSP定时(毫秒为单位)
#define DEFAULT_TIMEOUT         (u32)6000
//  MCU等待回复(秒为单位)
#define DELAY_TIME_DEFAULT      (u16)6
//  连接备用MCU超时时间(1秒)
#define CONNECT_BAKMCU_TIMEOUT  (s32)1000
//  查询MCU上CPU使用率超时时间(60秒)
#define MCU_REFRESHMCU_TIMEOUT  (u32)60 * 1000

//  传给上层的最大buffer大小, 32K
#define BUF_SIZE_TO_UI          (s32)(32 * 1024)

//  会议控制台连接MCU最大数
#define MAXNUM_MCS_MCU          (s32)16

//  查询起始端口
#define QUERY_PORT              (u16)6682

//  mcu名称最大长度
#define MAXLEN_MCU_NAME         (s32)64

//  终端列表递增步长
#define MTTABLE_STEP            (s32)32

//  消息栈最大长度
#define QUEUE_SIZE              (u16)5000

// 每个session占的端口数
#define MCS_SESSION_PORT_NUM    ((u16)(PORTSPAN*11))     
// 起始侦听端口
#define MCS_MONITOR_BASE_PORT	(u16)VCS_MONITOR_END_PORT	//从VCS结束端口之后找
// 结束侦听端口
#define MCS_MONITOR_END_PORT    ((u16)(MCS_MONITOR_BASE_PORT + (MAXNUM_MCS_MCU - 1)* MCS_SESSION_PORT_NUM ))

//VCS占用端口数
#define VCS_SESSION_PORT_NUM	((u16)(PORTSPAN*11)) 
//VCS起始端口
#define VCS_MONITOR_BASE_PORT	(u16)7200
//VCS结束端口
#define VCS_MONITOR_END_PORT    ((u16)(VCS_MONITOR_BASE_PORT + VCS_SESSION_PORT_NUM))

//  矩阵方案最大个数
#define MAXNUM_MATRIXSCHE       (s32)8

//  一块单板上运行的最大外设模块数
#define MAXNUM_BRDEQP           (s32)4

// H264 专用分辨率,在界面显示用
#define VIDEO_FORMAT_NULL                           255
// H264传到MCU替换的分辨率
#define VIDEO_FORMAT_H264REPL                       VIDEO_FORMAT_CIF

//录像文件名最大长度(包含路径以及'\0')
#define MAXLEN_REC_FILE_NAME                        256

//终端别名最大长度
#define MCS_MAXLEN_ALIAS                                16

//Vcs终端别名最大长度
#define VCS_MAXLEN_ALIASEX                              80

//终端别名最大长度
#define MTALIAS_MAXLEN_EX                              80

//集成测试
#define INTTEST_MAXNUM_ADDRENTRY    (u32)100    //每次发送地址簿条目最大数
#define INTTEST_MAXNUM_ADDRGROUP    (u32)40     //每次发送地址簿会议组最大数
#define INTTEST_MAXNUM_MT           (u32)60     //每次发送终端最大个数
#define INTTEST_MAXNUM_PERI         (u32)30     //每次发送外设最大个数
#define INTTEST_MAXNUM_BRD          (u32)14     //每次发送单板配置最大个数

#define FIRSTTIMESPAN               (u16)40      //第一个重传检测点
#define SECONDTIMESPAN              (u16)120     //第二个重传检测点
#define THIRDTIMESPAN               (u16)240     //第三个重传检测点
#define REJECTTIMESPAN              (u16)480     //过期丢弃的时间跨度
typedef enum enumTimerWaitState
{
    //  开始
    emTimerWaitNormal = 0,              //从0开始可作索引
    //  连接MCU
    emTimerWaitConnectMcu,              //连接MCU
    emTimerWaitSyncTime,                //时间同步
    emTimerWaitGetMcuStatus,            //获取MCU状态
    emTimerWaitGetMcuEqpCfg,            //获取MCU外设配置
    emTimerWaitGetMcuBrdCfg,            //获取MCU单板配置
    emTimerWaitGetMcuGeneralCfg,        //获取MCU基本配置
    emTimerWaitAddrReg,                 //注册地址簿
    emTimerWaitAddrGetEntryNum,         //获取地址簿条目总数
    emTimerWaitAddrGetEntry,            //获取地址簿条目
    emTimerWaitAddrGetGroupNum,         //获取地址簿会议组总数
    emTimerWaitAddrGetGroup,            //获取地址簿会议组
	// lrf [6/22/2006]
	emTimerWaitGetUserGroupList,		//获取用户组列表
    emTimerWaitGetUserList,             //获取用户列表
	emTimerWaitListAllConf,             //获取所有会议列表
    emTimerWaitGetPeriEqpStatus,        //获取外设状态
	emTimerWaitGetDcsStatus,			//获取DCS状态
    emTimerWaitGetLockInfo,             //获取会议锁定信息
    emTimerWaitGetAllMtStatus,          //获取所有终端状态
    emTimerWaitGetAllMtAlias,           //获取所有终端别名
    emTimerWaitGetAllMcuMediaSrc,       //获取所有MCU媒体源
    emTimerWaitGetAllSMcuLockStatus,    //获取所有下级MCU锁定情况
    emTimerWaitGetAllSMcuMixParam,      //获取所有下级MCU混音参数
    emTimerWaitGetVmpStyle,             //获取画面合成方案
    //  MCU操作
    emTimerWaitGetMcuMsStatus,          //获取MUC的主备状态
    emTimerWaitGetMcuCpuStatus,         //获取MCU的CPU使用率
    emTimerWaitModifyMcuEqpCfg,         //修改MCU外设配置
    emTimerWaitModifyMcuBrdCfg,         //修改MCU单板配置
    emTimerWaitChangeHDUSchemeInfo,     //xts20090120 修改电视墙预案配置
    emTimerWaitModifyMcuGeneralCfg,     //修改MCU基本配置
    emTimerWaitGetMcuTime,              //获取MCU系统时间
    emTimerWaitModifyMcuTime,           //修改MCU系统时间
    emTimerWaitRebootBrd,               //重起单板
    //  会议管理
    emTimerWaitCreateConf,              //创建会议
    emTimerWaitCreateConfByTemplate,    //根据模板创建会议
    emTimerWaitReleaseConf,             //结束会议
    emTimerWaitChangeConfLockMode,      //修改会议锁定模式
    emTimerWaitChangeConfPwd,           //修改会议密码
    emTimerWaitSaveConf,                //保存会议
    emTimerWaitModifyConf,              //修改会议
    emTimerWaitDelayConf,               //会议延时
    //  会议操作
    emTimerWaitSpecChairman,            //指定主席
    emTimerWaitCancelChairman,          //取消主席
    emTimerWaitSpecSpeaker,             //指定发言人
    emTimerWaitCancelSpeaker,           //取消发言人
    emTimerWaitAddMt,                   //添加终端
    emTimerWaitDelMt,                   //删除终端
    emTimerWaitStartMonitor,            //开始监控
    emTimerWaitStopMonitor,             //停止监控
    //  会议控制
    emTimerWaitGetPollParam,            //获取会议轮询参数
    emTimerWaitSpecPollPos,                 //指定当前轮询的终端
    emTimerWaitGetTwPollParam,          //获取电视墙轮询参数
    emTimerWaitChangeVacHoldTime,       //修改语音激励时间
    //  终端操作
    emTimerWaitCallMt,                  //呼叫终端
    emTimerWaitSetMtCallMode,           //设置终端呼叫方式
    emTimerWaitDropMt,                  //挂断终端
    emTimerWaitStartSee,                //终端选看
    emTimerWaitStopSee,                 //停止选看
    emTimerWaitStartVmpSee,             //vmp选看
    emTimerWaitStopVmpSee,              //停止vmp选看
    emTimerWaitGetMtAlias,              //获取终端别名
    emTimerWaitGetMtBitrate,            //获取终端码率
    emTimerWaitAudMute,                 //静音哑音
    emTimerWaitGetMatrix,               //获取终端矩阵
    emTimerWaitGetMatrixScheme,         //获取矩阵方案
    emTimerWaitGetMatrixExPort,         //获取外置矩阵连接端口
    emTimerWaitLockSMcu,                //锁定下级MCU
    emTimerWaitGetMtVersion,            //获取终端版本信息
    //  录像
    emTimerWaitStartRec,                //开始录像
    emTimerWaitPauseRec,                //暂停录像
    emTimerWaitResumeRec,               //恢复录像
    emTimerWaitStopRec,                 //停止录像
    emTimerWaitStartPlay,               //开始放像
    emTimerWaitPausePlay,               //暂停放像
    emTimerWaitResumePlay,              //恢复放像
    emTimerWaitStopPlay,                //停止放像
    emTimerWaitSeekPlay,                //放像进度调节
//    emTimerWaitGetAllRecFile,           //获取录像文件
    emTimerWaitDeleteRecFile,           //删除录像文件
    emTimerWaitChangeRecFileName,       //修改录像文件名
    emTimerWaitPublishRecFile,          //发布录像文件
    emTimerWaitCancelPub,               //取消录像文件发布
    emTimerWaitModifyRecFile,           //修改录像文件名
    //  混音
    emTimerWaitStartMix,                //开始混音
    emTimerWaitStopMix,                 //停止混音
    emTimerWaitStartVac,                //开始语音激励
    emTimerWaitStopVac,                 //停止语音激励
    emTimerWaitMixDelay,                //混音延时
    //  画面合成
    emTimerWaitStartVmp,                //开始画面合成
    emTimerWaitStopVmp,                 //停止画面合成
    emTimerWaitChangeVmpParam,          //修改画面合成参数
    emTimerWaitStartVmpBrdst,           //开始画面合成广播
    emTimerWaitStopVmpBrdst,            //停止画面合成广播
    emTimerWaitPollParamVmp,			//获取画面合成轮询参数
    //  电视墙
    emTimerWaitStartTW,                 //开始电视墙
    emTimerWaitStopTW,                  //停止电视墙
    //  多画面电视墙
    emTimerWaitStartMultiTW,            //开始多画面电视墙
    emTimerWaitChangeMultiTWParam,      //修改多画面电视墙参数
    emTimerWaitStopMultiTW,             //停止多画面电视墙
    //  用户
    emTimerWaitAddUser,                 //添加用户
    emTimerWaitDelUser,                 //删除用户
    emTimerWaitModifyUser,              //修改用户
    //  地址簿
    emTimerWaitAddrUnreg,               //注销地址簿
    emTimerWaitAddrAddEntry,            //添加地址簿条目
    emTimerWaitAddrDelEntry,            //删除地址簿条目
    emTimerWaitAddrModifyEntry,         //修改地址簿条目
    emTimerWaitAddrAddGroup,            //增加地址簿会议组
    emTimerWaitAddrDelGroup,            //删除地址簿会议组
    emTimerWaitAddrModifyGroup,         //修改地址簿会议组
    emTimerWaitAddrGroupAddEntry,       //向地址簿会议组添加条目
    emTimerWaitAddrGroupDelEntry,       //从地址簿会议组删除条目
	//ext
	emTimerWaitDownLoadCfg,				//下载配置文件
    emTimerWaitUpLoadCfg,				//上传配置文件
	emTimerUpgradeComplete,				// ftp上传完成
	//用户组操作
	emTimerWaitAddUserGroup,			//增加用户组
	emTimerWaitDelUserGroup,			//删除用户组
	emTimerWaitModifyUserGroup,			//修改用户组
    //指定本会场向上级回传的终端
    emTimerWaitSpecOutView,
    // 用OSP方式上传文件
    emTimerWaitUploadFileReq,           // 上传文件请求
    emTimerWaitUploadFilePackageReq,    // 上传包请求
    emTimerWaitUpdateTelnetLoginInfoReq,// 更新Telnet密码
    emTimerWaitStartRollCallReq,        // 开始点名
    emTimerWaitStopRollCallReq,         // 结束点名
    emTimerWaitChangeRollCallReq,       // 修改点名人员
    //高清电视墙
    emTimerWaitGetGetHduSchemeInfo,     //获取高清电视墙方案
    emTimerWaitSetChannelVolumeInfo,    //高清电视墙通道音量调节
    emTimerWaitStartHduBatchPollReq,    //开始高清电视墙通道批量轮询
    emTimerWaitStopHduBatchPollReq,     //停止高清电视墙通道批量轮询
    emTimerWaitPauseHduBatchPollReq,    //暂停高清电视墙通道批量轮询
    emTimerWaitResumeHduBatchPollReq,   //恢复高清电视墙通道批量轮询
    //调度相关
    emTimerWaitVcsChangeModeReq,        // 修改调度模式
    emTimerWaitVcsSchedulingMtReq,      // 调度终端
    emTimerWaitVcsEnterConfReq,         // 进入调度会议
    emTimerWaitVcsQuitConfReq,          // 离开调度会议
    emTimerWaitVcsSoundControlReq,      // 声音控制
    emTimerWaitVcsGetVCSNameReq,        // 获取VCS名称
    emTimerWaitVcsChangeVCSNameReq,     // 修改VCS名称
    emTimerWaitVcsCallGroupReq,         // 进入组呼模式
    emTimerWaitVcsDropCallGroupMtReq,   // 挂断组呼模式
    emTimerWaitVcsStartChairPollReq,    // 开启本地轮询
    emTimerWaitVcsStopChairPollReq,     // 停止本地轮询
    emTimerWaitVcsAddMtReq,             // 添加临时终端
    emTimerWaitVcsDeleteMtReq,          // 删除终端
    emTimerWaitVcsUserSettingReq,       //用户设置
	
	//预案相关
	emTimerWaitAddVcsSchemaNameReq,     //增加新预案
	emTimerWaitDeleteVcsSchemaNameReq,
	emTimerWaitModifyVcsSchemaNameReq,
	emTimerWaitGetALLSchemaDataReq,
	emTimerWaitLockOneSchemaReq,		//锁住一个预案

	//电视墙
	emTimerWaitVcsSetMtinTvWallReq,		//指定终端进电视墙
	emTimerWaitVcsChgTvWallModeReq,

	//取消本会场向上级回传的终端
    emTimerCancelCascadeSpyReq,

	//分包发送组信息
	emTimerWaitVcsGrpInfoPktReq,
    //切换抢答模式
    emTimerWaitVcsChgSpeakerModetReq,

    //切换双流状态
    emTimerWaitVcsChgDualStatusReq,
	emTimerWaitVcsStartMonitorUnionReq,
	emTimerWaitVcsStopMonitorUnionReq,

    //终端视频源别名
    emTimerWaitMtVideoSrcReq,

	emTimerLockInfoReq,
	//MCS预案
	emTimerWaitSchema,
	emTimerStartCaptionReq,
	emTimerStopCaptionReq,
    //  结束
    emTimerWaitEnd
} EMTimerWaitState;

// 监控联动所需APP和端口 by 牟兴茂
#define MONITOR_UNION_CLIENT_PORT	4830
#define MONITOR_UNION_CLIENT_APP_ID AID_NMC_BGN + 1
#define MONITOR_UNION_SERVER_PORT	4831
#define MONITOR_UNION_SERVER_APP_ID AID_NMC_BGN + 2

// 内存管理器页面个数
#define MEMORY_MANAGER_PAGE_COUNT		u32(2000)
// 内存管理器页面大小
#define MEMORY_MANAGER_PAGE_SIZE		u32(4*1024)

// 解码器数量
#define DECODER_NBR				4
// 解码器起始端口
#define MONITOR_UNION_DECODER_BASE_PORT				4832
// 解码器窗口间隔
#define WND_SPACE_X				2
// 消息起始
#define begin_MonitorUnionMsg	emTimerWaitEnd

// 枚举消息辅助宏
#define _ev_segment(val) ev_##val##_begin = begin_##val,
#define _event(val) val,
#define _body(val1 , val2)
#define _desc(val)
#define _ev_end
#define _ev_segment_end(val) ev_##val##_end 


enum EmMonitorUnionMsg
{
	_ev_segment( MonitorUnionMsg )
		
		_event(ev_V2M_CONNECT_REQ)
		_desc("连接请求. VCS->Mon")
		_body( 0, 0 )	
		_ev_end
		
		_event(ev_M2V_CONNECT_ACK)
		_desc("消息已处理. Mon->VCS")
		_body( 0, 0 )
		_ev_end
		
		_event(ev_M2V_CONNECT_NACK)
		_desc("消息未处理. Mon->VCS")
		_body( 0, 0 )
		_ev_end
		
		_event(ev_V2M_STOPMONITOR_REQ)
		_desc("停止监控. VCS->Mon")
		_body( 0, 0 )
		_ev_end
		
		_event(ev_M2V_STOPMONITOR_ACK)
		_desc("-")
		_body( 0, 0 )
		_ev_end
		
		_event(ev_M2V_STOPMONITOR_NACK)
		_desc("-")
		_body( 0, 0 )
		_ev_end
		
		_event(ev_V2M_STARTMONITOR_REQ)
		_desc("开始监控. VCS->Mon")
		_body( 0, 0 )
		_ev_end
		
		_event(ev_M2V_STARTMONITOR_ACK)
		_desc("-")
		_body( 0, 0 )
		_ev_end
		
		_event(ev_M2V_STARTMONITOR_NACK)
		_desc("-")
		_body( 0, 0 )
		_ev_end
		_event(ev_M2V_MONITOR_LIST_NOTIF)
		_desc("列表通知. mon->vcs")
		_body( TMt, n )
		_ev_end

		_event(ev_REPORT_REQ)
		_desc("发送数据给所连接的客户端. deamon->mon")
		_body( pMsg->content, 1 )
		_ev_end
		
		_event(ev_TIME_OUT)
		_desc("超时. instance自身")
		_body( pMsg->content, 1 )
		_ev_end

		_event(ev_VCS_MONITOR_UNION_OP_TIME_OUT)
		_desc("超时. vcs->vcs")
		_body( pMsg->content, 1 )
		_ev_end

		_event(ev_M2V_GET_IFRMAE_CMD)
		_desc("监控联动经VCS请求关键帧. Mon->VCS")
		_body( tMt, 1 )
		_body( TYPEMODE, 1 )
		_ev_end

		_ev_segment_end(MonitorUnionMsg)
};

#endif  //  _MCSCONST_20050426_H_
