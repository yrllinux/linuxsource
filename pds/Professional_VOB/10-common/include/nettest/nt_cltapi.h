#ifndef _NT_CLTAPI_H_
#define _NT_CLTAPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#define EV_TEST_CONNECT 1
#define EV_TEST_BW 2
#define EV_TEST_RTD 3
#define EV_TEST_PKT_LOST 4
#define EV_TEST_VIDEO_DEMO_REQ 5
#define EV_TEST_VIDEO_DEMO_END 6
#define EV_BYE 7

#define EV_TEST_UDP_PORT 8
#define EV_TEST_TCP_PORT 9

#define EV_TEST_DISCONNECTED 10

#define EV_TEST_SEND_STREAM 11

#define EV_TEST_RECV_STREAM 12


#define STAT_OK 0
#define STAT_CONNECT_FAILED -1
#define STAT_NETWORK_DISCONNECTED -2
#define STAT_MEM_FAILED -3
#define STAT_SRV_REJECTED -4
#define STAT_SRV_ERR -5
#define STAT_CLT_INNER_ERR -6
#define STAT_TEST_FAILED -7

#define STAT_PORT_BIND_ERR -8

#define STAT_PORT_MSG_ERR -9

#define STAT_SRV_TIMEOUT -10
#define STAT_NETWORK_ERR -11

#define STAT_ERR_ADR_OVERFLOW -12
	


#define STAT_BW_PROGRESS_IND 12


#define NT_LOG_FILTER_NONE 0
#define NT_LOG_FILTER_ALL 15

#define NT_LOG_FILTER_INFO 1
#define NT_LOG_FILTER_ERROR 2
#define NT_LOG_FILTER_DEBUG 4
#define NT_LOG_FILTER_EXCEPTION 8

extern int NT_CltInit(unsigned int localIp);

extern int NT_ConnectSrv(unsigned int srvIp, unsigned short port);

extern int NT_StartBwTest(unsigned int timeout);

extern int NT_StartRTDTest(unsigned int timeout);

extern int NT_StartPacketLostTest(unsigned int rate, unsigned int duration);

extern int NT_VideoDemoRequest(unsigned int rate, unsigned short vidRcvPort, unsigned short audRcvPort, unsigned int timeout);

extern void NT_VideoDemoEnd();

extern int NT_SetNatUsed(int bUsed);

extern int NT_IsTCPPortUsed(unsigned short port);

extern int NT_IsUDPPortUsed(unsigned short port);

extern int NT_SendStream(int rate);
extern int NT_RecvStream(int rate);

extern void NT_Terminate();


typedef void (*NT_CltTestResult)(int testId, int status, int para1, int para2, void * context);
typedef struct _NT_CltEvHandlers
{
    NT_CltTestResult pfnCltTestResult;
} NT_CltEvHandlers;

extern int NT_SetEvHandlers(NT_CltEvHandlers * handlers, void * context);

typedef void (*LogPrintFunc)(char * msg);

extern void NT_CltSetLogFilter(int filter);
extern void NT_CltSetLogPrintFunc(LogPrintFunc pfnFunc);

extern char* NT_CltGetVer();

#ifdef __cplusplus
}
#endif
#endif
