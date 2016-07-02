#ifndef _MTH323BRIDGELIB_H_
#define _MTH323BRIDGELIB_H_

#include "kdvtype.h"
//add by yj for ipv6
//#include "h323adapter.h"
#ifdef RV_42
#include "h323adapter4_2.h"
#else
#include "h323adapter.h"
#endif
//end
#include "firewallproxyclientlib.h"

/************************************************************************
 *   Defines & Enum
 ***********************************************************************/
#define MAX_CHAN_NUM 8
#define RETMTBRIDGEOK   0
#define RETMTBRIDGEFAIL -1
/************************************************************************
 *   Struct
 ***********************************************************************/
typedef struct tagStatus
{
    // Bridge Status.
    BOOL m_bBridgeRun;
    
    // Pxy Status.
    BOOL m_bPxyInited;
    u8   m_bPxyState;
    H323EVENT m_tPxyEvent;
//    HCALL m_hsPxyCall;
//    HCHAN m_hsPxyChan[MAX_CHAN_NUM];
//    u8  m_byPxyChanState[MAX_CHAN_NUM];    

    // Mt Status.
    BOOL m_bMtInited;
    u8   m_bMtState;
    H323EVENT m_tMtEvent;
//    HCALL m_hsMtCall;
//    HCHAN m_hsMtChan[MAX_CHAN_NUM];
    u32   m_dwCallOpened;
    u32   m_dwChanOpened;
//    u8  m_byMtChanState[MAX_CHAN_NUM];

    TTERLABEL m_tMTDstLabel;
    TTERLABEL m_tMTSrcLabel;
    TTERLABEL m_tPxyDstLabel;
    TTERLABEL m_tPxySrcLabel;
}TH323BridgeStatus;

//typedef struct tagStatis
//{
//    // Bridge statis
//    u32 m_dwBridgeRASIn;
//    u32 m_dwBridgeRASOut;
//    u32 m_dwBridgeConfCtrlIn;
//    u32 m_dwBridgeConfCtrlOut;
//    u32 m_dwBridgeChanCtrlIn;
//    u32 m_dwBridgeChanCtrlOut;
//    
//    // Pxy statis
//    u32 m_dwPxyRASIn;
//    u32 m_dwPxyRASOut;
//    u32 m_dwPxyConfCtrlIn;
//    u32 m_dwPxyConfCtrlOut;
//    u32 m_dwPxyChanCtrlIn;
//    u32 m_dwPxyChanCtrlOut;
//
//        
//    // Mt statis
//    u32 m_dwMtRASIn;
//    u32 m_dwMtRASOut;
//    u32 m_dwMtConfCtrlIn;
//    u32 m_dwMtConfCtrlOut;
//    u32 m_dwMtChanCtrlIn;
//    u32 m_dwMtChanCtrlOut;
//
//}TH323BridgeStatis;

//用户使用的MsgSrc指明是终端还是Pxy
typedef enum tagEmMsgSrc
{
    emMsgFromMt = 1,
    emMsgFromPxy
}EmBridgeMsgSrc;

typedef struct tagFeccStr
{
    TTERLABEL m_tSrcLabel;
    TTERLABEL m_tDstLabel;
    TFeccStruct feccStruct;
}TBridgeFeccStruct;

/*H323Bridge Manager*/
void *InitMtH323Bridge(TClientCfgStruc & tCfg);
void  CloseMtH323Bridge();
BOOL  BridgeLog(u8 byType, u8 byLevel);
TH323BridgeStatus BridgeStatus();
//TH323BridgeStatis BridgeStatis();


/*Interface*/
s32 MtH323BridgeSetCallBack(IN LPH323EVENT lpEvent, EmBridgeMsgSrc emSrc);
s32 MtH323BridgeCreateNewCall(IN HAPPCALL haCall, OUT LPHCALL lphsCall, EmBridgeMsgSrc emSrc);

s32 MtH323BridgeCreateNewChan(IN HCALL hsCall, IN HAPPCHAN haChan, 
                   OUT LPHCHAN lphsChan, EmBridgeMsgSrc emSrc);
s32 MtH323BridgeSendRasCtrlMsg(IN HCALL hsCall, IN HRAS hsRas, IN u16 msgType,
                   IN const void* pBuf, IN u16 nBufLen, EmBridgeMsgSrc emSrc);
s32 MtH323BridgeSendCallCtrlMsg(IN HCALL hsCall,IN u16 msgType,
                   IN const void* pBuf, IN u16 nBufLen, EmBridgeMsgSrc emSrc);
s32 MtH323BridgeSendChanCtrlMsg(IN HCHAN hsChan, IN u16 msgType,
                   IN const void* pBuf,IN u16 nBufLen, EmBridgeMsgSrc emSrc);
s32 MtH323BridgeSendConfCtrlMsg(IN HCALL hsCall, IN u16 msgType,
                   IN const void* pBuf, IN u16 nBufLen, EmBridgeMsgSrc emSrc);
s32 MtH323BridgeFeccBind(IN HCHAN hsChan, IN TTERLABEL localTer, 
                IN TTERLABEL remoteTer, EmBridgeMsgSrc emSrc);
s32 MtH323BridgeSendFeccMsg(IN HCHAN hsChan, IN TFeccStruct feccStruct, EmBridgeMsgSrc emSrc);
s32 MtH323BridgeSetGKRASAddress(IN HAPP hApp, IN cmTransportAddress tr);
s32 MtH323BridgeIsMasterCall(IN HCALL hsCall);
void MtH323BridgeReset();

s8* MtH323BridgeGetPeerVersionID(HCALL hsCall);
s8* MtH323BridgeGetPeerProductID(HCALL hsCall);

#endif


