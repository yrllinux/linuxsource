/*****************************************************************************
模块名      : MMP容器
文件名      : mmp.h
创建时间    : 2009年 02月 10日
实现功能    : 
作者        : 周广程
版本        : 
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2009/02/10  1.0         周广程        创建
******************************************************************************/
#ifndef _MMP_CONTAINER_H_
#define _MMP_CONTAINER_H_

#include "kdvtype.h"
#include "kdvlog.h"
#include "loguserdef.h"

#define MODCFG_INI     "modcfg.ini"
#define CONNECT_INI    "connect.ini"

#define LOGFILE_8KH_MMPPREIEQP "/opt/mcu/log/mmppreieqp.log"

struct TMmpStartMod
{
private:
    u8 m_byIsRunVmp;
    u8 m_byVmpId;
    s8 m_achVmpAlias[MAX_VALUE_LEN+1];

    u8 m_byIsRunMixer;
    u8 m_byMixerId;
    s8 m_achMixerAlias[MAX_VALUE_LEN+1];

    u8 m_byIsRunBas;
    u8 m_byBasId;
    s8 m_achBasAlias[MAX_VALUE_LEN+1];

	u32 m_dwVmpIfameInterval;
	u32 m_dwBasIfameInterval;

public:
    TMmpStartMod() { memset( this, 0, sizeof(TMmpStartMod) ); }

    void SetIsRunVmp(BOOL32 bIsRun) { m_byIsRunVmp = (bIsRun == TRUE) ? 1 : 0; }
    BOOL32 IsRunVmp(void) const { return (m_byIsRunVmp == 1) ? TRUE : FALSE; }

    void SetVmpId( u8 byVmpId ) { m_byVmpId = byVmpId; }
    u8   GetVmpId( void ) const { return m_byVmpId; } 

    void SetVmpAlias( const s8 *pchAlias )
    {
        if ( NULL != pchAlias )
        {
            strncpy( m_achVmpAlias, pchAlias, sizeof(m_achVmpAlias)-1 );
            m_achVmpAlias[MAX_VALUE_LEN] = '\0';
        }
        else
        {
            memset( m_achVmpAlias, 0, sizeof(m_achVmpAlias) );
        }
    }
    s8 * GetVmpAlias( void )
    {
        return m_achVmpAlias;
    }

    void SetIsRunMixer(BOOL32 bIsRun) { m_byIsRunMixer = (bIsRun == TRUE) ? 1 : 0; }
    BOOL32 IsRunMixer(void) const { return (m_byIsRunMixer == 1) ? TRUE : FALSE; }

    void SetMixerId( u8 byMixerId ) { m_byMixerId = byMixerId; }
    u8   GetMixerId( void ) const { return m_byMixerId; } 

    void SetMixerAlias( const s8 *pchAlias )
    {
        if ( NULL != pchAlias )
        {
            strncpy( m_achMixerAlias, pchAlias, sizeof(m_achMixerAlias)-1 );
            m_achMixerAlias[MAX_VALUE_LEN] = '\0';
        }
        else
        {
            memset( m_achMixerAlias, 0, sizeof(m_achMixerAlias) );
        }
    }
    s8 * GetMixerAlias( void )
    {
        return m_achMixerAlias;
    }

    void SetIsRunBas(BOOL32 bIsRun) { m_byIsRunBas = (bIsRun == TRUE) ? 1 : 0; }
    BOOL32 IsRunBas(void) const { return (m_byIsRunBas == 1) ? TRUE : FALSE; }

    void SetBasId( u8 byBasId ) { m_byBasId = byBasId; }
    u8   GetBasId( void ) const { return m_byBasId; } 

    void SetBasAlias( const s8 *pchAlias )
    {
        if ( NULL != pchAlias )
        {
            strncpy( m_achBasAlias, pchAlias, sizeof(m_achBasAlias)-1 );
            m_achBasAlias[MAX_VALUE_LEN] = '\0';
        }
        else
        {
            memset( m_achBasAlias, 0, sizeof(m_achBasAlias) );
        }
    }
    s8 * GetBasAlias( void )
    {
        return m_achBasAlias;
    }

	void  SetVmpIframeInterval(u32 dwIframeInterval) { m_dwVmpIfameInterval = dwIframeInterval; }
    u32   GetVmpIframeInterval(void) { return m_dwVmpIfameInterval; }
	
	void  SetBasIframeInterval(u32 dwIframeInterval) { m_dwBasIfameInterval = dwIframeInterval; }
    u32   GetBasIframeInterval(void) { return m_dwBasIfameInterval; }
};

struct TConnectInfo
{
private:
    u8  m_byDiscHeartBeatNum;
    u16 m_wDiscHeartBeatTime;
    
    u16 m_wMcuPort;
    u32 m_dwMcuIp;

    u32 m_dwLocalIp;
    
public:
    TConnectInfo() { memset( this, 0, sizeof(TConnectInfo) ); }

    void SetMcuIp( u32 dwMcuIp ) { m_dwMcuIp = htonl(dwMcuIp); }
    u32  GetMcuIp( void ) const { return ntohl(m_dwMcuIp); }

    void SetMcuPort( u16 wMcuPort ) { m_wMcuPort = htons(wMcuPort); }
    u16  GetMcuPort( void ) const { return ntohs(m_wMcuPort); }

    void SetLocalIp( u32 dwLocalIp ) { m_dwLocalIp = htonl(dwLocalIp); }
    u32  GetLocalIp( void ) const { return ntohl(m_dwLocalIp); }

    void SetDiscHeartBeatNum( u8 byNum ) { m_byDiscHeartBeatNum = byNum; }
    u8   GetDiscHeartBeatNum( void ) const { return m_byDiscHeartBeatNum; }

    void SetDiscHeartBeatTime( u16 wTime ) { m_wDiscHeartBeatTime = htons(wTime); }
    u16   GetDiscHeartBeatTime( void ) const { return ntohs(m_wDiscHeartBeatTime); }
};

void GetStartMod( TMmpStartMod &tMmpStartMod );
void CreatModCfg( void );
void GetConnectInfo( TConnectInfo &tConnectInfo );

void MmpFileLog( const s8 * pchLogFile, s8 * pszFmt, ... );

#endif //!_MMP_CONTAINER_H_


