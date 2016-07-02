#ifndef _XMLCODEC_041029_H_
#define _XMLCODEC_041029_H_

#include "kdvtype.h"

#ifndef NULL
#define NULL 0
#endif

#define IN
#define OUT

typedef int b32;

typedef struct tagTXMLParam
{
	s32 m_nMaxNodeNum;
	s32 m_nMaxNodeSize;
	b32 m_bEncode;
}TXMLParam;

typedef struct tagXMLNode
{
	s32   m_nMeId;        //-1 for AddNode
	s32   m_nChildId;     //-1 for AddNode
	s32   m_nParentId;	  
	s32   m_nBrotherId;
	const s8	 *m_pszNodeText;
    const s8     *m_pszNodeValue; //NULL for pure node 
	tagXMLNode()
	{
		m_nMeId		    = -1;
		m_nChildId		= -1;
		m_nParentId		= -1;
		m_nBrotherId	= -1;
		m_pszNodeText	= NULL;
		m_pszNodeValue  = NULL;
	}
}TXMLNode;

class CMemBlock
{
public:
	CMemBlock();
	~CMemBlock();
public:
	b32  	Alloc(u32 dwLen);
	void	Close();
	u8*		GetBuf(u32 dwLen);
	void	Reset();
private:
	u8		*m_pbyBuf;
	u32		dwTotleLen;
	u32		dwCurPos;
};

class CXMLTree
{
public:
	CXMLTree();
	virtual ~CXMLTree();
public:
	b32	 Create( s32 nNodeNum, s32 nNodeSize );
    void Close();
	void Reset();
	
	void       GetType(s8 *pszType, s32 nSize);   
	s32        AddNode(TXMLNode &tXMLNode);
	s32		   GetRootId();
	s32		   GetChildId( s32 nId );
	s32		   GetBrotherId( s32 nId );
	s32        FindNode(s32 nParentId, const s8 *pszNodeName);
	TXMLNode*  GetNode( s32 nId );
protected:
	CMemBlock *m_pcMem;
	u8        *m_pbyBuf;
	s32        m_nNodeNum;
	s32        m_nNodeSize;
	s32        m_nCurPos;
};

class CXMLEnc
{
public:
    static b32 Encode( CXMLTree IN *pcXMLTree, u8 IN OUT *pbyBuf, 
					   s32 IN nLen, s32 OUT *pnLen);
};

class CXMLCodecTree;
class CXMLDec
{
public:
	CXMLDec();
	~CXMLDec();
public:
	b32  Create( s32 IN nNodeNum, s32 IN nNodeSize);
	void Close();
    b32  Decode( u8 IN *pbyBuf, s32 IN nLen, 
			     CXMLTree IN OUT **ppcXMLTree);
private:
    CXMLCodecTree *m_pcXMLCodecTree;
};

typedef u8  *pu8;

class CXMLCodecTree: public CXMLTree
{
public:
	b32  SetBuff(u8 *pbyBuf, s32 nLen);
	b32  GetBuff(u8 *pbyBuf, s32 *pnLen);
private:
	inline b32 FindNode( u8 *pSrcBuf, s32 nSrcLen, 
		u8 **ppDstBuf, s32 &pnDstLen, 
		b32 &bBegin ); 
	inline b32 SetNode( u8 *pSrcBuf, s32 nSrcLen, 
		s32  &nId,   u8 **ppSrcPos );
	inline s32 GetXMLNode(pu8 &pBuf, s32 nLen, s32 nNode);
};


#endif//!_XMLCODEC_040715_H_

