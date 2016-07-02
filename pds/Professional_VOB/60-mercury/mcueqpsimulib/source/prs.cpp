#include "kdvtype.h"
#include "osp.h"
#include "netbuf.h"

//模块版本信息和编译时间 及 依赖的库的版本和编译时间
void netbufver()
{
}


//模块帮助信息 及 依赖的库的帮助信息
void netbufhelp()
{
}


/*=============================================================================
函数名		nbInit
功能		初始化 网络重发缓存 库,重载之一， 外部使用
算法实现	：（可选项）
输入参数说明：
返回值说明： 成功返回FE_OK,  失败返回错误码
=============================================================================*/
u16 nbInit()
{
	return NB_NO_ERROR;
}


/*=============================================================================
函数名		nbRelease
功能		反化 网络重发缓存 库,减少引用参数计数,直至释放库，重载之一， 外部使用
算法实现	：（可选项）
输入参数说明：无
返回值说明： 成功返回FE_OK,  失败返回错误码
=============================================================================*/
u16 nbRelease()
{
	return NB_NO_ERROR;
}

class CLinkerObj
{

};

CNetBuf::CNetBuf()
{
}
CNetBuf::~CNetBuf()
{
	if (m_pcLinkerObj)
	{
		delete m_pcLinkerObj;
		m_pcLinkerObj = NULL;
	}

	if (m_hSynSem)
	{
		delete m_hSynSem;
		m_hSynSem = NULL;
	}
}

	//打开
u16 CNetBuf::CreateNetBuf( TRSParam tRSParam, u16 wSndPort )
{
	return NB_NO_ERROR;
}
	//关闭
u16 CNetBuf::FreeNetBuf()
{
	return NB_NO_ERROR;
}
	//设置与发送端相互交互的接收地址，可重复设置，设置后新地址取代旧地址, 传递NULL则清除底层套节子
	//用于接收、缓存发送端的rtp包，检测并反馈rtcp重传请求包
u16 CNetBuf::SetLocalAddr( TNetLocalAddr *ptLocalAddr )
{
	return NB_NO_ERROR;
}
	//设置与接收端相互交互的接收地址，可重复设置，设置后新地址取代旧地址, 传递NULL则清除底层套节子
	//用于接收接收端的rtcp重传请求包，并根据请求发出相应rtp包
u16 CNetBuf::SetChannelLocalAddr( TNetAddr *ptChannelLocalAddr )
{
	return NB_NO_ERROR;
}

