/***************************************************************
模块名      ： bfin_tunnel                     
文件名      ： bfin_tunnel.h
相关文件    ： bfin_tunnel.c
文件实现功能： 双核间通信模块
作者        ： 邵笑杰
版本        ： 1.0   2005/10/26
------------------------------------------------------------------
修改记录:
日  期          版本            修改人          修改内容

*****************************************************************/
#ifndef COMM_H
#define COMM_H

#include "selftype.h"

#define pCOMM_TUNNEL_DESC_START_ADDR (u8*)0xFEB1F000	//通道的描述符起始地址
#define pCOMM_TUNNEL_START_ADDR (TCommTunnel*)0xFEB1FF00	//通道句柄地址
#define pCOMM_TUNNEL_INITIALIZE (s32*)0xFEB1FFFC		//初始化标志地址

#define COMM_TUNNEL_NUMBER 10		//可创建的通道数目
#define COMM_PER_TUNNEL_MAX_DESC 16 //每个通道的最大描述符数

typedef enum{
	COMM_OK,
	COMM_INVALID_PARM,
	COMM_INSUFFICIENT_MEMORY,
	COMM_NOT_INITIALIZED,
	COMM_NOT_CREATE_TUNNEL,
	COMM_TUNNEL_TYPE_CREATE,
	COMM_NOT_IDLE_TUNNEL
} TCommError;

//Buffer状态
typedef enum{
	CommBlankBuffer = 188,
	CommOperatingDone,
	CommFullBuffer,
	CommOperateFail
} TCommBufferStatus;

//通信模块描述符结构
typedef struct CommDesc{
	struct CommDesc *ptNext;
	l32 *pdwDataAddr;
	TCommBufferStatus tStatus;
	u32 dwDataLength;
} TCommDesc;

typedef enum{
	CommVideoDownTunnel,
	CommAudioDownTunnel,
	CommAudioUpTunnel,
	CommPrintInfoTunnel,
	CommUnknowTunnel
} TCommTunnelType;

//通信模块句柄结构
typedef struct{
	u32 dwTunnelNumber;
	TCommTunnelType tTunnelType;
	u32 dwBufferSize;
	u32 dwBufferBlock;
	TCommDesc *ptCommDescAddr;
	u32 dwIsUsed;
} TCommTunnel;

/*====================================================================
函数名      ：CommCreateTunnel
功能        ：创建一个通道
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：dwBufferSize通道的缓冲区大小，dwBufferBlock 通道缓冲去个数
			  tType 通道类型**ptHandle，返回的通道信息
返回值说明  ：COMM错误信息
====================================================================*/
TCommError CommCreateTunnel(u32 dwBufferSize, u32 dwBufferBlock, \
							TCommTunnelType tType, TCommTunnel **ptHandle);

/*====================================================================
函数名      ：CommDisposeBuffer
功能        ：销毁一个通道
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：*ptHandle 销毁的通道信息
返回值说明  ：COMM错误信息
====================================================================*/
TCommError CommDisposeTunnel(TCommTunnel *ptHandle);

/*====================================================================
函数名      ：CommGetDesc
功能        ：得到一个描述符
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：dwTunnelNumber 通道号，ptHandle返回的描述符
返回值说明  ：COMM错误信息
====================================================================*/
TCommError CommGetDesc(u32 dwTunnelNumber, TCommDesc **ptHandle);

/*====================================================================
函数名      ：CommFindTunnel
功能        ：查找tType类型通道
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：tType查找的通道类型，ptHandle返回的描述符
返回值说明  ：COMM错误信息
====================================================================*/
TCommError CommFindTunnel(TCommTunnelType tType, TCommTunnel **ptHandle);

/*====================================================================
函数名      ：CommInitialize
功能        ：初始化通信模块
算法实现    ：（可选项）
引用全局变量：无
输入参数说明：
返回值说明  ：
====================================================================*/
void CommInitialize(void);

#endif


