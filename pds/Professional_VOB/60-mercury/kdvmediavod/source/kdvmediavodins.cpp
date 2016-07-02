#include "kdvmediavod.h"
// #include "kdvsys.h"
#include "rpctrl.h"
#include "kdvmediavodinstance.h"

enum
{
    LOG_NOLOG = 0, 
    LOG_VOD_DEBUG,    
    LOG_VOD_EXP,
};


CMediavodAPP    g_cMediaVodApp;
TVodConfig      g_tVodConfig;
extern int		g_nLanguageType;


#define CHECKMSG(wRet, pMsg, TMsgType)              \
do {                                                \
    if (NULL == pMsg || NULL == pMsg->content)      \
    {                                               \
        wRet = ERROR_MEDIAVOD_PARAM; break;         \
    }                                               \
                                                    \
    if (sizeof(TMsgType) != pMsg->length)           \
    {                                               \
        wRet = ERROR_MEDIAVOD_PARAM; break;         \
    }                                               \
    wRet = MEDIAVOD_NO_ERROR;                       \
} while(0)


u16  GetAbsolutenessPath(s8* pBuffIn,s8* pBuffOut,u32 pBuffOutSize);
u16  FindFile(char* path, TVodFileList* ptVodFileList); //查找文件
void ReadConfigFile();
void GetMediaType(u8 byTypeNum,s8* wPayLoadType);

void NotifyPlayRateCB(u32 dwPlayerID, u32 dwCurrentPlayTime, u32 dwContext);


s32 MediaVodInit(TVodConfig* ptVodConfig)
{
    if (NULL == ptVodConfig)
    {
        return -1;
    }
    
    s8* pszRootDir = ptVodConfig->m_achRootDir;
    
    s32 nLen = strlen(pszRootDir);
    if (nLen > 0)
    {
        if (pszRootDir[nLen-1] != '\\')
        {
            pszRootDir[nLen] = '\\';
            pszRootDir[nLen+1] = '\0';
        }
    }
/*
    ptVodConfig->

    if ((pBuffOut[i-1] == '\\')&&(pBuffIn[0] == '\\'))
    {
        pBuffOut[i-1] = '\0';
    }
    else if((pBuffOut[i-1] != '\\')&&(pBuffIn[0] != '\\'))
    {
        pBuffOut[i] = '\\';
    }
*/        
    g_cMediaVodApp.CreateApp("KdvMediaVodServer", MEDIAVOD_SERVER_ID, MEDIAVOD_SERVER_PRI, 500);
    
    ::OspPost(MAKEIID(MEDIAVOD_SERVER_ID, CInstance::DAEMON), OSP_POWERON, ptVodConfig, sizeof(TVodConfig));
    return 0;
}


s32 MediaVodEnd(TVodConfig* ptVodConfig)
{
    return 0;
}


/////////////////////////////////////////////////
// Ckdvmediavodinstance 类函数定义
/////////////////////////////////////////////////

Ckdvmediavodinstance::Ckdvmediavodinstance()
{
    m_dwPlayID       = INVALID_PALYID;
	m_pcKdvASFFileReader = NULL;
}



//DAEMON 处理
void Ckdvmediavodinstance::OnPowerOn(CMessage* const pcMsg)
{
    u16 wRet;
    CHECKMSG(wRet, pcMsg, TVodConfig);
    if (wRet != 0)
    {
        return;
    }
    
//    g_tVodConfig = *(TVodConfig*)pcMsg->content;

    //创建监听
    OspCreateTcpNode(0, g_tVodConfig.m_wVodListenPort);
    
    //初始化RPCtrl
    TRPInitParam tRPInitParam;
    tRPInitParam.m_dwInitialPlayerNum = MEDIAVOD_MAX_CLIENT;
    tRPInitParam.m_dwInitialRecorderNum = 0;
    tRPInitParam.m_dwContext = 0;
    tRPInitParam.m_dwRemainSpaceAlarmLevelOne = 0;
    tRPInitParam.m_dwRemainSpaceAlarmLevelTwo = 0;
    tRPInitParam.m_pCallback = NULL;
    if (RPInit(&tRPInitParam) != RP_OK)//初始化录放像库
    {
        VodLog(LOG_EXP, "RPInit error !");
        return ;
    }
	// 2011-11-17 新需求：提示等到窗口初始化成功之后1s再出现
	OspDelay(1000);
	
	if( !OspIsLocalHost( g_tVodConfig.m_dwLocalIP ) )
	{
		// 2011-10-14 add by pgf: 新需求：配置文件为非本地IP时，给出窗口警告
        if ( g_nLanguageType == LANGUAGE_CHINESE )
        {
            AfxMessageBox( "服务器配置IP和本地IP不一致，将导致其不能正常工作，请重新配置为本地IP" );
        }
        else
        {
			// 启动语言，根据系统和语言默认设置而定
            AfxMessageBox( "Server maynot work without local IP, please configure right IP" );
        }
	}
}

void Ckdvmediavodinstance::OnPowerOff(CMessage* const pcMsg)
{

}

void Ckdvmediavodinstance::DaemonQuit()
{
	if (m_pcKdvASFFileReader != NULL)
	{
		m_pcKdvASFFileReader->Close();
		delete m_pcKdvASFFileReader;
		m_pcKdvASFFileReader = NULL;
	}

	OspSemGive(g_hSemQuit);
}

u16 Ckdvmediavodinstance::IsAutherUser(s8* pszUserName, s8* pszPassword)
{
    BOOL32 bRet;
    s8 achPassword[64];

    if (pszUserName == NULL ||
        pszPassword == NULL ||
        pszUserName[0] == '\0')
    {
        return ERROR_USERNOTEXIST;
    }
     
	bRet = GetRegKeyString(CONFIG_FILE_NAME,
        "USERCONFIG",
        pszUserName, 
        "",
        achPassword,
        sizeof(achPassword));
    if (!bRet)
    {
        return ERROR_USERNOTEXIST;
    }

    if (0 != strcmp(achPassword, pszPassword))
    {
        return ERROR_INVALIDPASSWD;
    }

    return MEDIAVOD_NO_ERROR;
}

void Ckdvmediavodinstance::OnClientReg(CMessage * const pcMsg, CApp* pcApp)
{
    u16 wRet;
    CHECKMSG(wRet, pcMsg, TVodReg);
    if (wRet != 0)
    {
        return;
    }

    TVodReg tVodReg;
    memcpy(&tVodReg, pcMsg->content, sizeof(TVodReg));

    wRet = IsAutherUser(tVodReg.m_achUserName, tVodReg.m_achPassword);
    if (MEDIAVOD_NO_ERROR != wRet)
    {
        u8 byRejectCode;

        if (ERROR_USERNOTEXIST == wRet)
        {
            byRejectCode = MEDIAVOD_REJ_USERNOTEXIST;
        }
        else if (ERROR_INVALIDPASSWD == wRet)
        {
            byRejectCode = MEDIAVOD_REJ_INVALIDPASSWD;
        }
        else
        {
            byRejectCode = MEDIAVOD_REJ_NOREASON;
        }
        
        post(pcMsg->srcid, MEDIAVOD_REG_REJ, &byRejectCode, sizeof(byRejectCode), pcMsg->srcnode);
        VodLog(LOG_DEBUG, "Deamon process ClientReg: Reject for %d.\n", byRejectCode);

        return;
    }

    TVodClientInfo tClientInfo;
    tClientInfo.m_dwClientId = pcMsg->srcid;
    tClientInfo.m_dwNodeId = pcMsg->srcnode;
    tClientInfo.m_LoginTime = time(NULL);  //tmp

    VodLog(LOG_DEBUG, "Recv Clinet Register: SrcId:0x%x SrcNodeId:%d\n",
           tClientInfo.m_dwClientId, tClientInfo.m_dwNodeId);

    strcpy(tClientInfo.m_achUserName, tVodReg.m_achUserName);
    strcpy(tClientInfo.m_achPassword, tVodReg.m_achPassword);

    s32 nIndex;
    CInstance* pcInst;
    s32 nInstNum = pcApp->GetInstanceNumber();

    for (nIndex = 1; nIndex <= nInstNum; nIndex++)
    {
        pcInst = pcApp->GetInstance(nIndex);
        if (VOD_STATE_LOGOUT == pcInst->CurState())
        {
            break;
        }
    }

    if (nIndex <= nInstNum)
    {
        post(MAKEIID(MEDIAVOD_SERVER_ID, CInstance::PENDING), MEDIAVOD_REG_REQ, &tClientInfo, sizeof(TVodClientInfo));
    }
    else
    {
        u8 byRejectCode = MEDIAVOD_REJ_MAXUSER;
        post(pcMsg->srcid, MEDIAVOD_REG_REJ, &byRejectCode, sizeof(byRejectCode), pcMsg->srcnode);
        VodLog(LOG_DEBUG, "Deamon process ClientReg: Reject for max user.\n");
    }

    return;
}

//Instance 处理
void Ckdvmediavodinstance::InstanceDump(u32 dwParam)
{
    u32 dwState = CurState();
    if (dwState == VOD_STATE_LOGOUT)
    {
        return;
    }

    struct tm t;
    t = *localtime(&m_tClientInfo.m_LoginTime);

    OspPrintf(TRUE, FALSE, "Inst:%d State:%d ClientId:0x%x NodeId:%d LoginTime:%2.2u:%2.2u:%2.2u\n",
              GetInsID(), dwState, m_tClientInfo.m_dwClientId, m_tClientInfo.m_dwNodeId,
              t.tm_hour, t.tm_min, t.tm_sec);


    if (dwState == VOD_STATE_PLAYSTART || dwState == VOD_STATE_PLAYPAUSE)
    {
        OspPrintf(TRUE, FALSE, "  PlayInfo FileName=%s Format A:%u V1:%u V2:%u ClientIP=0x%x  Port: A=%d V1=%d V2=%d\n",
                  m_tPlayInfo.m_achFileName,
                  m_tPlayInfo.m_byAudioType,
                  m_tPlayInfo.m_byVideo1Type,
                  m_tPlayInfo.m_byVideo2Type,
                  m_tPlayInfo.m_dwIP,
                  m_tPlayInfo.m_wAudioPort,
                  m_tPlayInfo.m_wVideo1Port,
                  m_tPlayInfo.m_wVideo2Port);
    }
}

void Ckdvmediavodinstance::InstanceEntry(CMessage* const pcMsg)
{
    if (pcMsg == NULL)
    {
        return;
    }
    
    switch(pcMsg->event)
    {
    case OSP_DISCONNECT:
        {                        
            OnDisconnect(pcMsg);
        } break;

    case MEDIAVOD_REG_REQ:
        {
            OnClientUp(pcMsg);
        } break;

    case MEDIAVOD_FILELIST_REQ:
        {
            OnFileListReq(pcMsg);			
        }break;
    
    case MEDIAVOD_FILEINFO_REQ:
        {
            OnFileInfoReq(pcMsg);
        }break;
    
    case MEDIAVOD_PLAY_REQ:
        {
            OnPlayReq(pcMsg);
        }break;
    
    case MEDIAVOD_STOP_REQ:
        {
            OnStopReq(pcMsg);
        }break;
    
    case MEDIAVOD_PLAYPAUSE_REQ:
        {
            OnPauseReq(pcMsg);
        }break;
    
    case MEDIAVOD_PLAYRESUME_REQ:
        {
            OnResumeReq(pcMsg);
        }break;
    
    case MEDIAVOD_PLAYGOTO_REQ:
        {
            OnPalyGotoReq(pcMsg);
        }break;
    
    case MEDIAVOD_PLAYINFO_REQ:
        {
            OnPlayInfoReq(pcMsg);
        }break;
    
    case MEDIAVOD_PLAYINFO_NTF:
        {
            OnNotifyPlayInfo(pcMsg);
        } break;

    case MEDIAVOD_UNREG_REQ:
        {
            OnUnRegReq(pcMsg);
        }break;
    
    case MEDIAVOD_STATE_REQ:
        {
        
            //先处理请求,然后用OSP发送处理结果
            //......
            /*
            OspPost(pcMsg->srcid,
                MEDIAVOD_STATE_ACK,&m_byState,
                sizeof(m_byState),pcMsg->srcnode,pcMsg->dstid);
            */
        }break;
        
    default:
        break;
    }
    
    return;
}


void Ckdvmediavodinstance::OnClientUp(CMessage * const pcMsg)
{
    u16 wRet;
    CHECKMSG(wRet, pcMsg, TVodClientInfo);
    if (wRet != 0)
    {
        return;
    }

    memcpy(&m_tClientInfo, pcMsg->content, sizeof(TVodClientInfo));

    post(m_tClientInfo.m_dwClientId, 
         MEDIAVOD_REG_ACK,
         NULL, 0, 
         m_tClientInfo.m_dwNodeId);

    OspNodeDiscCBReg(m_tClientInfo.m_dwNodeId, MEDIAVOD_SERVER_ID, GetInsID());

    NEXTSTATE(VOD_STATE_LOGIN);

    VodLog(LOG_DEBUG, "Inst:%d Send MEDIAVOD_REG_ACK\n", GetInsID());

    return;
}

void Ckdvmediavodinstance::InstClear()
{
    if (m_dwPlayID != INVALID_PALYID)
    {
        StopPlay(); 
        u16 wRet = RPReleasePlay(m_dwPlayID);
        if( wRet != RP_OK)
        {
            OspPrintf(TRUE,FALSE,"RPReleasePlay error, Error:%u\n", wRet);
        }
        m_dwPlayID = INVALID_PALYID;
    }

    NEXTSTATE(VOD_STATE_LOGOUT);
    
    return;
}

void Ckdvmediavodinstance::OnDisconnect(CMessage * const pcMsg)
{
    if (pcMsg->srcnode != pcMsg->dstnode)
    {
        return;
    }

    InstClear();

    return;
}


 
//文件列表请求
void Ckdvmediavodinstance::OnFileListReq(CMessage* const pcMsg)
{
    u16 wRet;
    CHECKMSG(wRet, pcMsg,  TVodListReq);
    if (wRet != 0)
    {
        u8 byErr = 0;
        post(m_tClientInfo.m_dwClientId,
             MEDIAVOD_FILELIST_REJ,
             &byErr, sizeof(u8),
             m_tClientInfo.m_dwNodeId);
        return;
    }

    TVodListReq tVodListReq;
    memcpy(&tVodListReq, pcMsg->content, sizeof(TVodListReq));

    VodLog(LOG_DEBUG, "Inst:%d OnRecvFileListReq: Dir=\"%s\"\n",
           GetInsID(), tVodListReq.m_achDirName);
    
    TVodFileList tVodFileList;
    FindFile(tVodListReq.m_achDirName, &tVodFileList);

    static u8 abyMsgBuff[MEDIAVOD_MAX_TRANSF_BUF];

    u32 dwMsgLen = sizeof(abyMsgBuff);

    wRet = tVodFileList.SetFileListToString(abyMsgBuff, &dwMsgLen);
    if (MEDIAVOD_NO_ERROR == wRet)
    {
        post(m_tClientInfo.m_dwClientId, 
            MEDIAVOD_FILELIST_ACK,
            abyMsgBuff, (u16)dwMsgLen, 
            m_tClientInfo.m_dwNodeId);
        
        VodLog(LOG_DEBUG, "Inst:%d File List ACK success.\n",
               GetInsID());
    }
    else
    {
        u8 byErr = MEDIAVOD_REJ_FILE_LIST_FAIL;
        post(m_tClientInfo.m_dwClientId, 
            MEDIAVOD_FILELIST_REJ,
            &byErr, sizeof(u8), 
             m_tClientInfo.m_dwNodeId);

        VodLog(LOG_EXP, "Inst:%d File List error. Error=%u\n",
               GetInsID(), wRet);

    }

    return;
}



void Ckdvmediavodinstance::OnFileInfoReq(CMessage* const pcMsg)
{

    u16 wRet;
    CHECKMSG(wRet, pcMsg, TVodFileInfoReq);
    if (wRet != 0)
    {
        u8 byErr = MEDIAVOD_REJ_MSG_NOT_MATCH;
        post(m_tClientInfo.m_dwClientId,
            MEDIAVOD_FILEINFO_REJ,
            &byErr, sizeof(u8),
            m_tClientInfo.m_dwNodeId);
        return;
    }


    TVodFileInfoReq tVodfileinfoReq;
    memcpy(&tVodfileinfoReq, pcMsg->content, sizeof(TVodFileInfoReq));
    

    VodLog(LOG_DEBUG, "Inst:%d OnRecvFileInfoReq: File=\"%s\"\n",
              GetInsID(), tVodfileinfoReq.m_achFileName);
    
    TVodFileInfoAck tVodFileInfoAck;			
	u8 byChnlNum = 0;
	u32 dwSamplesPerSecond = 0;
    wRet = GetFileInfo(tVodfileinfoReq.m_achFileName, &tVodFileInfoAck, byChnlNum, dwSamplesPerSecond);
    if (wRet == MEDIAVOD_NO_ERROR)
    {
		s8 szStr[sizeof(TVodFileInfoAck)+sizeof(u8)+sizeof(u32)+1] = {0};
		memcpy(szStr, &tVodFileInfoAck, sizeof(TVodFileInfoAck));
		memcpy(szStr+sizeof(tVodFileInfoAck), &byChnlNum, sizeof(u8));
		memcpy(szStr+sizeof(tVodFileInfoAck)+sizeof(u8), &dwSamplesPerSecond, sizeof(u32));
		
        post(m_tClientInfo.m_dwClientId,
			MEDIAVOD_FILEINFO_ACK,
			szStr, sizeof(TVodFileInfoAck)+sizeof(u8)+sizeof(u32),
                m_tClientInfo.m_dwNodeId);
        
        VodLog(LOG_DEBUG, "Inst:%d File Info ACK success, Duration:%ds Audio:%d Video1:%d Video2:%d,chnlNum:%d,samples:%d\n", 
                GetInsID(), 
                tVodFileInfoAck.GetDuration(),
                tVodFileInfoAck.m_byAudioType,
                tVodFileInfoAck.m_byVideo1Type,
                tVodFileInfoAck.m_byVideo2Type, byChnlNum, dwSamplesPerSecond);

    }
    else
    {
        u8 byErr;
        if (wRet == ERROR_MEDIAVOD_PARAM)
        {
            byErr = MEDIAVOD_REJ_PARAM_NOTASF;
        }
        else
        {
            byErr = MEDIAVOD_REJ_NOREASON;
        }
        
        post(m_tClientInfo.m_dwClientId,
            MEDIAVOD_FILEINFO_REJ,
            &byErr, sizeof(u8),
            m_tClientInfo.m_dwNodeId);

        VodLog(LOG_EXP, "Inst:%d File Info error. Error=%u\n",
               GetInsID(), wRet);
    }

    return;
}


void Ckdvmediavodinstance::OnPlayReq(CMessage* const pcMsg)
{
    u16 wRet;
    CHECKMSG(wRet, pcMsg, TVodPlayReq);
    if (wRet != 0)
    {
        u8 byErr = MEDIAVOD_REJ_MSG_NOT_MATCH;
        post(m_tClientInfo.m_dwClientId,
            MEDIAVOD_PLAY_REJ,
            &byErr, sizeof(u8),
            m_tClientInfo.m_dwNodeId);

        return;
    }

    
    TVodPlayReq* ptVodplay = (TVodPlayReq*)pcMsg->content;  
    
    VodLog(LOG_DEBUG, "Inst:%d OnRecvPlayReq: File=\"%s\" IP=0x%x,Audio:%d Video1=%d Video2=%d\n",
            GetInsID(),
            ptVodplay->m_achFileName,
            ptVodplay->GetAddrIP(),
            ptVodplay->GetAudioPort(),
            ptVodplay->GetVideo1Port(),
            ptVodplay->GetVideo2Port());
    
    if (CurState() == VOD_STATE_PLAYSTART)
    {
        u8 byErr = MEDIAVOD_REJ_PLAY_BUSY;
        post(m_tClientInfo.m_dwClientId,
             MEDIAVOD_PLAY_REJ,
             &byErr, sizeof(u8),
             m_tClientInfo.m_dwNodeId);

        VodLog(LOG_DEBUG, "Inst:%d Play error. State = VOD_STATE_PLAYSTART\n",
               GetInsID());
        return;
    }
    
    if (strlen(ptVodplay->m_achFileName) == 0)
    {
        u8 byErr = MEDIAVOD_REJ_PARAM_NOTASF;
        post(m_tClientInfo.m_dwClientId,
             MEDIAVOD_PLAY_REJ,
             &byErr, sizeof(u8),
             m_tClientInfo.m_dwNodeId);

        VodLog(LOG_DEBUG, "Inst:%d Play error. File Name Error.\n",
               GetInsID());
        return;
    }	
    
    wRet = StartPlay(ptVodplay);
    if (MEDIAVOD_NO_ERROR == wRet)
    {
        post(m_tClientInfo.m_dwClientId,
            MEDIAVOD_PLAY_ACK,
            NULL, 0,
            m_tClientInfo.m_dwNodeId);

        VodLog(LOG_DEBUG, "Inst:%d Play success.\n",
               GetInsID());
        
        strcpy(m_tPlayInfo.m_achFileName, ptVodplay->m_achFileName);
        m_tPlayInfo.m_dwIP = ptVodplay->GetAddrIP();
        m_tPlayInfo.m_wAudioPort = ptVodplay->GetAudioPort();
        m_tPlayInfo.m_wVideo1Port = ptVodplay->GetVideo1Port();
        m_tPlayInfo.m_wVideo2Port = ptVodplay->GetVideo2Port();

        NEXTSTATE(VOD_STATE_PLAYSTART);
    }
    else
    {
        u8 byErr = MEDIAVOD_REJ_NOREASON;
        post(m_tClientInfo.m_dwClientId,
            MEDIAVOD_PLAY_REJ,
            &byErr, sizeof(u8),
            m_tClientInfo.m_dwNodeId);

        VodLog(LOG_DEBUG, "Inst:%d Play error. Error=%u\n",
               GetInsID(), wRet);
	}

    return;
}


void Ckdvmediavodinstance::OnStopReq(CMessage* const pcMsg)
{
    u16 wRet = StopPlay();
    if (wRet  == MEDIAVOD_NO_ERROR)
    {
        post(m_tClientInfo.m_dwClientId,
            MEDIAVOD_STOP_ACK,
            NULL, 0,
            m_tClientInfo.m_dwNodeId);

        VodLog(LOG_DEBUG, "Inst:%d Play Stop success.\n",
               GetInsID());

        NEXTSTATE(VOD_STATE_LOGIN);

    }
    else
    {
        u8 byErr = MEDIAVOD_REJ_OPERATE_FAIL;
        post(m_tClientInfo.m_dwClientId,
            MEDIAVOD_STOP_REJ,
            &byErr, sizeof(u8),
            m_tClientInfo.m_dwNodeId);
        
        VodLog(LOG_EXP, "Inst:%d Play Stop error. Error=%u\n",
               GetInsID(), wRet);
	}

    return;
}


void Ckdvmediavodinstance::OnPauseReq(CMessage* const pcMsg)
{
    u16 wRet = PausePlay();
    if (wRet == MEDIAVOD_NO_ERROR)
    {
        post(m_tClientInfo.m_dwClientId,
             MEDIAVOD_PLAYPAUSE_ACK,
             NULL, 0,
             m_tClientInfo.m_dwNodeId);
        
        NEXTSTATE(VOD_STATE_PLAYPAUSE);

        VodLog(LOG_DEBUG, "Inst:%d Play Pause success\n", GetInsID());

    }
    else
    {
        u8 byErr = MEDIAVOD_REJ_OPERATE_FAIL;
        post(m_tClientInfo.m_dwClientId,
             MEDIAVOD_PLAYPAUSE_REJ,
             &byErr, sizeof(u8),
             m_tClientInfo.m_dwNodeId);


        VodLog(LOG_DEBUG, "Inst:%d Play Pause error. Error=%u\n",
               GetInsID(), wRet);
	}
    
    return;
}

void Ckdvmediavodinstance::OnResumeReq(CMessage* const pcMsg)
{
    if (CurState() != VOD_STATE_PLAYPAUSE)
    {
        u8 byErr = MEDIAVOD_REJ_OPERATE_FAIL;
        post(m_tClientInfo.m_dwClientId,
             MEDIAVOD_PLAYRESUME_REJ,
             &byErr, sizeof(u8),
             m_tClientInfo.m_dwNodeId);
        
        VodLog(LOG_DEBUG, "Inst:%d Play Resume error. State != VOD_STATE_PLAYPAUSE\n", GetInsID());

        return;
    }
    
    u16 wRet = StartAgain();
    if (wRet == MEDIAVOD_NO_ERROR)
    {
        post(m_tClientInfo.m_dwClientId,
             MEDIAVOD_PLAYRESUME_ACK,
             NULL, 0,
             m_tClientInfo.m_dwNodeId);

        NEXTSTATE(VOD_STATE_PLAYSTART);

        VodLog(LOG_DEBUG, "Inst:%d Play Resume success\n", GetInsID());

    }
    else
    {
        u8 byErr = MEDIAVOD_REJ_OPERATE_FAIL;
        post(m_tClientInfo.m_dwClientId,
            MEDIAVOD_PLAYRESUME_REJ,
            &byErr, sizeof(u8),
            m_tClientInfo.m_dwNodeId);
        
        VodLog(LOG_DEBUG, "Inst:%d Play Resume error. Error=%u\n",
               GetInsID(), wRet);
	}

    return;
}



void Ckdvmediavodinstance::OnPalyGotoReq(CMessage* const pcMsg)
{
    u16 wRet;
    CHECKMSG(wRet, pcMsg, TVodPlayGotoReq);
    if (wRet != 0)
    {
        u8 byErr = MEDIAVOD_REJ_MSG_NOT_MATCH;
        post(m_tClientInfo.m_dwClientId,
             MEDIAVOD_PLAYGOTO_REJ,
             &byErr, sizeof(u8),
             m_tClientInfo.m_dwNodeId);

        return;
    }


    TVodPlayGotoReq* pvodplaytotoreq = (TVodPlayGotoReq*)pcMsg->content;

    VodLog(LOG_DEBUG, "Inst:%d OnRecvPalyGotoReq: time=%ds\n",
                  GetInsID(), pvodplaytotoreq->GetPlayTime());

    if ((CurState() != VOD_STATE_PLAYPAUSE) && (CurState() != VOD_STATE_PLAYSTART))
    {
        
        u8 byErr = MEDIAVOD_REJ_STATE_ERROR;
        post(m_tClientInfo.m_dwClientId,
             MEDIAVOD_PLAYGOTO_REJ,
             &byErr, sizeof(u8),
             m_tClientInfo.m_dwNodeId);

        VodLog(LOG_DEBUG, "Inst:%d Play Goto error. State != VOD_STATE_PLAYPAUSE && State != VOD_STATE_PLAYSTART\n",
               GetInsID());

        return;
    }
    
    wRet = AppiontPlay(pvodplaytotoreq);
    if (wRet == MEDIAVOD_NO_ERROR)
    {
        post(m_tClientInfo.m_dwClientId,
             MEDIAVOD_PLAYGOTO_ACK,
             NULL, 0,
             m_tClientInfo.m_dwNodeId);

        NEXTSTATE(VOD_STATE_PLAYSTART);

        VodLog(LOG_DEBUG, "Inst:%d Play Goto success.\n",
               GetInsID());

    }
    else
    {
        u8 byErr = MEDIAVOD_REJ_OPERATE_FAIL;
        post(m_tClientInfo.m_dwClientId,
             MEDIAVOD_PLAYGOTO_REJ,
             &byErr, sizeof(u8),
             m_tClientInfo.m_dwNodeId);

        VodLog(LOG_DEBUG, "Inst:%d Play Goto error. Error=%u\n",
               GetInsID(), wRet);
	}

    return;
}

void Ckdvmediavodinstance::OnPlayInfoReq(CMessage* const pcMsg)
{
    VodLog(LOG_DEBUG, "Inst:%d OnRecvPlayInfoReq\n", GetInsID());

    TVodPlayInfoAck tVodPlayInfoAck;
    memset(&tVodPlayInfoAck, 0, sizeof(TVodPlayInfoAck));

    u16 wRet = PlayInfo(&tVodPlayInfoAck);	
    if (wRet == MEDIAVOD_NO_ERROR)
    {                
        post(m_tClientInfo.m_dwClientId, 
             MEDIAVOD_PLAYINFO_ACK,
             &tVodPlayInfoAck, sizeof(tVodPlayInfoAck) ,
             m_tClientInfo.m_dwNodeId);

        VodLog(LOG_DEBUG, "Inst:%d Play Info ACK success, Play time:%ds Duration:%ds\n", 
               GetInsID(), tVodPlayInfoAck.GetPlayTime(), tVodPlayInfoAck.GetDuration());
    }
    else
    {
        u8 byErr = 0;
        post(m_tClientInfo.m_dwClientId, 
             MEDIAVOD_PLAYINFO_REJ,
             &byErr, sizeof(u8),
             m_tClientInfo.m_dwNodeId);

        VodLog(LOG_DEBUG, "Inst:%d Play Info error. Error=%u\n", 
               GetInsID(), wRet);
    }
}

void Ckdvmediavodinstance::OnUnRegReq(CMessage* const pcMsg)
{
    VodLog(LOG_DEBUG, "Inst:%d OnRecvUnRegReq\n", GetInsID());
    
    if (CurState() ==  VOD_STATE_PLAYSTART)
    {
        StopPlay();
    }
    
    post(m_tClientInfo.m_dwClientId, 
         MEDIAVOD_UNREG_ACK,
         NULL, 0,
         m_tClientInfo.m_dwNodeId);

    InstClear();

    return;
}


void Ckdvmediavodinstance::OnNotifyPlayInfo(CMessage * const pcMsg)
{
    u16 wRet;
    CHECKMSG(wRet, pcMsg,  TPlayRateMsg);
    if (wRet != 0)
    {
        return;
    }
    
    TPlayRateMsg tPlayRateMsg;
    memcpy(&tPlayRateMsg, pcMsg->content, sizeof(TPlayRateMsg));
    
    if (tPlayRateMsg.wCheckValue != m_wPlayCheckValue)
    {
        return;
    }

    if (tPlayRateMsg.byRate == 100)
    {
        u8 byRate = 100;
        post(m_tClientInfo.m_dwClientId, 
            MEDIAVOD_PLAYINFO_NTF,
            &byRate, sizeof(u8), 
            m_tClientInfo.m_dwNodeId);

        VodLog(LOG_DEBUG, "Inst:%d Play Info Notify, Play rate=%u\n", 
               GetInsID(), byRate);
    }

    return;
}


void Ckdvmediavodinstance::DaemonInstanceEntry(CMessage* const pcMsg, CApp* pcApp)
{
	
	switch(pcMsg->event)
	{
    case OSP_POWERON:
        {
            OnPowerOn(pcMsg);
        } break;
        
	case MEDIAVOD_REG_REQ: 
		{
            OnClientReg(pcMsg, pcApp);
		}
		break;
        
    case MEDIAVOD_SERVERSTATE_REQ:
        {
            /*
            u16 wCount;
            u32 dwBusyInstanceTotal = 0;
            u32 dwPlayState = 0;
            u32 dwPause = 0;
            Ckdvmediavodinstance* pkdvmediavodinstance = NULL;
            for (wCount = 1; wCount <= MEDIAVOD_MAX_CLIENT; wCount++)//寻找空闲实例
            {
                pkdvmediavodinstance = (Ckdvmediavodinstance*)(pcApp->GetInstance(wCount));
                if(pkdvmediavodinstance->m_instanceState == Busy)															
                {
                    
                    dwBusyInstanceTotal++;
                    if(pkdvmediavodinstance->m_byState == VOD_STATE_PLAYSTART)
                    {
                        dwPlayState++;
                    }
                    if(pkdvmediavodinstance->m_byState == VOD_STATE_PLAYPAUSE)
                    {
                        dwPause++;
                    }
                }
                
            }   
            OspPrintf(TRUE,FALSE,"\n 当前一共有 %d 个客户端登录",dwBusyInstanceTotal);
            OspPrintf(TRUE,FALSE,"\n 当前一共有 %d 个客户端处于播放状态中",dwPlayState);
            OspPrintf(TRUE,FALSE,"\n 当前一共有 %d 个客户端处于暂停播放状态中 \n",dwPause);
            
            for (wCount = 1; wCount <= MEDIAVOD_MAX_CLIENT; wCount++)
            {
                pkdvmediavodinstance = (Ckdvmediavodinstance*)(pcApp->GetInstance(wCount));
                if(pkdvmediavodinstance->m_instanceState == Busy)															
                {
                       
                    if(pkdvmediavodinstance->m_byState == VOD_STATE_LOGIN)
                    {

                                       
                    }

                    if(pkdvmediavodinstance->m_byState == VOD_STATE_PLAYSTART)
                    {
                        OspPrintf(TRUE,FALSE,"\n \n当前处于播放状态中的Instance %d 详细信息：", wCount); 
                        u8 achDir[MEDIAVOD_MAX_PATHFILENAME];
                        memset(achDir, 0, sizeof(achDir));
                        memcpy(achDir, g_tVodConfig.m_achRootDir, sizeof(achDir));
                        strcat((char*)&achDir,pkdvmediavodinstance->m_tVodPlayReq.m_achFileName);
                        OspPrintf(TRUE,FALSE,"\n播放的文件名：%s",achDir);
                        u32 dwIP = 0;
                        dwIP = pkdvmediavodinstance->m_tVodPlayReq.GetAddrIP();                      
                        OspPrintf(TRUE,FALSE,"\n客户端IP：%d.%d.%d.%d",((u8*)(&dwIP))[0],((u8*)(&dwIP))[1],((u8*)(&dwIP))[2],((u8*)(&dwIP))[3]);
                        s8 achMediaType[30];
                        memset(achMediaType,0,sizeof(achMediaType));
                        GetMediaType(pkdvmediavodinstance->m_tVodFileInfoAck.m_byAudioType,achMediaType);
                        OspPrintf(TRUE,FALSE,"\n客户端AudioPort： %d ,编码类型：%s",pkdvmediavodinstance->m_tVodPlayReq.GetAudioPort(),achMediaType);
                        memset(achMediaType,0,sizeof(achMediaType));
                        GetMediaType(pkdvmediavodinstance->m_tVodFileInfoAck.m_byVideo1Type,achMediaType);                        
                        OspPrintf(TRUE,FALSE,"\n客户端Video1Port：%d ,编码类型：%s",pkdvmediavodinstance->m_tVodPlayReq.GetVideo1Port(),achMediaType);
                        memset(achMediaType,0,sizeof(achMediaType));
                        GetMediaType(pkdvmediavodinstance->m_tVodFileInfoAck.m_byVideo2Type,achMediaType);                              
                        OspPrintf(TRUE,FALSE,"\n客户端Video2Port：%d ,编码类型：%s \n",pkdvmediavodinstance->m_tVodPlayReq.GetVideo2Port(),achMediaType);                       
                    }

                    if(pkdvmediavodinstance->m_byState == VOD_STATE_PLAYPAUSE)
                    {
                        
                    }
                }
            }  */ 
        }
		break;
	case MEDIAVOD_QUIT:
		{
			DaemonQuit();
		}
	default:
		break;
	}		
}


u16 Ckdvmediavodinstance::GetFileInfo(s8* pszFileName, TVodFileInfoAck* ptVodFileInfoAck, u8 &byChnlNum, u32 &dwSamplesPerSecond)
{
	byChnlNum = 1;
    if (!IsValidPlayFile(pszFileName))
    {
        return ERROR_FILE_PATH;
    }

    static s8 achAbsPath[MEDIAVOD_MAX_PATHFILENAME];

    u16 wRet = 0;
    wRet = GetAbsolutenessPath(pszFileName, achAbsPath, sizeof(achAbsPath));
	if (wRet != MEDIAVOD_NO_ERROR)
    {
        return ERROR_MEDIAVOD_BASE;
    }
    
    if (m_dwPlayID == INVALID_PALYID)
    {
        u16 wCP = RPCreatePlay(&m_dwPlayID);
        if( wCP != RP_OK)
        {
            VodLog(LOG_EXP, "RPCreatePlay error, Error:%u\n", wRet);
            return wRet;
        }
    }

    TMediaInfo tMediaInfo;
    wRet = RPGetFileMediaInfo(m_dwPlayID, achAbsPath, &tMediaInfo);
    if (0 != wRet)
    {
        VodLog(LOG_EXP, "RPCreatePlay error, Error:%u\n", wRet);
        return wRet;
    }

	if ( 0 == tMediaInfo.m_byNum )
	{
		VodLog(LOG_EXP, "MediaInfo stream num is zero!\n");
		return ERROR_MEDIAVOD_PARAM;
	}

	if ( !IsVideoType(tMediaInfo.m_abyMediaType[1]) )
	{
		tMediaInfo.m_abyMediaType[1] = tMediaInfo.m_abyMediaType[2];
	}

	ptVodFileInfoAck->m_byAudioType = tMediaInfo.m_abyMediaType[0];
	ptVodFileInfoAck->m_byVideo1Type = tMediaInfo.m_abyMediaType[1];
	ptVodFileInfoAck->m_byVideo2Type = tMediaInfo.m_abyMediaType[2];

	if (m_pcKdvASFFileReader == NULL)
	{
		m_pcKdvASFFileReader = new CKdvASFFileReader();
	}

	if (m_pcKdvASFFileReader == NULL)
	{
		return ERROR_GET_FILEINFO;
	}

    //得到文件播放时长
    wRet = m_pcKdvASFFileReader->Open(achAbsPath);
    if (wRet != 0)
    {
        return ERROR_MEDIAVOD_PARAM;
    }

	TASFFileProperty tFileProperty;
	m_pcKdvASFFileReader->GetASFFileProPerty(&tFileProperty);  

#if defined (WIN32)
    
    FILETIME tFileTime;
    u64 qwRecTime = 0;
    qwRecTime = tFileProperty.qwCreateDate;
    tFileTime.dwLowDateTime   = (u32)(qwRecTime & 0xffffffff);
    tFileTime.dwHighDateTime  = (u32)((qwRecTime >> 32) & 0xffffffff); 
    SYSTEMTIME tSystemTime;
    FileTimeToSystemTime(&tFileTime, &tSystemTime);
    CTime t( tSystemTime.wYear, tSystemTime.wMonth,tSystemTime.wDay , tSystemTime.wHour, tSystemTime.wMinute, tSystemTime.wSecond ); 
    time_t osBinaryTime = t.GetTime();     
	ptVodFileInfoAck->SetRecordTime(osBinaryTime);

#endif // defined (WIN32)

    u32 dwRecordTime = (u32)(tFileProperty.qwPlayDuration/10000000);

	ptVodFileInfoAck->SetDuration(dwRecordTime);

	//[2012/11/1 zhangli]取音频声道和采样率
	for (u8 byLoop = 0; byLoop < tMediaInfo.m_byNum; ++byLoop)
	{
		TStreamProperty tTStreamProperty;
		if (RP_OK == m_pcKdvASFFileReader->GetStreamPropertyByIndex(byLoop, &tTStreamProperty)
			&& 1 == tTStreamProperty.byStreamNumber)	//音频流序号=1
		{
			byChnlNum = tTStreamProperty.tAudioMediaInfo.wNumberOfChannels;
			dwSamplesPerSecond = tTStreamProperty.tAudioMediaInfo.dwSamplesPerSecond;
		}
	}

    m_pcKdvASFFileReader->Close();

	return MEDIAVOD_NO_ERROR;
}


u16 Ckdvmediavodinstance::StartPlay(TVodPlayReq* ptVodPlayReq)
{
    u16 wRet;
    if (ptVodPlayReq == NULL)
    {
        return ERROR_MEDIAVOD_PARAM;
    }

    if (!IsValidPlayFile(ptVodPlayReq->m_achFileName))
    {
        return ERROR_FILE_PATH;
    }

    s8 achRootDir[MEDIAVOD_MAX_PATHFILENAME];
    memset(achRootDir, 0, sizeof(achRootDir));

    wRet = GetAbsolutenessPath(ptVodPlayReq->m_achFileName, achRootDir, sizeof(achRootDir));
    if (wRet != MEDIAVOD_NO_ERROR)
    {
        return ERROR_FILE_PATH;
    }

    if (m_dwPlayID == INVALID_PALYID)
    {
        wRet = RPCreatePlay(&m_dwPlayID);
        if (wRet != RP_OK)
        {
            VodLog(LOG_EXP, "RPCreatePlay error, Error:%u\n", wRet);
            return ERROR_CREATE_PLAYER;
        }
    }

    
    TMediaInfo tMediaInfo;
    wRet = RPGetFileMediaInfo(m_dwPlayID, achRootDir,&tMediaInfo);
    if (wRet != RP_OK)
    {
        VodLog(LOG_EXP, "RPGetFileMediaInfo error, Error:%u\n", wRet);
        return ERROR_GET_FILEINFO;
    }


    TLocalPTParam tPTParam;

    for( u32 dw = 0; dw < MAX_STREAM_NUM; dw++ )
    {
        tPTParam.m_abyLocalPt[dw] = tMediaInfo.m_abyMediaType[dw];       
    }
    
    if (RPSetLocalPT(m_dwPlayID,&tPTParam ) != RP_OK)
    {
        return ERROR_SET_LOCALPT;
    }

    m_tPlayInfo.m_byAudioType = tMediaInfo.m_abyMediaType[0];
    m_tPlayInfo.m_byVideo1Type = tMediaInfo.m_abyMediaType[1];
    m_tPlayInfo.m_byVideo2Type = tMediaInfo.m_abyMediaType[2];
    
	u8 byVideoNum = 0;
	if ( IsVideoType(m_tPlayInfo.m_byVideo1Type) )
	{
		byVideoNum ++;
	}
	if ( IsVideoType(m_tPlayInfo.m_byVideo2Type) )
	{
		byVideoNum ++;
	}
    TRPNetSndParam tNetSndParam;
	if( 2 == byVideoNum ||
        // zgc [06/26/2007] 单放 第二路码流文件 支持
        IsVideoType(m_tPlayInfo.m_byVideo2Type) )
    {
		tNetSndParam.m_byNum = 3;
	}
	else if( IsVideoType(m_tPlayInfo.m_byVideo1Type) )
	{
		tNetSndParam.m_byNum = 2;
	}
	else
	{
		VodLog(LOG_EXP, "Video stream num is 0!\n");
		return ERROR_MEDIAVOD_PARAM;
	}
	
	// 远端地址用OSP取节点IP，防止对端过路由, zgc, 2008-03-20
	u32 dwRemoteIp = OspNodeIpGet(m_tClientInfo.m_dwNodeId);

	//tNetSndParam.m_atRemoteAddr[0].m_dwIp  = ptVodPlayReq->GetAddrIP();
	tNetSndParam.m_atRemoteAddr[0].m_dwIp = dwRemoteIp;
    tNetSndParam.m_atRemoteAddr[0].m_wPort = ptVodPlayReq->GetAudioPort();
    //tNetSndParam.m_atRemoteAddr[1].m_dwIp  = ptVodPlayReq->GetAddrIP();
	tNetSndParam.m_atRemoteAddr[1].m_dwIp = dwRemoteIp;
    tNetSndParam.m_atRemoteAddr[1].m_wPort = ptVodPlayReq->GetVideo1Port();

    tNetSndParam.m_atLocalAddr[0].m_dwIp  = g_tVodConfig.m_dwLocalIP;
    tNetSndParam.m_atLocalAddr[0].m_wPort = g_tVodConfig.m_wSendPort + 10 * GetInsID();
    tNetSndParam.m_atLocalAddr[1].m_dwIp  = g_tVodConfig.m_dwLocalIP;
    tNetSndParam.m_atLocalAddr[1].m_wPort = g_tVodConfig.m_wSendPort + 10 * GetInsID() + 4;
	
	if( 2 == byVideoNum ||
        // zgc [06/26/2007] 单放 第二路码流文件 支持
        IsVideoType(m_tPlayInfo.m_byVideo2Type) )
	{
		//tNetSndParam.m_atRemoteAddr[2].m_dwIp  = ptVodPlayReq->GetAddrIP();
		tNetSndParam.m_atRemoteAddr[2].m_dwIp = dwRemoteIp;
        tNetSndParam.m_atRemoteAddr[2].m_wPort = ptVodPlayReq->GetVideo2Port();

        tNetSndParam.m_atLocalAddr[2].m_dwIp  = g_tVodConfig.m_dwLocalIP;
        tNetSndParam.m_atLocalAddr[2].m_wPort = g_tVodConfig.m_wSendPort + 10 * GetInsID() + 8;
	}
	else
	{
		tNetSndParam.m_atRemoteAddr[2].m_dwIp  = 0;
        tNetSndParam.m_atRemoteAddr[2].m_wPort = 0;

        tNetSndParam.m_atLocalAddr[2].m_dwIp  = 0;
        tNetSndParam.m_atLocalAddr[2].m_wPort = 0;
	}
   
    u8 achFileName[50];
    memset(achFileName, 0, sizeof(achFileName));
    u32 dwPathSize = sizeof(achRootDir);
    while (achRootDir[dwPathSize-1] != '\\')
    {
        dwPathSize--;
    }
    u32 dwN = 0;
    while(achRootDir[dwPathSize])
    {
        achFileName[dwN] = achRootDir[dwPathSize];
        achRootDir[dwPathSize] = '\0';
        dwN++;
		if (dwPathSize < sizeof(achRootDir)-1)
		{
			dwPathSize++;   
		}
    }

    TPlayParam tPlayParam;
    memset(&tPlayParam, 0, sizeof(tPlayParam));
    tPlayParam.m_tNetSndParam = tNetSndParam;
    strcpy(tPlayParam.m_achPlayFileName,(const char *)achFileName);
    strcpy(tPlayParam.m_achPlayFilePath,(const char *)achRootDir);
    tPlayParam.m_bPlyOnlyIFrm = FALSE;
    tPlayParam.m_bSmallFile = FALSE;
    tPlayParam.m_dwStartTime = 0;
    tPlayParam.m_dwStopTime = 0;
    tPlayParam.m_byRateGran = 5;
    tPlayParam.m_pCallBack = NotifyPlayRateCB;


    VodLog(LOG_DEBUG, "Inst:%d PlayID=%u PlayFilePath=\"%s\", PlayFileName=\"%s\"\n",
           GetInsID(), m_dwPlayID, tPlayParam.m_achPlayFilePath, tPlayParam.m_achPlayFileName); 

    // 把'\'换成'/'
    s8* p = tPlayParam.m_achPlayFilePath;
    while(*p != 0)
    {
        if ('\\' == *p) *p = '/';
        p++;
    }
  
    static u16 st_wCheckValue = 0;
    st_wCheckValue++;
    m_wPlayCheckValue = st_wCheckValue;
    tPlayParam.m_dwContext = ((m_wPlayCheckValue) << 8) + GetInsID(); 
    wRet = RPStartPlay( m_dwPlayID , &tPlayParam );
    if (wRet != RP_OK)
    {
        return wRet;
    }

    return MEDIAVOD_NO_ERROR;
}

u16 Ckdvmediavodinstance::StopPlay()
{

    RPStopPlay(m_dwPlayID);
 
	return MEDIAVOD_NO_ERROR;
}

u16 Ckdvmediavodinstance::PausePlay()
{
    RPPausePlay(m_dwPlayID);
	return MEDIAVOD_NO_ERROR;
}

u16 Ckdvmediavodinstance::StartAgain()
{
    RPResumePlay(m_dwPlayID);
	return MEDIAVOD_NO_ERROR;
}

u16 Ckdvmediavodinstance::AppiontPlay(TVodPlayGotoReq* ptVodPlayGotoReq)
{
    u32 dwTime = 0;
    dwTime = ptVodPlayGotoReq->GetPlayTime();
    
    RPDragDropPlay(m_dwPlayID,dwTime);          
	return MEDIAVOD_NO_ERROR;
}

u16 Ckdvmediavodinstance::PlayInfo(TVodPlayInfoAck* tVodPlayInfoAck)
{
    TPlayerStatis tPlayerStatis;
    u16 wRet = RPGetPlayerStatis(m_dwPlayID, &tPlayerStatis );
    if (wRet == RP_OK)
    {
        tVodPlayInfoAck->SetPlayTime(tPlayerStatis.m_dwPlyTime);
        tVodPlayInfoAck->SetDuration(tPlayerStatis.m_dwTotalPlyTime);
        
        return MEDIAVOD_NO_ERROR;
    }
    else
    {
        return wRet;
    }

}

//读文件进度回调函数
void NotifyPlayRateCB(u32 dwPlayerID, u32 dwCurrentPlayTime, u32 dwContext)
{
    TPlayerStatis tPlayerStatis;
    u16 wRet = RPGetPlayerStatis(dwPlayerID, &tPlayerStatis);
    if (wRet != RP_OK)
    {
        return;
    }

    TPlayRateMsg tPlayRateMsg;
    u16 wInstId = (u16)(dwContext & 0xff);

    tPlayRateMsg.wCheckValue = (u16)((dwContext >> 8) & 0xff);
    tPlayRateMsg.byRate = (u8)(100 * tPlayerStatis.m_dwPlyTime / tPlayerStatis.m_dwTotalPlyTime);

//     if (tPlayRateMsg.byRate > 90)
//     {
//         s32 tmp = 0;
//     }
    OspPost(MAKEIID(MEDIAVOD_SERVER_ID, wInstId), MEDIAVOD_PLAYINFO_NTF,
            &tPlayRateMsg, sizeof(tPlayRateMsg));
    
    return;	
}

API void serverstate()
{
    OspPost(MAKEIID(MEDIAVOD_SERVER_ID,CInstance::DAEMON),MEDIAVOD_SERVERSTATE_REQ,NULL,0,0);
}



