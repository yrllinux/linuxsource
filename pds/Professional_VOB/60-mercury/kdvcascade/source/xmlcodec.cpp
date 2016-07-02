
//lint -save -sem(CXMLDec::Close,cleanup)
//lint -save -sem(CMemBlock::Close,cleanup)
//lint -save -sem(CXMLTree::Close,cleanup)
/*lint -save -e613*/
/*lint -save -e662*/

#include "osp.h"
//#include "string.h"
#include "xmlcodec.h"
#ifdef NULL
#undef NULL
#endif
#define NULL 0

#ifndef WIN32
static char *strlwr(s8 *pszStr)
{
    char *p = pszStr;
    if(p == NULL)
    {
        return NULL;
    }

    while (*p)
    {
        if(*p>='A'&&*p<='Z')
            *p += 'a'-'A';
        p++;
    }
    return pszStr;

}

#endif

b32 CXMLEnc::Encode( CXMLTree IN *pcXMLTree, u8 IN OUT *pbyBuf, 
					s32 IN nLen, s32 OUT *pnLen)
{
	if( pcXMLTree == NULL|| pbyBuf == NULL|| 
		nLen == 0|| pnLen == NULL) return FALSE;
	*pnLen = nLen;
	return ((CXMLCodecTree *)pcXMLTree)->GetBuff(pbyBuf, pnLen);
}

CXMLDec::CXMLDec()
{
	m_pcXMLCodecTree = NULL;
}


CXMLDec::~CXMLDec()
{
	try
	{
		Close();
	}
	catch (...)
	{
	}
}

b32 CXMLDec::Create(s32 nNodeNum, s32 nNodeSize)
{
	if(nNodeNum <= 0|| nNodeSize <= 0) return FALSE;
	
	Close();
    
	m_pcXMLCodecTree = new CXMLCodecTree;
	if(m_pcXMLCodecTree == NULL) 
	{
		return FALSE;
	}
	return m_pcXMLCodecTree->Create(nNodeNum, nNodeSize);
	
}

void CXMLDec::Close()
{
	if(m_pcXMLCodecTree != NULL)
	{
		m_pcXMLCodecTree->Close();
		delete m_pcXMLCodecTree;
		m_pcXMLCodecTree = NULL;
	}
}

b32 CXMLDec::Decode(u8 IN *pbyBuf, s32 IN nLen, 
					CXMLTree IN OUT **ppcXMLTree)
{
    if(m_pcXMLCodecTree == NULL) return FALSE;
	if(!m_pcXMLCodecTree->SetBuff(pbyBuf, nLen))
	{
		return FALSE;
	}
	if(ppcXMLTree != NULL) *ppcXMLTree = m_pcXMLCodecTree;
    return TRUE;
}



//memory block class
CMemBlock::CMemBlock()
{
	m_pbyBuf = NULL;
	dwTotleLen = 0;
	dwCurPos   = 0;
}

CMemBlock::~CMemBlock()
{
	try
	{
		Close();
	}
	catch (...)
	{
	}
}

b32 CMemBlock::Alloc(u32 dwLen)
{
	Close();
	m_pbyBuf = new u8[dwLen];
	if(m_pbyBuf == NULL) return FALSE;
	dwTotleLen = dwLen;
	return TRUE;
}

void CMemBlock::Close()
{
	if(m_pbyBuf != NULL)
	{
		delete []m_pbyBuf;
		m_pbyBuf = NULL;
	}
	dwTotleLen = 0;
	dwCurPos   = 0;
}
	
u8 *CMemBlock::GetBuf(u32 dwLen)
{
	if(m_pbyBuf == NULL) return NULL;
    if(dwCurPos + dwLen > dwTotleLen)
	{
		return NULL;
	}
	dwCurPos += dwLen;
	return m_pbyBuf+dwCurPos-dwLen;
}
	
void CMemBlock::Reset()
{
	dwCurPos = 0;
}

//XML Tree class
CXMLTree::CXMLTree()
{
	m_pcMem	 = NULL;
	m_pbyBuf = NULL;
	m_nNodeNum  = 0;
    m_nNodeSize = 0;
	m_nCurPos	= 0;
}

CXMLTree::~CXMLTree()
{
	try
	{
		Close();
	}
	catch (...)
	{
	}
}

b32 CXMLTree::Create( s32 nNodeNum, s32 nNodeSize )
{
	if(nNodeSize <= 0|| nNodeNum <= 0) return FALSE;

	Close();
	
	m_pcMem = new CMemBlock;
	if(m_pcMem == NULL) return FALSE;
   
	m_pbyBuf = new u8[(u32)nNodeNum * sizeof(TXMLNode)];
	if(m_pbyBuf == NULL) 
	{
		Close();
		return FALSE;
	}

	if(!m_pcMem->Alloc(nNodeNum * nNodeSize))
	{
		Close();
		return FALSE;
	}
	m_nNodeNum = nNodeNum;
	m_nNodeSize = nNodeSize;
    
	return TRUE;
}

void CXMLTree::Close()
{
	if(m_pcMem != NULL)
	{
		delete m_pcMem;
		m_pcMem = NULL;
	}
	
	if(m_pbyBuf != NULL)
	{
		delete []m_pbyBuf;
		m_pbyBuf = NULL;
	}
	m_nNodeNum  = 0;
    m_nNodeSize = 0;
	m_nCurPos	= 0;
}

void CXMLTree::Reset()
{
	if(m_pcMem != NULL)
		m_pcMem->Reset();
	m_nCurPos	= 0;
}

s32 CXMLTree::GetRootId()
{
	return 0;
}
	
s32 CXMLTree::GetChildId(s32 nId)
{
	TXMLNode *p = (TXMLNode *)m_pbyBuf + nId;;
	return p->m_nChildId;
}

s32  CXMLTree::GetBrotherId(s32 nId)
{
	TXMLNode *p = (TXMLNode *)m_pbyBuf + nId;
	return p->m_nBrotherId;
}
	
TXMLNode* CXMLTree::GetNode(s32 nId)
{
	return (TXMLNode *)m_pbyBuf + nId;
}

s32 CXMLTree::AddNode(TXMLNode &tXMLNode)
{
	if(m_pcMem == NULL|| m_pbyBuf == NULL|| 
		tXMLNode.m_pszNodeText == NULL)
	{
		return -1;
	}

    if(m_nCurPos >= m_nNodeNum||
		tXMLNode.m_nBrotherId >= m_nNodeNum||
		tXMLNode.m_nParentId >= m_nNodeNum)
	{
		return -1;
	}

	TXMLNode *pNode		= (TXMLNode *)m_pbyBuf + m_nCurPos;
	pNode->m_nChildId = -1;
	pNode->m_nMeId    = m_nCurPos;
	pNode->m_nBrotherId = -1; //下一个兄弟
	pNode->m_nParentId = tXMLNode.m_nParentId;
	pNode->m_pszNodeText  = NULL;
	pNode->m_pszNodeValue = NULL;

	if(pNode->m_nParentId != -1)
	{
	    TXMLNode *pPareNode = (TXMLNode *)m_pbyBuf + pNode->m_nParentId;
		if(pPareNode->m_nChildId == -1) //紧接着父节点的字节点
		{
			pPareNode->m_nChildId = m_nCurPos;
		}
	}
	
	if(tXMLNode.m_pszNodeValue != NULL)
	{
		s8 *pszTempNodeVal = (s8 *)(m_pcMem->GetBuf(strlen(tXMLNode.m_pszNodeValue)+1));
		if( pszTempNodeVal == NULL) 
        { 
            pNode->m_pszNodeValue = NULL;
            return -1;
        }
		strcpy(pszTempNodeVal, tXMLNode.m_pszNodeValue);
        pNode->m_pszNodeValue = pszTempNodeVal;
	}
    
	s8 *pszTempNodeText = (s8 *)(m_pcMem->GetBuf(strlen(tXMLNode.m_pszNodeText)+1));
	if(pszTempNodeText == NULL)
	{
        pNode->m_pszNodeText = NULL;
        return -1;
	}
	strcpy(pszTempNodeText, tXMLNode.m_pszNodeText);
    pNode->m_pszNodeText = pszTempNodeText;
	return m_nCurPos++;
}
void CXMLTree::GetType(s8 *pszType, s32 nSize)
{
	*pszType = 0; 
	s32 nNode = GetRootId(); //<xml>
	if(nNode == -1)
	{
		return;
	}
	nNode = GetChildId(nNode); //<MCU_XML_API>
	if(nNode == -1)
	{
		return;
	}

	nNode = GetChildId(nNode);//version
	if(nNode == -1)
	{
		return;
	}
	if(GetChildId(nNode) != -1)//have child
	{
		nNode = GetChildId(nNode);//<msg>
		if(nNode == -1)
		{
			return;
		}
		TXMLNode *pNode = GetNode(nNode);
		s32 nLen = strlen(pNode->m_pszNodeText);
		nLen  = nLen > nSize ? nSize : nLen;
		strncpy(pszType, pNode->m_pszNodeText, nLen);
		return;
						
	}

	nNode = GetBrotherId(nNode);//account
	if(nNode == -1)
	{
		return;
	}
	if(GetChildId(nNode) != -1)//have child
	{
		nNode = GetChildId(nNode);//<msg>
		if(nNode == -1)
		{
			return;
		}
		TXMLNode *pNode = GetNode(nNode);
		s32 nLen = strlen(pNode->m_pszNodeText);
		nLen  = nLen > nSize ? nSize : nLen;
		strncpy(pszType, pNode->m_pszNodeText, nLen);
		return;
	}

	nNode = GetBrotherId(nNode);//Password
	if(nNode == -1)
	{
		return;
	}
	if(GetChildId(nNode) != -1)//have child
	{
		nNode = GetChildId(nNode);//<msg>
		if(nNode == -1)
		{
			return;
		}
		TXMLNode *pNode = GetNode(nNode);
		s32 nLen = strlen(pNode->m_pszNodeText);
		nLen  = nLen > nSize ? nSize : nLen;
		strncpy(pszType, pNode->m_pszNodeText, nLen);
		return;
		
	}


	nNode = GetBrotherId(nNode);//<msg tag>
	if(nNode == -1)
	{
		return;
	}

	nNode = GetChildId(nNode);//<msg>
	if(nNode == -1)
	{
		return;
	}
	TXMLNode *pNode = GetNode(nNode);
	s32 nLen = strlen(pNode->m_pszNodeText);
	nLen  = nLen > nSize ? nSize : nLen;
	strncpy(pszType, pNode->m_pszNodeText, nLen);
	return;
}

s32 CXMLTree::FindNode(s32 nParentId, const s8 *pszNodeName)
{
	TXMLNode *pNode = GetNode(nParentId);
	BOOL32 bHasChild = (pNode->m_nChildId !=-1 ) ? TRUE : FALSE; 
	if(pNode->m_pszNodeText && pszNodeName && strcmp(pNode->m_pszNodeText, pszNodeName) == 0)
	{
		return nParentId;
	}

	if(bHasChild) 
	{
		s32 nNode = FindNode(pNode->m_nChildId, pszNodeName);
		if(nNode != -1)
		{
			return nNode;
		}
	}
		
	if(pNode->m_nBrotherId != -1 )
	{		
		s32 nNode = FindNode(pNode->m_nBrotherId, pszNodeName);
		if(nNode != -1)
		{
			return nNode;
		}
	}
	return -1;
}

//编解码类
b32 CXMLCodecTree::SetBuff(u8 *pbyBuf, s32 nLen)
{
	if(m_pcMem == NULL|| m_pbyBuf == NULL||
		pbyBuf == NULL|| nLen == 0) return FALSE;
	
	Reset();
	
	u8   *pPos;
	s32  nId = -1;
	// [9/21/2010 xliang] 无条件返回TURE，不能有效应对局域网攻击可能造成的崩溃
//	SetNode(pbyBuf, nLen, nId, &pPos);
//	return TRUE;
	return SetNode(pbyBuf, nLen, nId, &pPos);
}

b32 CXMLCodecTree::GetBuff(u8 *pbyBuf, s32 *pnLen)
{
	if(m_pcMem == NULL|| m_pbyBuf == NULL||
		pbyBuf == NULL|| *pnLen==0)
	{
		return FALSE;
	}

	s32 nIdleLen = GetXMLNode(pbyBuf, *pnLen, GetRootId()); 
	if(nIdleLen < 0) return FALSE;

	*pnLen  -= nIdleLen; 

	return TRUE;
}

s32 CXMLCodecTree::GetXMLNode(pu8 &pBuf, s32 nLen, s32 nNode)
{
	TXMLNode *pNode = GetNode(nNode);
	b32 bHasChild = (pNode->m_nChildId !=-1 ) ? TRUE : FALSE; 
	s32 nTextLen = strlen(pNode->m_pszNodeText);
	s32 nValueLen = 0;
	s32 nRetLen   = nLen - nTextLen - 2; //include"<>"
	if(nRetLen <= 0)
	{
		return -1;
	}
	
	*pBuf++ = '<';
	strcpy((s8* )pBuf,pNode->m_pszNodeText);
	pBuf +=nTextLen;
	*pBuf++ = '>';

	if(pNode->m_pszNodeValue != NULL) 
	{
		nValueLen = strlen(pNode->m_pszNodeValue);
		nRetLen   -= nValueLen;
		if(nRetLen <= 0)
		{
			return -1;
		}
		strcpy((s8 *)pBuf, pNode->m_pszNodeValue);
        
		pBuf += nValueLen;
	}

	if(bHasChild) 
	{
		nRetLen = GetXMLNode(pBuf, nRetLen, pNode->m_nChildId);
	}

	if(nNode == 0) //exclude "<?xml version="1.0"?>"
	{
		return nRetLen;
	}

	nRetLen -= (nTextLen +3);//include"</>"
	if(nRetLen < 0)
	{
		return -1;
	}
	*pBuf++ = '<'; *pBuf++ = '/';
	strcpy((s8 *)pBuf, pNode->m_pszNodeText);
	pBuf +=nTextLen;
	*pBuf++ = '>';

	if(pNode->m_nBrotherId != -1 )
	{		
        nRetLen = GetXMLNode(pBuf, nRetLen, pNode->m_nBrotherId);
	}
	
	return nRetLen;
}

b32 CXMLCodecTree::FindNode( u8 *pSrcBuf, s32 nSrcLen, 
						u8 **ppDstBuf, 
						s32 &nDstLen, b32 &bBegin)
{
	*ppDstBuf = NULL;
	nDstLen = 0;
	b32 bFindBegin = FALSE;
	while((nSrcLen--) > 0)
	{
		if(*pSrcBuf == '<')
		{
			bFindBegin = TRUE;
			bBegin = (*(pSrcBuf+1) == '/') ? FALSE : TRUE;
			*ppDstBuf = pSrcBuf;
		}
		if(*pSrcBuf == '>' && bFindBegin)
		{
			nDstLen++;
			return TRUE;
		}
		if( bFindBegin ) nDstLen++;
		pSrcBuf++;
	}
    return FALSE;
}

b32 CXMLCodecTree::SetNode( u8 *pSrcBuf, s32 nSrcLen, 
					   s32 &nId,    u8 **ppSrcPos)
{
	u8 *pDstBuf		= NULL;
	b32 bBegin		= TRUE;
	u8 *pPos		= pSrcBuf;
	s32 nNodeLen	= 0;
	s32 nCurrentId	= nId+1;
	s32 nTmpSize	= nSrcLen;
	TXMLNode *pNode	   = (TXMLNode *)m_pbyBuf + (nId + 1); //指向下一个位置
	TXMLNode *pOldNode = NULL;
	BOOL32 bLoopFlag = TRUE;
	while(bLoopFlag)
	{		
		if( !FindNode(pPos, nTmpSize, &pDstBuf, nNodeLen, bBegin) )
		{
			return FALSE;		   
		}
		if( !bBegin )
		{//node XML</>	
			*ppSrcPos= pDstBuf;
			nId = nCurrentId;
			return TRUE;		
		}
		pNode->m_nParentId	= nId;
		pNode->m_nMeId		= nCurrentId;
		if(nId >= 0 && nCurrentId == nId+1) //父节点的第一个子节点
		{
			(pNode-1)->m_nChildId = nCurrentId;
		}
		else if(nCurrentId != nId+1) //兄节点
		{
			pOldNode->m_nBrotherId = nCurrentId;
		}

		pNode->m_nChildId   = -1;
		pNode->m_nBrotherId = -1;
        s8 *pszTempNodeText = (s8 *)(m_pcMem->GetBuf(nNodeLen-1));
		memcpy(pszTempNodeText, pDstBuf+1, nNodeLen-2);
		*(pszTempNodeText + nNodeLen-2) = 0;//截成字符串
		strlwr(pszTempNodeText);
        pNode->m_pszNodeText = pszTempNodeText;

		//设置子node
		b32 bRet  = SetNode(pPos + nNodeLen, 
							nTmpSize-nNodeLen, 
							nCurrentId, &pPos);

		if(bRet == TRUE)
		{
			
			if(nCurrentId > pNode->m_nMeId+1 )
			{//纯节点
				pNode->m_pszNodeValue = NULL; 
			}
			else
			{
				//获取节点值
				s32 nLen = pPos - pDstBuf - nNodeLen;
                s8 *pszTempNodeVal = (s8 *)( m_pcMem->GetBuf(nLen+1) );
				memcpy(pszTempNodeVal, pDstBuf + nNodeLen, nLen);
				*(pszTempNodeVal + nLen) = 0;
				//后续统一处理[11/16/2012 chendaiwei]
				if( strcmp(pszTempNodeText,"partname")!=0
					&& strcmp(pszTempNodeText,"dialstr")!=0)
				{
					strlwr(pszTempNodeVal);
				}
                pNode->m_pszNodeValue = pszTempNodeVal;
			}
			
			pOldNode = pNode;//记录node

			pPos	 += (nNodeLen + 1);
			nTmpSize  = nSrcLen - (pPos - pSrcBuf) ;
			pNode     = (TXMLNode *)m_pbyBuf + nCurrentId;
		}
		else 
		{
			pNode->m_pszNodeValue = NULL;
			// [9/20/2010 xliang] 由于结点开头有<?xml version="1.0"?>，所以结点本来就没有完全对称。下面不能无条件返回FALSE 
			if( nId == -1 )
			{
				return TRUE;
			}
			return FALSE; 		
		}
	}

	return TRUE; //实际这里不会走到
}

/*lint -restore*/


