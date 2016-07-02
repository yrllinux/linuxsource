#ifndef _XMLCODEC_040715_H_
#define _XMLCODEC_040715_H_
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
	s8	 *m_pszNodeText;
    s8   *m_pszNodeValue; //NULL for pure node 
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
	~CXMLTree();
public:
	b32	 Create( s32 nNodeNum, s32 nNodeSize );
    void Close();
	void Reset();
   
	s32        AddNode(TXMLNode &tXMLNode);
	s32		   GetRootId();
	s32		   GetChildId( s32 nId );
	s32		   GetBrotherId( s32 nId );
	TXMLNode*  GetNode( s32 nId );
protected:
	CMemBlock *m_pcMem;
	u8        *m_pbyBuf;
	s32       m_nNodeNum;
	s32       m_nNodeSize;
	s32       m_nCurPos;
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

#endif//!_XMLCODEC_040715_H_