#ifndef _MON_MSG_H_
#define _MON_MSG_H_

#include "kdvtype.h"
//#include "kdmconst.h"
#include "osp.h"

//#include "kdmstruct.h"

#ifdef _MSC_VER
#pragma pack( push )
#pragma pack( 1 )
#endif

#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif

#define LEN_KDM_NO 31
static u32 g_dwSerialNo = 1; // 流水号

#define MON_GEP_MAX_SERIAL (u32)0xFFFFFFF

#define MON_OSP_MSG_MAX_LEN (u16)(0x7000)

struct TOspMonMsgHead
{
    u32  m_dwSerialNo;                //消息流水号
    
    char m_szSrcID[LEN_KDM_NO + 1]; //源ID
    char m_szDstID[LEN_KDM_NO + 1]; //目的ID
    
    u32 m_dwSessID;                 //会话ID
    
    u16  m_wEventId;                //消息ID
    u16  m_wTimeout;                //消息超时时间
    u32  m_dwErrorCode;              //消息错误码
    u16  m_wMsgBodyLen;             //消息体长度
    
    u32  m_dwReserved1;	            //保留1
    u32  m_dwReserved2;	            //保留2
}
#ifndef WIN32
__attribute__ ((packed))
#endif
;

#define MON_MSG_BODY_OFFSET sizeof(TOspMonMsgHead)

#define MON_OSP_MSG_BODY_MAX_LEN (MON_OSP_MSG_MAX_LEN - MON_MSG_BODY_OFFSET)

class CMonMsg
{
public:
    CMonMsg(void)
    {
        memset(m_abyBuffer, 0, MON_OSP_MSG_MAX_LEN);
        SetSerialNo();
    }
    
    CMonMsg& operator= (CMonMsg& cMsg)
    {
        memcpy(m_abyBuffer, cMsg.m_abyBuffer, cMsg.GetMonMsgLen());
        return (*this);
    }
    
    CMonMsg(u8 * const pbyMsg, u16 wMsgLen)
    {
        memset(m_abyBuffer, 0, MON_OSP_MSG_MAX_LEN);
        
        if( wMsgLen < MON_MSG_BODY_OFFSET || pbyMsg == NULL )
            return;
        
        TOspMonMsgHead *m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        wMsgLen = min( wMsgLen, sizeof( m_abyBuffer ) );
        memcpy( m_abyBuffer, pbyMsg, wMsgLen );
        m_ptMsgHead->m_wMsgBodyLen = htons(wMsgLen - MON_MSG_BODY_OFFSET);
    }
    
//    //交换SrcID和DstID
//    void ReverseID()
//    {
//        TKDMNO tSrcId = GetSrcKdmNo();
//        SetSrcID(GetDstKdmNo().GetNOString());
//        SetDstID(tSrcId.GetNOString());
//    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: ClearMsgHead
    // 描述: 清除消息头部
    // 原型: void ClearMsgHead()
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    void ClearMsgHead(void)                //消息头清零
    {
        memset(m_abyBuffer, 0, MON_MSG_BODY_OFFSET);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: ClearMsgBody
    // 描述: 清除消息体全部内容
    // 原型: void ClearMsgBody()
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    void ClearMsgBody(void)                //清空有效消息体
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return;
        
        memset(&m_abyBuffer[MON_MSG_BODY_OFFSET], 0, ntohs(m_ptMsgHead->m_wMsgBodyLen));
        
        m_ptMsgHead->m_wMsgBodyLen = 0;
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: ClearMsg
    // 描述: 清除整个消息内容
    // 原型: void ClearMsg()
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    void ClearMsg(void)
    {
        memset(m_abyBuffer, 0, MON_OSP_MSG_MAX_LEN);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: GetSerialNo
    // 描述: 获取流水号
    // 原型: U32 GetSerialNo() const
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    u32  GetSerialNo(void) const       //获取流水号信息
    {        
        TOspMonMsgHead *m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return 0;
        return ntohl(m_ptMsgHead->m_dwSerialNo);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: SetSerialNo
    // 描述: 设置当前消息的流水号
    // 原型: void SetSerialNo(u32 dwSerialNo) // [in] 流水号
    // 参数: 
    // [in] dwSerialNo: 当前的流水号
    // 备注: 该方法一般不使用
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    void SetSerialNo(u32 dwSerialNo)    //设置流水号信息
    {
        TOspMonMsgHead *m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead != NULL)
        {
            m_ptMsgHead->m_dwSerialNo = htonl(dwSerialNo);
        }
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: SetSerialNo
    // 描述: 设置消息的流水号
    // 原型: void SetSerialNo()
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    void SetSerialNo()
    {
        TOspMonMsgHead *m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead != NULL)
        {
            m_ptMsgHead->m_dwSerialNo = htonl(g_dwSerialNo++);
            g_dwSerialNo %= MON_GEP_MAX_SERIAL;
        }
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: GetSrcID
    // 描述: 获取Src ID
    // 原型: LPCSTR GetSrcID() const
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    LPCSTR GetSrcID() const
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return "";
        return m_ptMsgHead->m_szSrcID;
    }
//    
//    TKDMNO GetSrcKdmNo() const
//    {
//        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
//        
//        if(m_ptMsgHead == NULL)
//            return TKDMNO();
//        
//        TKDMNO tSrcID;
//        tSrcID.SetNOString(m_ptMsgHead->m_szSrcID);
//        return tSrcID;
//    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: SetSrcID
    // 描述: 设置Src ID
    // 原型: LPCSTR SetSrcID(LPCSTR lpszSrcID)
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    void SetSrcID(LPCSTR lpszSrcID)
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return;
        strncpy(m_ptMsgHead->m_szSrcID, lpszSrcID, sizeof(m_ptMsgHead->m_szSrcID));
        m_ptMsgHead->m_szSrcID[sizeof(m_ptMsgHead->m_szSrcID) - 1] = 0 ;
    }
    
//    void SetSrcID(const TKDMNO& tKdmNo)
//    {
//        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
//        if(m_ptMsgHead == NULL)
//            return;
//        strncpy(m_ptMsgHead->m_szSrcID, tKdmNo.GetNOString(), sizeof(m_ptMsgHead->m_szSrcID));
//        m_ptMsgHead->m_szSrcID[sizeof(m_ptMsgHead->m_szSrcID) - 1] = 0 ;
//    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: GetDstID
    // 描述: 获取Dst ID
    // 原型: LPCSTR GetDstID() const
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    LPCSTR GetDstID() const
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return "";
        return m_ptMsgHead->m_szDstID;
    }
    
//    TKDMNO GetDstKdmNo() const
//    {
//        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
//        
//        if(m_ptMsgHead == NULL)
//            return TKDMNO();
//        
//        TKDMNO tDstID;
//        tDstID.SetNOString(m_ptMsgHead->m_szDstID);
//        return tDstID;
//    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: SetDstID
    // 描述: 设置Dst ID
    // 原型: LPCSTR SetDstID(LPCSTR lpszDstID) const
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    void SetDstID(LPCSTR lpszDstID)
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return;
        strncpy(m_ptMsgHead->m_szDstID, lpszDstID, sizeof(m_ptMsgHead->m_szDstID));
        m_ptMsgHead->m_szDstID[sizeof(m_ptMsgHead->m_szDstID) - 1] = 0;
    }
    
//    void SetDstID(const TKDMNO& tKdmNo)
//    {
//        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
//        if(m_ptMsgHead == NULL)
//            return;
//        strncpy(m_ptMsgHead->m_szDstID, tKdmNo.GetNOString(), sizeof(m_ptMsgHead->m_szDstID));
//        m_ptMsgHead->m_szDstID[sizeof(m_ptMsgHead->m_szDstID) - 1] = 0;
//    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: GetSessID
    // 描述: 获取会话编号
    // 原型: U32 GetSessID() const
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    u32 GetSessID() const
    {        
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return 0;
        return ntohl(m_ptMsgHead->m_dwSessID);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: GetXuiIns
    // 描述: Cmu获取XuiSsn和Xui(如Cui、Pui）连接的XuiSsn实例编号
    // 原型: u32 GetXuiID() const
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    u16 GetXuiIns() const
    {
        u32 dwSessID = GetSessID();
        return (u16)(dwSessID >> 16);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: GetXuiIns
    // 描述: Cmu获取XuiSsn和Xui(如Cui、Pui）连接的XuiSsn实例编号
    // 原型: u32 GetXuiIns() const
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    static u16 GetXuiIns(u32 dwSessId)
    {
        return (u16)(dwSessId >> 16);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: GetXuIns
    // 描述: Xui（如Cui、Pui）获取和Xu（如Cu、Pu）连接的Xui实例编号
    // 原型: u16 GetXuID() const
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    u16 GetXuIns() const
    {        
        u32 dwSessID = GetSessID();
        return ((u16)(dwSessID));
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: GetXuIns
    // 描述: Xui（如Cui、Pui）获取和Xu（如Cu、Pu）连接的Xui实例编号
    // 原型: u16 GetXuID() const
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    static u16 GetXuIns(u32 dwSessId)
    {
        return ((u16)(dwSessId));
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: SetSessID
    // 描述: 设置会话编号
    // 原型: void SetSessID() const
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    void SetSessID(u32 dwSessID)
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return;
        m_ptMsgHead->m_dwSessID = htonl(dwSessID);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: SetSessID
    // 描述: 设置会话编号
    // 原型: void SetSessID() const
    // 参数: u16 wXuInsId:Xu(cu、pu)和Xui(cui、pui)的连接的Xu实例号
    //       u16 wXuiInsId:Cmu中XuiSsn(cuissn、puissn)和Xui（cui、pui）的连接的Xuissn实例号
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    void SetSessID(u16 wXuInsId, u16 wXuiInsId = 0)
    {
        u32 dwSessId = ((wXuiInsId << 16) + wXuInsId);
        SetSessID(dwSessId);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: GetEventID
    // 描述: 获取Event ID
    // 原型: u16 GetEventID() const
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    u16 GetEventID() const
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return 0;
        return ntohs(m_ptMsgHead->m_wEventId);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: SetEventID
    // 描述: 设置Event ID
    // 原型: void SetEventID(u16 wEventID)
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    void SetEventID(u16 wEventID)
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return;
        m_ptMsgHead->m_wEventId = htons(wEventID);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: GetTimeout
    // 描述: 获取超时时间
    // 原型: u16 GetTimeout() const
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    u16 GetTimeout() const
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return 0;
        return ntohs(m_ptMsgHead->m_wTimeout);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: SetTimeout
    // 描述: 设置超时时间
    // 原型: void SetTimeout(u16 wTimeOut)
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    void SetTimeout(u16 wTimeOut)
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return;
        m_ptMsgHead->m_wTimeout = htons(wTimeOut);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: GetErrorCode
    // 描述: 获取错误号
    // 原型: u32 GetErrorCode(void) const
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    u32 GetErrorCode(void) const
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return (u32)~0;
        return ntohl(m_ptMsgHead->m_dwErrorCode);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: SetErrorCode
    // 描述: 设置错误号
    // 原型: void SetErrorCode(u32 dwErrorCode)
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    void SetErrorCode(u32 dwErrorCode)
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return;
        m_ptMsgHead->m_dwErrorCode = htonl(dwErrorCode);
    }
    
    u32 GetReserved1() const
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return 0;
        return ntohl(m_ptMsgHead->m_dwReserved1);
    }
    
    void SetReserved1(u32 dwReserved)
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return;
        m_ptMsgHead->m_dwReserved1 = htonl(dwReserved);
    }
    
    u32 GetReserved2() const
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return 0;
        return ntohl(m_ptMsgHead->m_dwReserved2);
    }
    
    void SetReserved2(u32 dwReserved)
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return;
        m_ptMsgHead->m_dwReserved2 = htonl(dwReserved);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: GetMsgBodyLen
    // 描述: 获取消息体长度
    // 原型: u16 GetMsgBodyLen(void) const
    // 参数: 
    // 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    u16 GetMsgBodyLen(void) const
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return 0;
        return ntohs(m_ptMsgHead->m_wMsgBodyLen);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: GetMsgBody
    // 描述: 获取消息内容
    // 原型: WORD GetMsgBody(   void* pMsgBody, // [in/out] 消息内容
    //                          u16 wBodyLen, // [in] 消息长度
    //                          u16 wOffset) // [in] 消息偏移
    // 参数: 
    // [in/out] pMsgBody: 获取得到的消息内容
    // [in] wBodyLen: 消息的长度
    // [in] wOffset: 消息的偏移
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    u16 GetMsgBody(void* pMsgBody, u16 wBodyLen, u16 wOffset = 0)
    {
        if(pMsgBody == NULL)
        {
            return 0;
        }
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return 0;
        u16 wLen = min(wBodyLen, ntohs(m_ptMsgHead->m_wMsgBodyLen));
        memcpy(pMsgBody, &m_abyBuffer[MON_MSG_BODY_OFFSET + wOffset], wLen);
        return wLen;
    }
    //////////////////////////////////////////////////////////////////////////
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: GetMsgBody
    // 描述: 获取消息内容
    // 原型: void* GetMsgBody(u16 wOffset) // [in] 内容偏移
    // 参数: 
    // [in] wOffset: 消息偏移
    // 备注: 直接返回消息内容指针
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    void* GetMsgBody(u16 wOffset = 0) const
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return NULL;
        if(m_ptMsgHead->m_wMsgBodyLen == 0)
            return NULL;
        return (void*)&m_abyBuffer[MON_MSG_BODY_OFFSET + wOffset];
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: SetMsgBody
    // 描述: 设置消息的内容
    // 原型: void SetMsgBody(void* pMsgBody, // [in] 消息内容
    // u16 wBodyLen) // [in] 内容长度
    // 参数: 
    // [in] pMsgBody: 本次消息的内容
    // [in] wBodyLen: 本次消息的内容长度
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    bool SetMsgBody(const void* pMsgBody, u16 wBodyLen)
    {
        if(pMsgBody != NULL && wBodyLen > 0)
        {			
            if(wBodyLen > MON_OSP_MSG_BODY_MAX_LEN)
            {
                return false;;
            }
            TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
            if(m_ptMsgHead == NULL)
                return false;;
            
            memcpy(&m_abyBuffer[MON_MSG_BODY_OFFSET], pMsgBody, wBodyLen);
            m_ptMsgHead->m_wMsgBodyLen = htons(wBodyLen);
            
            return true;
        }
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: CatMsgBody
    // 描述: 添加消息内容
    // 原型: void CatMsgBody(void* pMsgBody, // [in] 消息内容
    // u16 wBodyLen) // [in] 内容长度
    // 参数: 
    // [in] pMsgBody: 本次添加的消息内容
    // [in] wBodyLen: 内容的长度
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    bool CatMsgBody(const void* pMsgBody, u16 wBodyLen)
    {
        if(pMsgBody != NULL && wBodyLen > 0)
        {
            TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
            if(m_ptMsgHead == NULL)
                return false;
            
            if(wBodyLen > (MON_OSP_MSG_BODY_MAX_LEN - ntohs(m_ptMsgHead->m_wMsgBodyLen)))
            {
                return false;;
            }
            
            memcpy(&m_abyBuffer[MON_MSG_BODY_OFFSET + ntohs(m_ptMsgHead->m_wMsgBodyLen)],
                pMsgBody, wBodyLen);
            m_ptMsgHead->m_wMsgBodyLen = htons(ntohs(m_ptMsgHead->m_wMsgBodyLen) + wBodyLen);
            return true;
        }
        return false;
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: GetMonMsgLen
    // 描述: 获取整个消息长度
    // 原型: u16 GetMonMsgLen(void) const
    // 参数: 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    u16 GetMonMsgLen(void) const
    {
        TOspMonMsgHead* m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        if(m_ptMsgHead == NULL)
            return 0;
        return (ntohs(m_ptMsgHead->m_wMsgBodyLen) + MON_MSG_BODY_OFFSET);
    }
    
    //////////////////////////////////////////////////////////////////////////
    // 名称: SetMonMsg
    // 描述: 设置消息
    // 原型: void SetMonMsg(u8 * const pbyMsg, u16 wMsgLen)
    // 参数: 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2005/12/9 V2.00 luxz 创建 
    //________________________________________________________________________
    void SetMonMsg(u8 * const pbyMsg, u16 wMsgLen)
    {        
        memset(m_abyBuffer, 0, MON_OSP_MSG_MAX_LEN);
        
        if( wMsgLen < MON_MSG_BODY_OFFSET || pbyMsg == NULL )
            return;
        
        TOspMonMsgHead *m_ptMsgHead = (TOspMonMsgHead*)m_abyBuffer;
        wMsgLen = min( wMsgLen, sizeof( m_abyBuffer ) );
        memcpy( m_abyBuffer, pbyMsg, wMsgLen );
        m_ptMsgHead->m_wMsgBodyLen = htons(wMsgLen - MON_MSG_BODY_OFFSET);
    }
    //////////////////////////////////////////////////////////////////////////
    // 名称: SetMonMsg
    // 描述: 设置消息
    // 原型: void SetMonMsg(u8 * const pbyMsg, u16 wMsgLen)
    // 参数: 
    // 备注: 
    //
    // 修改记录
    // 日期 版本 修改人 修改内容
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 2006/03/17 V2.00 王冬林 创建 
    //________________________________________________________________________    
    u8 * const GetMonMsg( void ) const//获取整个消息指针，用户不需提供BUFFER
    {
        return( ( u8 * const )( m_abyBuffer ) );
    }
protected:    
    u8   m_abyBuffer[MON_OSP_MSG_MAX_LEN];		//消息Buffer，28K
};

#ifdef _MSC_VER
#pragma pack( pop )
#endif

#endif
