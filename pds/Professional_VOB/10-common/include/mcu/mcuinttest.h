/*****************************************************************************
   模块名      : mcu
   文件名      : mcuinttest.h
   相关文件    : 
   文件实现功能: MCU集成测试接口说明
   作者        : 胡昌威
   版本        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2004/01/31  3.0         胡昌威       创建
   2005/01/25  3.6         万春雷       接口完善
******************************************************************************/

#ifndef __MCUINTTEST_H_
#define __MCUINTTEST_H_

#include "osp.h"
#include "mcuconst.h"
#include "mcustruct.h"
#include "mcuutility.h"
#include "mcudata.h"

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif


class CMcuIntTester/**/
{	
public:
    CMcuIntTester();
    virtual ~CMcuIntTester();

public:
	
	// 建立Tester与指定MCU间的连接
	virtual BOOL ConnectMcu( u32 dwMcuIp, // Mcu Ip地址
					         u16 wPort    // Mcu 侦听端口
					       );

	// 断开Tester与MCU间的连接
	virtual BOOL DisconnectMcu();

// 2.2.1  会议部分 --------------------------
	
	// 2.2.1.1  获取MCU时间
	virtual BOOL GetMcuTime( TKdvTime* ptKdvTime );

	// 2.2.1.2  得到Mcu上当前会议数目
	virtual BOOL GetConfCount( OUT  u32 &dwConfCount );

	// 2.2.1.3  得到MCU上的会议号列表
	virtual BOOL GetConfIdList( CConfId* pcConfIdList,// 会议号列表存储空间
					            u32 dwBufSize     // 存储空间大小(应该为sizeof(CConfId)的倍数,最大倍数MAXNUM_MCU_CONF)
					          );

	// 2.2.1.4  得到会议信息
	virtual BOOL GetConfInfo( CConfId cConfId,    // 会议号
					          TConfInfo &tConfInfo// 会议信息
					        );

	// 2.2.1.5  得到会议终端列表
	virtual BOOL GetConfMtList( CConfId cConfId,  // 会议号
						        TMtExt* ptMtList, // 终端列表存储空间
						        u32 dwBufSize     // 存储空间大小(应该为sizeof(TMtExt)的倍数,最大倍数2*MAXNUM_CONF_MT)
					          );

	// 2.2.1.6  得到会议终端数据
	virtual BOOL GetConfMtData( CConfId cConfId,  // 会议号
						        u8 byMtId,        // 终端号
						        TMtData &tMtData  // 终端数据
					          );
				
	// 2.2.1.7  结束所有会议
	virtual BOOL ReleaseAllConf();

	// 2.2.1.8  设备重启
	virtual BOOL RebootMCU();
	
	// 2.2.1.9  得到MPC、CRI、DRI等各板的CPU的使用情况
	virtual BOOL GetCPUUsageOfVeneer();

	// 2.2.1.10	得到双速功能会议信息
	virtual BOOL GetConfInfoOfTwoSpeed( CConfId cConfId, TConfInfo &tConfInfo );

	// 2.2.1.11	得到会议加密信息
	virtual BOOL GetConfEncryptInfo( CConfId cConfId, TMediaEncrypt &tMediaEncrypt );

	// 2.2.1.12	获得会议交换信息
	virtual BOOL GetConfSwitchInfo( CConfId cConfId,            // 会议号
							        u8 byMpId,                  // MpId  [1, MAXNUM_DRI]
							        TSwitchTable &tSwitchTable  // 交换表
							      );

    virtual BOOL GetConfSwitchInfo( CConfId cConfId,            // 会议号
							        TConfSwitchTable &tConfSwitchTable  // 交换表
							      );

	// 2.2.1.13	查询会议轮询状态
	virtual BOOL GetConfPollInfo( CConfId cConfId,              // 会议号
		                          TPollInfo &tPollInfo,         // 当前轮询参数,仅轮询时有效
								  TConfPollParam &tConfPollParam// 轮询终端列表
								);

	// 2.2.1.14	得到会议讨论参数
	virtual BOOL GetConfDiscussInfo( CConfId cConfId, TDiscussParam &tDiscussParam );

	// 2.2.1.15	得到会议的视频复合参数
	virtual BOOL GetConfVMPInfo( CConfId cConfId, TVMPParam &tVMPParam );

	// 2.2.1.16	得到Mcu基本信息 （32个字节别名＋32字节E164）
	virtual BOOL GetMcuAlias( s8* pszInfoBuf, u8 byBufLen );

	// 2.2.1.17	获得简单级连会议信息
//	virtual BOOL GetConfInfoOfSCascade( CConfId cConfId, 
//		                                TConfCascadeInfo* ptConfCascadeInfo );

	// 2.2.1.18	获得简单级连会议终端信息
	virtual BOOL GetConfAllMtInfOfSCascadeo( CConfId cConfId, 
		                                     TConfAllMtInfo* ptConfAllMtInfo);
	
	// 2.2.1.19	获得合并级连会议信息
//	virtual BOOL GetConfInfoOfUCascade( CConfId cConfId, 
//		                                TConfCascadeInfo* ptConfCascadeInfo );

	// 2.2.1.20	获得合并级连会议终端信息
	virtual BOOL GetConfAllMtInfoOfUCascade( CConfId cConfId, 
		                                     TConfAllMtInfo* ptConfAllMtInfo);

	// 2.2.1.21	得到级连Mcu列表
//	virtual BOOL GetConfCascadeMcuList( CConfId cConfId, 
//		                                TConfCascadeInfo* ptConfCascadeInfo );

// 2.2.2  外设部分 --------------------------

	// 2.2.2.1  得到会议设备信息
	virtual BOOL GetConfEqp( CConfId cConfId,    // 会议号
					         TEqp& tVmpEqp,      // 画面合成外设
					         TEqp& tMixEqp,		 // 混音外设
					         u8&   byMixGrpId,   // 混音组ID
					         TEqp& tAdaptEqp,	 // 适配外设
					         u8&   byBasChnnl,   // 适配器通道号
					         TEqp& tRecEqp,  	 // 录像外设
					         u8&   byRecChnnl,	 // 录像信道号
					         TEqp& tPlayEqp,	 // 放像外设
					         u8&   byPlayChnnl,	 // 放像信道号
					         TEqp& tPrsEqp,		 // 重传设备
					         u8&   byPrsChnnl	 // 重传通道
					       );

	// 2.2.2.2  获得Mcu上所有外设信息（已在MCU上注册的）
	virtual BOOL GetMcuEqpList( TEqp* ptEqpList, // 设备列表存储空间
						        u32 dwBufSize    // 存储空间大小 sizeof(TEqp)*MAXNUM_MCU_PERIEQP 
						      );

	// 2.2.2.3  得到指定设备状态
	BOOL GetEqpStatus( u8 byEqpId,                                 // 设备号
					   TPeriEqpData &tPeriEqpData                  // 设备状态
					 );

	// 2.2.2.4  得到指定画面合成器参数
	virtual BOOL GetCurrEqpVmpData( CConfId cConfId,               // 会议号
						            u8 byEqpId,                    // 设备号
					                TVMPParam &tVMPParam           // 设备状态
					              );
	
	// 2.2.2.5  得到指定混音器工作参数
	virtual BOOL GetCurrEqpMixerData( CConfId cConfId,             // 会议号
						              u8 byEqpId,                  // 设备号
					                  TDiscussParam &tDiscussParam // 设备状态
					                );

	// 2.2.2.6  得到Mp信息表
	virtual BOOL GetMpList( TMpData* ptMpList,   // Mp列表存储空间
					        u32 dwBufSize    // 存储空间大小(应该为sizeof(TMpData)的倍数,最大倍数MAXNUM_DRI)
					      );

	// 2.2.2.7  得到Mp交换信息列表
	virtual BOOL GetMPSwitchInfo( u8 byMpId,                       // MpId  [1, MAXNUM_DRI]
							      TSwitchTable &tSwitchTable       // 交换表
							    );

	//断开外设的连接
	virtual BOOL DisconnectEqp(u8 byEqpId);

	
	//所有会议终端
	virtual BOOL GetConfAllMtInfo( CConfId cConfId, TConfAllMtInfo* ptConfAllMtInfo);

	//其它MCU终端信息
//	virtual BOOL GetOtherMcData( u8 byMcuId, TConfOtherMcInfo &tConfOtherMcInfo );
	
	virtual u32 GetMcuNode();

	virtual BOOL IsUserExist(LPCSTR pszUserName );

private:
	u32  m_dwMcuAppId;
	u32  m_dwMcuNode;
	
}

#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
#endif