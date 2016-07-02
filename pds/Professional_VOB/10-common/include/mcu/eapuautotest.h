/*==============================================================================
类名    :  CEapuTestClient
功能    :  EAPU自动测试客户端类
主要接口:  
备注    :  
-------------------------------------------------------------------------------
修改记录:  
日  期     版本          修改人          走读人          修改记录						
==============================================================================*/
#ifndef EAPUHARDWARE_AUTO_TEST_H
#define EAPUHARDWARE_AUTO_TEST_H

#include "eventid.h"
#include "kdvtype.h"
#include "osp.h"
#include "mcuconst.h"

#define EAPU_LOCALRCVPORT               (u16)40000
#define SERVER_RCVPORT                  (u16)50000
#define EAPU_LOCALSNDPORT               (u16)10000

#define EAPU_SERVER_APPID               200             // 测试服务器APPID号

#define EV_EAPUAUTOTEST_BGN                EV_MPUAUTOTEST_END
#define EV_EAPUAUTOTEST_END				   EV_EAPUAUTOTEST_BGN + 19	

#define TEST_C_S_LOGIN_REG                 EV_EAPUAUTOTEST_BGN + 1//外设向服务器进行注册请求。包括用户名和用户密码信息。以便服务器端进行身份验证。u8 *32  空	+ 用户信息 + 设备信息
#define TEST_S_C_LOGIN_ACK                 EV_EAPUAUTOTEST_BGN + 2
#define TEST_S_C_LOGIN_NACK                EV_EAPUAUTOTEST_BGN + 3
#define TEST_S_C_START_REG                 EV_EAPUAUTOTEST_BGN + 4//(u8)DSPID+(u8)AUDIOTYPE
#define TEST_C_S_START_ACK                 EV_EAPUAUTOTEST_BGN + 5
#define TEST_C_S_START_NACK                EV_EAPUAUTOTEST_BGN + 6
#define TEST_S_C_STOP_REG                  EV_EAPUAUTOTEST_BGN + 7
#define TEST_C_S_STOP_ACK                  EV_EAPUAUTOTEST_BGN + 8
#define TEST_C_S_STOP_NACK                 EV_EAPUAUTOTEST_BGN + 9
#define TEST_S_C_RESTORE                   EV_EAPUAUTOTEST_BGN + 10
#define C_S_EAPURESTORE_NOTIF              EV_EAPUAUTOTEST_BGN + 11
#define S_C_TEST_LIGHT_CMD				   EV_EAPUAUTOTEST_BGN + 12
	

enum EmEAPUTestItem
{
	emTestDspID0 = 0,
	emTestDspID1,
	emTestDspID2,
	emTestDspID3,
	emTestDsp,				        //测试DSP
	emTestLight,					//指示灯测试
	emTestPort						//网口功能测试
};

#define ERR_EAPUTEST_ERRDSPID                  (u16)1000//无效的DSP号
#define ERR_EAPUTEST_ERRAUDTYPE                (u16)1001//未知的音频类型
#define ERR_EAPUTEST_ERRDEVICE                 (u16)1002//未知的音频类型

// 测试消息头结构
struct TEapuAutoTestMsg
{	
private:
	// 测试类别
    EmEAPUTestItem m_emAutoTestType;
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
	TEapuAutoTestMsg()
	{
		memset( this, 0x0, sizeof( TEapuAutoTestMsg ) );
	}
	u16   GetPrtclIdentifer( void ){ return( ntohs( m_wProtocolIdentifer ) ); }
    void  SetPrtclIdentifer( u16 wProtocolIdentifer ){ m_wProtocolIdentifer = htons(wProtocolIdentifer); }
    
	EmEAPUTestItem   GetAutoTestType( void ){ return (EmEAPUTestItem)(ntohl((u32)m_emAutoTestType)); }
    void  SetAutoTestType( EmEAPUTestItem emHduTestItem ){ m_emAutoTestType = (EmEAPUTestItem)(htonl((u32)emHduTestItem)); }
    
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
		printf("TEapuAutoTestMsg as follows:\n");
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
