#ifndef _MCUCASCADECOMM_H_
#define _MCUCASCADECOMM_H_

//#include "string.h"
//#include "stdlib.h"
#include "osp.h"
#include "stdio.h"
#include "kdvtype.h"
#include "xmlcodec.h"
#include "tcpsocket.h"
#include "h323mcuconn.h"



#define XML_NODE_NUM			(s32)1024
#define XML_NODE_SIZE			(s32)128
#define MAX_CASCADES_COUNT		(s32)64
#define MAX_XML_BUFFER_LEN		(MAX_TCPMSG_LEN)//最大的编解码长度.
#define MAX_XML_DEAL_LEN        (MAX_XML_BUFFER_LEN*2)//
#define MAX_PACKNUM_PERNETPACK  (s32)32

#define WHITESPACE        (s8)0x20


#define BOOL_STR(b) (b?"True":"False")//
#define SUCC_STR(b) (b?"OK":"DENY")//
//日志级别定义
#define MMCU_EXP			0		//异常
#define MMCU_IMT			1		//重要日志
#define MMCU_DEBUG			2		//一级调试信息
#define MMCU_DEBUG2			3		//二级调试信息
#define MMCU_ALL			4       //所有调试信息 
#define MMCU_FILE           255     //打文件

s32 g_nloglevel = 0;

//  [pengguofeng 7/5/2013]          02.15
#define VER_MMCULIB (s8 *)"40.10.01.02.15.130708"

#define RAD_USER	((s8*)"rvmcu")
#define RAD_PASS	((s8*)"shoss")

#define ISMMCULOG(n)  (n <= g_nloglevel) 
#define CLOSE_NODE(node) {if(node != INVALID_NODE){TcpDisconnect(node); node=INVALID_NODE;}}

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

void mmculog(s32 nLevel, s8 * format,...)
{
    
    if(!ISMMCULOG(nLevel)) return;
    
    s8 szBuf[512] = {0};
    va_list arg;
    va_start(arg,format);
    vsprintf(szBuf,format,arg);
    OspPrintf(TRUE, FALSE, szBuf);
    va_end(arg);
}

 s8 cPrefix = ' ';
void PrintTree(CXMLTree *pcTree, int nNode, int nPrefixNum)
{
    TXMLNode *pNode = pcTree->GetNode(nNode);
    BOOL bHasChild = (pNode->m_nChildId !=-1 ) ? TRUE : FALSE; 
    
    for(s32 i=0;i<nPrefixNum;i++)
    {
        OspPrintf(TRUE, FALSE, "%c", cPrefix);
    }
    OspPrintf(TRUE, FALSE, "%s", pNode->m_pszNodeText);
    if(pNode->m_pszNodeValue != NULL)
    {
        OspPrintf(TRUE, FALSE, "%s%s", ":  ", pNode->m_pszNodeValue);
    }
    OspPrintf(TRUE, FALSE, "\n");
    
    
    if(bHasChild) PrintTree(pcTree, pNode->m_nChildId, nPrefixNum+2);
    
    if(pNode->m_nBrotherId != -1 )
    {		
        PrintTree(pcTree, pNode->m_nBrotherId, nPrefixNum);
    }
}

API void PrintXMLTree(void* pSession, s32 nLevel, CXMLTree *pcTree, const s8 *pszLabel)
{
    if (MMCU_DEBUG2  == g_nloglevel)
    {
        s8 szTitle[XML_NODE_SIZE]={0};
	    pcTree->GetType(szTitle, sizeof(szTitle));
        OspPrintf(TRUE, FALSE, "Session:0x%x %s: %s\n", pSession, pszLabel, szTitle);
    }

    if(!ISMMCULOG(nLevel)) return;
    OspPrintf(TRUE, FALSE, "Session:0x%x %s\n", pSession, pszLabel);
    PrintTree(pcTree, pcTree->GetRootId(), 2);
}

#define CMP_LEN 64
////////////////////////////////
#define NullStrCmpLimited(src1, src2) \
((src1 && src2)? strncmp(src1, src2, CMP_LEN) : 1)
#define NullStrCmpNLimited(src1, src2, nLen) \
((src1 && src2)? strncmp(src1, src2, nLen) : 1)
    
class CSemLock
{
public:
	CSemLock(SEMHANDLE* phSem)
	{
		m_phSem = NULL;
		if(phSem != NULL&&*phSem != NULL)
		{
			OspSemTake(*phSem);
			m_phSem = phSem;
		}
	}
	~CSemLock()
	{
		if(m_phSem != NULL)
		{
			OspSemGive(*m_phSem);
		}
	}
private:
	SEMHANDLE* m_phSem;
};

typedef struct
{	
	s32 m_nType;
	s8  m_szName[XML_NODE_SIZE];
}TTypeMap;

static const TTypeMap g_atMsgMap[]=
{
    {
		H_CASCADE_NONSTANDARD_REQ,
		"cascade_nonstandard_request"
    },
    {
		H_CASCADE_NONSTANDARD_RSP,
		"cascade_nonstandard_response"
    },
	{
		H_CASCADE_NONSTANDARD_NTF,
	    "cascade_nonstandard_notification"
	},
	{
         H_CASCADE_REGUNREG_REQ,
		"regunreg_neighbor_request"
    },
    { 
		 H_CASCADE_REGUNREG_RSP,
		"regunreg_neighbor_response"
	},
	{
		H_CASCADE_NEWROSTER_NTF,
		"new_neigbor_roster_notification"
	},
	{ 
		H_CASCADE_NEWPART_NTF,
		"new_participant_notification"
	},
	{	H_CASCADE_CALL_ALERTING_NTF,
		"call_alerting_notification"
	},
	{ 
		H_CASCADE_PARTLIST_REQ,
		"get_participant_list_request"
	},
	{ 
		H_CASCADE_PARTLIST_RSP,
		"get_participant_list_response"
	},
	{ 
		H_CASCADE_VIDEOINFO_REQ,
		"get_conference_video_info_request"
	},
	{ 
		H_CASCADE_VIDEOINFO_RSP,
		"get_conference_video_info_response"
	},
	{ 
		H_CASCADE_AUDIOINFO_REQ,
		"get_conference_audio_info_request"
	},
	{
		H_CASCADE_AUDIOINFO_RSP,
		"get_conference_audio_info_response"
	},
	{ 
		H_CASCADE_OUTPUT_NTF,
		"participant_output_layout_set_notification"
	},
	{
		H_CASCADE_NEWSPEAKER_NTF,
		"new_speaker_notification"
	},
	{ 
		H_CASCADE_SETIN_REQ,
		"participant_set_in_layout_request"
	},
	{ 
		H_CASCADE_SETIN_RSP,
		"participant_set_in_layout_response"
	},
	{ 
		H_CASCADE_INVITEPART_REQ,
		"invite_participant_request"
	},
	{ 
		H_CASCADE_INVITEPART_RSP,
		"invite_participant_response"
	},
	{ 
		H_CASCADE_REINVITEPART_REQ,
		"reinvite_participant_request"
	},
	{ 
		H_CASCADE_REINVITEPART_RSP,
		"reinvite_participant_response"
	},
	{ 
		H_CASCADE_DELETEPART_REQ,
		"delete_part_request"
	},
    { 
        H_CASCADE_DISCONNPART_REQ,
        "drop_participant_request"
    },

	{ 
		H_CASCADE_DELETEPART_RSP,
		"delete_part_response"
	},
    { 
        H_CASCADE_DISCONNPART_RSP,
        "drop_participant_response"
    },
    { 
		H_CASCADE_DELETEPART_NTF,
		"participant_deleted_notification"
	},
    { 
         H_CASCADE_DISCONNPART_REQ,
         "disconnect_part_request"
    },
    { 
         H_CASCADE_DISCONNPART_RSP,
         "disconnect_part_response"
    },
    { 
         H_CASCADE_DISCONNPART_NTF,
         "participant_disconnected_notification"
    },
    { 
         H_CASCADE_DISCONNPART_NTF,
         "participant_dropped_notification"
    },
	{ 
		 H_CASCADE_PARTMEDIACHAN_REQ,
		 "part_media_chan_operation_request"
	},
    { 
         H_CASCADE_PARTMEDIACHAN_RSP,
         "part_media_chan_operation_response"
    },
    { 
         H_CASCADE_PARTMEDIACHAN_NTF,
         "part_media_chan_operation_notification"
    },
    { 
         H_CASCADE_PARTMEDIACHAN_REQ,
         "participant_media_muted_unmuted_request"
    },
    { 
        H_CASCADE_PARTMEDIACHAN_RSP,
        "participant_media_muted_unmuted_response"
    },
    { 
        H_CASCADE_PARTMEDIACHAN_NTF,
        "participant_media_muted_unmuted_notification"
    },
    { 
        H_CASCADE_CONFVIEW_CHG_NTF,
        "conference_layout_changed_notification"
    },
    { 
        H_CASCADE_SETOUT_REQ,
        "participant_set_out_layout_request"
    },
    { 
        H_CASCADE_SETOUT_RSP,
        "participant_set_out_layout_response"
    },
    {
        H_CASCADE_VA_REQ,
        "va_set_in_layout_request"
    },
    {
        H_CASCADE_VA_RSP,       
        "va_set_in_layout_response"
    },
    {
         H_CASCADE_RELEASECONTROL_OF_CONF_REQ,
         "release_conference_control_request"
    },
    {
         H_CASCADE_RELEASECONTROL_OF_CONF_RSP,
         "release_conference_control_response"
    },
    {
         H_CASCADE_TAKECONTROL_OF_CONF_REQ,
         "take_conference_control_request"
    },
    {
         H_CASCADE_TAKECONTROL_OF_CONF_RSP,
         "take_conference_control_response"
    },
         
    {
         H_CASCADE_ALLPART_SETOUT_REQ,
         "all_participant_set_out_layout_request"
    },
    {
         H_CASCADE_ALLPART_SETOUT_RSP,
         "all_participant_set_out_layout_response"
    },
    {
         H_CASCADE_CREATE_CONF_REQ,
         "create_conference_request"
    },
    {
         H_CASCADE_CREATE_CONF_RSP,
         "create_conference_response"
    },
    {
         H_CASCADE_TERMINATE_CONF_REQ,
         "terminate_conference_request"
    },
	
    {
         H_CASCADE_TERMINATE_CONF_RSP,
         "terminate_conference_response"
    },
/////////////////////////
    {
         H_CASCADE_GET_CONFPROFILE_REQ,
         "get_conference_info_request"
    },

    {
         H_CASCADE_GET_CONFPROFILE_RSP,
         "get_conference_info_response"
    },
	
	{
		 H_CASCADE_GETMCUSERVICELIST_REQ,
         "get_services_request"		 
	},

	{
		 H_CASCADE_GETMCUSERVICELIST_RSP,
         "get_services_response"		 
	},

	{
		 H_CASCADE_GETMUCCONFLIST_REQ,
         "get_conference_list_request"
	},

	{
    	 H_CASCADE_GETMUCCONFLIST_RSP,
         "get_conference_list_response"
	},

	{
		 H_CASCADE_GETCONFGIDBYNAME_REQ,
         "get_confgid_by_name_request"
	},

	{
		 H_CASCADE_GETCONFGIDBYNAME_RSP,
        "get_confgid_by_name_response"
	},

	{
		 H_CASCADE_SET_LAYOUT_AUTOSWITCH_REQUEST,
		 "set_layout_autoswitch_request"
	},
	{
		 H_CASCADE_SET_LAYOUT_AUTOSWITCH_RESPONSE,
		 "set_layout_autoswitch_response"
	},
};

enum emXMLMsgType
{
    emRequest = 0,
    emResponse = 1,
    emNotification =2,
    emInvalid = -1
};
static const s8* const g_MsgType_LabelStr[] =
{
    "request",
    "response",
    "notification"
};

emXMLMsgType  GetMsgType(const s8 *pszLable)
{
    if(pszLable==NULL)
    {
        return emInvalid;
    }
    const s8 *pszStartPos = pszLable;
    const s8 *pszEndPos = pszLable + strlen(pszLable) - 1;
    while(*pszStartPos==' ' && (pszStartPos < pszLable + strlen(pszLable)))pszStartPos++;
    while(*pszEndPos == ' ' && pszEndPos >= pszLable)pszEndPos --;
    if((pszStartPos - pszEndPos)>= 0)
    {
        return emInvalid;
    }
    s32 nLen = strlen(g_MsgType_LabelStr[emRequest]);
    if(strncmp((pszEndPos - nLen + 1), g_MsgType_LabelStr[emRequest], nLen) == 0)
    {
        return emRequest;
    }

    nLen = strlen(g_MsgType_LabelStr[emResponse]);
    if(strncmp((pszEndPos - nLen + 1), g_MsgType_LabelStr[emResponse], nLen) == 0)
    {
        return emResponse;
    }
    
    nLen = strlen(g_MsgType_LabelStr[emNotification]);
    if(strncmp((pszEndPos - nLen + 1), g_MsgType_LabelStr[emNotification], nLen) == 0)
    {
        return emNotification;
    }
    return emInvalid;
}

emXMLMsgType  GetMsgTypeFromLable(const s8 *pszLable)
{
    if(pszLable==NULL)
    {
        return emInvalid;
    }
    if(strstr(pszLable, g_MsgType_LabelStr[emRequest]) != NULL)
    {
        return emRequest;
    }
    if(strstr(pszLable, g_MsgType_LabelStr[emResponse]) != NULL)
    {
        return emResponse;
    }
    if(strstr(pszLable, g_MsgType_LabelStr[emNotification]) != NULL)
    {
        return emNotification;
    }
        return emInvalid;
}


BOOL32 ConvertMsgLable(const s8 * pszSrcLable, s8 * pszDstLable, s32 nDstLen, emXMLMsgType emFrom, emXMLMsgType emTo)
{
    if(pszSrcLable==NULL || pszDstLable==NULL ||nDstLen<= 0)
    {
        return FALSE;
    }
/*
    const s8 *pszStartPos = pszSrcLable;
    const s8 *pszEndPos = pszSrcLable + strlen(pszSrcLable) - 1;
    while(*pszStartPos==' ' && (pszStartPos < pszSrcLable + strlen(pszSrcLable)))pszStartPos++;
    while(*pszEndPos == ' ' && pszEndPos >= pszSrcLable)pszEndPos --;
    if(pszStartPos> = pszEndPos)
    {
        return FALSE;
    }
    *(pszEndPos + 1) = NULL;

    s32 nLen = strlen(g_MsgType_LabelStr[emFrom]);
    if(strncmp((pszEndPos - nLen + 1), g_MsgType_LabelStr[emFrom], nLen) != 0)
    {
        return FALSE;
    }
    
    nLen = strlen(g_MsgType_LabelStr[emTo]);
    if(nDstLen <= nLen + (pszEndPos -pszStartPos))
    {
        return FALSE;
    }*/
    const s8 *pszEndPos   = NULL;
    
    pszEndPos = strstr(pszSrcLable,g_MsgType_LabelStr[emFrom]);
    if(pszEndPos == NULL)
    {
        return FALSE;
    }

    if((pszEndPos - pszSrcLable) + strlen(g_MsgType_LabelStr[emTo]) >= (u32)nDstLen)
    {
        return FALSE;
    }
    
    strncpy(pszDstLable, pszSrcLable, pszEndPos - pszSrcLable);
    *(pszDstLable + (pszEndPos - pszSrcLable)) = '\0'; 
    strcat(pszDstLable, g_MsgType_LabelStr[emTo]);
    return TRUE;
}

s32  GetMsgIdFromName(const s8 *pszName)
{
	s32 nNum = sizeof(g_atMsgMap)/sizeof(g_atMsgMap[0]);
	s32 index = 0;
    if(pszName == NULL)
    {
        return CASCADE_MSGTYPE_BEGINNING;
    }
	for(; index<nNum; index++)
	{
		if(NullStrCmpLimited(g_atMsgMap[index].m_szName, pszName) == 0)
		{
			break;
		}
	}
	if(index >= nNum)
	{
		if(GetMsgTypeFromLable(pszName) == emRequest)
        {
            return H_CASCADE_UNDEFINED_REQ;
        }
        else if(GetMsgTypeFromLable(pszName) == emResponse)
        {
            return H_CASCADE_UNDEFINED_RSP;
        }
        else if(GetMsgTypeFromLable(pszName) == emNotification)
        {
            return H_CASCADE_UNDEFINED_NTF;
        }
        return CASCADE_MSGTYPE_BEGINNING;
	}

	return g_atMsgMap[index].m_nType;
}

emReturnValue GetReturnValFromStr(const s8 *pszValStr)
{
    if(pszValStr==NULL)
    {
        return emReturnValue_Invalid;
    }
    
    while(*pszValStr==' ')pszValStr++;
    
    if(NullStrCmpNLimited(pszValStr,"ok", strlen("ok"))==0)
    {
        return emReturnValue_Ok;
    }
    
    if(NullStrCmpNLimited(pszValStr,"partnotfound", strlen("partnotfound"))==0)
    {
        return emReturnValue_PartNotFound;
    }
    
    if(NullStrCmpNLimited(pszValStr,"confnotfound", strlen("confnotfound"))==0)
    {
         return emReturnValue_ConfNotFound;
    }

    if(NullStrCmpNLimited(pszValStr,"channelnotfound", strlen("channelnotfound"))==0)
    {
        return emReturnValue_ChannelNotFound;
    }
    
    if(NullStrCmpNLimited(pszValStr,"nopermission", strlen("nopermission"))==0)
    {
        return emReturnValue_NoPermission;
    }
    if(NullStrCmpNLimited(pszValStr,"noresources", strlen("noresources"))==0)
    {
        return emReturnValue_NoResources;
    }
    if(NullStrCmpNLimited(pszValStr,"timeout", strlen("timeout"))==0)
    {
        return emReturnValue_TimeOut;
    }
    
    if(NullStrCmpNLimited(pszValStr,"xmlbadparams", strlen("xmlbadparams"))==0)
    {
        return emReturnValue_XmlBadParams;
    }

    if(NullStrCmpNLimited(pszValStr,"error", strlen("error"))==0)
    {
        return emReturnValue_Error;
    }

    return  emReturnValue_Invalid;
}

s8 * ReturnValToStr(emReturnValue emVal)
{
    switch(emVal)
    {
    case emReturnValue_Ok:
        {
            return "OK";
            break;
        }
    case emReturnValue_PartNotFound:
        {
            return "PartNotFound";
            break;
        }
    case emReturnValue_ConfNotFound:
        {
            return "ConfNotFound";
            break;
        }

    case emReturnValue_ChannelNotFound:
    {
        return "ChannelNotFound";
        break;
    }
    case emReturnValue_NoPermission:
        {
            return "NoPermission";
            break;
        }
    case emReturnValue_NoResources:
        {
            return "NoResources";
            break;
        }
    case emReturnValue_TimeOut:
        {
            return "TimeOut";
            break;
        }
    case emReturnValue_XmlBadParams:
        {
            return "XmlBadParams";
            break;
        }
    case emReturnValue_Error:
        {
            return "Error";
            break;
        }
    default: break;
    }
    return "Error";
}

s32 AddXMLHead(CXMLTree *pcTree,  const s8 *pszType,  const s8 *pszName=(s8 *)NULL,  const s8 *pszPass=(s8 *)NULL)
{
	TXMLNode *pNode = NULL;
	TXMLNode tNode;
	tNode.m_nChildId = -1;
	tNode.m_nParentId = -1;
	s8 szHead[24]={0};
        sprintf(szHead, "%s%c%s%c%s", "?xml version=",34,"1.0",34,"?");//?xml version="1.0"?;
	tNode.m_pszNodeText = szHead;
	tNode.m_pszNodeValue = NULL;
	s32 nNode = pcTree->AddNode(tNode);
	if(nNode == -1)
	{
		return -1;
	}
	pNode = pcTree->GetNode(nNode);

	//mcu_xml_api
	tNode.m_nParentId = nNode;
	tNode.m_pszNodeText = "MCU_XML_API";
	nNode = pcTree->AddNode(tNode);
	if(nNode == -1)
	{
		return -1;
	}
	pNode = pcTree->GetNode(nNode);
	
	s32 nParentNode = nNode;
		//version
		tNode.m_nParentId = nParentNode;
		tNode.m_pszNodeText = "Version";
		tNode.m_pszNodeValue = "Ver 3.0";
		nNode = pcTree->AddNode(tNode);
		if(nNode == -1)
		{
			return -1;
		}
		pNode->m_nChildId = nNode;
		pNode = pcTree->GetNode(nNode);

		if(NullStrCmpLimited("Request", pszType) == 0)
		{
			//Account
			tNode.m_nParentId = nParentNode;
			tNode.m_pszNodeText = "Account";
			if(pszName != NULL&& *pszName != 0)
				tNode.m_pszNodeValue = pszName;
			else
				tNode.m_pszNodeValue = "rvmcu";
			nNode = pcTree->AddNode(tNode);
			if(nNode == -1)
			{
				return -1;
			}
			pNode->m_nBrotherId = nNode;
			pNode = pcTree->GetNode(nNode);

			//Password
			tNode.m_nParentId = nParentNode;
			tNode.m_pszNodeText = "Password";

			if(pszName == NULL|| *pszName == 0)
			{
				tNode.m_pszNodeValue = "shoss";
			}
			else
			{
				if(pszPass != NULL&& *pszPass != 0)
					tNode.m_pszNodeValue = pszPass;
				else if(pszPass == NULL||*pszPass == 0)
				{
					tNode.m_pszNodeValue = "NULL";
				}
				else
					tNode.m_pszNodeValue = "shoss";
			}

			nNode = pcTree->AddNode(tNode);
			if(nNode == -1)
			{
				return -1;
			}
			pNode->m_nBrotherId = nNode;
			pNode = pcTree->GetNode(nNode);
		}
		//type
		tNode.m_nParentId = nParentNode;
		tNode.m_pszNodeText = pszType;
		tNode.m_pszNodeValue = NULL;
		nNode = pcTree->AddNode(tNode);
		if(nNode == -1)
		{
			return -1;
		}
		pNode->m_nBrotherId = nNode;
   return nNode;      
}

s32 AddXMLChild(CXMLTree *pcTree, s32 nParentNode,  const s8 *pszType,   const s8 *pszValue)
{
	TXMLNode tNode;
	TXMLNode *pNode = pcTree->GetNode(nParentNode);
	if(pNode == NULL)
	{
		return -1;
	}

	tNode.m_nParentId = nParentNode;
	tNode.m_pszNodeText = pszType;
	tNode.m_pszNodeValue = pszValue;
	s32 nNode = pcTree->AddNode(tNode);
	if(nNode == -1)
	{
		return -1;
	}
	pNode->m_nChildId = nNode;
	return nNode;
}

s32 AddXMLBrother(CXMLTree *pcTree, 
				  s32 nParentNode, 
				  s32  nBrotherNode,
				  const s8 *pszType,  
				  const s8 *pszValue)
{
	TXMLNode tNode;
	TXMLNode *pNode = pcTree->GetNode(nBrotherNode);
	if(pNode == NULL)
	{
		return -1;
	}
	
	tNode.m_nParentId = nParentNode;
	tNode.m_pszNodeText = pszType;
	tNode.m_pszNodeValue = pszValue;
	s32 nNode = pcTree->AddNode(tNode);
	if(nNode == -1)
	{
		return -1;
	}
	pNode->m_nBrotherId = nNode;
	return nNode;
}

s8 *OctToStr2(const u8 *pBuf, s32 nLen)
{
	static s8 aszStr[XML_NODE_SIZE+4];
	if(nLen > XML_NODE_SIZE/2)
	{
		return NULL;
	}
	for(s32 i=0; i<nLen; i++)
	{
	    sprintf(aszStr+2*i, "%02x", pBuf[i]);
	}
	aszStr[nLen*2] = 0;
	return aszStr;
}

s8 *OctToStr2(const u8 *pBuf, s32 nLen, s8 *pszBuf, s32 nStrLen)
{
	s32 nRealLen = min(nLen, nStrLen/2);
	if(nRealLen <= 0)
	{
		return NULL;
	}
	for(s32 i=0; i<nRealLen; i++)
	{
		sprintf(pszBuf+2*i, "%02x", pBuf[i]);
	}
	pszBuf[nLen*2] = 0;
	return pszBuf;	
}

s8 *ConnStateToStr(emConnState emState)
{
	switch(emState)
	{
	case emConnStateConnected:
		{
			return "Connected";
			break;
		}
	case emConnStateConnecting:
		{
			return "Connecting";
			break;
		}
	case emConnStateDisconnected:
		{
			return "Disconnected";
			break;
		}
	default: break;
	}
	return "Unknown";
}

s8 *NumToStr(s32 nNum)
{
	static s8 aszStr[16];
	memset(aszStr, 0, sizeof(aszStr));
        sprintf(aszStr, "%d", nNum);
	return aszStr;
}

s32 StrToNum(const s8 *pszStr)
{
    if(pszStr)
    {
        return atoi(pszStr);
    }
    else
    {
       return 0;
    }
}

s8 *IPToStr(u32 dwIP)
{
	if(dwIP == 0)
	{
		return "NULL";
	}
	static s8 aszStr[16];
	sprintf(aszStr, "%d.%d.%d.%d", ((u8 *)&dwIP)[0], 
								((u8 *)&dwIP)[1], 
								((u8 *)&dwIP)[2],
								((u8 *)&dwIP)[3]);
	return aszStr;
}


s8 *MediaTypeToStr(emMediaType emType)
{

        
    switch(emType)
    {
    case emMediaTypeVideo:
        {
            return "Video";
            break;
        }
    case emMediaTypeAudio:
        {
            return "Audio";
            break;
        }
    case emMediaTypeData:
        {
            return "Data";
        }
    default: break;
    }
    return "NA";
}
        
s8 *MuteDirectToStr(emMuteDirection emDirect)
{
    switch(emDirect)
    {
    case emDirectionIn:
        {
            return "In";
            break;
        }
    case emDirectionOut:
        {
            return "Out";
            break;
        }
    default: break;
    }
    return "NA";
}

emMediaType StrToMediaType(const s8 *pszTypeStr)
{
    if(pszTypeStr==NULL)
    {
        return emMediaTypeNone;
    }
    
    while(*pszTypeStr==' ')pszTypeStr++;
    
    if(NullStrCmpNLimited(pszTypeStr,"video",strlen("video"))==0)
    {
        return emMediaTypeVideo;
    }
    
    if(NullStrCmpNLimited(pszTypeStr,"audio",strlen("audio"))==0)
    {
        return emMediaTypeAudio;
    }

    if(NullStrCmpNLimited(pszTypeStr,"data",strlen("data"))==0)
    {
        return emMediaTypeData;
    }
   return  emMediaTypeNone;
}

emMuteDirection StrToMuteDirect(const s8 * pszDirectStr)
{
    if(pszDirectStr==NULL)
    {
        return emDirectionNone;
    }
    
    while(*pszDirectStr==' ')pszDirectStr++;
    
    if(NullStrCmpNLimited(pszDirectStr,"in",strlen("in"))==0)
    {
        return emDirectionIn;
    }
    
    if(NullStrCmpNLimited(pszDirectStr,"out",strlen("out"))==0)
    {
        return emDirectionOut;
    }
    return emDirectionNone;
}

s8 *NumToGeometryName(u32 dwGeometryNum)
{
    switch(dwGeometryNum)
    {
    case 10:
        return "SingleScreen";
    	break;
    case 20:
        return "DoubleScreen";
        break;
    case 30:
        return "TripleScreen";
        break;
    case 40:
        return "2x2";
        break;
    default:break;
    }
    return "PicInPic";
}

typedef struct 
{
    u8 m_byTopLeftX;
    u8 m_byTopLeftY;
    u8 m_byBottomRightX;
    u8 m_byBottomRightY;
    u8 m_byZOrder;
}TGeometryPos;

static const TGeometryPos g_tGeometryList[MAX_SUBFRAMES_PER_GEOMETRY][MAX_SUBFRAMES_PER_GEOMETRY]=
{
    {{0,0,100,100,0}},
    {{0,0,100,50,0},{0,50,100,100}},
    {{0,0,100,50,0},{0,50,50,100},{50,50,100,100}},
    {{0,0,25,25,0},{25,25,50,50},{50,50,75,75},{75,75,100,100}},
    {{0}},
};

s8 *EPTypeToStr(emEPType emType)
{
	switch(emType)
	{
	case emEPTypeMCU:
		{
			return "MCU";
			break;
		}
	case emEPTypeTerminal:
		{
			return "Terminal";
			break;
		}
	case emEPTypeCascConf:
		{
			return "CascConference";
		}
	default: break;
	}
	return "UnDefined";
}

s8 *PTypeToStr(PayloadType emType)
{
	switch(emType)
	{
	case a_g711a: return "G711A"; break;
	case a_g711u: return "G711U"; break;
	case a_g722:  return "G722";  break;
	case a_g7231: return "G7231"; break;
	case a_g728:  return "G728";  break;
	case a_g729:  return "G729";  break;
	case a_mp3:   return "Mp3";   break;
	case a_g723:  return "G723";  break;
	case a_g7221: return "G7221"; break;
    case a_mpegaaclc: return "AAC-LC"; break;
    case a_mpegaacld: return "AAC-LD"; break;
	case v_h261:  return "H261";  break;
	case v_h263:  return "H263";  break;
	case v_h263plus: return "H263+"; break;
	case v_h264:     return "H264";	 break;
	case v_mpeg4:    return "Mpeg4"; break;
	case v_h262:	 return "H262"; break;
	case t_none:     return "None";  break;
	default: break;
	}
	return "Unknown";
}

s8 *TimeToStr(TTimeSpec &tTime)
{
	static s8 aszStr[32];
	sprintf(aszStr, "%02d:%02d:%02d %02d-%02d", 
		             tTime.hour, 
					 tTime.minute,
					 tTime.second,
					 tTime.day,
					 tTime.month);
	return aszStr;
	
}

s8 *ResToStr(emResolution emRes)
{
	switch(emRes)
	{
	case emSQCIF: return "Sub-QCIF";	break;
	case emQCIF:  return "QCIF";		break;
	case emCIF:   return "CIF";			break;
	case emCIF2:  return "2CIF";		break;
	case emCIF4:  return "4CIF";		break; 
	case emCIF16: return "16CIF";		break;
	case em352240: return "352*240";	break;
	case em704480: return "704*480";	break;
	case em640480: return "VGA";		break;
	case em800600: return "SVGA";		break;
	case em1024768:return "XGA";		break;
	case emAUTO:   return "AUTO";		break;
    case emW4CIF:  return "W4CIF";      break;
    case emHD720:  return "720p";       break;
    case emSXGA:   return "SXGA";       break;
    case emUXGA:   return "UXGA";       break;
	case emHD1080: return "1080";		break;
//     case emHD1080p:return "1080p";      break;
//     case emHD1080i:return "1080i";      break;
	default:break;
	}
	return "Unknown";
}

/////////////////////////////////////
void StrToTime(TTimeSpec &tTime, const s8 *pchSrc)
{
    //03:01:40 01-01
    if( pchSrc == NULL)
    {
        return ;
    }
    
    while(*pchSrc == WHITESPACE)pchSrc++;

    tTime.hour   = (s8)((pchSrc[0]-'0')*10+(pchSrc[1]-'0'));
    tTime.minute = (s8)((pchSrc[3]-'0')*10+(pchSrc[4]-'0'));
    tTime.second = (s8)((pchSrc[6]-'0')*10+(pchSrc[7]-'0'));
    tTime.day    = (s8)((pchSrc[9]-'0')*10+(pchSrc[10]-'0'));
    tTime.month  = (s8)((pchSrc[12]-'0')*10+(pchSrc[13]-'0'));

    return ;
}

//network byte sequence,
u32 StrToIP(const s8 *pchIpSrc)
{
	if(pchIpSrc == NULL)
    {
        return 0;
    }
    return inet_addr(pchIpSrc);
}

/////////////////////

inline s32 C2V(s8 c)
{
	s32 v=0;
	if(c>='a'&&c<='f') v = (c-'a')+10;
	else if(c>='A'&&c<='F') v = (c-'A')+10;
	else if(c>='0'&&c<='9') v = c-'0'; 
	return v;
}

s32 Str2ToOct(const s8 *pchSrc2, u8 *pbyDst, s32 nLen)
{
    s8 byValue1, byValue2;
   
	if(pbyDst==NULL || pchSrc2==NULL)
    {
        return 0;
    }
    s32 nReal = 0;
    while(*pchSrc2)
    {
        byValue1= C2V(*(pchSrc2++)) * 16;
        byValue2= C2V(*(pchSrc2++));
		*pbyDst++ = byValue1 + byValue2; 
		nReal++;
		if(nReal >= nLen) break;
    }
    return nReal;
}

//Adding by JunY
emConnState StrToConnState(const s8 *pConnStr)
{
    if(pConnStr==NULL)
    {
        return emConnStateUnknown;
    }

    while(*pConnStr==' ')pConnStr++;
 
    if(NullStrCmpNLimited(pConnStr,"connected",strlen("connected"))==0)
    {
       return emConnStateConnected;
    }
    
    if(NullStrCmpNLimited(pConnStr,"alerting",strlen("alerting"))==0)
    {
        return emConnStateConnecting;
    }

    if(NullStrCmpNLimited(pConnStr,"disconnected",strlen("disconnected"))==0)
    {
        return emConnStateDisconnected;
    }

    return emConnStateUnknown;
}

emEPType StrToEPType(const s8 *pEPTypeStr)
{
    if(pEPTypeStr==NULL)
    {
        return emEPTypeUndefined;
    }
    
    while(*pEPTypeStr==WHITESPACE) pEPTypeStr++;
    
    if(NullStrCmpLimited(pEPTypeStr,"mcu")==0)
    {
        return emEPTypeMCU;
    }
    
    if(NullStrCmpLimited(pEPTypeStr,"terminal")==0)
    {
        return emEPTypeTerminal;
    }
	if(NullStrCmpLimited(pEPTypeStr, "cascconference")==0)
	{
		return emEPTypeCascConf;
	}
    if(NullStrCmpLimited(pEPTypeStr, "undefined")==0)
    {
        return emEPTypeUndefined;
    }
    
    return emEPTypeUndefined;
}

typedef struct 
{
     s8    *pEnumStr;
    s32     nEnumVal;
}TEnumTable;

PayloadType StrToPType(const s8 * pPayloadStr)
{
    u32 dwIndex=0;
    TEnumTable pTable[]=
    {
        {"g711a", a_g711a}, {"g711u", a_g711u},   {"g722", a_g722},
        {"g7231", a_g7231}, {"g728", a_g728},     {"g729", a_g729},
        {"mp3", a_mp3},     {"g723", a_g723},     {"g7221", a_g7221},
        {"aac-lc", a_mpegaaclc},     {"aac-ld", a_mpegaacld},  
        {"h261", v_h261},   {"h263", v_h263},     {"h263+", v_h263plus},
        {"h264", v_h264},   {"mpeg4", v_mpeg4},   {"h262", v_h262},
		{"none", t_none},   {NULL, t_none}
    };

    if(pPayloadStr==NULL)
    {
        return t_none;
    }
    
    while(*pPayloadStr==WHITESPACE ) pPayloadStr++;

    while(pTable[dwIndex].pEnumStr != NULL && 
		NullStrCmpLimited(pPayloadStr, pTable[dwIndex].pEnumStr)!=0)
    {
        dwIndex++;
    }

    return (PayloadType)( pTable[dwIndex].nEnumVal);
}

emResolution StrToPicS(const s8 * pPicSStr)
{
    u32 dwIndex=0;
    TEnumTable pTable[]=
    {
        {"SQCIF",emSQCIF},     {"QCIF",emQCIF},      {"CIF",emCIF},
        {"CIF2", emCIF2},       {"CIF4",emCIF4},      {"CIF16",emCIF16},
        {"352240", em352240},   {"704480",em704480},  {"VGA",em640480},
        {"SVGA",em800600},   {"XVGA",em1024768},       {"AUTO",emAUTO},
        {"W4CIF",emW4CIF},   
		{"720p",emHD720},
		//{"720p",emHD720p},
		{"SXGA",emSXGA},
        {"UXGA",emUXGA}, 
		//{"1080p",emHD1080p},       {"1080i",emHD1080i},               
        {"1080", emHD1080},
		{NULL, emAUTO}
    };

    if(pPicSStr==NULL)
    {
        return emAUTO;
    }
    
    while(*pPicSStr == WHITESPACE ) pPicSStr++;
    
    while(pTable[dwIndex].pEnumStr !=NULL && 
		NullStrCmpLimited(pPicSStr,pTable[dwIndex].pEnumStr) !=0 )
    {
        dwIndex++;
    }
    
    return (emResolution)(pTable[dwIndex].nEnumVal);
}

BOOL32 FindTag(u8 *pBuf, s32 nLen, s32* pnPos, s8 *pszTag)
{
	//<?xml
	*pnPos = 0; 
	s32 nTagLen = strlen(pszTag);
	while(nLen>=nTagLen)
	{
		if(memcmp(pBuf, pszTag , nTagLen)==0)
		{
			return TRUE;			
		}
		(*pnPos)++;pBuf++;nLen--;
	}
	return FALSE;
}
///Adding by JunY
/////////////////////////////////////////////////
#endif



