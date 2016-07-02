/*****************************************************************************
   ģ����      : Board Agent
   �ļ���      : driconfig.h
   ����ļ�    : 
   �ļ�ʵ�ֹ���: ������ú�������
   ����        : jianghy
   �汾        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   �޸ļ�¼:
   ��  ��      �汾        �޸���      �޸�����
   2003/08/25  1.0         jianghy       ����
   2004/12/05  3.5         �� ��        �½ӿڵ��޸�
******************************************************************************/
#ifndef DRICONFIG_H
#define DRICONFIG_H

#include "mcuconst.h"
#include "vccommon.h"
#include "mcuagtstruct.h"
#include "eqpcfg.h"

class CDriConfig
{
public:
	CDriConfig();
	~CDriConfig();
    friend class CBoardAgent;
	/*====================================================================
	���ܣ���ȡ����Ĳ�λ��
	��������
	����ֵ��IDֵ��0��ʾʧ��
	====================================================================*/
	u8	GetBoardId();

	/*====================================================================
	���ܣ���ȡҪ���ӵ�MCU��IP��ַ(������)
	��������
	����ֵ��MCU��IP��ַ(������)��0��ʾʧ��
	====================================================================*/
	u32 GetConnectMcuIpAddr();

	/*====================================================================
	���ܣ���ȡ���ӵ�MCU�Ķ˿ں�
	��������
	����ֵ��MCU�Ķ˿ںţ�0��ʾʧ��
	====================================================================*/
	u16  GetConnectMcuPort();

	/*====================================================================
	���ܣ��õ�����IP��ַ
	��������
	����ֵ������IP��ַ(������)
	====================================================================*/
	u32 GetBrdIpAddr( );

	/*====================================================================
	���ܣ��Ƿ�����Prs
	��������
	����ֵ�����з���TRUE����֮FALSE
	====================================================================*/
	BOOL IsRunPrs();

	/*====================================================================
	���ܣ���ȡprs������Ϣ
	��������
	����ֵ��MCU�Ķ˿ںţ�0��ʾʧ��
	====================================================================*/
	BOOL GetPrsCfg( TPrsCfg* ptCfg );

	/*====================================================================
	���ܣ�����PRS��������Ϣ
	��������
	����ֵ��TRUE/FALSE
	====================================================================*/
	BOOL SetPrsConfig( TEqpPrsEntry *ptPrsCfg );
	
	/*====================================================================
	���ܣ����õ���IP��ַ
	��������
	����ֵ��TRUE/FALSE
	====================================================================*/
	BOOL SetBrdIpAddr( u32 dwIp );

	/*====================================================================
	���ܣ����ļ���������Ϣ
	��������
	����ֵ��TRUE/FALSE
	====================================================================*/
	BOOL ReadConnectMcuInfo();

    u32  GetMpcBIp(void);
    void SetMpcBIp(u32 dwIp); // ������
    u16  GetMpcBPort(void);
    void SetMpcBPort(u16 wPort);

    u32  GetMpcAIp(void);
    void SetMpcAIp(u32 dwIp);// ������
    u16  GetMpcAPort(void);
    void SetMpcAPort(u16 wPort);
    
    u32  GetDriIp(void);
    void SetDriIp(u32 dwIp); // ������
public:
    u32	 m_dwDstMcuNode;
    u32  m_dwDstMcuNodeB;
    
    u16  m_wDiscHeartBeatTime;
    u8   m_byDiscHeartBeatNum;

private:
	BOOL    bIsRunPrs;      //�Ƿ�����PRS
	TPrsCfg m_prsCfg;
	u32	    m_dwMpcIpAddr;
	u16     m_wMpcPort;
    u32     m_dwMpcIpAddrB;
    u16     m_wMpcPortB;

	u8	m_byBrdId;
	u32 m_dwBrdIpAddr;	
    u8  m_byChoice;
};

#endif      /* DRICONFIG_H */