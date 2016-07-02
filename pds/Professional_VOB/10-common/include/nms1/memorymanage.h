// MemoryManage.h: interface for the CMemoryManage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYMANAGE_H__7C6E73D3_62C0_46F7_B3B9_C195F48AC87E__INCLUDED_)
#define AFX_MEMORYMANAGE_H__7C6E73D3_62C0_46F7_B3B9_C195F48AC87E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



#include "safeadapter.h"

/*
环形内存示意图
|																															 |
|--------------------------------------------------  总内存  ----------------------------------------------------------------|
|																															 |

|------------||------------||------------||------------||------------||------------||------------||------------||------------|

  TMemBlock1     TMemBlock2   TMemBlock3     TMemBlock4   TMemBlock5    TMemBlock6    TMemBlock7     TMemBlock8   TMemBlock9

  
修改纪录:
日      期  版本    修改人  修改内容
2010/12/17  4.0     沈  钦    创建
*/



//tagMemBlock单向环形列表的节点
typedef struct tagMemBlock
{
public:

	tagMemBlock			*pNext;					//下块MemBlock的位置
	char				*pBlock;				//这块MemBlock所指的真实内存的起始位置
	int					nDataSize;				//块内真实数据的长度
	int					nOccupyBlockSize;		//这次数据要占几个块（大数据的要占几个块） 
												//占一块的情况      该值为1
												//占几个块的情况    第一块该值为所占的块数 其余块的该值的都为-1
												//该内存没用时值为0	
	tagMemBlock()
	{
		Init();
	}

	void Init()
	{
		pBlock = NULL;
		nDataSize = 0;
		nOccupyBlockSize = 0;
		pNext = NULL;
	}

	//操作该TMemBlock数据
	//decreaseblocknum 减少的空闲块数   OccupyBlockSize占用几个内存块  pdata要存放的真实数据  datasize数据大小
	char* SetData( int OccupyBlockSize, char* pdata , int datasize )
	{

		nDataSize = datasize;
		nOccupyBlockSize = OccupyBlockSize;
		memcpy( pBlock, pdata, datasize );
		
		return pBlock;
	}

	//操作该TMemBlock数据
	//psource 获取数据的指针   OccupyBlockSize占用几个内存块  datasize  从这个Block中拷贝出的数据
	void GetData(char *psource, int &OccupyBlockSize, int &datasize)
	{
		OccupyBlockSize = nOccupyBlockSize;
		datasize   = nDataSize;

		memcpy( psource, pBlock, nDataSize );
		nDataSize = 0;
		nOccupyBlockSize = 0;


	}

}TMemBlock, *PTMemBlock;




typedef struct tagBlocksManageBorder
{
	char* startpos;
	char* endpos;
	
	tagBlocksManageBorder()
	{
		Init();
	}
	
	void Init()
	{
		startpos = 0;
		endpos = 0;
	}
}TBlockBorder, *PTBlockBorder;


#define ErrNum(num)		num,
#define ErrDesc(str)	
#define _ITEMEND	


enum EmMemoryError
{
	ErrNum( emMemNoERROR )
	ErrDesc("")
	_ITEMEND
	
	
	ErrNum(emMemErrNewError)
	ErrDesc("new时出错")
	_ITEMEND
	
	
	ErrNum(emMemErrNotEnoughError)
	ErrDesc("没有足够的内存")
	_ITEMEND


	ErrNum(emMemErrNotFound)
	ErrDesc("没有找到内存")
	_ITEMEND

	ErrNum(emMemErrReIniting)
	ErrDesc("正在重新分配内存")
	_ITEMEND		
};



#define DEFAULTBLOCKSIZE	(int) 5000
#define DEFAULTBLOCKNUM		(int) 1000

class CMemoryManage //: public CTAapater<CMemoryManage>
{
public:

	/*=============================================================================
	函 数 名:  InputData
	功    能:  放数据到内存
	参    数:  [in]pdata 数据指针   [in]len 要存入内容的大小     [pout]  要存储的数据在 拷贝后的首地址
	注    意:无
	返 回 值:无
	-------------------------------------------------------------------------------
	修改纪录:
	日      期  版本    修改人  修改内容
	2010/12/17  4.0     沈  钦    创建
		=============================================================================*/
	EmMemoryError	InputData(char *pdata, int len, char **pout);

	/*=============================================================================
	函 数 名:  GettData
	功    能:  获得数据
	参    数:  [in]pdata 要查找的内容指针    [out]pout 输出内容的指针 [out]len输出长度
	注    意:无
	返 回 值:无
	-------------------------------------------------------------------------------
	修改纪录:
	日      期  版本    修改人  修改内容
	2010/12/17  4.0     沈  钦    创建
		=============================================================================*/
	EmMemoryError	GetData(char *pdata, char *pout, int &len);

	/*=============================================================================
	函 数 名:  InitMemoryManage
	功    能:  初始化内存
	参    数:  [in]blocksize 块大小   [in]blocknum   块的数目
	注    意:无
	返 回 值:无
	-------------------------------------------------------------------------------
	修改纪录:
	日      期  版本    修改人  修改内容
	2010/12/17  4.0     沈  钦    创建
		=============================================================================*/
	EmMemoryError	InitMemoryManage(int blocksize = DEFAULTBLOCKSIZE , int blocknum = DEFAULTBLOCKNUM);

	/*=============================================================================
	函 数 名:  ResetVal
	功    能:  重置初始化值
	参    数:  
	注    意:无
	返 回 值:无
	-------------------------------------------------------------------------------
	修改纪录:
	日      期  版本    修改人  修改内容
	2010/12/17  4.0     沈  钦    创建
		=============================================================================*/
	void			ResetVal();

	/*=============================================================================
	函 数 名:  GetDropApplyTimes
	功    能:  获得申请内存失败的次数
	参    数:  
	注    意:无
	返 回 值:无
	-------------------------------------------------------------------------------
	修改纪录:
	日      期  版本    修改人  修改内容
	2010/12/17  4.0     沈  钦    创建
	=============================================================================*/
	DWORD			GetDropApplyTimes() { return m_dwDropApplyTimes ;}

	/*=============================================================================
	函 数 名:  ClearAllMemory
	功    能:  删除所有内存
	参    数:  
	注    意:无
	返 回 值:无
	-------------------------------------------------------------------------------
	修改纪录:
	日      期  版本    修改人  修改内容
	2010/12/17  4.0     沈  钦    创建
	=============================================================================*/
	void			ClearAllMemory();

	/*=============================================================================
	函 数 名:  ReInit
	功    能:  重新分配内存
	参    数:  
	注    意:无
	返 回 值:无
	-------------------------------------------------------------------------------
	修改纪录:
	日      期  版本    修改人  修改内容
	2010/12/17  4.0     沈  钦    创建
	=============================================================================*/
	EmMemoryError	ReInit( int blocksize = DEFAULTBLOCKSIZE, int blocknum = DEFAULTBLOCKNUM );


	/*=============================================================================
	函 数 名:  CheckMemory
	功    能:  检查内存是否可以分配
	参    数:  [in]len 为总长度    [out]numblock需要的内存块
	注    意:无
	返 回 值:无
	-------------------------------------------------------------------------------
	修改纪录:
	日      期  版本    修改人  修改内容
	2010/12/17  4.0     沈  钦    创建
	=============================================================================*/
	EmMemoryError	CheckMemory(int len, int &numblock)
	{
		return CheckMemorySize( len,  numblock);
	}

	/*=============================================================================
	函 数 名:  GetBlockBorder
	功    能:  获得内存第一块的起始地址 最后一块的起始地址
	参    数:  [out]tBorder  位置
	注    意:无
	返 回 值:无
	-------------------------------------------------------------------------------
	修改纪录:
	日      期  版本    修改人  修改内容
	2010/12/17  4.0     沈  钦    创建
	=============================================================================*/
	void			GetBlockBorder(TBlockBorder &tBorder)
	{
		int	nCount = m_nMemoryTotalSize / m_nBlockSize;	
		tBorder.startpos = m_pMemStart;
		tBorder.endpos = m_pMemStart + m_nBlockSize *(nCount - 1);

	}

	void GetInfo(int &TotalMemory, int &BlockSize, int &ApplyDropTimes  )
	{
		TotalMemory = m_nMemoryTotalSize;
		BlockSize = m_nBlockSize;
		ApplyDropTimes = m_dwDropApplyTimes;
	}

	void GetManageInfo(int &NowFreeBlock, int &MaxUsingBlocks)
	{
		NowFreeBlock = m_nFreeBlock;
		MaxUsingBlocks = m_nMaxUsingBlocks;
	}

	CMemoryManage();
	virtual ~CMemoryManage();

protected:
	PTMemBlock		FindPos( char* point );
	EmMemoryError	CheckMemorySize(int len, int &numblock);	
	char* SetData(PTMemBlock pBlock, int OccupyBlockSize, char* pdata , int datasize )
	{
		m_nFreeBlock--;
		return pBlock->SetData( OccupyBlockSize,  pdata , datasize );
	}

	void GetData(PTMemBlock pBlock, char *psource, int &OccupyBlockSize, int &datasize)
	{
		m_nFreeBlock++;
		pBlock->GetData( psource, OccupyBlockSize, datasize);
	}
private:


	char		* m_pMemStart;			//环形缓存的头指针（永不改变）
	PTMemBlock	m_pMemBlockStart;		//指向TMemBlock的第一个指针（永不改变）
	PTMemBlock	m_pCurrentBlock;		//当前可以存放的数据的TMemBlock指针
	int			m_nMemoryTotalSize;		//开辟的所有内存大小
	int			m_nBlockSize;			//每个Block的大小


	DWORD		m_dwDropApplyTimes;		//没有申请成功的次数
	int			m_nMaxUsingBlocks;		//使用最高峰时 用了几块Block

	BOOL		m_bReInting;			//正在重新分配内存
	
	
	int			m_nFreeBlock;			//当前内存中有几个块可以使用
};



//typedef  CTAapater< CMemoryManage >  CSafeCMemManager;


#endif // !defined(AFX_MEMORYMANAGE_H__7C6E73D3_62C0_46F7_B3B9_C195F48AC87E__INCLUDED_)
