/*****************************************************************************
   模块名      : MtAdp
   文件名      : mastructsat.h
   相关文件    : 
   文件实现功能: 卫星会议协议适配相关的数据结构和宏定义  
   作者        : 张宝卿
   版本        : V4.6  Copyright(C) 2009-2012 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2009/9      4.6         张宝卿          创建
******************************************************************************/

#ifndef _SATSTRUCT_H_
#define _SATSTRUCT_H_


#ifdef WIN32
#pragma comment( lib, "ws2_32.lib" ) 
#pragma pack( push )
#pragma pack( 1 )
#endif

#define  SIGNAL_CHN_BOTH				(u8)0
#define  SIGNAL_CHN_SD					(u8)1
#define  SIGNAL_CHN_HD					(u8)2

#define  MAXLEN_CONF_NAME               (u8)32
#define  MAXLEN_CONF_PASSWD             (u8)32
#define	 MAXNUM_SATMULTICAST            (u8)5       //卫星线路最大组播的限制
#define  MCU_SENDTOMULTI_PORT           (u16)5000
#define  MT_RCVMCUMEDIA_PORT            (u16)4000	//终端接受MCU数据的起始端口

#define  MAXNUM_SATMT                   (u16)1024   //最大终端数


#define MOD_CLKSRC_RCV					3			//发送：从时钟源
#define MOD_CLKSRC_INTER				0			//发送：主时钟源
#define DEM_CLKSRC_RCV					0			//接收：从时钟源
#define DEM_CLKSRC_INTER				1			//接收：主时钟源


#define MAXLEN_SATMTALIAS				16

//外设占用终端号
// const u16 BRDCST_VMP			        = 1026;
// const u16 BRDCST_MIXN			    = 1027;
// const u16 BRDCST_MIX			        = 1025;

const u16 BRDCST_VMP			        = 254;
const u16 BRDCST_MIXN			        = 253;
const u16 BRDCST_MIX			        = 252;


const u8 MTSEE_NONE						= 0;
const u8 MTSEE_BRDCAST			        = 1;
const u8 MTSEE_UNICAST			        = 2;


//卫星会议 配置数据
struct TMcuSatInfo
{
public:
    TMcuSatInfo():m_dwApplyIpAddr(0),
                  m_wApplyPort(0),
                  m_wApplyReceivPort(0),
                  m_wRcvMtSignalPort(0),
                  m_dwTimeRefreshIpAddr(0),
                  m_wTimeRefreshPort(0),
                  m_wMcuRcvMtMediaStartPort(0),
                  m_wMtRcvMcuMediaPort(0),
				  m_dwMcuMulticastSignalIpAddr(0),
				  m_wMcuMulticastSignalPort(0),
				  m_dwMcuMulticastDataIpAddr(0),
				  m_wMcuMulticastDataPort(0),
				  m_wMcuMulticastDataPortNum(0),
				  m_byMcuMulticastTTL(0),
				  m_byMcuMaxUploadCount(0),
				  m_dwMcuSecMulticastIpAddr(0)
    {
    }

    u32  GetApplyIp(void) const { return ntohl(m_dwApplyIpAddr); }
    void SetApplyIp(u32 dwIpAddr) { m_dwApplyIpAddr = htonl(dwIpAddr);  }

    u16  GetApplyPort(void) const { return ntohs(m_wApplyPort);    }
    void SetApplyPort(u16 wPort) { m_wApplyPort = htons(wPort); }

    u16  GetApplyRcvPort(void) const { return ntohs(m_wApplyReceivPort);    }
    void SetApplyRcvPort(u16 wPort) { m_wApplyReceivPort = htons(wPort);    }

    u16  GetRcvMtSignalPort(void) const { return ntohs(m_wRcvMtSignalPort);   }
    void SetRcvMtSignalPort(u16 wPort) { m_wRcvMtSignalPort = htons(wPort); }

    u32  GetTimeRefreshIpAddr(void) const { return ntohl(m_dwTimeRefreshIpAddr);    }
    void SetTimeRefreshIpAddr(u32 dwIp) { m_dwTimeRefreshIpAddr = htonl(dwIp);  } 

    u16  GetTimeRefreshPort(void) const { return ntohs(m_wTimeRefreshPort); }
    void SetTImeRefreshPort(u16 wPort) { m_wTimeRefreshPort = htons(wPort); }

    u16  GetMcuRcvMtMediaStartPort(void) const { return ntohs(m_wMcuRcvMtMediaStartPort);   }
    void SetMcuRcvMtMediaStartPort(u16 wPort) { m_wMcuRcvMtMediaStartPort = htons(wPort);   }

    u16  GetMtRcvMcuMediaPort(void) const { return ntohs(m_wMtRcvMcuMediaPort); }
    void SetMtRcvMcuMediaPort(u16 wPort) { m_wMtRcvMcuMediaPort = htons(wPort); }

	u32	 GetMcuMulticastSignalIpAddr(void) const { return ntohl(m_dwMcuMulticastSignalIpAddr);	}
	void SetMcuMulitcastSignalIpAddr(u32 dwIp) {	m_dwMcuMulticastSignalIpAddr = htonl(dwIp);	}

	u16  GetMcuMulticastSignalPort(void) const {	return ntohs(m_wMcuMulticastSignalPort);	}
	void SetMcuMulticastSignalPort(u16 wPort){	m_wMcuMulticastSignalPort = htons(wPort);	}

	u32	 GetMcuMulticastDataIpAddr(void) const { return ntohl(m_dwMcuMulticastDataIpAddr);	}
	void SetMcuMulticastDataIpAddr(u32 dwIp) { m_dwMcuMulticastDataIpAddr = htonl(dwIp);	}

	u16  GetMcuMulticastDataPort(void) const {	return ntohs(m_wMcuMulticastDataPort);	}
	void SetMcuMulticastDataPort(u16 wPort) { m_wMcuMulticastDataPort = htons(wPort);	}

	u16  GetMcuMulticastDataPortNum(void) const{ return ntohs(m_wMcuMulticastDataPortNum);	}
	void SetMcuMulticastDataPortNum(u16 wPort) { m_wMcuMulticastDataPortNum = htons(wPort);	}

	u8   GetMcuMulticastTTL(void) const { return m_byMcuMulticastTTL;	}
	void SetMcuMulticastTTL(u8 byTTL) { m_byMcuMulticastTTL = byTTL;	}

	u8   GetMcuMaxUploadNum(void) const { return m_byMcuMaxUploadCount;	}
	void SetMcuMaxUploadNum(u8 byNum) { m_byMcuMaxUploadCount = byNum;	}

	u32  GetMcuSecMulticastIpAddr(void) const { return ntohl(m_dwMcuSecMulticastIpAddr);	}
	void SetMcuSecMulticastIpAddr(u32 dwIp) { m_dwMcuSecMulticastIpAddr = htonl(dwIp);	}

    void Print() const
    {
        OspPrintf(TRUE, FALSE, "McuSatConfValue as follows:\n");
        OspPrintf(TRUE, FALSE, "\tm_dwApplyIpAddr: \t\t0x%x\n",			GetApplyIp());
        OspPrintf(TRUE, FALSE, "\tm_wApplyPort: \t\t\t%d\n",			GetApplyPort());
        OspPrintf(TRUE, FALSE, "\tm_wApplyReceivPort: \t\t%d\n",		GetApplyRcvPort());
        OspPrintf(TRUE, FALSE, "\tm_wRcvMtSignalPort: \t\t%d\n",		GetRcvMtSignalPort());
        OspPrintf(TRUE, FALSE, "\tm_dwTimeRefreshIpAddr: \t\t0x%x\n",	GetTimeRefreshIpAddr());
        OspPrintf(TRUE, FALSE, "\tm_wTimeRefreshPort: \t\t%d\n",		GetTimeRefreshPort());
        OspPrintf(TRUE, FALSE, "\tm_wMcuRcvMediaStartPort: \t%d\n",		GetMcuRcvMtMediaStartPort());
        OspPrintf(TRUE, FALSE, "\tm_dwMcuMulticastSignalIpAddr: \t0x%x\n", GetMcuMulticastSignalIpAddr());
        OspPrintf(TRUE,	FALSE, "\tm_wMcuMulticastSignalPort: \t%d\n",	GetMcuMulticastSignalPort());
		OspPrintf(TRUE, FALSE, "\tm_dwMcuMulticastDataIpAddr: \t0x%x\n", GetMcuMulticastDataIpAddr());
		OspPrintf(TRUE, FALSE, "\tm_dwMcuSecMulticastIpAddr:\t\0x%x\n",	GetMcuSecMulticastIpAddr());
        OspPrintf(TRUE,	FALSE, "\tm_wMcuMulticastDataPort: \t%d\n",		GetMcuMulticastDataPort());
        OspPrintf(TRUE,	FALSE, "\tm_wMcuMulticastDataPortNum: \t%d\n",	GetMcuMulticastDataPortNum());
		OspPrintf(TRUE, FALSE, "\tm_byMcuMaxUploadCount:\t\t%d\n",		GetMcuMaxUploadNum());
		OspPrintf(TRUE, FALSE, "\tm_byMcuMulticastTTL:\t\t%d\n",		GetMcuMulticastTTL());
    }

private:
    u32             m_dwApplyIpAddr;					/*申请频点-主机序*/
    u16             m_wApplyPort;						/*申请频点*/
    u16             m_wApplyReceivPort;					/*申请频点应答端口*/
    u16             m_wRcvMtSignalPort;                 /*接收终端卫星信令的端口*/
    u32             m_dwTimeRefreshIpAddr;              /*时间更新信令地址*/
    u16             m_wTimeRefreshPort;                 /*时间更新信令端口*/
    u16             m_wMcuRcvMtMediaStartPort;          /*Mcu接收Mt上传的媒体接收起始端口*/
    u16             m_wMtRcvMcuMediaPort;               /*MT接收Mcu广播的媒体接收端口*/
	u32				m_dwMcuMulticastSignalIpAddr;		/*MCU发送组播信令的IP*/
	u16				m_wMcuMulticastSignalPort;			/*MCU发送组播信令的端口*/
	u32				m_dwMcuMulticastDataIpAddr;			/*MCU发送组播数据的IP*/
	u16				m_wMcuMulticastDataPort;			/*MCU发送组播数据的端口*/
	u16				m_wMcuMulticastDataPortNum;			/*MCU发送组播数据端口的个数*/
	u8				m_byMcuMulticastTTL;				/*MCU组播TTL值*/
	u8				m_byMcuMaxUploadCount;				/*卫星会议最大上传路数*/
	u32				m_dwMcuSecMulticastIpAddr;			/*卫星会议第二组播Ip, 用于双速或双格式会议*/
	                                                    /*组播Ip稍后整理成数组进行管理，若卫星站点的格式码率超过了两个*/
};


typedef struct tagTInterfaceConfList
{
	u8 m_byConfIdx;
	char m_achName[16];
	tagTInterfaceConfList()
	{
		memset(this,0,sizeof(tagTInterfaceConfList));
	}
public:
	void SetConfIdx( u8 byIndex )
	{
		m_byConfIdx = byIndex;
	}
	u8 GetConfIdx(void)
	{
		return m_byConfIdx;
	}
	
	void SetConfName( LPCSTR lpszAlias )
	{
		if( lpszAlias != NULL )
		{
			strncpy( m_achName, lpszAlias, sizeof( m_achName ) );
			m_achName[sizeof( m_achName ) - 1] = '\0';
		}
		else
		{
			memset( m_achName, 0, sizeof( m_achName ) );
		}
	}
	char* GetConfName()
	{
		return m_achName;
	}
	
}PACKED TInterfaceConfList ,ITSatConfList;

//MODEM数据
struct TModemData
{
    u16 wConfIdxSnd;    //使用本MDOEM发送的会议号
	u32 dwSendFrequece; //本MDOEM的发送频率
	u32 dwSendBitRate;  //本MDOEM的发送码率
	TMt tMtSnd;			//使用本MDOEM的发送的终端/外设

    u16 wConfIdxRcv;    //使用本MDOEM接收的会议号
    u32 dwRevFrequece;  //本MDOEM的接收频率
    u32 dwRevBitRate;   //本MDOEM的接收码率
    TMt tMtRcv;			//使用本MDOEM的发送的终端/外设

	BOOL bConnected;    //是否连接
    u8   bySPortNum;	//本Modem接入MCU的S端口号
};

//会议上下星带宽记录 结构
struct TConfFreqBitRate
{
    u8 byConfIdx;                       //会议号
    u8 byNum;                           //接收频率个数
    u32 dwSendFreq;                     //发送频率
    u32 dwRevFreq[MAXNUM_MCU_MODEM];    //接收频率
    u32 dwSendBitRate;                  //发送码率
    u32 dwRevBitRate;                   //接收码率

	void Print()
	{
		OspPrintf(TRUE, FALSE, "[ConfIdx.%d]:\n", byConfIdx);
		OspPrintf(TRUE, FALSE, "\tSndFrq:\t\t%d\n", dwSendFreq);
		OspPrintf(TRUE, FALSE, "\tRcvFrqNum:\t%d\n", byNum);
		for (u8 byIdx = 0; byIdx < byNum; byIdx ++)
		{
			OspPrintf(TRUE, FALSE, "\t      [%d]:\t%d\n", byIdx, dwRevFreq[byIdx]);
		}
		OspPrintf(TRUE, FALSE, "\tRcvBitRate:\t%d\n", dwRevBitRate);
		OspPrintf(TRUE, FALSE, "\tSndBitRate:\t%d\n", dwSendBitRate);
	}
};


//向网管申请频点(MCU->NMS)
struct TMcuApplyFreq
{
protected:
	u16 wMessageType;   //命令字
	u32 dwSerialId;     //流水号
	u32 dwBrdBitRate;   //广播码率
	u16 wRcvNum;        //单播个数
	u32 dwRcvBitRate;   //单播码率
	u8	abyConfId[16];
public:
	void SetConfId( CConfId &cConfId )
	{
		u8 byConf[16];
		memset( byConf, 0 , 16 );
		cConfId.GetConfId( byConf, 16 );
		for ( u8 byLoop = 0; byLoop < 16 ; byLoop ++ )
			abyConfId[byLoop] = byConf[byLoop];
	}
	void SetMessageType( u16 wCmd )
	{
		wMessageType = htons( wCmd );
	}
	
	void SetSN( u32 dwSN )
	{
		dwSerialId = htonl( dwSN );
	}
	void SetBrdBitRate( u32 dwBit )
	{
		dwBrdBitRate = htonl( dwBit );
	}
	
	void SetRcvNum( u16 wNum )
	{
		wRcvNum = htons( wNum );
	}

	void SetRcvBitRate( u32 dwBit )
	{
		dwRcvBitRate = htonl( dwBit );
	}

	u32 GetSN()
	{
		return ntohl( dwSerialId );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//向网管释放频点(MCU->NMS)
struct TMcuReleaseFreq
{
protected:
	u16 wMessageType;   //命令字
	u32 dwSerialId;     //流水号
	u32 dwBrdBitRate;   //广播码率
	u32 dwBitRate;      //单播码率
	u16 wFreqCount;     //单收个数
	u32 dwBrdFreq;      //广播
	u32 dwFreq[5];
public:
	void SetCmd( u16 wCmd )
	{
		wMessageType = htons( wCmd );
	}
	
	void SetSN( u32 dwSN )
	{
		dwSerialId = htonl( dwSN );
	}
	void SetBrdBitRate( u32 dwBit )
	{
		dwBrdBitRate = htonl( dwBit );
	}
	void SetRcvBitRate( u32 dwBit )
	{
		dwBitRate = htonl( dwBit );
	}
	
	void SetNum( u16 wNum )
	{
		wFreqCount = htons( wNum );
	}
	
	u32 GetSN()
	{
		return ntohl( dwSerialId );
	}
	void SetBrdFreq( u32 dwFrq )
	{
		dwBrdFreq = htonl( dwFrq );
	}
	void SetFreeFreq( u8 byPos, u32 Freq )
	{
		if ( byPos < 1 || byPos > 5 )
			return;
		dwFreq[byPos-1] = htonl( Freq );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//网管申请的回应(NMS->MCU)
struct TFreqGetMsg
{
protected:
	u16 wMessageType;   //命令字
	u32 dwSerialId;     //流水号
	u16 wFreqCount; 
	u32 dwBrdFrq;
	u32 dwContent[5];
public:

	u16 GetMessageType()
	{
		return ntohs( wMessageType );
	}
	u32 GetSN()
	{
		return ntohl( dwSerialId );
	}
	u16 GetNum()
	{
		return ntohs( wFreqCount );
	}
	u32 GetContent( u8 byId )
	{
		if ( byId < 1 || byId > 5 )
			return 0;
		return ntohl( dwContent[byId-1] );
	}
	u32 GetBrdFreq()
	{
		return ntohl( dwBrdFrq );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//网管申请的回应(NMS->MCU)
struct TFreqReleaseMsg
{
protected:
	u16 wMessageType;   //命令字
	u32 dwSerialId;     //流水号
	u16 wFlags; 
public:
	u16 GetMessageType()
	{
		return ntohs( wMessageType );
	}
	u32 GetSN()
	{
		return ntohl( dwSerialId );
	}
	u16 GetFlags()
	{
		return ntohs( wFlags );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
//更改会议信息
struct TConfInfoNotif
{
protected:
	u16 wMessageType;   //命令字
	u32 dwSerialId;     //流水号
	u8	abyConfId[16];  //会议名称
	u32 dwStartTime;    //会议开始时间
	u32 dwEndTime;      //会议结束时间
public:
	void SetSerialId( u32 dwSN )
	{
		dwSerialId = htonl( dwSN );
	}
	void SetMessageType( u16 wMsg )
	{
		wMessageType = htons( wMsg );
	}
	void SetSN( u32 dwSn )
	{
		dwSerialId = htonl( dwSn );
	}
	void SetStartTime( u32 dwTime )
	{
		dwStartTime = htonl( dwTime );
	}
	void SetEndTime( u32 dwTime )
	{
		dwEndTime = htonl( dwTime );
	}
	void SetConfId( const CConfId &cConfId )
	{
		u8 byConf[16];
		memset( byConf, 0 , 16 );
		cConfId.GetConfId( byConf, 16 );
		for ( u8 byLoop = 0; byLoop < 16 ; byLoop ++ )
			abyConfId[byLoop] = byConf[byLoop];
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//与终端交互的卫星MCU会议信息结构
struct CSatConfInfo
{
protected:
	u8	m_byConfId;	        //在MCU中，该会议的会议索引号，对应每一个会议均不相同，可以
					        //作为区分会议的会议号
	u8  m_bySpeakerId;      //当前发言人的终端ID号，为零表示当前没有发言人。
	u8  m_byChairmanId;     //当前主席的终端ID号，为零表示当前没有主席。

	TTransportAddr  m_tBrdTransportAddr;    //发言人码流的组播地址
	TConfMtInfo     m_tConfMtInfo;          //与会终端状态
	TTransportAddr  m_tMpTransportAddr;     //终端发送码流的地址

	u8	m_byState;          //bit0: 0:未画面复合 1:画面复合
							//bit1-2: 00:非混音 01:混音 10:混音暂停
							//bit3-4: 00:非录像 01:录像 10:录像暂停
							//bit5-7: 000:非放像 001:放像 010:放像暂停 011:快进 100:快退		
	
    char m_achConfName[MAXLEN_CONF_NAME];   //会议名
	char m_achPasswd[MAXLEN_CONF_PASSWD];   //会议密码	
	u8	 m_byIntervalTime;
	TVMPParam m_tVmpParam;
	u8   m_byLineNum;
	u8	 m_byUpLoadCurrDmt[MAXNUM_MCU_MT / 8]; //当前上传成员列表
public:
	u8   m_byMtInMix[MAXNUM_MCU_MT / 8];
public:
	CSatConfInfo();
	
	//设置会议ID
	u8 		GetConfId( void );
	void	SetConfId( u8 byConfId );
	
	//设置发言人ID
	u8		GetSpeakerId( void );
	void	SetSpeakerId( u8 byMtId );
	BOOL	HasSpeaker( void );

	//操作主席ID
	u8		GetChairmanId( void );
	void	SetChairmanId( u8 byMtId );
	BOOL	HasChairman( void );

	//终端操作函数
	void	AddMt( u8 byMtId );
	void	AddJoinedMt( u8 byMtId );
	void	RemoveMt( u8 byMtId );
	void	RemoveJoinedMt( u8 byMtId );
	void	RemoveAllMt( void );
	BOOL	IsMtInConf( u8 byMtId );
	BOOL	IsMtJoinedConf( u8 byMtId );
	void	SetConfMtInfoMcuId( u8 byMcuId );
	u8		GetConfMtInfoMcuId();
	void	SetConfMtInfo( const TConfMtInfo & tConfMtInfo );
	const TConfMtInfo & GetConfMtInfo();
	
	//会议发送组播数据端口(其中的输入输出参数为主机序)
	u32		GetBrdMeidaIpAddr( void );
	u16		GetBrdMediaStartPort( void );
	void	SetBrdMeidaIpAddr ( u32 dwIpAddress );
	void	SetBrdMediaStartPort ( u16 wPort );

	//会议进行状态
	//是否处于画面复合状态
	BOOL IsVmp( void ) const	{ return( ( m_byState & 0x01 ) == 0x01 ? TRUE : FALSE ); }
	//设置是否处于画面复合状态
	void SetVmp( BOOL bVmp )    { if( bVmp ) m_byState |= 0x01; else m_byState &= ~0x01;}

	//会议混音状态
	BOOL IsNoMixing( void ) const	{ return( ( m_byState & 0x06 ) == 0x00 ? TRUE : FALSE ); }
	void SetNoMixing( void )	{ m_byState &= ~0x06; }
	BOOL IsMixing( void ) const	{ return( ( m_byState & 0x06 ) == 0x02 ? TRUE : FALSE ); }
	void SetMixing( void )	{ SetNoMixing(); m_byState |= 0x02; }
	BOOL IsMixingPause( void ) const	{ return( ( m_byState & 0x06 ) == 0x04 ? TRUE : FALSE ); }
	void SetMixingPause( void )	{ SetNoMixing(); m_byState |= 0x04; }
	
	//设置会议名
	LPCSTR	GetConfName( void );
	void	SetConfName( LPCSTR lpszBuf );
	//设置会议密码
	LPCSTR	GetConfPassword( void );
	void	SetConfPassword( LPCSTR lpszBuf );
	
	void SetSatIntervTime( u8 byTime );
	u8   GetSatIntervTime( );

	void SetMtInMix( u8 byMtId ){m_byMtInMix[byMtId / 8] |= 1 << ( byMtId % 8 );}
	BOOL IsMtInMix( u8 byMtId ){return ( ( m_byMtInMix[byMtId / 8] & ( 1 << ( byMtId % 8 ) ) ) != 0 );}

	void AddCurrUpLoadMt( u8 byMtId);
	void DelCurrUpLoadMt( u8 byMtId);
	u8	 IsMtInCurrUpLoad( u8 byMtId);

	BOOL IsVmpBrdst()
    {
        //FIXME:
        return 0;//m_tVmpParam.IsBrdst();
    }
	
    void SetVmpBrdst( BOOL bBrdst)
    {
        //m_tVmpParam.SetBrdst( bBrdst);
        return;
    }

	void SetVmpParam( TVMPParam &tVmpParam )
	{
		m_tVmpParam = tVmpParam;
	}

	TVMPParam* GetVmpParam()
	{
		return &m_tVmpParam;
	}

	void SetConfLineNum( u8 byNum )
	{
		m_byLineNum = byNum;
	}

	u8 GetConfLineNum()
	{
		return m_byLineNum;
	}

	void SetMpIpAddress(u32 dwIp)
	{
		m_tMpTransportAddr.SetIpAddr(dwIp);
	}
	u32 GetMpIpAddress()
	{
		return m_tMpTransportAddr.GetIpAddr();
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//与终端交互的卫星MCU终端信息结构
struct CMtMultiCastInfo
{
protected:
	u8 m_abyMtId[MAXNUM_SATMULTICAST];
	TTransportAddr m_atTransportAddr[MAXNUM_SATMULTICAST];
	
public:
	CMtMultiCastInfo();
	
	u8   GetMtIndex( u8 byMtId );  
	BOOL IsMtInMultiCastInfo( u8 byMtId );
	BOOL GetMtMultiCastAddr( u8 byMtId, TTransportAddr * ptTransAddr );
	BOOL GetMtMultiCastIpAddr( u8 byMtId, u32 * pdwIpAddr );
	BOOL GetMtMultiCastPort( u8 byMtId, u16 * pwPort );
	BOOL AddMtToMultiCastInfo( u8 byMtId, const TTransportAddr & tTransAddr );
	void RemoveMtFromMultiCastInfo( u8 byMtId );
	u8   GetIndexMt( u8 byIndex );
	BOOL GetIndexAddr( u8 byIndex, TTransportAddr * ptTransAddr );
	
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//定义速率结构
struct TBitRate
{
protected:
    u16		m_wBitRate;		//单位kbps
    
public:
    u16 GetBitRate( void ) const;
    void SetBitRate( u16 wBitRate );
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


struct CSatMtStatus
{
protected:
	u8	m_byMtId;               //终端ID号
	u8  m_byConfId;             //该终端所在会议的ID号，为0表示终端不在任何一个会议中
	u8  m_byMediaSrc;           //该终端当前接受的媒体源；可以为Speaker或者所选看的终端
	TBitRate m_tSendFlowRate;   //终端发送码流的速率(如果为0表示终端不需要发送)，单位kbps.
	u8  m_byState;	            // 0位: 是否需要处于轮循状态( 0:不需要轮循；1：需要轮循）。
					            // 1位：是否处于混音状态(0: 终端不在混音；1: 终端在混音）。
	//Added for Version 2
	u8  m_byCameraCmd;          //控制终端摄像头命令
	u8  m_byCameraVal;          //摄像头控制命令的参数
	u8	m_byVoiceState;	        //控制终端声音
							    // 0位：是否静音( 1:Y. 0:N )
							    // 1位：是否哑音( 1:Y. 0:N )
	u8  m_asDecodeKey[16];
	CMtMultiCastInfo m_tMtMultiCastInfo; //终端选看（包括监控）终端的地址信息
public:
	u8 m_byVideoSrcId;
	u8 m_byAudioSrcId;
	TTransportAddr m_tVideoRecAddr;
	TTransportAddr m_tAudioRecAddr;


public:
	CSatMtStatus();

	void	SetMtId( u8 byMtId );
	u8		GetMtId( void );

	void	SetMtConfId( u8 byConfId );
	u8		GetMtConfId( void );

	void	SetMtMediaSrc( u8 byMediaSrc );
	u8		GetMtMediaSrc( void );

	//输入/输出参数为主机序
	void	SetMtSendFlowRate( u16 wSendFlowRate );
	u16		GetMtSendFlowRate( void );

	void	SetMtNeedPolling( BOOL bNeedPolling = TRUE );
	BOOL	IsMtNeedPolling();

	void    SetMtMixing( BOOL bMixing = TRUE );
	BOOL	IsMtMixing();

	//Special For Version 2
	void    SetMtCameraCmd( u8 byCameraCmd );
	u8      GetMtCameraCmd();
	void    SetMtCameraVal( u8 byCameraVal );
	u8      GetMtCameraVal();
	
	void    SetMtVoiceState( u8 byVoiceState );
	
	void    SetMtMute( BOOL bMute = TRUE );
	BOOL	IsMtMute();

	void    SetMtDumb( BOOL bDumb = TRUE );
	BOOL	IsMtDumb();

	BOOL	IsMtInMultiCastInfo( u8 byMtId );
	BOOL	GetMtMultiCastIpAddr( u8 byMtId, u32 * pdwIpAddr );
	BOOL	GetMtMultiCastPort( u8 byMtId, u16 * pwPort );
	void	SetMtMultiCastInfo( const CMtMultiCastInfo & cMtMultiCastInfo );
	u8*    GetDecodeKey()
	{
		return m_asDecodeKey;
	}
	void   SetDecodeKey(u8* key)
	{
		for ( u8 byLoop = 0 ; byLoop < 16 ; byLoop ++)
			m_asDecodeKey[byLoop] = key[byLoop];
	}
	const CMtMultiCastInfo & GetMtMultiCastInfo();
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


struct CSatMsg
{
	enum
	{ 
		SATMSG_MAGIC = 0x0a0a0a0a,
		SATMSG_HEAD_LEN = 9,
		SATMSG_BODY_LEN = 1400 
	};
protected:
	u32	m_dwMagic;		//消息头的幻数。
	u16 m_wEvent;		//消息的事件号
	u8	m_byConfId;		//消息的会议号
	u16 m_wBodyLen;		//消息缓冲区的长度
	u8	m_abyBuffer[SATMSG_BODY_LEN];//接收消息缓冲区.
public:
    CSatMsg( void );
    CSatMsg( const u8 * pContent, u16 wLength );
    
	BOOL	IsValidSatMsg( void );	    //判断接受的消息是否合法，主要是校验幻数
	
	void	SetEventId( u16 wEvent );   //设置事件ID号
	u16		GetEventId( void );

	void	SetConfId( u8 byConfId );   //设置会议号
	u8		GetConfId( void );
    
	u16		GetSatMsgHeadLen( void );

	u16		GetSatMsgLen( void );	    //获取整个消息长度
    u8	*	GetSatMsg( void );

	void	SetSatMsgBody( const u8* pContent, u16 wLen );
	void	CatSatMsgBody( const u8* pContent, u16 wLen );
    
    u16		GetSatMsgBodyLen( void );	//获取消息缓冲区长度
 	void	SetSatMsgBodyLen( u16 wLen );   
	u8	*	GetSatMsgBody( void );     
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//定义MT拓扑结构
struct TSatMtTopo
{
protected:
	char m_achAlias[MAXLEN_ALIAS];	//本MT别名      
	u8 m_byMtId;					//MT号(1-127)
	u8 m_byMtConnected;
public:
	u32 m_dwMtModemIp;			
	u16 m_wMtModemPort;		
	u8 m_byMtModemType;
public:
	
	u32 GetModemIpAddr( )
	{
		return m_dwMtModemIp;
	}
	u16 GetModemPort( )
	{
		return m_wMtModemPort;
	}
	u8 GetModemType()
	{
		return m_byMtModemType;
	}
	void SetMtId( u8 byMtId );
	u8	 GetMtId( void );
	void SetConnected( u8 byConnect)
	{
		m_byMtConnected = byConnect;
	}
	u8   GetConnected()
	{
		return m_byMtConnected;
	}
	LPCSTR GetAlias( void );
	void SetAlias( LPCSTR achAlias );
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//容量
#define MAXNUM_TOPO_MCU     (u16)10
#define MAXNUM_TOPO_MT      (u16)1000


//终端类型
const u8	MTTYPE_RCVTRS		= 1;	//接收发送
const u8	MTTYPE_RCVONLY		= 2;	//只接收
const u8	MTTYPE_TRSONLY		= 3;	//只发送
const u8	MTTYPE_RCVTRSADAPT	= 4;	//接收发送（需适配）

//定义MCU拓扑结构
struct TMcuTopo
{
protected:
    u16	m_wMcuId;			//MCU号
    u16	m_wSuperiorMcuId;	//上级MCU号
    u32 m_dwIpAddr;			//本MCU的IP地址
    u32  m_dwGwIpAddr;		//本MCU的网关IP地址
    char m_achAlias[MAXLEN_ALIAS];	//本MCU别名
    
public:
    u16  GetMcuId( void ) const;
    void SetMcuId( u16 wMcuId );

    u16  GetSMcuId( void ) const;
    void SetSMcuId( u16 wSMcuId );

    u32  GetIpAddr( void ) const;
    void SetIpAddr( u32 dwIpAddr );

    u32  GetGwIpAddr( void ) const;
    void SetGwIpAddr( u32 dwGwIpAddr );

    LPCSTR GetAlias( void ) const;
    void   SetAlias( LPCSTR achAlias );
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


//定义MT拓扑结构
struct TMtTopo
{
public:
    u8 m_byMtId;			//MT号(1-127)
    u8 m_byPrimaryMtId;		//如果它本身Secondary MT，则是所连的MT号，否则是他自身的MT号
    u8 m_byMtType;			//1: 接收发送, 2: 只接受 3: 只发送
    u32 m_dwMtModemIp;			
    u16 m_wMtModemPort;
    u8  m_byMtModemType;
	u8  m_byMtConnected;     //是否直连终端
protected:
    u16	m_wMcuId;			//MT所在MCU号
    u32 m_dwIpAddr;			//本MT的IP地址
    u32 m_dwGwIpAddr;		//本MT所在的网关IP地址
    s8  m_achAlias[MAXLEN_ALIAS];	//本MT别名
	u8  m_byIsHD;			//本MT是否为高清MT

    
public:
    u16  GetMcuId( void ) const;
    void SetMcuId( u16 wMcuId );
    
    u32  GetIpAddr( void ) const;
    void SetIpAddr( u32 dwIpAddr );

    u32  GetGwIpAddr( void ) const;
    void SetGwIpAddr( u32 dwMtGwIpAddr );
    
    LPCSTR GetAlias( void ) const;
    void   SetAlias( LPCSTR achAlias );

    u8   GetMtId( void ) const;
    void SetMtId( u8 wMtId );

    u8   GetMtType( void ) const;
    void SetMtType( u8 byType );
    
    void SetConnected( u8 byConnect )
    {
        m_byMtConnected = byConnect >= 1 ? byConnect: 0;
    }
	BOOL GetConnected( void )
	{
		return m_byMtConnected == 1;
	}
	
	u32 GetModemIp( void ) const { return htonl(m_dwMtModemIp);	}
	u16 GetModemPort( void ) const { return htons(m_wMtModemPort);	}
	u8  GetModemType( void ) const { return m_byMtModemType;	}

	BOOL IsMtHd( void ) const { return m_byIsHD == 1 ? TRUE: FALSE;	};
	void SetMtHd( BOOL bHd ) { m_byIsHD = bHd ? 1: 0;	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;



/*====================================================================
函数名      : TMcuModemTopo
功能        ：
算法实现    ：
引用全局变量：
输入参数说明：
返回值说明  ：u8 byMtId
----------------------------------------------------------------------
修改记录    ：
日  期      版本        修改人        修改内容
03/11/26    1.0         zhangsh           创建
====================================================================*/
struct TMcuModemTopo
{
public:
    u8 m_byModemId; //编号
    u32 m_dwMcuModemIp;	//IP地址		
    u16 m_wMcuModemPort;//端口		
    u8 m_byMcuModemType;//类型
    u8 m_bySPortNum; //对应的S口号
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;


/*====================================================================
    函数名      ：GetMcuId
    功能        ：得到MCU号
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：u16 得到的MCU号
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/09/18    1.0         LI Yi         创建
====================================================================*/
inline u16 TMcuTopo::GetMcuId( void ) const
{
	return( ntohs( m_wMcuId ) );
}

/*====================================================================
    函数名      ：SetMcuId
    功能        ：设置MCU号
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wMcuId, MCU号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/09/18    1.0         LI Yi         创建
====================================================================*/
inline void TMcuTopo::SetMcuId( u16 wMcuId )
{
	m_wMcuId = htons( wMcuId );
}

/*====================================================================
    函数名      ：GetSMcuId
    功能        ：得到上级MCU号
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/09/18    1.0         LI Yi         创建
====================================================================*/
inline u16 TMcuTopo::GetSMcuId( void ) const
{
	return( ntohs( m_wSuperiorMcuId ) );
}

/*====================================================================
    函数名      ：SetSMcuId
    功能        ：设置上级MCU号
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wSMcuId, 上级MCU号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/09/18    1.0         LI Yi         创建
====================================================================*/
inline void TMcuTopo::SetSMcuId( u16 wSMcuId )
{
	m_wSuperiorMcuId = htons( wSMcuId );
}

/*====================================================================
    函数名      ：GetIpAddr
    功能        ：得到本MCU的IP地址
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：本MCU IP地址(网络序)
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人				修改内容
    02/10/24    1.0         LIAOWEIJIANG         创建
====================================================================*/
inline u32 TMcuTopo::GetIpAddr( void ) const
{
	return m_dwIpAddr;//网络序
}

/*====================================================================
    函数名      ：SetIpAddr
    功能        ：设置本MCU的IP地址
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 dwIpAddr, IP地址(网络序)
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人				修改内容
    02/10/24    1.0         LIAOWEIJIANG         创建
====================================================================*/
inline void TMcuTopo::SetIpAddr( u32 dwIpAddr )
{
	m_dwIpAddr = dwIpAddr;//网络序 
}

/*====================================================================
    函数名      ：GetGwIpAddr
    功能        ：得到本MCU的网关IP地址
    算法实现    ：
    引用全局变量：
    输入参数说明：void
    返回值说明  ：得到本MCU的网关IP地址(网络序)
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人				修改内容
    02/10/24    1.0         LIAOWEIJIANG         创建
====================================================================*/
inline u32 TMcuTopo::GetGwIpAddr( void ) const
{
	return m_dwGwIpAddr;//网络序
}

/*====================================================================
    函数名      ：SetGwIpAddr
    功能        ：设置本MCU的网关IP地址
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 dwGwIpAddr, 新的网关地址(网络序)
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人				修改内容
    02/10/24    1.0         LIAOWEIJIANG         创建
====================================================================*/
inline void TMcuTopo::SetGwIpAddr( u32 dwGwIpAddr )
{
	m_dwGwIpAddr = dwGwIpAddr;//网络序 
}

/*====================================================================
    函数名      ：GetAlias
    功能        ：得到MCU别名
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：得到的别名
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/24    1.0         LI Yi         创建
====================================================================*/
inline LPCSTR TMcuTopo::GetAlias( void ) const
{
	return( m_achAlias );
}

/*====================================================================
    函数名      ：SetAlias
    功能        ：设置MCU别名
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR lpszAlias, MCU别名
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/24    1.0         LI Yi         创建
====================================================================*/
inline void TMcuTopo::SetAlias( LPCSTR lpszAlias )
{
	strncpy( m_achAlias, lpszAlias, sizeof( m_achAlias ) );
	m_achAlias[sizeof( m_achAlias ) - 1] = '\0';
}

/*====================================================================
    函数名      ：GetMcuId
    功能        ：得到MCU号
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：u16 得到的MCU号
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/24    1.0         LI Yi         创建
====================================================================*/
inline u16 TMtTopo::GetMcuId( void ) const
{
	return( ntohs( m_wMcuId ) );
}

/*====================================================================
    函数名      ：SetMcuId
    功能        ：设置MCU号
    算法实现    ：
    引用全局变量：
    输入参数说明：u16 wMcuId, MCU号
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/24    1.0         LI Yi         创建
====================================================================*/
inline void TMtTopo::SetMcuId( u16 wMcuId )
{
	m_wMcuId = htons( wMcuId );
}

/*====================================================================
    函数名      ：GetIpAddr
    功能        ：得到本Mt的IP地址
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：得到的IP地址(网络序)
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/24    1.0         LI Yi         创建
====================================================================*/
inline u32 TMtTopo::GetIpAddr( void ) const
{
	return m_dwIpAddr;//网络序
}

/*====================================================================
    函数名      ：SetMtIpAddr
    功能        ：设置IP地址
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 dwIpAddr, 新的IP地址(网络序)
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/24    1.0         LI Yi         创建
====================================================================*/
inline void TMtTopo::SetIpAddr( u32 dwIpAddr )
{
	m_dwIpAddr = dwIpAddr;//网络序
}

/*====================================================================
    函数名      ：GetGwIpAddr
    功能        ：得到本Mt的网关IP地址
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：得到的IP地址(网络序)
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/24    1.0         LI Yi         创建
====================================================================*/
inline u32 TMtTopo::GetGwIpAddr( void ) const
{
	return m_dwGwIpAddr;//网络序
}

/*====================================================================
    函数名      ：SetGwIpAddr
    功能        ：设置该Mt的网关地址
    算法实现    ：
    引用全局变量：
    输入参数说明：u32 dwGwIpAddr, 新的网关IP地址(网络序)
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/24    1.0         LI Yi         创建
====================================================================*/
inline void TMtTopo::SetGwIpAddr( u32 dwGwIpAddr )
{
	m_dwGwIpAddr = dwGwIpAddr;//网络序
}

/*====================================================================
    函数名      ：GetAlias
    功能        ：得到MT别名
    算法实现    ：
    引用全局变量：
    输入参数说明：无
    返回值说明  ：得到的别名
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/24    1.0         LI Yi         创建
====================================================================*/
inline LPCSTR TMtTopo::GetAlias( void ) const
{
	return( m_achAlias );
}

/*====================================================================
    函数名      ：SetAlias
    功能        ：设置MCU号
    算法实现    ：
    引用全局变量：
    输入参数说明：LPCSTR lpszAlias, 别名字符串指针
    返回值说明  ：无
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/24    1.0         LI Yi         创建
====================================================================*/
inline void TMtTopo::SetAlias( LPCSTR lpszAlias )
{
	strncpy( m_achAlias, lpszAlias, sizeof( m_achAlias ) );
	m_achAlias[sizeof( m_achAlias ) - 1] = '\0';
}

inline void TMtTopo::SetMtId( u8 byMtId )
{
    m_byMtId = byMtId;
}

inline u8 TMtTopo::GetMtId( void ) const
{
    return m_byMtId;
}

inline void TMtTopo::SetMtType( u8 byType )
{
    m_byMtType = byType;
}

inline u8 TMtTopo::GetMtType( void ) const
{
    return m_byMtType;
}

inline CSatMsg::CSatMsg( void )
{
    memset( this, 0, sizeof( CSatMsg ) );
    m_dwMagic = htonl( SATMSG_MAGIC );
}

inline CSatMsg::CSatMsg( const u8* pContent, u16 wMsgLen )
{
    
    if( wMsgLen < SATMSG_HEAD_LEN || pContent == NULL )
        return;
    
    wMsgLen = min( wMsgLen, sizeof( m_abyBuffer ) + SATMSG_HEAD_LEN );
    memcpy( this, pContent, wMsgLen );
    //set length
    SetSatMsgBodyLen( wMsgLen - SATMSG_HEAD_LEN );
}

inline u16 CSatMsg::GetSatMsgHeadLen()
{
    return SATMSG_HEAD_LEN;
}

inline u16 CSatMsg::GetSatMsgLen()
{
    return ( GetSatMsgBodyLen() + SATMSG_HEAD_LEN );
}

inline  u8* CSatMsg::GetSatMsg()
{
    return( (  u8 * )( this ) );
}

inline u16 CSatMsg::GetSatMsgBodyLen()
{
    return ntohs( m_wBodyLen ); 
}

inline void CSatMsg::SetSatMsgBodyLen( u16 wLen )
{
    wLen = min( wLen, SATMSG_BODY_LEN );
    m_wBodyLen = htons( wLen );
}

inline u8 * CSatMsg::GetSatMsgBody()
{
    return ( u8 * )( m_abyBuffer  );
}

inline u8 CSatMsg::GetConfId()
{
    return m_byConfId;
}

inline void CSatMsg::SetConfId( u8 byConfId )
{
    m_byConfId = byConfId;
}

inline u16 CSatMsg::GetEventId()
{
    return ntohs( m_wEvent );
}

inline void CSatMsg::SetEventId( u16 wEvent )
{
    m_wEvent = htons( wEvent );
}



inline BOOL CSatMsg::IsValidSatMsg( )
{
    return( ntohl( m_dwMagic)  == SATMSG_MAGIC );
}


inline void CSatMsg::SetSatMsgBody( const u8* pContent, u16 wLen )
{
    wLen = min( wLen, SATMSG_BODY_LEN );
    memcpy( m_abyBuffer, pContent, wLen );
    SetSatMsgBodyLen( wLen );
}

inline void CSatMsg::CatSatMsgBody( const u8* pContent, u16 wLen )
{
    wLen = min( wLen, SATMSG_BODY_LEN - GetSatMsgBodyLen() );
    
    memcpy( m_abyBuffer + GetSatMsgBodyLen(), pContent, wLen );
    SetSatMsgBodyLen( GetSatMsgBodyLen() + wLen );
}





inline CSatConfInfo::CSatConfInfo()
{
	memset( this, 0, sizeof( CSatConfInfo ) );
}

inline void CSatConfInfo::SetConfId( u8 byConfId )
{
	m_byConfId = byConfId;
}

inline u8  CSatConfInfo::GetConfId()
{
	return m_byConfId;
}

inline void CSatConfInfo::SetChairmanId( u8 byMtId )
{
	m_byChairmanId  = byMtId;
}

inline u8 CSatConfInfo::GetChairmanId()
{
	return m_byChairmanId;
}

inline void CSatConfInfo::SetSpeakerId( u8 byMtId )
{
	m_bySpeakerId = byMtId;
}

inline u8 CSatConfInfo::GetSpeakerId()
{
	return m_bySpeakerId;
}

inline void CSatConfInfo::SetConfName( LPCSTR lpszConfName )
{
	if( lpszConfName != NULL )
	{
		strncpy( m_achConfName, lpszConfName, sizeof( m_achConfName ) );
		m_achConfName[sizeof( m_achConfName ) - 1] = '\0';
	}
	else
	{
		memset( m_achConfName, 0, sizeof( m_achConfName ) );
	}
}

inline LPCSTR CSatConfInfo::GetConfName()
{
	 return( ( LPCSTR )m_achConfName );
}

inline void CSatConfInfo::SetConfPassword( LPCSTR lpszConfPasswd )
{
	if( lpszConfPasswd != NULL )
	{
		strncpy( m_achPasswd, lpszConfPasswd, sizeof( m_achPasswd ) );
		m_achPasswd[sizeof( m_achPasswd ) - 1] = '\0';
	}
	else
	{
		memset( m_achPasswd, 0, sizeof( m_achPasswd ) );
	}
}

inline LPCSTR CSatConfInfo::GetConfPassword()
{
	 return( ( LPCSTR )m_achPasswd );
}

inline void CSatConfInfo::AddMt( u8 byMtId )
{
	m_tConfMtInfo.AddMt( byMtId );
}

inline void CSatConfInfo::RemoveMt( u8 byMtId )
{
	m_tConfMtInfo.RemoveMt( byMtId );
}

inline void CSatConfInfo::AddJoinedMt( u8 byMtId )
{
	m_tConfMtInfo.AddJoinedMt( byMtId );
}

inline void CSatConfInfo::RemoveJoinedMt( u8 byMtId )
{
	m_tConfMtInfo.RemoveJoinedMt( byMtId );
}

inline void CSatConfInfo::RemoveAllMt()
{
	m_tConfMtInfo.RemoveAllMt();
}

inline BOOL CSatConfInfo::IsMtInConf( u8 byMtId )
{
	return m_tConfMtInfo.MtInConf( byMtId );
}

inline BOOL CSatConfInfo::IsMtJoinedConf( u8 byMtId )
{
	return m_tConfMtInfo.MtJoinedConf( byMtId );
}

inline void CSatConfInfo::SetBrdMediaStartPort( u16 wPort )
{
	m_tBrdTransportAddr.SetPort(  wPort );
}

inline u16 CSatConfInfo::GetBrdMediaStartPort( )
{
	return ( m_tBrdTransportAddr.GetPort() );
}

inline void CSatConfInfo::SetBrdMeidaIpAddr( u32 dwIpAddress )
{
	m_tBrdTransportAddr.SetIpAddr( htonl( dwIpAddress ) );
}

inline u32 CSatConfInfo::GetBrdMeidaIpAddr()
{
	return ntohl( m_tBrdTransportAddr.GetIpAddr() );
}

inline void CSatConfInfo::SetConfMtInfo( const TConfMtInfo & tConfMtInfo )
{
	m_tConfMtInfo = tConfMtInfo;
}

inline const TConfMtInfo & CSatConfInfo::GetConfMtInfo()
{
	return m_tConfMtInfo;
}

inline  void CSatConfInfo::SetConfMtInfoMcuId( u8 byMcuId )
{
	m_tConfMtInfo.SetMcuId( byMcuId );
}

inline  u8 CSatConfInfo::GetConfMtInfoMcuId()
{
	return (u8)m_tConfMtInfo.GetMcuId();
}

inline void CSatConfInfo::SetSatIntervTime( u8 byTime )
{
	m_byIntervalTime = byTime;
	return ;
}

inline u8 CSatConfInfo::GetSatIntervTime()
{
	return m_byIntervalTime;
}

inline void CSatConfInfo::AddCurrUpLoadMt( u8 byMtId)
{
	if( byMtId > MAXNUM_CONF_MT - 1  )
	{
		OspLog( LOGLVL_EXCEPTION, "Mt Id out of range %d", byMtId );
		return ;
	}
	
	m_byUpLoadCurrDmt[byMtId / 8] |= 1 << ( byMtId % 8 );
}

inline void CSatConfInfo::DelCurrUpLoadMt( u8 byMtId)
{
	if( byMtId > MAXNUM_CONF_MT - 1  )
	{
		OspLog( LOGLVL_EXCEPTION, "Mt Id out of range %d", byMtId );
		return ;
	}
	
	m_byUpLoadCurrDmt[byMtId / 8] &= ~( 1 << ( byMtId % 8 ) );
}

inline u8 CSatConfInfo::IsMtInCurrUpLoad( u8 byMtId)
{
	if( byMtId > MAXNUM_CONF_MT - 1  )
	{
		OspLog( LOGLVL_EXCEPTION, "Mt Id out of range %d", byMtId );
		return FALSE;
	}
	return ( ( m_byUpLoadCurrDmt[byMtId / 8] & ( 1 << ( byMtId % 8 ) ) ) != 0 );
}



//消息封装
typedef struct tagITSatMessageHeader
{
    enum{
        emMCUID = 0x0000,
            emNMSID = 0xFFF0,
            emBROADCAST = 0xFFFF
    };
    protected:
        u16 m_wEventId; //消息ID
        u16 m_wMsgSN;   //消息流水号(1-65535)
        u8  m_byConfIdx; //会议索引(0xFF 表示向所有的会议广播消息)
        u16 m_wSrcId;   //消息发送者ID MCU应当天0 ,卫星网管系统应当天 0xFFF0 ,终端添相应的ID
        u16 m_wDstId;   //消息发送目的ID MCU应当天0 ,
        //卫星网管系统应当天 0xFFF0 ,终端添相应的ID
        //0xFFFF 表示广播消息 
        u8  m_byReserve;//保留字固定为零
    public:
        tagITSatMessageHeader()
        {
            m_wEventId = ntohs(0);
            m_wMsgSN   = ntohs(1);
            m_byConfIdx = 0xFF;
            m_wSrcId   = ntohs(emMCUID);
            m_wSrcId   = ntohs(emBROADCAST);
            m_byReserve = 0;
        }
        
        //获取消息ID
        u16 GetEventId()
        {
            return ntohs(m_wEventId);
        }
        //设置消息ID
        void SetEventId( u16 wEventId )
        {
            m_wEventId = htons(wEventId);
        }
        //获取消息流水号
        u16 GetMsgSN()
        {
            return ntohs(m_wMsgSN );
        }
        //设置消息流水号
        void SetMsgSN( u16 wMsgSN )
        {
            m_wMsgSN = ntohs(wMsgSN);
        }
        
        //获取会议索引号
        u8 GetConfIdx()
        {
            return m_byConfIdx;
        }
        //设置会议索引号
        void SetConfIdx( u8 byConfIdx)
        {
            m_byConfIdx = byConfIdx;
        }
        
        //获取消息发送源ID号
        u16 GetSrcId()
        {
            return ntohs(m_wSrcId);
        }
        //设置消息发送源ID号
        void SetSrcId( u16 wSrcId)
        {
            m_wSrcId = htons(wSrcId);
        }
        //获取消息发送目的ID号
        u16 GetDstId()
        {
            return ntohs(m_wDstId);
        }
        //设置消息发送目的ID号
        void SetDstId( u16 wDstId)
        {
            m_wDstId = htons(wDstId);
        }
}PACKED ITSatMsgHdr;

//终端信息
typedef struct tagTInterfaceMtTopo
{
	enum EMMtType
	{
		emFULL    = 1,//全双工 直连终端
			emUPALLOC = 2,//回传通道分配（卫星终端）
	};
	enum EMModemType
	{
		emMODEM_70M_LBAND     = 1,            
			emMODEM_LBAND_70M     = 2,            
			emMODEM_LBAND_LBAND   = 3 
	};
	tagTInterfaceMtTopo(){memset(this,0,sizeof(tagTInterfaceMtTopo));}
protected:
	u16 m_wMtId;
	u8  m_byConnectType;
	char m_achAlias[MAXLEN_SATMTALIAS];
	u32 m_dwMtIp;
	u32  m_dwModemIp;
	u16  m_wModemPort;
	u8   m_byModemType;
	
	
	
public:
	void SetMtId( u16 wMtId )
	{
		m_wMtId = htons(wMtId);
	}
	u16 GetMtId(void)
	{
		return ntohs(m_wMtId);
	}
	
	void SetMtConnectType( u8 byMode )
	{
		m_byConnectType = byMode;
	}
	u8 GetMtConnectType(void)
	{
		return m_byConnectType;
	}
	
	void SetMtModemIp( u32 dwIp )
	{
		m_dwModemIp = dwIp;
	}
	u32 GetMtModemIp( void )
	{
		return m_dwModemIp;
	}
	
	void SetMtModemPort( u16 wPort )
	{
		m_wModemPort = htons(wPort);
	}
	u16 GetMtModemPort(void)
	{
		return ntohs(m_wModemPort);
	}
	
	void SetMtModemType( u8 byType )
	{
		m_byModemType = byType;
	}
	u8 GetMtModemType()
	{
		return m_byModemType;
	}
	
	void SetMtAlias( LPCSTR lpszAlias )
	{
		if( lpszAlias != NULL )
		{
			strncpy( m_achAlias, lpszAlias, sizeof( m_achAlias ) );
			m_achAlias[sizeof( m_achAlias ) - 1] = '\0';
		}
		else
		{
			memset( m_achAlias, 0, sizeof( m_achAlias ) );
		}
	}
	char* GetMtAlias()
	{
		return m_achAlias;
	}
	
	void SetMtIp( u32 dwIp )
	{
		m_dwMtIp = dwIp;
	}
	
	u32 GetMtIp()
	{
		return m_dwMtIp;
	}
}PACKED TInterfaceMtTopo ,ITSatMtInfo;


typedef struct tagTSatBitrate
{
    
    tagTSatBitrate(){memset(this,0,sizeof(tagTSatBitrate));	}
    protected:
        u8 m_byBitrate;
    public:
        //获取码率 (单位:Kbps)
        u16 Get()
        {
            return m_byBitrate*32;
        }
        
        //设置码率 (单位:Kbps )
        void Set( u16 wBitrate )
        {
            if( wBitrate >= 255*32 )
            {
                m_byBitrate = 255;
            }
            else
            {
                m_byBitrate = (u8)(wBitrate/32);
            }
        }
}PACKED ITSatBitrate;


typedef struct tagTInterfaceList
{
    tagTInterfaceList()
    {
        memset(this,0,sizeof(tagTInterfaceList));
    }
public:
    u8 m_abyMemberList[MAXNUM_SATMT/8];
public:
    void SetMember( u16 wWhoId )
    {
        if( wWhoId > MAXNUM_SATMT || wWhoId==0 )
        {
            OspPrintf( TRUE,FALSE, "Mt Id out of range %d", wWhoId );
            return ;
        }
        wWhoId -=1;
        m_abyMemberList[wWhoId / 8] |= 1 << ( wWhoId % 8 );
        return;
    }
    void RemoveMember( u16 wWhoId )
    {
        if( wWhoId > MAXNUM_SATMT || wWhoId==0 )
        {
            OspPrintf(TRUE,FALSE,"Mt Id out of range %d", wWhoId );
            return ;
        }
        wWhoId -=1;
        m_abyMemberList[wWhoId / 8] &= ~( 1 << ( wWhoId % 8 ) );
        return;
    }
    
    BOOL IsInList( u16 wWhoId ) const
    {
        if( wWhoId > MAXNUM_SATMT || wWhoId==0 )
        {
            OspPrintf(TRUE,FALSE,"Mt Id out of range %d", wWhoId );
            return FALSE;
        }
        wWhoId -=1;
        return ( ( m_abyMemberList[wWhoId / 8] & ( 1 << ( wWhoId % 8 ) ) ) != 0 );
    }
}PACKED TInterfaceList ,ITSatMtList;


typedef TInterfaceList TMtList;

//定义画面复合
typedef struct tagTVMPParam
{
protected:
    u8      m_byVmpStyle;			//合成风格
    u8		m_byBrdcst;			//是否广播
    u16    	m_wMt[16];        		//成员ID
public:
    inline u8 GetVmpStyle( void ) const
    {
        return( m_byVmpStyle );
    }
    
    inline void SetVmpStyle( u8 byStyle )
    {
        m_byVmpStyle = byStyle;
    }
    
    inline void SetVmpBrd( BOOL bBroad )
    {
        if ( bBroad == TRUE )
            m_byBrdcst = 1;
        else
            m_byBrdcst = 0;
    }
    
    inline BOOL IsVmpBrd( ) const
    {
        return ( m_byBrdcst == 1 );
    }
    
    inline void SetMtId( u8 byPos , u16 wMtId )
    {
        if( byPos >16 )
            return;
        m_wMt[ byPos-1] = htons( wMtId );
    }
    
    inline u16 GetMtId( u8 byPos ) const
    {
        if ( byPos == 0 || byPos > 16 )
            return 0;
        return ntohs( m_wMt[byPos-1] );
    }
    
}PACKED ITSatVmpParam;


typedef struct tagTInterfaceConfInfo
{
	tagTInterfaceConfInfo()
	{
		memset(this,0,sizeof(tagTInterfaceConfInfo));
	}

protected:
	u16 m_wSpeakerId;
	u16 m_wChairId;
	u8  m_byLineNum;
	u8  m_bySwitchTime;
	char m_achName[32];
	u8  m_byAudioType;
	u8  m_byVideoType;   //视频频类型
	u8  m_byVidResolution;//视频分辨率
    ITSatBitrate m_tConfBitrate;
	/*--------------------------------*/
	u8  m_byState;//混音，
	/* 0x 0 0 0 0 0 x x x
		      |    | |_____混音状态
		      |    |_______画面复合状态
		      |____________加密信息
	*/
	u8  m_achKey[9];
public:
	TMtList m_tNonActiveMt;//受邀
	TMtList m_tActiveMt;//签到
	TMtList m_tCanSendMt;//可发送，上传(直连和占上传通路)
	
	TMtList m_tNearMuteMt; //静音
	TMtList m_tFarMuteMt; //哑音
	ITSatVmpParam m_tVmpMt; //画面复合

public:
	//获取会议视频格式
	u8 GetVideoType()
	{
		return m_byVideoType;
	}
	//设置会议视频格式
	void SetVideoType( u8 byType)
	{
		m_byVideoType = byType;
	}
	//获取会议视频分辨率
	u8 GetResolution()
	{
		return m_byVidResolution;
	}
	//设置会议视频分辨率
	void SetResolution( u8 byResolution)
	{
		m_byVidResolution = byResolution;
	}

	//设置会议音频分辨率
	void SetAudioType( u8 byType )
	{
		m_byAudioType = byType;
	}

	//设置会议音频分辨率
	u8 GetAudioType()
	{
		return m_byAudioType;
	}
	
	//设置会议码率 (单位64K)
	void SetBitrate( u16 wBitrate )
	{
		m_tConfBitrate.Set( wBitrate );
	}
	
	//获取会议会议码率 (单位64K)
	u16 GetBitrate()
	{
		return m_tConfBitrate.Get();
	}


	//发言人
	void SetSpeakerId( u16 wSpeakerId )
	{
		m_wSpeakerId = htons(wSpeakerId);
	}
	u16 GetSpeakerId(void)
	{
		return ntohs(m_wSpeakerId);
	}
	//主席
	void SetChairId( u16 wChairId )
	{
		m_wChairId = htons(wChairId);
	}
	u16 GetChairId(void)
	{
		return ntohs(m_wChairId);
	}
	//混音状态
	void SetMixing(void)
	{
		m_byState |= 1;
	}
	void SetNoMixing(void)
	{
		m_byState &=~1;
	}
	BOOL IsMixing(void)
	{
		return ( (m_byState & 1 ) != 0 );
	}
	//画面复合状态
	void SetVmping(void)
	{
		m_byState |= 0x2;
	}
	void SetNoVmping(void)
	{
		m_byState &= ~0x2;
	}
	BOOL IsVmping(void)
	{
		return ( (m_byState & 0x2 ) != 0 );
	}
	//画面复合广播状态
/*	void SetVmpBrdst(void)
	{
		m_byState |= 0x4;
	}
	void SetVmpNoBrdst(void)
	{
		m_byState &= ~0x4;
	}
	BOOL IsVmpBrdst(void)
	{
		return ( (m_byState & 0x4 ) != 0 );
	}*/
	/*会议是否加密,终端创建会议要进行设置
	*/
	void SetConfEncrypted( BOOL bEncrypt )
	{
		if ( bEncrypt )
			m_byState |= 0x08;
		else
			m_byState &= ~0x08;
	}
	
	BOOL IsConfEncrypted()
	{
		return ( (m_byState & 0x08 ) != 0 );
	}
	/*会议密码，终端创建会议时填写*/
	void SetConfKey( u8* buf, u8 len)
	{
		memcpy( m_achKey,buf,len);
		m_achKey[8] = '\0';
	}
	u8* GetConfKey( )
	{
		return m_achKey;
	}
	//会议线路
	void SetConfLineNum( u8 byNum )
	{
		m_byLineNum = byNum;
	}
	u8 GetConfLineNum(void) const
	{
		return m_byLineNum;
	}
	//切换间隔
	void SetConfSwitchTime( u8 byTime )
	{
		m_bySwitchTime = byTime;	
	}
	u8 GetConfSwitchTime(void) const
	{
		return m_bySwitchTime;
	}
	LPCSTR GetConfName( void ) const
	{
		return( ( LPCSTR )m_achName );
	}

	void SetConfName( LPCSTR lpszBuf )
	{
		strncpy( m_achName, lpszBuf, sizeof( m_achName ) );
		m_achName[ sizeof( m_achName ) - 1] = '\0';
	}
	
}PACKED TInterfaceConfInfo ,ITSatConfInfo;


/*====================================================================
    函数名      : TSchConfInfoHeader
    功能        ：
    算法实现    ：
    引用全局变量：
    输入参数说明：
    返回值说明  
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/11/26    1.0         zhangsh           创建
====================================================================*/
struct TSchConfInfoHeader
{
protected:
	u16 wMessageType;   //命令字
	u32 dwSerialId;     //流水号
	u8  chConfId[16];   //会议号
	u8  chPass[12];     //用户名
	u32 dwStartTime;    //开始时间
	u32 dwEndTime;      //结束时间
	u16 wMtNum;         //参会终端个数
public:
	void SetMessageType( u16 wCmd )
	{
		wMessageType = htons( wCmd );
	}

	u16 GetMessageType( )
	{
		return ntohs( wMessageType );
	}
	
	void SetSerialId( u32 dwSN )
	{
		dwSerialId = htonl( dwSN );
	}
	u32 GetSerialId( )
	{
		return ntohl( dwSerialId );
	}
	void SetStartTime( u32 dwTime )
	{
		dwStartTime = htonl( dwTime );
	}
	
	void SetEndTime( u32 dwTime )
	{
		dwEndTime = htonl( dwTime );
	}
	
	void SetMtNum( u16 wMt )
	{
		wMtNum = htons( wMt );
	}
	void SetConfId( CConfId cConfId )
	{
		u8 byConf[16];
		memset( byConf, 0 , 16 );
		cConfId.GetConfId( byConf, 16 );
		
        for ( u8 byLoop = 0; byLoop < 16 ; byLoop ++ )
        {
			chConfId[byLoop] = byConf[byLoop];
        }
	}
	void SetPassWord( u8* pPassWord )
	{
		if ( pPassWord == NULL )
        {
			return;
        }
		memcpy( chPass, pPassWord, 12 );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//预约会议信息(NMS->MCU)
struct TReSchConfNotif
{
	u16 wMessageType;   //命令字
	u32 dwSerialId;     //流水号
	u8  chConfId[16];
	u16 wMtNum;
public:
	u16 GetMessageType( )
	{
		return ntohs( wMessageType );
	}
	
	u32 GetSerialId( )
	{
		return ntohl( dwSerialId );
	}
	
	u16 GetMtNum( )
	{
		return ntohs( wMtNum );
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
struct TSchConfNotif
{
protected:
	u16 wMessageType;   //命令字
	u32 dwSerialId;     //流水号
	u8  chConfId[16];
	u8  byMcsCreated;   //是否允许创建
	u16 wMtNum;
public:
	u16 GetMessageType( )
	{
		return ntohs( wMessageType );
	}
	
	u32 GetSerialId( )
	{
		return ntohl( dwSerialId );
	}
	
	u16 GetMtNum( )
	{
		return ntohs( wMtNum );
	}
	u8 GetMcs()
	{
		return byMcsCreated;
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;
//终端添加和删除(MCU->NMS)
struct TMtOperateToNms
{
protected:
	u16 wMessageType;   //命令字
	u32 dwSerialId;     //流水号
	u8  chConfId[16];
	u16 wMtNum;
public:
	void SetMessageType( u16 wCmd )
	{
		wMessageType = htons( wCmd );
	}
	void SetSerialId( u32 dwSN )
	{
		dwSerialId = htonl( dwSN );
	}
	void SetMtNum( u16 wMt )
	{
		wMtNum = htons( wMt );
	}
	void SetConfId( CConfId cConfId )
	{
		u8 byConf[16];
		memset( byConf, 0 , 16 );
		cConfId.GetConfId( byConf, 16 );
		for ( u8 byLoop = 0; byLoop < 16 ; byLoop ++ )
			chConfId[byLoop] = byConf[byLoop];
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

//终端添加和删除的通知(NMS->MCU)
struct TMtOperateNotif
{
protected:
	u16 wMessageType;   //命令字
	u32 dwSerialId;     //流水号
	u8  chConfId[16];
	u16 wMtNum;
public:
	u16 GetMessageType( )
	{
		return ntohs( wMessageType );
	}
	u32 GetSerialId( )
	{
		return ntohl( dwSerialId );
	}
	u32 GetMtNum()
	{
		return ntohs(wMtNum);
	}
}
#ifndef WIN32
__attribute__ ( (packed) ) 
#endif
;

typedef struct tagTSatMtId
{
protected:
    u16 m_wMtId;
public:
    
    void Set( u16 wMtId )
    {
        m_wMtId = htons( wMtId );
    }
    u16 Get(void)
    {
        return ntohs( m_wMtId );
    }
}PACKED ITSatMtId;



typedef struct tagTInterfaceMtAddr
{
    tagTInterfaceMtAddr(){memset(this,0,sizeof(tagTInterfaceMtAddr));}
protected:
    u32 m_dwIpAddr;
    u16 m_wPort;
public:
    void SetIpAddr( u32 dwIp )
    {
        m_dwIpAddr =  htonl(dwIp);
    }
    u32 GetIpAddr(void)
    {
        return ntohl(m_dwIpAddr);
    }
    
    void SetPort( u16 wPort )
    {
        m_wPort = htons( wPort );
    }
    u16 GetPort(void)
    {
        return ntohs( m_wPort );
    }
}PACKED TInterfaceMtAddr,ITSatTransAddr;


//卫星会议地址信息
typedef struct tagTSatConfAddrInfo
{
    tagTSatConfAddrInfo(){ memset( this,0,sizeof(tagTSatConfAddrInfo));}
public:
    ITSatMtId      tAudMulticastSrcId;// 音频组播码流视频源ID
    ITSatMtId      tVidMulticastSrcId;// 视频组播码流视频源ID
    ITSatMtId      tAudUnitcastSrcId; // 音频单播码流视频源ID
    ITSatMtId      tVidUnitcastSrcId; // 视频单播码流视频源ID
    
    ITSatTransAddr tMcuRcvMediaAddr;     // MCU接收码流(交换板)地址 ,视频起始端口号
    ITSatTransAddr tAudMulticastAddr;    // 音频组播地址信息 
    ITSatTransAddr tVidMulticastAddr;    // 视频组播地址信息
	
	//ITSatTransAddr tSecVidMulticastAddr;    // 视频第二组播地址信息

    ITSatTransAddr tAudUnitcastAddr;     // 视频单播地址信息-该地址仅进端口号有效
    ITSatTransAddr tVidUnitcastAddr;     // 视频单播地址信息-该地址仅进端口号有效
    
    ITSatMtList    tAudMulticastMmbList; // 音频组播接收成员列表 
    ITSatMtList    tVidMulticastMmbList; // 视频组播接收成员列表 
	
	//ITSatMtList    tSecVidMulticastMmbList; // 视频第二组播接收成员列表 

    ITSatMtList    tAudUnitcastMmbList;  // 音频单播接收成员列表 
    ITSatMtList    tVidUnitcastMmbList;  // 视频单播接收成员列表
}PACKED ITSatConfAddrInfo;


//卫星会议地址信息: 高清终端使用
typedef struct tagTSatConfAddrInfoHd
{
    tagTSatConfAddrInfoHd(){ memset( this,0,sizeof(tagTSatConfAddrInfoHd));}

public:
    ITSatMtId      tAudMulticastSrcId;	// 音频组播码流视频源ID
    ITSatMtId      tVidMulticastSrcId;	// 视频组播码流视频源ID

    ITSatMtId      tAudUnitcastSrcId;	// 音频单播码流视频源ID
    ITSatMtId      tVidUnitcastSrcId;	// 视频单播码流视频源ID
    
    ITSatTransAddr tMcuRcvMediaAddr;    // MCU接收码流(交换板)地址 ,视频起始端口号

    ITSatTransAddr tAudMulticastAddr;   // 音频组播地址信息 
    ITSatTransAddr tVidMulticastAddr;   // 视频组播地址信息
	
    ITSatTransAddr tAudUnitcastAddr;    // 视频单播地址信息-该地址仅进端口号有效
    ITSatTransAddr tVidUnitcastAddr;    // 视频单播地址信息-该地址仅进端口号有效
    
    ITSatMtList    tAudMulticastMmbList;// 音频组播接收成员列表 
    ITSatMtList    tVidMulticastMmbList;// 视频组播接收成员列表 
	
    ITSatMtList    tAudUnitcastMmbList; // 音频单播接收成员列表 
    ITSatMtList    tVidUnitcastMmbList; // 视频单播接收成员列表


	//双流支持（暂不考虑双流单播，即双流选看）
	ITSatMtId      tDSMulticastSrcId;	// 双流组播码流视频源ID
	ITSatTransAddr tDSMulticastAddr;	// 双流组播地址信息
	ITSatMtList    tDSMulticastMmbList; // 双流组播接收成员列表 
}PACKED ITSatConfAddrInfoHd;


typedef struct tagTInterfaceMtKey
{
	tagTInterfaceMtKey()
	{
		memset(this,0,sizeof(tagTInterfaceMtKey));
	}
protected:
	u16 m_wMtId;
	char szDesKey[16];
public:
	void SetMtKey( u16 MtId,  char* key )
	{
		m_wMtId = htons( MtId );
		for ( u8 byLoop = 0 ; byLoop < 16 ; byLoop ++)
			szDesKey[byLoop] = key[byLoop];
	}
	//可能不含结束符，请直接拷贝16个字节
	char* GetMtKey()
	{
		return szDesKey;
	}
	//获取密钥的MT ID
	u16 GetMtId()
	{
		return ntohs(m_wMtId);
	}
	//设置密钥的MT ID
	void SetMtId( u16 wMtId )
	{
		m_wMtId = ntohs(wMtId);
	}
}PACKED TInterfaceMtKey,ITSatMtKey;


//定义短消息结构
typedef struct tagTMultiPackHeader
{
protected:
    u8          m_byTotalNum;		//包的总个数
    u8        m_byCurrNum;	//当前包的序号
    u8        m_byContentNum;   //当前包内容个数
public:
	inline u8 GetTotalNum( void ) const
	{
		return m_byTotalNum;
	}
	
	inline void SetTotalNum( u8 byNum )
	{
		m_byTotalNum = byNum;
	}
	
	inline u8 GetCurrNum( void ) const
	{
		return m_byCurrNum;
	}
	
	inline void SetCurrNum( u8 byNum )
	{
		m_byCurrNum = byNum;
	}
	
	inline u8 GetContentNum( void ) const
	{
		return m_byContentNum;
	}
	
	inline void SetContentNum( u8 byNum )
	{
		m_byContentNum = byNum;
	}
}PACKED ITSatMultiPackHeader;

#ifdef WIN32
#pragma pack( pop )
#endif


#endif

//END OF FILE

