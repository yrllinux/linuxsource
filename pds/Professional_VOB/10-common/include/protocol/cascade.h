#ifndef H323CASCADE_H_
#define H323CASCADE_H_
#include "osp.h"
#include "kdvtype.h"
#include "time.h"
#include "mcuconst.h"
//#define MMCU_API
//#define MMCU_OK					(u16)0
//#define MMCU_FALSE				(u16)1


//typedef void (*PCascadeHandler)(s32 nMsgID, u8 *pbyBuffer, u32 dwBufLen, u32 dwUserDatad);

//此机构也包含在h323adapter.h里 注意结构同步更新
//#ifndef _H323ADAPTER_H
#ifndef _H323ADAPTER_H

#define CASCADE_PORT		    (s32)3337
#define CASCADE_USERNAME_MAXLEN     (s32)32
#define CASCADE_USERPASS_MAXLEN     (s32)32 
#define CASCADE_NONSTANDARDMSG_MAXLEN (u32)1536
#define PART_NUM			     (s32)192//
#define STR_LEN                  (s32)50
#define STR_LEN_16				 (s32)16  // 16字节，用来保存原来的值的地方TInvitePart [pengguofeng 7/23/2013]
#define CONFID_BYTELEN                   (s32)12// 
#define PID_POS                          (s32)4//
//#define MAX_VIEWS_PER_CONF               (s32)2
#define MAX_VIEWS_PER_CONF               (s32)4
#define MAX_GEOMETRYS_PER_VIEW           (s32)5
#define MAX_SUBFRAMES_PER_GEOMETRY       (s32)16
#define MAX_VIDEOSCHEMES_PER_VIEW        (s32)4
#define MAX_MIXERS_PER_CONF              (s32)16

#define MCU_PID                          (u32)0XFFFFFFFF
#define CACSCONF_ID                      (u32)0XFFFFFFFF
#define EMPTY_MT_PID                     (u32)0x0


#define E_TOO_MANY_ITEMS                 (s32)-1
#define E_ALREADY_EXISTED                (s32)-2
#define E_INVALID_PARAMETER              (s32)-3
#define E_NOT_EXISTED                    (s32)-4
#define E_SUBFRAMES_CONTRIBUTERS_UNMATCH (s32)-5
//#define S_RIGHT                             (s32)1
#define S_REPLACED                       (s32)2
#define S_RIGHT                          (s32)1

enum emConnState
{
	emConnStateUnknown,                 //该MT,MCU的连接状态未知，表明它从未
		emConnStateConnected,               //"connected"
		emConnStateDisconnected,            //"disconnected"
		emConnStateConnecting               //"alerting"
};

enum emEPType
{
	emEPTypeUndefined,                  //不明类型
		emEPTypeMCU,                        //mcu,但是它已一个普通终端的身份参加级联会议或是会议
		emEPTypeTerminal,                   //普通终端
		emEPTypeCascConf                    //mcu,并且参加了级联会议
};


// [8/14/2010 xliang] 
enum  PayloadType 
{
    emPayloadTypeBegin = 0,
        //音频格式
        a_typeBegin,
        a_g711a,
        a_g711u,
        a_g722,
        a_g7231,
        a_g728,
        a_g729,
        a_mp3,
        
        a_g723,
        a_g7221,//代替a_g7221c
        
        // guzh
        a_mpegaaclc = 11,
        a_mpegaacld = 12,
		a_g719,//ruiyigen 20091012
        
        // 保持新老版本兼容
        v_mpeg4 = 17,    
        v_h262 = 18,
        
        a_typeGeneric = 48,   // 内部使用
        
        a_typeEnd = 49,
        
        //视频格式
        v_typeBegin = 50,	
        v_h261,
        v_h263,
        v_h263plus,
        v_h264,
        //v_mpeg4,
        //v_h262,
        v_generic = 97,
        v_extended = 98,
        v_typeEnd = 99,
        
        //数据格式
        d_typeBegin = 100,	
        d_t120,
        d_h224,
        d_typeEnd = 149,
        
        t_h235 = 150, //内部使用
        t_mmcu,       //级联通道
        t_none,       //此通道不支持, 仅用于级联
        t_nonStandard,
        t_huawei,
        
    emPayloadTypeEnd = 255
};

#define PayloardIsAudioType( paylord )  \
( (a_typeBegin < paylord && paylord < a_typeEnd && paylord != v_mpeg4 && paylord != v_h262) )

#define PayloardIsVideoType( paylord )  \
( ( (v_typeBegin < paylord && paylord < v_typeEnd) || paylord == v_mpeg4 || paylord == v_h262) )

#define PayloardIsDataType( paylord )  \
( (d_typeBegin < paylord && paylord < d_typeEnd) )



//分辨率
typedef enum
{
	emResolutionBegin,
		emSQCIF,
		emQCIF,
		emCIF,
		emCIF2,
		emCIF4,    // 576SD: stands for D1: 720*576
		emCIF16,
		
		em352240,
		em704480,  // 480SD
		
		em640480,
		em800600,
		em1024768,
		
		emAUTO,
		emResolutionEnd
}emResolution;

// High Definition Resolutions(1080i is i-mode, others are p-mode)
typedef enum
{    
    emW4CIF = 14,   // Wide4CIF 16:9 1024*576
		emHD720,        // 720   1280*720
		emSXGA,         // SXGA  1280*1024
		emUXGA,         // UXGA  1600*1200
		emHD1080,       // 1080  1920*1080	
		
		em1280x800,
		em1280x768
}emResolutionHD;


#define MCUCASC_ADAPTER_COMMON

#endif	//_H323ADAPTER_H


/*
#define CASCADE_PORT		    (s32)3337
#define CASCADE_USERNAME_MAXLEN     (s32)32
#define CASCADE_USERPASS_MAXLEN     (s32)32 
#define CASCADE_NONSTANDARDMSG_MAXLEN (u32)1536
#define PART_NUM			     (s32)192//
#define STR_LEN                  (s32)16
//#define CONFID_BYTELEN                   (s32)12// 
// #define PID_POS                          (s32)4//

#define MAX_VIEWS_PER_CONF               (s32)4
#define MAX_GEOMETRYS_PER_VIEW           (s32)5
#define MAX_SUBFRAMES_PER_GEOMETRY       (s32)16
#define MAX_VIDEOSCHEMES_PER_VIEW        (s32)4

#define MAX_MIXERS_PER_CONF              (s32)16

// [7/14/2010 xliang] 
#define MCU_PID                          (u32)0XFFFFFFFF
#define CACSCONF_ID                      (u32)0XFFFFFFFF
#define EMPTY_MT_PID                     (u32)0x0

// [7/14/2010 xliang] temp comment
#define E_TOO_MANY_ITEMS                 (s32)-1
#define E_ALREADY_EXISTED                (s32)-2
#define E_INVALID_PARAMETER              (s32)-3
#define E_NOT_EXISTED                    (s32)-4
#define E_SUBFRAMES_CONTRIBUTERS_UNMATCH (s32)-5

#define S_REPLACED                       (s32)2
#define S_RIGHT                          (s32)1
*/


// [7/14/2010 xliang] temp comment
// enum emConnState
// {
// 	    emConnStateUnknown,                 //该MT,MCU的连接状态未知，表明它从未
// 		emConnStateConnected,               //"connected"
// 		emConnStateDisconnected,            //"disconnected"
// 		emConnStateConnecting               //"alerting"
// };

// [7/14/2010 xliang] temp comment
// enum emEPType
// {
//     	emEPTypeUndefined,                  //不明类型
// 		emEPTypeMCU,                        //mcu,但是它已一个普通终端的身份参加级联会议或是会议
// 		emEPTypeTerminal,                   //普通终端
// 		emEPTypeCascConf                    //mcu,并且参加了级联会议
// };
/*
enum H323MCUMsgType
{
    CASCADE_MSGTYPE_BEGINNING,
        
        H_CASCADE_REGUNREG_REQ,   //[IN]&[OUT]:[TRegUnRegReq]	register request
        H_CASCADE_REGUNREG_RSP,   //[IN]&[OUT]:[TRegUnRegRsp]	register response
        
        H_CASCADE_NEWROSTER_NTF,  //[IN]&[OUT]:[TRosterList]  roster notify
        
        H_CASCADE_PARTLIST_REQ,   //[IN]&[OUT]:[TReq]         request part list 
        H_CASCADE_PARTLIST_RSP,   //[IN]&[OUT]:[TPartListRsp] reponse part list (dwpid ==0 表示对方的MCU)
        
        H_CASCADE_INVITEPART_REQ, //[IN]&[OUT]:[TInvitePartReq]  invite part request
        H_CASCADE_INVITEPART_RSP, //[IN]&[OUT]:[Tsp]             invite part response
        H_CASCADE_NEWPART_NTF,    //[IN]&[OUT]:[TPart]           new part notify
        
        H_CASCADE_CALL_ALERTING_NTF, //[IN]&[OUT]:[TCallNtf]
        
        H_CASCADE_REINVITEPART_REQ, //[IN]&[OUT]:[TReInviteReq]    reinvite part request
        H_CASCADE_REINVITEPART_RSP, //[IN]&[OUT]:[Tsp]        reinvite part response
        
        H_CASCADE_DISCONNPART_REQ, //[IN]&[OUT]:[TPartReq]     disconnect part request
        H_CASCADE_DISCONNPART_RSP, //[IN]&[OUT]:[Tsp]         disconnect part response
        H_CASCADE_DISCONNPART_NTF, //[IN]&[OUT]:[TDiscPartNF] disconnect part notify
        
        
        H_CASCADE_DELETEPART_REQ, //[IN]&[OUT]:[TPartReq]		delete part request
        H_CASCADE_DELETEPART_RSP, //[IN]&[OUT]:[Tsp]			delete part response
        H_CASCADE_DELETEPART_NTF, //[IN]&[OUT]:[TPartNtf]   delete part notify (whether or not local)
        
        H_CASCADE_SETIN_REQ,      //[IN]&[OUT]:[TSetInReq]    request video in
        H_CASCADE_SETIN_RSP,      //[IN]&[OUT]:[Tsp]          response msg
        
        H_CASCADE_SETOUT_REQ,     //[IN]&[OUT]:[TSetOutReq]//视频输出到自己的请求(视频流向:MCU->MT)
        H_CASCADE_SETOUT_RSP,     //[IN]&[OUT]:[Tsp]//视频输出到自己的应答(视频流向:MCU->MT)
        
        H_CASCADE_OUTPUT_NTF,     //[IN]&[OUT]:[TPartOutputNtf]      video output notify (dwpid ==0 表示对方的MCU)
        
        H_CASCADE_NEWSPEAKER_NTF, //[IN]&[OUT]:[TNewSpeakerNtf]      newSpeaker output notify (dwpid ==0 表示对方的MCU)
        
        H_CASCADE_AUDIOINFO_REQ,  //[IN]&[OUT]:[TConfVideInfoReq]         audio info request 
        H_CASCADE_AUDIOINFO_RSP,  //[IN]&[OUT]:[TConfAudioInfo]    audio info response
        
        H_CASCADE_VIDEOINFO_REQ,  //[IN]&[OUT]:[pReq]          conference videoinfo request 
        H_CASCADE_VIDEOINFO_RSP,  //[IN]&[OUT]:[TConfVideoInfo] conference videoinfo response
        
        
        H_CASCADE_CONFVIEW_CHG_NTF,//[IN]&[OUT]:[TConfViewChgNtf] conference view format(layout geometry)change notify
        
        H_CASCADE_PARTMEDIACHAN_REQ,   //[IN]&[OUT]:[TPartMediaChanReq] 请求会议成员打开/关闭音/视频/数据通道
        H_CASCADE_PARTMEDIACHAN_RSP,   //[IN]&[OUT]:[TPartRsp]    response
        H_CASCADE_PARTMEDIACHAN_NTF,   //[IN]&[OUT]:[TPartMediaChanNtf] 会议成员的音/视频/数据通道打开/关闭的通知

        H_CASCADE_GETMCUSERVICELIST_REQ, //[IN]&[OUT]:[TReq] 获取MCU上的服务模板列表的请求消息
		H_CASCADE_GETMCUSERVICELIST_RSP,//[IN]&[OUT]:[暂时未定义]  response

		H_CASCADE_GETMUCCONFLIST_REQ,//[IN]&[OUT]:[TMCUConfListReq] 获取MCU上当前召开的所有会议的列表
		H_CASCADE_GETMUCCONFLIST_RSP,//[IN]&[OUT]:[暂时未定义]
  
        H_CASCADE_GETCONFGIDBYNAME_REQ,//[IN]&[OUT]:[TGetConfIDByNameReq] 通过会议名字获得会议ID的请求消息
        H_CASCADE_GETCONFGIDBYNAME_RSP,//[IN]&[OUT]:[暂时未定义]
		
        H_CASCADE_GET_CONFPROFILE_REQ,//[IN]&[OUT]:[TConfProfileReq] 获取MCU上当前召开的所有会议的profile
		
        H_CASCADE_GET_CONFPROFILE_RSP,//[IN]&[OUT]:[暂时未定义]

        H_CASCADE_NONSTANDARD_REQ,//[IN]&[OUT]:[TNonStandardReq] 非标(自定义)请求消息
        H_CASCADE_NONSTANDARD_RSP,//[IN]&[OUT]:[TNonStandardRsp] 非标(自定义)response消息
		H_CASCADE_NONSTANDARD_NTF,//[IN]&[OUT]:[TNonStandardMsg] 非标(自定义)notify消息
		//
        H_CASCADE_NOT_SUPPORTED,
		//目前我们还不支持,但是radmuc支持的消息类型
        H_CASCADE_VA_REQ,       //[IN]&[OUT]:[TVAReq]
        H_CASCADE_VA_RSP,       //[IN]&[OUT]:[TRsp]

        H_CASCADE_RELEASECONTROL_OF_CONF_REQ,//[IN]&[OUT]:[]
        H_CASCADE_RELEASECONTROL_OF_CONF_RSP,//[IN]&[OUT]:[]

        H_CASCADE_TAKECONTROL_OF_CONF_REQ,//[IN]&[OUT]:[]
        H_CASCADE_TAKECONTROL_OF_CONF_RSP,//[IN]&[OUT]:[]

        H_CASCADE_ALLPART_SETOUT_REQ,//[IN]&[OUT]:[]
        H_CASCADE_ALLPART_SETOUT_RSP,//[IN]&[OUT]:[]
        
        H_CASCADE_CREATE_CONF_REQ,//[IN]&[OUT]:[]
        H_CASCADE_CREATE_CONF_RSP,//[IN]&[OUT]:[]
        
        H_CASCADE_TERMINATE_CONF_REQ,//[IN]&[OUT]:[]
        H_CASCADE_TERMINATE_CONF_RSP,//[IN]&[OUT]:[]

        H_CASCADE_UNDEFINED_REQ,//[IN]&[OUT]:[]
        H_CASCADE_UNDEFINED_RSP,//[IN]&[OUT]:[]
        H_CASCADE_UNDEFINED_NTF,//[IN]&[OUT]:[]


        //add 2007.04.28
        H_CASCADE_SET_LAYOUT_AUTOSWITCH_REQUEST,//[IN]&[OUT]:TSetLayoutAutoswitchReq
        H_CASCADE_SET_LAYOUT_AUTOSWITCH_RESPONSE,

        CASCADE_MSGTYPE_ENDING
};
*/

//time config struct
typedef struct tagTimeSpec
{
    s8 day;
    s8 hour;
    s8 minute;
    s8 second;
    s16 year;
    s8 month;
}TTimeSpec,*PTTimeSpec;


typedef struct tagMultiCascadeMsg
{
	u8 m_byCasLevel;
	u8 m_abyMtIdentify[MAX_CASCADELEVEL]; //对应级别中终端的标识 
}TMultiCascadeMsg;

typedef struct tagMultiCascadeInfo
{
	TMultiCascadeMsg m_tMsgSrc;
	TMultiCascadeMsg m_tMsgDst;
}TMultiCascadeInfo;


//标准请求头,他是所有请求的基类
typedef struct tagRequest : public TMultiCascadeInfo
{
    s32  m_nReqID;//流水号/系列号
	s8   m_aszUserName[CASCADE_USERNAME_MAXLEN];//帐号
	s8   m_aszUserPass[CASCADE_USERPASS_MAXLEN];//密码
	//设置帐号和密码
	inline void SetUserNameAndPass(const s8 * aszUserName, const s8 *aszUserPass)
	{
		strncpy(m_aszUserName, aszUserName, CASCADE_USERNAME_MAXLEN-1);
		m_aszUserName[CASCADE_USERNAME_MAXLEN-1] = '\0';
		strncpy(m_aszUserPass, aszUserPass, CASCADE_USERPASS_MAXLEN-1);
		m_aszUserPass[CASCADE_USERPASS_MAXLEN-1] = '\0';
	}
}TReq;


//非标级联消息的基类
typedef struct tagNonStandardMsg
{
	s32 m_nMsgLen;//消息长度
	u8	m_abyMsgBuf[CASCADE_NONSTANDARDMSG_MAXLEN];//消息体
}TNonStandardMsg;

//非标(自定义)请求消息
typedef struct tagNonStandardReq: public TReq, public TNonStandardMsg{}TNonStandardReq;

//response消息的返回值字段
enum emReturnValue
{
    emReturnValue_Ok = 0, //request消息已得到正确的处理
    emReturnValue_Error,//一般性错误，错误原因不明
    emReturnValue_PartNotFound,//会议组中不存在请求消息中PID指示的会议成员
    emReturnValue_ConfNotFound,//请求消息中confID指明的会议组不存在
    emReturnValue_ChannelNotFound,//会议成员的音频/视频通道不存在
    emReturnValue_NoPermission,//用户的权限不够，请求消息不被处理
    emReturnValue_XmlBadParams,//请求消息中字段名或是字段的值类型/范围错误
    emReturnValue_TimeOut,//请求消息处理超时
    emReturnValue_NoResources,//mcu没有足够的资源来满足请求消息对音视频资源的要求
    emReturnValue_Invalid//请求无效,不会被处理
};
typedef struct tagResponse : public TMultiCascadeInfo           //Request消息的回应消息(response消息)
{                           
    s32    m_nReqID;//流水号/系列号,它匹配request中的流水号/系列号
    emReturnValue m_emReturnVal;//request处理结果指示符
    tagResponse()
    {
        m_emReturnVal = emReturnValue_Ok;
        m_nReqID =((s32)time(NULL))%((s32)0x7fffffff);
    }
}TRsp;

//非标(自定义)response消息
typedef struct tagNonStandardRsp: public TRsp, public TNonStandardMsg{}TNonStandardRsp;


//通知类型消息(notification)的基类
typedef struct tagNotification : public TMultiCascadeInfo
{
    s32    m_nCookie;//cookie值
}TNtf;

//获取MCU上的服务列表的请求消息,它向对端mcu请求此mcu上所有已经定义的服务模板的信息
#define TMCUServiceListReq TReq

//获取MCU上当前召开的所有会议相关信息的列表
typedef struct tagMCUConfListReq: public TReq
{
	s32 m_nSnapShot;//MCU快照值
}TMCUConfListReq;

//通过会议名字获得会议ID的请求消息
typedef struct tagGetConfIDByNameReq:public TReq
{
	s8 m_aszConfName[STR_LEN];//会议名称
	s8 m_aszConfPass[STR_LEN];//会议密码,mcu管理者的密码也同样有效
}TGetConfIDByNameReq;

//请求进行会议级联的注册注销请求
typedef struct tagRegUnReg: public TReq     //向对端mcu注册注销的请求消息
{
    BOOL32 m_bReg;					//true:注册;false 注销
	BOOL32 m_bIsSupportMuitiCasade;	//是否支持多级联
}TRegUnRegReq;

//请求进行会议级联的注册/注销的回应消息(成功 m_emReturnVal为true，否则)
typedef struct tagRegUnRegRsp :public TRsp 
{
	BOOL32 m_bReg;  //true:注册;false 注销            
}TRegUnRegRsp;


typedef struct tagPartID           //会议参加者的ID,唯一的表示级连MCU会议中一个参与者
                                  //其中m_dwPID＝0xffffffff表示级连子会议;
{
    u32         m_dwPID;          // part id
    BOOL32      m_bLocal;         // true:会议参加者直接与本地MCU相连 
                                 // false:会议参加者直接与对端MCU相连
    tagPartID ()
    {
        m_dwPID = 0;
        m_bLocal =FALSE;
    }
    inline BOOL32 IsSamePart(tagPartID tPeerPart)
    {
        if(m_dwPID == tPeerPart.m_dwPID && m_bLocal == tPeerPart.m_bLocal)
        {
            return TRUE;
        }
        return FALSE;
    }
    inline BOOL32 IsSamePart(u32 dwPID, BOOL32 bLocal)
    {
        if(m_dwPID == dwPID && m_bLocal == bLocal)
        {
            return TRUE;
        }
        return FALSE;
    }
    inline u32    GetPid() { return m_dwPID ;}
    inline void   SetPid(u32 dwPID) { m_dwPID = dwPID;}
    inline BOOL32 IsLocalPart(){ return m_bLocal;}
    inline BOOL32 IsRemotePart(){ return !m_bLocal;}
    inline BOOL32 IsEmptyPart(){ return m_dwPID == EMPTY_MT_PID;}
    inline void   SetEmpty(){ m_dwPID = EMPTY_MT_PID; m_bLocal = FALSE; }
    inline void   SetIsLocalPart(BOOL32 bLocal = TRUE) { m_bLocal = bLocal; }
    //
    inline BOOL32 IsLocalKdvMCU(){  return (m_dwPID  == CACSCONF_ID && m_bLocal);}
    inline BOOL32 IsRemoteRadMcu(){ return (m_dwPID == CACSCONF_ID && !m_bLocal);}

    inline void SetPartGid(u32 dwPid, BOOL32 bLocal)
    {
        m_dwPID  = dwPid;
        m_bLocal = bLocal;
    }
    
    inline void GetPartGid(u32 *pdwPid, BOOL32 *pbLocal)
    {
        if(pdwPid)
        {
            *pdwPid = m_dwPID;
        }
        if(pbLocal)
        {
            *pbLocal = m_bLocal;
        }
        return;
    }
}TPartID;

//TPartRsp is for multicascade
typedef struct tagPartRsp: public TPartID, public TRsp{}TPartRsp;

/*
*与会者从级连会议中删除的通告，
*/
typedef struct tagDelPartNF : public TPartID, public TNtf
{}TDelPartNF,*PTDelPartNF;

//获取MCU上当前召开的所有会议的profile
//我们暂时未定义它的响应消息的结构
typedef struct tagConfProfileReq:public TReq
{
    s32 m_nSnapShot;//
}TConfProfileReq;

//
typedef struct tagPartReq: public TPartID, public TReq{}TPartReq;

//新发言人(Floor)通知
typedef struct tagNewSpeakerNtf:public TPartID,public TNtf
{
    s32 m_nMixerID;  //Mixer ID
    tagNewSpeakerNtf(){memset(this, 0, sizeof(*this)); }
}TNewSpeakerNtf,*PTNewSpeakerNtf;

//
typedef enum 
{
        emMediaTypeNone,    //unknown,invalid
        emMediaTypeVideo,   //video
        emMediaTypeAudio,   //audio
        emMediaTypeData     //data
}emMediaType;               //媒体类型

typedef enum 
{
    emDirectionNone,       //unknown,invalid
    emDirectionIn,        //从终端到MCU
    emDirectionOut       //从MCU到终端
}emMuteDirection;          //某一类媒体流的流向

typedef struct tagPartMediaChan:public TPartID
{
    emMediaType      m_emMediaType;        //媒体类型
    emMuteDirection  m_emMuteDirection;    //媒体流流动方向
    BOOL32           m_bMute;              //true:切断该方向的流动，false:打开该方向的媒体流动
    tagPartMediaChan(){memset(this, 0, sizeof(*this)); }
}TPartMediaChan; 

//会议成员音/视/数据通道打开/关闭通知
typedef struct tagPartMediaChanNtf:public TPartMediaChan,public TNtf{}TPartMediaChanNtf;

//会议成员音/视/数据通道打开/关闭请求
typedef struct tagPartMediaChanReq:public TPartMediaChan,public TReq{}TPartMediaChanReq;

// 一个从MCU到终端的视频输出规范的定义
typedef struct tagViewOutInfo:public TPartID 
{    
    s32 m_nOutViewID;         //LayoutID,输出的会议视图ID
    s32 m_nOutVideoSchemeID;  //VideoSchemeID,视频输出方案ID
}TViewOutInfo,*PTViewOutInfo;

//请求MCU输出视频流到会议成员
typedef struct tagSetOutReq :public TReq
{
    s32              m_dwPartCount;           //请求MCU输出视频到自己的终端数目
    TViewOutInfo     m_tPartOutInfo[PART_NUM];//视频输出规范的列表，每一个列表元素定义了一个终端的视频输出请求

private:
    
     BOOL32 FindPartViewOutItem(u32 dwPartId, BOOL32 bLocal, s32 &nPartIndex)
     {
         for(nPartIndex =0; nPartIndex < m_dwPartCount; nPartIndex++)
         {
             if(m_tPartOutInfo[nPartIndex].IsEmptyPart())
             {
                 return FALSE;
             }
             if(m_tPartOutInfo[nPartIndex].IsSamePart(dwPartId, bLocal))
             {
                 return TRUE;
             }
         }
         return FALSE;
     }

public: 
    tagSetOutReq(){memset(this, 0, sizeof(*this)); }

    inline s32 GetPartCount(){ return m_dwPartCount;}

    s32 AddNewPartViewOutItem(u32 dwPartId, BOOL32 bLocal, s32 nOutViewID, s32 m_nOutVideoSchemeID)
    {
             s32 nPartIndex = 0;
             if( FindPartViewOutItem(dwPartId, bLocal, nPartIndex))
             {
                 return E_ALREADY_EXISTED;
             }
             if(nPartIndex >= PART_NUM)
             {
                 return E_TOO_MANY_ITEMS;
             }
             m_tPartOutInfo[nPartIndex].m_nOutViewID = nOutViewID;
             m_tPartOutInfo[nPartIndex].m_nOutVideoSchemeID = m_nOutVideoSchemeID;
             m_tPartOutInfo[nPartIndex].SetPartGid(dwPartId, bLocal);
             m_dwPartCount++;
             return S_RIGHT;
    }
    ////////////////
    s32 DelPartViewOutItem(u32 dwPartId, BOOL32 bLocal)
    {
        s32 nPartIndex = 0;
        if(! FindPartViewOutItem(dwPartId, bLocal, nPartIndex))
        {
            return E_NOT_EXISTED;
        }
        for(; nPartIndex < m_dwPartCount-1; nPartIndex++)
        {
            memcpy(&(m_tPartOutInfo[nPartIndex]), 
                   &(m_tPartOutInfo[nPartIndex + 1]), sizeof(TViewOutInfo));
        }
        m_dwPartCount--;
        return S_RIGHT;
    }
    
    s32 GetPartViewOutItem(u32 dwPartId, BOOL32 bLocal,s32 &nViewId, s32 &nVideoSchemeId)
    {
        s32 nPartIndex = 0;
        if(! FindPartViewOutItem(dwPartId, bLocal, nPartIndex))
        {
            return E_NOT_EXISTED;
        }
        nViewId        = m_tPartOutInfo[nPartIndex].m_nOutViewID;
        nVideoSchemeId = m_tPartOutInfo[nPartIndex].m_nOutVideoSchemeID;
        return S_RIGHT;
    }

    s32 GetPartViewOutItem(u32 dwPartId, BOOL32 bLocal,TViewOutInfo &tPartViewOut)
    {
        s32 nPartIndex = 0;
        if(! FindPartViewOutItem(dwPartId, bLocal, nPartIndex))
        {
            return E_NOT_EXISTED;
        }
        tPartViewOut.m_nOutViewID        = m_tPartOutInfo[nPartIndex].m_nOutViewID;
        tPartViewOut.m_nOutVideoSchemeID = m_tPartOutInfo[nPartIndex].m_nOutVideoSchemeID;
        tPartViewOut.m_dwPID             = m_tPartOutInfo[nPartIndex].m_dwPID;
        tPartViewOut.m_bLocal            = m_tPartOutInfo[nPartIndex].m_bLocal;
        return S_RIGHT;
    }

    s32 SetPartViewOutItem(u32 dwPartId, BOOL32 bLocal,s32 nViewId, s32 nVideoSchemeId)
    {
        s32 nPartIndex = 0;
        if(! FindPartViewOutItem(dwPartId, bLocal, nPartIndex))
        {
            return E_NOT_EXISTED;
        }
        m_tPartOutInfo[nPartIndex].m_nOutViewID        = nViewId;
        m_tPartOutInfo[nPartIndex].m_nOutVideoSchemeID = nVideoSchemeId;
        return S_RIGHT;
    }

    s32 SetPartViewOutItem(u32 dwPartId, BOOL32 bLocal,TViewOutInfo &tPartViewOut)
    {
        s32 nPartIndex = 0;
        if(! FindPartViewOutItem(dwPartId, bLocal, nPartIndex))
        {
            return E_NOT_EXISTED;
        }
        m_tPartOutInfo[nPartIndex].m_nOutViewID        = tPartViewOut.m_nOutViewID;
        m_tPartOutInfo[nPartIndex].m_nOutVideoSchemeID = tPartViewOut.m_nOutVideoSchemeID;
    }

    inline TViewOutInfo * GetItemByIndex(s32 nIndex)
    { return (nIndex >= 0 && nIndex < m_dwPartCount) ? &(m_tPartOutInfo[nIndex]) : NULL; }

}TSetOutReq,*PTSetOutReq;

//MCU输出视频流到会议成员的通知
typedef struct tagPartOutputNtf:public TPartID,public TNtf
{
    s32 m_nViewID; //终端输出到的会议视图ID
    inline tagPartOutputNtf()
    {
        m_nViewID = m_dwPID = 0;
        m_bLocal = FALSE;
    }
}TPartOutputNtf, *PTPartOutPutNtf;


//会议花名册的一项,表示一个与会成员的基本信息
typedef struct tagTRosterItem:public TPartID
{
    emConnState  m_emConnState;    //连接状态
}TRosterItem, *PTRosterItem;

//会议花名册通知
typedef struct tagRosterList:public TNtf
{
    s32          m_nCount;               //花名册记录条数目
    TRosterItem  m_atRoster[PART_NUM];   //记录条列表
private: 
    BOOL32 FindRosterItem(u32 dwPartId, BOOL32 bLocal, s32 &nIndex)
    {
        for(nIndex =0; nIndex < m_nCount; nIndex++)
        {
            if(m_atRoster[nIndex].IsEmptyPart())
            {
                return FALSE;
            }
            if(m_atRoster[nIndex].IsSamePart(dwPartId, bLocal))
            {
                return TRUE;
            }
        }
        return FALSE;
    }

public:

    inline tagRosterList(){ memset(this, 0, sizeof(*this));}

    BOOL32 AddRosterItem(u32 dwPartId, BOOL32 bLocal, emConnState emState)
    {
        s32  nRosterIndex = 0;
        if( FindRosterItem(dwPartId, bLocal, nRosterIndex))
        {
            return E_ALREADY_EXISTED;
        }

        if( nRosterIndex >= PART_NUM)
        {
            return E_TOO_MANY_ITEMS;
        }
        m_atRoster[nRosterIndex].m_dwPID = dwPartId;
        m_atRoster[nRosterIndex].m_bLocal = bLocal;
        m_atRoster[nRosterIndex].m_emConnState = emState;
        m_nCount ++;
        return S_RIGHT;
    }
    
    BOOL32 DelRosterItem(u32 dwPartId, BOOL32 bLocal, emConnState emState)
    {
        s32  nRosterIndex = 0;
        if(! FindRosterItem(dwPartId, bLocal, nRosterIndex))
        {
            return E_NOT_EXISTED;
        }
        for(; nRosterIndex < m_nCount-1; nRosterIndex++)
        {
            memcpy(&(m_atRoster[nRosterIndex]), 
                &(m_atRoster[nRosterIndex + 1]), sizeof(TRosterItem));
        }
        m_nCount--;
        return S_RIGHT;
    } 
    
    inline TRosterItem * GetItemByIndex(s32 nIndex)
    { return (nIndex >= 0 && nIndex < m_nCount) ? &(m_atRoster[nIndex]) : NULL; }

}TRosterList, *PTRosterList;

//会议视图子窗口的位置标志
typedef struct 
{
    s32  m_nViewID;//会议视图ID
    u8   m_bySubframeIndex;//子窗口在会议视图中位置(按照从左到右从上到下的顺序从0开始计数)
}TViewPos;

//(voice activate)语音激活请求,把当前说话人/或者音量最大的会议成员的视频放置在视图的那个子窗口中
typedef struct tagVAReq:public TReq, public TViewPos{}TVAReq;

//会议成员的视频输出输入信息
typedef struct tagPartVideoInfo
{
    s32       m_nViewCount;                    //终端(会议成员)的视频总共输出到的视图数目
    TViewPos  m_anViewPos[MAX_VIEWS_PER_CONF]; //终端在哪些视图里
    s32       m_nOutputViewID;                 //终端接收哪个视图
    s32       m_nOutVideoSchemeID;             //输入到终端的视频方案ID(当一个会议支持两种会议格式)                        
    
private:
    BOOL32 FindViewPos(s32 nViewID, u8 bySubframeIndex, s32 &nIndex)
    {
        for(nIndex =0; nIndex < m_nViewCount; nIndex++)
        {
            if(m_anViewPos[nIndex].m_nViewID == 0)
            {
                return FALSE;
            }
            if(m_anViewPos[nIndex].m_nViewID == nViewID && m_anViewPos[nIndex].m_bySubframeIndex ==bySubframeIndex)
            {
                return TRUE;
            }
        }
        return FALSE;
    }

public:
    tagPartVideoInfo()
    {
        memset(this,0,sizeof(*this));
    }
    BOOL32 AddViewPos(s32 nViewID,u8 bySubframeIndex)
    {
        s32  nVPosIndex = 0;
        if(nViewID == 0)
        {
            return S_RIGHT;
        }
        if(FindViewPos(nViewID, bySubframeIndex, nVPosIndex))
        {
            return E_ALREADY_EXISTED;
        }
        
        if( nVPosIndex >= MAX_VIEWS_PER_CONF)
        {
            return E_TOO_MANY_ITEMS;
        }
        
        m_anViewPos[nVPosIndex].m_nViewID         = nViewID;
        m_anViewPos[nVPosIndex].m_bySubframeIndex = bySubframeIndex;

        m_nViewCount ++;
        return S_RIGHT;
    }
   
    BOOL32 DelViewPos(s32 nViewID,u8 bySubframeIndex)
    {
        s32  nVPosIndex = 0;
        if(! FindViewPos(nViewID, bySubframeIndex, nVPosIndex))
        {
            return E_NOT_EXISTED;
        }
        for(; nVPosIndex < m_nViewCount-1; nVPosIndex++)
        {
            memcpy(&(m_anViewPos[nVPosIndex]), 
                &(m_anViewPos[nVPosIndex + 1]), sizeof(TViewPos));
        }
        m_nViewCount--;
        return S_RIGHT;
    }

    BOOL32 IsViewPosExist(s32 nViewID, u8 bySubframeIndex)
    {
        s32  nVPosIndex = 0;
        if(! FindViewPos(nViewID, bySubframeIndex, nVPosIndex))
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }

    inline void SetVideoSpec(s32 nNewLID, s32 nNewSchemeID, s32 *nOldLID = NULL, s32 *nOldSchemeID = NULL)
    {
        if(nOldLID)
        {
            *nOldLID = m_nOutputViewID;
        }
        if(nOldSchemeID)
        {
            *nOldSchemeID = m_nOutVideoSchemeID;
        }
        m_nOutputViewID        = nNewLID;
        m_nOutVideoSchemeID = nNewSchemeID;
        return;
    }

    inline void GetVideoSpec(s32 *pnLID, s32 *pnSchemeID)
    {
        if(pnLID)
        {
            *pnLID = m_nOutputViewID;
        }
        if(pnSchemeID)
        {
            *pnSchemeID = m_nOutVideoSchemeID;
        }
    }

}TPartVideoInfo,*PTPartVideoInfo;


/*
 *与会者的视频，音频输入输出的统计信息，包括（带宽，发送/接受的包数目，乱序包数目，
 抖动(jitter)缓冲等,目前没有具体定义
*/
typedef struct tagPartAVStatis
{
  u32 m_dwTotalInBandWidth;
  u32 m_dwTotalOutBandWidth;
  u32 m_dwUnused_AudioIn;
  u32 m_dwUnused_AudioOut;
  u32 m_dwUnused_VideoIn;
  u32 m_dwUnused_VideoOut;
  // Currently We Define the bandwith with fixed value
  // because we did not decide to get extra info about part
  // Once we change to get the extra info, we should rewrite this code here.
  tagPartAVStatis()
  {
      m_dwTotalInBandWidth = 437720;
      m_dwTotalOutBandWidth = 437720; 
      m_dwUnused_AudioIn  = 0;
      m_dwUnused_AudioOut = 0;
      m_dwUnused_VideoIn  = 0;
      m_dwUnused_VideoOut = 0;
      
  }
}TPartAVStatis;


/*
 *与会者的信息
*/
typedef struct tagPart:public TPartID
{
    s8			m_aszPartName[STR_LEN];     //part name (长度:16Byte) 
    s8			m_aszDialStr[STR_LEN];      //dial string (长度:16Byte)
    u32			m_dwIP;            //part ip
    emEPType	m_emEPType;        //part type
    s8          m_aszEPDesc[STR_LEN];       //part des(product id)
    PayloadType m_emVideoIn;       //video in format (H263、G711.....None)
    PayloadType m_emVideoOut;      //.....
    PayloadType m_emVideo2In;      //.....
    PayloadType m_emVideo2Out;     //..... 
    PayloadType m_emAudioIn;       //.....
    PayloadType m_emAudioOut;      //..... 
    BOOL32      m_bDataMeeting;    //data meeting  
    BOOL32      m_bVideoMuteIn;    //whether or not video in mute(no camera)
    BOOL32      m_bVideoMuteOut;   //....(no dispaly)
    BOOL32      m_bAudioMuteIn;    //....(no mic)
	BOOL32      m_bAudioMuteOut;   //....(no audio player)
    BOOL32      m_bInvited;        //whether or not is Invited
    BOOL32      m_bMaster;         //whether or not Master ternial
    TTimeSpec   m_tStartTime;      //call start time
    emConnState m_emConnState;     //connect state
    BOOL32      m_bFECCEnable;     //whether or not enable FECC
    TPartVideoInfo m_tPartVideoInfo;  //视频输出输入信息
    TPartAVStatis  m_tAVStatis;    //视频音频输入输出统计信息
	u8          m_byCasLevel; //相对与本级，终端所处级联树中的相对级别
	u8          m_abyMtIdentify[MAX_CASCADELEVEL]; //对应级别中终端的标

    tagPart()
    {
        memset(this, 0, sizeof(*this));
    }
    inline void SetVideoAttr(PayloadType emVideoIn, PayloadType emVideoOut, 
                                PayloadType emVideo2In, PayloadType emVideo2Out, 
                                BOOL32 bVideoMuteIn, BOOL32 bVideoMuteOut)
    {
         m_emVideoIn      = emVideoIn;      
         m_emVideoOut     = emVideoOut; 
         m_emVideo2In      = emVideo2In;      
         m_emVideo2Out     = emVideo2Out; 
         m_bVideoMuteIn   = bVideoMuteIn;    
         m_bVideoMuteOut  = bVideoMuteOut;   
    }

    inline void GetVideoAttr(PayloadType *pemVideoIn, PayloadType *pemVideoOut, 
                                PayloadType *pemVideo2In, PayloadType *pemVideo2Out,
                                BOOL32 *pbVideoMuteIn, BOOL32 *pbVideoMuteOut)
    {
        if(pemVideoIn)
        { 
            *pemVideoIn = m_emVideoIn;
        }
        if(pemVideoOut)
        { 
            *pemVideoOut = m_emVideoOut; 
        }
        if(pemVideo2In)
        { 
            *pemVideo2In = m_emVideo2In;
        }
        if(pemVideo2Out)
        { 
            *pemVideo2Out = m_emVideo2Out; 
        }
        if(pbVideoMuteIn)
        { 
            *pbVideoMuteIn = m_bVideoMuteIn;
        }
        if(pbVideoMuteOut)
        { 
            *pbVideoMuteOut = m_bVideoMuteOut;
        }
        return;
    }
    
    inline void SetViewInAndOutAttr(TPartVideoInfo &tPartVideoInfo)
    {
        memcpy(&m_tPartVideoInfo, &tPartVideoInfo,sizeof(TPartVideoInfo));
    }

    inline TPartVideoInfo* GetViewInAndOutAttr()
    {
        return &m_tPartVideoInfo;
    }
    
    inline void SetAudioAttr(PayloadType emAudioIn, PayloadType emAudioOut, BOOL32 bAudioMuteIn, BOOL32 bAudioMuteOut)
    {
        m_emAudioIn      = emAudioIn;      
        m_emAudioOut     = emAudioOut;     
        m_bAudioMuteIn   = bAudioMuteIn;    
        m_bAudioMuteOut  = bAudioMuteOut;   
    }

    inline void GetAudioAttr(PayloadType *pemAudioIn, PayloadType *pemAudioOut, BOOL32 *pbAudioMuteIn, BOOL32 *pbAudioMuteOut)
    {
        if(pemAudioIn)
        { 
            *pemAudioIn = m_emAudioIn;
        }
        if(pemAudioOut)
        { 
            *pemAudioOut = m_emAudioOut; 
        }
        if(pbAudioMuteIn)
        { 
            *pbAudioMuteIn = m_bAudioMuteIn;
        }
        if(pbAudioMuteOut)
        { 
            *pbAudioMuteOut = m_bAudioMuteOut;
        }
        return;
    }

    inline void SetNameAttr(const s8 *pszName, s32 nNameLen, const s8 *pszDescr, s32 nDescrLen)
    {
        // assert(pszName != NULL);
        s32 nStrLen = (nNameLen >= STR_LEN ? STR_LEN -1 : nNameLen );
        strncpy(m_aszPartName, pszName, nStrLen);
        m_aszPartName[nStrLen] = '\0';

        if(pszDescr != NULL && nDescrLen > 0)
        {
            nStrLen = (nDescrLen >= STR_LEN ? STR_LEN -1 : nDescrLen );
            strncpy(m_aszDialStr, pszDescr, nStrLen);
            m_aszDialStr[nStrLen] = '\0';
        }
        else
        {
            m_aszDialStr[0] = '\0';
        }
    }

    inline void GetNameAttr(s8 *pszName, s32 nNameLen, s8 *pszDescr, s32 nDescrLen)
    {
        if(pszName != NULL && nNameLen > 0)
        {
            s32 nStrLen = (nNameLen >= STR_LEN ? STR_LEN -1 : nNameLen );
            strncpy(pszName, m_aszPartName, nStrLen);
            pszName[nStrLen] = '\0';  
        }

        if(pszDescr != NULL && nDescrLen > 0)
        {
            s32 nStrLen = (nDescrLen >= STR_LEN ? STR_LEN -1 : nDescrLen);
            strncpy(pszDescr, m_aszDialStr, nStrLen);     
            pszDescr[nStrLen] = '\0';
        }
    }

    inline void SetDialStr(const s8 *pszDialStr, s32 nLen)
    {
       if(pszDialStr !=NULL && nLen >0)
       {
           s32 nStrLen = (nLen >= STR_LEN ? STR_LEN -1 : nLen);
           strncpy(m_aszDialStr, pszDialStr, nStrLen);
           m_aszDialStr[nStrLen] = '\0';
       }
    }

    inline s8 * GetDialStr() { return this->m_aszDialStr; }

    inline void SetEntityAttr(emConnState emState, emEPType emEntityType, BOOL32 bFECCEnable, BOOL32 bMaster, BOOL32 bInvited)
    {
        m_emConnState    = emState;  
        m_emEPType       = emEntityType;
        m_bFECCEnable    = bFECCEnable;     
        m_bMaster        = bMaster;       
        m_bInvited       = bInvited;       
    }
    inline BOOL32 IsFECCEnable(){ return m_bFECCEnable;}
    inline BOOL32 IsMaster() { return m_bMaster;}
    inline BOOL32 IsInvited() { return m_bInvited;}
    inline emConnState GetConnState() { return m_emConnState;}
    inline void   SetConnState(emConnState emState) { m_emConnState = emState;}
    inline void   StopAsVideoSrc() { m_tPartVideoInfo.m_nViewCount = 0;}
    inline BOOL32 IsVideoSrc() { return (m_tPartVideoInfo.m_nViewCount > 0);}
    inline u32    GetTotalBandWith() { return m_tAVStatis.m_dwTotalInBandWidth + m_tAVStatis.m_dwTotalOutBandWidth;}

    inline void GetEntityAttr(emConnState *pemState, BOOL32 *pbFECCEnable, BOOL32 *pbMaster, BOOL32 *pbInvited)
    {
        if(pemState)
        { 
            *pemState = m_emConnState;
        }
        if(pbFECCEnable)
        { 
            *pbFECCEnable = m_bFECCEnable; 
        }
        if(pbMaster)
        { 
            *pbMaster = m_bMaster;
        }
        if(pbInvited)
        { 
            *pbInvited = m_bInvited;
        }
        return;
    }
    inline void GetVideoScheme(s32 *pnViewId, s32 *pnVideoSchemeId)
    {
        m_tPartVideoInfo.GetVideoSpec(pnViewId,pnVideoSchemeId);
    }
    inline void SetVideoScheme(s32 nViewId, s32 nVideoSchemeId)
    {
        m_tPartVideoInfo.SetVideoSpec(nViewId, nVideoSchemeId, NULL, NULL);
    }
}TPart,*PTPart;

//级连会议与会者的信息
typedef struct tagConfPartInfo
{
    s32    m_dwPartCount;       //fact part count
    s32    m_nSnapShot;         //会议的快照值
        //The first Element Denotes MCU, The Record series from second Denote Terminals 
    TPart  m_atParts[PART_NUM]; // part list 
 private:
     BOOL32 FindPart(u32 dwPartId, BOOL32 bLocal, s32 &nIndex,u8 byCasLevel,u8* pbyMtIdentify )
     {
		 u8 byIdx = 0;
		 u8 byIsEqual = 0;
         for(nIndex =0; nIndex < m_dwPartCount; nIndex++)
         {
             if(m_atParts[nIndex].IsEmptyPart())
             {
                 return FALSE;
             }
             if(m_atParts[nIndex].IsSamePart(dwPartId, bLocal))
             {
				 if( m_atParts[nIndex].m_byCasLevel == byCasLevel &&
					 0 != m_atParts[nIndex].m_byCasLevel &&
					 NULL != pbyMtIdentify )
				 {
					byIsEqual = 1;
					for( byIdx = 0;byIdx < byCasLevel;++byIdx )
					{
						if( m_atParts[nIndex].m_abyMtIdentify[byIdx] != pbyMtIdentify[byIdx] )
						{
							byIsEqual = 0;
							break;
						}
					}
				 }
				 if( 1 == byIsEqual )
				 {
					return TRUE;
				 }
             }
         }
         return FALSE;
     }
     
 public:
     void Clear()
     {
         memset(this, 0, sizeof(*this));
     }
     tagConfPartInfo()
     {
         memset(this, 0, sizeof(*this));
     }
     s32 AddPart(TPart &tPart, BOOL32 bReplace = TRUE)
     {
         s32  nPartIndex = 0;
         if(FindPart(tPart.m_dwPID, tPart.m_bLocal, nPartIndex,tPart.m_byCasLevel,&tPart.m_abyMtIdentify[0]))
         {
             if(bReplace)
             {
                 memcpy(&m_atParts[nPartIndex], &tPart, sizeof(TPart));
                 return S_RIGHT;
             }
             else
             {
                 return E_ALREADY_EXISTED;
             }
         }
         if( nPartIndex >= PART_NUM)
         {
             return E_TOO_MANY_ITEMS;
         }
         
         memcpy(&m_atParts[nPartIndex], &tPart, sizeof(TPart));
         m_dwPartCount ++;
         return S_RIGHT;
     }

    s32 DelPart(u32 dwPartId, BOOL32 bLocal,u8 byCasLevel,u8* pbyMtIdentify)
     {
         s32 nPartIndex = 0;
         if(! FindPart(dwPartId, bLocal, nPartIndex,byCasLevel,pbyMtIdentify))
         {
             return E_NOT_EXISTED;
         }
         for(; nPartIndex < m_dwPartCount-1; nPartIndex++)
         {
             memcpy(&(m_atParts[nPartIndex]), 
                 &(m_atParts[nPartIndex + 1]), sizeof(TPart));
         }
         m_dwPartCount--;
         return S_RIGHT;
     }
    
     s32 Disconnect(u32 dwPartId, BOOL32 bLocal, u32 dwNewPartId,u8 byCasLevel,u8* pbyMtIdentify)
     {
         s32 nPartIndex = 0;
         if(! FindPart(dwPartId, bLocal, nPartIndex,byCasLevel,pbyMtIdentify))
         {
             return E_NOT_EXISTED;
         }
         if(m_atParts[nPartIndex].GetConnState() == emConnStateDisconnected)
         {
             return E_ALREADY_EXISTED;
         }
         m_atParts[nPartIndex].m_dwPID = dwNewPartId;
         m_atParts[nPartIndex].SetConnState(emConnStateDisconnected);
         memset(&(m_atParts[nPartIndex].m_tPartVideoInfo), 0, sizeof(TPartVideoInfo));
         
         return S_RIGHT;
     }
    
     TPart *GetPart(u32 dwPartId, BOOL32 bLocal,u8 byCasLevel,u8* pbyMtIdentify)
     {
         s32 nPartIndex = 0;
         if(! FindPart(dwPartId, bLocal, nPartIndex,byCasLevel,pbyMtIdentify))
         {
             return NULL;
         }
         return &(m_atParts[nPartIndex]);
     }

     inline TPart *GetLocalMCU()
		 
     {
         return GetPart(CACSCONF_ID, TRUE,0,NULL);
     }

     inline TPart *GetPeerMCU()
     {
         return GetPart(CACSCONF_ID, FALSE,0,NULL);
     }

     BOOL32 IsPartExist(u32 dwPartId, BOOL32 bLocal,u8 byCasLevel,u8* pbyMtIdentify)
     {
         s32 nPartIndex = 0;
         if(! FindPart(dwPartId, bLocal, nPartIndex,byCasLevel,pbyMtIdentify))
         {
             return FALSE;
         }
         return TRUE;
     }
     
     inline TPart *GetItemByIndex(s32 nPosFrom)
     {
         return (nPosFrom >= 0 && m_dwPartCount > nPosFrom ? &m_atParts[nPosFrom] : NULL);
     }
     
}TConfPartInfo;

//对方MCU级连子会议的回应列表
typedef struct tagPartListRsp :public TConfPartInfo,public TRsp
{
   BOOL32 m_bLastPack;			//whether or not last pack，used only while receiving
   public:
    tagPartListRsp() { m_emReturnVal = emReturnValue_Ok; m_bLastPack = TRUE;}
    void Clear()
    { 
        memset(this, 0, sizeof(*this));
        m_emReturnVal = emReturnValue_Ok; 
        m_bLastPack = TRUE;
    }
    void BuildToPartListRsp(TConfPartInfo &tConfPart)
    {
        s32 nMemSize = 0;
        nMemSize = tConfPart.m_dwPartCount * sizeof(TPart);
        memcpy(&m_atParts[0], &tConfPart.m_atParts[0], nMemSize);
        m_nSnapShot = tConfPart.m_nSnapShot;
        m_dwPartCount = tConfPart.m_dwPartCount ;
        m_bLastPack = TRUE;
        m_emReturnVal = emReturnValue_Ok;
    }
    
    BOOL32 ExtractConfPartList(TConfPartInfo &tConfPart)
    {
        s32 nIndex      = 0;
        while( nIndex < m_dwPartCount)
        {
            tConfPart.AddPart(m_atParts[nIndex]);
            nIndex ++;
        }
        if(m_bLastPack)        
        {
            tConfPart.m_nSnapShot = m_nSnapShot;
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
}TPartListRsp,*PTPartListRsp;


//PartList消息收发头结构，后面跟m_dwPartCount个TPart结构
typedef struct tagPartListMsg: public TRsp
{
    s32    m_dwPartCount;       //fact part count
    s32    m_nSnapShot;         //会议的快照值
    BOOL32 m_bLastPack;         //whether or not last pack，used only while receiving
} TPartListMsg,*PTPartListMsg;


//邀请终端加入会议的参数
typedef struct tagInvitePart
{
    s8   m_aszDialStr[STR_LEN_16 + MAXLEN_CONFNAME];   //dial string  终端别名长度由16字节扩容至80字节
    u32  m_dwCallRate;                //call rate (kbps)
    
    //optional,advanced invitation parameter
    TPartVideoInfo m_tPartVideoInfo;
    
    tagInvitePart()
    {
        memset(this, 0, sizeof(*this));
    }
    
    void SetBasicInviteeParam(const s8 *pszDialStr, s32 nStrLen, u32 dwCallRate)
    {
        if(pszDialStr != NULL && nStrLen >0)
        {
            nStrLen = (nStrLen >= STR_LEN_16 + MAXLEN_CONFNAME ? STR_LEN_16 + MAXLEN_CONFNAME -1 : nStrLen );
            strncpy(m_aszDialStr, pszDialStr, nStrLen);
            m_aszDialStr[nStrLen] = '\0';           
        }
        m_dwCallRate = dwCallRate;
    }

    void GetBasicInviteeParam(s8 *pszDialStr, s32 nStrLen, u32 *pdwCallRate)
    {
        if(pszDialStr != NULL && nStrLen >0)
        {
            nStrLen = (nStrLen >= STR_LEN_16 + MAXLEN_CONFNAME ? STR_LEN_16 + MAXLEN_CONFNAME -1 : nStrLen );
            strncpy(pszDialStr, m_aszDialStr, nStrLen);
            pszDialStr[nStrLen] = '\0';
        }
        if(pdwCallRate)
        {
            *pdwCallRate = m_dwCallRate;
        }
        return;
    }

    inline void SetInviteeSetInAndOutAttr(TPartVideoInfo &tPartVideoInfo)
    {
        memcpy(&m_tPartVideoInfo, &tPartVideoInfo, sizeof(TPartVideoInfo));
    }

    inline TPartVideoInfo GetInviteeSetInAndOutAttr()
    {
       return m_tPartVideoInfo;
    }
  
}TInvitePart,*PTInvitePart;


/*
  邀请对象加入级连会议的消息体  
*/
typedef struct tagInvitePartReq: public TReq,public TPartID
{
	s32          m_nCount;           //同时受要求的对象的数目
    TInvitePart  m_atPart[PART_NUM]; //受邀请对象信息
public:
     tagInvitePartReq()
     {
        memset(this, 0, sizeof(*this));
     }

     inline BOOL32 AddInvitee(TInvitePart &tInvitee)
     {
         if(m_nCount >= PART_NUM)
         {
             return FALSE;
         }
         memcpy(&m_atPart[m_nCount], &tInvitee,sizeof(TInvitePart));
         m_nCount++;
         return TRUE;
     }

     inline TInvitePart *GetItemByIndex(s32 nPos)
     { return (nPos >= 0 && nPos < m_nCount) ? &(m_atPart[nPos]) : NULL; }

}TInvitePartReq;

//重新要求已经断开的终端加入会议
typedef struct tagReinvitePartReq: public TPartID, public TReq{}TReInvitePartReq;

//MCU_MCU_CALL_ALERTING_NTF 消息体
typedef struct tagCallNtf : public TPartID, public TNtf
{
	s8   m_aszDialStr[STR_LEN];   //dial string (终端别名为16字节)
   
    tagCallNtf()
    {
        memset(this, 0, sizeof(*this));
    }
    
    void inline GetPartCallingParam(s32 *pdwPartId, BOOL32 *pbLocal,s8 *pszDialStr, s32 nStrLen)
    {
        if(pdwPartId)
        {
            *pdwPartId = m_dwPID;
        }
        if(pbLocal)
        {
            *pbLocal = m_bLocal;
        }
        if(pszDialStr != NULL && nStrLen >0)
        {
            nStrLen = (nStrLen >= STR_LEN ? STR_LEN - 1 : nStrLen );
            strncpy(pszDialStr, m_aszDialStr, nStrLen);
            pszDialStr[nStrLen] = '\0';
        }
    }

    void inline SetPartCallingParam(u32 dwPartId, BOOL32 bLocal,s8 *pszDialStr, s32 nStrLen)
    {
        m_dwPID  = dwPartId;
        m_bLocal = bLocal;
        if(pszDialStr != NULL && nStrLen >0)
        {
            nStrLen = (nStrLen >= STR_LEN ? STR_LEN - 1 : nStrLen );
            strncpy(m_aszDialStr, pszDialStr, nStrLen);
            //strncpy(pszDialStr, m_aszDialStr, nStrLen);
            m_aszDialStr[nStrLen] = '\0';
        }
    }
}TCallNtf;

//请求终端输出图像到会议的某个视图
typedef struct tagSetInReq:public TPartID, public TReq
{
    s32 m_nViewID;          //图像输出到达的会议视图
    u8  m_bySubFrameIndex;  //会议视图中的子图像(subframe)索引
    tagSetInReq()
    {
        memset(this, 0, sizeof(*this));
    }
    
    void SetPartSetInParam(u32 dwPartId, BOOL32 bLocal, s32 nViewID, u8 bySubFrameIndex)
    {
        m_dwPID =  dwPartId;
        m_bLocal = bLocal;
        m_nViewID = nViewID;
        m_bySubFrameIndex = bySubFrameIndex;
    }

    void GetPartSetInParam(s32 *pdwPartId, BOOL32 *pbLocal, s32 *pnViewID, u8 *pbySubFrameIndex)
    {
        if(pdwPartId)
        {
            *pdwPartId = m_dwPID;
        }
        if(pbLocal)
        {
            *pbLocal = m_bLocal;
        }
        if(pnViewID)
        {
            *pnViewID = m_nViewID;
        }
        if(pbySubFrameIndex)
        {
            *pbySubFrameIndex = m_bySubFrameIndex;
        }
    }
    
}TSetInReq;

/*
 *与会者从级连会议中断开的通告，
*/
typedef struct tagDiscPartNF : public TPartID, public TNtf
{
    u32 m_dwNewPID;   //new part id
    tagDiscPartNF()
    {
        m_dwNewPID = 0;
    }
}TDiscPartNF,*PTDiscPartNF;

//混音器信息，可能会有多个speaker，这里只是制定一个
typedef struct tagAudioMixerInfo
{
    s32    m_nMixerID;             //mixer ID
    u32    m_dwSpeakerPID;         //speaker Pid
    BOOL32 m_bLocal;               //romote or local?
    tagAudioMixerInfo()
    {
        m_nMixerID = m_dwSpeakerPID = 0;
        m_bLocal = FALSE;
    }
    inline BOOL32 IsSpeakerExist(u32 dwPartId, BOOL32 bLocal)
    {
        return (m_dwSpeakerPID == dwPartId && m_bLocal == bLocal);
    }
}TAudioMixerInfo, *PTAudioMixerInfo;

//MCU的混音器的信息
typedef struct tagConfAudioInfo:public TRsp
{
    u8 m_byMixerCount;                                 //混音器数目
    u8 m_byDefaultMixerIndex;                          //缺省混音器，以索引表示,它也是当前正在播音的混音器
    s32 m_nSnapShot;                                    //会议快照值
    TAudioMixerInfo m_tMixerList[MAX_MIXERS_PER_CONF]; //混音器列表

 private:
     BOOL32 FindSpeaker(u32 dwPartId, BOOL32 bLocal, s32 &nMixerIndex)
     {
         for(nMixerIndex =0; nMixerIndex < m_byMixerCount; nMixerIndex++)
         {
             if(m_tMixerList[nMixerIndex].m_nMixerID == 0 || m_tMixerList[nMixerIndex].m_dwSpeakerPID == 0)
             {
                 return FALSE;
             }
             if(m_tMixerList[nMixerIndex].IsSpeakerExist(dwPartId, bLocal))
             {
                 return TRUE;
             }
         }
         return FALSE;
     }
 public:
     tagConfAudioInfo()
     {
         memset(this, 0, sizeof(*this));
         m_emReturnVal = emReturnValue_Ok;
         m_nSnapShot = 0;
     }

     void Clear(void)
     {
         memset(this, 0, sizeof(*this));
         m_emReturnVal = emReturnValue_Ok;
         m_nSnapShot = 0;
     }
     
     BOOL32 AddNewSpeaker(u32 dwPartId, BOOL32 bLocal, s32 nMixerId)
     {
         s32 nMixerIndex = 0;
         if(FindSpeaker(dwPartId, bLocal, nMixerIndex))
         {
             m_tMixerList[nMixerIndex].m_nMixerID = nMixerId;
             return FALSE;
         }
         if(nMixerIndex >= MAX_MIXERS_PER_CONF)
         {
             return FALSE;
         }

         m_tMixerList[nMixerIndex].m_dwSpeakerPID = dwPartId;
         m_tMixerList[nMixerIndex].m_bLocal       = bLocal;
         m_tMixerList[nMixerIndex].m_nMixerID     = nMixerId;

         m_byMixerCount++;
         return TRUE;   
     }
     
     BOOL32 DelSpeaker(u32 dwPartId, BOOL32 bLocal)
     {
         s32 nMixerIndex = 0;
         if(! FindSpeaker(dwPartId, bLocal, nMixerIndex))
         {
             return E_NOT_EXISTED;
         }
         
         if(IsCurrentSpeaker(dwPartId, bLocal))
         {
             m_byDefaultMixerIndex = 0;
         }

         for(; nMixerIndex < m_byMixerCount-1; nMixerIndex++)
         {
             memcpy(&(m_tMixerList[nMixerIndex]), 
                 &(m_tMixerList[nMixerIndex + 1]), sizeof(TAudioMixerInfo));
         }

         m_byMixerCount--;
         return S_RIGHT;        
     }
     
     inline void GetCurrentSpeaker(u32 &dwPartId, BOOL32 &bLocal)
     {
         dwPartId = m_tMixerList[m_byDefaultMixerIndex].m_dwSpeakerPID;
         bLocal   = m_tMixerList[m_byDefaultMixerIndex].m_bLocal;
     }

     s32 SetCurrentSpeaker(u32 dwPartId, BOOL32 bLocal)
     {
         s32 nMixerIndex = 0;
         if(FindSpeaker(dwPartId, bLocal, nMixerIndex))
         {
             m_byDefaultMixerIndex = nMixerIndex;
             return S_RIGHT;
         }
         s32 nNewMixerId = (nMixerIndex + 1) * 10;
         AddNewSpeaker(dwPartId, bLocal, nNewMixerId);
         m_byDefaultMixerIndex = nMixerIndex;
         return nNewMixerId;
     }

     inline BOOL32 IsCurrentSpeaker(u32 dwPartId, BOOL32 bLocal)
     {
         return (m_tMixerList[m_byDefaultMixerIndex].m_dwSpeakerPID == dwPartId
                && m_tMixerList[m_byDefaultMixerIndex].m_bLocal == bLocal);
     }
}TConfAudioInfo;



//会议视图几何布局变更通知
typedef struct tagConfViewChgNtf: public TNtf
{
    s32                   m_nViewID;                                 //发生几何布局变化的会议视图ID
    u8                    m_dwSubFrameCount;                           //变化后的几何布局划分成的子图像数目(subframe)
}TConfViewChgNtf;

//MCU的视频输出方案
typedef struct tagOutPutVideoSchemeInfo
{
    s32           m_nOutputVideoSchemeID;   //视频输出方案ID
    u32           m_dwMaxRate;              //max rate (kbps)
    u32           m_dwMinRate;              //min rate (kbps)
    BOOL32        m_bCanUpdateRate;         // whether or not enable update rate
    s32           m_nFrameRate;             // frame rate (30、25)
    PayloadType   m_emPayLoad;              // payload    (h261,h263,etc)
    tagOutPutVideoSchemeInfo()
    {
        memset(this, 0, sizeof(*this));
    }
}TOutPutVideoSchemeInfo,*PTOutPutVideoSchemeInfo;


//一个会议视图的定义，它包括一系列的几何布局，一系列的视图视频源(由pid来标志)的列表，
//以及可选的输出该会议视图的几个视频输出方案(rad mcu 中最多四个).
typedef struct tagViewInfo
{
    s32                m_nViewId;                                           //会议视图 ID
    emResolution       m_emRes;                                             //会议视图图像的分辨率
    u8                 m_byCurrentGeometryIndex;                            //会议视图当前正在使用(输出)的几何布局,以在几何布局列表中的索引来标明
    u8                 m_byGeometryCount;                                   //几何布局的数目
    u8                 m_abyGeometryInfo[MAX_GEOMETRYS_PER_VIEW];            //几何布局列表
    u8                 m_byContributePIDCount;                              //会议视图的视频源的数目
    TPartID            m_atContributePIDs[MAX_SUBFRAMES_PER_GEOMETRY];         //视频源列表
    u8                 m_byVideoSchemeCount;                                //可选的视频输出方案数目
    TOutPutVideoSchemeInfo    m_atVideoSchemeList[MAX_VIDEOSCHEMES_PER_VIEW];     //视频输出方案列表
    
    //add 2007.04.28
    BOOL32             m_bAutoSwitchStatus;
    BOOL32             m_bDynamic;
    s32                m_nAutoSwitchTime;
    s32                m_nVideoRole;

    
    inline tagViewInfo()
    {
        memset(this, 0, sizeof(*this));
    }
    
    inline void SetViewResolution(emResolution emRes)
    {
        m_emRes = emRes;
    }
    
    inline emResolution GetViewResolution()
    {
        return m_emRes;
    }
    
    inline void GetCurrentContributerList(TPartID **pptContributerParts, s32 &nArrayLen, s32 *pnCurrentSubframes)
    {
        if(pptContributerParts)
        {
           *pptContributerParts = m_atContributePIDs;
            nArrayLen = m_byContributePIDCount;
        }
        if(pnCurrentSubframes)
        {
           *pnCurrentSubframes = m_abyGeometryInfo[m_byCurrentGeometryIndex];
        }
        return;
    }

    inline BOOL32 SetCurrentContributerList(TPartID *ptContributerParts, s32 nArrayLen)
    {
        if(ptContributerParts != NULL && nArrayLen > 0)
        {
            s32 nSubFrameCount = 0;
            nSubFrameCount =(nArrayLen <= MAX_SUBFRAMES_PER_GEOMETRY ?
                                     nArrayLen : MAX_SUBFRAMES_PER_GEOMETRY);
            s32 nIndex = 0;
            while(nIndex < MAX_SUBFRAMES_PER_GEOMETRY && m_abyGeometryInfo[nIndex] != 0 && m_abyGeometryInfo[nIndex] != nSubFrameCount)
            {
                nIndex ++;
            }
            if(nIndex >= MAX_SUBFRAMES_PER_GEOMETRY)
            {
                return FALSE;
            }
            if(m_abyGeometryInfo[nIndex] == 0)
            {
                m_byContributePIDCount ++;
                m_byGeometryCount ++;
                m_abyGeometryInfo[nIndex] = nSubFrameCount;  
            }
            m_byCurrentGeometryIndex = nIndex;
            memcpy(m_atContributePIDs, ptContributerParts, sizeof(TPartID)*nSubFrameCount);
        }
        else
        {
            m_byGeometryCount        = 1;
            m_abyGeometryInfo[0]     = 1;
            m_byCurrentGeometryIndex = 0;
            m_byContributePIDCount   = 0;
        }
        return TRUE;
    }

    inline void SetGeometryList(u8 *pbyGeometryList, s32 nItemCount, s32 nIndexCurrentGeometry = 0)
    {
        s32 nGeometryCount = 0;
        nGeometryCount = (nItemCount < MAX_GEOMETRYS_PER_VIEW ?
                         nItemCount : MAX_GEOMETRYS_PER_VIEW);
        memcpy(m_abyGeometryInfo, pbyGeometryList,nGeometryCount * sizeof(u8));
        m_byGeometryCount = nGeometryCount;
        m_byCurrentGeometryIndex = ((m_byGeometryCount -1 > nIndexCurrentGeometry) ?
                                    nIndexCurrentGeometry : m_byGeometryCount -1);
        return;
    }
    
    inline void GetGeometryList(u8 **ppbyGeometryList, s32 *pnItemCount, s32 *pnIndexCurrentGeometry = NULL)
    {
        if(ppbyGeometryList && pnItemCount) 
        {
            *ppbyGeometryList = m_abyGeometryInfo;
            *pnItemCount      = m_byGeometryCount;
        }
        if(pnIndexCurrentGeometry)
        {
            *pnIndexCurrentGeometry = m_byCurrentGeometryIndex;
        }
        return;
    }
    
    inline void SetVideoSchemeList(TOutPutVideoSchemeInfo *ptSchemeList, s32 nItemCount)
    {
        this->m_byVideoSchemeCount = nItemCount;
        memcpy(this->m_atVideoSchemeList, ptSchemeList, nItemCount * sizeof(TOutPutVideoSchemeInfo));
    }

    inline void GetVideoSchemeList(TOutPutVideoSchemeInfo **pptSchemeList, s32 &nItemCount)
    {
        *pptSchemeList = this->m_atVideoSchemeList;
        nItemCount = this->m_byVideoSchemeCount;
        return;
    }
    
    inline s32 GetViewId() { return m_nViewId;}
    inline void SetViewId(s32 nViewId) { m_nViewId = nViewId;}

}TView, *PTView;

//
typedef struct tagConfVideInfoReq: public TReq
{
    u8          m_bFullInfo;    //是否请求发送几何布局视图
    tagConfVideInfoReq(){ m_bFullInfo = TRUE; }
}TConfVideInfoReq;
//级连会议在单个MCU上的视频输入输出信息
typedef struct tagConfVideoInfo:public TRsp
{
    u8          m_byConfViewCount;                              //会议视图的数目
    //默认使用的会议视图,如果与该MCU直连的终端或是MCU在连接建立时没有指定会议视图信息，
    //则默认观看该会议视图
    u8          m_byDefaultViewIndex;                            //默认会议视图索引
    u8          m_bFullInfo;                                     //是否发送几何布局视图
    s32         m_nSnapShot;                                     //会议快照值
    TView       m_atViewInfo[MAX_VIEWS_PER_CONF]; //会议视图列表
    
    private:
        BOOL32 FindView(s32 nViewId, s32 &nViewIndex)
        {
            for(nViewIndex =0; nViewIndex < m_byConfViewCount; nViewIndex++)
            {
                if(m_atViewInfo[nViewIndex].m_nViewId == 0)
                {
                    return FALSE;
                }
                if(m_atViewInfo[nViewIndex].m_nViewId == nViewId)
                {
                    return TRUE;
                }
            }
            return FALSE;
        }
        
    public:

        inline tagConfVideoInfo()
        {
            m_byConfViewCount = m_byDefaultViewIndex = 0;
            m_emReturnVal = emReturnValue_Ok;
            m_bFullInfo = TRUE;
        }
        inline void Clear(void)
        {
            memset(this, 0, sizeof(*this));
            m_byConfViewCount = m_byDefaultViewIndex = 0;
            m_emReturnVal = emReturnValue_Ok;
        }
        s32 AddView(TView &tView, BOOL32 bDefaultView = FALSE, BOOL32 bReplace = TRUE)
        {
            s32 nViewIndex = 0;
            if( FindView(tView.GetViewId(), nViewIndex))
            {
                if(bReplace)
                {
                    memcpy(&m_atViewInfo[nViewIndex], &tView, sizeof(TView));
                    if(bDefaultView)
                    {
                        m_byDefaultViewIndex = (u8)nViewIndex;
                    }
                    return S_REPLACED;
                }
                else
                {
                    if(bDefaultView)
                    {
                        m_byDefaultViewIndex = (u8)nViewIndex;
                    }
                    return E_ALREADY_EXISTED;
                }
            }

            if(nViewIndex >= MAX_VIEWS_PER_CONF)
            {
                return E_TOO_MANY_ITEMS;
            }

            memcpy(&m_atViewInfo[nViewIndex], &tView, sizeof(TView));
            if(bDefaultView)
            {
               m_byDefaultViewIndex = (u8)nViewIndex;
            }

            m_byConfViewCount++;
            return S_RIGHT;  
        }
        
        s32 DelView(s32 nViewId)
        {
            s32 nViewIndex = 0;
            if(! FindView(nViewId, nViewIndex))
            {
                return E_NOT_EXISTED;
            }
            if(nViewIndex == m_byDefaultViewIndex)
            {
                m_byDefaultViewIndex = 0;
            }
            memmove(&m_atViewInfo[nViewIndex], &m_atViewInfo[nViewIndex +1],
                   sizeof(TView) * (m_byConfViewCount - nViewIndex - 1));
            
            m_byConfViewCount--;
            return S_RIGHT;
        }
        
        s32 GetView(s32 nViewId, TView **pptView)
        {
            s32 nViewIndex = 0;
            if(! FindView(nViewId, nViewIndex))
            {
                *pptView = NULL;
                 return E_NOT_EXISTED;
            }
            if(pptView)
            {
                *pptView = &m_atViewInfo[nViewIndex];
                return nViewIndex;
            }
            return FALSE;
        }
        
        BOOL32 IsViewExist(s32 nViewId)
        {
            s32 nViewIndex = 0;
            if(! FindView(nViewId, nViewIndex))
            {
                return FALSE;
            }
            return TRUE;
        }
        inline TView* GetItemByIndex(s32 nIndex)
        {
           return nIndex >= 0 && nIndex < m_byConfViewCount ?
               &m_atViewInfo[nIndex] : NULL;
        }
        
        inline s32 GetDefaultViewId()
        { 
            return m_atViewInfo[m_byDefaultViewIndex].GetViewId();
        }

        void DelPartRelationship(s32 nPId, BOOL32 bLocal)
        {
            s32 nIndex            = 0;
            TView *ptView         = NULL;
            TPartID *ptPartId     = NULL;
            s32     nContributors = 0;
            while(ptView = GetItemByIndex(nIndex))
            {
                ptView->GetCurrentContributerList(&ptPartId, nContributors, NULL);
                while(nContributors-- > 0)
                {
                    if((ptPartId + nContributors)->IsSamePart(nPId, bLocal))
                    {
                        //will 
                        (ptPartId + nContributors)->SetEmpty();
                    }
                }           
                nIndex ++;
            }
        }
}TConfVideoInfo;



//add 2007.04.28
typedef struct tagAutoswitchReq: public TReq     //
{
  
    s32         m_nLid;
    s32         m_nAutoSwitchTime;
    s32         m_nAutoSwitchLevel;
    BOOL32      m_bAutoSwitchOn;

public: 
    tagAutoswitchReq()
    {
        Clear();
    }


    void Clear()
    {
        memset(this, 0, sizeof(tagAutoswitchReq));
    }
    
    void SetLid(s32 nLid)
    {
        m_nLid = nLid;
    }

    s32  GetLid()
    {
        return m_nLid;
    }

    void SetAutoSwitchTime(s32 nTime)
    {
        m_nAutoSwitchTime = nTime;
    }

    s32 GetAutoSwitchTime()
    {
        return m_nAutoSwitchTime;
    }

    void SetAutoSwitchLevel(s32 nLevel)
    {
        m_nAutoSwitchLevel = nLevel;
    }

    s32 GetAutoSwitchLevel()
    {
        return m_nAutoSwitchLevel;
    }
    
    void SetAutoSwitchOn(BOOL32 bOn)
    {
        m_bAutoSwitchOn = bOn;
    }

    BOOL32 GetAutoSwitchOn()
    {
        return m_bAutoSwitchOn;
    }

}TAutoswitchReq, *PTAutoswitchReq;


typedef struct tagAutoswitchRsp: public TRsp{}TAutoswitchRsp, PTAutoswitchRsp;

// [7/15/2010 xliang] 
//初始化级联库
// BOOL32 MMcuStackInit(u16 wListenPort,BOOL32 bSockInit = FALSE);
//解初始化级联库
// BOOL32 MMcuStackClean(BOOL32 bSockClean = FALSE);

//新的会议级联成功通知
/*#define 	H_CASCADE_NEW_CALL_NTF   (s32)CASCADE_MSGTYPE_ENDING*/  
//会议级联断开通知
/*#define     H_CASCADE_DROP_CALL_NTF  (s32)(H_CASCADE_NEW_CALL_NTF+1)*/

// API void mmcusetlog(u8 level);
// API void mmcuhelp();

//#endif //!_H323ADAPTER_H


// class CXMLTree;
// class CMCUCascade
// {
// public:
//     CMCUCascade();
//     ~CMCUCascade();
//     MMCU_API u16  Connect( u32 dwPeerIP, u16 wPeerPort );
//     MMCU_API u16  Disconnect();
//     MMCU_API u16  SendMsg(s32 nMsgID, u8 *pbyBuffer, u32 dwBufLen);
//     MMCU_API u16  SetSrcConfId(u8 *pbyBuf, s32 nLen);
//     MMCU_API u16  SetDstConfId(u8 *pbyBuf, s32 nLen);
// 	MMCU_API u16  RegisterEventHandler(PCascadeHandler pMsgCallback, u32 dwUserData);
//     
// public:
//     u32             m_dwCookie;
// 	u32             m_dwPeerCookie;
//     u32			    m_dwUserData;
//     PCascadeHandler m_pfAppHandler;
// 	u32             m_dwNode;
// 	SEMHANDLE       m_hSem; 
//     u8				m_abySrcConfID[CONFID_BYTELEN];
//     u8				m_abyDstConfID[CONFID_BYTELEN];
//     u8				m_abyChairID[CONFID_BYTELEN];
// 	u8*             m_pbyTcpBuf;
// 	s32             m_nTcpBufLen;
// 	u8*             m_pbyBackUp; 
// 
//     u32             m_dwLocalIP;
//     u16             m_wLocalPort;
//     u32             m_dwPeerIP;
//     u16             m_wPeerPort;
// 
// public:
//     MMCU_API u16  Init();
//     MMCU_API u16  UnInit();
// 	void DoRcvMsg(u8 *pBuf, s32 nLen);
// 	s8*  BuildPid(u32 dwPid, BOOL32 bLocal = TRUE);
//     BOOL32 ExtractPid(const s8* pszPID, u32 *pdwPid, BOOL32 *pbLocal=NULL);
// 	s32  AddPartToTree(TPart &tPart, s32 nParentNode, s32 nBrotherNode = -1); 
//     
//     s32 AddConfViewToRsp(TView &tViewInfo,  BOOL32 bFullInfo, s32 nParentNode, s32 nBrotherNode);
//     s32 AddVideoSchemeToView(TOutPutVideoSchemeInfo &tVideoSchemeInfo, s32 nParentNode, s32 nBrotherNode);   
//     s32 AddGeomtryToView(u8 nSubFrameCount,  s32 nParentNode, s32 nBrotherNode); 
//         
//     BOOL32 OnTcpIntegrity(u8 *pBuf, s32 nLen, u8** ppbyIntPack, s32 *pnIntSize);
//     friend void NewConnection(u32 hListenNode, u32 hNode, u32* phAppNode);
//     friend void DispatchCascadeMsg(u32 hNode, u32 hAppNode, u8 *pBuf, s32 nLen);
// 	friend CMCUCascade * RegMsgAssociateWithMMcu(u8 *pBuf, s32 nLen);
// 	friend CMCUCascade * FindCasIns(u8 abySrcConfID[CONFID_BYTELEN], u8 abyDstConfID[CONFID_BYTELEN]);
// 	void DoDefaultMsgResponse(CXMLTree *pXMLTree, const s8* pszLable);
//         
// private://Rcv
//     void OnRsp(CXMLTree *pcTree, /*const s8 *pszFlag, */H323MCUMsgType emMsgType);
// 	void OnNonStandardMsgReq(CXMLTree *pcTree);
// 	void OnNonStandardMsgRsp(CXMLTree *pcTree);
// 	void OnNonStandardMsgNtf(CXMLTree *pcTree);
// 	void OnRegUnRegReq(CXMLTree *pcTree);
// 	void OnRegUnRegRsp(CXMLTree *pcTree);
//     void OnNewNeighborNtf(CXMLTree *pcTree);
// 	void OnCallNtf(CXMLTree *pcTree);
//     void OnNewPartNtf(CXMLTree *pcTree);
//     void OnGetPartListReq(CXMLTree *pcTree);
//     void OnGetPartListRsp(CXMLTree *pcTree);
//     void OnCMVideoReq(CXMLTree *pcTree);
//     void OnCMVideoRsp(CXMLTree *pcTree);
//     void OnCMAudioReq(CXMLTree *pcTree);
//     void OnCMAudioRsp(CXMLTree *pcTree);
//     void OnOutputLayoutNtf(CXMLTree *pcTree);
// 	void OnNewSpeakerNtf(CXMLTree *pcTree);
//     void OnSetInLayoutReq(CXMLTree *pcTree);
//     void OnInvitePartReq(CXMLTree *pcTree);
//     void OnReInvitePartReq(CXMLTree *pcTree);
//     void OnPartDelReq(CXMLTree *pcTree);
//     void OnPartDiscNtf(CXMLTree *pcTree);
//     void OnPartDiscReq(CXMLTree *pcTree);
//     void OnPartDelNtf(CXMLTree *pcTree);
//     void OnSetOutLayoutReq(CXMLTree *pcTree);
//     void OnLayoutChgNtf(CXMLTree *pcTree);
//     void OnPartMediaChanReq(CXMLTree *pcTree);
//     void OnPartMediaChanNtf(CXMLTree *pcTree);
//     void OnVASetInReq(CXMLTree *pcTree);
//     void OnMcuServiceListReq(CXMLTree *pcTree);
//     void OnMcuConfListReq(CXMLTree *pcTree);
//     void OnMcuConfProfileReq(CXMLTree *pcTree);
//     void OnMcuConfIdByNameReq(CXMLTree *pcTree);
//     void OnMcuServiceListRsp(CXMLTree *pcTree);
//     void OnMcuConfListRsp(CXMLTree *pcTree);
//     void OnMcuConfProfileRsp(CXMLTree *pcTree);
//     void OnMcuConfIdByNameRsp(CXMLTree *pcTree);
// 
//     //2007.04.28
//     void OnSetLayoutAutoswitchReq(CXMLTree *pXMLTree);
// 	void OnSetLayoutAutoswitchRsp(CXMLTree *pXMLTree);
// 	
// 
//     s32 ParsePartInfo(CXMLTree *pcTree,s32 nFromNode,TPart *ptPart);
//     BOOL32 ParseVideoInfo(CXMLTree *pcTree,s32 nFromNode,TView *ptVideo);
//     
// private://Send
// private://Send
//     CXMLTree* ToNonStandardMsgReq(u8 *pBuf, s32 nLen);
//     CXMLTree* ToNonStandardMsgRsp(u8 *pBuf, s32 nLen);
// 	CXMLTree* ToNonStandardMsgNtf(u8 *pBuf, s32 nLen);
// 
//     CXMLTree* ToMcuServiceListReq(u8 *pBuf, s32 nLen);
//     CXMLTree* ToMcuConfListReq(u8 *pBuf, s32 nLen);
//     CXMLTree* ToMcuConfProfileReq(u8 *pBuf, s32 nLen);
//     CXMLTree* ToMcuConfIdByNameReq(u8 *pBuf, s32 nLen);
// 	//
//     CXMLTree* ToMcuServiceListRsp(u8 *pBuf, s32 nLen);
//     CXMLTree* ToMcuConfListRsp(u8 *pBuf, s32 nLen);
//     CXMLTree* ToMcuConfProfileRsp(u8 *pBuf, s32 nLen);
//     CXMLTree* ToMcuConfIdByNameRsp(u8 *pBuf, s32 nLen);
// 	
// //////////////////////////////////////////////////////////////////////////
//     CXMLTree* ToRegUnRegReq(u8 *pBuf, s32 nLen);
//     CXMLTree* ToRegUnRegRsp(u8 *pBuf, s32 nLen);
//     CXMLTree* ToNewRosterNtf(u8 *pBuf, s32 nLen);
//     CXMLTree* ToInvitePartReq(u8 *pBuf, s32 nLen);
//     CXMLTree* ToRsp(u8 *pBuf, s32 nLen, const s8 *pszLable);
// 	CXMLTree* ToCallNtf(u8 *pBuf, s32 nLen);
//     CXMLTree* ToNewPartNtf(u8 *pBuf, s32 nLen);
//     CXMLTree* ToPartListReq(u8 *pBuf, s32 nLen);
//     CXMLTree* ToPartListRsp(u8 *pBuf, s32 nLen, BOOL32 &bFinished, s32 nStartPartIndex, s32 nEncodedPartNum = 3);
//     CXMLTree* ToReInvitePartReq(u8 *pBuf, s32 nLen);
//     CXMLTree* ToDiscPartReq(u8 *pBuf, s32 nLen);
//     CXMLTree* ToDiscPartNtf(u8 *pBuf, s32 nLen);
//     CXMLTree* ToDelPartReq(u8 *pBuf, s32 nLen);
//     CXMLTree* ToDelPartNtf(u8 *pBuf, s32 nLen);
//     CXMLTree* ToSetInReq(u8 *pBuf, s32 nLen);
//     CXMLTree* ToOutputNtf(u8 *pBuf, s32 nLen);
//     CXMLTree* ToAudioInfoReq(u8 *pBuf, s32 nLen);
//     CXMLTree* ToAudioInfoRsp(u8 *pBuf, s32 nLen);
//     CXMLTree* ToVideoInfoReq(u8 *pBuf, s32 nLen);
//     CXMLTree* ToVideoInfoRsp(u8 *pBuf, s32 nLen);
//     CXMLTree* ToNewSpeakerNtf(u8 *pBuf, s32 nLen);
//     CXMLTree* ToSetOutLayoutReq(u8 *pBuf, s32 nLen);
//     CXMLTree* ToLayoutChgNtf(u8 *pBuf, s32 nLen);
//     CXMLTree* ToPartMediaChanReq(u8 *pBuf, s32 nLen);
//     CXMLTree* ToPartMediaChanNtf(u8 *pBuf, s32 nLen);
// 
//     CXMLTree* ToMcuSetLayoutAutoswitchRequest(u8* pbyBuffer, s32 nBufLen);
// 	CXMLTree* ToMcuSetLayoutAutoswitchResponse(u8* pbyBuffer, s32 nLen);
// };
#endif





















