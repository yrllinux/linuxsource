/*****************************************************************************
模块名      : 终端控制台
文件名      : MtcLibIntTest.h
相关文件    : 
文件实现功能: 集成测试接口
作者        : 熊广化
版本        : V3.0  Copyright(C) 1997-2004 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2004/02/01  3.0         熊广化      创建
2004/12/10  3.5         王慧琛      修改为3.5版
2004/12/30  3.5         王慧琛      去掉操作接口
******************************************************************************/
#pragma once
#include "MtcLib.h"

API u32 mtcinttestlibver();
API u32 mtcinttestlibhelp();

class CMtcLibIntTest
{
public:
	CMtcLibIntTest(void);
	virtual ~CMtcLibIntTest(void);

	virtual BOOL32 LinkInstance(u32 dwIP, u32 dwPort);
	virtual void DisLinkInstance(void);
	// 是否已和被测试实例建立连接
	virtual BOOL32 IsLinked(void);  

//////////////////////////////////////////////////////////////////////////
//CConnect
    //	查询终控是否已连上终端
	virtual BOOL32 IsMtcConnectedMt(void);
    //	查询当前所连的终端
	virtual BOOL32 GetConnectedMt(CMtcMt &mt);
    //	查询当前用户
	virtual BOOL32 GetConnectedUser(CLoginRequest *pcLoginuser);
	  //函数功能：终端是否在空闲状态
    virtual BOOL32 IsIdle();


//////////////////////////////////////////////////////////////////////////
//地址簿集成测试

	//	得到地址簿条目数
	virtual BOOL32 GetEntriesCount(u32 *pdwCount);
    //	得到地址簿组数
	virtual BOOL32 GetGroupCount(u32 *pdwCount);
    //	得到地址簿条目信息
	virtual BOOL32 GetEntry(u32 dwIndex, CAddrEntry &entry);
    //	得到首条条目信息
	virtual BOOL32 GetFirstEntry(CAddrEntry &entry);
    //	得到下一条条目信息
	virtual BOOL32 GetNextEntry(u32 u32Index, CAddrEntry &entry);
    //	得到地址簿组信息
	virtual BOOL32 GetGroup(u32 dwIndex, CAddrMultiSetEntry &mentry);
    //	得到首条组信息
	virtual BOOL32 GetFirstGroup(CAddrMultiSetEntry &mentry);
    //	得到下一条组信息
	virtual BOOL32 GetNextGroup(u32 u32Index, CAddrMultiSetEntry &mentry);
	//查询一条目是否存在
	virtual BOOL32  IsEntryExist(CAddrEntry *pcEntry);
	//查询一组是否存在
	virtual BOOL32 IsGroupExist(CAddrMultiSetEntry *pcAddrMentry);

//////////////////////////////////////////////////////////////////////////
// 用户管理集成测试

	//获得用户总数
	virtual BOOL32 GetUserCounts(u32 *pdwCount);
	//查询一用户是否存在
	virtual BOOL32 IsUserExist(CUserFullInfo *pcUserInfo);

//////////////////////////////////////////////////////////////////////////
//多点会议

    //	得到会议信息列表
	virtual BOOL32 GetConfs(vector<TConfId>& vConf);
	//3.42	得到当前会议
    virtual BOOL32 GetCurrentConf(TMtConfInfo *pConfInfo);
    //	查询是否正在等待创建会议回应
	virtual BOOL32 IsWaitingCreateAck();
    //3.25	查询是否正在等待加入会议回应
	virtual BOOL32 IsWaitingJoinAck();
    //3.26	查询是否在会议中
	virtual BOOL32 IsInMultConf();
    //3.27	查询是否正在轮询
	virtual BOOL32 IsPolling();
    //3.28	得到与会终端信息列表
	virtual BOOL32 GetMts(vector<CMtcMt>& vMts);    
    //3.29	得到当前发言人
	virtual BOOL32 GetSpeaker(CMtcMt *pcMtcMt);
    //3.30	得到当前主席
	virtual BOOL32 GetChairman(CMtcMt *pcMtcMt);
    //3.31	得到申请加入的终端
	virtual BOOL32 GetReqJoinMts(vector<CMtcMt>& vMts);
    //3.32	得到申请发言的终端
	virtual BOOL32 GetReqSpeakerMts(vector<CMtcMt>& vMts);
    //3.33	得到申请主席的终端
	virtual BOOL32 GetReqChairmanMts(vector<CMtcMt>& vMts);
	//是否发言人
	virtual BOOL32 IsSpeaker();
	//是否主席
	virtual BOOL32 IsChairman();
    //是否在插话
    virtual BOOL32 IsChime();
    //当前视频源
    virtual BOOL32 GetCurVideoSource(CMtcMt *pMt);
    //是否在讨论
    virtual BOOL32 IsDiscuss();
    //是否在语音激励
    virtual BOOL32 IsVAC();
    //获取当前的画面复合状态
    virtual BOOL32 GetVMPInfo(TMtVMPParam *ptVMPinfo);
    //是否自环
    virtual BOOL32 IsSelpLoop();
    //获得当前终端快照的个数
    virtual BOOL32 GetNumOfSnapshot(u16 *pwNum);
	//3.44	得到当前正在选看的终端
    virtual BOOL32 GetCurrentChoic(CMtcMt &Mt);
//////////////////////////////////////////////////////////////////////////
//点对点会议
	//3.21	查询是否在点对点会议中
	virtual BOOL32 IsInP2PConf();
    //3.22	查询是否正在呼叫
	virtual BOOL32 IsCalling();
	//获取点对点会议中的呼叫者
	virtual BOOL32 GetP2PCaller(CMtcMt *pMt);
//////////////////////////////////////////////////////////////////////////
//监控
	//是否双流
	virtual BOOL32 IsDual();
	//获取双流类型
	virtual BOOL32 GetDualType(u8 *pchType);

//	virtual BOOL32 IsMonitorNear();
//    virtual BOOL32 IsMonitorFar();
	

	
//////////////////////////////////////////////////////////////////////////
//矩阵
   //3.4	得到音频矩阵
//     virtual BOOL32 GetAudioSche(_TMATRIXINFO &ms);
    //3.5	得到视频矩阵
//     virtual BOOL32 GetVideoSche(_TMATRIXINFO &ms);
    //3.6	得到音频输入源
	virtual u32 GetAudioInputSrc();
    //3.7	得到视频输入源
	virtual u32 GetVideoInputSrc();
    //3.8	是否静音
	virtual BOOL32 IsLocalSpeakerMute();
    //3.9	是否哑音
	virtual BOOL32 IsLocalMicMute();
    //3.10	得到输出音量
	virtual BOOL32 GetLocalSpeakerVolume(s32 *pnVolNum);
    //3.11	得到输入音量
	virtual BOOL32 GetLocalMicVolume(s32 *pnVolNum);
    //3.12	是否遥控器使能
	virtual BOOL32 IsEnableRemoteCtrl();
//////////////////////////////////////////////////////////////////////////
//配置文件 
//(少的配置项)  2:自动切换为监控终端
//				3:矩阵配置
//				4:桌面双流
//				5:横幅配置
 
	//	得到当前的语言设置
	virtual BOOL32 GetCfgLanguage(EmLanguage& emLanguage);
	//	获取H323的配置信息
	virtual BOOL32 GetCfgH323(PTH323Cfg pCfg);
    //	得到以太网的配置信息
	virtual BOOL32 GetCfgEthnet(u8 byIndex, PTEthnetInfo pCfg);
	//	得到当前的用户配置信息
	virtual BOOL32 GetCfgUser(PTUserCfg pCfg);
	//	得到当前的视频配置参数(主视频和辅视频)
	virtual BOOL32 GetVideoParam(EmVideoType emType, PTVideoParam pCfg);
	//	得到当前的台标配置信息
	virtual BOOL32 GetCfgOsd(PTOsdCfg pCfg);
	//	得到当前的E1配置信息
	virtual BOOL32 GetCfgE1(PTE1Config pCfg);
	//	得到当前的串口配置信息
	virtual BOOL32 GetCfgSerial(EmSerialType emType ,PTSerialCfg pCfg);
	//	得到当前的Qos配置信息
	virtual BOOL32 GetCfgQos(PTIPQoS pCfg);
	//	得到当前的防火墙配置信息
	virtual BOOL32 GetCfgFireWall(PTEmbedFwNatProxy pCfg);
	//	得到一条路由配置
	virtual BOOL32 GetCfgRoute(u8 byIndex ,PTRouteCfg pCfg);
	//获取全部的路由配置信息
	virtual BOOL32 GetAllRouteCfg(vector<TRouteCfg> &vctRouteCfg);
	//	得到当前的pppoe配置
	virtual BOOL32 GetCfgPPPoE(PTPPPOECfg pCfg);
	//	得到当前的SNMP配置
	virtual BOOL32 GetCfgSNMP(PTSNMPCfg pCfg);
	//	得到静态Nap映射的配置
	virtual BOOL32 GetCfgNapMap(PTNATMapAddr pCfg);
	//	得到发送重传的配置
	virtual BOOL32 GetCfgRetrans(TLostPackResend* pCfg);
	//	是否AEC
	virtual	BOOL32 GetCfgAEC(BOOL32& bUsed);
	//	是否AGC
	virtual BOOL32 GetCfgAGC(BOOL32& bUsed);
	//	得到优选协议的配置
	virtual BOOL32 GetCfgAVPrior(PTAVPriorStrategy pCfg);
	//	得到输入音量
	virtual BOOL32 GetCfgInVol(u8 &byVol);
	//	得到输出音量
	virtual BOOL32 GetCfgOutVol(u8 &byVol);
	//	得到图像配置
	virtual BOOL32 GetCfgImage(PTImageAdjustParam pCfg);
	//	得到视频制式的配置(主视频和辅视频)
	virtual BOOL32 GetCfgVStandard(EmVideoType emType, EmVideoStandard &emStandard);
	//	得到呼叫速率的配置
	virtual BOOL32 GetCfgCallRate(u32 &dwCallRate);
	//	得到摄像头的配置0....5
	virtual BOOL32 GetCfgCam(u8 byIndex, PTCameraCfg pCfg);
	//	得到流媒体的配置
	virtual BOOL32 GetCfgStream(PTStreamMedia pCfg);
	 // 获取当前连接节点标识
	//2005 04 22 张寒冰
	virtual u32 GetConnectedNodeID() const;
	virtual BOOL32 GetMtStatus(TTerStatus *pMtStatus);
protected:
    //发送同步请求
	BOOL32 SendEvent(u16 u16Event, const void *pBuf, u16 u16Size,
		void *pAckBuf, u16 u16AckSize, u16 &u16AckSizeRet);
private:
	// 当前建立连接的节点标识
	u32 m_u32ConnectedNode;
	
};
