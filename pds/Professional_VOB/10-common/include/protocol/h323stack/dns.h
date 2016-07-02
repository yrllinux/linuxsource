#ifdef __cplusplus
extern "C" {
#endif

#ifndef _DNS_H_
#define _DNS_H_
    
#include "mti.h"
#include "ema.h"

DECLARE_OPAQUE(HDNSMGR);
DECLARE_OPAQUE(HADNS);

#define MAX_QUERY_STRING_LEN        256
#define DNS_SERVER_PORT             53
#define MAX_DNS_USERDATA_LEN        64
#define MAX_DNS_MSGBUFFLEN_LEN      8*1024

typedef struct dnsSession_tag dnsSession;

typedef struct 
{
    RVHLOG  m_hLog;
    HSTIMER m_hTimers;
    
    HEMA    m_hDnsRa;
    UINT16  m_wNextId;
    int     m_hDnsSock;

    BYTE    m_abyMsgBuff[MAX_DNS_MSGBUFFLEN_LEN];
    
    dnsSession* m_ptSessionFirst;
    dnsSession* m_ptSessionLast;
    
} dnsModule;


typedef int (*evDnsResponseT)(dnsSession* dnsTx, int result, int nIPNum, UINT32 adwIP[]);


struct dnsSession_tag
{
    dnsSession* m_ptPreSession;
    dnsSession* m_ptNextSession;

    char m_achName[MAX_QUERY_STRING_LEN];
    
    UINT32  m_dwDnsServerIp;
    UINT16  m_wDnsServerPort;

    evDnsResponseT m_evDnsResponse;

    HTI m_hTimer;
    UINT16 m_wSessionId;
    BYTE m_byUsrData[MAX_DNS_USERDATA_LEN];
    int m_nUsrDataLen;
};



int dnsEncodeQuery(IN dnsSession* dnsTx, OUT BYTE* buffer, IN int length, OUT int* encoded);

int dnsDecode(IN dnsSession* dnsTx, OUT BYTE* buffer, IN int length);

/* dns module init */
RVAPI HDNSMGR RVCALLCONV dnsInitialize(IN RVHLOGMGR logMgr, IN UINT32 maxDnsTrans, IN  UINT32 dwStackIP = 0);

/* get dns module handle */
RVAPI HDNSMGR RVCALLCONV getDnsModule();

/* create dns transaction */
RVAPI dnsSession* RVCALLCONV dnsCreateOutTx(IN HDNSMGR hDnsMgr);

/* set transaction's server ip & port */
RVAPI int RVCALLCONV dnsSetDnsServer(IN dnsSession* dnsTx, IN UINT32 dnsIp, IN UINT16 dnsPort);

/* set transaction's callback function */
RVAPI int RVCALLCONV dnsSetCallBack(IN dnsSession* dnsTx, IN evDnsResponseT evDnsResponse);

/* set transaction's user data for save Upper context data */
RVAPI int RVCALLCONV dnsSetUsrData(IN dnsSession* dnsTx, IN BYTE* pbyUsrData, IN int nUsrDataLen);

/* get transaction's user data */
RVAPI int RVCALLCONV dnsGetUsrData(IN dnsSession* dnsTx, IN BYTE* pbyUsrDataBuff, IN int nUsrDataBuffLen);

/* start query */
RVAPI int RVCALLCONV dnsQuery(IN HDNSMGR hDnsMgr, IN dnsSession* dnsTx, IN char* pchQueryString);

/* stop and end query */
RVAPI int RVCALLCONV dnsEnd();

#endif // _DNS_H_


#ifdef __cplusplus
}
#endif
