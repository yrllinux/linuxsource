#ifndef _EVMCUTEST_H_
#define _EVMCUTEST_H_

#include "osp.h"
#include "eventid.h"

/*
 *	集成测试消息
 */
//获取特定会议的信息,消息体CConfId+TConfFullInfo
OSPEVENT( TEST_MCU_GETCONFINFO_CMD,			    EV_TEST_MCU_BGN + 2 );
//获取MCU上所有会议列表,消息体void*( 缓冲区指针)+u32长度
OSPEVENT( TEST_MCU_GETCONFIDLIST_CMD,		    EV_TEST_MCU_BGN + 3 );
// 得到Mcu上当前会议数目,消息体 u32( 当前会议数目)
OSPEVENT( TEST_MCU_GETCONFCOUNT_CMD,		    EV_TEST_MCU_BGN + 4 );
// MCU设备重启
OSPEVENT( TEST_MCU_REBOOT_CMD,				    EV_TEST_MCU_BGN + 5 );
// 得到Mp交换信息列表
OSPEVENT( TEST_MCU_GETMPSWITCHINFO_CMD,		    EV_TEST_MCU_BGN + 6 );

//获取特定会议中相关外设状态,消息体CConfId+TEqp( VMP)+TEqp( MIX)+u8( 混音组)
//    +TEqp( BAS)+u8( BAS通道数)+TEqp( REC)+u8( 录像通道数)
//    +TEqp( PLAY)+u8( 放像通道数)
OSPEVENT( TEST_MCU_GETCONFEQP_CMD,			    EV_TEST_MCU_BGN + 11 );
//获取外设数据,消息体u8( 外设ID)+TPeriEqpData
OSPEVENT( TEST_MCU_GETEQPSTATUS_CMD,		    EV_TEST_MCU_BGN + 12 );
//获取外列表,消息体TEqp *( 外设列表指针)+u32( 缓冲长度)
OSPEVENT( TEST_MCU_GETMCUEQPLIST_CMD,		    EV_TEST_MCU_BGN + 13 );
//断开外设连接,消息体u8( 外设ID号)
OSPEVENT( TEST_MCU_DISCONNECTEQP_CMD,		    EV_TEST_MCU_BGN + 14 );
//得到外设的工作数据,消息体CConfId+u8 byEqpId+TVMPParam& tVMPParam
OSPEVENT( TEST_MCU_GETEQPVMPDATA_CMD,		    EV_TEST_MCU_BGN + 15 );
//得到外设的工作数据,消息体CConfId cConfId+u8 byEqpId+TDiscussParam& tDiscussParam
OSPEVENT( TEST_MCU_GETEQPMIXERDATA_CMD,		    EV_TEST_MCU_BGN + 16 );
//获取特定会议中特定终端数据,消息体CConfId cConfId+u8 byMtId+TMtData &tMtData
OSPEVENT( TEST_MCU_GETCONFSPECMTDATA_CMD,	    EV_TEST_MCU_BGN + 21 );
//获取特定会议中终端列表,消息体CConfId cConfId+TMtExt *ptMtList+u32 byBufSize
OSPEVENT( TEST_MCU_GETCONFMTLIST_CMD,		    EV_TEST_MCU_BGN + 22 );
//获取特定会议中交换信息列表,消息体CConfId cConfId+TConfSwitchTable &tConfSwitchTable
OSPEVENT( TEST_MCU_GETCONFMPSWITCHINFO_CMD,	    EV_TEST_MCU_BGN + 30 );
//获取特定会议中交换信息列表,消息体CConfId cConfId+TConfSwitchTable &tConfSwitchTable
OSPEVENT( TEST_MCU_GETCONFSWITCHINFO_CMD,	    EV_TEST_MCU_BGN + 31 );

// 得到双速功能会议信息,消息体CConfId, ack tTSpeedParam
OSPEVENT( TEST_MCU_GETCONFTSPEEDDATA_CMD,       EV_TEST_MCU_BGN + 32 );
// 得到会议加密信息,消息体CConfId, ack tEncryptParam
OSPEVENT( TEST_MCU_GETCONFENCRYPTDATA_CMD,      EV_TEST_MCU_BGN + 33 );
// 查询会议轮询状态,消息体CConfId, ack tPollParam
OSPEVENT( TEST_MCU_GETCONFPOLLDATA_CMD,		    EV_TEST_MCU_BGN + 34 );
// 得到会议讨论参数,消息体CConfId, ack tDiscussParam
OSPEVENT( TEST_MCU_GETCONFMIXERDATA_CMD,        EV_TEST_MCU_BGN + 35 );
// 得到会议的视频复合参数,消息体CConfId, ack tVMPParam
OSPEVENT( TEST_MCU_GETCONFVMPDATA_CMD,		    EV_TEST_MCU_BGN + 36 );

//获取所MP的列表,消息体TMp *ptMpList+u32 byBufSize
OSPEVENT( TEST_MCU_GETMPLIST_CMD,			    EV_TEST_MCU_BGN + 41 );
//获取当MCU的时间,消息体TKdvTime *ptKdvTime
OSPEVENT( TEST_MCU_GETMCUCURTIME_CMD,		    EV_TEST_MCU_BGN + 51 );
//结束所会议,消息体为空
OSPEVENT( TEST_MCU_RELEASEALLCONF_CMD,		    EV_TEST_MCU_BGN + 61 );
//得到级连MCU信息
OSPEVENT( TEST_MCU_GETCASCADEMCU_CMD,		    EV_TEST_MCU_BGN + 71 );
//得到所有终端信息
OSPEVENT( TEST_MCU_GETALLMTINFO_CMD,		    EV_TEST_MCU_BGN + 81 );
//得到MCU信息
OSPEVENT( TEST_MCU_GETMCUINFO_CMD,			    EV_TEST_MCU_BGN + 82 );
//得到所有MT( 主从MCU)信息
OSPEVENT( TEST_MCU_GETALLMCUMTINFO_CMD,		    EV_TEST_MCU_BGN + 83 );

OSPEVENT( TEST_MCU_TESTUSERINFO_CMD,		    EV_TEST_MCU_BGN + 84 );

// 获取会控监控信息
OSPEVENT(  TEST_MCU_GETMCCHANNEL_CMD,		    EV_TEST_MCU_BGN + 85 );




/*======================================================================
						单元测试消息定义( 53001--53501)
 ======================================================================*/

#define EV_UNITTEST_MCU_BGN					    EV_TEST_MCU_BGN + 500

/*---------------------------------------------------------------------------
 MCU测试消息（52501-53500）
----------------------------------------------------------------------------*/

OSPEVENT( EV_TEST_MT_CONNECT_REQ,				EV_UNITTEST_MCU_BGN + 1 );
OSPEVENT( EV_MT_TEST_CONNECT_ACK,				EV_UNITTEST_MCU_BGN + 2 );
OSPEVENT( EV_MT_TEST_CONNECT_NACK,				EV_UNITTEST_MCU_BGN + 3 );

//-----------      MCU集成数据会议测试接口(52511-52600)    ---------------

//得到某会议是否带数据功能召开, 消息体 byConfIdx
OSPEVENT( EV_TEST_CONF_ONGOING_INDCS_REQ,		EV_UNITTEST_MCU_BGN + 11 );
//某会议带数据功能召开应答
OSPEVENT( EV_TEST_CONF_ONGOING_INDCS_ACK,		EV_UNITTEST_MCU_BGN + 12 );
//某会议没有带数据功能召开应答
OSPEVENT( EV_TEST_CONF_ONGOING_INDCS_NACK,		EV_UNITTEST_MCU_BGN + 13 );

//得到某数据会议是否结束, 消息体 byConfIdx
OSPEVENT( EV_TEST_CONF_END_INDCS_REQ,			EV_UNITTEST_MCU_BGN + 14 );
//某数据会议结束应答
OSPEVENT( EV_TEST_CONF_END_INDCS_ACK,			EV_UNITTEST_MCU_BGN + 15 );
//某数据会议未结束应答
OSPEVENT( EV_TEST_CONF_END_INDCS_NACK,			EV_UNITTEST_MCU_BGN + 16 );

//得到某终端是否在数据会议中, 消息体TMt, 待确定终端
OSPEVENT( EV_TEST_MT_INDATACONF_REQ,			EV_UNITTEST_MCU_BGN + 17 );
//某终端在数据会议中应答
OSPEVENT( EV_TEST_MT_INDATACONF_ACK,			EV_UNITTEST_MCU_BGN + 18 );
//某终端不在数据会议中应答
OSPEVENT( EV_TEST_MT_INDATACONF_NACK,			EV_UNITTEST_MCU_BGN + 19 );

//得到某终端是是否离数据会议, 消息体TMt, 待确定终端
OSPEVENT( EV_TEST_MT_OFFDATACONF_REQ,			EV_UNITTEST_MCU_BGN + 20 );
//某终端离数据会议应答
OSPEVENT( EV_TEST_MT_OFFDATACONF_ACK,			EV_UNITTEST_MCU_BGN + 21 );
//某终端未离数据会议应答
OSPEVENT( EV_TEST_MT_OFFDATACONF_NACK,			EV_UNITTEST_MCU_BGN + 22 );

//----------      MCU集成数据会议测试接口完毕(52511-52600)    --------------

//-------------      MCU主控热备份测试接口(52601-52700)    -----------------

//得到会议的模板信息, 消息体 CConfId
OSPEVENT( EV_TEST_TEMPLATEINFO_GET_REQ,			EV_UNITTEST_MCU_BGN + 101 );
//得到会议的模板信息成功应答, 消息体 byConfIdx + TConfInfo
OSPEVENT( EV_TEST_TEMPLATEINFO_GET_ACK,			EV_UNITTEST_MCU_BGN + 102 );
//得到会议的模板信息失败应答, 消息体 byConfIdx
OSPEVENT( EV_TEST_TEMPLATEINFO_GET_NACK,		EV_UNITTEST_MCU_BGN + 103 );

//得到会议的会议信息, 消息体 CConfId
OSPEVENT( EV_TEST_CONFINFO_GET_REQ,				EV_UNITTEST_MCU_BGN + 104 );
//得到会议的会议信息成功应答, 消息体 byConfIdx + TConfInfo
OSPEVENT( EV_TEST_CONFINFO_GET_ACK,				EV_UNITTEST_MCU_BGN + 105 );
//得到会议的会议信息失败应答, 消息体 byConfIdx
OSPEVENT( EV_TEST_CONFINFO_GET_NACK,			EV_UNITTEST_MCU_BGN + 106 );

//得到会议的终端列表信息, 消息体 CConfId
OSPEVENT( EV_TEST_CONFMTLIST_GET_REQ,			EV_UNITTEST_MCU_BGN + 107 );
//得到会议的终端列表信息成功应答, 消息体 byConfIdx + TMt[]
OSPEVENT( EV_TEST_CONFMTLIST_GET_ACK,			EV_UNITTEST_MCU_BGN + 108 );
//得到会议的终端列表信息失败应答, 消息体 byConfIdx
OSPEVENT( EV_TEST_CONFMTLIST_GET_NACK,			EV_UNITTEST_MCU_BGN + 109 );

//得到会议的地址薄信息, 消息体 CConfId
OSPEVENT( EV_TEST_ADDRBOOK_GET_REQ,				EV_UNITTEST_MCU_BGN + 110 );
//得到会议的地址薄信息成功应答, 消息体 byConfIdx +
OSPEVENT( EV_TEST_ADDRBOOK_GET_ACK,				EV_UNITTEST_MCU_BGN + 111 );
//得到会议的地址薄信息失败应答, 消息体 byConfIdx
OSPEVENT( EV_TEST_ADDRBOOK_GET_NACK,			EV_UNITTEST_MCU_BGN + 112 );

//得到MCU的配置信息, 消息体 CConfId
OSPEVENT( EV_TEST_MCUCONFIG_GET_REQ,			EV_UNITTEST_MCU_BGN + 113 );
//得到MCU的配置信息成功应答, 消息体 byConfIdx +
OSPEVENT( EV_TEST_MCUCONFIG_GET_ACK,			EV_UNITTEST_MCU_BGN + 114 );
//得到MCU的配置信息失败应答, 消息体 byConfIdx
OSPEVENT( EV_TEST_MCUCONFIG_GET_NACK,			EV_UNITTEST_MCU_BGN + 115 );

//------------      MCU主控热备份测试接口完毕(52601-52700)    --------------

#endif  //!_EVMCUTEST_H_
