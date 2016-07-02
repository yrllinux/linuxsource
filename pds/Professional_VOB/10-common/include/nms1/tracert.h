/*=============================================================================
ģ   ��   ��: ��
��   ��   ��: TraceRT.h
�� ��  �� ��: TraceRT.cpp
�ļ�ʵ�ֹ���: tracert����
��        ��: ���
ע        ��: ������30������, ÿ�����ز�������
��        ��: V4.0  Copyright(C) 2003-2006 Kedacom, All rights reserved.
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���      �޸�����
2006/06/06  4.0     ���        ����
=============================================================================*/

#ifndef __TRACERT_H__
#define __TRACERT_H__

#ifdef WIN32
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#endif  //  WIN32

#define MAX_GW_NUM          (s32)30     //������30������
#define TEST_PER_GW         (s32)3      //ÿ�����ز�������

//TraceRt��������
enum enumTraceRt
{
    emTraceRtEcho           = (u8)0,    //��������Ӧ
    emTraceRtGW             = (u8)1,    //���ص�ַ
    emTraceRtUnreachable    = (u8)2,    //�Զ˲��ɼ�
    emTraceRtOtherMsg       = (u8)3,    //������Ϣ, ���账��, �������жϲ���
    emTraceRtEnd            = (u8)4     //ȫ�����Խ���
};

//Ping�������ֵ
typedef struct tagTraceRtRst
{
    tagTraceRtRst() : m_byTraceRt(0), m_dwEchoTime(0), m_dwGWIP(0) {}

    u8  m_byTraceRt;    //���ؽ��
    u32 m_dwEchoTime;   //����ʱ��(������), m_byTraceRtΪemTraceRtEchoʱ��Ч
    u32 m_dwGWIP;       //��������IP��ַ(������), m_byTraceRtΪemTraceRtGWʱ��Ч
} TTraceRtRst, *PTTraceRtRst;

/*=============================================================================
�� �� ��:PFNTraceRTResult
��    ��:�ص�����
��    ��:void *pcbData                      [in]    �ص�����
         const TTraceRtRst &tTraceRtRst     [in]    ����ֵ
ע    ��:��
�� �� ֵ:��������: TRUE; ֹͣtracert����: FALSE
-------------------------------------------------------------------------------
�޸ļ�¼:
��      ��  �汾    �޸���  �޸�����
2006/06/06  4.0     ���    ����
=============================================================================*/
typedef BOOL32 (*PFNTraceRTResult)(void *pcbData, const TTraceRtRst &tTraceRtRst);

class CTraceRoute
{
public:
    CTraceRoute();
    virtual ~CTraceRoute();

    /*=============================================================================
    �� �� ��:SetParam
    ��    ��:���ò���
    ��    ��:u32 dwHostIP                   [in]    �Զ�IP��ַ(������)
             PFNTraceRTResult pfnResult     [in]    �ص�����
             void *pcbData                  [in]    �ص�����
    ע    ��:��
    �� �� ֵ:���������
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/06/06  4.0     ���    ����
    =============================================================================*/
    BOOL32 SetParam(u32 dwHostIP, PFNTraceRTResult pfnResult, void *pcbData);

    /*=============================================================================
    �� �� ��:Start
    ��    ��:��ʼ����
    ��    ��:��
    ע    ��:��
    �� �� ֵ:�ɹ�: TRUE; ʧ��: FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/06/06  4.0     ���    ����
    =============================================================================*/
    BOOL32 Start(void);

private:
    /*=============================================================================
    �� �� ��:CheckParam
    ��    ��:������
    ��    ��:u32 dwHostIP                   [in]    �Զ�IP��ַ(������)
             PFNTraceRTResult pfnResult     [in]    �ص�����
    ע    ��:��
    �� �� ֵ:�����Ϸ�: TRUE; �����Ƿ�: FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/06/06  4.0     ���    ����
    =============================================================================*/
    BOOL32 CheckParam(u32 dwHostIP, PFNTraceRTResult pfnResult);

    /*=============================================================================
    �� �� ��:SendReq
    ��    ��:��������
    ��    ��:SOCKET s                       [in]    socket
             u16 wSN                        [in]    ���к�
    ע    ��:��
    �� �� ֵ:���ͳɹ�: TRUE; ����ʧ��: FALSE
    -------------------------------------------------------------------------------
    �޸ļ�¼:
    ��      ��  �汾    �޸���  �޸�����
    2006/06/06  4.0     ���    ����
    =============================================================================*/
    BOOL32 SendReq(SOCKET s, u16 wSN);

private:
    u32                 m_dwHostIP; //�Զ�IP��ַ(������)
    PFNTraceRTResult    m_pfnResult;//�ص�����
    void                *m_pcbData; //�ص�����
};


#endif  //  __TRACERT_H__