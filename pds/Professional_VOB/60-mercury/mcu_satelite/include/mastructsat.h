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

#define  MAXLEN_CONF_NAME           (u8)32
#define  MAXLEN_CONF_PASSWD         (u8)32
#define	 MAXNUM_SATMULTICAST        (u8)5       //卫星线路最大组播的限制
#define  MCU_SENDTOMULTI_PORT       (u16)5000
#define  MT_RCVMCUMEDIA_PORT        (u16)4000	//终端接受MCU数据的起始端口


//与终端交互的卫星MCU会议信息结构
struct CSatConfInfo
{
protected:
	u8	m_byConfId;	//在MCU中，该会议的会议索引号，对应每一个会议均不相同，可以
					//作为区分会议的会议号
	u8  m_bySpeakerId; //当前发言人的终端ID号，为零表示当前没有发言人。
	u8  m_byChairmanId;//当前主席的终端ID号，为零表示当前没有主席。

	TTransportAddr m_tBrdTransportAddr; //发言人码流的组播地址
	TConfMtInfo m_tConfMtInfo;//与会终端状态
	TTransportAddr m_tMpTransportAddr; //终端发送码流的地址

	u8	m_byState;       //bit0: 0:未画面复合 1:画面复合
							//bit1-2: 00:非混音 01:混音 10:混音暂停
							//bit3-4: 00:非录像 01:录像 10:录像暂停
							//bit5-7: 000:非放像 001:放像 010:放像暂停 011:快进 100:快退		
	char m_achConfName[MAXLEN_CONF_NAME];//会议名
	char m_achPasswd[MAXLEN_CONF_PASSWD];//会议密码	
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

////与终端交互的卫星MCU终端信息结构
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
	u8	m_byMtId; //终端ID号
	u8  m_byConfId;//该终端所在会议的ID号，为0表示终端不在任何一个会议中
	u8  m_byMediaSrc;//该终端当前接受的媒体源；可以为Speaker或者所选看的终端
	TBitRate m_tSendFlowRate;//终端发送码流的速率(如果为0表示终端不需要发送)，单位kbps.
	u8  m_byState;	// 0位: 是否需要处于轮循状态( 0:不需要轮循；1：需要轮循）。
					// 1位：是否处于混音状态(0: 终端不在混音；1: 终端在混音）。
	//Added for Version 2
	u8  m_byCameraCmd; //控制终端摄像头命令
	u8  m_byCameraVal; //摄像头控制命令的参数
	u8	m_byVoiceState;	   //控制终端声音
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
    
	BOOL	IsValidSatMsg( void );	//判断接受的消息是否合法，主要是校验幻数
	
	void	SetEventId( u16 wEvent ); //设置事件ID号
	u16		GetEventId( void );

	void	SetConfId( u8 byConfId ); //设置会议号
	u8		GetConfId( void );
    
	u16		GetSatMsgHeadLen( void );

	u16		GetSatMsgLen( void );		//获取整个消息长度
    u8	*	GetSatMsg( void );

	void	SetSatMsgBody( const u8* pContent, u16 wLen );
	void	CatSatMsgBody( const u8* pContent, u16 wLen );
    
    u16		GetSatMsgBodyLen( void );		//获取消息缓冲区长度
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
    u8 m_byMtConnected;     //是否直连终端
    u32 m_dwMtModemIp;			
    u16 m_wMtModemPort;		
    u8 m_byMtModemType;
protected:
    u16	m_wMcuId;			//MT所在MCU号
    u32 m_dwIpAddr;			//本MT的IP地址
    u32 m_dwGwIpAddr;		//本MT所在的网关IP地址
    char m_achAlias[MAXLEN_ALIAS];	//本MT别名 
    
    
public:
    u16 GetMcuId( void ) const;
    void SetMcuId( u16 wMcuId );
    
    u32 GetIpAddr( void ) const;
    void SetIpAddr( u32 dwIpAddr );

    u32 GetGwIpAddr( void ) const;
    void SetGwIpAddr( u32 dwMtGwIpAddr );
    
    LPCSTR GetAlias( void ) const;
    void SetAlias( LPCSTR achAlias );

    u8 GetMtId( void ) const;
    void SetMtId( u8 wMtId );

    u8 GetMtType( void ) const;
    void SetMtType( u8 byType );
    
    void SetConnected( u8 byConnect )
    {
        m_byMtConnected = byConnect;
    }
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
	if( byMtId > MAXNUM_MCU_MT - 1  )
	{
		OspLog( LOGLVL_EXCEPTION, "Mt Id out of range %d", byMtId );
		return ;
	}
	
	m_byUpLoadCurrDmt[byMtId / 8] |= 1 << ( byMtId % 8 );
}

inline void CSatConfInfo::DelCurrUpLoadMt( u8 byMtId)
{
	if( byMtId > MAXNUM_MCU_MT - 1  )
	{
		OspLog( LOGLVL_EXCEPTION, "Mt Id out of range %d", byMtId );
		return ;
	}
	
	m_byUpLoadCurrDmt[byMtId / 8] &= ~( 1 << ( byMtId % 8 ) );
}

inline u8 CSatConfInfo::IsMtInCurrUpLoad( u8 byMtId)
{
	if( byMtId > MAXNUM_MCU_MT - 1  )
	{
		OspLog( LOGLVL_EXCEPTION, "Mt Id out of range %d", byMtId );
		return FALSE;
	}
	return ( ( m_byUpLoadCurrDmt[byMtId / 8] & ( 1 << ( byMtId % 8 ) ) ) != 0 );
}

#endif

//END OF FILE