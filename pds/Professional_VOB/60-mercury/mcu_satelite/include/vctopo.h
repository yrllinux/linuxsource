/*****************************************************************************
   模块名      : 业务新方案
   文件名      : vctopo.h
   相关文件    : 
   文件实现功能: MCU拓扑公共函数定义，本文件所有函数均以topo开头
   作者        : 李屹
   版本        : V0.9  Copyright(C) 2001-2002 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2002/07/15  0.9         李屹        创建
******************************************************************************/

#ifndef _VCTOPO_H_
#define _VCTOPO_H_

#include "mcuvc.h"

//交换码流方向
const u8 DIRECTION_UP		= 1;	//向上码流交换
const u8 DIRECTION_DOWN		= 2;	//向下码流交换
const u8 DIRECTION_TOPMCU	= 3;	//上下交换方向转换地点
const u8 DIRECTION_NONWAY	= 4;	//不在交换路径上

//MCU拓扑配置文件
#define FILENAME_TOPOCFG				(LPCSTR)"topocfg.ini"
/*mcuTopoTable*/
#define SECTION_mcuTopoTable			(LPCSTR)"mcuTopoTable"
#define FILED_McuId						(LPCSTR)"McuId"
#define FILED_SuperiorMcuId				(LPCSTR)"SuperiorMcuId"
#define FILED_McuIpAddr					(LPCSTR)"McuIpAddr"
#define FILED_McuGwIpAddr				(LPCSTR)"McuGwIpAddr"
#define FILED_McuAlias					(LPCSTR)"McuAlias"
/*mtTopoTable*/
#define SECTION_mtTopoTable				(LPCSTR)"mtTopoTable"
#define FILED_MtId						(LPCSTR)"MtId"
#define FILED_McuId						(LPCSTR)"McuId"
#define FILED_MtIpAddr					(LPCSTR)"MtIpAddr"
#define FILED_MtGwIpAddr				(LPCSTR)"MtGwIpAddr"
#define FILED_MtAlias					(LPCSTR)"MtAlias"
#define FILED_PrimaryMt					(LPCSTR)"PrimaryMt"
#define FILED_MtType					(LPCSTR)"MtType"
#define FILED_MtConnected				(LPCSTR)"MTConnect"

/*====================================================================
    函数名      ：topoGetAllTopoSubMcu
    功能        ：获取整个拓扑结构中该MCU的所有下级MCU(可选一层或所有层)
    算法实现    ：递归
    引用全局变量：无
    输入参数说明：WORD wMcuId, 用户输入的McuId
				  u16 wMcuTopo[], 数组，用于存放查找的结果
				  u8 bySize, 数组元素个数
				  TMcuTopo atMcuTopo[], MCU拓扑数组
				  u16 wMcuTopoNum, 全网中MCU数目
				  u8 byMode, 访问层数，0表示所有层，1表示一层，缺省为0
    返回值说明  ：实际数目
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/08    1.0         方华平        创建
    02/09/19    1.0         LI Yi         从CMcuAgent中移过来
    02/10/24    1.0         LI Yi         添加参数byMode
====================================================================*/
 u16 topoGetAllTopoSubMcu( u16    wMcuId,
                                 u16    wSubMcu[],
                                 u8  bySize, 
							     TMcuTopo atMcuTopo[],
                                 u16    wMcuTopoNum,
                                 u8  byMode = 0 );
/*====================================================================
    函数名      ：topoGetMcuTopo
    功能        ：获取指定MCU的自TOP MCU起的拓扑路径
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId, 用户输入的McuId
				  u16 wMcuTopo[], 数组，用于存放查找的结果，从指定MCU
						          到TOP MCU，如"3,2,1"， 0结尾
				  u8 bySize, 数组元素个数，要求不小于5
				  TMcuTopo atMcuTopo[], MCU拓扑数组
				  u16 wMcuTopoNum, 全网中MCU数目
    返回值说明  ：成功返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/08/02    1.0         方华平        创建
    02/09/19    1.0         LI Yi         从CMcuAgent中移过来
====================================================================*/
  BOOL topoGetMcuTopo( u16     wMcuId,
                            u16     wMcuTopo[],
                            u8   bySize, 
					        TMcuTopo atMcuTopo[],
                            u16     wMcuTopoNum );

/*====================================================================
    函数名      ：topoGetInterMcu
    功能        ：计算从源MCU出发到目的MCU的下一站直连MCU号
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wSrcMcu, 源MCU号
				  u16 wDstMcu, 目的MCU号
				  TMcuTopo atMcuTopo[], MCU拓扑数组
				  u16 wMcuTopoNum, 全网中MCU数目
    返回值说明  ：返回直连MCU号，0xffff表示下一站为源MCU的上级MCU，
				  失败返回NULL
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/07/30    1.0         LI Yi         创建
====================================================================*/
  u16 topoGetInterMcu( u16 wSrcMcu, u16 wDstMcu, TMcuTopo atMcuTopo[], u16 wMcuTopoNum );

/*====================================================================
    函数名      ：topoFindMcuPath
    功能        ：计算从源MCU出发到目的MCU的路
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wSrcMcu, 源MCU号
				  u16 wDstMcu, 目的MCU号
				  u16 awMcuPath[], 返回的路径BUFFER，从源到目的MCU，
						如"1,2,3"，0结尾
				  u8 byBufSize, BUFFER大小
				  TMcuTopo atMcuTopo[], MCU拓扑数组
				  u16 wMcuTopoNum, 全网中MCU数目
    返回值说明  ：成功返回TRUE，失败返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/09/19    1.0         LI Yi         创建
====================================================================*/
  BOOL topoFindMcuPath( u16 wSrcMcu, u16 wDstMcu, u16 awMcuPath[], u8 byBufSize, 
					         TMcuTopo atMcuTopo[], u16 wMcuTopoNum );

/*====================================================================
    函数名      ：topoIsCollide
    功能        ：判断用户正在看或听的终端（或外设）是否与跨MCU正在
					发生的改变有线路冲突
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wCurSrcMcuId, 用户正选看或听的终端或外设所属MCU号
				  u16 wConnMcuId, 用户连接的MCU号
				  u16 wSrcMcuId, 线路改变的源终端或外设所属MCU号
				  u16 wDstMcuId, 线路改变的目的终端或外设所属MCU号
				  TMcuTopo atMcuTopo[], MCU拓扑数组
				  u16 wMcuTopoNum, 全网中MCU数目
    返回值说明  ：有冲突返回TRUE，反之FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/09/19    1.0         LI Yi         创建
====================================================================*/
  BOOL topoIsCollide( u16 wCurSrcMcuId,
                           u16 wConnMcuId,
                           u16 wSrcMcuId, 
					       u16 wDstMcuId,
                           TMcuTopo atMcuTopo[],
                           u16      wMcuTopoNum );
/*====================================================================
    函数名      ：topoGetMcuInfo
    功能        ：得到所要的Mcu的TMcuTopo结构；
				  用户需自己保证输入的MCU号在数组中实际存在
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId,要求的MCU号
				  TMcuTopo atMcuTopo[]，MCU拓扑数组 
				  u16 wMcuTopoNum，全网中的MCU数目
    返回值说明  ：成功返回TMcuTopo，否则返回一个内部全为0的TMcuTopo结构
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/25    1.0         Liaoweijiang  创建
====================================================================*/
  TMcuTopo topoGetMcuInfo( u16 wMcuId,  
					            TMcuTopo atMcuTopo[], u16 wMcuTopoNum );

/*====================================================================
    函数名      ：topoGetMtInfo
    功能        ：得到所要的Mt的TMtTopo结构
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId，MT所在的MCU号
				  u8 byMtId，MT在MCU中的编号
				  TMcuTopo atMtTopo[]，Mt的拓扑数组
				  u16 wMtTopoNum，全网的Mt的数目
    返回值说明  ：成功返回TMtTopo，否则返回一个为全0的TMtTopo结构
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/25    1.0         Liaoweijiang  创建
====================================================================*/
  TMtTopo topoGetMtInfo( u16 wMcuId, u8 byMtId, 
					          TMtTopo atMtTopo[], u16 wMtTopoNum );

/*====================================================================
    函数名      ：topoIsSecondaryMt
    功能        ：判断一个Mt是否是次级终端
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId，MT所在的MCU号
				  u8 byMtId，在所给的MCU中的MT号
				  TMtTopo atMtTopo[]，MT的拓扑数组
				  u16 wMtTopoNum，全网MT的数目
    返回值说明  ：如果是返回TRUE, 否则FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/25    1.0         Liaoweijiang  创建
====================================================================*/
  BOOL topoIsSecondaryMt( u16 wMcuId, u8 byMtId, 
					           TMtTopo atMtTopo[], u16 wMtTopoNum );
/*====================================================================
    函数名      ：topoHasSecondaryMt
    功能        ：判断一个Mt是否有次级终端
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId，MT所在的MCU号
				  u8 byMtId，MT在此MCU中的号
				  TMtTopo atMtTopo[]，全网的MT拓扑数组
				  u16 wMtTopoNum，全网中的MT数目
    返回值说明  ：成功返回TRUE, 否则FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/25    1.0         Liaoweijiang  创建
====================================================================*/
  BOOL topoHasSecondaryMt( u16 wMcuId, u8 byMtId, 
					            TMtTopo atMtTopo[], u16 wMtTopoNum );

/*====================================================================
    函数名      ：topoGetPrimaryMt
    功能        ：得到一个终端的主终端
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId，MT所在的MCU号
				  u8 byMtId，MT在此MCU中的号
				  TMtTopo atMtTopo[]，全网的MT拓扑数组
				  u16 wMtTopoNum，全网中的MT数目
    返回值说明  ：成功返回它的主终端号,返回0说明它就是主终端，或非法终端
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/25    1.0         Liaoweijiang  创建
====================================================================*/
  u8 topoGetPrimaryMt( u16 wMcuId, u8 byMtId, 
					           TMtTopo atMtTopo[],  u16  wMtTopoNum );

/*====================================================================
    函数名      ：topoGetExcludedMt
    功能        ：得到一个终端的所有互斥终端
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId，MT所在的MCU号
				  u8 byMtId，MT在此MCU中的号
				  TMtTopo atMtTopo[]，全网的MT拓扑数组
				  u16 wMtTopoNum，全网中的MT数目
				  u8 MtBuffer[]，传进来准备装互斥终端号的数组
				  u16 BufferSize，传进来数组的大小
    返回值说明  ：成功返回实际的终端数目, 否则NULL
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/25    1.0         Liaoweijiang  创建
====================================================================*/
  u16 topoGetExcludedMt( u16 wMcuId, u8 byMtId, 
					          TMtTopo atMtTopo[], u16 wMtTopoNum,
                              u8 MtBuffer[], u16 wBufferSize);

/*====================================================================
    函数名      ：topoGetAllSubMt
    功能        ：得到一个Mcu下的所有Mt
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId : MCU号
				  TMtTopo atMtTopo[] : Mt的拓扑数组
				  u16 wMtTopoNum：全网MT的数目
				  TMt MtBuffer[], 准备放置得到MT的TMt结构的数组
				  u16 wBufferSize, 数组大小
				  byMode 访问模式
					为0,表示只得到本级的MT
					为1,表示可以得到下一级MCU的MT
					为2,表示得到往下二级MCU的MT
    返回值说明  ：实际得到的MT的数目
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/10/25    1.0         Liaoweijiang  创建
====================================================================*/
  u16 topoGetAllSubMt( u16 wMcuId, TMtTopo atMtTopo[], u16 wMtTopoNum, 
							TMt MtBuffer[], u16 wBufferSize, u8 byMode = 0 );

/*====================================================================
    函数名      ：topoGetTopMcuBetweenTwoMcu
    功能        ：得到两个MCU的最低至高点的MCU
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId1 : MCU号
				  u16 wMcuId2 : MCU号
				  TMtTopo atMtTopo[] : Mcu的拓扑数组
				  u16 wMtTopoNum：全网Mcu的数目
    返回值说明  ：两个MCU的最低至高点的MCU号
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/11/07    1.0         Liaoweijiang  创建
====================================================================*/
  u16 topoGetTopMcuBetweenTwoMcu ( u16 wMcuId1, u16 wMcuId2,
										TMcuTopo atMcuTopo[], u16 wMcuTopoNum );

/*====================================================================
    函数名      ：topoGetTopMcu
    功能        ：得到所有MCU的最低至高点的Mcu
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId[] : MCU号数组
				  u8 byMcuNum : 数组大小
				  TMtTopo atMtTopo[] : Mcu的拓扑数组
				  u16 wMtTopoNum：全网Mcu的数目
    返回值说明  ：最低至高点的Mcu
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/11/07    1.0         Liaoweijiang  创建
====================================================================*/
  u16 topoGetTopMcu( u16 wMcuId[], u8 byMcuNum, 
						  TMcuTopo atMcuTopo[], u16 wMcuTopoNum );

/*====================================================================
    函数名      ：topoJudgeLegality
    功能        ：拓扑结构合法性判断
    算法实现    ：
    引用全局变量：
    输入参数说明：TMcuTopo atMcuTopo[], Mcu的拓扑数组
				  u16 wMcuTopoNum, 全网Mcu的数目
				  TMtTopo atMtTopo[] : Mt的拓扑数组
				  u16 wMtTopoNum：全网Mt的数目
    返回值说明  ：最低至高点的Mcu
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/11/13    1.0         Liaoweijiang  创建
====================================================================*/
BOOL topoJudgeLegality( TMcuTopo atMcuTopo[], u16 wMcuTopoNum,
							   TMtTopo atMtTopo[], u16 wMtTopoNum );

/*====================================================================
    函数名      ：topoIsValidMt
    功能        ：根据终端号，IP地址和假名进行合法性判断
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId, MCU号
				  u8 byMtId, MT号
				  LPCSTR lpszAlias, 终端假名
				  TMtTopo atMtTopo[], Mt的拓扑数组
				  u16 wMtTopoNum, 全网Mt的数目
    返回值说明  ：成功返回TRUE，失败返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/05    1.0         LI Yi         创建
====================================================================*/
BOOL topoIsValidMt( u16 wMcuId, u8 byMtId, LPCSTR lpszAlias, TMtTopo atMtTopo[], u16 wMtTopoNum );

/*====================================================================
    函数名      ：topoIsValidMcu
    功能        ：根据MCU号，IP地址和假名进行合法性判断
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId, MCU号
				  u32 dwIpAddr, 终端IP地址，网络序
				  LPCSTR lpszAlias, 终端假名
				  TMcuTopo atMcuTopo[], Mcu的拓扑数组
				  u16 wMcuTopoNum, 全网Mcu的数目
    返回值说明  ：成功返回TRUE，失败返回FALSE
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    02/12/05    1.0         LI Yi         创建
====================================================================*/
BOOL topoIsValidMcu( u16 wMcuId, u32 dwIpAddr, LPCSTR lpszAlias, 
						   TMcuTopo atMcuTopo[], u16 wMcuTopoNum );

/*====================================================================
    函数名      ：topoFindSubMtByIp
    功能        ：根据MCU号，IP地址得到下级终端号
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId, MCU号
				  u32 dwIpAddr, 终端IP地址，网络序
				  TMcuTopo atMtTopo[]，Mt的拓扑数组
				  u16 wMtTopoNum，全网的Mt的数目
    返回值说明  ：成功返回终端号，失败返回0
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/03/12    1.0         LI Yi         创建
====================================================================*/
u8 topoFindSubMtByIp( u16 wMcuId, u32 dwIpAddr, TMtTopo atMtTopo[], u16 wMtTopoNum );

/*====================================================================
    函数名      ：topoMtNeedAdapt
    功能        ：判断终端是否需要进行适配
    算法实现    ：
    引用全局变量：
    输入参数说明：WORD wMcuId, MCU号
				  u32 dwIpAddr, 终端IP地址，网络序
				  TMcuTopo atMtTopo[]，Mt的拓扑数组
				  u16 wMtTopoNum，全网的Mt的数目
    返回值说明  ：成功返回终端号，失败返回0
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
    03/03/12    1.0         LI Yi         创建
====================================================================*/
BOOL topoMtNeedAdapt( u16 wMcuId, u8 byMtId, TMtTopo atMtTopo[], u16 wMtTopoNum );


/*====================================================================
    函数名      ：ReadMcuTopoTable
    功能        ：读入MCU拓扑信息
    算法实现    ：
    引用全局变量：无
    输入参数说明：LPCSTR lpszProfileName, 文件名（含绝对路径）
                  TMcuTopo atMcuTopoBuf[], MCU信息BUFFER
				  u16 wBufSize, BUFFER大小
    返回值说明  ：拓扑中MCU实际数目
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	02/09/18    1.1         LI Yi         创建
====================================================================*/
u8 ReadMcuTopoTable( LPCSTR lpszProfileName, TMcuTopo atMcuTopo[], u16 wBufSize );

/*====================================================================
    函数名      ：ReadMtTopoTable
    功能        ：读入MT拓扑信息
    算法实现    ：
    引用全局变量：无
    输入参数说明：LPCSTR lpszProfileName, 文件名（含绝对路径）
                  TMtTopo atMtTopoBuf[], Mt信息BUFFER
				  u16 wBufSize, BUFFER大小
    返回值说明  ：拓扑中Mt实际数目
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	02/10/24    1.0         liaoweijiang  创建
====================================================================*/
u16 ReadMtTopoTable( LPCSTR lpszProfileName, TMtTopo atMtTopo[], u16 wBufSize );

/*====================================================================
    函数名      :ReadMcuModemTable
    功能        ：读入MCU Modem拓扑信息
    算法实现    ：
    引用全局变量：无
    输入参数说明：LPCSTR lpszProfileName, 文件名（含绝对路径）
                  TMtTopo atMtTopoBuf[], Mt信息BUFFER
				  u16 wBufSize, BUFFER大小
    返回值说明  ：拓扑中Mt实际数目
----------------------------------------------------------------------
    修改记录    ：
    日  期      版本        修改人        修改内容
	02/10/24    1.0         liaoweijiang  创建
====================================================================*/
u8 ReadMcuModemTable( LPCSTR lpszProfileName, TMcuModemTopo atMcuModemTopo[], u16 wBufSize );

#endif /* _VCTOPO_H_ */

