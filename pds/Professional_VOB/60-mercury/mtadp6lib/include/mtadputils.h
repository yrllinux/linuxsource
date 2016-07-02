/*****************************************************************************
   模块名      : mtadp
   文件名      : mtadputils.h
   相关文件    : mtadputils.cpp
   文件实现功能: 数据结构转换头文件
   作者        : Tan Guang
   版本        : V1.0  Copyright(C) 2003-2004 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期		版本        修改人      走读人		修改内容
   2003/11		1.0         Tan Guang				创建
   2005/12/31	4.0			张宝卿					兼容3.6终端
******************************************************************************/
#ifndef _MTADPUTILS_H
#define _MTADPUTILS_H

#include "osp.h"

/* channel name definitions */
#define CHANNAME_MP3		( LPSTR )"mp3Audio"
#define CHANNAME_MPEG4		( LPSTR )"mpeg4Video"
#define CHANNAME_G711A		( LPSTR )"g711Alaw64k"
#define CHANNAME_G711U		( LPSTR )"g711Ulaw64k"
#define CHANNAME_G722		( LPSTR )"g722"
#define CHANNAME_G728		( LPSTR )"g728"
#define CHANNAME_G729		( LPSTR )"g729"
#define CHANNAME_G7231		( LPSTR )"g7231"
#define CHANNAME_G7221C		( LPSTR )"g7221c"
#define CHANNAME_G719		( LPSTR )"g719"
#define CHANNAME_AACLC      ( LPSTR )"aac-lc"
#define CHANNAME_AACLD      ( LPSTR )"aac-ld"
#define CHANNAME_H261		( LPSTR )"h261VideoCapability"
#define CHANNAME_H262		( LPSTR )"h262VideoCapability"
#define CHANNAME_H263		( LPSTR )"h263VideoCapability"
#define CHANNAME_H263PLUS	( LPSTR )"h263VideoCapabilityPlus"
#define CHANNAME_H264		( LPSTR )"genericVideoCapability"
#define CHANNAME_H239		( LPSTR )"H239VideoCapability"
#define CHANNAME_H224		( LPSTR )"h224"
#define CHANNAME_T120		( LPSTR )"t120"
#define CHANNAME_MMCU       ( LPSTR )"mmcu"

class CMtAdpUtils
{	
public:
	static u32   PackMtInfoListIntoBuf  ( const TMt atMt[], const TMtAlias atMtAlias[], u8 byArraySize, u8 *pbyBuf, u32 dwBufLen );
	static u8    UnpackMtInfoListFromBuf( const u8 *pbyBuf, u32 dwBufLen, TMt atMt[], TMtAlias atMtAlias[], u8 byArraySize );
	
//	static s32   TFacilityInfo2TConfList( const TFACILITYINFO tFacilityInfo, u8 *buf, s32 nBufLen );
	static void  TConfList2FacilityInfo ( const u8 *buf, /*s32*/u16 wBufLen, TFACILITYINFO &tFacilityInfo );

	static void  CameraCommandIn2Out( const CServMsg &cMsg, TFeccStruct &tFeccStruct );
	static void  CameraCommandOut2In( TFeccStruct &tFeccStruct, u16 &wEvent, u8 &byParam );	

	static void  CapabilitiesSetIn2Out( const TCapSupport &tCapSupport, const TCapSupportEx &tCapEx, 
		TCapSet *ptCapSet, u8 byVendorId, TAudioTypeDesc *ptAudioTypeDesc, u8 byAudioCapNum,const TVideoStreamCap &tDsStreampEx);
	static void  CapSupportCommon     ( const TCapSupport &tLocalCap, const TCapSupportEx &tCapEx, TCapSet *ptRemoteCapSet,
		TMultiCapSupport &tCommonCap,const TVideoStreamCap *ptMainStreamCapEx, u8 byMSCapExNum,const TVideoStreamCap *ptDoubleStreamCapEx, 
		u8 byDSCapExNum, TAudioTypeDesc *ptLocalAudioTypeDesc, u8 byAudioCapNum);
//     static void  VideoCapCommon       ( const TSimCapSet &tLocalCap,  TCapSet *ptRemoteCapSet, TSimCapSet &tCommonCap, u8 byVendor, u8 byEncryptMode);
//    static BOOL  VideoDCapCommon      ( const TDStreamCap tDStreamCap, BOOL bH239, u8 byEncryptMode,
//                                         const TVideoStreamCap tFstVideoCap,
//                                         TCapSet *ptRemoteCapSet);

	static void	 CapabilityExSetIn2Out( TCapSupportEx &tCapSupportEx, TCapSet *ptCapSet );
	static void  FECTypeIn2Out( u8 byFECType, emFECType &FECType );
	static void  FECTypeOut2In( emFECType FECType, u8 &byFECType );
	static void  FECTypeCommon( emFECType LocalFECType, u8 byRemoteFECType, emFECType &CommonFECType );

	static u8	 PayloadTypeOut2In( u8 byPayLoadType );
	static u8    PayloadTypeIn2Out( u8 byPayloadType );
	static void	 PayloadTypeOut2In( u8 byPayLoadType, u8 &byPayload, u8 &byMediaType );

    static void  VideoCap264In2Out(const TVideoStreamCap tInCap, TH264VideoCap &tOutCap);
    static void  VideoCap264Out2In(const TH264VideoCap tRemoteCap, 
                                   const TVideoStreamCap &tLocalCap,
                                   TVideoStreamCap &tInCap, BOOL32 bDual = FALSE, u8 byDSFPS = 0, BOOL32 bDSEqMV = FALSE );

	static void  MaxVideoCap264Out2In(TCapSet *ptRemoteCapSet, 
		                              const TVideoStreamCap &tLocalVideoCap,
									  u16 wFStreamComType,
		                              TVideoStreamCap &tMaxRemoteCap);

// 	static BOOL32  MaxDSCap264Out2In(TCapSet *ptRemoteCapSet,
// 								     const TDStreamCap &tLocalDStreamCap, 
// 								     const TVideoStreamCap &tMainStreamCap,
// 								     u8 byEncryptMode, 
// 								     TVideoStreamCap  &tCommonDSCap,
// 								     BOOL32 bDSEqMV);
	//[4/8/2011 zhushengze]匹配共同能力集
	static BOOL32 MatchCommonCap( const TCapSupport &tLocalCap,
                                       const TCapSupportEx &tCapEx,
									   TCapSet *ptRemoteCapSet,
									   TMultiCapSupport &tCommonCap,
									   u8 byEncryptMode,
									   const TVideoStreamCap *ptMSCapEx,
									   u8 byMSCapExNum,
									   const TVideoStreamCap *ptDSCapEx,
									   u8 byDSCapExNum,
									   const TAudioTypeDesc *ptLocalAudioTypeDesc,
									   u8 byAudioCapNum);

	static BOOL32 MatchVideoCap( const TSimCapSet &tLocalCap, 
										const TDStreamCap &tLocalDStreamCap,
		                                TCapSet *ptRemoteCapSet, 
		                                TSimCapSet &tCommonCap,
								        u8 byEncryptMode,
                                        TSimCapSet &tExtraCommonCap,
										const TVideoStreamCap *ptMSCapEx,
										u8 byMSCapExNum);

    static BOOL32 MatchDSVideoCap(const TVideoStreamCap &tLocalMainVideoCap,
                                  const TDStreamCap &tLocalDStreamCap,
                                  TCapSet * ptRemoteCapSet,
                                  /*TVideoStreamCap*/TDStreamCap  &tCommonDSCap,
                                  TDStreamCap &tSecComDStreamCap,
                                  u8 byEncryptMode,
								  const TVideoStreamCap *ptDSCapEx,
								  u8 byDSCapEXNum
                                  );

	static BOOL32 MatchAudioCap( u8 byEncryMode,
							     u8 byAudioCapNum,
							     const TAudioTypeDesc *ptLocalAudioTypeDesc,
							     TCapSet *ptRemoteCapSet,
							     TMultiCapSupport &tCommonCap);

    static BOOL32 ReduceOneLevelMainCap(TVideoStreamCap &tViedoCap, u8 byAdpFps, u8 byOldFps);
//	static u8	 AliasTypeOut2In( AliasType type );
//	static u8	 AliasTypeIn2Out( u8 byMtAliasType );

	static void  MtAliasIn2Out( TMtAlias &tAlias, cmAlias &tCmAlias );
	static void  MtAliasOut2In( const cmAlias &tCmAlias, TMtAlias &tAlias );

	static s8*   PszGetRASReasonName( s32 nReason, s8* pszBuf, u8 byBufLen );
	static s8*   PszGetChannelName( u8 byChannelType, s8* pszBuf, u8 byBufLen );

	static u8    EncryptTypeIn2Out( u8 byEncryptMode );	
	static u8    EncryptTypeOut2In( emEncryptType emType );

	static u8    GetVendorId( u8 byVendorId );
	static u8    FrameRateIn2Out( u8 byFrameRate );
	static u8    FrameRateOut2In( u8 byFrameRate );
    static u16   GetVideoBitrate( TCapSet *ptCapSet, u8 byMediaType );
	
	static _TTERSTATUS  TMtStatusIn2Out( TMtStatus &tMtStatus );
	static TMtStatus    TMtStatusOut2In( _TTERSTATUS &tTerStatus );
	static TMtBitrate   TMtBitrateOut2In( _TTERBITRATE &tTerBitrate );

	static s32          ResIn2Out( u8 byInRes );
	static u8           ResOut2In( s32 emRes );
 	static emMediaType  MediaModeIn2Out( u8 byMediaMode );
	static u8           MediaModeOut2In( emMediaType mediatype );
	
	// 解决TConfInfo版本兼容问题，将Tconfinfo结构转换函数作为成员函数移至新类CStructConvert中
	//static void ConfIdVer36ToVer40( CConfId &cConfId, CConfIdVer36 &cConfIdVer36 );
	//static void ConfIdVer40ToVer36( CConfIdVer36 &cConfIdVer36, CConfId &cConfId );

	//static void KdvTimeVer36ToVer40( TKdvTime &tKdvTime, TKdvTimeVer36 &tKdvTimeVer36 );
	//static void KdvTimeVer40ToVer36( TKdvTimeVer36 &tKdvTimeVer36, TKdvTime &tKdvTime );

	//static void MtAliasVer36ToVer40( TMtAlias &tMtAlias, TMtAliasVer36 &tMtAliasVer36 );
	//static void MtAliasVer40ToVer36( TMtAliasVer36 &tMtAliasVer36, TMtAlias &tMtAlias );

	//3.6 <-> 4.6
    //static void ConfInfoV36ToV4R6( TConfInfo &tConfInfo, TConfInfoVer36 &tConfInfoVer36 );
    //static void ConfInfoV4R6ToV36( TConfInfoVer36 &tConfInfoVer36, TConfInfo &tConfInfo );
    
	//3.6 <-> 4.0
    //static void ConfInfoVer36ToVer40V4R4B2( TConfInfoV4R4B2 &tConfInfo, TConfInfoVer36 &tConfInfoVer36 );
	//static void ConfInfoVer40V4R4B2ToVer36( TConfInfoVer36 &tConfInfoVer36, TConfInfoV4R4B2 &tConfInfo );

	//4.0 <-> 4.5
    //static void ConfInfoVer40V4R4B2ToVer40V4R5( TConfInfoV4R5 &tConfInfo, TConfInfoV4R4B2 &tConfInfoV4R4B2 );
    //static void ConfInfoVer40V4R5ToVer40V4R4B2( TConfInfoV4R4B2 &tConfInfoV4R4B2, TConfInfoV4R5 &tConfInfo );
	
	//4.0 <-> 4.6
	//static void ConfInfoV4R6ToV4R4( TConfInfoV4R4B2 &tConfInfoV4R4B2, TConfInfo &tConfInfo );
	//static void ConfInfoV4R4ToV4R6( TConfInfo &tConfInfo, TConfInfoV4R4B2 &tConfInfoV4R4B2 );
	
	//4.5 <-> 4.6
	//static void ConfInfoV4R5ToV4R6( TConfInfo &tConfInfo, TConfInfoV4R5 &tConfInfoV4R5 );
	//static void ConfInfoV4R6ToV4R5( TConfInfoV4R5 &TConfInfoV4R5, TConfInfo &tConfInfo );

	//static void ConfStatusV4R5ToV4R6( TConfStatus &tConfStatus, TConfStatusV4R5 &tConfStatusV4R5 );
	//static void ConfStatusV4R6ToV4R5( TConfStatusV4R5 &tConfStatusV4R5, TConfStatus &tConfStatus );

	//static void VMPParamV4R5ToV4R6( TVMPParam &tVMPParam, TVMPParamV4R5 &tVMPParamV4R5 );
	//static void VMPParamV4R6ToV4R5( TVMPParamV4R5 &tVMPParamV4R5, TVMPParam &tVMPParam );
	//=====================================================

	//static void ConfAttribVer36ToVer40( TConfAttrb &tConfAttrb, TConfAttrbVer36 &tConfAttrbVer36 );
	//static void ConfAttribVer40ToVer36( TConfAttrbVer36 &tConfAttrbVer36, TConfAttrb &tConfAttrb );
	
	//static void ConfStatusVer36ToVer40V4R4B2( TConfStatusV4R4B2 &tConfStatus, TConfStatusVer36 &tConfStatusVer36 );
	//static void ConfStatusVer40V4R4B2ToVer36( TConfStatusVer36 &tConfStatusVer36, TConfStatusV4R4B2 &tConfStatus );

    //static void ConfStatusVer40V4R4B2ToVer40V4R5( TConfStatusV4R5 &tConfStatus, TConfStatusV4R4B2 &tConfStatusV4R4B2 );
    //static void ConfStatusV4R5ToV4R4B2( TConfStatusV4R4B2 &tConfStatusV4R4B2, TConfStatusV4R5 &tConfStatus );

	//static void CapSupportVer36ToVer40( TCapSupport &tCapSupport, TCapSupportVer36 &tCapSupportVer36 );
	//static void CapSupportVer40ToVer36( TCapSupportVer36 &tCapSupportVer36, TCapSupport &tCapSupport );
	
	//static void MediaEncryptVer36ToVer40( TMediaEncrypt &tEncryptKey, TMediaEncryptVer36 &tEncryptKeyVer36 );
	//static void MediaEncryptVer40ToVer36( TMediaEncryptVer36 &tEncryptKeyVer36, TMediaEncrypt &tEncryptKey );
	
	//static void MtVer36ToVer40( TMt &tMt, TMtVer36 &tMtVer36 );
	//static void MtVer40ToVer36( TMtVer36 &tMtVer36, TMt &tMt );

	//static void RecProgVer36ToVer40( TRecProg &tRecProg, TRecProgVer36 &tRecProgVer36 );
	//static void RecProgVer40ToVer36( TRecProgVer36 &tRecProgVer36, TRecProg &tRecProg );

	//static void ConfModeVer36ToVer40V4R4B2( TConfModeV4R4B2 &tConfMode, TConfModeVer36 &tConfModeVer36 );
	//static void ConfModeVer40V4R4B2ToVer36( TConfModeVer36 &tConfModeVer36, TConfModeV4R4B2 &tConfMode );

    //static void ConfModeVer40V4R4B2ToVer40Daily( TConfMode &tConfMode, TConfModeV4R4B2 &tConfModeR4V4B2 );
    //static void ConfModeVer40DailyToVer40V4R4B2( TConfModeV4R4B2 &tConfModeR4V4B2, TConfMode &tConfMode );

	//static void PollInfoVer36ToVer40( TPollInfo &tPollInfo, TPollInfoVer36 &tPollInfoVer36 );
	//static void PollInfoVer40ToVer36( TPollInfoVer36 &tPollInfoVer36, TPollInfo &tPollInfo );

	//static void VMPParamVer36ToVer40( TVMPParamV4R4B2 &tVMPParam, TVMPParamVer36 &tVMPParamVer36 );
	//static void VMPParamVer40ToVer36( TVMPParamVer36 &tVMPParamVer36, TVMPParamV4R4B2 &tVMPParam );

	//static void TERSTATUSVer36ToVer40( _TTERSTATUS &tTerStatus, _TTERSTATUS_VER36 &tTerStatus36 );
	//static void TERSTATUSVer40ToVer36( _TTERSTATUS_VER36 &tTerStatus36, _TTERSTATUS &tTerStatus );

	//static void VMPMemberVer36ToVer40( TVMPMember &tVMPMember, TVMPMemberVer36 &tVMPMemberVer36 );
	//static void VMPMemberVer40ToVer36( TVMPMemberVer36 &tVMPMemberVer36, TVMPMember &tVMPMember );

	//static void MtPollParamVer36ToVer40( TMtPollParam &tMtPollParam, TMtPollParamVer36 &tMtPollParamVer36 );
	//static void MtPollParamVer40ToVer36( TMtPollParamVer36 &tMtPollParamVer36, TMtPollParam &tMtPollParam );

	//static void DiscussParamVer36ToVer40V4R4B2( TDiscussParamV4R4B2 &tDiscussParam, TDiscussParamVer36 &tDiscussParamVer36 );
	//static void DiscussParamVer40V4R4B2ToVer36( TDiscussParamVer36 &tDiscussParamVer36, TDiscussParamV4R4B2 &tDiscussParam );

    //static void DiscussParamVer40V4R4B2ToVer40Daily( TMixParamV4R5 &tDiscussParam, TDiscussParamV4R4B2 &tDiscussParamVer36 );
    //static void DiscussParamVer40DailyToVer40V4R4B2( TDiscussParamV4R4B2 &tDiscussParamVer36, TMixParamV4R5 &tDiscussParam );
	
	//已经启用的类型兼容适配
	//static u8 MediaTypeVer36ToVer40( u8 byMediaTypeVer36 );
	//static u8 MediaTypeVer40ToVer36( u8 byMediaType40 );

	//static u8 VideoFormatVer36ToVer40( u8 byVideoFormatVer36 );
	//static u8 VideoFormatVer40ToVer36( u8 byVideoFormat40 );

	//static u8 ConfBasModeVer36ToVer40( u8 byConfBasModeVer36 );
	//static u8 ConfBasModeVer40ToVer36( u8 byConfBasMode40 );

	//static u8 ConfCallModeVer36ToVer40( u8 byConfCallModeVer36 );
	//static u8 ConfCallModeVer40ToVer36( u8 byConfCallMode40 );
	
	//还未启用的类型兼容适配
	//static u8 ActiveMediaTypeVer36ToVer40( u8 byActiveMediaTypeVer36 );
	//static u8 ActiveMediaTypeVer40ToVer36( u8 byActiveMediaType40 );

	//static u8 EqpMainTypeVer36ToVer40( u8 byEqpMainTypeVer36 );
	//static u8 EqpMainTypeVer40ToVer36( u8 byEqpMainType40 );

	//static u8 MediaModeVer36ToVer40( u8 byMediaModeVer36 );
	//static u8 MediaModeVer40ToVer36( u8 byMediaMode40 );

	//static u8 VideoDStreamTypeVer36ToVer40( u8 byDStreamTypeVer36 );
	//static u8 VideoDStreamTypeVer40ToVer36( u8 byDStreamType40 );

	//static u8 ConfOpenModeVer36ToVer40( u8 byConfOpenModeVer36 );
	//static u8 ConfOpenModeVer40ToVer36( u8 byConfOpenMode40 );

	//static u8 ConfEncryptModeVer36ToVer40( u8 byConfEncryptModeVer36 );
	//static u8 ConfEncryptModeVer40ToVer36( u8 byConfEncryptMode40 );

	//static u8 ConfUniformModeVer36ToVer40( u8 byConfUniformModeVer36 );
	//static u8 ConfUniformModeVer40ToVer36( u8 byConfUniformMode40 );

	//static u8 SwitchChnnlUniformModeVer36ToVer40( u8 bySwitchChnnlUniformModeVer36 );
	//static u8 SwitchChnnlUniformModeVer40ToVer36( u8 bySwitchChnnlUniformMode40 );

	//static u8 ConfDataModeVer36ToVer40( u8 byConfDataModeVer36 );
	//static u8 ConfDataModeVer40ToVer36( u8 byConfDataMode40 );

	//static u8 ConfReleaseModeVer36ToVer40( u8 byConfReleaseModeVer36 );
	//static u8 ConfReleaseModeVer40ToVer36( u8 byConfReleaseMode40 );

	//static u8 ConfTakeModeVer36ToVer40( u8 byConfTakeModeVer36 );
	//static u8 ConfTakeModeVer40ToVer36( u8 byConfTakeMode40 );

	//static u8 ConfLockModeVer36ToVer40( u8 byConfLockModeVer36 );
	//static u8 ConfLockModeVer40ToVer36( u8 byConfLockMode40 );

	//static u8 ConfSpeakerSrcVer36ToVer40( u8 byConfSpeakerSrcVer36 );
	//static u8 ConfSpeakerSrcVer40ToVer36( u8 byConfSpeakerSrc40 );

	//static u8 ConfVmpModeVer36ToVer40( u8 byConfVmpModeVer36 );
	//static u8 ConfVmpModeVer40ToVer36( u8 byConfVmpMode40 );

	//static u8 ConfRecModeVer36ToVer40( u8 byConfRecModeVer36 );
	//static u8 ConfRecModeVer40ToVer36( u8 byConfRecMode40 );

	//static u8 ConfPlayModeVer36ToVer40( u8 byConfPlayModeVer36 );
	//static u8 ConfPlayModeVer40ToVer36( u8 byConfPlayMode40 );

	//static u8 ConfPollModeVer36ToVer40( u8 byConfPollModeVer36 );
	//static u8 ConfPollModeVer40ToVer36( u8 byConfPollMode40 );

	//static u8 VmpMemberTypeVer36ToVer40( u8 byVmpMemberTypeVer36 );
	//static u8 VmpMemberTypeVer40ToVer36( u8 byVmpMemberType40 );
	
	//static u8 VmpStyleTypeVer36ToVer40( u8 byVmpStyleTypeVer36 );
	//static u8 VmpStyleTypeVer40ToVer36( u8 byVmpStyleType40 ); 
	
    static void McuToMcuMtStatusVerR4V5ToVer40( TMcuToMcuMtStatus &tSmcuMtStatus,TMcuToMcuMtStatusBeforeV4R5 &tSmcuMtStatusBeforeV4R5 );
	//其他功能函数
	static void   Ipdw2Str( u32 dwIp, s8* pszStr);
	static u8     GetAliasType( s8* pszStr);
	static BOOL32 IsIncludeAllChar( s8* pszSrc, s8* pszDst ); 
	static BOOL32 IsIpStr( s8* pszStr);
	static u8     GetResFromStr(s8* pszRes);
	static u16    GetComType( u8 byMediaType, u8 byEncrypt );
	static u16    GetGKErrCode( u16 wErrCode );
	static BOOL32 IsSrcFpsAcptable(u8 bySrcFPS, u8 byDstFPS);
	static emDisconnectReason   DisconnectReasonStack2MtAdp( CallDisconnectReason emReason );
	static CallDisconnectReason DisconnectReasonMtAdp2Stack( emDisconnectReason emReason );
    static u8                   DisconnectReasonMtAdp2Vc( emDisconnectReason emReason );
	
	static u32 GetResProduct(u8 byRes);
	static s8 ResCmp(u8 byRes1, u8 byRes2);

	//zjj20100617 以下3个函数从mcupfmlmt.cpp中复制过来
	static BOOL32 IsIPTypeFromString(s8* pszAliasString);
	static BOOL32 IsE164TypeFromString(s8 *pszSrc, s8* pszDst);
	static u8 GetMtAliasTypeFromString(s8* pszAliasString);

	static BOOL32 ConverTMt2TTERLABEL( const TMt &tMt, TTERLABEL &tMtLabel, BOOL32 bForcLocal = TRUE );
	static void ConverTTERLABEL2TMt( /*const*/ TTERLABEL &tMtLabel, TMt &tMt, u8 byConfIdx = 0, BOOL32 bForcLocal = TRUE );
    static BOOL32 IsDynamicRes( const u8 byRes );
    static BOOL32 IsStaticRes( const u8 byRes );
    static BOOL32 IsResIsSameType( const u8 bySrcRes, const u8 byDstRes);
    static u8 AdjustDStreamFps( u8 byFps); 
	static TAACCap::emAACChnlCfg GetAACChnnNum(u8 byAudioTrackNum);
};

inline BOOL32 TRY_MEM_COPY( void *pDst, void *pSrc, u32 dwLen, void *pLimit );

#endif  // _MTADPUTILS_H
