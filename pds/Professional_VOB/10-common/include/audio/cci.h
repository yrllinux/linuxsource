/**                                                                            
*** Copyright (c) 2002-2003 Keda Communication Co.LTD.
添加注释
**/     

#ifndef _CCI_H___
#define _CCI_H___



#ifdef __cplusplus
extern "C" {
#endif
/*
*	Defines macro for CCI Function return.
*/
//#include "algorithmtype.h"
	
#define	CCI_OPEN_FAIL 			-1		//CCI打开失败
#define CCI_OPEN_SUCCESS		 0		// CCI打开成功

#define	CCI_CLOSE_SUCCESS	   	 0		//CCI关闭成功
#define	CCI_CLOSE_FAIL			-1		//CCI关闭失败

#define CCI_SND_LEN_ERR 		12		//发送长度错
#define	CCI_BUF_DAMAGE			11		//CCI队列缓冲被破坏
#define	CCI_SND_FAIL			10		//CCI发送失败
#define	CCI_SEND_SUCCESS		9		// CCI发送成功
#define CCI_HOST_UNOPEN			8		//HOST还没有打开
#define	CCI_QUE_OVERFLOW 		7		//队列满。



#define CCI_REV_LEN_ERR 		6		//接收长度错误
#define CCI_USERBUF_NOT_ENOUGH	5		//buf长度不足
#define CCI_PARAM_ERR			4		//参数错误
#define	CCI_REV_WRONG_TYPE		3		//收到错误类型
#define	CCI_REV_SUCCESS			2		//接收正确
#define CCI_REV_NO_MSG 			1		//没有消息

//
//msg type define
//
#define USER_MSG_TYPE			0		//用户定义的消息
#define	CYCLE_TEST_TYPE			1		//自环测试消息
#define	MAP_CYCLE_TEST			2		//MAP自环测试消息
#define	HOST_PRINT_TYPE			3		//HOST打印消息
#define MAP_SEND_TYPE			4		//MAP发送消息
#define	MAP_PRINT_TYPE			5		//MAP打印消息
#define LIGHT_MAP				6		//点灯消息


//#define MAX_DN_QUEUENUM		200			//最大下行队列数。

/* 
* Defines API function for CCI-MAP
*/
SCODE CciMsgOpen(l32 *piQueNum);

SCODE CciMsgClose();

SCODE CciMsgSnd(void* pvMsg, l32 iMsgLen);

SCODE CciSndData(void* pvMsg,l32 iMsgLen,l32 iMsgType);

SCODE CciMsgRev(void* pvMsgStoreBuf,l32 iMsgBufLen,l32 *piMsgLen,l32 iQueNo);

void MsgsInQue(l32 iQueNo, l32 *piMsgsInQue);

u32 HeartBeatAdd();

void CciPrintf(const char *fmt,... );

void CciGetMaxMsgLen(l32 iQueueNo,l32 *piMaxMsgLen);

SCODE CciConcateSndData(void* pvFirstMsg,l32 iFirstMsgLen, void* pvSecondMsg, l32 iSecondMsgLen, l32 iMsgType);

void  CciWriteErrNo(l32 nErrIdx, l32  nErrNo); // nErrIdx从0到15

void  ErrNoWriteEnable(BOOL bEnable);	//设置是否准许cci纪录错误码, 如果不调用，准许写

SCODE CciWriteDownQue(l32 iQueNo, void *pbyFirstMsg, l32 iFirstMsgLen, void *pbySecondMsg, l32 iSecondMsgLen);	//模拟主机写下行队列

#ifdef __cplusplus
}
#endif
#endif /* _CCI_H___ */
