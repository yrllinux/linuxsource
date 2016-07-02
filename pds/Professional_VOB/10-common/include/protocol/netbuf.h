

/*****************************************************************************
模块名      : NetBuf
文件名      : NetBuf.h
相关文件    : NetBuf.cpp
文件实现功能: CNetBuf Class Define
作者        : 万春雷
版本        : V2.0  Copyright(C) 2003-2005 KDC, All rights reserved.
-----------------------------------------------------------------------------
修改记录:
日  期      版本        修改人      修改内容
2004/03/26  3.0         万春雷      Create
******************************************************************************/


#ifndef _NETBUF_0601_H_
#define _NETBUF_0601_H_



//模块版本信息和编译时间 及 依赖的库的版本和编译时间
API void netbufver();


//模块帮助信息 及 依赖的库的帮助信息
API void netbufhelp();



#define  NB_NO_ERROR             (u16)0		//NetBuf库操作成功 
#define  NBERR_BASE_CODE         (u16)22000	//错误码基准值

//没有初始化nb库
#define  NBERR_NOT_INIT_LIB      (u16)(NBERR_BASE_CODE + 1)
//没有释放nb库
#define  NBERR_NOT_RELEASE_LIB   (u16)(NBERR_BASE_CODE + 2)
//内存分配错误
#define  NBERR_OUTOFMEMORY       (u16)(NBERR_BASE_CODE + 3)

//NetBuf对象没有调用CreateNetBuf()
#define  NBERR_NB_NOCREATE       (u16)(NBERR_BASE_CODE+11) 
//CSrcLinker对象未创建
#define  NBERR_SL_NOCREATE       (u16)(NBERR_BASE_CODE+12) 
//CDstLinker对象未创建
#define  NBERR_DL_NOCREATE       (u16)(NBERR_BASE_CODE+13) 

#define  NBERR_LOOP_BUF_PARAM    (u16)(NBERR_BASE_CODE+21) //设置环状缓冲参数出错
#define  NBERR_LOOP_BUF_NULL     (u16)(NBERR_BASE_CODE+22) //环状缓冲的有效数据空
#define  NBERR_LOOP_BUF_FULL     (u16)(NBERR_BASE_CODE+23) //环状缓冲的有效数据满 
#define  NBERR_LOOP_BUF_NOCREATE (u16)(NBERR_BASE_CODE+24) //环状缓冲对象没有创建
#define  NBERR_LOOP_BUF_SIZE     (u16)(NBERR_BASE_CODE+25) //环状缓冲中的数据单元有效长度出错
#define  NBERR_LOOP_BUF_MEMORY   (u16)(NBERR_BASE_CODE+26) //环状缓冲中的内存操作出错


#define  NBERR_ADD_CHANNEL_EXIST  (u16)(NBERR_BASE_CODE+31) //指定增加的接收通道已存在
#define  NBERR_ADD_CHANNEL_FULL   (u16)(NBERR_BASE_CODE+32) //接收通道已满
#define  NBERR_ADD_CHANNEL_MEMORY (u16)(NBERR_BASE_CODE+33) //接收通道对象内存分配失败
#define  NBERR_REMOVE_CHANNEL     (u16)(NBERR_BASE_CODE+34) //移除一路接收通道 操作失败


#define  ERROR_SL_RTP_CREATE_SOCK  (u16)(NBERR_BASE_CODE+41) //发送源的通讯器 创建rtp socket 操作失败
#define  ERROR_SL_RTCP_CREATE_SOCK (u16)(NBERR_BASE_CODE+42) //发送源的通讯器 创建rtcp socket 操作失败
#define  ERROR_DL_RTP_CREATE_SOCK  (u16)(NBERR_BASE_CODE+43) //接收源的通讯器 创建rtp socket 操作失败
#define  ERROR_DL_RTCP_CREATE_SOCK (u16)(NBERR_BASE_CODE+44) //接收源的通讯器 创建rtcp socket 操作失败


#define  NBERR_WSA_STARTUP       (u16)(NBERR_BASE_CODE+100) //wsastartup error
#define  NBERR_CREATE_SEMAPORE   (u16)(NBERR_BASE_CODE+101) //create semapore error
#define  NBERR_SOCKET_CALL       (u16)(NBERR_BASE_CODE+102) //调用socket() 函数出错
#define  NBERR_BIND_SOCKET       (u16)(NBERR_BASE_CODE+103) //socket 绑定出错
#define  NBERR_CREATE_THREAD     (u16)(NBERR_BASE_CODE+104) //创建线程出错 

//----------------------------------------------------------------------

//网络参数

typedef struct tagNetAddr
{
	u32   m_dwIP;		//本地ip  (网络序)
	u16   m_wPort;		//本地port(本机序)
}TNetAddr;

typedef struct tagNetLocalAddr
{
	TNetAddr    m_tLocalAddr;
	TNetAddr    m_tRtcpBackAddr;	//远端RTCP回馈信息的接收地址
}TNetLocalAddr;

typedef struct tagNetFeedbackAddr
{
	TNetAddr    m_tLocalAddr;
	TNetAddr    m_tRtpRemoteAddr;	//远端RTP包的接收地址
}TNetFeedbackAddr;


typedef struct tagRSParam
{
    u16  m_wFirstTimeSpan;	 //第一个重传检测点(ms)
	u16  m_wSecondTimeSpan;  //第二个重传检测点(ms)
	u16  m_wThirdTimeSpan;   //第三个重传检测点(ms)
	u16  m_wRejectTimeSpan;  //过期丢弃的时间跨度(ms)
} TRSParam;

/*=============================================================================
函数名		nbInit
功能		初始化 网络重发缓存 库,重载之一， 外部使用
算法实现	：（可选项）
输入参数说明：
返回值说明： 成功返回FE_OK,  失败返回错误码
=============================================================================*/
u16 nbInit();


/*=============================================================================
函数名		nbRelease
功能		反化 网络重发缓存 库,减少引用参数计数,直至释放库，重载之一， 外部使用
算法实现	：（可选项）
输入参数说明：无
返回值说明： 成功返回FE_OK,  失败返回错误码
=============================================================================*/
u16 nbRelease();


//----------------------------------------------------------------------

class CLinkerObj;

class CNetBuf
{
public:
    CNetBuf();
    ~CNetBuf();

public:
	//打开
	u16 CreateNetBuf( TRSParam tRSParam, u16 wSndPort );
	//关闭
	u16 FreeNetBuf();
	
	//设置与发送端相互交互的接收地址，可重复设置，设置后新地址取代旧地址, 传递NULL则清除底层套节子
	//用于接收、缓存发送端的rtp包，检测并反馈rtcp重传请求包
	u16 SetLocalAddr( TNetLocalAddr *ptLocalAddr );

	//设置与接收端相互交互的接收地址，可重复设置，设置后新地址取代旧地址, 传递NULL则清除底层套节子
	//用于接收接收端的rtcp重传请求包，并根据请求发出相应rtp包
	u16 SetChannelLocalAddr( TNetAddr *ptChannelLocalAddr );

private:
	CLinkerObj  *m_pcLinkerObj;
	void        *m_hSynSem;     //用于对象的单线程操作的同步量
};

//----------------------------------------------------------------------


#endif /////__NETBUF_0601_H_
