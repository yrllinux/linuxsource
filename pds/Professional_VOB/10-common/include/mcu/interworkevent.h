#ifndef _h_interworkevent_h__
#define _h_interworkevent_h__

//#include "umscommstruct.h"

#define SIPREG_LISTEN_PORT               6000
#define AID_REGINTERWORK_APP             103
#define AID_GKINTERWORK_APP              227
#define SIP_MAX_ALIAS_LEN		         64		//别名最大长度
#define SIP_MAX_ALIAS_NUM                128
#define SIP_MAX_NUM_CONTACT_ADDR         5		//注册给注册服务器的接入地址数量
#define SIP_MAX_NUM_CALLID		        128	    //SIP协议栈回调的CALLID最大长度

#define H323_MAX_ALIAS_NUM              128
#define H323_MAX_NUM_CONTACT_ADDR         8		//注册给注册服务器的接入地址数量
#define H323_MAX_NUM_EPID		        256	    //SIP协议栈回调的CALLID最大长度

#define LEN_H323ALIAS					(u16)128				    //最大别名长度h323-id


enum  EmRegInterEvent
{
	ev_interwork_start_con,
	ev_interwork_reqallreg,
	ev_interwork_con_ack,
	ev_interwork_reg,  
	ev_interwork_unreg,

	ev_inter_gk_start_con,
	ev_inter_gk_reqallreg,
	ev_inter_gk_con_ack,
	ev_inter_gk_reg,
	ev_inter_gk_unreg,
};

enum EmSipRegAliasType
{
	    SipReg_Alias_Unknow,
		SipReg_Alias_e164,
		SipReg_Alias_h323,
		SipReg_Alias_IP,
};

//别名类型
enum EmAliasType
{
	gk_reg_aliastype,
		gk_reg_e164 = 0x1,
		gk_reg_h323ID,
		gk_reg_email,
		gk_reg_epID,
		gk_reg_GKID,
		gk_reg_partyNumber,
		gk_reg_others
};

typedef struct tagTSipRegAlias
{
	EmSipRegAliasType	m_byType;
	s8				m_abyAlias[SIP_MAX_ALIAS_LEN + 1];
	tagTSipRegAlias()
	{
		Clear();
	}
	void Clear()
	{
		m_byType = SipReg_Alias_Unknow;
		m_abyAlias[0]	= '\0';
	}
	
	BOOL32 IsInValid()
	{
		return m_byType == SipReg_Alias_Unknow || strlen(m_abyAlias) == 0;
	}
	
	void SetAlias(EmSipRegAliasType type, const s8* alias)
	{
		m_byType = type;
		strncpy(m_abyAlias, alias, SIP_MAX_ALIAS_LEN);
		m_abyAlias[SIP_MAX_ALIAS_LEN] = '\0';
	}
	
	void SetAlias(const s8* alias)
	{
		m_byType = SipReg_Alias_h323;
		strncpy(m_abyAlias, alias, SIP_MAX_ALIAS_LEN);
		m_abyAlias[SIP_MAX_ALIAS_LEN] = '\0';
	}
	
	BOOL32 operator == (const tagTSipRegAlias& tRhs)
	{
		return (0 == strcmp(m_abyAlias, tRhs.m_abyAlias));
	}
	
	tagTSipRegAlias& operator = (const tagTSipRegAlias& tRhs)
	{
		if (this == &tRhs)
		{
			return *this;
		}
		m_byType = tRhs.m_byType;
		strncpy(m_abyAlias, tRhs.m_abyAlias, sizeof(m_abyAlias));
		return *this;
	}

	void HostToNet()
	{
		m_byType = (EmSipRegAliasType)(htonl((u32)m_byType));
	}
	void NetToHost()
	{
		m_byType =(EmSipRegAliasType)(ntohl((u32)m_byType));
	}

}TSipRegAlias, *PTSipRegAlias;


typedef struct tagTSipTransAddr
{
public:
	u32	m_dwIP;		//网络序
	u16 m_wPort;	//主机序
	
public:
	tagTSipTransAddr()
		:m_dwIP(0)
		,m_wPort(0)
	{
	}
	
	u32 GetIP()	{ return m_dwIP; }
	u16 GetPort()	{ return m_wPort; }	
	
	void SetAddr(u32 dwIP, u16 wPort)
	{
		m_dwIP = dwIP;
		m_wPort = wPort;
	}
	void Clear()
	{
		m_wPort = 0;
		m_dwIP = 0;
	}
	tagTSipTransAddr& operator = (const tagTSipTransAddr& tRhs)
	{
		if (this == &tRhs)
		{
			return *this;
		}
		this->m_dwIP = tRhs.m_dwIP;
		this->m_wPort = tRhs.m_wPort;
		return *this;
	}
    void HostToNet()
	{
	   m_dwIP = htonl(m_dwIP);
	   m_wPort = htons(m_wPort);
	}
	void NetToHost()
	{
		 m_dwIP=ntohl(m_dwIP);
		 m_wPort=ntohs(m_wPort);
	}

}TSipTransAddr, *PTSipTransAddr;

typedef struct tagTSipRegInterwork 
{
   
	u16				m_wAliasNum;
	TSipRegAlias	m_atAlias[SIP_MAX_ALIAS_NUM];
	u16             m_wAddrNum;
	TSipTransAddr	m_atCalledAddr[SIP_MAX_NUM_CONTACT_ADDR];	//呼叫地址
	
	s8				m_aSipCallId[SIP_MAX_NUM_CALLID];    //呼叫CallID, 表识一条注册
	u32				m_dwExpires;//超时时间

     tagTSipRegInterwork()
	 {
		 memset(this, 0, sizeof(tagTSipRegInterwork));
	 }
     void HostToNet()	
	 {
		u16 wIndex=0;
		for( wIndex=0; wIndex <m_wAliasNum; wIndex++)
		{
			m_atAlias[wIndex].HostToNet();
		}
		for( wIndex=0;wIndex< m_wAddrNum; wIndex++)
		{
			m_atCalledAddr[wIndex].HostToNet();
		}
		m_wAliasNum = htons(m_wAliasNum) ;
		m_wAddrNum = htons(m_wAddrNum);
		m_dwExpires = htonl(m_dwExpires);
	 }
	 void NetToHost()
	 {
		u16 wIndex =0;

		m_wAliasNum = ntohs(m_wAliasNum);
		m_wAddrNum = ntohs(m_wAddrNum);
		m_dwExpires = ntohl(m_dwExpires);
 
		for(wIndex=0; wIndex <m_wAliasNum; wIndex++)
		{
			m_atAlias[wIndex].NetToHost();
		}
		for(wIndex=0;wIndex< m_wAddrNum; wIndex++)
		{
			m_atCalledAddr[wIndex].NetToHost();
		}
		
	 }
}TSipRegInterwork;

//transport地址
typedef struct tagInterNetAddr
{
private:
    u32 m_dwIP;
    u16 m_wPort;
public:
    void SetNetAddr(u32 dwIP, u16 wPort)
    {
        m_dwIP = dwIP;
        m_wPort = wPort;
    }
	
    void SetIPAddr(u32 dwIP)
    {
        m_dwIP = dwIP;
    }
    u32 GetIPAddr()
    {
        return m_dwIP;
    }
	
    void SetIPPort(u16 wPort)
    {
        m_wPort = htons(wPort);
    }
    u16 GetIPPort()
    {
        return m_wPort;
    }
	
	BOOL operator==(tagInterNetAddr& tAddr)
	{
		if ( GetIPAddr() == tAddr.GetIPAddr()
			&& GetIPPort() == tAddr.GetIPPort() )
		{
			return TRUE;
		}
		
		return FALSE;
	}
	
    tagInterNetAddr()
    {
        m_dwIP  = 0;
        m_wPort = 0;
    }

	void HostToNet()
	{
		m_wPort = htons( m_wPort );
	}
	void NetToHost()
	{
		m_wPort = ntohs( m_wPort );
	}

}TINTERNETADDR,*PTINTERNETADDR;

//别名地址
typedef struct tagH323ALIASADDR
{
	EmAliasType  m_emType;
	s8		   m_achAlias[LEN_H323ALIAS+2];			//h323-Id 别名后两位为空
	
	tagH323ALIASADDR()
	{
		Clear();
	}
	
	void Clear()
	{
		m_emType = gk_reg_aliastype;
		memset( m_achAlias, 0, sizeof(m_achAlias) );
	}
	
	//设置别名
	BOOL SetAliasAddr( EmAliasType aliasType, const char* pAlias )
	{
		if ( aliasType <= gk_reg_aliastype || aliasType >= gk_reg_others ||
			pAlias == NULL ) 
			return FALSE;
		
		memset( m_achAlias, 0, sizeof(m_achAlias) );
		
		m_emType = aliasType;
		
		//u16 nLen = min( strlen(pAlias), LEN_H323ALIAS );
		u16 nLen = strlen(pAlias) < LEN_H323ALIAS ? strlen(pAlias) : LEN_H323ALIAS;
		strncpy( m_achAlias, pAlias, nLen );
		m_achAlias[nLen] = '\0';
		return TRUE;
	}	
	//获得type
	EmAliasType GetAliasType()
	{
		return m_emType;
	}
	//获得别名
	s8* GetAliasName()  
	{
		return m_achAlias;	
	}

	void HostToNet()
	{
		m_emType = (EmAliasType)(htonl((u32)m_emType));
	}
	void NetToHost()
	{
		m_emType =(EmAliasType)(ntohl((u32)m_emType));
	}

}TH323ALIASADDR,*PTH323ALIASADDR;


typedef struct tagGKRegInterwork 
{	
	u16				m_wAliasNum;
//	TH323ALIASADDR	m_atAlias[H323_MAX_ALIAS_NUM];
//	u16             m_wAddrNum;
//	TINTERNETADDR	m_atCalledAddr[H323_MAX_NUM_CONTACT_ADDR];	//呼叫地址
	
	s8				m_aEPId[H323_MAX_NUM_EPID];    //呼叫EPID
	u32				m_dwExpires;//超时时间
	
	tagGKRegInterwork()
	{
		memset(this, 0, sizeof(tagGKRegInterwork));
	}	


	void HostToNet()
	{
		m_wAliasNum = htons( m_wAliasNum );
/*		m_wAddrNum = htons( m_wAddrNum );*/
		m_dwExpires = htonl( m_dwExpires );
	}

	void NetToHost()
	{
		m_wAliasNum = ntohs( m_wAliasNum );
/*		m_wAddrNum = ntohs( m_wAddrNum );*/
		m_dwExpires = ntohl( m_dwExpires );
	}
}TGKRegInterwork;



#endif 
