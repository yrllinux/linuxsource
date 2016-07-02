/*lint -save -e40*/
/*lint -save -e10*/
/*lint -save -e1013*/
/*lint -save -e63*/
//lint -save -sem(CMCUCascade::UnInit,cleanup)
/*lint -save -e611*/
/*lint -save -esym(1788,lock)*/
/*lint -save -e1039*/
/*lint -save -e1790*/
/*lint -save -e845*/
/*lint -save -e694*/

#include "osp.h"
//#include "kdvtype.h"
#include "cascade.h"
#include "h323mcuconn.h"
#include "mcucascadecomm.h"
//#include <time.h>
#ifdef NULL
#undef NULL
#endif
#define NULL 0

#define CASC_DATA    ((s8 *)"4097")
//#define ISMCU(pid)   (pid==0xffffffff?TRUE:FALSE)
#define PARTS_PER_PACKET 4
#define MAX_MSGLABLE_LEN (s32)128
static CMCUCascade*  g_apcCascadeInstance[MAX_CASCADES_COUNT];
static SEMHANDLE     g_hSem = NULL;
static CXMLTree*     g_pXMLEncTree = NULL; 
static CXMLDec*      g_pXMLDec = NULL;
static u8            g_abyBuf[MAX_XML_BUFFER_LEN];
static u8            g_abyPid[CONFID_BYTELEN];

#define NORMAL_PART (s32)1
#define ABNORMAL_PART ((s32)-1)
#define BAD_PART (s32)0
static  u32   dwListenNode = 0;

const s8 * const pszStrTrue = "true";
//const s8 * const pszStrFalse = "false";
const s8 * const pszStrNone = "none";
const s8 * const pszStrNormal = "normal";

//用于调试
const s8 * const pszWF = "XMLW.txt";
static FILE *g_pWF = NULL;
static FILE *g_pRF = NULL;
////////////////////

typedef struct tagDefaultRsp:public TRsp
{
    s8 m_pszLable[MAX_MSGLABLE_LEN];
}TDefaultRsp;


static s8 num2str(s8 n)
{
    s8 ch;
    
    if (n >= 0 && n <= 9)
    {
        ch = '0' + n;
    }
    else
    {
        ch = 'a' + n - 0x0a;
    }
    
    return ch;
}


static void printsession()
{
    s32 nMaxCascadeNum = sizeof(g_apcCascadeInstance)/sizeof(g_apcCascadeInstance[0]);
    for(s32 idx=0; idx<nMaxCascadeNum; idx++)
    {
        if (g_apcCascadeInstance[idx] != NULL)
        {
            u8 achSrcBuff[32];
            u8 achDstBuff[32];
            s32 i;
            s8* pch;
            pch = (s8*)g_apcCascadeInstance[idx]->m_abySrcConfID;
            for(i = 0; i < CONFID_BYTELEN; i++)
            {
                s8 ch = *pch++;
                achSrcBuff[2*i] = num2str(((unsigned)(int)ch>>4) & 0x0f);
                achSrcBuff[2*i+1] = num2str((ch) & 0x0f);
            }
            achSrcBuff[2*CONFID_BYTELEN] = '\0';

            pch = (s8*)g_apcCascadeInstance[idx]->m_abyDstConfID;
            for(i = 0; i < CONFID_BYTELEN; i++)
            {
                s8 ch = *pch++;
                achDstBuff[2*i] = num2str(((unsigned)(int)ch>>4) & 0x0f);
                achDstBuff[2*i+1] = num2str((ch) & 0x0f);
            }
            achDstBuff[2*CONFID_BYTELEN] = '\0';
            
            u8* pchLocalIP = (u8*)&g_apcCascadeInstance[idx]->m_dwLocalIP;
            u8* pchPeerIP = (u8*)&g_apcCascadeInstance[idx]->m_dwPeerIP;
            OspPrintf(TRUE, FALSE, "Session[%d](0x%x), Local:%u.%u.%u.%u:%u, Peer:%u.%u.%u.%u:%u\n\tsrcid:%s dstid:%s CB:x%x\n",
                idx, g_apcCascadeInstance[idx], 
                pchLocalIP[0], pchLocalIP[1], pchLocalIP[2], pchLocalIP[3],
                g_apcCascadeInstance[idx]->m_wLocalPort,
                pchPeerIP[0], pchPeerIP[1], pchPeerIP[2], pchPeerIP[3],
                g_apcCascadeInstance[idx]->m_wPeerPort,
                achSrcBuff, achDstBuff, g_apcCascadeInstance[idx]->m_pfAppHandler);
            
        }
    }
}

static BOOL32 AddCasIns(CMCUCascade *pIns)
{
	CSemLock lock(&g_hSem);
    s32 nMaxCascadeNum = sizeof(g_apcCascadeInstance)/sizeof(g_apcCascadeInstance[0]);
	for(s32 idx=0; idx<nMaxCascadeNum; idx++)
	{
		if(g_apcCascadeInstance[idx] == NULL)
		{
			g_apcCascadeInstance[idx] = pIns;
            //mmculog(MMCU_DEBUG2, "Session(0x%x) Add OK\n", pIns);
			LogPrint(LOG_LVL_DETAIL,MID_MCULIB_MMCULIB,"Session(0x%x) Add OK\n", pIns);
			return TRUE;
		}
	}

	LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB, "AddCasIns(0x%x) fail, max num:%d\n", pIns, nMaxCascadeNum);
    //mmculog(MMCU_EXP, "AddCasIns(0x%x) fail, max num:%d\n", pIns, nMaxCascadeNum);

	return FALSE;
}


static BOOL32 DelCasIns(CMCUCascade *pIns)
{
	CSemLock lock(&g_hSem);
	s32 nMaxCascadeNum = sizeof(g_apcCascadeInstance)/sizeof(g_apcCascadeInstance[0]);
	for(s32 idx=0; idx<nMaxCascadeNum; idx++)
	{
		if(g_apcCascadeInstance[idx] == pIns)
		{
			g_apcCascadeInstance[idx] = NULL;

            LogPrint(LOG_LVL_DETAIL,MID_MCULIB_MMCULIB, "Session(0x%x) Delete OK\n", pIns);
            //mmculog(MMCU_DEBUG2, "Session(0x%x) Delete OK\n", pIns);
            break;
		}
	}

	return TRUE;
}

CMCUCascade *FindCasIns(u8 abySrcConfID[CONFID_BYTELEN], u8 abyDstConfID[CONFID_BYTELEN])
{
	s32 nMaxCascadeNum = sizeof(g_apcCascadeInstance)/sizeof(g_apcCascadeInstance[0]);
	for(s32 idx=0; idx<nMaxCascadeNum; idx++)
	{
		if(g_apcCascadeInstance[idx] != NULL 
		   && g_apcCascadeInstance[idx]->m_pfAppHandler !=NULL
		   && memcmp(g_apcCascadeInstance[idx]->m_abySrcConfID, abySrcConfID, CONFID_BYTELEN) == 0
		   && memcmp(g_apcCascadeInstance[idx]->m_abyDstConfID, abyDstConfID, CONFID_BYTELEN) == 0)
		{
            return g_apcCascadeInstance[idx];
		}
	}

    u8 achSrcBuff[32];
    u8 achDstBuff[32];
    s32 i;
    s8* pch;
    pch = (s8*)abySrcConfID;
    for(i = 0; i < CONFID_BYTELEN; i++)
    {
        s8 ch = *pch++;
        achSrcBuff[2*i] = num2str(((unsigned)(int)ch>>4) & 0x0f);
        achSrcBuff[2*i+1] = num2str((ch) & 0x0f);
    }
    achSrcBuff[2*CONFID_BYTELEN] = '\0';
    
    pch = (s8*)abyDstConfID;
    for(i = 0; i < CONFID_BYTELEN; i++)
    {
        s8 ch = *pch++;
        achDstBuff[2*i] = num2str(((unsigned)(int)ch>>4) & 0x0f);
        achDstBuff[2*i+1] = num2str((ch) & 0x0f);
    }
    achDstBuff[2*CONFID_BYTELEN] = '\0';

	LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"FindCasIns Fail, srcid:%s dstid:%s\n", achSrcBuff, achDstBuff);
    //mmculog(MMCU_EXP, "FindCasIns Fail, srcid:%s dstid:%s\n", achSrcBuff, achDstBuff);

    if (MMCU_DEBUG2  == g_nloglevel)
    {
        printsession();
    }

	return NULL;
}


BOOL32 MMcuStackInit(u16 wListenPort, BOOL32 bSockInit)
{
	BOOL32 bRet = FALSE;
    bRet = TcpSocketInit(bSockInit);
	if(!bRet)
	{
		//mmculog(MMCU_EXP, "[MMCU]MMcuStackInit: TcpSocketInit f!");
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[MMCU]MMcuStackInit: TcpSocketInit f!");
		return FALSE;
	}

    TTCPEvent tcpLinkCallback;
    tcpLinkCallback.pNewConnEvent=NewConnection;
    tcpLinkCallback.pDataCallBack=DispatchCascadeMsg;
   	TcpSetCallBack(tcpLinkCallback);

	memset(g_apcCascadeInstance, 0, sizeof(g_apcCascadeInstance));
    if(g_hSem != NULL)
	{
		OspSemDelete(g_hSem);
		g_hSem =  NULL;
	}

	if(!OspSemBCreate(&g_hSem))
	{
		MMcuStackClean(bSockInit);
		//mmculog(MMCU_EXP, "[MMCU]MMcuStackInit: OspSemBCreate error!");
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[MMCU]MMcuStackInit: OspSemBCreate error!");
		return FALSE;
	}

	g_pXMLEncTree     = new  CXMLTree();
	if(g_pXMLEncTree == NULL)
	{
		MMcuStackClean(bSockInit);
		return FALSE;
	}
	if(!g_pXMLEncTree->Create(XML_NODE_NUM, XML_NODE_SIZE))
	{
		MMcuStackClean(bSockInit);
		//mmculog(MMCU_EXP, "[MMCU]MMcuStackInit: create tree error(num:%d,size:%d)!", XML_NODE_NUM, XML_NODE_SIZE);
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[MMCU]MMcuStackInit: create tree error(num:%d,size:%d)!", XML_NODE_NUM, XML_NODE_SIZE);
		return FALSE;
	}
	
	g_pXMLDec = new CXMLDec();
	if(g_pXMLDec == NULL)
	{
		MMcuStackClean(bSockInit);
		return FALSE;
	}
	if(!g_pXMLDec->Create(XML_NODE_NUM, XML_NODE_SIZE))
	{
		MMcuStackClean(bSockInit);
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[MMCU]MMcuStackInit: create XMLdec error(num:%d,size:%d)!", XML_NODE_NUM, XML_NODE_SIZE);
		//mmculog(MMCU_EXP, "[MMCU]MMcuStackInit: create XMLdec error(num:%d,size:%d)!", XML_NODE_NUM, XML_NODE_SIZE);
		return FALSE;
	}

	u32 dwLocalIP = htonl(INADDR_ANY);
	dwListenNode = TcpCreateNode(dwLocalIP, wListenPort);
	if(dwListenNode == INVALID_NODE)
	{
		return FALSE;
	}
	
    mmcusetlog(0);

#ifdef _LINUX_
	OspRegCommand("mmcuhelp", (void*)mmcuhelp, "");
	OspRegCommand("mmcusetlog", (void*)mmcusetlog,"");
#endif
	
    return TRUE;
}

BOOL32 MMcuStackClean(BOOL32 bSockClean)
{
	memset(g_apcCascadeInstance, 0, sizeof(g_apcCascadeInstance));
    if(g_hSem != NULL)
	{
		OspSemDelete(g_hSem);
		g_hSem =  NULL;
	} 
	if(g_pXMLEncTree != NULL)
	{
		delete g_pXMLEncTree;
		g_pXMLEncTree= NULL;
	}
	if(g_pXMLDec != NULL)
	{
		delete g_pXMLDec;
		g_pXMLDec = NULL;
	}
	if(g_pWF != NULL)
	{
		fclose(g_pWF);
		g_pWF = NULL;
	}
	if(g_pRF != NULL)
	{
		fclose(g_pRF);
		g_pRF = NULL;
	}
    TcpDestroyNode(dwListenNode);
	return TcpSocketClean(bSockClean);
}

CMCUCascade::CMCUCascade()
{
    m_dwCookie        = 0;
    m_dwUserData      = 0;
    m_pfAppHandler    = NULL;
    m_dwNode          = INVALID_NODE;
    m_nTcpBufLen      = 0;
    m_hSem            = NULL;
    m_pbyTcpBuf       = NULL;
    m_pbyBackUp       = NULL;

    memset(m_abySrcConfID, 0, sizeof(m_abySrcConfID));
    memset(m_abyDstConfID, 0, sizeof(m_abyDstConfID));
    memset(m_abyChairID, 0, sizeof(m_abyChairID));
    
    //2007.04.06 可能会失败，由外部调用并检查返回值
    //Init();
}

CMCUCascade::~CMCUCascade()
{
	try
	{
		UnInit();
	}
	catch (...)
	{
	}
}

u16 CMCUCascade::Init()
{
	
	UnInit();

    m_dwCookie        = OspTickGet();
	if(!OspSemBCreate(&m_hSem))
	{
		UnInit();
		return MMCU_FALSE;
	}

	m_pbyTcpBuf = new u8[MAX_XML_DEAL_LEN];
	if(m_pbyTcpBuf == NULL)
	{
		UnInit();
		return MMCU_FALSE;
	}

	m_pbyBackUp = new u8[MAX_XML_BUFFER_LEN];
	if(m_pbyBackUp == NULL)
	{
		UnInit();
		return MMCU_FALSE;
	}

	if(!AddCasIns(this))
	{
		UnInit();
		return MMCU_FALSE;
	}

    m_dwLocalIP = 0;
    m_wLocalPort = 0;
    m_dwPeerIP = 0;
    m_wPeerPort = 0;

	return MMCU_OK;
}

u16 CMCUCascade::UnInit()
{   
	 DelCasIns(this);
     if (m_hSem != NULL)
     {
         OspSemTake(m_hSem);
     }     
	 CLOSE_NODE(m_dwNode);
	 
	 m_dwCookie        = 0;
	 m_dwUserData      = 0;
	 m_pfAppHandler    = NULL;
	 m_dwNode          = INVALID_NODE;
	 m_nTcpBufLen      = 0;
	 memset(m_abySrcConfID, 0, sizeof(m_abySrcConfID));
	 memset(m_abyDstConfID, 0, sizeof(m_abyDstConfID));
	 memset(m_abyChairID, 0, sizeof(m_abyChairID));
	 
	 if(m_hSem != NULL)
	 {
		 OspSemDelete(m_hSem);
		 m_hSem = NULL;
	 }
	 if(m_pbyTcpBuf != NULL)
	 {
		 delete []m_pbyTcpBuf;
		 m_pbyTcpBuf = NULL;
	 }

	 if(m_pbyBackUp != NULL)
	 {
		 delete []m_pbyBackUp;
		 m_pbyBackUp = NULL;
	 }

     m_dwLocalIP = 0;
     m_wLocalPort = 0;
     m_dwPeerIP = 0;
     m_wPeerPort = 0;

	 return MMCU_OK;
}


u16 CMCUCascade::SetSrcConfId(u8 *pbyBuf, s32 nLen)
{
    if (MMCU_DEBUG2  == g_nloglevel)
    {
        u8 achSrcBuff[32];
        s32 i;
        s8* pch;
        pch = (s8*)pbyBuf;
        for(i = 0; i < CONFID_BYTELEN; i++)
        {
            s8 ch = *pch++;
            achSrcBuff[2*i] = num2str(((unsigned)(int)ch>>4) & 0x0f);
            achSrcBuff[2*i+1] = num2str((ch) & 0x0f);
        }
        achSrcBuff[2*CONFID_BYTELEN] = '\0';

        OspPrintf(TRUE, FALSE, "SetSrcConfId, session:0x%x srcid:%s, bufflen:%d\n", 
                 this, achSrcBuff, nLen);
    }
    
    if(nLen != CONFID_BYTELEN) 
	{
		return MMCU_FALSE;
	}

    memcpy(m_abySrcConfID, pbyBuf, nLen);
    return MMCU_OK;
}

u16  CMCUCascade::SetDstConfId(u8 *pbyBuf, s32 nLen)
{
    if (MMCU_DEBUG2  == g_nloglevel)
    {
        u8 achDstBuff[32];
        s32 i;
        s8* pch;
        pch = (s8*)pbyBuf;
        for(i = 0; i < CONFID_BYTELEN; i++)
        {
            s8 ch = *pch++;
            achDstBuff[2*i] = num2str(((unsigned)(int)ch>>4) & 0x0f);
            achDstBuff[2*i+1] = num2str((ch) & 0x0f);
        }
        achDstBuff[2*CONFID_BYTELEN] = '\0';

        OspPrintf(TRUE, FALSE, "SetDstConfId, session:0x%x dstid:%s, bufflen:%d\n", 
            this, achDstBuff, nLen);
    }

    if( nLen != CONFID_BYTELEN )
	{
		return MMCU_FALSE;
	}

    memcpy(m_abyDstConfID, pbyBuf, nLen);
    return MMCU_OK;
}
    
u16 CMCUCascade::Connect(u32 dwPeerIP,u16 wPeerPort)
{
   CSemLock lock(&m_hSem);
   if(m_dwNode != INVALID_NODE)
   {
	   return MMCU_FALSE;
   }

   m_dwNode = TcpConnect(dwPeerIP,wPeerPort, (u32)this);
   if(m_dwNode == INVALID_NODE)
   {
	   return MMCU_FALSE;
   }

   return MMCU_OK;
}

u16 CMCUCascade::Disconnect()
{
	CSemLock lock(&m_hSem);
	if(m_dwNode == INVALID_NODE)
	{
		return MMCU_OK;
	}

    if(!TcpDisconnect(m_dwNode))
	{
		return MMCU_FALSE;
	}
    m_dwNode = INVALID_NODE;
	return MMCU_OK;
}

u16 CMCUCascade::SendMsg(s32 nMsgID,u8 *pbyBuffer,u32 dwBufLen)
{
    CSemLock lock(&m_hSem);

	if(m_dwNode == INVALID_NODE)
	{
		return MMCU_FALSE;
	}

	CXMLTree *pcTree = NULL;
	g_pXMLEncTree->Reset();

	switch(nMsgID)
	{
	case H_CASCADE_NONSTANDARD_REQ:
		{
            pcTree = ToNonStandardMsgReq(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_NONSTANDARD_RSP:
		{
			pcTree = ToNonStandardMsgRsp(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_NONSTANDARD_NTF:
		{
			pcTree = ToNonStandardMsgNtf(pbyBuffer, dwBufLen);
			break;
		}
		//
	case H_CASCADE_REGUNREG_REQ: 
		{
            pcTree = ToRegUnRegReq(pbyBuffer, dwBufLen);
			break;
		}
    case H_CASCADE_REGUNREG_RSP:
		{
			pcTree = ToRegUnRegRsp(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_NEWROSTER_NTF:
		{
			pcTree = ToNewRosterNtf(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_INVITEPART_REQ:
		{
			pcTree = ToInvitePartReq(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_INVITEPART_RSP:
		{
			pcTree = ToRsp(pbyBuffer, dwBufLen, "Invite_Participant_Response");
			break;
		}
	case H_CASCADE_NEWPART_NTF:
		{
			pcTree = ToNewPartNtf(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_CALL_ALERTING_NTF:
		{
			pcTree = ToCallNtf(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_PARTLIST_REQ:
		{
			pcTree = ToPartListReq(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_PARTLIST_RSP:
		{
            s32 nStartPartIndex = 0;
            BOOL32 bFinished    = FALSE;
            s32 nPartsPerPacket = PARTS_PER_PACKET;
            if(ISMMCULOG(MMCU_FILE))	
            {
                if(g_pWF == NULL)
                {
                    g_pWF = fopen(pszWF,"w");
                }
            } 

            //从消息中构造TPartListRsp
            TPartListMsg* ptMsg = (TPartListMsg*)pbyBuffer;
            
            if (dwBufLen != sizeof(TPartListMsg) + (u32)ptMsg->m_dwPartCount*sizeof(TPart))
            {
                return MMCU_FALSE;
            }

            //mmculog(MMCU_DEBUG, "Session:0x%x Send PartList Msg: ReqId=%d, Count=%d\n",this, ptMsg->m_nReqID, ptMsg->m_dwPartCount);
            LogPrint(LOG_LVL_DETAIL,MID_MCULIB_MMCULIB,"Session:0x%x Send PartList Msg: ReqId=%d, Count=%d\n",
                    this, ptMsg->m_nReqID, ptMsg->m_dwPartCount);
            
            
            TPartListRsp tPartListRsp;

            tPartListRsp.m_emReturnVal = ptMsg->m_emReturnVal;
            tPartListRsp.m_nSnapShot   = ptMsg->m_nSnapShot;
            tPartListRsp.m_nReqID      = ptMsg->m_nReqID;
            tPartListRsp.m_bLastPack   = ptMsg->m_bLastPack;

            s32 i;
            TPart tPart;
            TPart* ptPart = (TPart*)(pbyBuffer+sizeof(TPartListMsg));
            
            for(i = 0; i < ptMsg->m_dwPartCount; i++)
            {
                memcpy(&tPart, ptPart, sizeof(TPart));
                tPartListRsp.AddPart(tPart, FALSE);
                ptPart++;
            }
            
            do
            {
                pcTree = ToPartListRsp((u8*)&tPartListRsp, sizeof(TPartListRsp), bFinished, nStartPartIndex, nPartsPerPacket);

                if(pcTree == NULL)
                {
                    return MMCU_FALSE;
                }
                
                PrintXMLTree(this, MMCU_ALL, pcTree, "SendMsg");
                
                s32 nLen = 0;
                if(!CXMLEnc::Encode(pcTree, g_abyBuf, sizeof(g_abyBuf), &nLen))
                {
                    //mmculog(MMCU_EXP, "[MMCU]Session:0x%x SendMsg: CXMLEnc::Encode error!", this);
					LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB, "[MMCU]Session:0x%x SendMsg: CXMLEnc::Encode error!", this);
                    return MMCU_FALSE;
                }
                
                s32 nSend = TcpSend(m_dwNode, g_abyBuf, nLen);
                if(nSend != nLen)
                {
                    //mmculog(MMCU_EXP, "[MMCU]Session:0x%x SendMsg: TcpSend error(len:%d, nSend:%d\n)!", this, nLen,nSend);
					LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[MMCU]Session:0x%x SendMsg: TcpSend error(len:%d, nSend:%d\n)!", this, nLen,nSend);
                    return MMCU_FALSE;
                }
				LogPrint(LOG_LVL_DETAIL,MID_MCULIB_MMCULIB,"[MMCU]Session:0x%x SendMsg: OK(len:%d)!\n", this, nLen);
                //mmculog(MMCU_DEBUG, "[MMCU]Session:0x%x SendMsg: OK(len:%d)!\n", this, nLen);
                
                if(ISMMCULOG(MMCU_FILE))	
                {
                    if(g_pWF != NULL)
                    {
                        fwrite(g_abyBuf, nSend, 1,g_pWF);
                        fflush(g_pWF);
                    }
                }

				//mmculog(MMCU_DEBUG, "[MMCU]Session:0x%x Send PartList No.%d--\n", this, nStartPartIndex);
				LogPrint(LOG_LVL_DETAIL,MID_MCULIB_MMCULIB,"[MMCU]Session:0x%x Send PartList No.%d--\n", this, nStartPartIndex);
				nStartPartIndex = nStartPartIndex + nPartsPerPacket;
				g_pXMLEncTree->Reset();
            }while(bFinished == FALSE);//end of do-while

//             if(ISMMCULOG(MMCU_FILE))	
//             {
                if(g_pWF != NULL)
                {
                    fclose(g_pWF);
                }
//             }
			break;
		}
	case H_CASCADE_REINVITEPART_REQ:
		{
			pcTree = ToReInvitePartReq(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_REINVITEPART_RSP:
		{
			pcTree = ToRsp(pbyBuffer, dwBufLen, "Reinvite_Participant_Response");
			break;
		}
	case H_CASCADE_DISCONNPART_REQ:
		{
			pcTree = ToDiscPartReq(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_DISCONNPART_RSP:
		{
			pcTree = ToRsp(pbyBuffer, dwBufLen, "Drop_Participant_Response");
			break;
		}
	case H_CASCADE_DISCONNPART_NTF:
		{
			pcTree = ToDiscPartNtf(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_DELETEPART_REQ:
		{
			pcTree = ToDelPartReq(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_DELETEPART_RSP:
		{
			pcTree = ToRsp(pbyBuffer, dwBufLen, "Delete_Part_Response");
			break;
		}
	case H_CASCADE_DELETEPART_NTF:
		{
			pcTree = ToDelPartNtf(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_SETIN_REQ:
		{
			pcTree = ToSetInReq(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_SETIN_RSP:
		{
			pcTree = ToRsp(pbyBuffer, dwBufLen, "Participant_Set_In_Layout_Response");
			break;
		}
	case H_CASCADE_OUTPUT_NTF:
		{
			pcTree = ToOutputNtf(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_AUDIOINFO_REQ:
		{
			pcTree = ToAudioInfoReq(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_AUDIOINFO_RSP:
		{	
			pcTree = ToAudioInfoRsp(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_NEWSPEAKER_NTF:
		{
			pcTree = ToNewSpeakerNtf(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_VIDEOINFO_REQ:
		{
			pcTree = ToVideoInfoReq(pbyBuffer, dwBufLen);
		    break;	
		}
	case H_CASCADE_VIDEOINFO_RSP:
		{
			pcTree = ToVideoInfoRsp(pbyBuffer, dwBufLen);
			break;
		}
    case H_CASCADE_PARTMEDIACHAN_REQ:
        { 
           pcTree = ToPartMediaChanReq(pbyBuffer, dwBufLen);
           break;
        }
    case H_CASCADE_PARTMEDIACHAN_RSP:
        { 
            pcTree = ToRsp(pbyBuffer, dwBufLen, "part_media_chan_operation_response");
            break;
        }
    case H_CASCADE_PARTMEDIACHAN_NTF:
        { 
           pcTree = ToPartMediaChanNtf(pbyBuffer, dwBufLen);
            break;
        }
    case H_CASCADE_CONFVIEW_CHG_NTF:
        {
            pcTree = ToLayoutChgNtf(pbyBuffer, dwBufLen);
            break;
        }
    case H_CASCADE_SETOUT_REQ:
        {
            pcTree = ToSetOutLayoutReq(pbyBuffer, dwBufLen);
            break;
        }
    case H_CASCADE_SETOUT_RSP:
        {
            pcTree = ToRsp(pbyBuffer, dwBufLen, "participant_set_out_layout_response");
            break;
        }
    case H_CASCADE_VA_RSP:
        {
            pcTree = ToRsp(pbyBuffer, dwBufLen, "va_set_in_layout_response");
            break;
        }
    case H_CASCADE_UNDEFINED_RSP:
        {
            TDefaultRsp *pDefRsp = (TDefaultRsp *)pbyBuffer;
            pcTree = ToRsp((u8*)((TRsp *)pbyBuffer),  sizeof(TRsp), pDefRsp->m_pszLable);
            break;
        }
	case H_CASCADE_GETMCUSERVICELIST_REQ: 
		{
			pcTree = ToMcuServiceListReq(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_GETMUCCONFLIST_REQ: 
		{
			pcTree = ToMcuConfListReq(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_GETCONFGIDBYNAME_REQ: 
		{
			pcTree = ToMcuConfIdByNameReq(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_GET_CONFPROFILE_REQ: 
		{
			pcTree = ToMcuConfProfileReq(pbyBuffer, dwBufLen);
			break;
		}
//add 2007.06.13
	case H_CASCADE_SET_LAYOUT_AUTOSWITCH_REQUEST:
		{
			pcTree = ToMcuSetLayoutAutoswitchRequest(pbyBuffer, dwBufLen);
			break;
		}
	case H_CASCADE_SET_LAYOUT_AUTOSWITCH_RESPONSE:
		{
			pcTree = ToMcuSetLayoutAutoswitchResponse(pbyBuffer, dwBufLen);
			break;
		}
    case H_CASCADE_UNDEFINED_REQ:
    case H_CASCADE_UNDEFINED_NTF:
        break;
	default:
        break;	
	}

    //Specially Handled once nMsgID == H_CASCADE_PARTLIST_RSP
    if(nMsgID != (s32)H_CASCADE_PARTLIST_RSP)
    {
        if(pcTree == NULL)
        {
            return MMCU_FALSE;
        }
        
        if (nMsgID == (s32)H_CASCADE_REGUNREG_REQ || 
            nMsgID == (s32)H_CASCADE_REGUNREG_RSP)
        {
            PrintXMLTree(this, MMCU_DEBUG2, pcTree, "SendMsg");
        }
        else
        {
            PrintXMLTree(this, MMCU_ALL, pcTree, "SendMsg");
        }        

        s32 nLen = 0;
        if(!CXMLEnc::Encode(pcTree, g_abyBuf, sizeof(g_abyBuf), &nLen))
        {
            //mmculog(MMCU_EXP, "[MMCU]Session:0x%x SendMsg: CXMLEnc::Encode error!\n", this);
			LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[MMCU]Session:0x%x SendMsg: CXMLEnc::Encode error!\n", this);
            return MMCU_FALSE;
        }
        
        s32 nSend = TcpSend(m_dwNode, g_abyBuf, nLen);
        if(nSend != nLen)
        {
            //mmculog(MMCU_EXP, "[MMCU]Session:0x%x SendMsg: TcpSend error(len:%d, nSend:%d\n)!", this, nLen,nSend);
			LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[MMCU]Session:0x%x SendMsg: TcpSend error(len:%d, nSend:%d\n)!", this, nLen,nSend);
            return MMCU_FALSE;
        }
        
        if(ISMMCULOG(MMCU_FILE))	
        {
            if(g_pWF == NULL)
            {
                g_pWF = fopen(pszWF, "w");
            }
            if(g_pWF != NULL)
            {
                fwrite(g_abyBuf, nSend, 1,g_pWF);
                fclose(g_pWF);
				//fflush(g_pWF);
            }
        }
        //mmculog(MMCU_DEBUG, "[MMCU]Session:0x%x SendMsg: OK(len:%d)!\n", this, nLen);
		LogPrint(LOG_LVL_DETAIL,MID_MCULIB_MMCULIB,"[MMCU]Session:0x%x SendMsg: OK(len:%d)!\n", this, nLen);
    }
	return MMCU_OK;
}


u16  CMCUCascade::RegisterEventHandler(PCascadeHandler pMsgCallback, u32 dwUserData)
{
    m_pfAppHandler = pMsgCallback;
    m_dwUserData   = dwUserData;
    return MMCU_OK;
}

BOOL32 CMCUCascade::OnTcpIntegrity(u8 *pBuf, s32 nLen, u8 **ppbyIntPack, s32 *pnIntSize)
{
	// [3/22/2011 xliang] add protection
	if( m_pbyTcpBuf == NULL )
	{
		//mmculog(MMCU_EXP, "[MMCU] OnTcpIntegrity--m_pbyTcpBuf NULL, return FALSE!\n");
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[MMCU] OnTcpIntegrity--m_pbyTcpBuf NULL, return FALSE!\n");
		return FALSE;
	}
	
	if( m_pbyBackUp == NULL )
	{
		//mmculog(MMCU_EXP, "[MMCU] OnTcpIntegrity--m_pbyBackUp NULL, return FALSE!\n");
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[MMCU] OnTcpIntegrity--m_pbyBackUp NULL, return FALSE!\n");
		return FALSE;
	}

	const s8 * const pszHead = "<?xml";
	s32 nHeadLen = strlen(pszHead);
	const s8 * const pszTail = "</MCU_XML_API>";
	s32 nTailLen = strlen(pszTail); 

    s32 anHeadTag[MAX_PACKNUM_PERNETPACK] = {0};
	s32 nTagCount = 0;
	s32 nLastPos = 0;
	while(FindTag(pBuf + nLastPos, 
		          nLen - nLastPos, 
				  &(anHeadTag[nTagCount]), 
				  (s8*)pszHead))
	{
		nLastPos += anHeadTag[nTagCount]+nHeadLen;
		anHeadTag[nTagCount] = nLastPos - nHeadLen;
		nTagCount++;
		if(nTagCount > (MAX_PACKNUM_PERNETPACK - 1))
		{
			break;
		}
	}

	if(nTagCount == 0)
	{
		if(m_nTcpBufLen == 0)
		{
			return FALSE;
		}
		if(m_nTcpBufLen + nLen > MAX_XML_DEAL_LEN)
		{
			//mmculog(MMCU_EXP, "[MMCU]OnTcpIntegrity-Err1\n" );
			LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[MMCU]OnTcpIntegrity-Err1\n" );

			//数据丢失
			m_nTcpBufLen = 0;
			return FALSE;
		}

        memcpy(m_pbyTcpBuf+m_nTcpBufLen, pBuf, nLen);
        m_nTcpBufLen +=nLen;
        s32 nPos = 0;
		if(!FindTag(m_pbyTcpBuf, m_nTcpBufLen, &nPos, (s8*)pszTail))
		{
			return FALSE;						
		}

		memcpy(m_pbyBackUp, m_pbyTcpBuf, nPos+nTailLen);
		ppbyIntPack[0] = m_pbyBackUp;
		pnIntSize[0] = nPos+nTailLen;

		m_nTcpBufLen -= (nPos+nTailLen);
		if(m_nTcpBufLen>=0)
		{
			memcpy(m_pbyTcpBuf,  m_pbyTcpBuf+nPos+nTailLen, m_nTcpBufLen);
		}
		else
		{
			//mmculog(MMCU_EXP, "/*****m_nTcpBufLen<0*********/\n");
			LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB, "/*****m_nTcpBufLen<0*********/\n");
		}
		return TRUE;		
	}
	else
	{ 
		s32 nPos = 0;
		if(m_nTcpBufLen == 0)
		{
			for(s32 i=0; i<nTagCount-1; i++)
			{
				ppbyIntPack[i] = pBuf + anHeadTag[i];
				pnIntSize[i] =  anHeadTag[i+1]-anHeadTag[i];
			}
			if(!FindTag(pBuf+anHeadTag[nTagCount-1], nLen-anHeadTag[nTagCount-1], &nPos, (s8*)pszTail))
			{
				memcpy(m_pbyTcpBuf, pBuf+anHeadTag[nTagCount-1], nLen-anHeadTag[nTagCount-1]);
				m_nTcpBufLen = nLen-anHeadTag[nTagCount-1];
                if( nTagCount > 1 )
                {
                    return TRUE;
                }
                return FALSE;
			}
			ppbyIntPack[nTagCount-1] = pBuf + anHeadTag[nTagCount-1];
			pnIntSize[nTagCount-1] =  nPos+nTailLen;
			s32 nTailPos = anHeadTag[nTagCount-1]+nPos+nTailLen;
			if(nTailPos < nLen)
			{
				//有剩余
				memcpy(m_pbyTcpBuf+m_nTcpBufLen, 
					    pBuf + nTailPos, 
						nLen - nTailPos);
				m_nTcpBufLen += (nLen - nTailPos);
			}
			return TRUE;
		}
		else
		{
		
			if(anHeadTag[0] == 0||
				m_nTcpBufLen + anHeadTag[0] > MAX_XML_BUFFER_LEN)
			{	
				//mmculog(MMCU_EXP, "[MMCU]OnTcpIntegrity-Err2\n" );
				LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[MMCU]OnTcpIntegrity-Err2\n" );

				//数据丢失
				m_nTcpBufLen = 0;

				for(s32 i=0; i<nTagCount-1; i++)
				{
					ppbyIntPack[i] = pBuf + anHeadTag[i];
					pnIntSize[i] =  anHeadTag[i+1]-anHeadTag[i];
				}
				
				if(!FindTag(pBuf+anHeadTag[nTagCount-1], nLen-anHeadTag[nTagCount-1], &nPos, (s8*)pszTail))
				{
					memcpy(m_pbyTcpBuf, pBuf+anHeadTag[nTagCount-1], nLen-anHeadTag[nTagCount-1]);
					m_nTcpBufLen = nLen-anHeadTag[nTagCount-1];

					if( nTagCount > 1 )
					{
						return TRUE;
					}
					return FALSE;
				}
				ppbyIntPack[nTagCount-1] = pBuf + anHeadTag[nTagCount-1];
				pnIntSize[nTagCount-1] =  nPos+nTailLen;
				s32 nTailPos = anHeadTag[nTagCount-1]+nPos+nTailLen;
				if(nTailPos < nLen)
				{
					//有剩余
					memcpy(m_pbyTcpBuf+m_nTcpBufLen, 
						pBuf + nTailPos, 
						nLen - nTailPos);
					m_nTcpBufLen += (nLen - nTailPos);
				}
				return TRUE;
			}
		

			memcpy(m_pbyBackUp, m_pbyTcpBuf, m_nTcpBufLen);
			memcpy(m_pbyBackUp+m_nTcpBufLen, pBuf, anHeadTag[0]);
			ppbyIntPack[0] = m_pbyBackUp;
			pnIntSize[0] =   m_nTcpBufLen + anHeadTag[0];

			/////////////////////////////////////////2005-10-18
			m_nTcpBufLen = 0;
			/////////////////////////////////////////

			for(s32 i=1; i<nTagCount; i++)
			{
				ppbyIntPack[i] = pBuf + anHeadTag[i-1];
				pnIntSize[i] =  anHeadTag[i]-anHeadTag[i-1];
			}
			
			if(!FindTag(pBuf+anHeadTag[nTagCount-1], nLen-anHeadTag[nTagCount-1], &nPos, (s8*)pszTail))
			{				
				m_nTcpBufLen = nLen - anHeadTag[nTagCount-1];
				memcpy(m_pbyTcpBuf, pBuf+anHeadTag[nTagCount-1], m_nTcpBufLen);
				return TRUE;
			}

			ppbyIntPack[nTagCount] = pBuf + anHeadTag[nTagCount-1];
			pnIntSize[nTagCount] = nPos+nTailLen;
			
			s32 nTailPos = anHeadTag[nTagCount-1]+nPos+nTailLen;
			if(nTailPos < nLen)
			{
				m_nTcpBufLen = nLen - nTailPos;
				memcpy(m_pbyTcpBuf, pBuf+nTailPos, m_nTcpBufLen);
			}
			return TRUE;
		}
	}
}

void CMCUCascade::DoDefaultMsgResponse(CXMLTree *pXMLTree, const s8* pszLable)
{
    TDefaultRsp tRsp;
    s32  nNode                  = -1;
    TXMLNode *pNode             = NULL;
    s32 nParentNode             = pXMLTree->GetRootId();
    nParentNode = pXMLTree->FindNode(nParentNode, "request");
    if(nParentNode > 0)
    {
        nNode = pXMLTree->FindNode(nParentNode, "requestid");
        if(nNode == -1)
        {
            return;
        }
        pNode = pXMLTree->GetNode(nNode);
        tRsp.m_nReqID = StrToNum(pNode->m_pszNodeValue);
        tRsp.m_emReturnVal = emReturnValue_Ok;
        ConvertMsgLable(pszLable, tRsp.m_pszLable, MAX_MSGLABLE_LEN, emRequest,emResponse); 
        SendMsg((s32)H_CASCADE_UNDEFINED_RSP, (u8*)&tRsp, sizeof(tRsp));
    }
}

void CMCUCascade::DoRcvMsg(u8 *pBuf, s32 nLen)
{
    CXMLTree *pXMLTree = NULL;
    if(!g_pXMLDec->Decode(pBuf, nLen, &pXMLTree))
	{
		//mmculog(MMCU_EXP, "[MMCU]Session:0x%x DoRcvMsg: decode XML data error(nLen:%d)\n", this, nLen);
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[MMCU]Session:0x%x DoRcvMsg: decode XML data error(nLen:%d)\n", this, nLen);
		return;
	}
	
	//PrintXMLTree(this, MMCU_ALL, pXMLTree, "RcvMsg");
	
	s8 szTitle[XML_NODE_SIZE]={0};
    pXMLTree->GetType(szTitle, sizeof(szTitle));	
	s32 nMsgId = GetMsgIdFromName(szTitle); 

    if (nMsgId == (s32)H_CASCADE_REGUNREG_REQ || 
        nMsgId == (s32)H_CASCADE_REGUNREG_RSP)
    {
        PrintXMLTree(this, MMCU_DEBUG2, pXMLTree, "RcvMsg");
    }
    else
    {
        PrintXMLTree(this, MMCU_ALL, pXMLTree, "RcvMsg");
    }

	if(nMsgId == (s32)CASCADE_MSGTYPE_BEGINNING || nMsgId >= (s32)H_CASCADE_NOT_SUPPORTED)
	{
		DoDefaultMsgResponse(pXMLTree, szTitle);
        //mmculog(MMCU_EXP, "[MMCU]Session:0x%x msg(%s)is not Handled And/Or return default response\n", this, szTitle);
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[MMCU]Session:0x%x msg(%s)is not Handled And/Or return default response\n", this, szTitle);
		return;
	}

	switch(nMsgId)
	{
	case H_CASCADE_NONSTANDARD_REQ:
		{
			OnNonStandardMsgReq(pXMLTree);
			break;
		}
	case H_CASCADE_NONSTANDARD_RSP:
		{
			OnNonStandardMsgRsp(pXMLTree);
			break;
		}
	case H_CASCADE_NONSTANDARD_NTF:
		{
			OnNonStandardMsgNtf(pXMLTree);
			break;
		}
	case H_CASCADE_REGUNREG_REQ:
		{
            OnRegUnRegReq(pXMLTree);
			break;
		}
    case H_CASCADE_REGUNREG_RSP:
        {
            OnRegUnRegRsp(pXMLTree);
            break;
        }
    case H_CASCADE_NEWROSTER_NTF:
        { 
            OnNewNeighborNtf(pXMLTree);
            break;
        }
    case H_CASCADE_NEWPART_NTF:
        {
            OnNewPartNtf(pXMLTree);
            break;
        }
	case H_CASCADE_CALL_ALERTING_NTF:
		{
			OnCallNtf(pXMLTree);
			break;
		}
    case H_CASCADE_PARTLIST_REQ:
        { 
            OnGetPartListReq(pXMLTree);
            break;
        }
    case H_CASCADE_PARTLIST_RSP:
        { 
            OnGetPartListRsp(pXMLTree);
            break;
        }
    case H_CASCADE_VIDEOINFO_REQ:
        { 
            OnCMVideoReq(pXMLTree);
            break;
        }
    case H_CASCADE_VIDEOINFO_RSP:
        { 
            OnCMVideoRsp(pXMLTree);
            break;
          }
    case H_CASCADE_AUDIOINFO_REQ:
        { 
            OnCMAudioReq(pXMLTree);
            break;
        }
    case H_CASCADE_AUDIOINFO_RSP:
        { 
            OnCMAudioRsp(pXMLTree);
            break;
        }
    case H_CASCADE_OUTPUT_NTF:
        { 
            OnOutputLayoutNtf(pXMLTree);
            break;
        }
	case H_CASCADE_NEWSPEAKER_NTF:
		{
            OnNewSpeakerNtf(pXMLTree);
			break;
		}
    case H_CASCADE_SETIN_REQ:
        { 
            OnSetInLayoutReq(pXMLTree);
            break;
        }
    case H_CASCADE_SETIN_RSP:
        { 
            OnRsp(pXMLTree, H_CASCADE_SETIN_RSP);
            break;
        }
    case H_CASCADE_INVITEPART_REQ:
        { 
            OnInvitePartReq(pXMLTree);
            break;
        }
    case H_CASCADE_INVITEPART_RSP:
        { 
            OnRsp(pXMLTree, H_CASCADE_INVITEPART_RSP);
            break;
        }
    case H_CASCADE_REINVITEPART_REQ:
        { 
            OnReInvitePartReq(pXMLTree);
            break;
        }
    case H_CASCADE_REINVITEPART_RSP:
        { 
            OnRsp(pXMLTree, H_CASCADE_REINVITEPART_RSP);
            break;
        }
    case H_CASCADE_DELETEPART_REQ:
        { 
            OnPartDelReq(pXMLTree);
            break;
        }
    case H_CASCADE_DELETEPART_RSP:
        { 
            OnRsp(pXMLTree,  H_CASCADE_DELETEPART_RSP);
            break;
        }
    case H_CASCADE_DELETEPART_NTF:
        { 
            OnPartDelNtf(pXMLTree);
            break;
        }
    case H_CASCADE_DISCONNPART_REQ:
        { 
            OnPartDiscReq(pXMLTree);
            break;
        }
    case H_CASCADE_DISCONNPART_RSP:
        { 
            OnRsp(pXMLTree, H_CASCADE_DISCONNPART_RSP);
            break;
        }
    case H_CASCADE_DISCONNPART_NTF:
        { 
            OnPartDiscNtf(pXMLTree);
            break;
        }
        
    case H_CASCADE_PARTMEDIACHAN_REQ:
        { 
            OnPartMediaChanReq(pXMLTree);
            break;
        }
    case H_CASCADE_PARTMEDIACHAN_NTF:
        { 
            OnPartMediaChanNtf(pXMLTree);
            break;
        }
    case H_CASCADE_PARTMEDIACHAN_RSP:
        { 
            OnRsp(pXMLTree,  H_CASCADE_PARTMEDIACHAN_RSP);
            break;
        }
     case H_CASCADE_CONFVIEW_CHG_NTF:
        {
            OnLayoutChgNtf(pXMLTree);
            break;
        }
     case H_CASCADE_SETOUT_REQ:
        {
            OnSetOutLayoutReq(pXMLTree);
            break;
        }
     case H_CASCADE_SETOUT_RSP:
        {
            OnRsp(pXMLTree, H_CASCADE_SETOUT_RSP);
            break;
        } 
     case H_CASCADE_VA_REQ:
         {
			 OnVASetInReq(pXMLTree);
			 break;
		 }
	 case H_CASCADE_SET_LAYOUT_AUTOSWITCH_REQUEST:
		 {
			 OnSetLayoutAutoswitchReq(pXMLTree);
			 break;
		 }
	 case H_CASCADE_SET_LAYOUT_AUTOSWITCH_RESPONSE:
		 {
			 OnSetLayoutAutoswitchRsp(pXMLTree);
			 break;
		 }
	 default:
		 {
			 DoDefaultMsgResponse(pXMLTree, szTitle);
			 OspPrintf(TRUE, FALSE, "[MMCU]default msg(%s)is not Handled And/Or return default.\n", szTitle);
			 //return;
		 }
		 break;
	}
	
}

CXMLTree* CMCUCascade::ToRegUnRegReq(u8 *pBuf, s32 nLen)
{
	TRegUnRegReq *pReq = (TRegUnRegReq *)pBuf;
	if((u32)nLen != sizeof(TRegUnRegReq))
	{
		return NULL;
	}

    s32 nNode = AddXMLHead(g_pXMLEncTree, "Request", pReq->m_aszUserName,  pReq->m_aszUserPass);
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "RegUnreg_Neighbor_Request", NULL);
	if(nNode == -1)
	{
		return NULL;
	}


	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pReq->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
							"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"SourceConfGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"Casc_Data", CASC_DATA);
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
							"Cookie", NumToStr(m_dwCookie));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
							"Register", BOOL_STR(pReq->m_bReg));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ChairID", "000000000000000000000000");//ctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ChairPassword", "NULL");
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "MultiCascade", BOOL_STR(pReq->m_bIsSupportMuitiCasade));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}

	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}

	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pReq->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pReq->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}

	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}

	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pReq->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pReq->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}

	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToRegUnRegRsp(u8 *pBuf, s32 nLen)
{
	TRegUnRegRsp *pRsp = (TRegUnRegRsp *)pBuf;
	if((u32)nLen != sizeof(TRegUnRegRsp))
	{
		return NULL;
	}
	
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Response");
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "RegUnreg_Neighbor_Response", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pRsp->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"ConfGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"Register", BOOL_STR(pRsp->m_bReg));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ReturnValue", ReturnValToStr(pRsp->m_emReturnVal));
	if(nNode == -1)
	{
		return NULL;
	}


	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pRsp->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pRsp->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pRsp->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pRsp->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToNewRosterNtf(u8 *pBuf, s32 nLen)
{
	if((u32)nLen != sizeof(TRosterList))
	{
		return NULL;
	}

	TRosterList *pList = (TRosterList *)pBuf;
	s32 nNode = AddXMLHead(g_pXMLEncTree, "Notification");
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "New_Neigbor_Roster_Notification", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Cookie",NumToStr(m_dwPeerCookie));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"NeigborGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
		"Change", "NewPart");
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "Roster_List", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	s32 nBrotherNodeforMultiCascade = nNode;

	nParentNode = nNode;
	s32 nBrotherId = 0; //for save brotherid
	for(s32 idx=0; idx < pList->m_nCount; idx++)
	{
		if(idx == 0)
		{
			//add child Node
			nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Roster_Member", NULL);
			if(nNode == -1)
			{
				return NULL;
			}
		}
		else
		{
			//add last's brother node
            nNode = AddXMLBrother(g_pXMLEncTree, nParentNode,nBrotherId, "Roster_Member", NULL);
			if(nNode == -1)
			{
				return NULL;
			}
		}
		nBrotherId = nNode; //save brother id 
	
		s32 nChildParentNode = nNode;
		nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, 
							"GID", BuildPid(pList->m_atRoster[idx].m_dwPID,
                            pList->m_atRoster[idx].m_bLocal));
		if(nNode == -1)
		{
			return NULL;
		}

		if(pList->m_atRoster[idx].m_bLocal)
        {
            nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, 
                "OwnerGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
        }
        else
        {
            nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, 
                "OwnerGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
        }
		if(nNode == -1)
		{
			return NULL;
		}
		
        if(pList->m_atRoster[idx].m_bLocal)
        {
            nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, 
                "EntityType", "Call");
        }
        else
        {
            nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, 
                "EntityType", "RemoteCall");
        }
		if(nNode == -1)
		{
			return NULL;
		}

		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, 
							"State", ConnStateToStr(pList->m_atRoster[idx].m_emConnState));
		if(nNode == -1)
		{
			return NULL;
		}
		
		//nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, 
		//								"Change", "Newpart");
		//		if(nNode == -1)
		//		{
		//			return NULL;
		//		}
		
	
	}//end of for

	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherNodeforMultiCascade, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherIdx = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherIdx = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pList->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pList->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherIdx, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pList->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pList->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}


	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToInvitePartReq(u8 *pBuf, s32 nLen)
{
	if((u32)nLen != sizeof(TInvitePartReq) + sizeof(u8)*PART_NUM)
	{
		return NULL;
	}
	TInvitePartReq *pPart = (TInvitePartReq *)pBuf;

    //呼叫别名类型
    u8 abyAliasType[PART_NUM] = { 0 };
    memcpy(abyAliasType, pBuf + sizeof(TInvitePartReq), sizeof(u8)*PART_NUM);

	s32 nNode = AddXMLHead(g_pXMLEncTree, "Request" , pPart->m_aszUserName, pPart->m_aszUserPass);
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Invite_Participant_Request", NULL);
	if(nNode == -1)
	{
		return NULL;
	}

	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pPart->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"ChairID", "000000000000000000000000");
	if(nNode == -1)
	{
		return NULL;
	}

	//add pid
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode,
		"PID", BuildPid(pPart->m_dwPID, pPart->m_bLocal));

	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"Invite_Part_List", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	// save node for multicascascadeInfo
	s32 nParentNode4MulticasInfo = nParentNode;
	s32 nBrotherNode4MulticasInfo = nNode;

	nParentNode = nNode;
	s32 nBrotherId = -1;
	for(s32 idx=0; idx< pPart->m_nCount; idx++)
	{
		if(nBrotherId == -1)
		{
			//add child Node
			nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Invite_Part", NULL);
			if(nNode == -1)
			{
				return NULL;
			}
		}
		else
		{
			//add last's brother node
            nNode = AddXMLBrother(g_pXMLEncTree, nParentNode,nBrotherId, "Invite_Part", NULL);
			if(nNode == -1)
			{
				return NULL;
			}
		}
        nBrotherId = nNode; //save brother id
        
		nNode = AddXMLChild(g_pXMLEncTree, nBrotherId, 
						"DialStr", pPart->m_atPart[idx].m_aszDialStr);
		if(nNode == -1)
		{
			return NULL;
		}

        nNode = AddXMLBrother(g_pXMLEncTree, nBrotherId, nNode, 
            "DialStrType", NumToStr(abyAliasType[idx]));
        if(nNode == -1)
        {
            return NULL;
		}

		nNode = AddXMLBrother(g_pXMLEncTree, nBrotherId, nNode, 
								"ViaNode", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
		if(nNode == -1)
		{
			return NULL;
		}

		nNode = AddXMLBrother(g_pXMLEncTree, nBrotherId, nNode, 
						"ProtocolType", "None");
		if(nNode == -1)
		{
			return NULL;
		}

        if(pPart->m_atPart[idx].m_dwCallRate != 0)
        {
            nNode = AddXMLBrother(g_pXMLEncTree, nBrotherId, nNode, 
                "PrefCallRate", NumToStr(pPart->m_atPart[idx].m_dwCallRate*1000));
            if(nNode == -1)
            {
                return NULL;
            }
        }
		else
		{
			nNode = AddXMLBrother(g_pXMLEncTree, nBrotherId, nNode, 
                "PrefCallRate", NumToStr(-1000));
            if(nNode == -1)
            {
                return NULL;
            }
		}

		nNode = AddXMLBrother(g_pXMLEncTree, nBrotherId, nNode, 
							"Set_In_Layout_List", NULL);
		if(nNode == -1)
		{
			return NULL;
		}
        
        //advance part option
        s32 nPrevNode       = nNode;
        s32 nVPosNode       = -1;
        s32 nVposParentNode = nNode;
        for(s32 nVPos = 0; nVPos < pPart->m_atPart[idx].m_tPartVideoInfo.m_nViewCount; nVPos++)
        {
			if(nVPosNode == -1)
			{
				 nNode = AddXMLChild(g_pXMLEncTree, nVposParentNode, "V_Pos", NULL);
				 if(nNode == -1)
				 {
					  return NULL;
				 }
			}
			else
			{
			   nNode = AddXMLBrother(g_pXMLEncTree,nVposParentNode, nVPosNode, "V_Pos", NULL);
			   if(nNode == -1)
			   {
					return NULL;
			   }
			}
			nVPosNode = nNode; //hual
    
			nNode = AddXMLChild(g_pXMLEncTree, nVPosNode, "LID", 
					NumToStr(pPart->m_atPart[idx].m_tPartVideoInfo.m_anViewPos[nVPos].m_nViewID));
			if(nNode == -1)
			{
			   return NULL;
			}
    
			nNode = AddXMLBrother(g_pXMLEncTree, nVPosNode, nNode, 
					"SbFrIndx",  NumToStr(pPart->m_atPart[idx].m_tPartVideoInfo.m_anViewPos[nVPos].m_bySubframeIndex));
			if(nNode == -1)
			{
				return NULL;
			}
		}//end of for
		//advance part option
  
        nNode = AddXMLBrother(g_pXMLEncTree, nBrotherId, nPrevNode, 
								"Set_Out_Layout_Params", NULL);
		if(nNode == -1)
		{
			return NULL;
		}

		s32 nOutLayourParamNode = nNode;

        nNode = AddXMLChild(g_pXMLEncTree, nOutLayourParamNode, "LID", 
                NumToStr(pPart->m_atPart[idx].m_tPartVideoInfo.m_nOutputViewID));
        if(nNode == -1)
        {
            return NULL;
        }
            
        nNode = AddXMLBrother(g_pXMLEncTree, nOutLayourParamNode, nNode, 
                "LayoutType", "Video");
        if(nNode == -1)
        {
            return NULL;
        }
            
        nNode = AddXMLBrother(g_pXMLEncTree, nOutLayourParamNode, nNode, 
               "VideoOutputID", NumToStr(pPart->m_atPart[idx].m_tPartVideoInfo.m_nOutVideoSchemeID)); //hual
        if(nNode == -1)
        {
            return NULL;
        }
	}//end of outer for 

	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode4MulticasInfo, nBrotherNode4MulticasInfo, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pPart->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pPart->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pPart->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pPart->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}


	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToRsp(u8 *pBuf, s32 nLen, const s8 *pszLable)
{
	// [7/22/2010 xliang] 
	//va set in Rsp和
	//undefined Rsp (szLable 未知) 不同其他处理
	if( strcmp(pszLable, "Invite_Participant_Response") != 0
		||strcmp(pszLable, "Reinvite_Participant_Response") != 0
		||strcmp(pszLable, "Drop_Participant_Response") != 0
		||strcmp(pszLable, "Delete_Part_Response") != 0
		||strcmp(pszLable, "Participant_Set_In_Layout_Response") != 0
		||strcmp(pszLable, "part_media_chan_operation_response") != 0
		||strcmp(pszLable, "participant_set_out_layout_response") != 0
		)
	{
		TRsp *pRsp = (TRsp *)pBuf;
		if((u32)nLen != sizeof(TRsp))
		{
			return NULL;
		}
		
		s32 nNode = AddXMLHead(g_pXMLEncTree, "Response");
		if(nNode == -1)
		{
			return NULL;
		}
		
		s32 nParentNode = nNode;
		nNode = AddXMLChild(g_pXMLEncTree, nNode, pszLable, NULL);
		if(nNode == -1)
		{
			return NULL;
		}
		
		
		nParentNode = nNode;
		nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pRsp->m_nReqID));
		if(nNode == -1)
		{
			return NULL;
		}
		
		nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
			"ConfGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
		if(nNode == -1)
		{
			return NULL;
		}
		
		nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
			"ReturnValue", ReturnValToStr(pRsp->m_emReturnVal));
		if(nNode == -1)
		{
			return NULL;
		}

		//add multicascadeinfo
		nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
			"MultiCascadeInfo", NULL);
		if(nNode == -1)
		{
			return NULL;
		}
		
		//src and dst cascade info
		nParentNode = nNode;
		s32 nBrotherId = -1;
		s32 nChildParentNode = -1;
		u8 byLvlId = 0;
		//src
		nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
		nBrotherId = nNode;					 
		if(nNode == -1)
		{
			return NULL;
		}
		
		nChildParentNode = nNode;
		nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pRsp->m_tMsgSrc.m_byCasLevel));
		if(nNode == -1)
		{
			return NULL;
		}
		
		for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
		{
			nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pRsp->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
			if(nNode == -1)
			{
				return NULL;
			}
		}
		
		//dst
		nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
		
		if(nNode == -1)
		{
			return NULL;
		}
		
		nChildParentNode = nNode;
		nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pRsp->m_tMsgDst.m_byCasLevel));
		if(nNode == -1)
		{
			return NULL;
		}
		
		for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
		{
			nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pRsp->m_tMsgDst.m_abyMtIdentify[byLvlId]));
			if(nNode == -1)
			{
				return NULL;
			}
		}
	}
	else
	{
	
		TPartRsp *pRsp = (TPartRsp *)pBuf;
		if((u32)nLen != sizeof(TPartRsp))
		{
			return NULL;
		}
		
		s32 nNode = AddXMLHead(g_pXMLEncTree, "Response");
		if(nNode == -1)
		{
			return NULL;
		}
		
		s32 nParentNode = nNode;
		nNode = AddXMLChild(g_pXMLEncTree, nNode, pszLable, NULL);
		if(nNode == -1)
		{
			return NULL;
		}
		
		
		nParentNode = nNode;
		nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pRsp->m_nReqID));
		if(nNode == -1)
		{
			return NULL;
		}
		
		nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
			"ConfGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
		if(nNode == -1)
		{
			return NULL;
		}
		
		nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
			"ReturnValue", ReturnValToStr(pRsp->m_emReturnVal));
		if(nNode == -1)
		{
			return NULL;
		}
		
		// the following nodes are add for multiCascade support
		// add PID
		nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
			"PID", BuildPid(pRsp->m_dwPID, pRsp->m_bLocal));
		if(nNode == -1)
		{
			return NULL;
		}

		//add multicascadeinfo
		nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
			"MultiCascadeInfo", NULL);
		if(nNode == -1)
		{
			return NULL;
		}
		
		//src and dst cascade info
		nParentNode = nNode;
		s32 nBrotherId = -1;
		s32 nChildParentNode = -1;
		u8 byLvlId = 0;
		//src
		nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
		nBrotherId = nNode;					 
		if(nNode == -1)
		{
			return NULL;
		}
		
		nChildParentNode = nNode;
		nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pRsp->m_tMsgSrc.m_byCasLevel));
		if(nNode == -1)
		{
			return NULL;
		}
		
		for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
		{
			nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pRsp->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
			if(nNode == -1)
			{
				return NULL;
			}
		}
		
		//dst
		nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
		
		if(nNode == -1)
		{
			return NULL;
		}
		
		nChildParentNode = nNode;
		nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pRsp->m_tMsgDst.m_byCasLevel));
		if(nNode == -1)
		{
			return NULL;
		}
		
		for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
		{
			nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pRsp->m_tMsgDst.m_abyMtIdentify[byLvlId]));
			if(nNode == -1)
			{
				return NULL;
			}
		}
	}
	return g_pXMLEncTree;
}

//Adding One conference view profile into get_video_response message body
s32 CMCUCascade::AddConfViewToRsp(TView &tViewInfo, BOOL32 bFullInfo, s32 nParentNode, s32 nBrotherNode)
{
    s32 nNode     = -1;
    s32 nPartNode = -1;
    s32 nBrother  = -1;
    if(nBrotherNode == -1)
    {
        nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Layout_Info",	NULL);
        if(nNode == -1)
        {
            return -1;
        }
    }
    else
    {
        nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherNode, "Layout_Info",	NULL);
        if(nNode == -1)
        {
            return -1;
        }
    }
    nPartNode = nNode;

    //Adding Conference view common Head attributes
    nNode = AddXMLChild(g_pXMLEncTree, nPartNode, "LID", NumToStr(tViewInfo.m_nViewId));
    if(nNode == -1)
    {
        return -1;
    }
    
    nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, "VideoRole", "Live");
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, "Auto_Switch_Status", "Off");
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, "Auto_Switch_Time", "15");
    if(nNode == -1)
    {
        return -1;
    }
	
	BOOL32 bRet = FALSE;
	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, "Dynamic", BOOL_STR(bRet));
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, "CurrentVideoGeometryIndex", NumToStr(0));
    if(nNode == -1)
    {
        return -1;
    }
    //Adding Geometry List only if bFullInfo == True;
    if(bFullInfo)
    {
        nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, "Available_Geometries_List", NULL);
        if(nNode == -1)
        {
            return -1;
        }
        
        //Adding conference view Geometry List
        nParentNode    = nNode;
        nBrother       = -1;
        for(s32 nGeometryIndex=0; nGeometryIndex < tViewInfo.m_byGeometryCount; nGeometryIndex++)
        {
            nBrother = AddGeomtryToView(tViewInfo.m_abyGeometryInfo[nGeometryIndex], nParentNode, nBrother);
            if(nBrother == -1)
            {
                return -1;
            }
        }//end of for
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, "Display_Sub_Frames_List", NULL);
    if(nNode == -1)
    {
        return -1;
    }

    //Adding conference view Contributer Participant List
    nParentNode    = nNode;
    nBrother       = -1;
    for(s32 nVideoSrcIndex=0; nVideoSrcIndex < tViewInfo.m_byContributePIDCount; nVideoSrcIndex++)
    {
        if(nBrother == -1)
        {
            nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "SubFramePartID",
                            BuildPid(tViewInfo.m_atContributePIDs[nVideoSrcIndex].m_dwPID,
                                    tViewInfo.m_atContributePIDs[nVideoSrcIndex].m_bLocal));
            if(nNode == -1)
            {
                return -1;
            }
        }
        else
        {
            nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrother, "SubFramePartID",
                            BuildPid(tViewInfo.m_atContributePIDs[nVideoSrcIndex].m_dwPID,
                                     tViewInfo.m_atContributePIDs[nVideoSrcIndex].m_bLocal));
            if(nNode == -1)
            {
                return -1;
            }
        }
        nBrother = nNode;
    }//end of for
    
    //Adding some Conference view attributes
    nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nParentNode, 
								"CurrentSpeakerSubFrameIndex",/*currently ignored*/ "-1");
    if(nNode == -1)
    {
        return -1;
    }
    
    nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
								"CurrentSpeakerFrameColor", /*No Background Color*/"000000");
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
								"VASubFrameIndex",
            /*Voice-Activiated Mode Is Off,this field is Ineffective*/NumToStr(0));
    if(nNode == -1)
    {
        return -1;
    }

	bRet = TRUE;
    nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
        "VAModeOn", /*Defaultly,Voice-Activiated Mode Is Off*/BOOL_STR(bRet));
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, "PicS", ResToStr(tViewInfo.m_emRes));
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, "MaxPicS", ResToStr(tViewInfo.m_emRes));
    if(nNode == -1)
    {
        return -1;
    }

	bRet = FALSE; 
    nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
        "CanUpdatePicS", /*Defaultly Kdv MCU not Allowed to change view Pic Resolution*/BOOL_STR(bRet));
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
        "FrameTitleShowedState", /*Defaultly Kdv MCU not support this function*/"NA");
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, "Video_Output_List", NULL);
    if(nNode == -1)
    {
        return -1;
    }
    
    //Adding ViewScheme List into conference view profile
    nParentNode    = nNode;
    nBrother       = -1;
    for(s32 nVideoSchemeIndex=0; nVideoSchemeIndex < tViewInfo.m_byVideoSchemeCount; nVideoSchemeIndex++)
    {
       nNode = AddVideoSchemeToView(tViewInfo.m_atVideoSchemeList[nVideoSchemeIndex], nParentNode, nBrother);
       if(nNode == -1 )
       {
           return -1;
       }
       nBrother = nNode;
    }//end of for
    
   return nPartNode;
}

s32 CMCUCascade::AddVideoSchemeToView(TOutPutVideoSchemeInfo &tVideoSchemeInfo, s32 nParentNode, s32 nBrotherNode)
{
    s32 nNode            = -1;
    s32 nVideoSchemeNode = -1;

    if(nBrotherNode == -1)
    {
        nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Video_Output",	NULL);
        if(nNode == -1)
        {
            return -1;
        }
    }
    else
    {
        nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherNode, "Video_Output",	NULL);
        if(nNode == -1)
        {
            return -1;
        }
    }
    nVideoSchemeNode = nNode;

    nNode = AddXMLChild(g_pXMLEncTree, nVideoSchemeNode, "VideoOutputID",
                                          NumToStr(tVideoSchemeInfo.m_nOutputVideoSchemeID));
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nVideoSchemeNode, nNode, "CurrentBitRate", 
                                          NumToStr(tVideoSchemeInfo.m_dwMaxRate*1000));
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nVideoSchemeNode, nNode, "MaxBitRate", 
                                          NumToStr(tVideoSchemeInfo.m_dwMaxRate*1000));
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nVideoSchemeNode, nNode, "MinBitRate",
                                          NumToStr(tVideoSchemeInfo.m_dwMinRate*1000));
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nVideoSchemeNode, nNode, "CanUpdateBitRate",
                            BOOL_STR(tVideoSchemeInfo.m_bCanUpdateRate));
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nVideoSchemeNode, nNode, "CurrentFrameRate",
                                            NumToStr(tVideoSchemeInfo.m_nFrameRate));
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nVideoSchemeNode, nNode, "MaxFrameRate", 
                                            NumToStr(tVideoSchemeInfo.m_nFrameRate));
    if(nNode == -1)
    {
        return -1;
    }
	BOOL32 bRet = FALSE;
    nNode = AddXMLBrother(g_pXMLEncTree, nVideoSchemeNode, nNode, "CanUpdateFrameRate", 
                                           BOOL_STR(bRet));
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nVideoSchemeNode, nNode, "CurrentCodec", 
                                          PTypeToStr(tVideoSchemeInfo.m_emPayLoad));
    if(nNode == -1)
    {
        return -1;
    }
	
	bRet = FALSE;
    nNode = AddXMLBrother(g_pXMLEncTree, nVideoSchemeNode, nNode, "CanUpdateCodec",
                                          BOOL_STR(bRet));
    if(nNode == -1)
    {
        return -1;
    }
    //Return This VideoScheme Node Value
    return nVideoSchemeNode;
}

s32 CMCUCascade::AddGeomtryToView(u8 nSubFrameCount, s32 nParentNode, s32 nBrotherNode)
{
    s32 nNode            = -1;
    s32 nGeometryNode    = -1;
    
    if(nBrotherNode == -1)
    {
        nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Video_Geometry", NULL);
        if(nNode == -1)
        {
            return -1;
        }
    }
    else
    {
        nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherNode, "Video_Geometry", NULL);
        if(nNode == -1)
        {
            return -1;
        }
    }
    nGeometryNode = nNode;
    nParentNode   = nNode;
    
    nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "VideoGeometryPresetType", 
                                         NumToStr(nSubFrameCount*10));
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "Video_Geometry_Params", NULL);
    if(nNode == -1)
    {
        return -1;
    }

    nParentNode = nNode;
    nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "GeometryName", 
                                         NumToGeometryName(nSubFrameCount*10));
    if(nNode == -1)
    {
        return -1;
    }
    
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "AutoSelectable", "True");
    if(nNode == -1)
    {
        return -1;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "Video_Sub_Frames_List", NULL);
    if(nNode == -1)
    {
        return -1;
    }
    
    nParentNode             = nNode;
    s32 nParentNodeSubFrame = -1;
    //only one format availabe per Geometry from KDV MCU ,currently
    for(s32 nSubFrameIndex=0; nSubFrameIndex < nSubFrameCount; nSubFrameIndex++)
    {
        if(nParentNodeSubFrame == -1)
        {
            nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Video_Sub_Frame", NULL);
            if(nNode == -1)
            {
                return -1;
            }
        }
        else
        {
            nNode = AddXMLBrother(g_pXMLEncTree,nParentNode, nParentNodeSubFrame, 
                "Video_Sub_Frame", NULL);
            if(nNode == -1)
            {
                return -1;
            }
        }
       nParentNodeSubFrame = nNode;
       
       nNode = AddXMLChild(g_pXMLEncTree, nParentNodeSubFrame, "TopLeftX",
                             NumToStr(g_tGeometryList[nSubFrameCount-1][nSubFrameIndex].m_byTopLeftX));
       if(nNode == -1)
       {
           return -1;
       }

       nNode = AddXMLBrother(g_pXMLEncTree, nParentNodeSubFrame, nNode,"TopLeftY",
                             NumToStr(g_tGeometryList[nSubFrameCount-1][nSubFrameIndex].m_byTopLeftY));
       if(nNode == -1)
       {
           return -1;
       }

       nNode = AddXMLBrother(g_pXMLEncTree, nParentNodeSubFrame, nNode,"BottomRightX", 
                  NumToStr(g_tGeometryList[nSubFrameCount-1][nSubFrameIndex].m_byBottomRightX));
       if(nNode == -1)
       {
           return -1;
       }
       
       nNode = AddXMLBrother(g_pXMLEncTree, nParentNodeSubFrame, nNode,"BottomRightY",
                  NumToStr(g_tGeometryList[nSubFrameCount-1][nSubFrameIndex].m_byBottomRightY));
       if(nNode == -1)
       {
           return -1;
       }
       
       nNode = AddXMLBrother(g_pXMLEncTree, nParentNodeSubFrame, nNode,"ZOrder", 
                  NumToStr(g_tGeometryList[nSubFrameCount-1][nSubFrameIndex].m_byZOrder));
       if(nNode == -1)
       {
           return -1;
       }
    }//end of for

    //Return One of Conference View Geometry Node Value
    return nGeometryNode;
}

s32 CMCUCascade::AddPartToTree(TPart &tPart, s32 nParentNode, s32 nBrotherNode)
{
	s32 nNode = -1;
	s32 nPartNode = -1;
	if(nBrotherNode == -1)
	{
		nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Part",	NULL);
		if(nNode == -1)
		{
			return -1;
		}
	}
	else
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherNode, "Part",	NULL);
		if(nNode == -1)
		{
			return -1;
		}
	}
	nPartNode = nNode;
	
	nNode = AddXMLChild(g_pXMLEncTree, nPartNode, "PID", BuildPid(tPart.m_dwPID, tPart.m_bLocal));
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
							"PartName", tPart.m_aszPartName);
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"DialStr", tPart.m_aszDialStr);
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"IP", IPToStr(tPart.m_dwIP));
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"EPType", EPTypeToStr(tPart.m_emEPType));
	if(nNode == -1)
	{
		return -1;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"EPDesc", tPart.m_aszEPDesc ? tPart.m_aszEPDesc : "NULL");
	if(nNode == -1)
	{
		return -1;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"VIn", PTypeToStr(tPart.m_emVideoIn));
	if(nNode == -1)
	{
		return -1;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"VOut", PTypeToStr(tPart.m_emVideoOut));
	if(nNode == -1)
	{
		return -1;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"VIn2", PTypeToStr(tPart.m_emVideo2In));
	if(nNode == -1)
	{
		return -1;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"VOut2", PTypeToStr(tPart.m_emVideo2Out));
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"AIn", PTypeToStr(tPart.m_emAudioIn));
	if(nNode == -1)
	{
		return -1;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"AOut", PTypeToStr(tPart.m_emAudioOut));
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"DataM", "None");
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"VMuteIn", BOOL_STR(tPart.m_bVideoMuteIn));
	if(nNode == -1)
	{
		return -1;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"VMuteOut",BOOL_STR(tPart.m_bVideoMuteOut));
	if(nNode == -1)
	{
		return -1;
	}
	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"AMuteIn", BOOL_STR(tPart.m_bAudioMuteIn));
	if(nNode == -1)
	{
		return -1;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"AMuteOut",BOOL_STR(tPart.m_bAudioMuteOut));
	if(nNode == -1)
	{
		return -1;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"Invited",BOOL_STR(tPart.m_bInvited));
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"IsMaster",BOOL_STR(tPart.m_bMaster));
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"StartT", TimeToStr(tPart.m_tStartTime));
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"PConfGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
	if(nNode == -1)
	{
		return -1;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"ConSt", ConnStateToStr(tPart.m_emConnState));
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"BOSID", NumToStr(0));
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"QASt",  "None");
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"VoteSt",  "None");
	if(nNode == -1)
	{
		return -1;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"VideoMonitoringClassification",  NumToStr(1));
	if(nNode == -1)
	{
		return -1;
	}
	
	BOOL32 bRet = FALSE;
	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"IsManualClassification",  BOOL_STR(bRet));
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"IsFECCEnabled",  BOOL_STR(tPart.m_bFECCEnable));
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"Lang",  "English");
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"Disp_In_LOuts", NULL);
	if(nNode == -1)
	{
		return -1;
	}

    //advance part option
    s32 nVPosNode       = -1;
    s32 nVposParentNode = nNode;
    for(s32 nVPos = 0; nVPos < tPart.m_tPartVideoInfo.m_nViewCount; nVPos++)
    {
        if(nVPosNode == -1)
        {
            nNode = AddXMLChild(g_pXMLEncTree, nVposParentNode, "V_Pos", NULL);
            if(nNode == -1)
            {
                return -1;
            }
        }
        else
        {
            nNode = AddXMLBrother(g_pXMLEncTree,nVposParentNode, nVPosNode, "V_Pos", NULL);
            if(nNode == -1)
            {
                return -1;
            }
        }
        nVPosNode = nNode;

        nNode = AddXMLChild(g_pXMLEncTree, nVPosNode, "LID", 
                    NumToStr(tPart.m_tPartVideoInfo.m_anViewPos[nVPos].m_nViewID));
        if(nNode == -1)
        {
            return -1;
        }
        
        nNode = AddXMLBrother(g_pXMLEncTree, nVPosNode, nNode, 
            "SbFrIndx",  NumToStr(tPart.m_tPartVideoInfo.m_anViewPos[nVPos].m_bySubframeIndex));
        if(nNode == -1)
        {
            return -1;
        }
    }//end of for
    //advance part option
    
	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nVposParentNode, 
					  	"DispLOut",  NumToStr(tPart.m_tPartVideoInfo.m_nOutputViewID));
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"DispOPut",  NumToStr(tPart.m_tPartVideoInfo.m_nOutVideoSchemeID));
	if(nNode == -1)
	{
		return -1;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
					  	"DiscRsn",  "Normal");
	if(nNode == -1)
	{
		return -1;
	}

	// save node for multicascascadeInfo
// 	s32 nParentNode4MulticasInfo = nParentNode;
// 	s32 nBrotherNode4MulticasInfo = nNode;
	
	
	//add multicascade
	nNode = AddXMLBrother(g_pXMLEncTree, nPartNode, nNode, 
						  "MultiCascade", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//caslevel and casmtid
	//s32 nBrotherId = -1;
	s32 nChildParentNode = nNode;
	u8 byLvlId = 0;

	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslvl", NumToStr(tPart.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmt", NumToStr(tPart.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	// no dst
	//dst
// 	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
// 	
// 	if(nNode == -1)
// 	{
// 		return NULL;
// 	}
// 	
// 	nChildParentNode = nNode;
// 	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(tPart.m_tMsgDst.m_byCasLevel));
// 	if(nNode == -1)
// 	{
// 		return NULL;
// 	}
// 	
// 	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
// 	{
// 		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(tPart.m_tMsgDst.m_abyMtIdentify[byLvlId]));
// 		if(nNode == -1)
// 		{
// 			return NULL;
// 		}
// 	}

    
	return nPartNode;
}

CXMLTree* CMCUCascade::ToNewPartNtf(u8 *pBuf, s32 nLen)
{
	if((u32)nLen != sizeof(TPart))
	{
		return NULL;
	}
	
	TPart *pPart = (TPart *)pBuf;
	s32 nNode = AddXMLHead(g_pXMLEncTree, "Notification");
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "New_Participant_Notification", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Cookie",NumToStr(m_dwPeerCookie));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}

	if(AddPartToTree(*pPart, nParentNode, nNode) < 0)
	{
		return NULL;
	}
	
	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToCallNtf(u8 *pBuf, s32 nLen)
{
	if((u32)nLen != sizeof(TCallNtf))
	{
		return NULL;
	}
	
	TCallNtf *pNtf = (TCallNtf *)pBuf;
	s32 nNode = AddXMLHead(g_pXMLEncTree, "Notification");
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Call_Alerting_Notification", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Cookie",NumToStr(m_dwPeerCookie));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
					"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
					"PID", BuildPid(pNtf->m_dwPID, pNtf->m_bLocal));
	if(nNode == -1)
	{	
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
				"DialStr", pNtf->m_aszDialStr);
	if(nNode == -1)
	{	
		return NULL;
	}

	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pNtf->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pNtf->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pNtf->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pNtf->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}

	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToPartListReq(u8 *pBuf, s32 nLen)
{
	if((u32)nLen != sizeof(TReq))
	{
		return NULL;
	}

	TReq *pReq = (TReq *)pBuf;
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Request", pReq->m_aszUserName, pReq->m_aszUserPass);
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Get_Participant_List_Request", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pReq->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	BOOL32 bRet = FALSE;
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
					"GetExtendedInfo", BOOL_STR(bRet));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"SnapShotValue", "0");
	if(nNode == -1)
	{
		return NULL;
	}


	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"SenderGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
							"ExpandedMCUs", NULL);
	if(nNode == -1)
	{
		return NULL;
	}

	// save node for multicascascadeInfo
	s32 nParentNode4MulticasInfo = nParentNode;
	s32 nBrotherNode4MulticasInfo = nNode;

	nParentNode = nNode;
    //adopt expand-all-mcu mode; possible be changed to be "None" by singleCascading
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "ExpandMCUMode", "All");
	if(nNode == -1)
	{
		return NULL;
	}

	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode4MulticasInfo, nBrotherNode4MulticasInfo, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pReq->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pReq->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pReq->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pReq->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}


	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToPartListRsp(u8 *pBuf, s32 nLen, BOOL32 &bFinished, s32 nStartPartIndex, s32 nEncodedPartNum )
{
    TPartListRsp *pRsp = (TPartListRsp *)pBuf;
    s32 nUpperBoundary  = 0;
    if((u32)nLen != sizeof(TPartListRsp))
    {
        return NULL;
    }

    //Whether the Last netpack of participant list,
    //Or whether the participant list is already Sent completely
    if(pRsp->m_dwPartCount - nEncodedPartNum <= nStartPartIndex)
    {
        nUpperBoundary = pRsp->m_dwPartCount;
        bFinished = TRUE;
    }
    else
    {
        nUpperBoundary = nStartPartIndex + nEncodedPartNum ;
        bFinished = FALSE;
    }
 
	if(ISMMCULOG(MMCU_DEBUG))
	{
		OspPrintf(TRUE, FALSE, "Session:0x%x Send PartList Total %d,Start %d, End %d, perPacket %d",
			this, pRsp->m_dwPartCount, nStartPartIndex, nUpperBoundary-1, nEncodedPartNum);
    }

    s32 nNode = AddXMLHead(g_pXMLEncTree, "Response");
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Get_Participant_List_Response", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pRsp->m_nReqID));
    if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
							"ConfGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ReturnValue", ReturnValToStr(pRsp->m_emReturnVal));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "IsLastResponseBlock", BOOL_STR(bFinished));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "SnapShotValue", NumToStr(pRsp->m_nSnapShot));
	if(nNode == -1)
	{
		return NULL;
	}

	BOOL32 bRet = FALSE;
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "IsChairValid", BOOL_STR(bRet));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "Participants_List", NULL);
	if(nNode == -1)
	{
		return NULL;
	}

	// save node for multicascadeinfo
	s32 nParentNode4MulticasInfo = nParentNode;
	s32 nBrotherNode4MulticasInfo = nNode;

	nParentNode = nNode;
	s32 nBrother= -1;
	for(s32 idx = nStartPartIndex; idx< nUpperBoundary; idx++)
	{
		nBrother = AddPartToTree(pRsp->m_atParts[idx], nParentNode, nBrother);
		if(nBrother == -1)
		{
			return NULL;
		}
	}

	
	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode4MulticasInfo, nBrotherNode4MulticasInfo, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pRsp->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pRsp->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pRsp->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pRsp->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}

    return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToReInvitePartReq(u8 *pBuf, s32 nLen)
{
	if((u32)nLen != sizeof(TPartReq))
	{
		return NULL;
	}
	
	TPartReq *pPartID = (TPartReq *)pBuf;
	s32 nNode = AddXMLHead(g_pXMLEncTree, "Request", pPartID->m_aszUserName, pPartID->m_aszUserPass);
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Reinvite_Participant_Request", NULL);
	if(nNode == -1)
	{
		return NULL;
	}

	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pPartID->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"PID", BuildPid(pPartID->m_dwPID, pPartID->m_bLocal));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"ChairID", "000000000000000000000000");
	if(nNode == -1)
	{
		return NULL;
	}

	
	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pPartID->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pPartID->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pPartID->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pPartID->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}

	return g_pXMLEncTree;
}

CXMLTree *CMCUCascade::ToDiscPartReq(u8 *pBuf, s32 nLen)
{
	TPartReq *pPartId= (TPartReq *)pBuf;
	if((u32)nLen != sizeof(TPartReq))
	{
		return NULL;
	}
	
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Request", pPartId->m_aszUserName, pPartId->m_aszUserPass);
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Drop_Participant_Request", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pPartId->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
		"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "PID", BuildPid(pPartId->m_dwPID, pPartId->m_bLocal));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ChairID", "000000000000000000000000");
	if(nNode == -1)
	{
		return NULL;
	}

	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pPartId->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pPartId->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pPartId->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pPartId->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToDiscPartNtf(u8 *pBuf, s32 nLen)
{
	if((u32)nLen != sizeof(TDiscPartNF))
	{
		return NULL;
	}
	
	TDiscPartNF *ptDiscPartNF = (TDiscPartNF *)pBuf;
	s32 nNode = AddXMLHead(g_pXMLEncTree, "Notification");
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Participant_Disconnected_Notification", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Cookie",NumToStr(m_dwPeerCookie));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
							"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
							"PID", BuildPid(ptDiscPartNF->m_dwPID, ptDiscPartNF->m_bLocal));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
							"NewPID", BuildPid(ptDiscPartNF->m_dwNewPID,  ptDiscPartNF->m_bLocal));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"DiscRsn", "Normal");
	if(nNode == -1)
	{
		return NULL;
	}
	
	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(ptDiscPartNF->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(ptDiscPartNF->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(ptDiscPartNF->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(ptDiscPartNF->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}

	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToDelPartReq(u8 *pBuf, s32 nLen)
{
    TPartReq *pPartId= (TPartReq *)pBuf;
	if((u32)nLen != sizeof(TPartReq))
	{
		return NULL;
	}
	
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Request", pPartId->m_aszUserName, pPartId->m_aszUserPass);
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Delete_Part_Request", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pPartId->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ChairID", "000000000000000000000000");
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "Delete_Part_List",NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode,
		"PID", BuildPid(pPartId->m_dwPID, pPartId->m_bLocal));
	if(nNode == -1)
	{
		return NULL;
	}
	
	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pPartId->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pPartId->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pPartId->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pPartId->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	
	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToDelPartNtf(u8 *pBuf, s32 nLen)
{
    if((u32)nLen != sizeof(TDelPartNF))
    {
        return NULL;
    }

	TDelPartNF *ptDelPartNF = (TDelPartNF *)pBuf;
	//TPartID *ptDelPartNF = (TPartID *)pBuf;

	s32 nNode = AddXMLHead(g_pXMLEncTree, "Notification");
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Participant_Deleted_Notification", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Cookie",NumToStr(m_dwPeerCookie));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
	                    	"PID", BuildPid(ptDelPartNF->m_dwPID, ptDelPartNF->m_bLocal));
	if(nNode == -1)
	{
		return NULL;
	}
		
	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(ptDelPartNF->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(ptDelPartNF->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(ptDelPartNF->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(ptDelPartNF->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}

	return g_pXMLEncTree;	
}

CXMLTree* CMCUCascade::ToSetInReq(u8 *pBuf, s32 nLen)
{
	TSetInReq *pSetIn= (TSetInReq *)pBuf;
	if((u32)nLen != sizeof(TSetInReq))
	{
		return NULL;
	}
	
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Request", pSetIn->m_aszUserName, pSetIn->m_aszUserPass);
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Participant_Set_In_Layout_Request", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, 
		         "SenderGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
								"RequestID", NumToStr(pSetIn->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ChairID", "000000000000000000000000");
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode,nNode, 
						"PID", BuildPid(pSetIn->m_dwPID, pSetIn->IsLocalPart()));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode,nNode, 
							"LID", NumToStr(pSetIn->m_nViewID));
	if(nNode == -1)
	{
		return NULL;
	}

    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode,nNode, 
        "LayoutType", "Video");
    if(nNode == -1)
    {
        return NULL;
    }
    
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode,nNode, 
        "VideoRole", "Live");
    if(nNode == -1)
    {
        return NULL;
    }
    
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode,nNode, 
        "SbFrIndx", NumToStr(pSetIn->m_bySubFrameIndex));
    if(nNode == -1)
    {
        return NULL;
    }
	
	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pSetIn->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pSetIn->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pSetIn->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pSetIn->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}

	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToOutputNtf(u8 *pBuf, s32 nLen)
{
	if((u32)nLen != sizeof(TPartOutputNtf))
	{
		return NULL;
	}
	
	TPartOutputNtf *ptOutputPart = (TPartOutputNtf *)pBuf;
	s32 nNode = AddXMLHead(g_pXMLEncTree, "Notification");
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Participant_Output_Layout_Set_Notification", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Cookie",NumToStr(m_dwPeerCookie));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}

    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
					    "PID", BuildPid(ptOutputPart->m_dwPID,ptOutputPart->m_bLocal));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"LID", NumToStr(ptOutputPart->m_nViewID));
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(ptOutputPart->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtId", NumToStr(ptOutputPart->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(ptOutputPart->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtId", NumToStr(ptOutputPart->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}

	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToNewSpeakerNtf(u8 *pBuf, s32 nLen)
{
    if((u32)nLen != sizeof(TNewSpeakerNtf))
    {
        return NULL;
    }
	
	TNewSpeakerNtf *ptNewSpeakPart = (TNewSpeakerNtf *)pBuf;
	s32 nNode = AddXMLHead(g_pXMLEncTree, "Notification");
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "New_Speaker_Notification", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Cookie",NumToStr(m_dwPeerCookie));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
		"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
							"PID", BuildPid(ptNewSpeakPart->m_dwPID,ptNewSpeakPart->m_bLocal));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
		"MixerID", NumToStr(ptNewSpeakPart->m_nMixerID));
	if(nNode == -1)
	{
		return NULL;
	}
	return g_pXMLEncTree;		
}


CXMLTree* CMCUCascade::ToAudioInfoReq(u8 *pBuf, s32 nLen)
{
    TReq *pReq = (TReq *)pBuf;

    if((u32)nLen != sizeof(TReq))
    {
        return NULL;
    }

    s32 nNode = AddXMLHead(g_pXMLEncTree, "Request", pReq->m_aszUserName, pReq->m_aszUserPass);
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Get_Conference_Audio_Info_Request", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pReq->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"SnapShotValue", NumToStr(0));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"SenderGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
	if(nNode == -1)
	{
		return NULL;
	}

	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pReq->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pReq->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pReq->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pReq->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}

	return g_pXMLEncTree;	
}

CXMLTree* CMCUCascade::ToAudioInfoRsp(u8 *pBuf, s32 nLen)
{
	
	TConfAudioInfo *pRsp = (TConfAudioInfo *)pBuf;
	if((u32)nLen != sizeof(TConfAudioInfo))
	{
		return NULL;
	}
	
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Response");
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Get_Conference_Audio_Info_Response", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pRsp->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"ConfGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ReturnValue", ReturnValToStr(pRsp->m_emReturnVal));
	if(nNode == -1)
	{
		return NULL;
	}
	
	BOOL32 bRet = TRUE;
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "IsLastResponseBlock", BOOL_STR(bRet));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "SnapShotValue", NumToStr(pRsp->m_nSnapShot));
	if(nNode == -1)
	{
		return NULL;
	}
	
	bRet = FALSE;
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "IsChairValid", BOOL_STR(bRet));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "Audio_Info", NULL);
	if(nNode == -1)
	{
		return NULL;
	}

	// save node for multicascascadeInfo
	s32 nParentNode4MulticasInfo = nParentNode;
	s32 nBrotherNode4MulticasInfo = nNode;

	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode,
            "DefaultAudioMixerID", NumToStr(pRsp->m_tMixerList[pRsp->m_byDefaultMixerIndex].m_nMixerID));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "NumOfAudioMixers", NumToStr(pRsp->m_byMixerCount));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "Mixers_Info_List", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
    
	nParentNode       = nNode;
    s32 nBrotherNode  = -1;
    for(s32 nMixerIndex =0; nMixerIndex < pRsp->m_byMixerCount; nMixerIndex++)
    {
        if(nBrotherNode == -1)
        {
            nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Mixers_Info", NULL);
            if(nNode == -1)
            {
                return NULL;
            }
        }
        else
        {
            nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherNode, "Mixers_Info", NULL);
            if(nNode == -1)
            {
                return NULL;
            }
        }
        
        nBrotherNode = nNode;
        nNode = AddXMLChild(g_pXMLEncTree, nBrotherNode, "MixerID", NumToStr(pRsp->m_tMixerList[nMixerIndex].m_nMixerID));
        if(nNode == -1)
        {
            return NULL;
        }

        nNode = AddXMLBrother(g_pXMLEncTree, nBrotherNode, nNode, 
            "CurrentSpeaker", BuildPid(pRsp->m_tMixerList[nMixerIndex].m_dwSpeakerPID, pRsp->m_tMixerList[nMixerIndex].m_bLocal));
        if(nNode == -1)
        {
            return NULL;
        }
    }//end of for

	
	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode4MulticasInfo, nBrotherNode4MulticasInfo, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pRsp->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pRsp->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pRsp->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pRsp->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}

	return g_pXMLEncTree;	
}


CXMLTree* CMCUCascade::ToVideoInfoReq(u8 *pBuf, s32 nLen)
{
	TConfVideInfoReq *pReq = (TConfVideInfoReq *)pBuf;
    
    if ((u32)nLen != sizeof(TConfVideInfoReq))
    {
        return NULL;
    }

    s32 nNode = AddXMLHead(g_pXMLEncTree, "Request", pReq->m_aszUserName, pReq->m_aszUserPass);
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Get_Conference_Video_Info_Request", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pReq->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"SnapShotValue", NumToStr(0));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"SenderGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
	if(nNode == -1)
	{
		return NULL;
	}

    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
        "FullInfo", BOOL_STR(pReq->m_bFullInfo));
    if(nNode == -1)
    {
        return NULL;
    }

	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pReq->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pReq->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pReq->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pReq->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}

    return g_pXMLEncTree;	
}

CXMLTree* CMCUCascade::ToVideoInfoRsp(u8 *pBuf, s32 nLen)
{
	TConfVideoInfo *pRsp = (TConfVideoInfo *)pBuf;
	if((u32)nLen != sizeof(TConfVideoInfo))
	{
		return NULL;
	}
	
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Response");
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Get_Conference_Video_Info_Response", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pRsp->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						"ConfGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ReturnValue", ReturnValToStr(pRsp->m_emReturnVal));
	if(nNode == -1)
	{
		return NULL;
	}

	//Send The XML Trunk In several TCP packet:please alter here
	BOOL32 bRet = TRUE;
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "IsLastResponseBlock", BOOL_STR(bRet));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "SnapShotValue", NumToStr(pRsp->m_nSnapShot));
	if(nNode == -1)
	{
		return NULL;
	}
	
	bRet = FALSE;
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "IsChairValid", BOOL_STR(bRet));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "Video_Info", NULL);
	if(nNode == -1)
	{
		return NULL;
	}

	// save node for multicascascadeInfo
	s32 nParentNode4MulticasInfo = nParentNode;
	s32 nBrotherNode4MulticasInfo = nNode;

	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "DefaultLID",
                       NumToStr(pRsp->m_atViewInfo[pRsp->m_byDefaultViewIndex].m_nViewId));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "Layouts_Info_List", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
    
    nParentNode = nNode;
    s32 nBrother= -1;
    for(s32 nViewindex=0; nViewindex < pRsp->m_byConfViewCount; nViewindex++)
    {
        nBrother = AddConfViewToRsp(pRsp->m_atViewInfo[nViewindex], pRsp->m_bFullInfo, nParentNode, nBrother);
        if(nBrother == -1)
        {
            return NULL;
        }
    }
	
	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode4MulticasInfo, nBrotherNode4MulticasInfo, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pRsp->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pRsp->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pRsp->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pRsp->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}

    return g_pXMLEncTree;	
}

CXMLTree* CMCUCascade::ToSetOutLayoutReq(u8 *pBuf, s32 nLen)
{
    TSetOutReq *pReq = (TSetOutReq *)pBuf;
    if((u32)nLen != sizeof(TSetOutReq))
    {
        return NULL;
    }
    
	s32 nNode = AddXMLHead(g_pXMLEncTree, "Request", pReq->m_aszUserName, pReq->m_aszUserPass);
    if(nNode == -1)
    {
        return NULL;
    }
    
    s32 nParentNode = nNode;
    nNode = AddXMLChild(g_pXMLEncTree, nNode, "Participant_Set_Out_Layout_Request", NULL);
    if(nNode == -1)
    {
        return NULL;
    }
    
    nParentNode = nNode;
    nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pReq->m_nReqID));
    if(nNode == -1)
    {
        return NULL;
    }
    
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
        "ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
    if(nNode == -1)
    {
        return NULL;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ChairID", "000000000000000000000000");
    if(nNode == -1)
    {
        return NULL;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "Out_Layout_Part_List", NULL);
    if(nNode == -1)
    {
        return NULL;
    }

	// save node for multicascascadeInfo
	s32 nParentNode4MulticasInfo = nParentNode;
	s32 nBrotherNode4MulticasInfo = nNode;

    nParentNode = nNode;
    s32 nBrotherNode = -1;

    for(s32 nPartIndex = 0; nPartIndex < pReq->m_dwPartCount; nPartIndex++)    
    {
        if(nBrotherNode == -1)
        {
            nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Out_Layout_Part", NULL);
            if(nNode == -1)
            {
                return NULL;
            }
        }
        else
        {
            nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherNode, "Out_Layout_Part", NULL);
            if(nNode == -1)
            {
                return NULL;
            }
        }

        nBrotherNode = nNode;

        nNode = AddXMLChild(g_pXMLEncTree, nBrotherNode, "PID",
                     BuildPid(pReq->m_tPartOutInfo[nPartIndex].m_dwPID, pReq->m_tPartOutInfo[nPartIndex].m_bLocal));
        if(nNode == -1)
        {
            return NULL;
        }
        
        nNode = AddXMLBrother(g_pXMLEncTree, nBrotherNode, nNode,
            "LID", NumToStr(pReq->m_tPartOutInfo[nPartIndex].m_nOutViewID));
        if(nNode == -1)
        {
            return NULL;
        }        

        nNode = AddXMLBrother(g_pXMLEncTree, nBrotherNode, nNode,
            "LayoutType", "Video");
        if(nNode == -1)
        {
            return NULL;
        }        

        nNode = AddXMLBrother(g_pXMLEncTree, nBrotherNode, nNode,
            "VideoOutputID", "0"/*NumToStr(pReq->m_tPartOutInfo[nPartIndex].m_nOutVideoSchemeID)*/);
        if(nNode == -1)
        {
            return NULL;
        }  
        
    }//end of for


	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode4MulticasInfo, nBrotherNode4MulticasInfo, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pReq->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pReq->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pReq->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pReq->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}

    return g_pXMLEncTree;
}


CXMLTree* CMCUCascade::ToLayoutChgNtf(u8 *pBuf, s32 nLen)
{
    TConfViewChgNtf *ptNtf = (TConfViewChgNtf *)pBuf;
    if((u32)nLen != sizeof(TConfViewChgNtf))
    {
        return NULL;
    }
    
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Notification");
    if(nNode == -1)
    {
        return NULL;
    }
    
    s32 nParentNode = nNode;
    nNode = AddXMLChild(g_pXMLEncTree, nNode, "Conference_Layout_Changed_Notification", NULL);
    if(nNode == -1)
    {
        return NULL;
    }
    
    nParentNode = nNode;
    nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Cookie", NumToStr(m_dwPeerCookie));
    if(nNode == -1)
    {
        return NULL;
    }
    
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
        "ConfGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
    if(nNode == -1)
    {
        return NULL;
    }
    
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "LID", NumToStr(ptNtf->m_nViewID));
    if(nNode == -1)
    {
        return NULL;
    }
    
    //Adding One Changed Geometry Into notification
    nNode = AddGeomtryToView(ptNtf->m_dwSubFrameCount, nParentNode, -1);
    if(nNode == -1)
    {
        return NULL;
    }
    
    return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToPartMediaChanNtf(u8 *pBuf, s32 nLen)
{
    TPartMediaChanNtf *pNtf  = (TPartMediaChanNtf *)pBuf;
    if((u32)nLen != sizeof(TPartMediaChanNtf))
    {
        return NULL;
    }
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Notification");
    if(nNode == -1)
    {
        return NULL;
    }

    s32 nParentNode = nNode;
    nNode = AddXMLChild(g_pXMLEncTree, nNode,"Participant_Media_Muted_Unmuted_Notification", NULL);
    if(nNode == -1)
    {
        return NULL;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
        "Cookie", NumToStr(m_dwPeerCookie));
    if(nNode == -1)
    {
        return NULL;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
        "ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
    if(nNode == -1)
    {
        return NULL;
    }
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "PID", BuildPid(pNtf->m_dwPID, pNtf->m_bLocal));
    if(nNode == -1)
    {
        return NULL;
    }
    
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ChannelMediaType", MediaTypeToStr(pNtf->m_emMediaType));
    if(nNode == -1)
    {
        return NULL;
    }
    
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ChannelDirection", MuteDirectToStr(pNtf->m_emMuteDirection));
    if(nNode == -1)
    {
        return NULL;
    }
    
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "MuteOn", BOOL_STR(pNtf->m_bMute));
    if(nNode == -1)
    {
        return NULL;
    }
    return g_pXMLEncTree;   
}

CXMLTree* CMCUCascade::ToPartMediaChanReq(u8 *pBuf, s32 nLen)
{
    TPartMediaChanReq *pReq  = (TPartMediaChanReq *)pBuf;
    if((u32)nLen != sizeof(TPartMediaChanReq))
    {
        return NULL;
    }
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Request" , pReq->m_aszUserName, pReq->m_aszUserPass);
    if(nNode == -1)
    {
        return NULL;
    }
    
    s32 nParentNode = nNode;
    nNode = AddXMLChild(g_pXMLEncTree, nNode,"Part_Media_Chan_Operation_Request", NULL);
    if(nNode == -1)
    {
        return NULL;
    }
    
    nParentNode = nNode;
    nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pReq->m_nReqID));
    if(nNode == -1)
    {
        return NULL;
    }
    
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
        "ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
    if(nNode == -1)
    {
        return NULL;
    }
    
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
        "ChairID", "000000000000000000000000");
    if(nNode == -1)
    {
        return NULL;
    }
    
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "PID", BuildPid(pReq->m_dwPID, pReq->m_bLocal));
    if(nNode == -1)
    {
        return NULL;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ChannelMediaType", MediaTypeToStr(pReq->m_emMediaType));
    if(nNode == -1)
    {
        return NULL;
    }

    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ChannelDirection", MuteDirectToStr(pReq->m_emMuteDirection));
    if(nNode == -1)
    {
        return NULL;
    }
    
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "MuteOn", BOOL_STR(pReq->m_bMute));
    if(nNode == -1)
    {
        return NULL;
    }

	//add multicascadeinfo
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "MultiCascadeInfo", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	//src and dst cascade info
	nParentNode = nNode;
	s32 nBrotherId = -1;
	s32 nChildParentNode = -1;
	u8 byLvlId = 0;
	//src
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "src", NULL);
	nBrotherId = nNode;					 
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pReq->m_tMsgSrc.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pReq->m_tMsgSrc.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}
	
	//dst
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nBrotherId, "dst", NULL);
	
	if(nNode == -1)
	{
		return NULL;
	}
	
	nChildParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nChildParentNode, "caslevel", NumToStr(pReq->m_tMsgDst.m_byCasLevel));
	if(nNode == -1)
	{
		return NULL;
	}
	
	for(byLvlId = 0; byLvlId < MAX_CASCADELEVEL; byLvlId ++)
	{
		nNode = AddXMLBrother(g_pXMLEncTree, nChildParentNode, nNode, "casmtid", NumToStr(pReq->m_tMsgDst.m_abyMtIdentify[byLvlId]));
		if(nNode == -1)
		{
			return NULL;
		}
	}

    return g_pXMLEncTree;
}


//add 2007.06.13
CXMLTree* CMCUCascade::ToMcuSetLayoutAutoswitchRequest(u8* pBuf, s32 nLen)
{
	TAutoswitchReq *pReq = (TAutoswitchReq *)pBuf;
	if((u32)nLen != sizeof(TAutoswitchReq))
	{
		return NULL;
	}
	//该请求用户:密码固定.
	pReq->SetUserNameAndPass(RAD_USER, RAD_PASS);
	
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Request", pReq->m_aszUserName,  pReq->m_aszUserPass);
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Set_Layout_Autoswitch_Request", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pReq->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
		"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
        "ChairID", "000000000000000000000000");
    if(nNode == -1)
    {
        return NULL;
    }
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
		"SenderGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, "LID", NumToStr(pReq->m_nLid));
    if(nNode == -1)
    {
		return NULL;
    }
	
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, "SwitchOn", BOOL_STR(pReq->m_bAutoSwitchOn));
    if(nNode == -1)
    {
		return NULL;
    }
	
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, "Auto_Switch_Time", NumToStr(pReq->m_nAutoSwitchTime));
    if(nNode == -1)
    {
		return NULL;
    }
	
    nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, "Auto_Switch_Level", NumToStr(pReq->m_nAutoSwitchLevel));
    if(nNode == -1)
    {
		return NULL;
    }
	
	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToMcuSetLayoutAutoswitchResponse(u8* pBuf, s32 nLen)
{
	TAutoswitchRsp *pRsp = (TAutoswitchRsp *)pBuf;
	if((u32)nLen != sizeof(TAutoswitchRsp))
	{
		return NULL;
	}

    s32 nNode = AddXMLHead(g_pXMLEncTree, "Response");
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Set_Layout_Autoswitch_Response", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pRsp->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
		"ConfGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ReturnValue", ReturnValToStr(pRsp->m_emReturnVal));
	if(nNode == -1)
	{
		return NULL;
	}
    
    return g_pXMLEncTree;
}


s8* CMCUCascade::BuildPid(u32 dwPid, BOOL32 bLocal)
{
	if(dwPid == 0)
	{
		memset(g_abyPid, 0, sizeof(g_abyPid));
	}
	else
	{
        //added by juny
        if(dwPid == 0xffffffff)
        {
           dwPid = 0;
        }
        //added by juny
        
        u32 dwLocalPid = htonl(dwPid);
		u8 *pConfId = bLocal ? m_abySrcConfID : m_abyDstConfID;
		memcpy(g_abyPid, pConfId, sizeof(g_abyPid));
		memcpy(g_abyPid+PID_POS, &dwLocalPid, sizeof(dwLocalPid));
	}
	return OctToStr2(g_abyPid, sizeof(g_abyPid));
}

BOOL32 CMCUCascade::ExtractPid(const s8* pszPID, u32 *pdwPid, BOOL32 *pbLocal)
{
	u8 abyGID[CONFID_BYTELEN] = {0};
	if((u32)Str2ToOct(pszPID, abyGID, sizeof(abyGID)) != sizeof(abyGID))
	{
		return FALSE;
	}

	*pdwPid = ntohl(*((u32 *)(abyGID+PID_POS)));

    //adding by JunYi
    if(*pdwPid == 0)
    {
        *pdwPid=0xffffffff;
    }
    //adding by Junyi

	if(pbLocal != NULL)
	{       
		memset(abyGID+PID_POS, 0, sizeof(u32));
		if(memcmp(m_abySrcConfID, abyGID, sizeof(abyGID)) == 0)
		{
            *pbLocal = TRUE;
		}
		else if(memcmp(m_abyDstConfID, abyGID, sizeof(abyGID)) == 0)
		{
			
            *pbLocal = FALSE;
		}
		else
		{
			*pdwPid=0xffffffff;
			return FALSE;
		}
	}
	return TRUE;
}

void NewConnection(u32 hListenNode, u32 hNode, u32* phAppNode)
{
	printf("[NewConnection]hListenNode.%d,hNode.%d,phAppNode.%p\n",hListenNode,hNode,phAppNode);

	CSemLock lock(&g_hSem);
	//s32 nMaxCascadeNum = sizeof(g_apcCascadeInstance)/sizeof(g_apcCascadeInstance[0]);
/*
	for(; index<nMaxCascadeNum; index++)
    {
        if(g_apcCascadeInstance[index] == NULL)
		{
			continue;
		}
        if(g_apcCascadeInstance[index]->m_dwListenNode == hListenNode)
        {
			break;
        }
	}
	if(index >= nMaxCascadeNum)
	{
		return;
	}
*/

    //phAppNode = NULL;

/*
    OspSemTake(g_apcCascadeInstance[index]->m_hSem);
	CMCUCascade *pMMcu = g_apcCascadeInstance[index];
	
	if(pMMcu->m_dwNode != INVALID_NODE)
	{
		TcpDisconnect(hNode);
		OspSemGive(pMMcu->m_hSem);
		return;
	}
    pMMcu->m_dwNode = hNode;
    *phAppNode = (u32)pMMcu;
    OspSemGive(g_apcCascadeInstance[index]->m_hSem);
		
	if(g_apcCascadeInstance[index]->m_pfAppHandler != NULL)
	{
		g_apcCascadeInstance[index]->m_pfAppHandler(H_CASCADE_NEW_CALL_NTF,
												NULL,
												0,
												g_apcCascadeInstance[index]->m_dwUserData);
	}
*/

}

CMCUCascade *RegMsgAssociateWithMMcu(u8 *pBuf, s32 nLen)
{
	CXMLTree *pXMLTree = NULL;
	if(!g_pXMLDec->Decode(pBuf, nLen, &pXMLTree))
	{
		//mmculog(MMCU_EXP, "[MMCU]DoRcvMsg: decode XML data error(nLen:%d)\n", nLen);
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[MMCU]DoRcvMsg: decode XML data error(nLen:%d)\n", nLen);
		return NULL;
	}

	PrintXMLTree(NULL, MMCU_ALL, pXMLTree, "RcvMsg");
	

	s8 szTitle[XML_NODE_SIZE]={0};
	pXMLTree->GetType(szTitle, sizeof(szTitle));	
	
	s32 nMsgId = GetMsgIdFromName(szTitle); 
	if(nMsgId != (s32)H_CASCADE_REGUNREG_REQ )
	{
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[MMCU] First Msg is Not H_CASCADE_REGUNREG_REQ (nLen:%d)\n", nLen);
		//mmculog(MMCU_EXP, "[MMCU] First Msg is Not H_CASCADE_REGUNREG_REQ (nLen:%d)\n", nLen);
		return NULL;
	}

    BOOL32 bRegister = FALSE;
    u8	abySrcConfID[CONFID_BYTELEN];
    u8  abyDstConfID[CONFID_BYTELEN];
    TXMLNode *pNode = NULL;
	s32 nParentNode = pXMLTree->GetRootId();
    s32 nNode       = -1;

	nNode = pXMLTree->FindNode(nParentNode,"sourceconfgid");
	if(nNode == -1)
	{
		//mmculog(MMCU_EXP, "[ERR] Find sourceconfgid failed(nLen:%d)\n", nLen);
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[ERR] Find sourceconfgid failed(nLen:%d)\n", nLen);
        return NULL;
	}
	pNode=pXMLTree->GetNode(nNode);
	if(pNode->m_pszNodeValue != NULL)
	{
       Str2ToOct(pNode->m_pszNodeValue, abyDstConfID, CONFID_BYTELEN);
	}
	else
	{
        //mmculog(MMCU_EXP, "[ERR] get sourceconfgid failed(nLen:%d)\n", nLen);
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[ERR] get sourceconfgid failed(nLen:%d)\n", nLen);
		return NULL;
	}
	
	nNode = pXMLTree->FindNode(nParentNode,"confgid");
	if(nNode == -1)
	{
        //mmculog(MMCU_EXP, "[ERR] find confgid failed(nLen:%d)\n", nLen);
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[ERR] find confgid failed(nLen:%d)\n", nLen);
		return NULL;
	}
	pNode=pXMLTree->GetNode(nNode);
	if(pNode->m_pszNodeValue != NULL)
	{
		Str2ToOct(pNode->m_pszNodeValue, abySrcConfID, CONFID_BYTELEN);
	}
	else
	{
        //mmculog(MMCU_EXP, "[ERR] get confgid failed(nLen:%d)\n", nLen);
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[ERR] get confgid failed(nLen:%d)\n", nLen);
		return NULL;
	}
	
    nNode = pXMLTree->FindNode(nParentNode, "register");
	if(nNode == -1)
	{
        //mmculog(MMCU_EXP, "[ERR] find register failed(nLen:%d)\n", nLen);
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[ERR] find register failed(nLen:%d)\n", nLen);
		return NULL;
	}
    pNode = pXMLTree->GetNode(nNode);
    if(pNode->m_pszNodeValue!=NULL)
    {
        bRegister = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) == 0);
		if(bRegister == FALSE)
		{
		    //mmculog(MMCU_EXP, "[ERR] register not ture(nLen:%d)\n", nLen);
			LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[ERR] register not ture(nLen:%d)\n", nLen);
            return NULL;
		}
    }
	//
	return FindCasIns(abySrcConfID, abyDstConfID);
}

void DispatchCascadeMsg(u32 hNode, u32 hAppNode, u8 *pBuf, s32 nLen)
{
   
	CMCUCascade *pcSession = (CMCUCascade *)hAppNode;
	if(pcSession == NULL) 
	{
		pcSession = RegMsgAssociateWithMMcu(pBuf, nLen);
		if(pcSession == NULL)
		{
			//mmculog(MMCU_EXP, "[ERR] RegMsgAssociateWithMMcu failed1: nLen=%d.\n", nLen);
			LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[ERR] RegMsgAssociateWithMMcu failed1: nLen=%d.\n", nLen);
            return;
		}
		else
		{
            BOOL32 bRet = SocketBindToCascIns(hNode, (HAPPNODE)(pcSession));
			if(bRet == TRUE)
			{
				pcSession->m_dwNode = hNode;
				if(pcSession->m_pfAppHandler != NULL)
				{
					pcSession->m_pfAppHandler(H_CASCADE_NEW_CALL_NTF,
						NULL, 0, pcSession->m_dwUserData);
				}
				else
				{
                    //mmculog(MMCU_EXP, "Session:0x%x [ERR] RegMsgAssociateWithMMcu failed2: nLen=%d.\n", pcSession, nLen);
					LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"Session:0x%x [ERR] RegMsgAssociateWithMMcu failed2: nLen=%d.\n", pcSession, nLen);
					return;
				}
			}
            else
            {
                //mmculog(MMCU_EXP, "Session:0x%x [ERR] RegMsgAssociateWithMMcu failed3: nLen=%d.\n", pcSession, nLen);
				LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"Session:0x%x [ERR] RegMsgAssociateWithMMcu failed3: nLen=%d.\n", pcSession, nLen);
            }

            
            u8* pchLocalIP = (u8*)&pcSession->m_dwLocalIP;
            u8* pchPeerIP = (u8*)&pcSession->m_dwPeerIP;
            
//             mmculog(MMCU_DEBUG, "bBind:%d Session(0x%x), Local:%u.%u.%u.%u:%u, Peer:%u.%u.%u.%u:%u\n",
//                 bRet, pcSession, 
//                 pchLocalIP[0], pchLocalIP[1], pchLocalIP[2], pchLocalIP[3],
//                 pcSession->m_wLocalPort,
//                 pchPeerIP[0], pchPeerIP[1], pchPeerIP[2], pchPeerIP[3],
//                 pcSession->m_wPeerPort);
            LogPrint(LOG_LVL_DETAIL,MID_MCULIB_MMCULIB,"bBind:%d Session(0x%x), Local:%u.%u.%u.%u:%u, Peer:%u.%u.%u.%u:%u\n",
                bRet, pcSession, 
                pchLocalIP[0], pchLocalIP[1], pchLocalIP[2], pchLocalIP[3],
                pcSession->m_wLocalPort,
                pchPeerIP[0], pchPeerIP[1], pchPeerIP[2], pchPeerIP[3],
                pcSession->m_wPeerPort);
		}
	}
	
	if(nLen > MAX_XML_BUFFER_LEN)
	{
		//mmculog(MMCU_EXP, "Session:0x%x [MMCU]invalid XML pack(len:%d)\n", pcSession, nLen);
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"Session:0x%x [MMCU]invalid XML pack(len:%d)\n", pcSession, nLen);
		return;
	}

	if(pBuf == NULL|| nLen <= 0)
    {
		pcSession->m_dwNode = INVALID_NODE;
		if(pcSession->m_pfAppHandler != NULL)
		{
			pcSession->m_pfAppHandler(H_CASCADE_DROP_CALL_NTF,
										NULL,
										0,
										pcSession->m_dwUserData);
		}
        return;
    }

	if(ISMMCULOG(MMCU_FILE))	
	{
		if(g_pRF == NULL)
		{
			g_pRF = fopen("XMLR.txt","w");
		}
		if(g_pRF != NULL)
		{
			fwrite(pBuf, nLen, 1,g_pRF);
			fflush(g_pRF);
		}
	}

	//处理TCP完整包
	u8* apbyIntPack[MAX_PACKNUM_PERNETPACK] = {NULL};
	s32 anIntSize[MAX_PACKNUM_PERNETPACK] = {0};
	if(!(pcSession->OnTcpIntegrity(pBuf, nLen, apbyIntPack, anIntSize)))
	{
		LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"[MMCU]Session:0x%x DoRcvMsg: Tcp Pack is not Integrity(len:%d)!", pcSession, nLen);
		//mmculog(MMCU_DEBUG, "[MMCU]Session:0x%x DoRcvMsg: Tcp Pack is not Integrity(len:%d)!", pcSession, nLen);
        return;		
	}
	s32 i=0;
	while(anIntSize[i] !=0)
	{
		pcSession->DoRcvMsg(apbyIntPack[i], anIntSize[i]);
		i++;
	}
  
}//end if

void CMCUCascade::OnRegUnRegReq(CXMLTree *pcTree)
{
    TRegUnRegReq tReq;
    TXMLNode *pNode = NULL;
	s32 nParentNode = pcTree->GetRootId();
    s32 nNode       = -1;
	//Account
	nNode = pcTree->FindNode(nParentNode, "account");
	if(nNode == -1)
	{
		tReq.m_aszUserName[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserName, pNode->m_pszNodeValue, CASCADE_USERNAME_MAXLEN-1);
	}
	//Password
	nNode = pcTree->FindNode(nParentNode, "password");
	if(nNode == -1)
	{
		tReq.m_aszUserPass[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserPass, pNode->m_pszNodeValue, CASCADE_USERPASS_MAXLEN-1);
	}
	/////////////////////
    nNode = pcTree->FindNode(nParentNode, "requestid");
	if(nNode == -1)
	{
		return;
	}
	pNode = pcTree->GetNode(nNode);
    tReq.m_nReqID   = StrToNum(pNode->m_pszNodeValue);
	
	nNode = pcTree->FindNode(nParentNode, "cookie");
	if(nNode == -1)
	{
		return;
	}
	pNode = pcTree->GetNode(nNode);
    m_dwPeerCookie   = StrToNum(pNode->m_pszNodeValue);
    
    nNode = pcTree->FindNode(nParentNode, "register");
	if(nNode == -1)
	{
		return;
	}
    pNode = pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue!=NULL)
    {
        tReq.m_bReg = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) == 0);
    }
    else
    {
        tReq.m_bReg = FALSE;
    } 
    
	// if support multicascade or not
	tReq.m_bIsSupportMuitiCasade = FALSE;
	nNode = pcTree->FindNode(nParentNode, "multicascade");
	if(nNode == -1)
	{
		tReq.m_bIsSupportMuitiCasade = FALSE;
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue!=NULL)
		{
			tReq.m_bIsSupportMuitiCasade = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) == 0);
		}
		else
		{
			tReq.m_bIsSupportMuitiCasade = FALSE;
		}
	}
	
	//get multicascadeInfo
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;

	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else 
	{
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tReq.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tReq.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tReq.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tReq.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}
	//get multicascadeInfo end

	if(m_pfAppHandler != NULL)
	{
		m_pfAppHandler((s32)H_CASCADE_REGUNREG_REQ, (u8 *)&tReq, sizeof(tReq), m_dwUserData); 
	}

}

void CMCUCascade::OnRegUnRegRsp(CXMLTree *pcTree)
{
	TRegUnRegRsp tRsp;
    TXMLNode *pNode = NULL;
    s32 nParentNode = pcTree->GetRootId();
    s32 nNode       = -1;
    
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tRsp.m_nReqID = StrToNum(pNode->m_pszNodeValue);
    
	//register
    nNode = pcTree->FindNode(nParentNode, "register");
	if(nNode == -1)
	{
		return;
	}
    pNode = pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue!=NULL)
    {
        tRsp.m_bReg = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) == 0);
    }
    else
    {
        tRsp.m_bReg = FALSE;
    }
	
	//return value
    nNode = pcTree->FindNode(pNode->m_nParentId, "returnvalue");
	if(nNode == -1)
	{
		return;
	}
    pNode = pcTree->GetNode(nNode);
    tRsp.m_emReturnVal = GetReturnValFromStr(pNode->m_pszNodeValue);

	//get multicascadeInfo
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;
	
	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else
	{
	
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tRsp.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tRsp.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tRsp.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tRsp.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tRsp.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tRsp.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tRsp.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tRsp.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}


	if(m_pfAppHandler != NULL)
	{
		m_pfAppHandler((s32)H_CASCADE_REGUNREG_RSP, (u8 *)&tRsp, sizeof(tRsp), m_dwUserData); 
	}
}


void CMCUCascade::OnNewNeighborNtf(CXMLTree *pcTree)
{
    
    TRosterList    tRosterList;
    TXMLNode      *pNode                   = NULL;
    s32            nNextBrotherNode        = pcTree->GetRootId();
    s32            nNode                   = -1;
    s32            nListCount              = 0;

    //Roster_Member
    do
    {
        s32 nMember = pcTree->FindNode(nNextBrotherNode, "roster_member");
        if(nMember == -1)
        {
           break;
        }

        nNode = pcTree->FindNode(nMember, "gid");
        if(nNode == -1)
        {
            break;
        }
        pNode = pcTree->GetNode(nNode);
		if(!ExtractPid(pNode->m_pszNodeValue, 
						&(tRosterList.m_atRoster[nListCount].m_dwPID),
                        &(tRosterList.m_atRoster[nListCount].m_bLocal)))
		{
			nNextBrotherNode = pcTree->GetBrotherId(nMember);
			continue;
		}
		
        nNode = pcTree->FindNode(nMember, "state");
        if(nNode == -1)
        {
            break ;
        }
        pNode = pcTree->GetNode(nNode);
        tRosterList.m_atRoster[nListCount].m_emConnState = StrToConnState(pNode->m_pszNodeValue);

        nListCount++; 
        nNextBrotherNode = pcTree->GetBrotherId(nMember);

    }while(nNextBrotherNode != -1 && nListCount < PART_NUM);

    tRosterList.m_nCount = nListCount;

	//get multicascadeInfo
	u8  byLvlMtId = 0;
	s32 nParentNode = pcTree->GetRootId();
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;
	
	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else
	{
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tRosterList.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tRosterList.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tRosterList.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tRosterList.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tRosterList.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tRosterList.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tRosterList.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tRosterList.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}
	

    if(m_pfAppHandler != NULL)
    {
        m_pfAppHandler((s32)H_CASCADE_NEWROSTER_NTF, (u8 *)&tRosterList, sizeof(tRosterList), m_dwUserData); 
    }
}

s32 CMCUCascade::ParsePartInfo(CXMLTree *pcTree,s32 nFromNode,TPart *ptPart)
{
    
    s32 nStrLenFrom, nStrLenMin;
    memset(ptPart,0,sizeof(TPart));

    s32 nPart = pcTree->FindNode(nFromNode, "part");
    if(nPart==-1)
    {
        return BAD_PART;
    }

    s32 nNode=pcTree->FindNode(nPart,"pid");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    TXMLNode *pNode = pcTree->GetNode(nNode);
    if(!ExtractPid(pNode->m_pszNodeValue, &(ptPart->m_dwPID),&(ptPart->m_bLocal)))
	{
		return BAD_PART;
	}
    
    nNode=pcTree->FindNode(nPart,"partname");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue)
    {
        nStrLenFrom = strlen(pNode->m_pszNodeValue);
        nStrLenMin = (nStrLenFrom < STR_LEN ? nStrLenFrom : (STR_LEN - 1));
        strncpy(ptPart->m_aszPartName, pNode->m_pszNodeValue, nStrLenMin);
        ptPart->m_aszPartName[nStrLenMin] = 0;
    }
    ptPart->m_aszPartName[STR_LEN - 1] = NULL;
    
    nNode=pcTree->FindNode(nPart,"dialstr");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue)
    {
        nStrLenFrom = strlen(pNode->m_pszNodeValue);
        nStrLenMin = (nStrLenFrom < STR_LEN ? nStrLenFrom : (STR_LEN - 1));
        strncpy(ptPart->m_aszDialStr, pNode->m_pszNodeValue, nStrLenMin);
        ptPart->m_aszDialStr[nStrLenMin] = 0;
    }
    ptPart->m_aszDialStr[STR_LEN - 1] = NULL;
    
    nNode=pcTree->FindNode(nPart,"ip");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    ptPart->m_dwIP = StrToIP(pNode->m_pszNodeValue);
        
    nNode=pcTree->FindNode(nPart,"eptype");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    ptPart->m_emEPType=StrToEPType(pNode->m_pszNodeValue);

    nNode=pcTree->FindNode(nPart,"epdesc");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue)
    {
        nStrLenFrom = strlen(pNode->m_pszNodeValue);
        nStrLenMin = (nStrLenFrom < STR_LEN ? nStrLenFrom : (STR_LEN - 1));
        strncpy(ptPart->m_aszEPDesc, pNode->m_pszNodeValue, nStrLenMin);
        ptPart->m_aszEPDesc[nStrLenMin] = NULL;
    }
    ptPart->m_aszEPDesc[STR_LEN - 1] = NULL;
    
    nNode=pcTree->FindNode(nPart,"vin");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    ptPart->m_emVideoIn=StrToPType(pNode->m_pszNodeValue);
    
    nNode=pcTree->FindNode(nPart,"vout");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    ptPart->m_emVideoOut=StrToPType(pNode->m_pszNodeValue);
    
    nNode=pcTree->FindNode(nPart,"vin2");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    ptPart->m_emVideo2In=StrToPType(pNode->m_pszNodeValue);
    
    nNode=pcTree->FindNode(nPart,"vout2");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    ptPart->m_emVideo2Out=StrToPType(pNode->m_pszNodeValue);
    
    nNode=pcTree->FindNode(nPart,"ain");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    ptPart->m_emAudioIn=StrToPType(pNode->m_pszNodeValue);
    
    nNode=pcTree->FindNode(nPart,"aout");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    ptPart->m_emAudioOut=StrToPType(pNode->m_pszNodeValue);
    
    nNode=pcTree->FindNode(nPart,"datam");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue!=NULL)
    {
        ptPart->m_bDataMeeting = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrNone) != 0);
    }
    else
    {
        ptPart->m_bDataMeeting = BAD_PART;//be better to define a third state 
    }
    
    nNode = pcTree->FindNode(nPart,"vmutein");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode = pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue!=NULL)
    {
        ptPart->m_bVideoMuteIn = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) == 0);
    }
    else
    {
        ptPart->m_bVideoMuteIn = BAD_PART;
    }
    
    nNode=pcTree->FindNode(nPart,"vmuteout");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue!=NULL)
    {
        ptPart->m_bVideoMuteOut = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) == 0);
    }
    else
    {
        ptPart->m_bVideoMuteOut = BAD_PART;
    }
    
    nNode=pcTree->FindNode(nPart,"amutein");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue!=NULL)
    {
        ptPart->m_bAudioMuteIn = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) == 0);
    }
    else
    {
        ptPart->m_bAudioMuteIn = BAD_PART;
    }
    
    nNode=pcTree->FindNode(nPart,"amuteout");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue!=NULL)
    {
        ptPart->m_bAudioMuteOut = (NullStrCmpLimited(pNode->m_pszNodeValue,pszStrTrue) == 0);
    }
    else
    {
        ptPart->m_bAudioMuteOut = BAD_PART;
    }
    
    nNode=pcTree->FindNode(nPart,"invited");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue!=NULL)
    {
        ptPart->m_bInvited = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) == 0);
    }
    else
    {
        ptPart->m_bInvited = BAD_PART;
    }
    
    nNode=pcTree->FindNode(nPart,"ismaster");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue!=NULL)
    {
        ptPart->m_bMaster = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) == 0);
    }
    else
    {
        ptPart->m_bMaster = BAD_PART;
    }
    
    nNode=pcTree->FindNode(nPart,"startt");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    StrToTime(ptPart->m_tStartTime, pNode->m_pszNodeValue);

	nNode=pcTree->FindNode(nPart,"pconfgid");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
	u32 dwConfId;
	BOOL32 bLocal;
	if(!ExtractPid(pNode->m_pszNodeValue, &(dwConfId),&(bLocal)))
	{
		//other mcu
		return BAD_PART;
	}
    
    
    nNode=pcTree->FindNode(nPart,"const");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    ptPart->m_emConnState=StrToConnState(pNode->m_pszNodeValue);
    
    nNode=pcTree->FindNode(nPart,"isfeccenabled");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    ptPart->m_bFECCEnable = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) == 0);

	nNode=pcTree->FindNode(nPart,"disp_in_louts");
    if(nNode ==-1)
    {
        return BAD_PART;
    }
    //Adding conference view list that Participant will input its video image into it
    s32 nVPos                                   = 0;
    ptPart->m_tPartVideoInfo.m_nViewCount       = 0; 

    s32 nParentNode = 0;
    nParentNode     = pcTree->FindNode(nParentNode,"v_pos");
    while(nParentNode != -1 && nVPos < MAX_VIEWS_PER_CONF)
    {
        nNode=pcTree->FindNode(nParentNode,"lid");
        if(nNode ==-1)
        {
            break;
        }
        pNode=pcTree->GetNode(nNode);
        ptPart->m_tPartVideoInfo.m_anViewPos[nVPos].m_nViewID = StrToNum(pNode->m_pszNodeValue);

        nNode=pcTree->FindNode(nParentNode,"sbfrindx");
        if(nNode ==-1)
        {
            break;
        }
        pNode=pcTree->GetNode(nNode);
        ptPart->m_tPartVideoInfo.m_anViewPos[nVPos].m_bySubframeIndex = StrToNum(pNode->m_pszNodeValue);
        
        nVPos++;
        nParentNode = pcTree->GetBrotherId(nParentNode);
    }

    ptPart->m_tPartVideoInfo.m_nViewCount       = nVPos;

    //Add the ViewID that participant is now displaying in its moniter
    //also Add the videoschemeID that participant is receiving it using this spec.

	nNode=pcTree->FindNode(nPart,"displout");
    if(nNode==-1)
    {
        return BAD_PART;
    }
	pNode=pcTree->GetNode(nNode);
	ptPart->m_tPartVideoInfo.m_nOutputViewID = StrToNum(pNode->m_pszNodeValue);

    nNode=pcTree->FindNode(nPart,"dispoput");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    ptPart->m_tPartVideoInfo.m_nOutVideoSchemeID = StrToNum(pNode->m_pszNodeValue);

	//-new info
	nNode = pcTree->FindNode(nPart, "multicascade"); 
	if(nNode==-1)
    {
		// [9/14/2010 xliang] the peer doesn't support multiple cascade, we just ignore it, but it's not BAD_PART
        // return BAD_PART;
    }
	else
	{
		s32 nMultiCascadeNode = nNode;
		//1, caslevel
		nNode = pcTree->FindNode(nMultiCascadeNode, "caslvl");
		if(nNode==-1)
		{
			return BAD_PART;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			ptPart->m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			ptPart->m_byCasLevel = 0;
		}
		
		//2, casmtid
		s32 nBrotherNode = nMultiCascadeNode;
		for( u8 byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmt");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				ptPart->m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				ptPart->m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		//-new info end
	}
	


    nNode=pcTree->FindNode(nPart,"discrsn");
    if(nNode==-1)
    {
        return BAD_PART;
    }
    pNode=pcTree->GetNode(nNode);
    if((NullStrCmpLimited(pNode->m_pszNodeValue, pszStrNormal) == 0))
    {
        return NORMAL_PART;
    }
    else
    {
        return ABNORMAL_PART;
    }
}

void CMCUCascade::OnNewPartNtf(CXMLTree *pcTree)
{

    TPart           tPart;
    s32             nParentNode  = pcTree->GetRootId();
    s32             nResult      = BAD_PART;
    memset(&tPart, 0, sizeof(tPart));

    nResult = ParsePartInfo(pcTree,nParentNode,&tPart);
	if(nResult == BAD_PART || nResult == ABNORMAL_PART)
	{
		return;
	}

    if(m_pfAppHandler != NULL)
    {
        m_pfAppHandler((s32)H_CASCADE_NEWPART_NTF, (u8 *)&tPart, sizeof(tPart), m_dwUserData); 
    }
}

void CMCUCascade::OnCallNtf(CXMLTree *pcTree)
{
    TCallNtf        tCallNtf;	
    s32             nParentNode  = pcTree->GetRootId();
	
    s32 nNode=pcTree->FindNode(nParentNode,"pid");
    if(nNode == -1)
    {
		return;
    } 
    TXMLNode *pNode=pcTree->GetNode(nNode);
    if(!ExtractPid(pNode->m_pszNodeValue, &(tCallNtf.m_dwPID), &(tCallNtf.m_bLocal)))
	{
		return;
	}
	
	nNode=pcTree->FindNode(nParentNode,"dialstr");
    if(nNode == -1)
    {
		return;
    } 
    //Get DialStr String
    pNode=pcTree->GetNode(nNode);
	u32 nLen = strlen(pNode->m_pszNodeValue);
	nLen = nLen > (sizeof(tCallNtf.m_aszDialStr)-1) ? (sizeof(tCallNtf.m_aszDialStr)-1) : nLen;
    strncpy(tCallNtf.m_aszDialStr, pNode->m_pszNodeValue, nLen);
	tCallNtf.m_aszDialStr[nLen] = 0;
	
	//get multicascadeInfo
	nParentNode             = pcTree->GetRootId();
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;
	
	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else
	{
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tCallNtf.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tCallNtf.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tCallNtf.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tCallNtf.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tCallNtf.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tCallNtf.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tCallNtf.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tCallNtf.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}
	//get multicascadeInfo end

    if(m_pfAppHandler != NULL)
    {
        m_pfAppHandler((s32)H_CASCADE_CALL_ALERTING_NTF, (u8 *)&tCallNtf, sizeof(tCallNtf), m_dwUserData); 
    }
}

void CMCUCascade::OnGetPartListRsp(CXMLTree *pcTree)
{
    TXMLNode     *pNode =NULL;
    s32           nNode                   = -1;
    s32           nParentNode             = pcTree->GetRootId();
    s32           nListCount              = 0;
    s32           nResult                 = BAD_PART;
    TPartListRsp  tPartListRsp;

    memset(&tPartListRsp,0,sizeof(tPartListRsp));
    
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tPartListRsp.m_nReqID = StrToNum(pNode->m_pszNodeValue);
    
    nNode=pcTree->FindNode(nParentNode,"islastresponseblock");
    if(nNode == -1)
    {
       return;
    } 
    pNode=pcTree->GetNode(nNode);
    tPartListRsp.m_bLastPack = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) == 0);
    

    nNode=pcTree->FindNode(nParentNode,"returnvalue");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    tPartListRsp.m_emReturnVal = GetReturnValFromStr(pNode->m_pszNodeValue);
    do
    {
        nParentNode = pcTree->FindNode(nParentNode, "part");
        if(nParentNode == -1)
		{
			break;
		}
        nResult = ParsePartInfo(pcTree,nParentNode, &(tPartListRsp.m_atParts[nListCount]));
        if(nResult == BAD_PART)
        {
           nParentNode = pcTree->GetBrotherId(nParentNode);
		   continue;
        }

        nListCount++;
        nParentNode = pcTree->GetBrotherId(nParentNode);
    }while(nParentNode != -1 && nListCount < PART_NUM);

     tPartListRsp.m_dwPartCount=nListCount;
     
	 TPartListMsg tMsg;

	 //get multicascadeInfo
	 nParentNode             = pcTree->GetRootId();
	 u8  byLvlMtId = 0;
	 s32 nChildParentNode = -1;
	 s32 nChild2PNode = -1;
	 s32 nBrotherNode = -1;
	 
	 nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	 if(nNode == -1)
	 {
		 //the peer doesn't support multiple cascade		
	 }
	 else
	 {
		 
		 
		 nChildParentNode = nNode;
		 
		 //src
		 nNode = pcTree->FindNode(nChildParentNode, "src");
		 if(nNode == -1)
		 {
			 return;
		 }
		 nChild2PNode = nNode;
		 nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		 if(nNode == -1)
		 {
			 return;
		 }
		 pNode = pcTree->GetNode(nNode);
		 if(pNode->m_pszNodeValue != NULL)
		 {
			 tMsg.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		 }
		 else
		 {
			 tMsg.m_tMsgSrc.m_byCasLevel = 0;
		 }
		 
		 nBrotherNode = nChild2PNode;
		 for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		 {
			 nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			 if(nNode == -1)
			 {
				 break;
			 }
			 pNode = pcTree->GetNode(nNode);
			 if(pNode->m_pszNodeValue != NULL)
			 {
				 tMsg.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			 }
			 else
			 {
				 tMsg.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			 }
			 
			 nBrotherNode = pcTree->GetBrotherId(nNode);
		 }
		 
		 // dst info
		 nNode = pcTree->FindNode(nChildParentNode, "dst");
		 if(nNode == -1)
		 {
			 return;
		 }
		 nChild2PNode = nNode;
		 nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		 if(nNode == -1)
		 {
			 return;
		 }
		 pNode = pcTree->GetNode(nNode);
		 if(pNode->m_pszNodeValue != NULL)
		 {
			 tMsg.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		 }
		 else
		 {
			 tMsg.m_tMsgDst.m_byCasLevel = 0;
		 }
		 
		 nBrotherNode = nChild2PNode;
		 for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		 {
			 nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			 if(nNode == -1)
			 {
				 break;
			 }
			 pNode = pcTree->GetNode(nNode);
			 if(pNode->m_pszNodeValue != NULL)
			 {
				 tMsg.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			 }
			 else
			 {
				 tMsg.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			 }
			 
			 nBrotherNode = pcTree->GetBrotherId(nNode);
		 }
	 }
	 
    if(m_pfAppHandler != NULL)
    {
        u8 abyBuff[sizeof(TPartListMsg) + PARTS_PER_PACKET*sizeof(TPart)];

        tMsg.m_nReqID = tPartListRsp.m_nReqID;
        tMsg.m_emReturnVal = tPartListRsp.m_emReturnVal;
        tMsg.m_dwPartCount = tPartListRsp.m_dwPartCount;
        tMsg.m_nSnapShot = tPartListRsp.m_nSnapShot;
        tMsg.m_bLastPack = tPartListRsp.m_bLastPack;
        
        u32 nRealMsgLen = sizeof(tMsg) + (u32)tMsg.m_dwPartCount*sizeof(TPart);

        if (tMsg.m_dwPartCount <= PARTS_PER_PACKET)
        {
            memcpy(abyBuff, &tMsg, sizeof(tMsg));
            memcpy(abyBuff+sizeof(tMsg), tPartListRsp.m_atParts, (u32)tPartListRsp.m_dwPartCount*sizeof(TPart));
            
            m_pfAppHandler((s32)H_CASCADE_PARTLIST_RSP, abyBuff, nRealMsgLen, m_dwUserData); 
        }
        else
        {
            //mmculog(MMCU_EXP, "CMCUCascade::OnGetPartListRsp Get More than %d Part\n", PARTS_PER_PACKET);
			LogPrint(LOG_LVL_ERROR,MID_MCULIB_MMCULIB,"CMCUCascade::OnGetPartListRsp Get More than %d Part\n", PARTS_PER_PACKET);
        }

        //m_pfAppHandler(H_CASCADE_PARTLIST_RSP, (u8 *)&tPartListRsp, sizeof(tPartListRsp), m_dwUserData); 
    }

}//end of function OnGetPartListResp

void CMCUCascade::OnGetPartListReq(CXMLTree *pcTree)
{
  
    TXMLNode     *pNode =NULL;
    s32 nParentNode     = pcTree->GetRootId();
	s32 nNode           = -1;
    TReq          tReq;
	//Account
	nNode = pcTree->FindNode(nParentNode, "account");
	if(nNode == -1)
	{
		tReq.m_aszUserName[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserName, pNode->m_pszNodeValue, CASCADE_USERNAME_MAXLEN-1);
	}
	//Password
	nNode = pcTree->FindNode(nParentNode, "password");
	if(nNode == -1)
	{
		tReq.m_aszUserPass[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserPass, pNode->m_pszNodeValue, CASCADE_USERPASS_MAXLEN-1);
	}
	/////////////////////
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tReq.m_nReqID = StrToNum(pNode->m_pszNodeValue);
    
	
	//get multicascadeInfo
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;
	
	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else
	{
		
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tReq.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tReq.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tReq.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tReq.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}

    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_PARTLIST_REQ, (u8 *)&tReq, sizeof(tReq), m_dwUserData); 
    }
}
////////////////////
void CMCUCascade::OnCMVideoReq(CXMLTree *pcTree)
{
    TXMLNode     *pNode =NULL;
    s32 nNode    = -1;
    s32 nParentNode    = pcTree->GetRootId();
    TConfVideInfoReq tReq;
	//Account
	nNode = pcTree->FindNode(nParentNode, "account");
	if(nNode == -1)
	{
		tReq.m_aszUserName[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserName, pNode->m_pszNodeValue, CASCADE_USERNAME_MAXLEN-1);
	}
	//Password
	nNode = pcTree->FindNode(nParentNode, "password");
	if(nNode == -1)
	{
		tReq.m_aszUserPass[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserPass, pNode->m_pszNodeValue, CASCADE_USERPASS_MAXLEN-1);
	}
	/////////////////////
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tReq.m_nReqID = StrToNum(pNode->m_pszNodeValue);

    nNode = pcTree->FindNode(nParentNode, "fullinfo");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tReq.m_bFullInfo = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) == 0);
    
	//get multicascadeInfo
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;
	
	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else
	{
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tReq.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tReq.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tReq.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tReq.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}

    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_VIDEOINFO_REQ, (u8 *)&tReq, sizeof(tReq), m_dwUserData); 
    }
}

BOOL32 CMCUCascade::ParseVideoInfo(CXMLTree *pcTree,s32 nFromNode,TView *ptVideo)
{
    TXMLNode *pNode                   = NULL;
    s32       nNode                   = -1;
    s32       nParentNode             = pcTree->GetRootId();
    s32       nListCount              = 0;
    
	nNode = pcTree->FindNode(nFromNode, "lid");
    if(nNode==-1)
    {
        return FALSE;
    }
    pNode=pcTree->GetNode(nNode);
    ptVideo->m_nViewId = StrToNum(pNode->m_pszNodeValue);

    nNode = pcTree->FindNode(nFromNode, "currentvideogeometryindex");
    if(nNode==-1)
    {
        return FALSE;
    }
    pNode=pcTree->GetNode(nNode);
    ptVideo->m_byCurrentGeometryIndex = StrToNum(pNode->m_pszNodeValue);

    nNode = pcTree->FindNode(nFromNode, "available_geometries_list");
    if(nNode !=-1)
    {
        //Parse Geometry List per conference view BEGIGNGING
        nParentNode   = nNode;
        nListCount    = 0;
        do
        {
            nParentNode = pcTree->FindNode(nParentNode, "video_geometry");
            if(nParentNode == -1)
            {
                return FALSE;
            }
            
            nNode = pcTree->FindNode(nParentNode, "videogeometrypresettype");
            if(nNode==-1)
            {
                return FALSE;
            }
            pNode = pcTree->GetNode(nNode);
            ptVideo->m_abyGeometryInfo[nListCount] = (u8)(StrToNum(pNode->m_pszNodeValue)/10);
            
            nListCount++;
            nParentNode = pcTree->GetBrotherId(nParentNode);
            
        }while(nParentNode != -1 && nListCount < MAX_GEOMETRYS_PER_VIEW);
        
        ptVideo->m_byGeometryCount = nListCount;
        //Parse Geometry List per conference view ENDING
    }

    nNode = pcTree->FindNode(nFromNode, "display_sub_frames_list");
    if(nNode==-1)
    {
        return FALSE;
    }

    //Adding participant contributer List of the conference view BEGINGING
    nParentNode   = nNode;
    nListCount    = 0;
    do
    {
        nParentNode = pcTree->FindNode(nParentNode, "subframepartid");
        if(nParentNode == -1)
        {
            break;
        }
        pNode = pcTree->GetNode(nParentNode);
        ExtractPid(pNode->m_pszNodeValue, 
                   &(ptVideo->m_atContributePIDs[nListCount].m_dwPID),
                   &(ptVideo->m_atContributePIDs[nListCount].m_bLocal));
        
        nListCount++;
        nParentNode = pcTree->GetBrotherId(nParentNode);
    }while(nParentNode != -1 && nListCount < MAX_SUBFRAMES_PER_GEOMETRY);

    ptVideo->m_byContributePIDCount = nListCount;
    //Adding participant contributer List of the conference view ENDING
    
    //Parsing Common View Attributes BEGINGING
    nNode=pcTree->FindNode(nFromNode,"pics");
    if(nNode==-1)
    {
        return FALSE;
    }
    pNode=pcTree->GetNode(nNode);
    ptVideo->m_emRes=StrToPicS(pNode->m_pszNodeValue);
    //Parsing Common View Attributes ENDING

    nNode = pcTree->FindNode(nFromNode,"video_output_list");
    if(nNode == -1)
    {
        return FALSE;
    }
    
    //Parsing View videoscheme list BEGIGNING
    nParentNode   = nNode;
    nListCount    = 0;
    do 
    {
        nParentNode = pcTree->FindNode(nParentNode,"video_output");
        if(nParentNode == -1)
        {
            return FALSE;
        }
        
        nNode = pcTree->FindNode(nParentNode,"videooutputid");
        if(nNode == -1)
        {
            return FALSE;
        }
        pNode=pcTree->GetNode(nNode);
        ptVideo->m_atVideoSchemeList[nListCount].m_nOutputVideoSchemeID = StrToNum(pNode->m_pszNodeValue);

        nNode=pcTree->FindNode(nParentNode, "maxbitrate");
        if(nNode==-1)
        {
            return FALSE;
        }
        pNode=pcTree->GetNode(nNode);
        ptVideo->m_atVideoSchemeList[nListCount].m_dwMaxRate = StrToNum(pNode->m_pszNodeValue)/1000;

        nNode=pcTree->FindNode(nParentNode, "minbitrate");
        if(nNode==-1)
        {
            return FALSE;
        }
        pNode=pcTree->GetNode(nNode);
        ptVideo->m_atVideoSchemeList[nListCount].m_dwMinRate = StrToNum(pNode->m_pszNodeValue)/1000;
        
        nNode=pcTree->FindNode(nParentNode, "canupdatebitrate");
        if(nNode==-1)
        {
            return FALSE;
        }
        pNode=pcTree->GetNode(nNode);
        if(pNode->m_pszNodeValue)
        {
            ptVideo->m_atVideoSchemeList[nListCount].m_bCanUpdateRate = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) == 0);
        }
        else
        {
            ptVideo->m_atVideoSchemeList[nListCount].m_bCanUpdateRate = FALSE;
        }
        
        nNode=pcTree->FindNode(nParentNode,"currentframerate");
        if(nNode==-1)
        {
            return FALSE;
        }
        pNode=pcTree->GetNode(nNode);
        ptVideo->m_atVideoSchemeList[nListCount].m_nFrameRate =StrToNum(pNode->m_pszNodeValue);
        
        nNode=pcTree->FindNode(nParentNode,"currentcodec");
        if(nNode==-1)
        {
            return FALSE;
        }
        pNode=pcTree->GetNode(nNode);
        ptVideo->m_atVideoSchemeList[nListCount].m_emPayLoad = StrToPType(pNode->m_pszNodeValue);

        nListCount ++;
        nParentNode = pcTree->GetBrotherId(nParentNode);
    } while(nParentNode != -1 && nListCount < MAX_VIDEOSCHEMES_PER_VIEW);

    ptVideo->m_byVideoSchemeCount = nListCount;
    //Parsing View videoscheme list ENDING
    return TRUE;
}

void CMCUCascade::OnCMVideoRsp(CXMLTree *pcTree)
{

    TXMLNode     *pNode =NULL;
    s32           nNode                   = -1;
    s32           nParentNode             = pcTree->GetRootId();
    s32           nListCount              = 0;
    BOOL32        bDecode;
    TConfVideoInfo tRsp;
    
    memset(&tRsp,0,sizeof(tRsp));
    
    
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tRsp.m_nReqID = StrToNum(pNode->m_pszNodeValue);
    
    
    nNode=pcTree->FindNode(nParentNode,"returnvalue");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    tRsp.m_emReturnVal = GetReturnValFromStr(pNode->m_pszNodeValue);
    
    nNode=pcTree->FindNode(nParentNode,"islastresponseblock");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    if(NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) != 0)
    {
        //mmculog(MMCU_ALL, "[MMCU]OnCMVideoRsp: Conf Video Info Package Too Large\n");
		LogPrint(LOG_LVL_DETAIL,MID_MCULIB_MMCULIB,"[MMCU]OnCMVideoRsp: Conf Video Info Package Too Large\n");
    }
    
    nNode = pcTree->FindNode(nParentNode,"defaultlid");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    s32 nDefaultViewId = StrToNum(pNode->m_pszNodeValue);
    

    nNode = pcTree->FindNode(nParentNode,"layouts_info_list");
    if(nNode == -1)
    {
        return;
    }

    //Generating multiple conference view profile(Layout) into xml packet
    nParentNode = nNode;
    do
    {
        nParentNode = pcTree->FindNode(nParentNode, "layout_info");
        if(nParentNode == -1)
		{
			break;
		}

        bDecode=ParseVideoInfo(pcTree,nParentNode, &(tRsp.m_atViewInfo[nListCount]));
        if(bDecode == FALSE)
        {
            nParentNode=pcTree->GetBrotherId(nParentNode);
			continue;
        }

        nListCount++;
        nParentNode=pcTree->GetBrotherId(nParentNode);
    }while(nParentNode != -1 && nListCount < MAX_VIEWS_PER_CONF);
    
    tRsp.m_byConfViewCount = nListCount;
    tRsp.m_byDefaultViewIndex = 0;

    //convert Lid value to Lid index
    for(s32 nViewIndex = 0; nViewIndex < tRsp.m_byConfViewCount; nViewIndex++)
    {
         if(tRsp.m_atViewInfo[nViewIndex].m_nViewId == nDefaultViewId)
         {
             tRsp.m_byDefaultViewIndex = nViewIndex;
             break;
         }
    }

	//get multicascadeInfo
	nParentNode             = pcTree->GetRootId();
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;
	
	nNode = pcTree->FindNode(nParentNode,"Get_Conference_Video_Info_Response");
	if(nNode == -1)
	{
		return;
	}
	nNode = pcTree->FindNode(nNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else
	{
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tRsp.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tRsp.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tRsp.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tRsp.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tRsp.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tRsp.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tRsp.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tRsp.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}
    //Call back Application custom function
    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_VIDEOINFO_RSP, (u8 *)&tRsp, sizeof(tRsp), m_dwUserData); 
    }
}

void CMCUCascade::OnCMAudioReq(CXMLTree *pcTree)
{
   
    TReq tReq;
    s32 nNode                   = -1;
    TXMLNode *pNode             = NULL;
    s32 nParentNode             = pcTree->GetRootId();
	//Account
	nNode = pcTree->FindNode(nParentNode, "account");
	if(nNode == -1)
	{
		tReq.m_aszUserName[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserName, pNode->m_pszNodeValue, CASCADE_USERNAME_MAXLEN-1);
	}
	//Password
	nNode = pcTree->FindNode(nParentNode, "password");
	if(nNode == -1)
	{
		tReq.m_aszUserPass[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserPass, pNode->m_pszNodeValue, CASCADE_USERPASS_MAXLEN-1);
	}
	/////////////////////
	nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tReq.m_nReqID = StrToNum(pNode->m_pszNodeValue);
    
	//get multicascadeInfo
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;
	
	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else
	{
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tReq.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tReq.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tReq.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tReq.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}

    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_AUDIOINFO_REQ, (u8 *)&tReq, sizeof(tReq), m_dwUserData); 
    }
}

void CMCUCascade::OnCMAudioRsp(CXMLTree *pcTree)
{
    TXMLNode     *pNode                   =NULL;
    s32           nNode                   = -1;
    s32           nParentNode             = pcTree->GetRootId();
    s32           nListCount              = 0;
    s32           nDefaultMixerId         = 0;
    TConfAudioInfo tRsp;

    memset(&tRsp,0,sizeof(tRsp));
    
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tRsp.m_nReqID = StrToNum(pNode->m_pszNodeValue);
    
    nNode=pcTree->FindNode(nParentNode,"returnvalue");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    tRsp.m_emReturnVal = GetReturnValFromStr(pNode->m_pszNodeValue);
    
    nNode = pcTree->FindNode(nParentNode,"defaultaudiomixerid");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    nDefaultMixerId = StrToNum(pNode->m_pszNodeValue);

    nNode = pcTree->FindNode(nParentNode,"mixers_info_list");
    if(nNode == -1)
    {
        return;
    }
	nNode = pcTree->FindNode(nParentNode,"numofaudiomixers");
	if(nNode == -1)
	{
		return;
	}
	pNode=pcTree->GetNode(nNode);
	tRsp.m_byMixerCount = StrToNum(pNode->m_pszNodeValue);

    //Parsing Mixer List of Conference BEGINGING
    nListCount = 0;
    nParentNode = nNode;
    do
    {
        //nParentNode = pcTree->FindNode(nParentNode,"mixers_info");
        nParentNode = pcTree->FindNode(nParentNode,"mixers_info"); //ljx
        if(nParentNode == -1)
        {
            return;
        }

        nNode=pcTree->FindNode(nParentNode,"currentspeaker");
        if(nNode == -1)
        {
            return;
        }
        pNode=pcTree->GetNode(nNode);
        tRsp.m_tMixerList[nListCount].m_nMixerID = StrToNum(pNode->m_pszNodeValue);
        
        nNode=pcTree->FindNode(nParentNode,"currentspeaker");
        if(nNode == -1)
        {
            return;
        }
        pNode=pcTree->GetNode(nNode);
        if(!ExtractPid(pNode->m_pszNodeValue,
                    &(tRsp.m_tMixerList[nListCount].m_dwSpeakerPID),
                    &(tRsp.m_tMixerList[nListCount].m_bLocal)))
		{
			
			nParentNode=pcTree->GetBrotherId(nParentNode);
			continue;
		}

        nListCount++;
        nParentNode=pcTree->GetBrotherId(nParentNode);
        
    }while(nParentNode != -1 && nListCount < MAX_MIXERS_PER_CONF);
    
    //tRsp.m_byMixerCount = nListCount;
    //Parsing Mixer List of Conference ENDING

    //convert MixerId value to Mixer index
    tRsp.m_byDefaultMixerIndex =  0;
    for(s32 nMixerIndex = 0; nMixerIndex < tRsp.m_byMixerCount; nMixerIndex++)
    {
        if(tRsp.m_tMixerList[nMixerIndex].m_nMixerID  == nDefaultMixerId)
        {
            tRsp.m_byDefaultMixerIndex = nMixerIndex;
            break;
        }
    }

	//get multicascadeInfo
	nParentNode             = pcTree->GetRootId();
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;
	
	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else
	{
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tRsp.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tRsp.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tRsp.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tRsp.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tRsp.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tRsp.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tRsp.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tRsp.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}

    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_AUDIOINFO_RSP, (u8 *)&tRsp, sizeof(tRsp), m_dwUserData); 
    }
}

void CMCUCascade::OnOutputLayoutNtf(CXMLTree *pcTree)
{
    TXMLNode* pNode			= NULL;
    s32       nNode         = -1;
    s32       nParentNode   = pcTree->GetRootId();
	TPartOutputNtf   tNtf;
	
    memset(&tNtf,0,sizeof(tNtf));

    nNode=pcTree->FindNode(nParentNode,"pid");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    if(!ExtractPid(pNode->m_pszNodeValue, &(tNtf.m_dwPID), &(tNtf.m_bLocal)))
	{
		return;
	}

	nNode=pcTree->FindNode(nParentNode,"lid");
    if(nNode == -1)
    {
        return;
    }
	pNode=pcTree->GetNode(nNode);
    tNtf.m_nViewID = StrToNum(pNode->m_pszNodeValue);
    
	//get multicascadeInfo
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;

	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else 
	{
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tNtf.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tNtf.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tNtf.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tNtf.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tNtf.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tNtf.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tNtf.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tNtf.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}
	//get multicascadeInfo end

    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_OUTPUT_NTF, (u8 *)&tNtf, sizeof(tNtf), m_dwUserData); 
    }     
}

void CMCUCascade::OnNewSpeakerNtf(CXMLTree *pcTree)
{
    TXMLNode* pNode			= NULL;
    s32       nNode         = -1;
    s32       nParentNode   = pcTree->GetRootId();
	TNewSpeakerNtf   tNtf;
	
    memset(&tNtf,0,sizeof(tNtf));
    
    nNode=pcTree->FindNode(nParentNode,"pid");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    if(!ExtractPid(pNode->m_pszNodeValue, &(tNtf.m_dwPID), &(tNtf.m_bLocal)))
	{
		return;
	}

    nNode=pcTree->FindNode(nParentNode,"mixerid");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    tNtf.m_nMixerID = StrToNum(pNode->m_pszNodeValue);

    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_NEWSPEAKER_NTF, (u8 *)&tNtf, sizeof(tNtf), m_dwUserData); 
    }     
}

void CMCUCascade::OnSetInLayoutReq(CXMLTree *pcTree)
{
    TXMLNode* pNode        = NULL;
    s32       nNode        = -1;
    s32       nParentNode  = pcTree->GetRootId();
	TSetInReq tReq;
    memset(&tReq,0,sizeof(tReq));
	//Account
	nNode = pcTree->FindNode(nParentNode, "account");
	if(nNode == -1)
	{
		tReq.m_aszUserName[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserName, pNode->m_pszNodeValue, CASCADE_USERNAME_MAXLEN-1);
	}
	//Password
	nNode = pcTree->FindNode(nParentNode, "password");
	if(nNode == -1)
	{
		tReq.m_aszUserPass[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserPass, pNode->m_pszNodeValue, CASCADE_USERPASS_MAXLEN-1);
	}
	/////////////////////
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tReq.m_nReqID = StrToNum(pNode->m_pszNodeValue);

    nNode=pcTree->FindNode(nParentNode,"pid");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    ExtractPid(pNode->m_pszNodeValue, &(tReq.m_dwPID), &(tReq.m_bLocal));

    nNode=pcTree->FindNode(nParentNode,"lid");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    tReq.m_nViewID = StrToNum(pNode->m_pszNodeValue);

    nNode=pcTree->FindNode(nParentNode,"sbfrindx");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    tReq.m_bySubFrameIndex = StrToNum(pNode->m_pszNodeValue);
    
	//get multicascadeInfo
	nParentNode             = pcTree->GetRootId();
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;
	
	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else
	{
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tReq.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tReq.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tReq.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tReq.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}
	//get multicascadeInfo end

    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_SETIN_REQ, (u8 *)&tReq, sizeof(tReq), m_dwUserData); 
    }
}

void CMCUCascade::OnInvitePartReq(CXMLTree *pcTree)
{
    
    TInvitePartReq tReq;
    TXMLNode      *pNode              = NULL;
    s32            nParentNode        = pcTree->GetRootId();
    s32            nNode              = -1;
    s32            nListCount         = 0;
    memset(&tReq,0,sizeof(tReq));

    u8 abyAliasType[PART_NUM] = { 0 };

	//Account
	nNode = pcTree->FindNode(nParentNode, "account");
	if(nNode == -1)
	{
		tReq.m_aszUserName[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserName, pNode->m_pszNodeValue, CASCADE_USERNAME_MAXLEN-1);
	}
	//Password
	nNode = pcTree->FindNode(nParentNode, "password");
	if(nNode == -1)
	{
		tReq.m_aszUserPass[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserPass, pNode->m_pszNodeValue, CASCADE_USERPASS_MAXLEN-1);
	}
	/////////////////////
    
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tReq.m_nReqID = StrToNum(pNode->m_pszNodeValue);
    
	// get pid
	nNode=pcTree->FindNode(nParentNode,"pid");
    if(nNode == -1)
    {
		//the peer doesn't support multiple cascade
        //return;
    }
	else
	{
		pNode=pcTree->GetNode(nNode);
		ExtractPid(pNode->m_pszNodeValue, &(tReq.m_dwPID), &(tReq.m_bLocal));
	}

    //parsing invited participant list BEGINGING
    do
    {
        nParentNode = pcTree->FindNode(nParentNode, "invite_part");
        if(nParentNode == -1)
        {
            break;
        }
        
        nNode = pcTree->FindNode(nParentNode, "dialstr");
        if(nNode == -1)
        {
            break;
        }
        pNode=pcTree->GetNode(nNode);
        if(pNode->m_pszNodeValue != NULL)
        {
            strncpy(tReq.m_atPart[nListCount].m_aszDialStr, pNode->m_pszNodeValue, STR_LEN + MAXLEN_CONFNAME);
        }

        //呼叫别名类型
        nNode = pcTree->FindNode(nParentNode, "dialstrtype");
        if(nNode != -1)
        {
            pNode=pcTree->GetNode(nNode);
            if(pNode->m_pszNodeValue != NULL)
            {
                abyAliasType[nListCount]=StrToNum(pNode->m_pszNodeValue);
            }
        }        

        nNode = pcTree->FindNode(nParentNode, "vianode");
        if(nNode == -1)
        {
            break;
        }
        pNode=pcTree->GetNode(nNode);
        u32    dwMcuId = 0;
        BOOL32 bKdvMcu = TRUE;
        ExtractPid(pNode->m_pszNodeValue, &(dwMcuId), &(bKdvMcu));
        
        nNode = pcTree->FindNode(nParentNode, "prefcallrate");
        if(nNode == -1)
        {
           tReq.m_atPart[nListCount].m_dwCallRate = 0;
        }
		else
		{
			pNode=pcTree->GetNode(nNode);
			tReq.m_atPart[nListCount].m_dwCallRate = StrToNum(pNode->m_pszNodeValue)/1000;
		}
        
        //Parsing View List where that participant is displaying itself BEGINGING
        nNode = pcTree->FindNode(nParentNode, "set_in_layout_list");
        s32 nVPosNode  = nNode;
        s32 nViewIndex =0;
        if(nNode != -1 )
        {
            do
            {
                nVPosNode = pcTree->FindNode(nVPosNode, "v_pos");
                if(nVPosNode == -1)
                {
                    break;
                }
                
                nNode = pcTree->FindNode(nVPosNode, "lid");
                if(nNode == -1)
                {
                    break;
                }
                pNode=pcTree->GetNode(nNode);
                tReq.m_atPart[nListCount].m_tPartVideoInfo.m_anViewPos[nViewIndex].m_nViewID
                                                 = StrToNum(pNode->m_pszNodeValue);

                nNode = pcTree->FindNode(nVPosNode, "sbfrindx");
                if(nNode == -1)
                {
                    break;
                }
                pNode=pcTree->GetNode(nNode);
                tReq.m_atPart[nListCount].m_tPartVideoInfo.m_anViewPos[nViewIndex].m_bySubframeIndex
                                                 = StrToNum(pNode->m_pszNodeValue);
               
                nViewIndex++;
                
                nVPosNode = pcTree->GetBrotherId(nVPosNode); //hual 跳至下一个
            } while(nVPosNode != -1 && nViewIndex < MAX_VIEWS_PER_CONF);
        }
        tReq.m_atPart[nListCount].m_tPartVideoInfo.m_nViewCount = nViewIndex;
        //Parsing View List where that participant is displaying itself ENDING
        
        //Parsing ViewId that is now displaying in the participant's monitor
        //and Video Scheme by which MCU distribute the video stream into the participant's monitor
        nNode = pcTree->FindNode(nParentNode, "set_out_layout_params");
        s32 nOutPutViewNode = nNode;
        if(nNode != -1)
        {
            nNode = pcTree->FindNode(nOutPutViewNode, "lid");
            if(nNode == -1)
            {
                break;
            }
            pNode=pcTree->GetNode(nNode);
            tReq.m_atPart[nListCount].m_tPartVideoInfo.m_nOutputViewID = StrToNum(pNode->m_pszNodeValue);
            
            nNode = pcTree->FindNode(nOutPutViewNode, "videooutputid");
            if(nNode == -1)
            {
                break;
            }
            pNode=pcTree->GetNode(nNode);
            tReq.m_atPart[nListCount].m_tPartVideoInfo.m_nOutVideoSchemeID = StrToNum(pNode->m_pszNodeValue);
        }

        nListCount++; 
        nParentNode = pcTree->GetBrotherId(nParentNode);

    }while(nParentNode != -1 && nListCount < PART_NUM);
    
    tReq.m_nCount=nListCount;
    //parsing invited participant list ENDING
    

	nParentNode             = pcTree->GetRootId();


	//get multicascadeInfo
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;
	
	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else
	{
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tReq.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tReq.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tReq.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tReq.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}
	//get multicascadeInfo end


    if(m_pfAppHandler != NULL)
    {
        u8 abyBuf[sizeof(tReq)+sizeof(abyAliasType)] = { 0 };
        memcpy( abyBuf, &tReq, sizeof(tReq));
        memcpy( abyBuf + sizeof(tReq), abyAliasType, sizeof(abyAliasType));
        m_pfAppHandler((s32)H_CASCADE_INVITEPART_REQ, (u8 *)&abyBuf, sizeof(abyBuf), m_dwUserData); 
        //m_pfAppHandler((s32)H_CASCADE_INVITEPART_REQ, (u8 *)&tReq, sizeof(tReq), m_dwUserData); 
    }
}

void CMCUCascade::OnReInvitePartReq(CXMLTree *pcTree)
{
     TXMLNode     *pNode         = NULL;
     s32           nNode         = -1;
     s32           nParentNode   = pcTree->GetRootId();
     TPartReq      tReinvitePIDReq;
     
     memset(&tReinvitePIDReq,0,sizeof(tReinvitePIDReq));
	 //Account
	 nNode = pcTree->FindNode(nParentNode, "account");
	 if(nNode == -1)
	 {
		 tReinvitePIDReq.m_aszUserName[0] = '\0';
	 }
	 else
	 {
		 pNode = pcTree->GetNode(nNode);
		 strncpy(tReinvitePIDReq.m_aszUserName, pNode->m_pszNodeValue, CASCADE_USERNAME_MAXLEN-1);
	 }
	 //Password
	 nNode = pcTree->FindNode(nParentNode, "password");
	 if(nNode == -1)
	 {
		 tReinvitePIDReq.m_aszUserPass[0] = '\0';
	 }
	 else
	 {
		 pNode = pcTree->GetNode(nNode);
		 strncpy(tReinvitePIDReq.m_aszUserPass, pNode->m_pszNodeValue, CASCADE_USERPASS_MAXLEN-1);
	 }
	 /////////////////////
     nNode = pcTree->FindNode(nParentNode, "requestid");
     if(nNode == -1)
     {
         return;
     }
     pNode = pcTree->GetNode(nNode);
     tReinvitePIDReq.m_nReqID = StrToNum(pNode->m_pszNodeValue);

     nNode=pcTree->FindNode(nParentNode,"pid");
     if(nNode == -1)
     {
         return;
     }
     pNode=pcTree->GetNode(nNode);
	 ExtractPid(pNode->m_pszNodeValue, &(tReinvitePIDReq.m_dwPID),&(tReinvitePIDReq.m_bLocal));
     
	 //get multicascadeInfo
	 u8  byLvlMtId = 0;
	 s32 nChildParentNode = -1;
	 s32 nChild2PNode = -1;
	 s32 nBrotherNode = -1;
	 
	 nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	 if(nNode == -1)
	 {
		 //the peer doesn't support multiple cascade
	 }
	 else
	 {
		 nChildParentNode = nNode;
		 
		 //src
		 nNode = pcTree->FindNode(nChildParentNode, "src");
		 if(nNode == -1)
		 {
			 return;
		 }
		 nChild2PNode = nNode;
		 nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		 if(nNode == -1)
		 {
			 return;
		 }
		 pNode = pcTree->GetNode(nNode);
		 if(pNode->m_pszNodeValue != NULL)
		 {
			 tReinvitePIDReq.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		 }
		 else
		 {
			 tReinvitePIDReq.m_tMsgSrc.m_byCasLevel = 0;
		 }
		 
		 nBrotherNode = nChild2PNode;
		 for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		 {
			 nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			 if(nNode == -1)
			 {
				 break;
			 }
			 pNode = pcTree->GetNode(nNode);
			 if(pNode->m_pszNodeValue != NULL)
			 {
				 tReinvitePIDReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			 }
			 else
			 {
				 tReinvitePIDReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			 }
			 
			 nBrotherNode = pcTree->GetBrotherId(nNode);
		 }
		 
		 // dst info
		 nNode = pcTree->FindNode(nChildParentNode, "dst");
		 if(nNode == -1)
		 {
			 return;
		 }
		 nChild2PNode = nNode;
		 nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		 if(nNode == -1)
		 {
			 return;
		 }
		 pNode = pcTree->GetNode(nNode);
		 if(pNode->m_pszNodeValue != NULL)
		 {
			 tReinvitePIDReq.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		 }
		 else
		 {
			 tReinvitePIDReq.m_tMsgDst.m_byCasLevel = 0;
		 }
		 
		 nBrotherNode = nChild2PNode;
		 for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		 {
			 nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			 if(nNode == -1)
			 {
				 break;
			 }
			 pNode = pcTree->GetNode(nNode);
			 if(pNode->m_pszNodeValue != NULL)
			 {
				 tReinvitePIDReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			 }
			 else
			 {
				 tReinvitePIDReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			 }
			 
			 nBrotherNode = pcTree->GetBrotherId(nNode);
		 }
	 }
	//get multicascadeInfo end

     if(m_pfAppHandler != NULL )
     {
         m_pfAppHandler((s32)H_CASCADE_REINVITEPART_REQ, (u8 *)&tReinvitePIDReq, sizeof(tReinvitePIDReq), m_dwUserData); 
     }
}

void CMCUCascade::OnPartDelReq(CXMLTree *pcTree)//not complete
{
    TXMLNode     *pNode                   =NULL;
    s32           nNode                   = -1;
    s32           nParentNode             = pcTree->GetRootId();
    TPartReq      tPartDelReq;
    
    memset(&tPartDelReq,0,sizeof(tPartDelReq));
	//Account
	nNode = pcTree->FindNode(nParentNode, "account");
	if(nNode == -1)
	{
		tPartDelReq.m_aszUserName[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tPartDelReq.m_aszUserName, pNode->m_pszNodeValue, CASCADE_USERNAME_MAXLEN-1);
	}
	//Password
	nNode = pcTree->FindNode(nParentNode, "password");
	if(nNode == -1)
	{
		tPartDelReq.m_aszUserPass[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tPartDelReq.m_aszUserPass, pNode->m_pszNodeValue, CASCADE_USERPASS_MAXLEN-1);
	}
	/////////////////////
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tPartDelReq.m_nReqID = StrToNum(pNode->m_pszNodeValue);
    
    nNode=pcTree->FindNode(nParentNode,"pid");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
	ExtractPid(pNode->m_pszNodeValue, &(tPartDelReq.m_dwPID),&(tPartDelReq.m_bLocal));
	
	//get multicascadeInfo
	nParentNode             = pcTree->GetRootId();
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;
	
	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else
	{
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tPartDelReq.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tPartDelReq.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tPartDelReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tPartDelReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tPartDelReq.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tPartDelReq.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tPartDelReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tPartDelReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}
	//get multicascadeInfo end

	if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_DELETEPART_REQ, (u8 *)&tPartDelReq, sizeof(tPartDelReq), m_dwUserData); 
    }
}

void CMCUCascade::OnPartDiscNtf(CXMLTree *pcTree)
{
    TXMLNode     *pNode =NULL;
    TDiscPartNF   tDiscPartNF;
    s32           nNode                   = -1;
    s32           nParentNode             = pcTree->GetRootId();
    
    memset(&tDiscPartNF,0,sizeof(tDiscPartNF));

    nNode=pcTree->FindNode(nParentNode,"pid");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    if(!ExtractPid(pNode->m_pszNodeValue, &(tDiscPartNF.m_dwPID)))
	{
		return;
	}
    
    nNode=pcTree->FindNode(nParentNode,"newpid");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    ExtractPid(pNode->m_pszNodeValue, &(tDiscPartNF.m_dwNewPID), &(tDiscPartNF.m_bLocal));
    
	//get multicascadeInfo
	nParentNode             = pcTree->GetRootId();
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;
	
	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else
	{
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tDiscPartNF.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tDiscPartNF.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tDiscPartNF.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tDiscPartNF.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tDiscPartNF.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tDiscPartNF.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tDiscPartNF.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tDiscPartNF.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}
	//get multicascadeInfo end

    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_DISCONNPART_NTF, (u8 *)&tDiscPartNF, sizeof(tDiscPartNF), m_dwUserData); 
    } 
}

void  CMCUCascade::OnRsp(CXMLTree *pcTree, /*const s8 *pszFlag,*/ H323MCUMsgType emMsgType)
{
    TXMLNode     *pNode                   = NULL;
    s32           nNode                   = -1;
    s32           nParentNode             = pcTree->GetRootId();

	if(emMsgType == H_CASCADE_INVITEPART_RSP
		|| emMsgType == H_CASCADE_REINVITEPART_RSP
		|| emMsgType == H_CASCADE_DISCONNPART_RSP
		|| emMsgType == H_CASCADE_DELETEPART_RSP
		|| emMsgType == H_CASCADE_SETIN_RSP
		|| emMsgType == H_CASCADE_PARTMEDIACHAN_RSP
		|| emMsgType == H_CASCADE_SETOUT_RSP
		)
	{
		TPartRsp          tRsp;
				
		memset(&tRsp, 0, sizeof(tRsp));
		
		nNode = pcTree->FindNode(nParentNode, "requestid");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		tRsp.m_nReqID = StrToNum(pNode->m_pszNodeValue);
		
		nNode=pcTree->FindNode(nParentNode,"returnvalue");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		tRsp.m_emReturnVal = GetReturnValFromStr(pNode->m_pszNodeValue);

		// get pid
		nNode=pcTree->FindNode(nParentNode,"pid");
		if(nNode == -1)
		{
			return;
		}
		pNode=pcTree->GetNode(nNode);
		if(!ExtractPid(pNode->m_pszNodeValue, &(tRsp.m_dwPID), &(tRsp.m_bLocal)))
		{
			return;
		}

		//get multicascadeInfo
		nParentNode             = pcTree->GetRootId();
		u8  byLvlMtId = 0;
		s32 nChildParentNode = -1;
		s32 nChild2PNode = -1;
		s32 nBrotherNode = -1;
		
		nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
		if(nNode == -1)
		{
			//the peer doesn't support multiple cascade
			//return;
		}
		else
		{
			nChildParentNode = nNode;
			
			//src
			nNode = pcTree->FindNode(nChildParentNode, "src");
			if(nNode == -1)
			{
				return;
			}
			nChild2PNode = nNode;
			nNode = pcTree->FindNode(nChild2PNode, "caslevel");
			if(nNode == -1)
			{
				return;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tRsp.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tRsp.m_tMsgSrc.m_byCasLevel = 0;
			}
			
			nBrotherNode = nChild2PNode;
			for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
			{
				nNode = pcTree->FindNode(nBrotherNode, "casmtid");
				if(nNode == -1)
				{
					break;
				}
				pNode = pcTree->GetNode(nNode);
				if(pNode->m_pszNodeValue != NULL)
				{
					tRsp.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
				}
				else
				{
					tRsp.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
				}
				
				nBrotherNode = pcTree->GetBrotherId(nNode);
			}
			
			// dst info
			nNode = pcTree->FindNode(nChildParentNode, "dst");
			if(nNode == -1)
			{
				return;
			}
			nChild2PNode = nNode;
			nNode = pcTree->FindNode(nChild2PNode, "caslevel");
			if(nNode == -1)
			{
				return;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tRsp.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tRsp.m_tMsgDst.m_byCasLevel = 0;
			}
			
			nBrotherNode = nChild2PNode;
			for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
			{
				nNode = pcTree->FindNode(nBrotherNode, "casmtid");
				if(nNode == -1)
				{
					break;
				}
				pNode = pcTree->GetNode(nNode);
				if(pNode->m_pszNodeValue != NULL)
				{
					tRsp.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
				}
				else
				{
					tRsp.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
				}
				
				nBrotherNode = pcTree->GetBrotherId(nNode);
			}
		}
		//get multicascadeInfo end

		if(m_pfAppHandler != NULL )
		{
			m_pfAppHandler((s32)emMsgType, (u8 *)&tRsp, sizeof(tRsp), m_dwUserData); 
		}
	}
	else
	{
		
		TRsp          tRsp;
		
		memset(&tRsp,0,sizeof(tRsp));
		
		nNode = pcTree->FindNode(nParentNode, "requestid");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		tRsp.m_nReqID = StrToNum(pNode->m_pszNodeValue);
		
		nNode=pcTree->FindNode(nParentNode,"returnvalue");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		tRsp.m_emReturnVal = GetReturnValFromStr(pNode->m_pszNodeValue);
		
		//get multicascadeInfo
		nParentNode             = pcTree->GetRootId();
		u8  byLvlMtId = 0;
		s32 nChildParentNode = -1;
		s32 nChild2PNode = -1;
		s32 nBrotherNode = -1;
		
		nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
		if(nNode == -1)
		{
			return;
		}
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tRsp.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tRsp.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tRsp.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tRsp.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tRsp.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tRsp.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tRsp.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tRsp.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		//get multicascadeInfo end

		if(m_pfAppHandler != NULL )
		{
			m_pfAppHandler((s32)emMsgType, (u8 *)&tRsp, sizeof(tRsp), m_dwUserData); 
		}
	}
}

void CMCUCascade::OnPartDiscReq(CXMLTree *pcTree)
{
    TXMLNode     *pNode                   = NULL;
    s32           nNode                   = -1;
    s32           nParentNode             = pcTree->GetRootId();
    TPartReq      tPartID;
    
    memset(&tPartID,0,sizeof(tPartID));
	//Account
	nNode = pcTree->FindNode(nParentNode, "account");
	if(nNode == -1)
	{
		tPartID.m_aszUserName[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tPartID.m_aszUserName, pNode->m_pszNodeValue, CASCADE_USERNAME_MAXLEN-1);
	}
	//Password
	nNode = pcTree->FindNode(nParentNode, "password");
	if(nNode == -1)
	{
		tPartID.m_aszUserPass[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tPartID.m_aszUserPass, pNode->m_pszNodeValue, CASCADE_USERPASS_MAXLEN-1);
	}
	/////////////////////
	
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tPartID.m_nReqID = StrToNum(pNode->m_pszNodeValue);


    nNode=pcTree->FindNode(nParentNode,"pid");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    if(!ExtractPid(pNode->m_pszNodeValue, &(tPartID.m_dwPID), &(tPartID.m_bLocal)))
	{
		return;
	}

	//get multicascadeInfo
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;
	
	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else
	{
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tPartID.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tPartID.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tPartID.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tPartID.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tPartID.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tPartID.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tPartID.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tPartID.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}
	//get multicascadeInfo end
    
    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_DISCONNPART_REQ, (u8 *)&tPartID, sizeof(tPartID), m_dwUserData); 
    }
}

void CMCUCascade::OnPartDelNtf(CXMLTree *pcTree)
{
    TXMLNode     *pNode                   =NULL;
    s32           nNode                   = -1;
    s32           nParentNode             = pcTree->GetRootId();
    TDelPartNF       tPartDelNF;
    
    memset(&tPartDelNF, 0, sizeof(tPartDelNF));
    
    nNode=pcTree->FindNode(nParentNode,"pid");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    if(!ExtractPid(pNode->m_pszNodeValue, &(tPartDelNF.m_dwPID), &(tPartDelNF.m_bLocal)))
	{
		return ;
	}
   
	//get multicascadeInfo
	nParentNode             = pcTree->GetRootId();
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;
	
	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else
	{
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tPartDelNF.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tPartDelNF.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tPartDelNF.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tPartDelNF.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tPartDelNF.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tPartDelNF.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tPartDelNF.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tPartDelNF.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}
	//get multicascadeInfo end

    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_DELETEPART_NTF, (u8 *)&tPartDelNF, sizeof(tPartDelNF), m_dwUserData); 
    } 
}

void CMCUCascade::OnSetOutLayoutReq(CXMLTree *pcTree)
{
    TXMLNode            *pNode                   =NULL;
    TSetOutReq           tReq;
    s32                  nNode                   = -1;
    s32                  nParentNode             = pcTree->GetRootId();
    s32                  nListCount              = 0;
    
    memset(&tReq,0,sizeof(tReq));
	//Account
	nNode = pcTree->FindNode(nParentNode, "account");
	if(nNode == -1)
	{
		tReq.m_aszUserName[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserName, pNode->m_pszNodeValue, CASCADE_USERNAME_MAXLEN-1);
	}
	//Password
	nNode = pcTree->FindNode(nParentNode, "password");
	if(nNode == -1)
	{
		tReq.m_aszUserPass[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserPass, pNode->m_pszNodeValue, CASCADE_USERPASS_MAXLEN-1);
	}
	/////////////////////   
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tReq.m_nReqID = StrToNum(pNode->m_pszNodeValue);
    
    nParentNode=pcTree->FindNode(nParentNode,"out_layout_part_list");
    if(nParentNode == -1)
    {
        return;
    }

    //participant list that demand the video stream of MCU
    do
    {
        nParentNode = pcTree->FindNode(nParentNode, "out_layout_part");
        if(nParentNode == -1)
        {
            break;
        }

        nNode=pcTree->FindNode(nParentNode,"pid");
        if(nNode == -1)
        {
            return;
        }
        pNode=pcTree->GetNode(nNode);
        ExtractPid(pNode->m_pszNodeValue, &(tReq.m_tPartOutInfo[nListCount].m_dwPID),
                                              &(tReq.m_tPartOutInfo[nListCount].m_bLocal));
        
        nNode=pcTree->FindNode(nParentNode,"lid");
        if(nNode == -1)
        {
            return;
        }
        tReq.m_tPartOutInfo[nListCount].m_nOutViewID = StrToNum(pNode->m_pszNodeValue);

        nNode=pcTree->FindNode(nParentNode,"videooutputid");
        if(nNode == -1)
        {
            return;
        }
        tReq.m_tPartOutInfo[nListCount].m_nOutVideoSchemeID = StrToNum(pNode->m_pszNodeValue);
        
        nListCount++;
        nParentNode = pcTree->GetBrotherId(nParentNode);

    }while(nParentNode != -1 && nListCount < PART_NUM);
    
    tReq.m_dwPartCount = nListCount;


	//get multicascadeInfo
	nParentNode             = pcTree->GetRootId();
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;
	
	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else
	{
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tReq.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tReq.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tReq.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tReq.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}
	//get multicascadeInfo end

    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_SETOUT_REQ, (u8 *)&tReq, sizeof(tReq), m_dwUserData); 
    } 
}

void CMCUCascade::OnLayoutChgNtf(CXMLTree *pcTree)
{
    TXMLNode            *pNode =NULL;
    TConfViewChgNtf      tNtf;
    s32                  nNode                   = -1;
    s32                  nParentNode             = pcTree->GetRootId();
    
    memset(&tNtf,0,sizeof(tNtf));
    
    nParentNode = pcTree->FindNode(nParentNode,"conference_layout_changed_notification");
    if(nParentNode == -1)
    {
        return;
    }
    
    //nNode=pcTree->FindNode(nParentNode,"lid");
    nNode=pcTree->FindNode(nParentNode,"video_geometry"); //ljx
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tNtf.m_nViewID = StrToNum(pNode->m_pszNodeValue);
    
    nNode = pcTree->FindNode(nParentNode,"videogeometrypresettype");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    tNtf.m_dwSubFrameCount = StrToNum(pNode->m_pszNodeValue)/10;
    
    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_CONFVIEW_CHG_NTF, (u8 *)&tNtf, sizeof(tNtf), m_dwUserData); 
    } 
}

void CMCUCascade::OnVASetInReq(CXMLTree *pcTree)
{
    TXMLNode            *pNode =NULL;
    TVAReq               tReq;
    s32                  nNode                   = -1;
    s32                  nParentNode             = pcTree->GetRootId();
    nParentNode  = pcTree->FindNode(nParentNode,"va_set_in_layout_request");
    memset(&tReq,0,sizeof(tReq));
	//Account
	nNode = pcTree->FindNode(nParentNode, "account");
	if(nNode == -1)
	{
		tReq.m_aszUserName[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserName, pNode->m_pszNodeValue, CASCADE_USERNAME_MAXLEN-1);
	}
	//Password
	nNode = pcTree->FindNode(nParentNode, "password");
	if(nNode == -1)
	{
		tReq.m_aszUserPass[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserPass, pNode->m_pszNodeValue, CASCADE_USERPASS_MAXLEN-1);
	}
	/////////////////////   
	
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tReq.m_nReqID = StrToNum(pNode->m_pszNodeValue);

    nParentNode  = pcTree->FindNode(nParentNode,"lid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tReq.m_nViewID = StrToNum(pNode->m_pszNodeValue);
	pcTree->FindNode(nParentNode,"sbfrindx");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tReq.m_bySubframeIndex = StrToNum(pNode->m_pszNodeValue);
    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_VA_REQ, (u8 *)&tReq, sizeof(tReq), m_dwUserData); 
    } 
}

void CMCUCascade::OnPartMediaChanReq(CXMLTree *pcTree)
{
    TXMLNode            *pNode =NULL;
    TPartMediaChanReq    tReq;
    s32                  nNode                   = -1;
    s32                  nParentNode             = pcTree->GetRootId();
    
    memset(&tReq,0,sizeof(tReq));
	//Account
	nNode = pcTree->FindNode(nParentNode, "account");
	if(nNode == -1)
	{
		tReq.m_aszUserName[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserName, pNode->m_pszNodeValue, CASCADE_USERNAME_MAXLEN-1);
	}
	//Password
	nNode = pcTree->FindNode(nParentNode, "password");
	if(nNode == -1)
	{
		tReq.m_aszUserPass[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserPass, pNode->m_pszNodeValue, CASCADE_USERPASS_MAXLEN-1);
	}
	/////////////////////   
	
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tReq.m_nReqID = StrToNum(pNode->m_pszNodeValue);
    
    nNode=pcTree->FindNode(nParentNode,"pid");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    if(!ExtractPid(pNode->m_pszNodeValue, &(tReq.m_dwPID), &(tReq.m_bLocal)))
	{
		return;
	}
    
    nNode=pcTree->FindNode(nParentNode,"channelmediatype");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    tReq.m_emMediaType = StrToMediaType(pNode->m_pszNodeValue);

    nNode=pcTree->FindNode(nParentNode,"channeldirection");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    tReq.m_emMuteDirection = StrToMuteDirect(pNode->m_pszNodeValue);

    nNode=pcTree->FindNode(nParentNode,"muteon");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue)
    {
        tReq.m_bMute = (NullStrCmpLimited(pNode->m_pszNodeValue,pszStrTrue) == 0);
    }
    else
    {
        tReq.m_bMute = FALSE;
    }

	//get multicascadeInfo
	nParentNode             = pcTree->GetRootId();
	u8  byLvlMtId = 0;
	s32 nChildParentNode = -1;
	s32 nChild2PNode = -1;
	s32 nBrotherNode = -1;
	
	nNode = pcTree->FindNode(nParentNode, "multicascadeinfo");
	if(nNode == -1)
	{
		//the peer doesn't support multiple cascade
		//return;
	}
	else
	{
		nChildParentNode = nNode;
		
		//src
		nNode = pcTree->FindNode(nChildParentNode, "src");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tReq.m_tMsgSrc.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tReq.m_tMsgSrc.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tReq.m_tMsgSrc.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
		
		// dst info
		nNode = pcTree->FindNode(nChildParentNode, "dst");
		if(nNode == -1)
		{
			return;
		}
		nChild2PNode = nNode;
		nNode = pcTree->FindNode(nChild2PNode, "caslevel");
		if(nNode == -1)
		{
			return;
		}
		pNode = pcTree->GetNode(nNode);
		if(pNode->m_pszNodeValue != NULL)
		{
			tReq.m_tMsgDst.m_byCasLevel = (u8) StrToNum(pNode->m_pszNodeValue);
		}
		else
		{
			tReq.m_tMsgDst.m_byCasLevel = 0;
		}
		
		nBrotherNode = nChild2PNode;
		for( byLvlMtId = 0; byLvlMtId < MAX_CASCADELEVEL; byLvlMtId ++)
		{
			nNode = pcTree->FindNode(nBrotherNode, "casmtid");
			if(nNode == -1)
			{
				break;
			}
			pNode = pcTree->GetNode(nNode);
			if(pNode->m_pszNodeValue != NULL)
			{
				tReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = (u8) StrToNum(pNode->m_pszNodeValue);
			}
			else
			{
				tReq.m_tMsgDst.m_abyMtIdentify[byLvlMtId] = 0;
			}
			
			nBrotherNode = pcTree->GetBrotherId(nNode);
		}
	}
	//get multicascadeInfo end

    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_PARTMEDIACHAN_REQ, (u8 *)&tReq, sizeof(tReq), m_dwUserData); 
    } 
}

void CMCUCascade::OnPartMediaChanNtf(CXMLTree *pcTree)
{
    TXMLNode            *pNode =NULL;
    TPartMediaChanNtf    tNtf;
    s32                  nNode                   = -1;
    s32                  nParentNode             = pcTree->GetRootId();
    
    memset(&tNtf,0,sizeof(tNtf));
    
    nNode=pcTree->FindNode(nParentNode,"pid");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    ExtractPid(pNode->m_pszNodeValue, &(tNtf.m_dwPID), &(tNtf.m_bLocal));
    
    nNode=pcTree->FindNode(nParentNode,"channelmediatype");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    tNtf.m_emMediaType = StrToMediaType(pNode->m_pszNodeValue);
    
    nNode=pcTree->FindNode(nParentNode,"channeldirection");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    tNtf.m_emMuteDirection = StrToMuteDirect(pNode->m_pszNodeValue);
    
    nNode=pcTree->FindNode(nParentNode,"muteon");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue)
    {
        tNtf.m_bMute = (NullStrCmpLimited(pNode->m_pszNodeValue,pszStrTrue) == 0);
    }
    else
    {
        tNtf.m_bMute = FALSE;
    }
    
    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_PARTMEDIACHAN_NTF, (u8 *)&tNtf, sizeof(tNtf), m_dwUserData); 
    } 
}

void CMCUCascade::OnNonStandardMsgReq(CXMLTree *pcTree)
{
	TXMLNode            *pNode =NULL;
    TNonStandardReq      tReq;
    s32                  nNode                   = -1;
    s32                  nParentNode             = pcTree->GetRootId();
	
    memset(&tReq,0,sizeof(tReq));
	nNode = pcTree->FindNode(nParentNode, "account");
	if(nNode == -1)
	{
		tReq.m_aszUserName[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserName, pNode->m_pszNodeValue, CASCADE_USERNAME_MAXLEN-1);
	}
	//Password
	nNode = pcTree->FindNode(nParentNode, "password");
	if(nNode == -1)
	{
		tReq.m_aszUserPass[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserPass, pNode->m_pszNodeValue, CASCADE_USERPASS_MAXLEN-1);
	}
	/////////////////////
    nNode = pcTree->FindNode(nParentNode, "requestid");
	if(nNode == -1)
	{
		return;
	}
	pNode = pcTree->GetNode(nNode);
    tReq.m_nReqID   = StrToNum(pNode->m_pszNodeValue);
	//
    nNode=pcTree->FindNode(nParentNode,"bytelen");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
	tReq.m_nMsgLen = StrToNum(pNode->m_pszNodeValue)/2;
    nNode=pcTree->FindNode(nParentNode,"nonstandardmsg");
	if(nNode == -1)
	{
		tReq.m_nMsgLen = 0;
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		tReq.m_nMsgLen = Str2ToOct(pNode->m_pszNodeValue, tReq.m_abyMsgBuf, sizeof(tReq.m_abyMsgBuf));
	}

    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_NONSTANDARD_REQ, (u8 *)&tReq, sizeof(tReq), m_dwUserData); 
    } 
	
}

void CMCUCascade::OnNonStandardMsgNtf(CXMLTree *pcTree)
{
	TXMLNode            *pNode =NULL;
    TNonStandardMsg      tMsg;
    s32                  nNode                   = -1;
    s32                  nParentNode             = pcTree->GetRootId();
	
 
    nNode=pcTree->FindNode(nParentNode,"bytelen");
    if(nNode == -1)
    {
        return;
    }
    pNode=pcTree->GetNode(nNode);
	tMsg.m_nMsgLen = StrToNum(pNode->m_pszNodeValue)/2;
	//
    nNode=pcTree->FindNode(nParentNode,"nonstandardmsg");
	if(nNode == -1)
	{
		tMsg.m_nMsgLen = 0;
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		tMsg.m_nMsgLen = Str2ToOct(pNode->m_pszNodeValue, tMsg.m_abyMsgBuf, sizeof(tMsg.m_abyMsgBuf)); 
	}

    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_NONSTANDARD_NTF, (u8 *)&tMsg, sizeof(tMsg), m_dwUserData); 
    } 
	
}

void CMCUCascade::OnNonStandardMsgRsp(CXMLTree *pcTree)
{
   	TXMLNode            *pNode =NULL;
    TNonStandardRsp      tRsp;
    s32                  nNode                   = -1;
    s32                  nParentNode             = pcTree->GetRootId();
	
    memset(&tRsp,0,sizeof(tRsp));
	//
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tRsp.m_nReqID = StrToNum(pNode->m_pszNodeValue);
	//return value
    nNode = pcTree->FindNode(pNode->m_nParentId, "returnvalue");
	if(nNode == -1)
	{
		return;
	}
    pNode = pcTree->GetNode(nNode);
    tRsp.m_emReturnVal = GetReturnValFromStr(pNode->m_pszNodeValue); 
	//
	nNode=pcTree->FindNode(nParentNode,"bytelen");
    if(nNode == -1)
    {
        return;
    }
   
	pNode=pcTree->GetNode(nNode);
	tRsp.m_nMsgLen = StrToNum(pNode->m_pszNodeValue)/2;
    nNode=pcTree->FindNode(nParentNode,"nonstandardmsg");
	if(nNode == -1)
	{
		tRsp.m_nMsgLen = 0;
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		tRsp.m_nMsgLen = Str2ToOct(pNode->m_pszNodeValue, tRsp.m_abyMsgBuf, sizeof(tRsp.m_abyMsgBuf));
	}
	//
    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_NONSTANDARD_RSP, (u8 *)&tRsp, sizeof(tRsp), m_dwUserData); 
    } 
}


//add 2007.06.13
void CMCUCascade::OnSetLayoutAutoswitchReq(CXMLTree *pcTree)
{
    TXMLNode     *pNode                   = NULL;
    s32           nNode                   = -1;
    s32           nParentNode             = pcTree->GetRootId();
    TAutoswitchReq      tAutoSwitchReq;
    
    memset(&tAutoSwitchReq,0,sizeof(tAutoSwitchReq));
	//Account
	nNode = pcTree->FindNode(nParentNode, "account");
	if(nNode == -1)
	{
		tAutoSwitchReq.m_aszUserName[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tAutoSwitchReq.m_aszUserName, pNode->m_pszNodeValue, CASCADE_USERNAME_MAXLEN-1);
	}
	//Password
	nNode = pcTree->FindNode(nParentNode, "password");
	if(nNode == -1)
	{
		tAutoSwitchReq.m_aszUserPass[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tAutoSwitchReq.m_aszUserPass, pNode->m_pszNodeValue, CASCADE_USERPASS_MAXLEN-1);
	}
	/////////////////////
	
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tAutoSwitchReq.m_nReqID = StrToNum(pNode->m_pszNodeValue);
	
    nNode  = pcTree->FindNode(nParentNode,"lid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tAutoSwitchReq.m_nLid = StrToNum(pNode->m_pszNodeValue);
    
    nNode = pcTree->FindNode(nParentNode, "switchon");
	if(nNode == -1)
	{
		return;
	}
    pNode = pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue!=NULL)
    {
        tAutoSwitchReq.m_bAutoSwitchOn = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) == 0);
    }
    else
    {
        tAutoSwitchReq.m_bAutoSwitchOn = FALSE;
    } 
	
    nNode  = pcTree->FindNode(nParentNode,"auto_switch_time");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tAutoSwitchReq.m_nAutoSwitchTime = StrToNum(pNode->m_pszNodeValue);
	
	
    nNode  = pcTree->FindNode(nParentNode,"auto_switch_level");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tAutoSwitchReq.m_nAutoSwitchLevel = StrToNum(pNode->m_pszNodeValue);
	
    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_SET_LAYOUT_AUTOSWITCH_REQUEST, (u8 *)&tAutoSwitchReq, sizeof(tAutoSwitchReq), m_dwUserData); 
    }	
}

void CMCUCascade::OnSetLayoutAutoswitchRsp(CXMLTree *pcTree)
{
   	TXMLNode            *pNode =NULL;
    TAutoswitchRsp       tRsp;
    s32                  nNode                   = -1;
    s32                  nParentNode             = pcTree->GetRootId();
	
    memset(&tRsp,0,sizeof(tRsp));
	//
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tRsp.m_nReqID = StrToNum(pNode->m_pszNodeValue);
	//return value
    nNode = pcTree->FindNode(pNode->m_nParentId, "returnvalue");
	if(nNode == -1)
	{
		return;
	}
    pNode=pcTree->GetNode(nNode);
    tRsp.m_emReturnVal = GetReturnValFromStr(pNode->m_pszNodeValue);
	
    if(m_pfAppHandler != NULL )
    {
        m_pfAppHandler((s32)H_CASCADE_SET_LAYOUT_AUTOSWITCH_RESPONSE, (u8 *)&tRsp, sizeof(tRsp), m_dwUserData); 
    } 
}


CXMLTree* CMCUCascade::ToNonStandardMsgReq(u8 *pBuf, s32 nLen)
{
	TNonStandardReq *pReq = (TNonStandardReq *)pBuf;
	if((u32)nLen != sizeof(TNonStandardReq))
	{
		return NULL;
	}
	
	s8 szBuf[CASCADE_NONSTANDARDMSG_MAXLEN*2+2];
	
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Request", pReq->m_aszUserName,  pReq->m_aszUserPass);
    if(nNode == -1)
	{
		return NULL;
	}
	//
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Cascade_NonStandard_Request", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
    
    //
    nParentNode = nNode;
    nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pReq->m_nReqID));
    if(nNode == -1)
    {
        return NULL;
    }
    
    //
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, "ByteLen", NumToStr(pReq->m_nMsgLen*2));
	if(nNode == -1)
	{
		return NULL;
	}
    //	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, "NonStandardMsg",
		OctToStr2(pReq->m_abyMsgBuf,pReq->m_nMsgLen, szBuf, sizeof(szBuf)));
	if(nNode == -1)
	{
		return NULL;
	}
	//
	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToNonStandardMsgRsp(u8 *pBuf, s32 nLen)
{
	TNonStandardRsp *pRsp = (TNonStandardRsp *)pBuf;
	if((u32)nLen != sizeof(TNonStandardRsp))
	{
		return NULL;
	}

	s8 szBuf[CASCADE_NONSTANDARDMSG_MAXLEN*2+2];
	
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Response");
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Cascade_NonStandard_Response", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pRsp->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, "ByteLen", NumToStr(pRsp->m_nMsgLen*2));
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
							"NonStandardMsg", 
							OctToStr2(pRsp->m_abyMsgBuf, pRsp->m_nMsgLen, szBuf, sizeof(szBuf)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ReturnValue", ReturnValToStr(pRsp->m_emReturnVal));
	if(nNode == -1)
	{
		return NULL;
	}
	return g_pXMLEncTree;  
}

CXMLTree* CMCUCascade::ToNonStandardMsgNtf(u8 *pBuf, s32 nLen)
{
	TNonStandardMsg *ptMsg = (TNonStandardMsg *)pBuf;
	if((u32)nLen != sizeof(TNonStandardMsg))
	{
		return NULL;
	}
	s8 szBuf[CASCADE_NONSTANDARDMSG_MAXLEN*2+2];
	s32 nNode = AddXMLHead(g_pXMLEncTree, "Notification");
    if(nNode == -1)
    {
        return NULL;
    }
    
    s32 nParentNode = nNode;
    nNode = AddXMLChild(g_pXMLEncTree, nNode, "Cascade_NonStandard_Notification", NULL);
    if(nNode == -1)
    {
        return NULL;
    }
    
    nParentNode = nNode;
    nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "Cookie", NumToStr(m_dwPeerCookie));
    if(nNode == -1)
    {
        return NULL;
    }
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, "ByteLen", NumToStr(ptMsg->m_nMsgLen*2));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
					"NonStandardMsg", 
					OctToStr2(ptMsg->m_abyMsgBuf,  ptMsg->m_nMsgLen, szBuf, sizeof(szBuf)));
	if(nNode == -1)
	{
		return NULL;
	}
	return g_pXMLEncTree;  
}

//////////////////////////////////////////////////////////////////////////
void CMCUCascade::OnMcuServiceListReq(CXMLTree *pcTree)
{
	TRegUnRegReq tReq;
    TXMLNode *pNode = NULL;
	s32 nParentNode = pcTree->GetRootId();
    s32 nNode       = -1;
	//Account
	nNode = pcTree->FindNode(nParentNode, "account");
	if(nNode == -1)
	{
		tReq.m_aszUserName[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserName, pNode->m_pszNodeValue, CASCADE_USERNAME_MAXLEN-1);
	}
	//Password
	nNode = pcTree->FindNode(nParentNode, "password");
	if(nNode == -1)
	{
		tReq.m_aszUserPass[0] = '\0';
	}
	else
	{
		pNode = pcTree->GetNode(nNode);
		strncpy(tReq.m_aszUserPass, pNode->m_pszNodeValue, CASCADE_USERPASS_MAXLEN-1);
	}
	/////////////////////
    nNode = pcTree->FindNode(nParentNode, "requestid");
	if(nNode == -1)
	{
		return;
	}
	pNode = pcTree->GetNode(nNode);
    tReq.m_nReqID   = StrToNum(pNode->m_pszNodeValue);
    
    nNode = pcTree->FindNode(nParentNode, "register");
	if(nNode == -1)
	{
		return;
	}
    pNode = pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue!=NULL)
    {
        tReq.m_bReg = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) == 0);
    }
    else
    {
        tReq.m_bReg = FALSE;
    } 
    
	if(m_pfAppHandler != NULL)
	{
		m_pfAppHandler((s32)H_CASCADE_REGUNREG_REQ, (u8 *)&tReq, sizeof(tReq), m_dwUserData); 
	}
}

// void CMCUCascade::OnMcuConfListReq(CXMLTree *pcTree)
// {
// 
// }
// void CMCUCascade::OnMcuConfProfileReq(CXMLTree *pcTree)
// {
// 
// }
// void CMCUCascade::OnMcuConfIdByNameReq(CXMLTree *pcTree)
// {
// 
// }
void CMCUCascade::OnMcuServiceListRsp(CXMLTree *pcTree)
{
	TRegUnRegRsp tRsp;
    TXMLNode *pNode = NULL;
    s32 nParentNode = pcTree->GetRootId();
    s32 nNode       = -1;
    
    nNode = pcTree->FindNode(nParentNode, "requestid");
    if(nNode == -1)
    {
        return;
    }
    pNode = pcTree->GetNode(nNode);
    tRsp.m_nReqID = StrToNum(pNode->m_pszNodeValue);
    
	//register
    nNode = pcTree->FindNode(nParentNode, "register");
	if(nNode == -1)
	{
		return;
	}
    pNode = pcTree->GetNode(nNode);
    if(pNode->m_pszNodeValue!=NULL)
    {
        tRsp.m_bReg = (NullStrCmpLimited(pNode->m_pszNodeValue, pszStrTrue) == 0);
    }
    else
    {
        tRsp.m_bReg = FALSE;
    }
	
	//return value
    nNode = pcTree->FindNode(pNode->m_nParentId, "returnvalue");
	if(nNode == -1)
	{
		return;
	}
    pNode = pcTree->GetNode(nNode);
    tRsp.m_emReturnVal = GetReturnValFromStr(pNode->m_pszNodeValue);
	
	if(m_pfAppHandler != NULL)
	{
		m_pfAppHandler((s32)H_CASCADE_REGUNREG_RSP, (u8 *)&tRsp, sizeof(tRsp), m_dwUserData); 
	}
}
// void CMCUCascade::OnMcuConfListRsp(CXMLTree *pcTree)
// {
// 
// }
// void CMCUCascade::OnMcuConfProfileRsp(CXMLTree *pcTree)
// {
// 
// }
// void CMCUCascade::OnMcuConfIdByNameRsp(CXMLTree *pcTree)
// {
// 
// }

//////////////////////////////////////////////////////////////////////////
CXMLTree* CMCUCascade::ToMcuServiceListReq(u8 *pBuf, s32 nLen)
{
	TMCUServiceListReq *pReq = (TMCUServiceListReq *)pBuf;
	if((u32)nLen != sizeof(TMCUServiceListReq))
	{
		return NULL;
	}
	
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Request", pReq->m_aszUserName,  pReq->m_aszUserPass);
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Get_Services_Request", NULL);
	if(nNode == -1)
	{
		return NULL;
	}

	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pReq->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}
	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToMcuConfListReq(u8 *pBuf, s32 nLen)
{
	TMCUConfListReq *pReq = (TMCUConfListReq *)pBuf;
	if((u32)nLen != sizeof(TMCUConfListReq))
	{
		return NULL;
	}
	
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Request", pReq->m_aszUserName,  pReq->m_aszUserPass);
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Get_Conference_List_Request", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pReq->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
		"SnapShotValue", "0");
	if(nNode == -1)
	{
		return NULL;
	}
	
	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToMcuConfProfileReq(u8 *pBuf, s32 nLen)
{
	TConfProfileReq *pReq = (TConfProfileReq *)pBuf;
	if((u32)nLen != sizeof(TConfProfileReq))
	{
		return NULL;
	}
	
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Request", pReq->m_aszUserName,  pReq->m_aszUserPass);
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Get_Conference_Info_Request", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pReq->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
		"ConfGID", OctToStr2(m_abyDstConfID, sizeof(m_abyDstConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
		"SnapShotValue", "0");
	if(nNode == -1)
	{
		return NULL;
	}
	
	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToMcuConfIdByNameReq(u8 *pBuf, s32 nLen)
{
	TGetConfIDByNameReq *pReq = (TGetConfIDByNameReq *)pBuf;
	if((u32)nLen != sizeof(TGetConfIDByNameReq))
	{
		return NULL;
	}
	
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Request", pReq->m_aszUserName,  pReq->m_aszUserPass);
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "Get_ConfGID_By_Name_Request", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pReq->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
		"Name", pReq->m_aszConfName);
	if(nNode == -1)
	{
		return NULL;
	}

	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ConfPassword", pReq->m_aszConfPass);
	if(nNode == -1)
	{
		return NULL;
	}

	return g_pXMLEncTree;
}

CXMLTree* CMCUCascade::ToMcuServiceListRsp(u8 *pBuf, s32 nLen)
{
	TRegUnRegRsp *pRsp = (TRegUnRegRsp *)pBuf;
	if((u32)nLen != sizeof(TRegUnRegRsp))
	{
		return NULL;
	}
	
    s32 nNode = AddXMLHead(g_pXMLEncTree, "Response");
    if(nNode == -1)
	{
		return NULL;
	}
	
    s32 nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nNode, "RegUnreg_Neighbor_Response", NULL);
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nParentNode = nNode;
	nNode = AddXMLChild(g_pXMLEncTree, nParentNode, "RequestID", NumToStr(pRsp->m_nReqID));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
		"ConfGID", OctToStr2(m_abySrcConfID, sizeof(m_abySrcConfID)));
	if(nNode == -1)
	{
		return NULL;
	}
	
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
		"Register", BOOL_STR(pRsp->m_bReg));
	if(nNode == -1)
	{
		return NULL;
	}
	
	nNode = AddXMLBrother(g_pXMLEncTree, nParentNode, nNode, 
						  "ReturnValue", ReturnValToStr(pRsp->m_emReturnVal));
	if(nNode == -1)
	{
		return NULL;
	}
	return g_pXMLEncTree;
}

// CXMLTree* CMCUCascade::ToMcuConfListRsp(u8 *pBuf, s32 nLen)
// {
// 	return g_pXMLEncTree;
// 	
// }
// 
// CXMLTree* CMCUCascade::ToMcuConfProfileRsp(u8 *pBuf, s32 nLen)
// {
// 	return g_pXMLEncTree;
// 	
// }
// 
// CXMLTree* CMCUCascade::ToMcuConfIdByNameRsp(u8 *pBuf, s32 nLen)
// {
// 	return g_pXMLEncTree;
// 	
// }


////////////////////////////////
//Debug Functions
API void mmcusetlog(u8 level)
{
    g_nloglevel = (s32)level;	
    OspPrintf(1,0,"[mmculib] g_nloglevel = %d\n", g_nloglevel);

	if( g_nloglevel == 0 )
	{
		logdisablemod(MID_MCULIB_MMCULIB);	
	}
	else
	{
		logenablemod(MID_MCULIB_MMCULIB);
	}
}

API void mmcuhelp()
{
	::OspPrintf( TRUE,
		FALSE,
		"mmculib ver: %s\t compile time: %s:%s  macro STR_LEN:%d\n",
		VER_MMCULIB,
		__DATE__,
		__TIME__, STR_LEN);
	OspPrintf(TRUE, FALSE, "These are some debug commands:\n");
	OspPrintf(TRUE, FALSE, "	===============================================\n");
	OspPrintf(TRUE, FALSE, "	name	 :      mmcusetlog(level).\n");
	OspPrintf(TRUE, FALSE, "	Function :      set mmcu print level.\n");
	OspPrintf(TRUE, FALSE, "	Param	 :      u8 level * %d printf exception log;\n", MMCU_EXP);
	OspPrintf(TRUE, FALSE, "	                           %d printf important log;\n", MMCU_IMT);
	OspPrintf(TRUE, FALSE, "	                           %d printf debug log\n", MMCU_DEBUG);
	OspPrintf(TRUE, FALSE, "	                           %d printf all   log\n", MMCU_ALL);
	OspPrintf(TRUE, FALSE, "	                           %d printf info to file\n", MMCU_FILE);
	OspPrintf(TRUE, FALSE, "	===============================================\n");
}
/*lint -restore*/

