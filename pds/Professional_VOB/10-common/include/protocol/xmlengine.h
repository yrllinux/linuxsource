/*******************************************************************************
 *  模块名   : XML编解码                                                       *
 *  文件名   : xmlengine.h                                                     *
 *  相关文件 :                                                                 *
 *  实现功能 :                                                                 *
 *  作者     : 俞佩菲                                                          *
 *  版本     : V4.0  Copyright(c) 1997-2010 KDC, All rights reserved.          *
 *                                                                             *
 *    =======================================================================  *
 *  修改记录:                                                                  *
 *    日  期      版本        修改人      修改内容                             *
 *  2010/03/18    4.0         俞佩菲      创建                                 *
 *                                                                             *
 *******************************************************************************/

#ifndef _XML_ENGINE_H_
#define _XML_ENGINE_H_

#include "kdvtype.h"

typedef u32 u32_ip;
typedef int BOOL;

struct TXDATANODE
{
	u16   type;        //如em_EmChanType
	u16   pretypesize; //如sizeof(EmChanType)
	char* pretype;     //如EmChanType
	int   arraynum;    //维数
};

struct TXDATAINFO
{
	char* name;			  //所属结构类型名
	int   type;			  //所属结构数据类型	
	char* membertypename; //该成员类型名
	int	  size; 		  //该成员数据类型大小	
	int   arraymembernum; //该成员数据个数	
	int   matrixnum; 	  //如果该成员是二维数组，值为第一维的数目，否则为0
	int   offset;		  //该成员相对于其结构的数据地址偏移量	
	char* member; 		  //成员变量名称
};

struct TXENUMINFO
{
	char* descrip;
	int   val;
};

#define  XML_ARRAY_SIZE       8
#define  XML_INVALID_INDEX    -1
#define  XML_OffSetOf(s,m)   (size_t)&(((s *)0)->m)
#define  XML_CountOf(array)  (sizeof(array)/sizeof(array[0]))
#define  XML_SizeOfMember(s,m) (size_t)(sizeof((((s *)0)->m)))
#define  XML_SizeOfMatrix(s,type,m) (XML_SizeOfMember(s,m)>1 ? (XML_SizeOfMember(s,m))/(size_t)(sizeof(((s *)0)->m[0])):1)

#ifndef NULL
//#define NULL    ((void *)0)
#define NULL    (0)
#endif

enum EmXMLOpsType
{
	em_XMLNone = 0, 
	em_XMLAuto, 
	em_XMLCompression,  //lzw压缩
	em_XMLEncryption,   //加密
	em_XMLCompAndEnc,   //压缩和加密
	em_XMLEnd
};

char* GetXmlStructVer();

s32 InitXMLEngine( TXDATAINFO** tAtomDataInfo,   //初始化g_txAtomDataInfo
					  TXDATAINFO** tEnumDataInfo,   //初始化g_txEnumDataInfo
					  TXDATAINFO** tStructDataInfo, //初始化g_txStructDataInfo,
					  u16 wAtomTypeBegin,
					  u16 wEnumTypeBegin,
					  u16 wStructTypeBegin,
					  u16 wDataTypeEnd );
void ExitXMLEngine( s32 nXMLIndex );

void InitAtomType_s32    ( u16 wEmx_s32     );
void InitAtomType_BOOL32 ( u16 wEmx_BOOL32  );
void InitAtomType_BOOL   ( u16 wEmx_BOOL    );
void InitAtomType_u8     ( u16 wEmx_u8      );
void InitAtomType_s8     ( u16 wEmx_s8      );
void InitAtomType_u16    ( u16 wEmx_u16     );
void InitAtomType_u32    ( u16 wEmx_u32     );
void InitAtomType_time_t ( u16 wEmx_time_t );
void InitAtomType_u32_ip ( u16 wEmx_u32_ip  );
void InitAtomType_u8_blob( u16 wEmx_u8_blob );


BOOL32 XML_Encode( TXDATANODE* ptDataNode,	  //结构语法表数组指针
				   void* pSrcBuf,             //event要发送的buffer
				   u32   dwSrcBufLen,         //event要发送的buffer长度
				   EmXMLOpsType emXmlType,    //xml操作类型(压缩、加密等)
				   s8*   pDstBuf,             //经过xml正向操作之后返回的buffer
				   u32&  dwDstBufLen,         //经过xml正向操作之后返回的有效buffer长度
				   u32&  dwOverFlow,          //溢出标志，0为xml编码，1为压缩，2为内容为空无需压缩
				   u16   wEvent = 0,
				   s8*   pbyEventName = NULL,
				   u32   dwEventHash = 0,
				   s32   nIndex = 0 ); 

BOOL32 XML_Decode( TXDATANODE* ptDataNode,	  //结构语法表数组指针
				   s8*   pSrcBuf,	          //从底层拿到的buffer
				   u32   dwSrcBufLen,         //从底层拿到的buffer长度
				   EmXMLOpsType emXmlType,    //xml操作类型(解压缩、解密等)
				   void* pDstBuf,             //经过xml逆向操作之后返回的buffer
				   u32&  dwDstBufLen,         //经过xml逆向操作之后返回的有效buffer长度
				   u32   dwOverFlow = 0,      //1表示需要解压缩
				   u16   wEvent = 0,
				   s8*   pbyEventName = NULL,
				   s32   nIndex = 0 );

BOOL32 XML_Encode( u16 wDataType,			  //单个结构/数据类型, EmXDataType
				   void* pSrcBuf, 
				   EmXMLOpsType emXmlType, 
				   s8* pDstBuf,
				   u32& dwDstBufLen,
				   u32& dwOverFlow,
				   s32  nIndex = 0 );

BOOL32 XML_Decode( u16 wDataType,			  //单个结构/数据类型, EmXDataType
				   s8* pSrcBuf, 
				   u32 dwSrcBufLen,
				   EmXMLOpsType emXmlType, 
				   void* pDstBuf,
				   u32& dwDstBufLen,
				   u32 dwOverFlow,
				   s32 nIndex = 0 );

BOOL32 XML_GetEventHash( s8* pSrcBuf, u32   dwSrcBufLen, EmXMLOpsType emXmlType, s32& dwHash, u32 dwOverFlow );
BOOL32 XML_GetEventName( s8* pSrcBuf, u32   dwSrcBufLen, EmXMLOpsType emXmlType, s8* pEventName, u32 dwOverFlow );

#endif //_XML_ENGINE_H_


