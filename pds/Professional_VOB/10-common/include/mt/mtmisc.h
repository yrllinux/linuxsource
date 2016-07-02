
#ifndef _SHORT_MESSAGE_HEADER
#define _SHORT_MESSAGE_HEADER

#include "mtstruct.h"
// 短消息结构
class CShortMsg
{
public:
    CShortMsg();
    CShortMsg( EmSMSType emSMSType,TMtId tMtSrc, u8 byDstNum,
			       TMtId* ptDstMt, u8 byRollSpeed, u8 byRollTimes, u8* pchText );
	BOOL Marshal( u8 *pchContent, u16 wInLen, u16 &wOutLen );//pack
	BOOL UnMarshal( u8 *pchContent, u16 wInLen );            //unpack
	void SetType( EmSMSType emSMSType ) { m_emType = emSMSType; };//设置滚动消息类型
	EmSMSType GetType( void ) { return m_emType; }//获取滚动消息类型
	void SetRollTimes ( u8 byTimes ) { m_byRollTimes = byTimes; } //设置滚动次数
	u8 GetRollTimes( void ) { return m_byRollTimes; }  //获取滚动次数
	void SetRollSpeed( u8 byRollSpeed ) { m_byRollSpeed = byRollSpeed;} //设置滚动速度
	u8 GetRollSpeed( void ) { return m_byRollSpeed; }  //获取滚动速度
	void SetMsgSrcMtId( TMtId tMtId ) { SetMsgSrcMtId( tMtId.byMcuNo, tMtId.byTerNo ); } //设置消息源
	void SetMsgSrcMtId( u8 byMcuNo, u8 byMtNo ) { m_tMtSrc.byMcuNo = byMcuNo, m_tMtSrc.byTerNo  = byMtNo; }  //设置消息源
	TMtId GetMsgSrcMtId( void ) { return m_tMtSrc; } //获取消息发源终端ID
	u8   GetDstMtNum( void ) { return m_byDstNum; }//获取目的终端个数
	void AddDstMt( const TMtId *ptMtList, u16 wDstMtNum ); //增加目的终端列表
	void AddDstMt( u8 byMcuNo, u8 byMtNo ); //增加目的终端列表
	void AddDstMt( TMtId tMt );      //增加目的终端列表
	BOOL GetDstMt( TMtId &tMt, u8 byIndex  ); //获取指定的目的终端列表
	u8  GetDstMt( TMtId *ptMt, u8 byMaxCount  ); //获取指定所有终端列表
	s8 *GetText( void ) { return m_abyContent; } //获取文本
	void SetText( const s8* pbyText, u16 wTextLen );  //设置文本


private :
	EmSMSType m_emType;
	TMtId m_tMtSrc;
	u8   m_byDstNum;
	TMtId m_atMtDst[MT_MAXNUM_SMSDST];
	u8    m_byRollSpeed; //速度 (1-5)
	u8    m_byRollTimes; //滚动次数
	s8    m_abyContent[MT_MAXLEN_SMSTEXT+1];
};

//const default value
class CDefaultCfg
{
protected:
	CDefaultCfg(){};
	CDefaultCfg(CDefaultCfg&){};
public:
	~CDefaultCfg(){};
	static CDefaultCfg* GetInstance();
public:
	void Serial(TSerialCfg& tCfg);
	void ImageAdjustParam(TImageAdjustParam& tCfg, EmMtModel emModel);
	void ResetOsdCfg( TOsdCfg& tCfg );
	void ResetUserCfg( TUserCfg& tUserCfg );
	void ResetH323Cfg( TH323Cfg& tCfg );
	void ResetAVPriorCfg(TAVPriorStrategy& tAVPriorCfg);
	void ResetEthnetCfg( TEthnetInfo& tEthInfo );
	void ResetSNMPCfg( TSNMPCfg& tSnmpCfg );
	void ResetCommonBasePort( u32& dwTCPPort, u32& dwUDPPort );
	void ResetLostPacketResend( TLostPackResend &tCfg );
	void ResetStreamCfg( TStreamMedia &tStreamCfg );
	void ResetVideoParam( TVideoParam& tVideoCfg );
	void ResetCamerCfg( TCameraCfg& tCfg );
	void ResetQosCfg( TIPQoS& tCfg  );
	void ResetE1Cfg( TE1Config& tCfg );
    void ResetPPOECfg( TPPPOECfg& tcfg );
	void ResetVideoDisplayFrm( TVideoDisplayFrm& tFrm );
	void ResetCallProtocal( TGuideCfg& tGuideCfg );
	void ResetAudioPrecedence( u32& dwAudioPrecedence );
	void ResetIPQOs( TIPQoS& val );
	void ResetMainImageCode( TVideoParam& val,EmMtModel emModel  );
	void ResetIsSendingStaticPic( BOOL& bSendStaticPic );
	void ResetMTU( s32& dwVal );
	void ResetMiscCfg( TMiscCfg& tInfo );
	void ResetHDResOutputParam( u32& dwHDResEnable );
	void ResetHDPicPlusParam( BOOL& bPicPlusEnable );
	void ResetConfTime( BOOL& bConfTimeShow );
	void ResetFarCtrl( BOOL& bFarCtrlEnable );
public:
	void ResetAll();
};


class CAddrGroupOpt
{
public:
    CAddrGroupOpt();
   	BOOL Marshal( u8 *pchContent, u16 wInLen, u16 &wOutLen );//字节序转换以及紧凑
	BOOL UnMarshal( u8 *pchContent, u16 wInLen );            
	void SetType( EmAddrGroupOptType emType ){ m_emOptType = emType; }//设置操作类型
	EmAddrGroupOptType GetType(){ return m_emOptType; }//获取操作类型
	void SetVersion( u8 byVersion ){ m_byVersion = byVersion; }//设置版本
	u8 GetVersion(){ return m_byVersion; }//获取版本
    //设置主叫呼集信息
    void SetSiteCallInfo( TSitecallInformation tSiteCallInfo ) { m_tSiteCallInfo = tSiteCallInfo; }
	TSitecallInformation GetSiteCallInfo(){ return m_tSiteCallInfo; }//获取主叫呼集信息

	//设置会议模板信息
	void SetConfTemplateInfo( TConfBaseInfo tConfTemplateInfo ) { m_tConfInfo = tConfTemplateInfo; }
	TConfBaseInfo GetConfTemplateInfo(){ return m_tConfInfo; }//获取会议模板信息

private :
	BOOL Check( u16 wLen ); 
    TConfBaseInfo m_tConfInfo;
	TSitecallInformation m_tSiteCallInfo;
	EmAddrGroupOptType m_emOptType;
	u8    m_byVersion;
	s8    m_abyContent[MT_MAX_ADDRUSERDEF_LEN+1];
};


#endif

