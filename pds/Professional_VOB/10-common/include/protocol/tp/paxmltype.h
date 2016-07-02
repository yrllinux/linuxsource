#ifndef _PA_XML_TYPE_H_
#define _PA_XML_TYPE_H_

#include "xmlengine.h"

//Data struct type begin
enum EmPAXDataType
{
  empaxAtomTypeBegin =0, //原子类型数据(kdvtype.h)
	empax_s32,
	empax_BOOL32,
	empax_BOOL,
	empax_u8,
	empax_s8,
	empax_u16,
	empax_u32,
	empax_u32_ip,
	empax_u8_blob,
	empax_time_t,
	empax_string,
  empaxEnumTypeBegin = 20, //枚举类型数据(mttype.h 业务层枚举头文件的解析结果)
	empax_EmBegin,
	empax_EmPAMediaType,
	empax_EmPAProfileMask,
	empax_EmPAVideoFormat,
	empax_EmPAAudioFormat,
	empax_EmPAPayloadType,
	empax_EmPAResolution,
	empax_EmPAVideoResolution,
	empax_EmDualReason,
	empax_EmPAOprType,
	empax_EmPATPEndpointType,
	empax_EmPATpApplySpeakerRet,
	empax_EmPATPDualReason,
	empax_EmPACnsCallReason,
	empax_EmPATpMixStatus,
	empax_EmPAAliasType,
	empax_EmPADisListOprType,
	empax_EmPADisListOprReslt,
	empax_EmPAAACSampleFreq,
	empax_EmPAAACChnlCfg,
  empaxStructTypeBegin = 200, //数据结构类型数据(mtstruct.h 业务层结构类型头文件的解析结果)
	empax_TYouAreSeeing,
	empax_TPAIPAddress,
	empax_TPAConfStatusInd,
	empax_TPAPollNodeChange,
	empax_TPACascadingSpeaker,
	empax_TPAPeerMuteCmd,
	empax_TPAPeerMuteInd,
	empax_THangupCascadingConf,
	empax_TPAName,
	empax_TPAPollDstName,
	empax_TPAAskKeyByScreenTr,
	empax_TPAAlias,
	empax_TPAEpKeyID,
	empax_TPAUmsVidFormat,
	empax_TPAUmsAudFormat,
	empax_TPAConfNodeUpdateTr,
	empax_TPAConfNodeUpdateTrList,
	empax_TPAConfNodeUpdateB2NEWTr,
	empax_TPAConfNodeUpdateB2NEWTrList,
	empax_TPAConfNodeDelTrList,
	empax_TPAAudMixEpCmdTr,
	empax_TPAAudMixEpAckTr,
	empax_TDualInfo,
	empax_TDualInfoAck,
	empax_TDualRequest,
	empax_TPATvMonitorCmd,
	empax_TPATvMonitorInd,
	empax_TPANodeCapsetTr,
	empax_TPaMediaTransAddr,
	empax_TPACnsInfo,
	empax_TConfCnsList,
	empax_TPAConfCnsList,
	empax_TPAChanSelecteCascad,
	empax_TPAChanSelecteCascadRes,
	empax_TPATmpChanSelecteCascadRes,
	empax_TPAChanSelecteCascadCancelRes,
	empax_TPAPollCmd,
	empax_TPAPollAck,
	empax_TPAViewCmd,
	empax_TPAAdjustFrameRate,
	empax_TPAAdjustVidRes,
	empax_TPAViewAck,
	empax_TPACascadeOpenChanCmd,
	empax_TPACascadeOpenChanAck,
	empax_TPAUmsReasonInd,
	empax_TPAUmsUpRecPlayState,
	empax_TPAEpPathKey,
	empax_TPAEpKey,
	empax_TPAFlowControl,
	empax_TPAAudChanCmd,
	empax_TPAAudChanAck,
	empax_TPAVidUpAddr,
	empax_TPAVidDownAddr,
	empax_TPAVidChanCmd,
	empax_TPAVidChanAck,
	empax_TPAAuxMixList,
	empax_TPAConfAuxMixInfo,
	empax_TPAConfEpID,
	empax_TPAConfTurnList,
	empax_TPAConfTurnInfo,
	empax_TPAConfCallEpAddr,
	empax_TPAChairConfInfo,
	empax_TPAAudMixStat,
	empax_TPADisScreenStat,
	empax_TPADisAskKeyFrame,
	empax_TPADisListOpr,
	empax_TPATerminalLabel,
	empax_TPATerLabelList,
	empax_TPATerminalInfo,
empaxDataTypeEnd = 500 //类型定义结束
};
//	Data struct type end

extern TXDATAINFO* g_tPAAtomDataInfo[];
extern TXDATAINFO* g_tPAEnumDataInfo[];
extern TXDATAINFO* g_tPAStructDataInfo[];
#define _InitPAAtomType( type ) InitAtomType_##type( empax_##type )
#define _InitPABlobType( type ) InitAtomType_##type##_blob( empax_##type##_blob )

#define UserInitPAXmlEngine() do { \
	_InitPAAtomType( s32    ); \
	_InitPAAtomType( BOOL32 ); \
	_InitPAAtomType( BOOL   ); \
	_InitPAAtomType( u8     ); \
	_InitPAAtomType( s8     ); \
	_InitPAAtomType( u16    ); \
	_InitPAAtomType( u32    ); \
	_InitPAAtomType( u32_ip ); \
	_InitPABlobType( u8     ); \
	_InitPAAtomType( time_t ); \
	                         \
} while (0)

#define XML_MARSHAL( Type, pSrcBuf, pOutBuf, dwOutputLen, dwOverFlow ) \
	XML_Encode( empax_##Type, pSrcBuf, em_PAXMLNone, pOutBuf, dwOutputLen, dwOverFlow )

#define XML_UNMARSHAL( Type, pSrcBuf, dwSrcBufLen, pOutBuf, dwOutputLen, dwOverFlow ) \
	XML_Decode( empax_##Type, pSrcBuf, dwSrcBufLen, em_PAXMLNone, pOutBuf, dwOutputLen, dwOverFlow )
 

const char XmlTypeVersion[] = "2013-9-5 8:2:56";

#endif //_XML_TYPE_H_
