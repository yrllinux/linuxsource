/*****************************************************************************
   模块名      : MCU-DCS带内方式集成，通信数据
   文件名      : mcudcspdu.h
   相关文件    : 
   文件实现功能: 
   作者        : 赖齐
   版本        : V4.0  Copyright(C) 2003-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人      修改内容
   2005/12/02  0.1         赖齐        创建
   2006/06/14  0.2         罗健锋      添加实现
******************************************************************************/
#ifndef _MCU_DCS_PDU_H_
#define _MCU_DCS_PDU_H_


#define IN
#define OUT
#define IN_OUT
#define CONDITION_RETURN(func) if ( !func ) return FALSE;

#ifdef WIN32
    #pragma pack( push )
    #pragma pack( 1 )
    #ifndef PACKED
        #define PACKED
    #endif
#else
    #ifndef PACKED
        #define PACKED __attribute__ ( (packed) ) 
    #endif
#endif


/*====================================================================
-- Part 1 : 本文件说明和使用方法示例
====================================================================*/
// 本文件的结构：
//    接口部分：
//                编解码器接口  CKdvMcuDcsNetData
//                基本数据接口  CKdvMcuDcsItem
//                PDU接口       CMcuDcsPDU
//    实现部分：
//                编解码器实现  CNetDataCoder
//                基本数据实现  
//                PDU实现
//
// 使用示例
//
// CDcsMcuMtMtOfflineNotifyPdu cExamplePdu;     // 协议PDU
// CNetDataCoder cCoder;                        // 编解码器
// u8 achNetData[100];                          // 网络数据
//
// 编码实例
// cCoder.SetNetDataBuffer(achNetData, 100);
// cExamplePdu.PduEncode(cCoder);
// 编码后的数据存放在achNetData中
// 
// 解码实例
// cCoder.SetNetDataBuffer(achNetData, 100);
// cExamplePdu.PduDecode(cCoder);
// 解码后的数据存放在cExamplePdu中

/*====================================================================
-- Part 2 : 基类接口定义
====================================================================*/

/*====================================================================
类名 ： CKdvMcuDcsNetData
描述 ： 编解码器，定义基本编码方案
====================================================================*/
class CKdvMcuDcsNetData
{
protected:
    u32 m_dwBufferLength;
    u8* m_pBufferPointer;
    u32 m_dwCurrentPos;
public:
    // 构造函数
    CKdvMcuDcsNetData() : m_dwBufferLength(0), m_pBufferPointer(NULL), m_dwCurrentPos(0)
    {
    }
    u32 GetCurrentLength()
    {
        return m_dwCurrentPos;
    }
    // 设置网络数据缓冲区，(编码后/解码前)的数据存放在这里。
    virtual void SetNetDataBuffer(IN u8* pBufferPointer, IN u32 dwBufferLength)
    {
        m_pBufferPointer = pBufferPointer;
        m_dwBufferLength = dwBufferLength;
        m_dwCurrentPos = 0;
    }
    // 跳过指定长度的数据，不进行编解码
    BOOL32 SkipData(IN u16 nCount)
    {
        if (m_dwCurrentPos + nCount <= m_dwBufferLength)
        {
            m_dwCurrentPos += nCount;
            return TRUE;
        }
        return FALSE;
    }
    // U8类型编码
    virtual BOOL32 U8Encode(IN u8 nCount, IN const u8* pStartAt) = 0;
    // S8类型编码
    virtual BOOL32 S8Encode(IN u8 nCount, IN const s8* pStartAt) = 0;
    // U16类型编码
    virtual BOOL32 U16Encode(IN u8 nCount, IN const u16* pStartAt) = 0;
    // S16类型编码
    virtual BOOL32 S16Encode(IN u8 nCount, IN const s16* pStartAt) = 0;
    // U32类型编码
    virtual BOOL32 U32Encode(IN u8 nCount, IN const u32* pStartAt) = 0;
    // S32类型编码
    virtual BOOL32 S32Encode(IN u8 nCount, IN const s32* pStartAt) = 0;
    // U8类型解码
    virtual BOOL32 U8Decode(IN u8 nCount, OUT u8* pStartAt) = 0;
    // S8类型解码
    virtual BOOL32 S8Decode(IN u8 nCount, OUT s8* pStartAt) = 0;
    // U16类型解码
    virtual BOOL32 U16Decode(IN u8 nCount, OUT u16* pStartAt) = 0;
    // S16类型解码
    virtual BOOL32 S16Decode(IN u8 nCount, OUT s16* pStartAt) = 0;
    // U32类型解码
    virtual BOOL32 U32Decode(IN u8 nCount, OUT u32* pStartAt) = 0;
    // S32类型解码
    virtual BOOL32 S32Decode(IN u8 nCount, OUT s32* pStartAt) = 0;
}PACKED;


/*====================================================================
类名 ： CKdvMcuDcsItem
描述 ： 协议的基本数据，可独立编解码
====================================================================*/
class CKdvMcuDcsItem
{
public:
    // 基本数据类型定义
    enum EItemType
    {
        ErrorInfor      = 0,
        DcsInfor        = 1,
        ConfBasicInfor  = 2,
        ConfShortInfor  = 3,
        MtShortInfor    = 4,
        McuMsstateInfor = 5
    };
    // 基本数据编码
    BOOL32 ItemEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        CONDITION_RETURN(ItemEncodeHead(cNetData));
        CONDITION_RETURN(ItemDataEncode(cNetData));
        return TRUE;
    }
    // 基本数据解码
    BOOL32 ItemDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        CONDITION_RETURN(ItemDecodeHead(cNetData));
        CONDITION_RETURN(ItemDataDecode(cNetData));
        return TRUE;
    }
protected:
    // 基本数据内容编码
    virtual BOOL32 ItemDataEncode(IN_OUT CKdvMcuDcsNetData&) const = 0;
    // 基本数据内容解码
    virtual BOOL32 ItemDataDecode(IN_OUT CKdvMcuDcsNetData&) = 0;
    // 得到基本数据类型
    virtual EItemType GetItemType() const = 0;
    // 得到基本数据的内容的长度
    virtual u16 GetItemLength() const = 0;
private:
    BOOL32 ItemEncodeHead(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        u8 byItemType = GetItemType();
        CONDITION_RETURN(cNetData.U8Encode(1, &byItemType));
        u16 wItemLengh = GetItemLength();
        CONDITION_RETURN(cNetData.U16Encode(1, &wItemLengh));
        return TRUE;
    }
    BOOL32 ItemDecodeHead(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        u8 byItemType;
        u16 wItemLengh;
        CONDITION_RETURN(cNetData.U8Decode(1, &byItemType));
        CONDITION_RETURN(cNetData.U16Decode(1, &wItemLengh));
        // 如果不是指定需要的Item，跳过该Item
        if ( byItemType != (u8)GetItemType() )
        {
            cNetData.SkipData(wItemLengh);
            return FALSE;
        }
        return TRUE;
    }
}PACKED;


/*====================================================================
类名 ： CMcuDcsPDU
描述 ： PDU，可独立编解码
====================================================================*/
class CMcuDcsPDU
{
public:
    // 包头长度
    u8 m_nHeadLength;
    // 版本号
    u8 m_byVersion;
    // 网络数据序列号
    u8 m_bySeq;
    // 会议ID
    u16 m_wConfId;
    // 终端ID
    u16 m_wMtId;
    // 构造函数
    CMcuDcsPDU() : m_nHeadLength(7), m_byVersion(1), m_bySeq(0), m_wConfId(0), m_wMtId(0)
    {
    }
    // 从网络数据得到本PDU
    virtual BOOL32 PduDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        CONDITION_RETURN(PduDecodeHead(cNetData));
        CONDITION_RETURN(PduDataDecode(cNetData));
        return TRUE;
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        CONDITION_RETURN(PduEncodeHead(cNetData));
        CONDITION_RETURN(PduDataEncode(cNetData));
        return TRUE;
    }
protected:
    // PDU数据内容编码
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return TRUE;
    };
    // PDU数据内容解码
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return TRUE;
    };
private:
    // PDU包头编码
    BOOL32 PduEncodeHead(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        CONDITION_RETURN(cNetData.U8Encode(1, &m_nHeadLength));
        CONDITION_RETURN(cNetData.U8Encode(1, &m_byVersion));
        CONDITION_RETURN(cNetData.U8Encode(1, &m_bySeq));
        CONDITION_RETURN(cNetData.U16Encode(1, &m_wConfId));
        CONDITION_RETURN(cNetData.U16Encode(1, &m_wMtId));
        return TRUE;
    }
    // PDU包头解码
    BOOL32 PduDecodeHead(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        CONDITION_RETURN(cNetData.U8Decode(1, &m_nHeadLength));
        if (m_nHeadLength < 7)
        {
            return FALSE;
        }
        CONDITION_RETURN(cNetData.U8Decode(1, &m_byVersion));
        CONDITION_RETURN(cNetData.U8Decode(1, &m_bySeq));
        CONDITION_RETURN(cNetData.U16Decode(1, &m_wConfId));
        CONDITION_RETURN(cNetData.U16Decode(1, &m_wMtId));
        if (m_nHeadLength > 7)
        {
            cNetData.SkipData(m_nHeadLength-7);
        }
        return TRUE;
    }
}PACKED;


/*====================================================================
-- Part 3 : 编解码器实现
====================================================================*/


/*====================================================================
类名 ： CNetDataCoder
描述 ： 简单编解码器实现
====================================================================*/
class CNetDataCoder : public CKdvMcuDcsNetData
{
public:
    // U8类型编码
    virtual BOOL32 U8Encode(IN u8 nCount, IN const u8* pStartAt)
    {
        if ( m_dwCurrentPos + nCount <= m_dwBufferLength )
        {
            memcpy(m_pBufferPointer+m_dwCurrentPos, pStartAt, nCount);
            m_dwCurrentPos += nCount;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    };
    // S8类型编码
    virtual BOOL32 S8Encode(IN u8 nCount, IN const s8* pStartAt)
    {
        return U8Encode(nCount, (const u8*)pStartAt);
    };
    // U16类型编码
    virtual BOOL32 U16Encode(IN u8 nCount, IN const u16* pStartAt)
    {
        if (m_dwCurrentPos+2*nCount <= m_dwBufferLength)
        {
            u16* pCurrentU16Pointer = (u16*)(m_pBufferPointer+m_dwCurrentPos);
            m_dwCurrentPos += 2*nCount;
            for (s32 nPos=0; nPos<nCount; nPos++)
            {
                *pCurrentU16Pointer = htons(*(pStartAt+nPos));
                pCurrentU16Pointer++;
            }
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    };
    // S16类型编码
    virtual BOOL32 S16Encode(IN u8 nCount, IN const s16* pStartAt)
    {
        return U16Encode(nCount, (const u16*)pStartAt);
    };
    // U32类型编码
    virtual BOOL32 U32Encode(IN u8 nCount, IN const u32* pStartAt)
    {
        if (m_dwCurrentPos+4*nCount <= m_dwBufferLength)
        {
            u32* pCurrentU32Pointer = (u32*)(m_pBufferPointer+m_dwCurrentPos);
            m_dwCurrentPos += 4*nCount;
            for (s32 nPos=0; nPos<nCount; nPos++)
            {
                *pCurrentU32Pointer = htonl(*(pStartAt+nPos));
                pCurrentU32Pointer++;
            }
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    // S32类型编码
    virtual BOOL32 S32Encode(IN u8 nCount, IN const s32* pStartAt)
    {
        return U32Encode(nCount, (const u32*)pStartAt);
    }

    // U8类型解码
    virtual BOOL32 U8Decode(IN u8 nCount, OUT u8* pStartAt)
    {
        if ( m_dwCurrentPos + nCount <= m_dwBufferLength )
        {
            memcpy(pStartAt, m_pBufferPointer+m_dwCurrentPos, nCount);
            m_dwCurrentPos += nCount;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    // S8类型解码
    virtual BOOL32 S8Decode(IN u8 nCount, OUT s8* pStartAt)
    {
        return U8Decode(nCount, (u8*)pStartAt);
    }
    // U16类型解码
    virtual BOOL32 U16Decode(IN u8 nCount, OUT u16* pStartAt)
    {
        if (m_dwCurrentPos+2*nCount <= m_dwBufferLength)
        {
            u16* pCurrentU16Pointer = (u16*)(m_pBufferPointer+m_dwCurrentPos);
            m_dwCurrentPos += 2*nCount;
            for (s32 nPos=0; nPos<nCount; nPos++)
            {
                *(pStartAt+nPos) = ntohs(*pCurrentU16Pointer);
                pCurrentU16Pointer++;
            }
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    // S16类型解码
    virtual BOOL32 S16Decode(IN u8 nCount, OUT s16* pStartAt)
    {
        return U16Decode(nCount, (u16*)pStartAt);
    }
    // U32类型解码
    virtual BOOL32 U32Decode(IN u8 nCount, OUT u32* pStartAt)
    {
        if (m_dwCurrentPos+4*nCount <= m_dwBufferLength)
        {
            u32* pCurrentU32Pointer = (u32*)(m_pBufferPointer+m_dwCurrentPos);
            m_dwCurrentPos += 4*nCount;
            for (s32 nPos=0; nPos<nCount; nPos++)
            {
                *(pStartAt+nPos) = ntohl(*pCurrentU32Pointer);
                pCurrentU32Pointer++;
            }
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    // S32类型解码
    virtual BOOL32 S32Decode(IN u8 nCount, OUT s32* pStartAt)
    {
        return U32Decode(nCount, (u32*)pStartAt);
    }
}PACKED;

/*====================================================================
-- Part 4 : 基本数据实现
====================================================================*/

/*====================================================================
类名 ： CErrorInfor
描述 ： 基本数据：错误信息
====================================================================*/
class CErrorInfor : public CKdvMcuDcsItem
{
public:
    // 错误类型定义
    enum EErrorType
    {
        e_Succuss               = 0,    // 成功
        e_UnSpecified           = 1,    // 未描述
		e_UnConfig				= 2,	// 该DCS没有配置
        e_NoSuchConf            = 3,    // 无此会议
        e_ConfAlreadyExist      = 4,    // 会议已存在
        e_NoSuchMt              = 5,    // 无此终端
        e_MtAlreadyExist        = 6,    // 终端已存在
        e_NotOnline             = 7,    // 终端没有上线
        e_AlreadyOnline         = 8,    // 终端已上线
        e_PermissionDeny        = 9,    // 无此权限
        e_NotReady              = 10,    // 未准备好
        e_PassWordError         = 11,   // 密码不正确
        e_ConfFull              = 12,   // 会议已满
        e_MtFull                = 13,   // 终端已满
		e_DcsFull				= 14,	// 注册的DCS已满
		e_NotDaemon				= 15,	// 注册消息没有发到DAEMON里
        e_NotDoubleMcu          = 16    // 没有储备MCU 
    };
    // 错误类型
    EErrorType m_emErrorType;
protected:
    // 基本数据内容编码
    virtual BOOL32 ItemDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        u8 byErrorType = (u8)m_emErrorType;
        CONDITION_RETURN(cNetData.U8Encode(1, &byErrorType));
        return TRUE;
    }
    // 基本数据内容解码
    virtual BOOL32 ItemDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        u8 byErrorType;
        CONDITION_RETURN(cNetData.U8Decode(1, &byErrorType));
        m_emErrorType = (EErrorType)byErrorType;
        return TRUE;
    }
    // 得到基本数据类型
    virtual EItemType GetItemType() const
    {
        return ErrorInfor;
    }
    // 得到基本数据的内容的长度
    virtual u16 GetItemLength() const
    {
        return (sizeof(u8));
    }
}PACKED;


/*====================================================================
类名 ： CDcsInfor
描述 ： 基本数据：DCS信息
====================================================================*/
class CDcsInfor : public CKdvMcuDcsItem
{
private:
    // 可选项情况：如果某一位为1，表示该可选项存在，0表示不存在
        // 第一位 ---- 最大会议数
        // 第二位 ---- 最大终端数
		// 第三位 ---- 最大直接下级终端数
        // 第四位 ---- 最高级联数
        // 第五位 ---- 预留端口起始端口和范围
        // 第六为 ---- DCS ID（MCU内部使用）
    u8 m_byOptionField;

public:
    // DCS IP地址
    u32 m_dwDcsIp;
    // DCS端口
    u16 m_wDcsPort;
    // 最大会议数，默认32
    u8 m_byMaxConfCount;
    // 会议中最大终端数，默认64
    u8 m_byMaxMtCount;
    // 最大直接下级终端数，默认32
    u8 m_byMaxDirectMtCount;
    // 最高级联数，默认16
    u8 m_byMaxHeight;
    // 预留端口范围起始端口，默认9000
    u16 m_wReservedPortStart;
    // 预留端口范围大小，默认100
    u16 m_wReservedPortRang;
	// DCS ID (MCU内部使用)
	u8  m_byDcsId;
    CDcsInfor() : m_byOptionField(0), m_byMaxConfCount(32), m_byMaxMtCount(64),
        m_byMaxHeight(16), m_wReservedPortStart(9000), m_wReservedPortRang(100), m_byDcsId(0)
    {
    }
public:
    enum EOption
    {
        e_MaxConfCount   = 0x80>>0, // 最大会议数
        e_MaxMtCount     = 0x80>>1, // 会议中最大终端数
        e_MaxDirectCount = 0x80>>2, // 最大直接下级终端数
        e_Height         = 0x80>>3, // 最高级联数
        e_ReservedPort   = 0x80>>4, // 预留端口信息
		e_DcsId			 = 0x80>>5	// DCS ID号
    };
    // 加入可选项
    void IncludeOption(IN EOption emOption)
    {
        m_byOptionField |= emOption;
    }
    // 判断是否有可选项
    BOOL32 HasOption(IN EOption emOption) const
    {
        return ( (m_byOptionField & emOption) != 0 );
    }

protected:
    // 基本数据内容编码
    virtual BOOL32 ItemDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        CONDITION_RETURN(cNetData.U32Encode(1, &m_dwDcsIp));
        CONDITION_RETURN(cNetData.U16Encode(1, &m_wDcsPort));
        CONDITION_RETURN(cNetData.U8Encode(1, &m_byOptionField));
        if ( HasOption(e_MaxConfCount) )
        {
            CONDITION_RETURN(cNetData.U8Encode(1, &m_byMaxConfCount));
        }
        if ( HasOption(e_MaxMtCount) )
        {
            CONDITION_RETURN(cNetData.U8Encode(1, &m_byMaxMtCount));
        }
        if ( HasOption(e_MaxDirectCount) )
        {
            CONDITION_RETURN(cNetData.U8Encode(1, &m_byMaxDirectMtCount));
        }
        if ( HasOption(e_Height) )
        {
            CONDITION_RETURN(cNetData.U8Encode(1, &m_byMaxHeight));
        }
        if ( HasOption(e_ReservedPort) )
        {
            CONDITION_RETURN(cNetData.U16Encode(1, &m_wReservedPortStart));
            CONDITION_RETURN(cNetData.U16Encode(1, &m_wReservedPortRang));
        }
		if ( HasOption(e_DcsId) )
        {
            CONDITION_RETURN(cNetData.U8Encode(1, &m_byDcsId));
        }
        return TRUE;
    }
    // 基本数据内容解码
    virtual BOOL32 ItemDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        CONDITION_RETURN(cNetData.U32Decode(1, &m_dwDcsIp));
        CONDITION_RETURN(cNetData.U16Decode(1, &m_wDcsPort));
        CONDITION_RETURN(cNetData.U8Decode(1, &m_byOptionField));
        if ( HasOption(e_MaxConfCount) )
        {
            CONDITION_RETURN(cNetData.U8Decode(1, &m_byMaxConfCount));
        }
        if ( HasOption(e_MaxMtCount) )
        {
            CONDITION_RETURN(cNetData.U8Decode(1, &m_byMaxMtCount));
        }
        if ( HasOption(e_MaxDirectCount) )
        {
            CONDITION_RETURN(cNetData.U8Decode(1, &m_byMaxDirectMtCount));
        }
        if ( HasOption(e_Height) )
        {
            CONDITION_RETURN(cNetData.U8Decode(1, &m_byMaxHeight));
        }
        if ( HasOption(e_ReservedPort) )
        {
            CONDITION_RETURN(cNetData.U16Decode(1, &m_wReservedPortStart));
            CONDITION_RETURN(cNetData.U16Decode(1, &m_wReservedPortRang));
        }
		if ( HasOption(e_DcsId) )
        {
            CONDITION_RETURN(cNetData.U8Decode(1, &m_byDcsId));
        }
        return TRUE;
    }
    // 得到基本数据类型
    virtual EItemType GetItemType() const
    {
        return DcsInfor;
    }
    // 得到基本数据的内容的长度
    virtual u16 GetItemLength() const
    {
        u16 wLength = sizeof(u32) + sizeof(u16) + sizeof(u8);
        if ( HasOption(e_MaxConfCount) )
        {
            wLength += sizeof(u8);
        }
        if ( HasOption(e_MaxMtCount) )
        {
            wLength += sizeof(u8);
        }
        if ( HasOption(e_MaxDirectCount) )
        {
            wLength += sizeof(u8);
        }
        if ( HasOption(e_Height) )
        {
            wLength += sizeof(u8);
        }
        if ( HasOption(e_ReservedPort) )
        {
            wLength += 2*sizeof(u16);
        }
		if ( HasOption(e_DcsId) )
        {
            wLength += sizeof(u8);
        }

        return wLength;
    }
}PACKED;


/*====================================================================
类名 ： CConfBasicInfor
描述 ： 基本数据：会议基本信息
====================================================================*/
class CConfBasicInfor : public CKdvMcuDcsItem
{
public:
    // 会议名称
    s8 m_achConfName[64];
    // 是否带有密码
    BOOL32 m_bHasPassword;
    // 会议密码
    s8 m_achConfPassword[64];
    // 构造函数
    CConfBasicInfor() : m_bHasPassword(FALSE)
    {
        memset(m_achConfName, 0, sizeof(m_achConfName));
        memset(m_achConfPassword, 0, sizeof(m_achConfPassword));
    }
protected:
    // 基本数据内容编码
    virtual BOOL32 ItemDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        CONDITION_RETURN(cNetData.S8Encode(64, m_achConfName));
        u8 byHasPassword = (u8)m_bHasPassword;
        CONDITION_RETURN(cNetData.U8Encode(1, &byHasPassword));
        if (m_bHasPassword)
        {
            CONDITION_RETURN(cNetData.S8Encode(64, m_achConfPassword));
        }
        return TRUE;
    }
    // 基本数据内容解码
    virtual BOOL32 ItemDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        CONDITION_RETURN(cNetData.S8Decode(64, m_achConfName));
        u8 byHasPassword;
        CONDITION_RETURN(cNetData.U8Decode(1, &byHasPassword));
        m_bHasPassword = (byHasPassword!=0);
		if (m_bHasPassword)
		{
			CONDITION_RETURN(cNetData.S8Decode(64, m_achConfPassword));
		}
        return TRUE;
    }
    // 得到基本数据类型
    virtual EItemType GetItemType() const
    {
        return ConfBasicInfor;
    }
    // 得到基本数据的内容的长度
    virtual u16 GetItemLength() const
    {
        u16 wItemLength = sizeof(m_achConfName) + sizeof(u8);
        if (m_bHasPassword)
        {
            wItemLength += sizeof(m_achConfPassword);
        }
        return wItemLength;
    }
}PACKED;


/*====================================================================
类名 ： CConfShortInfor
描述 ： 基本数据：会议简单信息
====================================================================*/
class CConfShortInfor : public CKdvMcuDcsItem
{
private:
    // 可选项情况：如果某一位为1，表示该可选项存在，0表示不存在
        // 第一位 ---- 带宽单位KB
        // 第二位 ---- 是否接受终端主动呼入
    u8 m_byOptionField;

public:
    enum EOption
    {
        e_BandWidth   = 0x80>>0, // 带宽
        e_SupportJoin = 0x80>>1  // 是否接受终端主动呼入
    };

 	// 带宽单位KB
    u32 m_dwBandWidth;
    // 是否接受终端主动呼入
    BOOL32 m_bSupportJoinedMt;

public:
    // 构造函数
    CConfShortInfor() :
        m_byOptionField(0), m_dwBandWidth(64), m_bSupportJoinedMt(FALSE)
    {
    }
    // 加入可选项
    void IncludeOption(IN EOption emOption)
    {
        m_byOptionField |= emOption;
    }
    // 判断是否有可选项
    BOOL32 HasOption(IN EOption emOption) const
    {
        return ( (m_byOptionField&emOption) != 0 );
    }

protected:
    // 基本数据内容编码
    virtual BOOL32 ItemDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        u8 byTempBool;
        CONDITION_RETURN(cNetData.U8Encode(1, &m_byOptionField));
        if ( HasOption(e_BandWidth) )
        {
            CONDITION_RETURN(cNetData.U32Encode(1, &m_dwBandWidth));
        }
        if ( HasOption(e_SupportJoin) )
        {
            byTempBool = (u8)m_bSupportJoinedMt;
            CONDITION_RETURN(cNetData.U8Encode(1, &byTempBool));
        }
        return TRUE;
    }
    // 基本数据内容解码
    virtual BOOL32 ItemDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        u8 byTempBool;
        CONDITION_RETURN(cNetData.U8Decode(1, &m_byOptionField));
        if ( HasOption(e_BandWidth) )
        {
            CONDITION_RETURN(cNetData.U32Decode(1, &m_dwBandWidth));
        }
        if ( HasOption(e_SupportJoin) )
        {
            CONDITION_RETURN(cNetData.U8Decode(1, &byTempBool));
            m_bSupportJoinedMt = (byTempBool!=0);
        }
        return TRUE;
    }
    // 得到基本数据类型
    virtual EItemType GetItemType() const
    {
        return ConfShortInfor;
    }
    // 得到基本数据的内容的长度
    virtual u16 GetItemLength() const
    {
        u16 wItemLength = sizeof(m_byOptionField);
        if ( HasOption(e_BandWidth) )
        {
            wItemLength += sizeof(m_dwBandWidth);
        }
        if ( HasOption(e_SupportJoin) )
        {
            wItemLength += sizeof(u8);
        }
        return wItemLength;
    }
}PACKED;


/*====================================================================
类名 ： CMtShortInfor
描述 ： 基本数据：终端简单信息
====================================================================*/
class CMtShortInfor : public CKdvMcuDcsItem
{
public:
    enum EPositionType
    {
        // 下级节点：等待该终端主动加入
        e_Below_Joined,
        // 下级节点：主动邀请该终端
        e_Below_Invite,
        // 上级节点：主动加入该终端上指定的会议
        e_Above_Join,
        // 上级节点：被该终端邀请加入
        e_Above_Invited
    };
    // 终端在会议中的位置类型
    EPositionType m_emPositionType;
private:
    // 可选项情况：如果某一位为1，表示该可选项存在，0表示不存在
        // 第一位 ---- 邀请地址
        // 第二位 ---- 等待加入超时
        // 第三位 ---- 终端设备类型
    u8 m_byOptionField;
public:
    // 终端IP地址【e_Above_Join有效,作为上级节点地址】【e_Below_Joined有效,作为加入请求校验】
    u32 m_dwMtIp;
    // 终端端口【e_Above_Join有效,作为上级节点地址】
    u16 m_wMtPort;
    // 等待时间，单位秒【e_WaitForJoin有效】
    u16 m_wTimeOut;

    enum EDeviceType
    {
        // T120协议中的MCU
        e_Mcu,
        // T120协议中的MT
        e_Mt,
        // T120协议中的Terminal（未使用）
        e_Terminal
    };
    // 终端的设备类型
    EDeviceType m_emDeviceType;

    CMtShortInfor() :
        m_emPositionType(e_Below_Joined), m_byOptionField(0),
        m_dwMtIp(0), m_wMtPort(0), m_wTimeOut(0), m_emDeviceType(e_Mt)
    {
    }
public:
    enum EOption
    {
        e_NodeAddress     = 0x80>>0, // 邀请地址
        e_WaitJoinTimeout = 0x80>>1, // 等待加入超时
        e_DeviceType      = 0x80>>2  // 终端设备类型
    };
    // 加入可选项
    void IncludeOption(IN EOption emOption)
    {
        m_byOptionField |= emOption;
    }
    // 判断是否有可选项
    BOOL32 HasOption(IN EOption emOption) const
    {
        return ( (m_byOptionField&emOption) != 0 );
    }

protected:
    // 基本数据内容编码
    virtual BOOL32 ItemDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        u8 byPositionType = (u8)m_emPositionType;
        CONDITION_RETURN(cNetData.U8Encode(1, &byPositionType));
        CONDITION_RETURN(cNetData.U8Encode(1, &m_byOptionField));

        if ( HasOption(e_NodeAddress) )
        {
            CONDITION_RETURN(cNetData.U32Encode(1, &m_dwMtIp));
            CONDITION_RETURN(cNetData.U16Encode(1, &m_wMtPort));
        }
        if ( HasOption(e_WaitJoinTimeout) )
        {
            CONDITION_RETURN(cNetData.U16Encode(1, &m_wTimeOut));
        }
        if ( HasOption(e_DeviceType) )
        {
            u8 byDeviceType = (u8)m_emDeviceType;
            CONDITION_RETURN(cNetData.U8Encode(1, &byDeviceType));
        }
        return TRUE;
    }
    // 基本数据内容解码
    virtual BOOL32 ItemDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        u8 byPositionType;
        CONDITION_RETURN(cNetData.U8Decode(1, &byPositionType));
        m_emPositionType = (EPositionType)byPositionType;
        CONDITION_RETURN(cNetData.U8Decode(1, &m_byOptionField));

        if ( HasOption(e_NodeAddress) )
        {
            CONDITION_RETURN(cNetData.U32Decode(1, &m_dwMtIp));
            CONDITION_RETURN(cNetData.U16Decode(1, &m_wMtPort));
        }
        if ( HasOption(e_WaitJoinTimeout) )
        {
            CONDITION_RETURN(cNetData.U16Decode(1, &m_wTimeOut));
        }
        if ( HasOption(e_DeviceType) )
        {
            u8 byDeviceType;
            CONDITION_RETURN(cNetData.U8Decode(1, &byDeviceType));
            m_emDeviceType = (EDeviceType)byDeviceType;
        }
        return TRUE;
    }
    // 得到基本数据类型
    virtual EItemType GetItemType() const
    {
        return MtShortInfor;
    }
    // 得到基本数据的内容的长度
    virtual u16 GetItemLength() const
    {
        u16 wItemLength = sizeof(u8) + sizeof(m_byOptionField);
        if ( HasOption(e_NodeAddress) )
        {
            wItemLength += sizeof(m_dwMtIp) + sizeof(m_wMtPort);
        }
        if ( HasOption(e_WaitJoinTimeout) )
        {
            wItemLength += sizeof(m_wTimeOut);
        }
        if ( HasOption(e_DeviceType) )
        {
            wItemLength += sizeof(u8);
        }
        return wItemLength;
    }
}PACKED;

/*====================================================================
类名 ： CMcuMsstateInfor
描述 ： 基本数据：主备MCU数据转换状态信息
====================================================================*/
class CMcuMsstateInfor : public CKdvMcuDcsItem
{
public:
    BOOL32 m_bMcuMsstateInfo;

protected:
    // 基本数据内容编码
    virtual BOOL32 ItemDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        u8 byMcuMsstateInfo = (u8)m_bMcuMsstateInfo;
        CONDITION_RETURN(cNetData.U8Encode(1, &byMcuMsstateInfo));
        return TRUE;
    }
    // 基本数据内容解码
    virtual BOOL32 ItemDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        u8 byMcuMsstateinfo;
        CONDITION_RETURN(cNetData.U8Decode(1, &byMcuMsstateinfo));
        m_bMcuMsstateInfo = (byMcuMsstateinfo !=0 );
        return TRUE;
    }
    // 得到基本数据类型
    virtual EItemType GetItemType() const
    {
        return McuMsstateInfor;
    }
    // 得到基本数据的内容的长度
    virtual u16 GetItemLength() const
    {
        return (sizeof(u8));
    }

}PACKED;


/*====================================================================
-- Part 5 : PDU实现
====================================================================*/

/*====================================================================
类名 ： CDcsMcuRegReqPdu
描述 ： DCS_MCU_REG_REQ数据
====================================================================*/
class CDcsMcuRegReqPdu : public CMcuDcsPDU
{
public:
    // DCS信息
    CDcsInfor m_cDcsInfor;
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return m_cDcsInfor.ItemDecode(cNetData);
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return m_cDcsInfor.ItemEncode(cNetData);
    }
}PACKED;

/*====================================================================
类名 ： CMcuDcsRegAckPdu
描述 ： MCU_DCS_REG_ACK数据
====================================================================*/
class CMcuDcsRegAckPdu : public CMcuDcsPDU
{
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return TRUE;
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return TRUE;
    }
}PACKED;


/*====================================================================
类名 ： CMcuDcsRegNackPdu
描述 ： MCU_DCS_REG_NACK数据
====================================================================*/
class CMcuDcsRegNackPdu : public CMcuDcsPDU
{
public:
    // 错误信息
    CErrorInfor m_cErrorInfor;
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return m_cErrorInfor.ItemDecode(cNetData);
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return m_cErrorInfor.ItemEncode(cNetData);
    }
}PACKED;


/*====================================================================
类名 ： CMcuDcsCreateConfReqPdu
描述 ： MCU_DCS_CREATECONF_REQ数据
====================================================================*/
class CMcuDcsCreateConfReqPdu : public CMcuDcsPDU
{
public:
    // 会议基本信息
    CConfBasicInfor m_cConfBasicInfor;
    // 会议简单信息
    CConfShortInfor m_cConfShortInfor;
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        CONDITION_RETURN(m_cConfBasicInfor.ItemDecode(cNetData));
        CONDITION_RETURN(m_cConfShortInfor.ItemDecode(cNetData));
        return TRUE;
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        CONDITION_RETURN(m_cConfBasicInfor.ItemEncode(cNetData));
        CONDITION_RETURN(m_cConfShortInfor.ItemEncode(cNetData));
        return TRUE;
    }
}PACKED;

/*====================================================================
类名 ： CDcsMcuCreateConfAckPdu
描述 ： DCS_MCU_CREATECONF_ACK数据
====================================================================*/
class CDcsMcuCreateConfAckPdu : public CMcuDcsPDU
{
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return TRUE;
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return TRUE;
    }
}PACKED;

/*====================================================================
类名 ： CDcsMcuCreateConfNackPdu
描述 ： DCS_MCU_CREATECONF_NACK数据
====================================================================*/
class CDcsMcuCreateConfNackPdu : public CMcuDcsPDU
{
public:
    // 错误信息
    CErrorInfor m_cErrorInfor;
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return m_cErrorInfor.ItemDecode(cNetData);
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return m_cErrorInfor.ItemEncode(cNetData);
    }
}PACKED;


/*====================================================================
类名 ： CDcsMcuCreateConfNotifyPdu
描述 ： DCS_MCU_CREATECONF_NOTIFY数据
====================================================================*/
class CDcsMcuCreateConfNotifyPdu : public CMcuDcsPDU
{
public:
    // 会议基本信息
    CConfBasicInfor m_cConfBasicInfor;
    // 会议简单信息
    CConfShortInfor m_cConfShortInfor;
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        CONDITION_RETURN(m_cConfBasicInfor.ItemDecode(cNetData));
        CONDITION_RETURN(m_cConfShortInfor.ItemDecode(cNetData));
        return TRUE;
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        CONDITION_RETURN(m_cConfBasicInfor.ItemEncode(cNetData));
        CONDITION_RETURN(m_cConfShortInfor.ItemEncode(cNetData));
        return TRUE;
    }
}PACKED;

/*====================================================================
类名 ： CMcuDcsReleaseConfReqPdu
描述 ： MCU_DCS_RELEASECONF_REQ数据
====================================================================*/
class CMcuDcsReleaseConfReqPdu : public CMcuDcsPDU
{
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return TRUE;
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return TRUE;
    }
}PACKED;

/*====================================================================
类名 ： CDcsMcuReleaseConfAckPdu
描述 ： DCS_MCU_RELEASECONF_ACK数据
====================================================================*/
class CDcsMcuReleaseConfAckPdu : public CMcuDcsPDU
{
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return TRUE;
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return TRUE;
    }
}PACKED;

/*====================================================================
类名 ： CDcsMcuReleaseConfNackPdu
描述 ： DCS_MCU_RELEASECONF_NACK数据
====================================================================*/
class CDcsMcuReleaseConfNackPdu : public CMcuDcsPDU
{
public:
    // 错误信息
    CErrorInfor m_cErrorInfor;
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return m_cErrorInfor.ItemDecode(cNetData);
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return m_cErrorInfor.ItemEncode(cNetData);
    }
}PACKED;


/*====================================================================
类名 ： CDcsMcuReleaseConfNotifyPdu
描述 ： DCS_MCU_RELEASECONF_NOTIFY数据
====================================================================*/
class CDcsMcuReleaseConfNotifyPdu : public CMcuDcsPDU
{
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return TRUE;
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return TRUE;
    }
}PACKED;

/*====================================================================
类名 ： CMcuDcsAddMtReqPdu
描述 ： MCU_DCS_ADDMT_REQ数据
====================================================================*/
class CMcuDcsAddMtReqPdu : public CMcuDcsPDU
{
public:
    // 终端简单信息
    CMtShortInfor m_cMtShortInfor;
    // 会议简单信息（当终端类型为e_Above_Join时有效）
    CConfBasicInfor m_cConfBasicInfor;
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        CONDITION_RETURN(m_cMtShortInfor.ItemDecode(cNetData));
        if (m_cMtShortInfor.m_emPositionType == CMtShortInfor::e_Above_Join)
        {
            CONDITION_RETURN(m_cConfBasicInfor.ItemDecode(cNetData));
        }
        return TRUE;
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        CONDITION_RETURN(m_cMtShortInfor.ItemEncode(cNetData));
        if (m_cMtShortInfor.m_emPositionType == CMtShortInfor::e_Above_Join)
        {
            CONDITION_RETURN(m_cConfBasicInfor.ItemEncode(cNetData));
        }
        return TRUE;
    }
}PACKED;

/*====================================================================
类名 ： CDcsMcuAddMtAckPdu
描述 ： DCS_MCU_ADDMT_ACK数据
====================================================================*/
class CDcsMcuAddMtAckPdu : public CMcuDcsPDU
{
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return TRUE;
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return TRUE;
    }
}PACKED;

/*====================================================================
类名 ： CDcsMcuAddMtNackPdu
描述 ： DCS_MCU_ADDMT_NACK数据
====================================================================*/
class CDcsMcuAddMtNackPdu : public CMcuDcsPDU
{
public:
    // 错误信息
    CErrorInfor m_cErrorInfor;
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return m_cErrorInfor.ItemDecode(cNetData);
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return m_cErrorInfor.ItemEncode(cNetData);
    }
}PACKED;



/*====================================================================
类名 ： CMcuDcsDelMtReqPdu
描述 ： MCU_DCS_DELMT_REQ数据
====================================================================*/
class CMcuDcsDelMtReqPdu : public CMcuDcsPDU
{
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return TRUE;
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return TRUE;
    }
}PACKED;

/*====================================================================
类名 ： CDcsMcuDelMtAckPdu
描述 ： DCS_MCU_DELMT_ACK数据
====================================================================*/
class CDcsMcuDelMtAckPdu : public CMcuDcsPDU
{
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return TRUE;
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return TRUE;
    }
}PACKED;

/*====================================================================
类名 ： CDcsMcuDelMtNackPdu
描述 ： DCS_MCU_DELMT_NACK数据
====================================================================*/
class CDcsMcuDelMtNackPdu : public CMcuDcsPDU
{
public:
    // 错误信息
    CErrorInfor m_cErrorInfor;
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return m_cErrorInfor.ItemDecode(cNetData);
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return m_cErrorInfor.ItemEncode(cNetData);
    }
}PACKED;


/*====================================================================
类名 ： CDcsMcuMtMtOnlineNotifyPdu
描述 ： DCS_MCU_MT_ONLINE_NOTIFY数据
====================================================================*/
class CDcsMcuMtMtOnlineNotifyPdu : public CMcuDcsPDU
{
public:
    // 终端简单信息
    CMtShortInfor m_cMtShortInfor;
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        CONDITION_RETURN(m_cMtShortInfor.ItemDecode(cNetData));
        return TRUE;
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        CONDITION_RETURN(m_cMtShortInfor.ItemEncode(cNetData));
        return TRUE;
    }
}PACKED;


/*====================================================================
类名 ： CDcsMcuMtMtOfflineNotifyPdu
描述 ： DCS_MCU_MT_OFFLINE_NOTIFY数据
====================================================================*/
class CDcsMcuMtMtOfflineNotifyPdu : public CMcuDcsPDU
{
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return TRUE;
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return TRUE;
    }
}PACKED;


/*====================================================================
类名 ： CDcsMcuGetmsstatusReqPdu
描述 ： DCS_MCU_GETMSSTATUS_REQ数据
====================================================================*/
class CDcsMcuGetmsstatusReqPdu : public CMcuDcsPDU
{
public:
    // DCS信息
    CDcsInfor m_cDcsInfor;
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return m_cDcsInfor.ItemDecode(cNetData);
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return m_cDcsInfor.ItemEncode(cNetData);
    }
}PACKED;


/*====================================================================
类名 ： CMcuDcsGetmsstatusAckPdu
描述 ： MCU_DCS_GETMSSTATUS_ACK数据
====================================================================*/
class CMcuDcsGetmsstatusAckPdu : public CMcuDcsPDU
{
public:
    //主备MCU数据转换状态信息
    CMcuMsstateInfor m_cMcuMsstate;
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return m_cMcuMsstate.ItemDecode(cNetData);
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return m_cMcuMsstate.ItemEncode(cNetData);
    }
}PACKED;

/*====================================================================
类名 ： CMcuDcsGetmsstatusNackPdu
描述 ： MCU_DCS_GETMSSTATUS_NACK数据
====================================================================*/
class CMcuDcsGetmsstatusNackPdu : public CMcuDcsPDU
{
public:
    // 错误信息
    CErrorInfor m_cErrorInfor;
protected:
    // 从网络数据得到本PDU
    virtual BOOL32 PduDataDecode(IN_OUT CKdvMcuDcsNetData& cNetData)
    {
        return m_cErrorInfor.ItemDecode(cNetData);
    }
    // 把本PDU解码为网络数据
    virtual BOOL32 PduDataEncode(IN_OUT CKdvMcuDcsNetData& cNetData) const
    {
        return m_cErrorInfor.ItemEncode(cNetData);
    }
}PACKED;


#ifdef WIN32
    #pragma pack( pop )
#endif

#endif  // _MCU_DCS_PDU_H_
