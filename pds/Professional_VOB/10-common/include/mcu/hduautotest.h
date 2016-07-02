 /*****************************************************************************
   模块名      : hduhardwate auto test client
   文件名      : hduautotest.h
   相关文件    : hduhwautotestclient.cpp
   文件实现功能: hdu硬件自动化测试
   作者        : 
   版本        : V4.0  Copyright(C) 2009-2015 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2009/04/28  1.0         江乐斌        创建   
******************************************************************************/

#ifndef HDUHARDWARE_AUTO_TEST_H
#define HDUHARDWARE_AUTO_TEST_H

#include "eventid.h"
#include "kdvtype.h"
#include "osp.h"
#include "mcuconst.h"

#define HDU_SERVER_APPID   100            // 服务器APPID号

/*hdu自动检测消息（57601－57640）*/ 
#define C_S_REG_REQ                       EV_HDUAUTOTEST_BGN        // 注册请求
#define S_C_REG_ACK                       EV_HDUAUTOTEST_BGN + 1    // ACK
#define S_C_REG_NACK                      EV_HDUAUTOTEST_BGN + 2    // NACK


//HDU接受码流端口 
#define PRODUCETEST_RECVBASEPORT	12000
//HDU接受码流端口 
//第一个HDU
//（软件把一路码流 发2份到不同端口）
//第一份 接受端口
#define HDU_RECRTPPORT1_1_vid		PRODUCETEST_RECVBASEPORT + 0
#define HDU_RECRTCPPORT1_1_vid		PRODUCETEST_RECVBASEPORT + 1
#define HDU_RECRTPPORT1_1_aud		PRODUCETEST_RECVBASEPORT + 2
#define HDU_RECRTCPPORT1_1_aud		PRODUCETEST_RECVBASEPORT + 3

//第二份 接受端口
#define HDU_RECRTPPORT1_2_vid		PRODUCETEST_RECVBASEPORT + 4
#define HDU_RECRTCPPORT1_2_vid		PRODUCETEST_RECVBASEPORT + 5
#define HDU_RECRTPPORT1_2_aud		PRODUCETEST_RECVBASEPORT + 6
#define HDU_RECRTCPPORT1_2_aud		PRODUCETEST_RECVBASEPORT + 7

#define  ev_HDUAUTOTEST_CMD      11010
#define  ev_HDURESTORE_CMD       11011
#define  ev_HDURESTORE_NOTIFY    11012

enum EmHDUTestItem
{
	emVGASXGA60Hz = 100,
	emVGAXGA60Hz,
	emYPbPr1080P60fps,
	emYPbPr576i50Hz,
	emAudio,
	emDviToHdmi,
	emDviToVga

};

// 测试消息头结构
struct THDUAutoTestMsg
{	
private:
	// 测试类别
    EmHDUTestItem m_emAutoTestType;
	// 版本号 0x0100
    u16  m_wProtocolIdentifer;
    // 开始测试标志位：TRUE，开始测试；FALSE，停止测试
    u8 m_bAutoTestStart;
    // 视频类型
    u8 m_byVideoType;
    // 视频端口
    u8 m_byVideoPort;
	// 音频类型
	u8 m_byAudioType;
	// 音频端口
	u8 m_byAudioPort;
    // 测试结果: 0，失败; 1，成功。
    u8 m_bAutoTestReult; 
    // 测试失败原因:1, 硬件测试结果失败; 2, 测试正在进行;  
    // 3, 不支持该项测试; 4,HDU状态错误;5, 测试软件版本号错误; 
    // 其它附加返回信息
    u8 m_byErrorReason;

	u8 m_abyRemain[512];
public:
	THDUAutoTestMsg()
	{
		memset( this, 0x0, sizeof( THDUAutoTestMsg ) );
	}
	u16   GetPrtclIdentifer( void ){ return( ntohs( m_wProtocolIdentifer ) ); }
    void  SetPrtclIdentifer( u16 wProtocolIdentifer ){ m_wProtocolIdentifer = htons(wProtocolIdentifer); }
    
	EmHDUTestItem   GetAutoTestType( void ){ return (EmHDUTestItem)(ntohl((u32)m_emAutoTestType)); }
    void  SetAutoTestType( EmHDUTestItem emHduTestItem ){ m_emAutoTestType = (EmHDUTestItem)(htonl((u32)emHduTestItem)); }
    
    u8  GetAutoTestStart( void ){ return m_bAutoTestStart; }
    void  SetAutoTestStart( u8 bAutoTestStart ){ m_bAutoTestStart = bAutoTestStart; }

	u8  GetAutoTestReult( void ){ return m_bAutoTestReult; }
    void  SetAutoTestReult( u8 bAutoTestReult ){ m_bAutoTestReult = bAutoTestReult; }
    
	u8    GetVideoType(void){ return m_byVideoType; }
    void  SetVideoType( u8 byVideoType ){ m_byVideoType = byVideoType; }

	u8    GetVideoPort(void){ return m_byVideoPort; }
    void  SetVideoPort( u8 byVideoPort ){ m_byVideoPort = byVideoPort; }

	u8    GetAudioType(void){ return m_byAudioType; }
    void  SetAudioType( u8 byAudioType ){ m_byAudioType = byAudioType; }

	u8    GetAudioPort(void){ return m_byAudioPort; }
    void  SetAudioPort( u8 byAudioPort ){ m_byAudioPort = byAudioPort; }

	u8    GetErrorReason(void){ return m_byErrorReason; }
    void  SetErrorReason( u8 byErrorReason ){ m_byErrorReason = byErrorReason; }

	void  Print(void)
	{
		printf("THDUAutoTestMsg as follows:\n");
		printf("\tm_wProtocolIdentifer:\t0x%x\n", GetPrtclIdentifer());
		printf("\tm_emAutoTestType:\t0x%x\n", GetAutoTestType());
		printf("\tm_bAutoTestStart:\t%d\n", m_bAutoTestStart);
		printf("\tm_byVideoType:\t\t%d\n", m_byVideoType);
		printf("\tm_byAudioType:\t\t%d\n", m_byAudioType);
		printf("\tm_byVideoPort:\t\t%d\n", m_byVideoPort);
		printf("\tm_byAudioPort:\t\t%d\n", m_byAudioPort);
		printf("\tm_bAutoTestReult:\t%d\n", m_bAutoTestReult);
		printf("\tm_byErrorReason:\t%d\n", m_byErrorReason);
	}

}

;

#endif



