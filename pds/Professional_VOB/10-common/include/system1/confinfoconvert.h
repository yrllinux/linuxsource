/*****************************************************************************
   模块名      :  MCU
   文件名      :  confinfoconvert.h
   相关文件    : 
   文件实现功能:  解决TConfInfo结构体版本兼容问题 
   作者        :  苗庆松
   版本        : V0.1  Copyright(C) 2005-2006 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期      版本        修改人        修改内容
   2011/04     1.0         苗庆松          创建
******************************************************************************/

#ifndef _CONFINFOCONVERT_H
#define _CONFINFOCONVERT_H

#include "confinfostruct36.h"
#include "confinfostructv4r4r5.h"

#ifdef WIN32
#pragma pack( push )
#pragma pack( 1 )
#define PACKED 
#else
#define PACKED __attribute__((__packed__))	
#endif

/*-------------------------------------------------------------------
            解决TConfInfo结构体版本兼容问题   CStructConvert                             
--------------------------------------------------------------------*/
class CStructConvert
{
public:
	static void VMPParamV4R5ToV4R6( TVMPParam &tVMPParam, TVMPParamV4R5 &tVMPParamV4R5 );
	static void VMPParamV4R6ToV4R5( TVMPParamV4R5 &tVMPParamV4R5, TVMPParam &tVMPParam );
	
	static void TERSTATUSVer36ToVer40( _TTERSTATUS &tTerStatus, _TTERSTATUS_VER36 &tTerStatus36 );
	static void TERSTATUSVer40ToVer36( _TTERSTATUS_VER36 &tTerStatus36, _TTERSTATUS &tTerStatus );
	
	//3.6 <-> 4.6
    static void ConfInfoV36ToV4R6( TConfInfo &tConfInfo, TConfInfoVer36 &tConfInfoVer36 );
    static void ConfInfoV4R6ToV36( TConfInfoVer36 &tConfInfoVer36, TConfInfo &tConfInfo );
    
	//3.6 <-> 4.0
	static void ConfInfoVer36ToVer40V4R4B2( TConfInfoV4R4B2 &tConfInfo, TConfInfoVer36 &tConfInfoVer36 );
	static void ConfInfoVer40V4R4B2ToVer36( TConfInfoVer36 &tConfInfoVer36, TConfInfoV4R4B2 &tConfInfo );
	
	//4.0 <-> 4.5
    static void ConfInfoVer40V4R4B2ToVer40V4R5( TConfInfoV4R5 &tConfInfo, TConfInfoV4R4B2 &tConfInfoV4R4B2 );
    static void ConfInfoVer40V4R5ToVer40V4R4B2( TConfInfoV4R4B2 &tConfInfoV4R4B2, TConfInfoV4R5 &tConfInfo );
	
	//4.0 <-> 4.6
	static void ConfInfoV4R6ToV4R4( TConfInfoV4R4B2 &tConfInfoV4R4B2, TConfInfo &tConfInfo );
	static void ConfInfoV4R4ToV4R6( TConfInfo &tConfInfo, TConfInfoV4R4B2 &tConfInfoV4R4B2 );
	
	//4.5 <-> 4.6
	static void ConfInfoV4R5ToV4R6( TConfInfo &tConfInfo, TConfInfoV4R5 &tConfInfoV4R5 );
	static void ConfInfoV4R6ToV4R5( TConfInfoV4R5 &tConfInfoV4R5, TConfInfo &tConfInfo );

	//解决TConfInfo结构体版本兼容问题,提供的公共调用接口
	static void ConvertTConfInfoIn2Out( TConfInfo &tConfInfo, LPCSTR Version, void *ptConfInfo, u16 &wLength );
	static void ConvertTConfInfoOut2In( LPCSTR Version, void *ptConfInfo, u16 wLength, TConfInfo &tConfInfo );

private:
    static void ConfIdVer36ToVer40( CConfId &cConfId, CConfIdVer36 &cConfIdVer36 );
	static void ConfIdVer40ToVer36( CConfIdVer36 &cConfIdVer36, CConfId &cConfId );
	
	static void KdvTimeVer36ToVer40( TKdvTime &tKdvTime, TKdvTimeVer36 &tKdvTimeVer36 );
	static void KdvTimeVer40ToVer36( TKdvTimeVer36 &tKdvTimeVer36, TKdvTime &tKdvTime );
	
	static void MtAliasVer36ToVer40( TMtAlias &tMtAlias, TMtAliasVer36 &tMtAliasVer36 );
	static void MtAliasVer40ToVer36( TMtAliasVer36 &tMtAliasVer36, TMtAlias &tMtAlias );

	static void ConfAttribVer36ToVer40( TConfAttrb &tConfAttrb, TConfAttrbVer36 &tConfAttrbVer36 );
	static void ConfAttribVer40ToVer36( TConfAttrbVer36 &tConfAttrbVer36, TConfAttrb &tConfAttrb );

	static void CapSupportVer36ToVer40( TCapSupport &tCapSupport, TCapSupportVer36 &tCapSupportVer36 );
	static void CapSupportVer40ToVer36( TCapSupportVer36 &tCapSupportVer36, TCapSupport &tCapSupport );
	
	static void MediaEncryptVer36ToVer40( TMediaEncrypt &tEncryptKey, TMediaEncryptVer36 &tEncryptKeyVer36 );
	static void MediaEncryptVer40ToVer36( TMediaEncryptVer36 &tEncryptKeyVer36, TMediaEncrypt &tEncryptKey );
	
	static void MtVer36ToVer40( TMt &tMt, TMtVer36 &tMtVer36 );
	static void MtVer40ToVer36( TMtVer36 &tMtVer36, TMt &tMt );
	
	static void RecProgVer36ToVer40( TRecProg &tRecProg, TRecProgVer36 &tRecProgVer36 );
	static void RecProgVer40ToVer36( TRecProgVer36 &tRecProgVer36, TRecProg &tRecProg );

	static void PollInfoVer36ToVer40( TPollInfo &tPollInfo, TPollInfoVer36 &tPollInfoVer36 );
	static void PollInfoVer40ToVer36( TPollInfoVer36 &tPollInfoVer36, TPollInfo &tPollInfo );
	
	static void VMPParamVer36ToVer40( TVMPParamV4R4B2 &tVMPParam, TVMPParamVer36 &tVMPParamVer36 );
	static void VMPParamVer40ToVer36( TVMPParamVer36 &tVMPParamVer36, TVMPParamV4R4B2 &tVMPParam );
	
	static void VMPMemberVer36ToVer40( TVMPMember &tVMPMember, TVMPMemberVer36 &tVMPMemberVer36 );
	static void VMPMemberVer40ToVer36( TVMPMemberVer36 &tVMPMemberVer36, TVMPMember &tVMPMember );
	
	static void MtPollParamVer36ToVer40( TMtPollParam &tMtPollParam, TMtPollParamVer36 &tMtPollParamVer36 );
	static void MtPollParamVer40ToVer36( TMtPollParamVer36 &tMtPollParamVer36, TMtPollParam &tMtPollParam );

	//已经启用的类型兼容适配
	static u8 MediaTypeVer36ToVer40( u8 byMediaTypeVer36 );
	static u8 MediaTypeVer40ToVer36( u8 byMediaType40 );
	
	static u8 VideoFormatVer36ToVer40( u8 byVideoFormatVer36 );
	static u8 VideoFormatVer40ToVer36( u8 byVideoFormat40 );
	
	static u8 ConfBasModeVer36ToVer40( u8 byConfBasModeVer36 );
	static u8 ConfBasModeVer40ToVer36( u8 byConfBasMode40 );
	
	static u8 ConfCallModeVer36ToVer40( u8 byConfCallModeVer36 );
	static u8 ConfCallModeVer40ToVer36( u8 byConfCallMode40 );

	static void ConfStatusVer36ToVer40V4R4B2( TConfStatusV4R4B2 &tConfStatus, TConfStatusVer36 &tConfStatusVer36 );
	static void ConfStatusVer40V4R4B2ToVer36( TConfStatusVer36 &tConfStatusVer36, TConfStatusV4R4B2 &tConfStatus );
	
    static void ConfStatusVer40V4R4B2ToVer40V4R5( TConfStatusV4R5 &tConfStatus, TConfStatusV4R4B2 &tConfStatusV4R4B2 );
    static void ConfStatusV4R5ToV4R4B2( TConfStatusV4R4B2 &tConfStatusV4R4B2, TConfStatusV4R5 &tConfStatus );

	static void ConfModeVer36ToVer40V4R4B2( TConfModeV4R4B2 &tConfMode, TConfModeVer36 &tConfModeVer36 );
	static void ConfModeVer40V4R4B2ToVer36( TConfModeVer36 &tConfModeVer36, TConfModeV4R4B2 &tConfMode );
	
    static void ConfModeVer40V4R4B2ToVer40Daily( TConfMode &tConfMode, TConfModeV4R4B2 &tConfModeR4V4B2 );
    static void ConfModeVer40DailyToVer40V4R4B2( TConfModeV4R4B2 &tConfModeR4V4B2, TConfMode &tConfMode );

	static void DiscussParamVer36ToVer40V4R4B2( TDiscussParamV4R4B2 &tDiscussParam, TDiscussParamVer36 &tDiscussParamVer36 );
	static void DiscussParamVer40V4R4B2ToVer36( TDiscussParamVer36 &tDiscussParamVer36, TDiscussParamV4R4B2 &tDiscussParam );
	
    static void DiscussParamVer40V4R4B2ToVer40Daily( TMixParamV4R5 &tDiscussParam, TDiscussParamV4R4B2 &tDiscussParamVer36 );
    static void DiscussParamVer40DailyToVer40V4R4B2( TDiscussParamV4R4B2 &tDiscussParamVer36, TMixParamV4R5 &tDiscussParam );

	static void ConfStatusV4R5ToV4R6( TConfStatus &tConfStatus, TConfStatusV4R5 &tConfStatusV4R5 );
	static void ConfStatusV4R6ToV4R5( TConfStatusV4R5 &tConfStatusV4R5, TConfStatus &tConfStatus );

	//还未启用的类型兼容适配
	static u8 ActiveMediaTypeVer36ToVer40( u8 byActiveMediaTypeVer36 );
	static u8 ActiveMediaTypeVer40ToVer36( u8 byActiveMediaType40 );
	
	static u8 EqpMainTypeVer36ToVer40( u8 byEqpMainTypeVer36 );
	static u8 EqpMainTypeVer40ToVer36( u8 byEqpMainType40 );
	
	static u8 MediaModeVer36ToVer40( u8 byMediaModeVer36 );
	static u8 MediaModeVer40ToVer36( u8 byMediaMode40 );
	
	static u8 VideoDStreamTypeVer36ToVer40( u8 byDStreamTypeVer36 );
	static u8 VideoDStreamTypeVer40ToVer36( u8 byDStreamType40 );
	
	static u8 ConfOpenModeVer36ToVer40( u8 byConfOpenModeVer36 );
	static u8 ConfOpenModeVer40ToVer36( u8 byConfOpenMode40 );
	
	static u8 ConfEncryptModeVer36ToVer40( u8 byConfEncryptModeVer36 );
	static u8 ConfEncryptModeVer40ToVer36( u8 byConfEncryptMode40 );
	
	static u8 ConfUniformModeVer36ToVer40( u8 byConfUniformModeVer36 );
	static u8 ConfUniformModeVer40ToVer36( u8 byConfUniformMode40 );
	
	static u8 SwitchChnnlUniformModeVer36ToVer40( u8 bySwitchChnnlUniformModeVer36 );
	static u8 SwitchChnnlUniformModeVer40ToVer36( u8 bySwitchChnnlUniformMode40 );
	
	static u8 ConfDataModeVer36ToVer40( u8 byConfDataModeVer36 );
	static u8 ConfDataModeVer40ToVer36( u8 byConfDataMode40 );
	
	static u8 ConfReleaseModeVer36ToVer40( u8 byConfReleaseModeVer36 );
	static u8 ConfReleaseModeVer40ToVer36( u8 byConfReleaseMode40 );
	
	static u8 ConfTakeModeVer36ToVer40( u8 byConfTakeModeVer36 );
	static u8 ConfTakeModeVer40ToVer36( u8 byConfTakeMode40 );
	
	static u8 ConfLockModeVer36ToVer40( u8 byConfLockModeVer36 );
	static u8 ConfLockModeVer40ToVer36( u8 byConfLockMode40 );
	
	static u8 ConfSpeakerSrcVer36ToVer40( u8 byConfSpeakerSrcVer36 );
	static u8 ConfSpeakerSrcVer40ToVer36( u8 byConfSpeakerSrc40 );
	
	static u8 ConfVmpModeVer36ToVer40( u8 byConfVmpModeVer36 );
	static u8 ConfVmpModeVer40ToVer36( u8 byConfVmpMode40 );
	
	static u8 ConfRecModeVer36ToVer40( u8 byConfRecModeVer36 );
	static u8 ConfRecModeVer40ToVer36( u8 byConfRecMode40 );
	
	static u8 ConfPlayModeVer36ToVer40( u8 byConfPlayModeVer36 );
	static u8 ConfPlayModeVer40ToVer36( u8 byConfPlayMode40 );
	
	static u8 ConfPollModeVer36ToVer40( u8 byConfPollModeVer36 );
	static u8 ConfPollModeVer40ToVer36( u8 byConfPollMode40 );
	
	static u8 VmpMemberTypeVer36ToVer40( u8 byVmpMemberTypeVer36 );
	static u8 VmpMemberTypeVer40ToVer36( u8 byVmpMemberType40 );
	
	static u8 VmpStyleTypeVer36ToVer40( u8 byVmpStyleTypeVer36 );
	static u8 VmpStyleTypeVer40ToVer36( u8 byVmpStyleType40 ); 

}
PACKED;

inline void CStructConvert::ConfIdVer36ToVer40( CConfId &cConfId, CConfIdVer36 &cConfIdVer36 )
{
	u8 abyConfId[MAXLEN_CONFID];
	u8 byLen = 0;
	cConfIdVer36.GetConfId( abyConfId, sizeof(abyConfId) );
	cConfId.SetConfId( abyConfId, sizeof(abyConfId) );
}

inline void CStructConvert::ConfIdVer40ToVer36( CConfIdVer36 &cConfIdVer36, CConfId &cConfId )
{
	u8 abyConfId[MAXLEN_CONFID];
	u8 byLen = 0;
	cConfId.GetConfId( abyConfId, sizeof(abyConfId) );
	cConfIdVer36.SetConfId( abyConfId, sizeof(abyConfId) );
}

inline void CStructConvert::KdvTimeVer36ToVer40( TKdvTime &tKdvTime, TKdvTimeVer36 &tKdvTimeVer36 )
{
	tKdvTime.SetDay( tKdvTimeVer36.GetDay() );
	tKdvTime.SetHour( tKdvTimeVer36.GetHour() );
	tKdvTime.SetMinute( tKdvTimeVer36.GetMinute() );
	tKdvTime.SetMonth( tKdvTimeVer36.GetMonth() );
	tKdvTime.SetSecond( tKdvTimeVer36.GetSecond() );
	tKdvTime.SetYear( tKdvTimeVer36.GetYear() );
}

inline void CStructConvert::KdvTimeVer40ToVer36( TKdvTimeVer36 &tKdvTimeVer36, TKdvTime &tKdvTime )
{
	tKdvTimeVer36.SetDay( tKdvTime.GetDay() );
	tKdvTimeVer36.SetHour( tKdvTime.GetHour() );
	tKdvTimeVer36.SetMinute( tKdvTime.GetMinute() );
	tKdvTimeVer36.SetMonth( tKdvTime.GetMonth() );
	tKdvTimeVer36.SetSecond( tKdvTime.GetSecond() );
	tKdvTimeVer36.SetYear( tKdvTime.GetYear() );
}

inline void CStructConvert::MtAliasVer36ToVer40( TMtAlias &tMtAlias, TMtAliasVer36 &tMtAliasVer36 )
{
	strncpy( tMtAlias.m_achAlias, tMtAliasVer36.m_achAlias, sizeof(tMtAliasVer36.m_achAlias) );
	tMtAlias.m_AliasType				 = tMtAliasVer36.m_AliasType;
	tMtAlias.m_tTransportAddr.m_dwIpAddr = tMtAliasVer36.m_tTransportAddr.m_dwIpAddr;
	tMtAlias.m_tTransportAddr.m_wPort    = tMtAliasVer36.m_tTransportAddr.m_wPort;
	
	return;
}

inline void CStructConvert::MtAliasVer40ToVer36( TMtAliasVer36 &tMtAliasVer36, TMtAlias &tMtAlias )
{
	strncpy( tMtAliasVer36.m_achAlias, tMtAlias.m_achAlias, sizeof(tMtAlias.m_achAlias) );
	tMtAliasVer36.m_AliasType				  = tMtAlias.m_AliasType;
	tMtAliasVer36.m_tTransportAddr.m_dwIpAddr = tMtAlias.m_tTransportAddr.m_dwIpAddr;
	tMtAliasVer36.m_tTransportAddr.m_wPort    = tMtAlias.m_tTransportAddr.m_wPort;
	
	return;
}

inline void CStructConvert::ConfAttribVer36ToVer40( TConfAttrb &tConfAttrb, TConfAttrbVer36 &tConfAttrbVer36 )
{
	tConfAttrb.SetOpenMode( tConfAttrbVer36.GetOpenMode() );
	tConfAttrb.SetEncryptMode( tConfAttrbVer36.GetEncryptMode() );
	tConfAttrb.SetMulticastMode( tConfAttrbVer36.IsMulticastMode() ? TRUE : FALSE );
	tConfAttrb.SetReleaseMode( tConfAttrbVer36.IsReleaseNoMt() ? TRUE : FALSE );
	tConfAttrb.SetQualityPri( tConfAttrbVer36.IsQualityPri() ? TRUE : FALSE );	
	tConfAttrb.SetDataMode( tConfAttrbVer36.GetDataMode() );
	tConfAttrb.SetSupportCascade( tConfAttrbVer36.IsSupportCascade() );
	tConfAttrb.SetPrsMode( tConfAttrbVer36.IsResendLosePack() );
	tConfAttrb.SetHasTvWallModule( tConfAttrbVer36.IsHasTvWallModule() );
	tConfAttrb.SetHasVmpModule( tConfAttrbVer36.IsHasVmpModule() );
	tConfAttrb.SetDiscussConf( tConfAttrbVer36.IsDiscussConf() );
	tConfAttrb.SetUseAdapter( tConfAttrbVer36.IsUseAdapter() );
	tConfAttrb.SetSpeakerSrc( tConfAttrbVer36.GetSpeakerSrc() );
	tConfAttrb.SetSpeakerSrcMode( tConfAttrbVer36.GetSpeakerSrcMode() );
	tConfAttrb.SetMulcastLowStream( tConfAttrbVer36.IsMulcastLowStream() );
	tConfAttrb.SetAllInitDumb( tConfAttrbVer36.IsAllInitDumb() );
	tConfAttrb.SetUniformMode( tConfAttrbVer36.IsAdjustUniformPack() );
	
	return;
}

inline void CStructConvert::ConfAttribVer40ToVer36( TConfAttrbVer36 &tConfAttrbVer36, TConfAttrb &tConfAttrb )
{
	tConfAttrbVer36.SetOpenMode( tConfAttrb.GetOpenMode() );
	tConfAttrbVer36.SetEncryptMode( tConfAttrb.GetEncryptMode() );
	tConfAttrbVer36.SetMulticastMode( tConfAttrb.IsMulticastMode() ? TRUE : FALSE );
	tConfAttrbVer36.SetReleaseMode( tConfAttrb.IsReleaseNoMt() ? TRUE : FALSE );
	tConfAttrbVer36.SetQualityPri( tConfAttrb.IsQualityPri() ? TRUE : FALSE );	
	tConfAttrbVer36.SetDataMode( tConfAttrb.GetDataMode() );
	tConfAttrbVer36.SetSupportCascade( tConfAttrb.IsSupportCascade() );
	tConfAttrbVer36.SetPrsMode( tConfAttrb.IsResendLosePack() );
	tConfAttrbVer36.SetHasTvWallModule( tConfAttrb.IsHasTvWallModule() );
	tConfAttrbVer36.SetHasVmpModule( tConfAttrb.IsHasVmpModule() );
	tConfAttrbVer36.SetDiscussConf( tConfAttrb.IsDiscussConf() );
	tConfAttrbVer36.SetUseAdapter( tConfAttrb.IsUseAdapter() );
	tConfAttrbVer36.SetSpeakerSrc( tConfAttrb.GetSpeakerSrc() );
	tConfAttrbVer36.SetSpeakerSrcMode( tConfAttrb.GetSpeakerSrcMode() );
	tConfAttrbVer36.SetMulcastLowStream( tConfAttrb.IsMulcastLowStream() );
	tConfAttrbVer36.SetAllInitDumb( tConfAttrb.IsAllInitDumb() );
	tConfAttrbVer36.SetUniformMode( tConfAttrb.IsAdjustUniformPack() );
	
	return;
}

inline void CStructConvert::CapSupportVer36ToVer40( TCapSupport &tCapSupport, TCapSupportVer36 &tCapSupportVer36 )
{
	//主同时能力集
	TSimCapSet tMainSimCapSet;
	if ( !tCapSupportVer36.GetMainSimCapSet().IsNull() )
	{
		tMainSimCapSet.SetAudioMediaType( MediaTypeVer36ToVer40( tCapSupportVer36.GetMainSimCapSet().GetAudioMediaType() ) );
		tMainSimCapSet.SetVideoMediaType( MediaTypeVer36ToVer40( tCapSupportVer36.GetMainSimCapSet().GetVideoMediaType() ) );
		tCapSupport.SetMainSimCapSet( tMainSimCapSet );
	}
	else
	{
		tCapSupport.GetMainSimCapSet().Clear();
	}
	//辅同时能力集
	TSimCapSet tSecSimCapSet;
	if ( !tCapSupportVer36.GetSecondSimCapSet().IsNull() )
	{
		tSecSimCapSet.SetAudioMediaType( MediaTypeVer36ToVer40( tCapSupportVer36.GetSecondSimCapSet().GetAudioMediaType() ) );
		tSecSimCapSet.SetVideoMediaType( MediaTypeVer36ToVer40( tCapSupportVer36.GetSecondSimCapSet().GetVideoMediaType() ) );
		tCapSupport.SetSecondSimCapSet( tSecSimCapSet );
	}
	else
	{
		tCapSupport.GetSecondSimCapSet().Clear();
	}
	//合并级联
	tCapSupport.SetSupportMMcu( tCapSupportVer36.IsSupportMMcu() );
	
	//数据
	tCapSupport.SetSupportH224( tCapSupportVer36.GetSupportH224() );
	tCapSupport.SetSupportT120( tCapSupportVer36.GetSupportT120() );
	
	//239双流: 3.6版本的GetSupportH239()功能重用, 具体为:
	//(1) FALSE表示不支持; (2) !FALSE不仅表示使用, 其值还表示具体的H239双流类型
	if ( FALSE == tCapSupportVer36.GetSupportH239() )
	{
		tCapSupport.SetDStreamSupportH239( FALSE );
	}
	else
	{
		tCapSupport.SetDStreamSupportH239( TRUE );
		tCapSupport.SetDStreamMediaType( tCapSupportVer36.GetSupportH239() );
	}
	//263+双流
	if ( tCapSupportVer36.GetSupportH263Plus() )
	{
		tCapSupport.SetDStreamMediaType( MEDIA_TYPE_H263PLUS );
	}
	//加密: 取3.6版本中的加密模式中的主模式, 辅模式默认为空, 这里不作判断
	tCapSupport.SetEncryptMode( tCapSupportVer36.GetMainEncryptMode() );
	
	return;
}

inline void CStructConvert::CapSupportVer40ToVer36( TCapSupportVer36 &tCapSupportVer36, TCapSupport &tCapSupport )
{
	//主同时能力集
	TSimCapSetVer36 tMainSimCapSet36;
	if ( !tCapSupport.GetMainSimCapSet().IsNull() ) 
	{
		tMainSimCapSet36.SetAudioMediaType( MediaTypeVer40ToVer36( tCapSupport.GetMainSimCapSet().GetAudioMediaType() ) );
		tMainSimCapSet36.SetVideoMediaType( MediaTypeVer40ToVer36( tCapSupport.GetMainSimCapSet().GetVideoMediaType() ) );
		tMainSimCapSet36.SetDataMediaType1( tCapSupport.IsSupportH224() ? MEDIA_TYPE_H224 : MEDIA_TYPE_NULL );
		tMainSimCapSet36.SetDataMediaType2( tCapSupport.IsSupportT120() ? MEDIA_TYPE_T120 : MEDIA_TYPE_NULL );
		tCapSupportVer36.SetMainSimCapSet( tMainSimCapSet36 );
	}
	else
	{
		tCapSupportVer36.GetMainSimCapSet().SetNull();
	}
	//辅的同时能力集
	TSimCapSetVer36 tSecSimCapSet36;
	if ( !tCapSupport.GetSecondSimCapSet().IsNull() )
	{
		tSecSimCapSet36.SetAudioMediaType( MediaTypeVer40ToVer36( tCapSupport.GetSecondSimCapSet().GetAudioMediaType() ) );
		tSecSimCapSet36.SetVideoMediaType( MediaTypeVer40ToVer36( tCapSupport.GetSecondSimCapSet().GetVideoMediaType() ) );
		tCapSupportVer36.SetSecondSimCapSet( tSecSimCapSet36 );
	}
	else
	{
		tCapSupportVer36.GetSecondSimCapSet().SetNull();
	}
	//合并级联
	tCapSupportVer36.SetSupportMMcu( tCapSupport.IsSupportMMcu() );
	
	//数据
	tCapSupportVer36.SetSupportH224( tCapSupport.IsSupportH224() );
	tCapSupportVer36.SetSupportT120( tCapSupport.IsSupportT120() );
	
	//239双流: 3.6版本的GetSupportH239()功能重用, 具体为:
	//(1) FALSE表示不支持; (2) !FALSE不仅表示使用, 其值还表示具体的H239双流类型
	if ( FALSE == tCapSupport.IsDStreamSupportH239() ) 
	{
		tCapSupportVer36.SetSupportH239( FALSE );
	}
	else
	{
		tCapSupportVer36.SetSupportH239( tCapSupport.GetDStreamMediaType() );
	}
	
	//263+双流
	if ( MEDIA_TYPE_H263PLUS == tCapSupport.GetDStreamMediaType() )
	{
		tCapSupportVer36.SetSupportH263Plus( TRUE );
	}
	//加密: 取3.6版本中的加密模式中的主模式, 辅模式默认为空, 这里不作判断
	tCapSupportVer36.SetMainEncryptMode( tCapSupport.GetEncryptMode() );
	tCapSupportVer36.SetSecondEncryptMode( NULL );
	
	return;
}

inline void CStructConvert::MediaEncryptVer36ToVer40( TMediaEncrypt &tMediaKey, TMediaEncryptVer36 &tEncryptKeyVer36 )
{
	s32 nKeyLen = 0;
	u8  abyKey[MAXLEN_KEY] = {0};
	tEncryptKeyVer36.GetEncryptKey( abyKey, &nKeyLen );
	tMediaKey.SetEncryptKey( abyKey, nKeyLen );
	tMediaKey.SetEncryptMode( tEncryptKeyVer36.GetEncryptMode() );
	
	return;
}

inline void CStructConvert::MediaEncryptVer40ToVer36( TMediaEncryptVer36 &tEncryptKeyVer36, TMediaEncrypt &tEncryptKey )
{
	s32 nKeyLen = 0;
	u8  abyKey[MAXLEN_KEY] = {0};
	tEncryptKey.GetEncryptKey( abyKey, &nKeyLen );	
	tEncryptKeyVer36.SetEncryptKey( abyKey, nKeyLen );
	tEncryptKeyVer36.SetEncryptMode( tEncryptKey.GetEncryptMode() );
	return;
}

inline void CStructConvert::MtVer36ToVer40( TMt &tMt, TMtVer36 &tMtVer36 )
{
	tMtVer36.SetType( tMtVer36.GetType() );
	
	if ( TYPE_MCU == tMtVer36.GetType() )
	{
		tMt.SetMcu( tMtVer36.GetMcuId() );
		tMt.SetMcuId( tMtVer36.GetMcuId() );
		tMt.SetMcuType( tMtVer36.GetMcuType() );
	}
	if ( TYPE_MCUPERI == tMtVer36.GetType() )
	{
		tMt.SetMcuEqp( tMtVer36.GetMcuId(), tMtVer36.GetEqpId(), tMtVer36.GetEqpType() );
		tMt.SetEqpId( tMtVer36.GetEqpId() );
		tMt.SetEqpType( tMtVer36.GetEqpType() );
	}
	if ( TYPE_MT == tMtVer36.GetType() )
	{
		tMt.SetMt( tMtVer36.GetMcuId(), tMtVer36.GetMtId(), tMtVer36.GetDriId(), tMtVer36.GetConfIdx() );
		tMt.SetMtId( tMtVer36.GetMtId() );
		tMt.SetMtType( tMtVer36.GetMtType() );
	}
	tMt.SetDriId( tMtVer36.GetDriId() );
	tMt.SetConfIdx( tMtVer36.GetConfIdx() );
	
	return;
}

inline void CStructConvert::MtVer40ToVer36( TMtVer36 &tMtVer36, TMt &tMt )
{
	tMtVer36.SetType( tMt.GetType() );
	
	if ( TYPE_MCU == tMt.GetType() )
	{
		tMtVer36.SetMcu( (u8)tMt.GetMcuId() );
		tMtVer36.SetMcuId( (u8)tMt.GetMcuId() );
		tMtVer36.SetMcuType( tMt.GetMcuType() );
	}
	if ( TYPE_MCUPERI == tMt.GetType() )
	{
		tMtVer36.SetMcuEqp( (u8)tMt.GetMcuId(), tMt.GetEqpId(), tMt.GetEqpType() );
		tMtVer36.SetEqpId( tMt.GetEqpId() );
		tMtVer36.SetEqpType( tMt.GetEqpType() );
	}
	if ( TYPE_MT == tMt.GetType() )
	{
		tMtVer36.SetMt( (u8)tMt.GetMcuId(), tMt.GetMtId(), tMt.GetDriId(), tMt.GetConfIdx() );
		tMtVer36.SetMtId( tMt.GetMtId() );
		tMtVer36.SetMtType( tMt.GetMtType() );
	}
	tMtVer36.SetDriId( tMt.GetDriId() );
	tMtVer36.SetConfIdx( tMt.GetConfIdx() );
	
	return;
}

inline void CStructConvert::RecProgVer36ToVer40( TRecProg &tRecProg, TRecProgVer36 &tRecProgVer36 )
{
	tRecProg.SetCurProg( tRecProgVer36.GetCurProg() );
	tRecProg.SetTotalTime( tRecProgVer36.GetTotalTime() );
	return;
}

inline void CStructConvert::RecProgVer40ToVer36( TRecProgVer36 &tRecProgVer36, TRecProg &tRecProg )
{
	tRecProgVer36.SetCurProg( tRecProg.GetCurProg() );
	tRecProgVer36.SetTotalTime( tRecProg.GetTotalTime() );
	return;
}

inline void CStructConvert::PollInfoVer36ToVer40( TPollInfo &tPollInfo, TPollInfoVer36 &tPollInfoVer36 )
{
	//3.6不指定轮询次数, 4.0默认填0, 表示无限轮询
	tPollInfo.SetPollNum( 0 );
	tPollInfo.SetMediaMode( tPollInfoVer36.GetMediaMode() );
	tPollInfo.SetPollState( tPollInfoVer36.GetPollState() );
	
	TMtPollParamVer36 tMtPollParam36;
	memset( &tMtPollParam36, 0, sizeof(tMtPollParam36) );
	tMtPollParam36 = tPollInfoVer36.GetMtPollParam();
	
	TMtPollParam tMtPollParam;
	memset( &tMtPollParam, 0, sizeof(tMtPollParam) );
	MtPollParamVer36ToVer40( tMtPollParam, tMtPollParam36 );
	tPollInfo.SetMtPollParam( tMtPollParam );
	
	return;
}

inline void CStructConvert::PollInfoVer40ToVer36( TPollInfoVer36 &tPollInfoVer36, TPollInfo &tPollInfo )
{
	tPollInfoVer36.SetMediaMode( tPollInfo.GetMediaMode() );
	tPollInfoVer36.SetPollState( tPollInfo.GetPollState() );
	
	TMtPollParam tMtPollParam;
	memset( &tMtPollParam, 0, sizeof(tMtPollParam) );
	tMtPollParam = tPollInfo.GetMtPollParam();
	
	TMtPollParamVer36 tMtPollParam36;
	memset( &tMtPollParam36, 0, sizeof(tMtPollParam36) );
	MtPollParamVer40ToVer36( tMtPollParam36, tMtPollParam );
	tPollInfoVer36.SetMtPollParam( tMtPollParam36 );
	
	return;
}

inline void CStructConvert::VMPParamVer36ToVer40( TVMPParamV4R4B2 &tVMPParam, TVMPParamVer36 &tVMPParamVer36 )
{
	tVMPParam.SetVMPAuto( tVMPParamVer36.IsVMPAuto() );
	tVMPParam.SetVMPBrdst( tVMPParamVer36.IsVMPBrdst() );
	tVMPParam.SetVMPStyle( tVMPParamVer36.GetVMPStyle() );
	tVMPParam.SetVMPSchemeId( tVMPParamVer36.GetMaxMemberNum() );
	
	for( s32 nIndex = 0; nIndex < MAXNUM_MPUSVMP_MEMBER; nIndex ++ )
	{
		if ( NULL != tVMPParamVer36.GetVmpMember(nIndex) ) 
		{
			TVMPMember tVmpMember;
			memset( &tVmpMember, 0, sizeof(tVmpMember) );
			VMPMemberVer36ToVer40( tVmpMember, *tVMPParamVer36.GetVmpMember(nIndex) );
			tVMPParam.SetVmpMember( nIndex, tVmpMember );
		}
	}	
	return;
}

inline void CStructConvert::VMPParamVer40ToVer36( TVMPParamVer36 &tVMPParamVer36, TVMPParamV4R4B2 &tVMPParam )
{
	tVMPParamVer36.SetVMPAuto( tVMPParam.IsVMPAuto() );
	tVMPParamVer36.SetVMPBrdst( tVMPParam.IsVMPBrdst() );
	tVMPParamVer36.SetVMPStyle( tVMPParam.GetVMPStyle() );
	
	for( s32 nIndex = 0; nIndex < MAXNUM_VMP_MEMBER_VER36; nIndex ++ )
	{
		if ( NULL != tVMPParam.GetVmpMember(nIndex) ) 
		{
			TVMPMemberVer36 tVMPMember36;
			memset( &tVMPMember36, 0, sizeof(tVMPMember36) );
			VMPMemberVer40ToVer36( tVMPMember36, *tVMPParam.GetVmpMember(nIndex) );
			tVMPParamVer36.SetVmpMember( nIndex, tVMPMember36 );
		}
	}
	return;
}

inline void CStructConvert::TERSTATUSVer36ToVer40( _TTERSTATUS &tTerStatus, _TTERSTATUS_VER36 &tTerStatus36 )
{
	//tTerStatus.SetPollStat( tTerStatus36.byPollStat );
    // guzh [3/7/2007] 3.6 不支持VideoLose状态上报，忽略
    tTerStatus.SetVideoLose( FALSE );
	tTerStatus.SetBoardType( tTerStatus36.byMtBoardType );
	tTerStatus.SetMatrixStatus( tTerStatus36.bMatrixStatus );
	tTerStatus.SetVideoSrc( tTerStatus36.byVideoSrc );
	tTerStatus.SetRemoteCtrlEnalbe( tTerStatus36.bRemoteCtrl );
	tTerStatus.SetInputVolume( tTerStatus36.byEncVol );
	tTerStatus.SetOutputVolume( tTerStatus36.byDecVol );
	tTerStatus.SetMute( tTerStatus36.bMute );
	tTerStatus.SetQuiet( tTerStatus36.bQuiet );
	tTerStatus.SetExVidSrcNum( 0 );		// 0 表示没有外部视频源
	
	return;
}

inline void CStructConvert::TERSTATUSVer40ToVer36( _TTERSTATUS_VER36 &tTerStatus36, _TTERSTATUS &tTerStatus )
{
    // guzh [3/7/2007] 4.0不再记录 pollstat
	// tTerStatus36.byPollStat = tTerStatus.GetPollStat();
    tTerStatus36.byPollStat = poll_none;
	tTerStatus36.byMtBoardType = tTerStatus.GetBoardType();
	tTerStatus36.bMatrixStatus = tTerStatus.GetMatrixStatus();
	tTerStatus36.byVideoSrc = tTerStatus.GetVideoSrc();
	tTerStatus36.bMute = tTerStatus.IsMute();
	tTerStatus36.bQuiet = tTerStatus.IsQuiet();
	tTerStatus36.byEncVol = tTerStatus.GetInputVolume();
	tTerStatus36.byDecVol = tTerStatus.GetOutputVolume();
	
	return;
}

inline void CStructConvert::VMPMemberVer36ToVer40( TVMPMember &tVMPMember, TVMPMemberVer36 &tVMPMemberVer36 )
{
	//3.6中不指定画面合成成员状态, 4.0中默认为观众
	tVMPMember.SetMemStatus( MT_STATUS_AUDIENCE ); 	
	tVMPMember.SetMemberType( tVMPMemberVer36.GetMemberType() );
	return;
}

inline void CStructConvert::VMPMemberVer40ToVer36( TVMPMemberVer36 &tVMPMemberVer36, TVMPMember &tVMPMember )
{
	tVMPMemberVer36.SetMemberType( tVMPMember.GetMemberType() );
	return;
}

inline void CStructConvert::MtPollParamVer36ToVer40( TMtPollParam &tMtPollParam, TMtPollParamVer36 &tMtPollParamVer36 )
{
	//3.6不指定轮询次数, 4.0默认填0, 表示无限轮询
	tMtPollParam.SetPollNum( 0 );
	tMtPollParam.SetKeepTime( tMtPollParamVer36.GetKeepTime() );
	
	return;
}

inline void CStructConvert::MtPollParamVer40ToVer36( TMtPollParamVer36 &tMtPollParamVer36, TMtPollParam &tMtPollParam )
{
	tMtPollParamVer36.SetKeepTime( tMtPollParam.GetKeepTime() );
	return;
}

inline u8 CStructConvert::MediaTypeVer36ToVer40( u8 byMediaType36 )
{
	switch( byMediaType36 )
	{
		//音频
	case MEDIA_TYPE_MP3_VER36:		return MEDIA_TYPE_MP3;
	case MEDIA_TYPE_MP2_VER36:		return MEDIA_TYPE_H262;
	case MEDIA_TYPE_PCMA_VER36:		return MEDIA_TYPE_PCMA;
	case MEDIA_TYPE_PCMU_VER36:		return MEDIA_TYPE_PCMU;
	case MEDIA_TYPE_G721_VER36:		return MEDIA_TYPE_G721;
	case MEDIA_TYPE_G722_VER36:		return MEDIA_TYPE_G722;
	case MEDIA_TYPE_G7231_VER36:	return MEDIA_TYPE_G7231;
	case MEDIA_TYPE_G728_VER36:		return MEDIA_TYPE_G728;
	case MEDIA_TYPE_G729_VER36:		return MEDIA_TYPE_G729;
	case MEDIA_TYPE_G7221_VER36:	return MEDIA_TYPE_G7221C;
		//视频
	case MEDIA_TYPE_MP4_VER36:		return MEDIA_TYPE_MP4;
	case MEDIA_TYPE_H261_VER36:		return MEDIA_TYPE_H261;
	case MEDIA_TYPE_H262_VER36:		return MEDIA_TYPE_H262;
	case MEDIA_TYPE_H263_VER36:		return MEDIA_TYPE_H263;
	case MEDIA_TYPE_H263PLUS_VER36:	return MEDIA_TYPE_H263PLUS;
	case MEDIA_TYPE_H264_VER36:		return MEDIA_TYPE_H264;
		//数据
	case MEDIA_TYPE_H224_VER36:		return MEDIA_TYPE_H224;
	case MEDIA_TYPE_T120_VER36:		return MEDIA_TYPE_T120;
	case MEDIA_TYPE_H239_VER36:		return MEDIA_TYPE_H239;
	case MEDIA_TYPE_MMCU_VER36:		return MEDIA_TYPE_MMCU;
	case MEDIA_TYPE_NULL_VER36:		return MEDIA_TYPE_NULL;
	default:						return MEDIA_TYPE_NULL;
	}
}

inline u8 CStructConvert::MediaTypeVer40ToVer36( u8 byMediaType40 )
{
	switch( byMediaType40 )
	{
		//音频
	case MEDIA_TYPE_MP3:			return MEDIA_TYPE_MP3_VER36;
	case MEDIA_TYPE_PCMA:			return MEDIA_TYPE_PCMA_VER36;
	case MEDIA_TYPE_PCMU:			return MEDIA_TYPE_PCMU_VER36;
	case MEDIA_TYPE_G721:			return MEDIA_TYPE_G721_VER36;
	case MEDIA_TYPE_G722:			return MEDIA_TYPE_G722_VER36;
	case MEDIA_TYPE_G7231:			return MEDIA_TYPE_G7231_VER36;
	case MEDIA_TYPE_G728:			return MEDIA_TYPE_G728_VER36;
	case MEDIA_TYPE_G729:			return MEDIA_TYPE_G729_VER36;
	case MEDIA_TYPE_G7221C:			return MEDIA_TYPE_G7221_VER36;
		//视频
	case MEDIA_TYPE_MP4:			return MEDIA_TYPE_MP4_VER36;
	case MEDIA_TYPE_H261:			return MEDIA_TYPE_H261_VER36;
	case MEDIA_TYPE_H262:			return MEDIA_TYPE_H262_VER36;
	case MEDIA_TYPE_H263:			return MEDIA_TYPE_H263_VER36;
	case MEDIA_TYPE_H263PLUS:		return MEDIA_TYPE_H263PLUS_VER36;
	case MEDIA_TYPE_H264:			return MEDIA_TYPE_H264_VER36;
		//数据
	case MEDIA_TYPE_H224:			return MEDIA_TYPE_H224_VER36;
	case MEDIA_TYPE_T120:			return MEDIA_TYPE_T120_VER36;
	case MEDIA_TYPE_H239:			return MEDIA_TYPE_H239_VER36;
	case MEDIA_TYPE_MMCU:			return MEDIA_TYPE_MMCU_VER36;
	case MEDIA_TYPE_NULL:			return MEDIA_TYPE_NULL_VER36;
	default:						return MEDIA_TYPE_NULL_VER36;
	}
}

inline u8 CStructConvert::VideoFormatVer36ToVer40( u8 byVideoFormat36 )
{
	switch( byVideoFormat36 )
	{
	case VIDEO_FORMAT_SQCIF_VER36:		return VIDEO_FORMAT_SQCIF;
	case VIDEO_FORMAT_QCIF_VER36:		return VIDEO_FORMAT_QCIF;
	case VIDEO_FORMAT_CIF_VER36:		return VIDEO_FORMAT_CIF;
	case VIDEO_FORMAT_4CIF_VER36:		return VIDEO_FORMAT_4CIF;
	case VIDEO_FORMAT_16CIF_VER36:		return VIDEO_FORMAT_16CIF;
	case VIDEO_FORMAT_AUTO_VER36:		return VIDEO_FORMAT_AUTO;
	case VIDEO_FORMAT_2CIF_VER36:		return VIDEO_FORMAT_2CIF;
	default:							return VIDEO_FORMAT_CIF;
	}
}

inline u8 CStructConvert::VideoFormatVer40ToVer36( u8 byVideoFormat40 )
{
	switch( byVideoFormat40 )
	{
	case VIDEO_FORMAT_SQCIF:		return VIDEO_FORMAT_SQCIF_VER36;
	case VIDEO_FORMAT_QCIF:			return VIDEO_FORMAT_QCIF_VER36;
	case VIDEO_FORMAT_CIF:			return VIDEO_FORMAT_CIF_VER36;
	case VIDEO_FORMAT_4CIF:			return VIDEO_FORMAT_4CIF_VER36;
	case VIDEO_FORMAT_16CIF:		return VIDEO_FORMAT_16CIF_VER36;
	case VIDEO_FORMAT_AUTO:			return VIDEO_FORMAT_AUTO_VER36;
	case VIDEO_FORMAT_2CIF:			return VIDEO_FORMAT_2CIF_VER36;
		//4.0新增分辨率
	case VIDEO_FORMAT_SIF:			return VIDEO_FORMAT_CIF_VER36;
	case VIDEO_FORMAT_2SIF:			return VIDEO_FORMAT_CIF_VER36;
	case VIDEO_FORMAT_4SIF:			return VIDEO_FORMAT_CIF_VER36;
	case VIDEO_FORMAT_VGA:			return VIDEO_FORMAT_CIF_VER36;
	case VIDEO_FORMAT_SVGA:			return VIDEO_FORMAT_CIF_VER36;
	case VIDEO_FORMAT_XGA:			return VIDEO_FORMAT_CIF_VER36;
	default:						return VIDEO_FORMAT_CIF_VER36;
	}
}

inline u8 CStructConvert::ConfBasModeVer36ToVer40( u8 byConfBasMode36 )
{
	switch( byConfBasMode36 )
	{
		//BasMode是终端不关心的一个参数, 这里转换的实现都默认填写NONE
	case CONF_BASMODE_BR_VER36:		return CONF_BASMODE_NONE;
	case CONF_BASMODE_RTP_VER36:	return CONF_BASMODE_NONE;
	case CONF_BASMODE_MDTP_VER36:	return CONF_BASMODE_NONE;
	case CONF_BASMODE_NONE_VER36:	return CONF_BASMODE_NONE;
	default:						return CONF_BASMODE_NONE;
	}
}

inline u8 CStructConvert::ConfBasModeVer40ToVer36( u8 byConfBasMode40 )
{
	switch( byConfBasMode40 )
	{
		//BasMode是终端不关心的一个参数, 这里转换的实现都默认填写NONE
	case CONF_BASMODE_BR:			return CONF_BASMODE_NONE_VER36;
	case CONF_BASMODE_AUD:			return CONF_BASMODE_NONE_VER36;
	case CONF_BASMODE_VID:			return CONF_BASMODE_NONE_VER36;
	case CONF_BASMODE_CASDAUD:		return CONF_BASMODE_NONE_VER36;
	case CONF_BASMODE_CASDVID:		return CONF_BASMODE_NONE_VER36;
	case CONF_BASMODE_NONE:			return CONF_BASMODE_NONE_VER36;
	default:						return CONF_BASMODE_NONE_VER36;
	}
}

inline u8 CStructConvert::ConfCallModeVer36ToVer40( u8 byConfCallMode36 )
{
	switch( byConfCallMode36 )
	{
	case CONF_CALLMODE_NONE_VER36:		return CONF_CALLMODE_NONE;
	case CONF_CALLMODE_ONCE_VER36:		return CONF_CALLMODE_TIMER;
	case CONF_CALLMODE_TIMER_VER36:		return CONF_CALLMODE_TIMER;
	default:							return CONF_CALLMODE_TIMER;
	}
}

inline u8 CStructConvert::ConfCallModeVer40ToVer36( u8 byConfCallMode40 )
{
	switch( byConfCallMode40 )
	{
	case CONF_CALLMODE_NONE:		return CONF_CALLMODE_NONE_VER36;
	case CONF_CALLMODE_TIMER:		return CONF_CALLMODE_TIMER_VER36;
	default:						return CONF_CALLMODE_TIMER_VER36;
	}
}

inline void CStructConvert::ConfInfoV36ToV4R6( TConfInfo &tConfInfo, TConfInfoVer36 &tConfInfoVer36 )
{
    TConfInfoV4R4B2 tConfInfoV4R4B2;

    ConfInfoVer36ToVer40V4R4B2(tConfInfoV4R4B2, tConfInfoVer36);	//3.6->4.0
	ConfInfoV4R4ToV4R6(tConfInfo, tConfInfoV4R4B2);					//4.0->4.6

    return;
}

inline void CStructConvert::ConfInfoV4R6ToV36( TConfInfoVer36 &tConfInfoVer36, TConfInfo &tConfInfo )
{
    TConfInfoV4R4B2 tConfInfoV4R4B2;

	ConfInfoV4R6ToV4R4(tConfInfoV4R4B2, tConfInfo);                 //4.6->4.0
    ConfInfoVer40V4R4B2ToVer36(tConfInfoVer36, tConfInfoV4R4B2);    //4.0->3.6
    return;
}

inline void CStructConvert::ConfInfoVer36ToVer40V4R4B2( TConfInfoV4R4B2 &tConfInfo, TConfInfoVer36 &tConfInfo36 )
{
	tConfInfo.SetDcsBitRate( 0 );
	tConfInfo.SetBitRate( tConfInfo36.GetBitRate() );
	tConfInfo.SetConfPwd( tConfInfo36.GetConfPwd() );
	tConfInfo.SetDuration( tConfInfo36.GetDuration() );
	tConfInfo.SetConfName( tConfInfo36.GetConfName() );
	tConfInfo.SetConfE164( tConfInfo36.GetConfE164() );
	tConfInfo.SetSecBitRate( tConfInfo36.GetSecBitRate() );
	tConfInfo.SetTalkHoldTime( tConfInfo36.GetTalkHoldTime() );
	
	//会议号
	CConfIdVer36 cConfId36;
	memset( &cConfId36, 0, sizeof(cConfId36) );
	cConfId36 = tConfInfo36.GetConfId();
	
	CConfId cConfId;
	memset( &cConfId, 0, sizeof(cConfId) );
	ConfIdVer36ToVer40( cConfId, cConfId36 );
	tConfInfo.SetConfId( cConfId );
	
	//时间结构
	TKdvTimeVer36 tKdvTime36;
	memset( &tKdvTime36, 0, sizeof(tKdvTime36) );
	tKdvTime36 = tConfInfo36.GetKdvStartTime();
	
	TKdvTime tKdvTime;
	memset( &tKdvTime, 0, sizeof(tKdvTime) );
	KdvTimeVer36ToVer40( tKdvTime, tKdvTime36 );
	tConfInfo.SetKdvStartTime( tKdvTime );
	
	//能力集
	TCapSupportVer36 tCapSupport36;
	tCapSupport36 = tConfInfo36.GetCapSupport();
	
	TCapSupport tCapSupport;
	CapSupportVer36ToVer40( tCapSupport, tCapSupport36 );
	tConfInfo.SetCapSupport( tCapSupport );
	
	//主席
	TMtAliasVer36 tChairAlias36;
	memset( &tChairAlias36, 0, sizeof(tChairAlias36) );
	tChairAlias36 = tConfInfo36.GetChairAlias();
	
	TMtAlias tChairAlias;
	memset( &tChairAlias, 0, sizeof(tChairAlias) );
	MtAliasVer36ToVer40( tChairAlias, tChairAlias36 );
	tConfInfo.SetChairAlias( tChairAlias );
	
	//发言人
	TMtAliasVer36 tSpeakerAlias36;
	memset( &tSpeakerAlias36, 0, sizeof(tSpeakerAlias36) );
	tSpeakerAlias36 = tConfInfo36.GetSpeakerAlias();
	
	TMtAlias tSpeakerAlias;
	memset( &tSpeakerAlias, 0, sizeof(tSpeakerAlias) );
	MtAliasVer36ToVer40( tSpeakerAlias, tSpeakerAlias36 );
	tConfInfo.SetSpeakerAlias( tSpeakerAlias );
	
	//会议属性
	TConfAttrbVer36 tConfAttrb36;
	memset( &tConfAttrb36, 0, sizeof(tConfAttrb36) );
	tConfAttrb36 = tConfInfo36.GetConfAttrb();
	
	TConfAttrb tConfAttrb;
	memset( &tConfAttrb, 0, sizeof(tConfAttrb) );
	ConfAttribVer36ToVer40( tConfAttrb, tConfAttrb36 ); 
	tConfInfo.SetConfAttrb( tConfAttrb );
	
	//媒体密码
	TMediaEncryptVer36 tMediaKey36;
	memset( &tMediaKey36, 0, sizeof(tMediaKey36) );
	tMediaKey36 = tConfInfo36.GetMediaKey();
	
	TMediaEncrypt tMediaKey;
	memset( &tMediaKey, 0, sizeof(tMediaKey) );
	MediaEncryptVer36ToVer40( tMediaKey, tMediaKey36 );
	tConfInfo.SetMediaKey( tMediaKey );
	
	//会议状态
	TConfStatusVer36 tConfStatus36;
	memset( &tConfStatus36, 0, sizeof(tConfStatus36) );
	tConfStatus36 = tConfInfo36.GetStatus();
	
	TConfStatusV4R4B2 tConfStatus;
	memset( &tConfStatus, 0, sizeof(tConfStatus) );
	ConfStatusVer36ToVer40V4R4B2( tConfStatus, tConfStatus36 );
	tConfInfo.SetStatus( tConfStatus );
	
	return;
}

inline void CStructConvert::ConfInfoVer40V4R4B2ToVer36( TConfInfoVer36 &tConfInfo36, TConfInfoV4R4B2 &tConfInfo )
{
	
	tConfInfo36.SetBitRate( tConfInfo.GetBitRate() );
	tConfInfo36.SetConfPwd( tConfInfo.GetConfPwd() );
	tConfInfo36.SetDuration( tConfInfo.GetDuration() );
	tConfInfo36.SetConfName( tConfInfo.GetConfName() );
	tConfInfo36.SetConfE164( tConfInfo.GetConfE164() );
	tConfInfo36.SetSecBitRate( tConfInfo.GetSecBitRate() );
	tConfInfo36.SetTalkHoldTime( tConfInfo.GetTalkHoldTime() );
	
	//会议号
	CConfId cConfId;
	memset( &cConfId, 0, sizeof(cConfId) );
	cConfId = tConfInfo.GetConfId();
	
	CConfIdVer36 cConfId36;
	memset( &cConfId36, 0, sizeof(cConfId36) );
	ConfIdVer40ToVer36( cConfId36, cConfId );
	tConfInfo36.SetConfId( cConfId36 );
	
	//时间结构
	TKdvTime tKdvTime;
	memset( &tKdvTime, 0, sizeof(tKdvTime) );
	tKdvTime = tConfInfo.GetKdvStartTime();
	
	TKdvTimeVer36 tKdvTime36;
	memset( &tKdvTime36, 0, sizeof(tKdvTime36) );
	KdvTimeVer40ToVer36( tKdvTime36, tKdvTime );
	tConfInfo36.SetKdvStartTime( tKdvTime36 );
	
	//能力集
	TCapSupport tCapSupport;
	tCapSupport = tConfInfo.GetCapSupport();
	
	TCapSupportVer36 tCapSupport36;
	CapSupportVer40ToVer36( tCapSupport36, tCapSupport );
	tConfInfo36.SetCapSupport( tCapSupport36 );
	
	//主席
	TMtAlias tChairAlias;
	memset( &tChairAlias, 0, sizeof(tChairAlias) );
	tChairAlias = tConfInfo.GetChairAlias();	
	
	TMtAliasVer36 tChairAlias36;
	memset( &tChairAlias36, 0, sizeof(tChairAlias36) );
	MtAliasVer40ToVer36( tChairAlias36, tChairAlias );
	tConfInfo36.SetChairAlias( tChairAlias36 );
	
	//发言人
	TMtAlias tSpeakerAlias;
	memset( &tSpeakerAlias, 0, sizeof(tSpeakerAlias) );
	tSpeakerAlias = tConfInfo.GetSpeakerAlias();
	
	TMtAliasVer36 tSpeakerAlias36;
	memset( &tSpeakerAlias36, 0, sizeof(tSpeakerAlias36) );
	MtAliasVer40ToVer36( tSpeakerAlias36, tSpeakerAlias );
	tConfInfo36.SetSpeakerAlias( tSpeakerAlias36 );
	
	//会议属性	
	TConfAttrb tConfAttrb;
	memset( &tConfAttrb, 0, sizeof(tConfAttrb) );
	tConfAttrb = tConfInfo.GetConfAttrb();
	
	TConfAttrbVer36 tConfAttrb36;
	memset( &tConfAttrb36, 0, sizeof(tConfAttrb36) );
	ConfAttribVer40ToVer36( tConfAttrb36, tConfAttrb );
	tConfInfo36.SetConfAttrb( tConfAttrb36 );
	
	//媒体密码
	TMediaEncrypt tMediaKey;
	memset( &tMediaKey, 0, sizeof(tMediaKey) );
	tMediaKey = tConfInfo.GetMediaKey();
	
	TMediaEncryptVer36 tMediaKey36;
	memset( &tMediaKey36, 0, sizeof(tMediaKey36) );
	MediaEncryptVer40ToVer36( tMediaKey36, tMediaKey );
	tConfInfo36.SetMediaKey( tMediaKey36 );
	
	//会议状态
	TConfStatusV4R4B2 tConfStatus;
	memset( &tConfStatus, 0, sizeof(tConfStatus) );
	tConfStatus = tConfInfo.GetStatus();
	
	TConfStatusVer36 tConfStatus36;
	memset( &tConfStatus36, 0, sizeof(tConfStatus36) );
	ConfStatusVer40V4R4B2ToVer36( tConfStatus36, tConfStatus );
	tConfInfo36.SetStatus( tConfStatus36 );
	
	return;
}

inline void CStructConvert::ConfInfoVer40V4R4B2ToVer40V4R5( TConfInfoV4R5 &tConfInfo, TConfInfoV4R4B2 &tConfInfoV4R4B2 )
{
    memcpy( &tConfInfo, &tConfInfoV4R4B2, min(sizeof(TConfInfoV4R5),sizeof(TConfInfoV4R4B2)) );
    
    //会议状态
    TConfStatusV4R4B2 tConfStatusV4R4B2;
    memset( &tConfStatusV4R4B2, 0, sizeof(tConfStatusV4R4B2) );
    tConfStatusV4R4B2 = tConfInfoV4R4B2.GetStatus();
	
    TConfStatusV4R5 tConfStatus;
    memset( &tConfStatus, 0, sizeof(tConfStatus) );  
    ConfStatusVer40V4R4B2ToVer40V4R5( tConfStatus, tConfStatusV4R4B2 );
	
    tConfInfo.SetStatus( tConfStatus );
    return;
}

inline void CStructConvert::ConfInfoVer40V4R5ToVer40V4R4B2( TConfInfoV4R4B2 &tConfInfoV4R4B2, TConfInfoV4R5 &tConfInfo )
{
    memcpy( &tConfInfoV4R4B2, &tConfInfo, min(sizeof(TConfInfoV4R4B2),sizeof(TConfInfoV4R5)) );
	
    //会议状态
    TConfStatusV4R5 tConfStatus;
    memset( &tConfStatus, 0, sizeof(tConfStatus) );
    tConfStatus = tConfInfo.GetStatus();
    
    TConfStatusV4R4B2 tConfStatusV4R4B2;
    memset( &tConfStatusV4R4B2, 0, sizeof(tConfStatusV4R4B2) );
    ConfStatusV4R5ToV4R4B2( tConfStatusV4R4B2, tConfStatus );
    tConfInfoV4R4B2.SetStatus( tConfStatusV4R4B2 );
    return;
}

inline void CStructConvert::ConfInfoV4R6ToV4R4( TConfInfoV4R4B2 &tConfInfoV4R4B2, TConfInfo &tConfInfo )
{
	
	TConfInfoV4R5	tConfInfoV4R5;
	ConfInfoV4R6ToV4R5(tConfInfoV4R5, tConfInfo);	                   //4.6->4.5
    ConfInfoVer40V4R5ToVer40V4R4B2(tConfInfoV4R4B2, tConfInfoV4R5);    //4.5->4.0
    return;
}

inline void CStructConvert::ConfInfoV4R4ToV4R6( TConfInfo &tConfInfo, TConfInfoV4R4B2 &tConfInfoV4R4B2)
{
	
	TConfInfoV4R5	tConfInfoV4R5;
	ConfInfoVer40V4R4B2ToVer40V4R5(tConfInfoV4R5, tConfInfoV4R4B2);	  //4.0->4.5
    ConfInfoV4R5ToV4R6(tConfInfo, tConfInfoV4R5);				      //4.5->4.6
    return;
}

inline void CStructConvert::ConfInfoV4R5ToV4R6( TConfInfo &tConfInfo, TConfInfoV4R5 &tConfInfoV4R5 )
{

    memcpy( &tConfInfo, &tConfInfoV4R5, min(sizeof(TConfInfo),sizeof(TConfInfoV4R5)) );
    
    //会议状态
    TConfStatusV4R5 tConfStatusV4R5;
    memset( &tConfStatusV4R5, 0, sizeof(tConfStatusV4R5) );
    tConfStatusV4R5 = tConfInfoV4R5.GetStatus();
	
    TConfStatus tConfStatus;
    memset( &tConfStatus, 0, sizeof(tConfStatus) );  
    ConfStatusV4R5ToV4R6( tConfStatus, tConfStatusV4R5 );
	
    tConfInfo.SetStatus( tConfStatus );
	
	
	TConfAttrbEx tConfAttrbEx = tConfInfoV4R5.GetConfAttrbEx();
	
	tConfInfo.SetConfAttrbEx(tConfAttrbEx);
	
    return;
}

inline void CStructConvert::ConfInfoV4R6ToV4R5( TConfInfoV4R5 &tConfInfoV4R5, TConfInfo &tConfInfo )
{
    memcpy( &tConfInfoV4R5, &tConfInfo, min(sizeof(TConfInfo),sizeof(TConfInfoV4R5)) );
    
    //会议状态
	TConfStatus tConfStatus;
    memset( &tConfStatus, 0, sizeof(tConfStatus) ); 
	tConfStatus = tConfInfo.GetStatus();
	
    TConfStatusV4R5 tConfStatusV4R5;
    //memset( &tConfStatusV4R5, 0, sizeof(tConfStatusV4R5) );
    ConfStatusV4R6ToV4R5( tConfStatusV4R5, tConfStatus );
	
    tConfInfoV4R5.SetStatus( tConfStatusV4R5 );

	//zjj20130530 终端有个bug,如果mcu呼叫终端那么终端会在setup信令中获得会议等级，但是
	//如果终端呼叫会议，终端必须在会议信息中获取会议等级，发现这个问题的时候正好是三代高清发布版本和pcmt发布版本的时候，
	//所以终端组改了这个问题，因为三代的Tconfinfo和我们一致，所以不用转换，但pcmt的TConfinfo比较老，是V4R5版本的，所以这里
	//要针对pcmt做特殊处理，看了一下终端组的pcmt流的tconfinfo结构中的TConfAttrbEx结构，他们定义的conflevel正好是tConfAttrbEx.m_byMainResEx
	//后面一位，但我们认为的V4R5版本的tConfAttrbEx.m_byMainResEx后面一位是tConfAttrbEx.m_bySchemeIdxInBatchPoll，但这里又不考虑其他v4r5的版本
	//的终端(比如一代高清，因为不发版本，这个bug就不改了，所以填不填对都无所谓)，所以这里转换的时候把conflevel填入到tConfAttrbEx.m_bySchemeIdxInBatchPoll，
	//保证pcmt能正确读到会议等级
	TConfAttrbEx tConfAttrbEx = tConfInfoV4R5.GetConfAttrbEx();
	tConfAttrbEx.SetSchemeIdxInBatchPoll( tConfInfo.GetConfLevel() );
	tConfInfoV4R5.SetConfAttrbEx(tConfAttrbEx);
    return;
}

inline void CStructConvert::ConfStatusVer36ToVer40V4R4B2( TConfStatusV4R4B2 &tConfStatus, TConfStatusVer36 &tConfStatusVer36 )
{
	//主席
	TMtVer36 tChairManVer36;
	memset( &tChairManVer36, 0, sizeof(tChairManVer36) );
	tChairManVer36 = tConfStatusVer36.GetChairman();
	
	TMt tChairMan;
	memset( &tChairMan, 0, sizeof(tChairMan) );
	MtVer36ToVer40( tChairMan, tChairManVer36 );
	tConfStatus.SetChairman( tChairMan );
	
	//发言人	
	TMtVer36 tSpeakerVer36;
	memset( &tSpeakerVer36, 0, sizeof(tSpeakerVer36) );
	tSpeakerVer36 = tConfStatusVer36.GetSpeaker();
	
	TMt tSpeaker;
	memset( &tSpeaker, 0, sizeof(tSpeaker) );
	MtVer36ToVer40( tSpeaker, tSpeakerVer36 );
	tConfStatus.SetSpeaker( tSpeaker );
	
	//录像进度
	TRecProgVer36 tRecProgVer36;
	memset( &tRecProgVer36, 0, sizeof(tRecProgVer36) );
	tRecProgVer36 = tConfStatusVer36.GetRecProg();
	
	TRecProg tRecProg;
	memset( &tRecProg, 0, sizeof(tRecProg) );
	RecProgVer36ToVer40( tRecProg, tRecProgVer36 );
	tConfStatus.SetRecProg( tRecProg );
	
	//放象进度	
	TRecProgVer36 tPlayProgVer36;
	memset( &tPlayProgVer36, 0, sizeof(tPlayProgVer36) );
	tPlayProgVer36 = tConfStatusVer36.GetPlayProg();
	
	TRecProg tPlayProg;
	memset( &tPlayProg, 0, sizeof(tPlayProg) );
	RecProgVer36ToVer40( tPlayProg, tPlayProgVer36 );
	tConfStatus.SetPlayProg( tPlayProg );
	
	//轮询信息
	TPollInfoVer36 tPollInfoVer36;
	memset( &tPollInfoVer36, 0, sizeof(tPollInfoVer36) );
	tPollInfoVer36 = *tConfStatusVer36.GetPollInfo();
	
	TPollInfo tPollInfo;
	memset( &tPollInfo, 0, sizeof(tPollInfo) );
	PollInfoVer36ToVer40( tPollInfo, tPollInfoVer36 );
	tConfStatus.SetPollInfo( tPollInfo );
	
	//混音信息
	TDiscussParamVer36 tDiscussParamVer36;
	memset( &tDiscussParamVer36, 0, sizeof(tDiscussParamVer36) );
	tDiscussParamVer36 = tConfStatusVer36.GetDiscussParam();
	
	TDiscussParamV4R4B2 tDiscussParam;
	memset( &tDiscussParam, 0, sizeof(tDiscussParam) );
	DiscussParamVer36ToVer40V4R4B2( tDiscussParam, tDiscussParamVer36 );
	tConfStatus.SetDiscussParam( tDiscussParam );
	
	//会议模式
	TConfModeVer36 tConfModeVer36;
	memset( &tConfModeVer36, 0, sizeof(tConfModeVer36) );
	tConfModeVer36 = tConfStatusVer36.GetConfMode();
	
	TConfModeV4R4B2 tConfMode;
	memset( &tConfMode, 0, sizeof(tConfMode) );
	ConfModeVer36ToVer40V4R4B2( tConfMode, tConfModeVer36 );
	tConfStatus.SetConfMode( tConfMode );
	
	//画面合成参数: 将3.6 ConfMode的VmpMode写入4.0的VmpParam中
	TVMPParamVer36 tVMPParamVer36;
	memset( &tVMPParamVer36, 0, sizeof(tVMPParamVer36) );
	tVMPParamVer36 = tConfStatusVer36.GetVmpParam();
	
	TVMPParamV4R4B2 tVMPParam;	// xliang [2/24/2009] 
	memset( &tVMPParam, 0, sizeof(tVMPParam) );
	VMPParamVer36ToVer40( tVMPParam, tVMPParamVer36 );
	tVMPParam.SetVMPMode( tConfModeVer36.GetVMPMode() );
	tConfStatus.SetVmpParam( tVMPParam );
	
	//是否智能丢包恢复
	tConfStatus.SetPrsing( tConfStatusVer36.IsPrsing() );
	
	//3.6没有电视墙轮询相关, 这里转4.0的时候填空值
	TVMPParamV4R4B2 tTwVmpParam;
	memset( &tTwVmpParam, 0, sizeof(tTwVmpParam) );
	tConfStatus.SetVmpTwParam( tTwVmpParam );
	
	TTvWallPollInfoV4R4B2 tTvWallPollInfo;
	memset( &tTvWallPollInfo, 0, sizeof(tTvWallPollInfo) );
	tConfStatus.SetTvWallPollInfo( tTvWallPollInfo );
	
	return;
}

inline void CStructConvert::ConfStatusVer40V4R4B2ToVer36( TConfStatusVer36 &tConfStatusVer36, TConfStatusV4R4B2 &tConfStatus )
{
	//主席
	TMt tChairMan;
	memset( &tChairMan, 0, sizeof(tChairMan) );
	tChairMan = tConfStatus.GetChairman();
	
	TMtVer36 tChairManVer36;
	memset( &tChairManVer36, 0, sizeof(tChairManVer36) );
	MtVer40ToVer36( tChairManVer36, tChairMan );
	tConfStatusVer36.SetChairman( tChairManVer36 );
	
	//发言人
	TMt tSpeaker;
	memset( &tSpeaker, 0, sizeof(tSpeaker) );
	tSpeaker = tConfStatus.GetSpeaker();
	
	TMtVer36 tSpeakerVer36;
	memset( &tSpeakerVer36, 0, sizeof(tSpeakerVer36) );
	MtVer40ToVer36( tSpeakerVer36, tSpeaker );
	tConfStatusVer36.SetSpeaker( tSpeakerVer36 );
	
	//录像进度
	TRecProg tRecProg;
	memset( &tRecProg, 0, sizeof(tRecProg) );
	tRecProg = tConfStatus.GetRecProg();
	
	TRecProgVer36 tRecProgVer36;
	memset( &tRecProgVer36, 0, sizeof(tRecProgVer36) );
	RecProgVer40ToVer36( tRecProgVer36, tRecProg );
	tConfStatusVer36.SetRecProg( tRecProgVer36 );
	
	//放象进度
	TRecProg tPlayProg;
	memset( &tPlayProg, 0, sizeof(tPlayProg) );
	tPlayProg = tConfStatus.GetPlayProg();
	
	TRecProgVer36 tPlayProgVer36;
	memset( &tPlayProgVer36, 0, sizeof(tPlayProgVer36) );
	RecProgVer40ToVer36( tPlayProgVer36, tPlayProg );
	tConfStatusVer36.SetPlayProg( tPlayProgVer36 );
	
	//轮询信息
	TPollInfo tPollInfo;
	memset( &tPollInfo, 0, sizeof(tPollInfo) );
	tPollInfo = *tConfStatus.GetPollInfo();
	
	TPollInfoVer36 tPollInfoVer36;
	memset( &tPollInfoVer36, 0, sizeof(tPollInfoVer36) );
	PollInfoVer40ToVer36( tPollInfoVer36, tPollInfo );
	tConfStatusVer36.SetPollInfo( tPollInfoVer36 );
	
	//混音信息
	TDiscussParamV4R4B2 tDiscussParam;
	memset( &tDiscussParam, 0, sizeof(tDiscussParam) );
	tDiscussParam = tConfStatus.GetDiscussParam();
    
	TDiscussParamVer36 tDiscussParamVer36;
	memset( &tDiscussParamVer36, 0, sizeof(tDiscussParamVer36) );
	DiscussParamVer40V4R4B2ToVer36( tDiscussParamVer36, tDiscussParam );
	tConfStatusVer36.SetDiscussParam( tDiscussParamVer36 );
	
	//画面合成参数
	TVMPParamV4R4B2 tVMPParam;
	memset( &tVMPParam, 0, sizeof(tVMPParam) );
	tVMPParam = tConfStatus.GetVmpParam();
	
	TVMPParamVer36 tVMPParamVer36;
	memset( &tVMPParamVer36, 0, sizeof(tVMPParamVer36) );
	VMPParamVer40ToVer36( tVMPParamVer36, tVMPParam );
	tConfStatusVer36.SetVmpParam( tVMPParamVer36 );
	
	//是否智能丢包恢复
	tConfStatusVer36.SetPrsing( tConfStatus.IsPrsing() );
	
	//会议模式: 将4.0 VmpParam的VmpMode写入3.6的ConfMode中
	TConfModeV4R4B2 tConfMode;
	memset( &tConfMode, 0, sizeof(tConfMode) );
	tConfMode = tConfStatus.GetConfMode();
	
	TConfModeVer36 tConfModeVer36;
	memset( &tConfModeVer36, 0, sizeof(tConfModeVer36) );
	ConfModeVer40V4R4B2ToVer36( tConfModeVer36, tConfMode );
	tConfModeVer36.SetVMPMode( tVMPParam.GetVMPMode() );
	tConfStatusVer36.SetConfMode( tConfModeVer36 );
	
	return;
}

inline void CStructConvert::ConfStatusVer40V4R4B2ToVer40V4R5( TConfStatusV4R5 &tConfStatus, TConfStatusV4R4B2 &tConfStatusV4R4B2 )
{
    memcpy( &tConfStatus, &tConfStatusV4R4B2, sizeof(tConfStatus));
	
    //重新整理TDiscussParam和TConfMode
    TDiscussParamV4R4B2 tDisParam;
    tDisParam = tConfStatusV4R4B2.GetDiscussParam();
	
    TMixParamV4R5 tMixParam;
    DiscussParamVer40V4R4B2ToVer40Daily(tMixParam, tDisParam);
    tConfStatus.SetMixerParam(tMixParam);
	
    TConfModeV4R4B2 tConfModeV4R4B2;
    tConfModeV4R4B2 = tConfStatusV4R4B2.GetConfMode();
	
    TConfMode tConfMode;
    ConfModeVer40V4R4B2ToVer40Daily(tConfMode, tConfModeV4R4B2);
    tConfStatus.SetConfMode(tConfMode);
	
    return;
}

inline void CStructConvert::ConfStatusV4R5ToV4R4B2( TConfStatusV4R4B2 &tConfStatusV4R4B2, TConfStatusV4R5 &tConfStatus )
{
    memcpy( &tConfStatusV4R4B2, &tConfStatus, min(sizeof(tConfStatus),sizeof(tConfStatusV4R4B2)) );
	
    //重新整理TDiscussParam和TConfMode
    TMixParamV4R5 tMixParam;
    tMixParam = tConfStatus.GetMixerParam();
    
    TDiscussParamV4R4B2 tDisParam;
    DiscussParamVer40DailyToVer40V4R4B2(tDisParam, tMixParam);
    tConfStatusV4R4B2.SetDiscussParam(tDisParam);
    
    TConfMode tConfMode;
    tConfMode = tConfStatus.GetConfMode();
    
    TConfModeV4R4B2 tConfModeV4R4B2;
    
    ConfModeVer40DailyToVer40V4R4B2(tConfModeV4R4B2, tConfMode);
    tConfModeV4R4B2.SetAudioMixMode(!tConfStatus.IsNoMixing() || tConfStatus.IsVACing());
    tConfModeV4R4B2.SetDiscussMode(!tConfStatus.IsNoMixing());
    tConfModeV4R4B2.SetMixSpecMt(tConfStatus.IsSpecMixing());
    tConfModeV4R4B2.SetVACMode(tConfStatus.IsVACing());
	
    tConfStatusV4R4B2.SetConfMode(tConfModeV4R4B2);
    
    return;
}

inline void CStructConvert::ConfModeVer36ToVer40V4R4B2( TConfModeV4R4B2 &tConfMode, TConfModeVer36 &tConfModeVer36 )
{
	tConfMode.SetTakeMode( tConfModeVer36.GetTakeMode() );
	tConfMode.SetLockMode( tConfModeVer36.GetLockMode() );
	tConfMode.SetCallMode( ConfCallModeVer36ToVer40( tConfModeVer36.GetCallMode() ) );
	tConfMode.SetVACMode( tConfModeVer36.IsVACMode() );
	tConfMode.SetAudioMixMode( tConfModeVer36.IsAudioMixMode() );
	tConfMode.SetRecordMode( tConfModeVer36.GetRecordMode() );
	tConfMode.SetPlayMode( tConfModeVer36.GetPlayMode() );
	tConfMode.SetBasMode( ConfBasModeVer36ToVer40( tConfModeVer36.GetBasMode(1) ), TRUE );
	tConfMode.SetPollMode( tConfModeVer36.GetPollMode() );
	tConfMode.SetDiscussMode( tConfModeVer36.IsDiscussMode() );
	tConfMode.SetForceRcvSpeaker( tConfModeVer36.IsForceRcvSpeaker() );
	tConfMode.SetNoChairMode( tConfModeVer36.IsNoChairMode() );
	tConfMode.SetRegToGK( tConfModeVer36.IsRegToGK() );
	tConfMode.SetMixSpecMt( tConfModeVer36.IsMixSpecMt() );
	tConfMode.SetCallInterval( tConfModeVer36.GetCallInterval() );
	tConfMode.SetCallTimes( tConfModeVer36.GetCallTimes() );
	
	return;
}

inline void CStructConvert::ConfModeVer40V4R4B2ToVer36( TConfModeVer36 &tConfModeVer36, TConfModeV4R4B2 &tConfMode )
{
	tConfModeVer36.SetTakeMode( tConfMode.GetTakeMode() );
	tConfModeVer36.SetLockMode( tConfMode.GetLockMode() );
	tConfModeVer36.SetCallMode( ConfCallModeVer40ToVer36( tConfMode.GetCallMode() ) );
	tConfModeVer36.SetVACMode( tConfMode.IsVACMode() );
	tConfModeVer36.SetAudioMixMode( tConfMode.IsAudioMixMode() );
	tConfModeVer36.SetRecordMode( tConfMode.GetRecordMode() );
	tConfModeVer36.SetPlayMode( tConfMode.GetPlayMode() );
	tConfModeVer36.SetBasMode( ConfBasModeVer40ToVer36( tConfMode.GetBasMode(1) ), TRUE );
	tConfModeVer36.SetPollMode( tConfMode.GetPollMode() );
	tConfModeVer36.SetDiscussMode( tConfMode.IsDiscussMode() );
	tConfModeVer36.SetForceRcvSpeaker( tConfMode.IsForceRcvSpeaker() );
	tConfModeVer36.SetNoChairMode( tConfMode.IsNoChairMode() );
	tConfModeVer36.SetRegToGK( tConfMode.IsRegToGK() );
	tConfModeVer36.SetMixSpecMt( tConfMode.IsMixSpecMt() );
	tConfModeVer36.SetCallInterval( tConfMode.GetCallInterval() );
	tConfModeVer36.SetCallTimes( tConfMode.GetCallTimes() );
	
	return;
}

inline void CStructConvert::ConfModeVer40V4R4B2ToVer40Daily( TConfMode &tConfMode, TConfModeV4R4B2 &tConfModeR4V4B2 )
{
    tConfMode.SetBasMode(tConfModeR4V4B2.GetBasModeValue(), TRUE);
    tConfMode.SetCallInterval(tConfModeR4V4B2.GetCallInterval());
    tConfMode.SetCallMode(tConfModeR4V4B2.GetCallMode());
    tConfMode.SetCallTimes(tConfModeR4V4B2.GetCallTimes());
    tConfMode.SetForceRcvSpeaker(tConfModeR4V4B2.IsForceRcvSpeaker());
    tConfMode.SetGkCharge(tConfModeR4V4B2.IsGkCharge());
    tConfMode.SetLockMode(tConfModeR4V4B2.GetLockMode());
    tConfMode.SetNoChairMode(tConfModeR4V4B2.IsNoChairMode());
    tConfMode.SetPlayMode(tConfModeR4V4B2.GetPlayMode());
    tConfMode.SetPollMode(tConfModeR4V4B2.GetPollMode());
    tConfMode.SetRecordMode(tConfModeR4V4B2.GetRecordMode());
    tConfMode.SetRecSkipFrame(tConfModeR4V4B2.IsRecSkipFrame());
    tConfMode.SetRegToGK(tConfModeR4V4B2.IsRegToGK());
    tConfMode.SetRollCallMode(ROLLCALL_MODE_NONE);
    tConfMode.SetTakeFromFile(tConfModeR4V4B2.IsTakeFromFile());
    tConfMode.SetTakeMode(tConfModeR4V4B2.GetTakeMode());
    
    return;
}

inline void CStructConvert::ConfModeVer40DailyToVer40V4R4B2( TConfModeV4R4B2 &tConfModeR4V4B2, TConfMode &tConfMode )
{
    //tConfModeR4V4B2.SetAudioMixMode()
    tConfModeR4V4B2.SetBasMode(tConfMode.GetBasModeValue(), TRUE);
    tConfModeR4V4B2.SetCallInterval(tConfMode.GetCallInterval());
    tConfModeR4V4B2.SetCallMode(tConfMode.GetCallMode());
    tConfModeR4V4B2.SetCallTimes(tConfMode.GetCallTimes());
    //tConfModeR4V4B2.SetDiscussMode()
    tConfModeR4V4B2.SetForceRcvSpeaker(tConfMode.IsForceRcvSpeaker());
    tConfModeR4V4B2.SetGkCharge(tConfMode.IsGkCharge());
    tConfModeR4V4B2.SetLockMode(tConfMode.GetLockMode());
    //tConfModeR4V4B2.SetMixSpecMt()
    tConfModeR4V4B2.SetNoChairMode(tConfMode.IsNoChairMode());
    tConfModeR4V4B2.SetPlayMode(tConfMode.GetPlayMode());
    tConfModeR4V4B2.SetPollMode(tConfMode.GetPollMode());
    tConfModeR4V4B2.SetRecordMode(tConfMode.GetRecordMode());
    tConfModeR4V4B2.SetRecSkipFrame(tConfMode.IsRecSkipFrame());
    tConfModeR4V4B2.SetRegToGK(tConfMode.IsRegToGK());
    tConfModeR4V4B2.SetTakeFromFile(tConfMode.IsTakeFromFile());
    tConfModeR4V4B2.SetTakeMode(tConfMode.GetTakeMode());
    //tConfModeR4V4B2.SetVACMode()
	
    return;
}

inline void CStructConvert::DiscussParamVer36ToVer40V4R4B2( TDiscussParamV4R4B2 &tDiscussParam, TDiscussParamVer36 &tDiscussParamVer36 )
{
	tDiscussParam.m_byMixMode   = tDiscussParamVer36.m_byMixMode;
	tDiscussParam.m_byMemberNum = tDiscussParamVer36.m_byMemberNum;
	
	for( s32 nIndex = 0; nIndex < MAXNUM_MIXER_DEPTH; nIndex ++ )
	{
		if ( !tDiscussParamVer36.m_atMtMember[nIndex].IsNull() )
		{
			TMt tMtMember;
			memset( &tMtMember, 0, sizeof(tMtMember) );
			MtVer36ToVer40( tMtMember, tDiscussParamVer36.m_atMtMember[nIndex] );
			tDiscussParam.m_atMtMember[nIndex] = tMtMember;
		}
	}
	return;
}

inline void CStructConvert::DiscussParamVer40V4R4B2ToVer36(TDiscussParamVer36 &tDiscussParamVer36, TDiscussParamV4R4B2 &tDiscussParam )
{
	tDiscussParamVer36.m_byMixMode   = tDiscussParam.m_byMixMode;
	tDiscussParamVer36.m_byMemberNum = tDiscussParam.m_byMemberNum;
	
	for( s32 nIndex = 0; nIndex < MAXNUM_MIXER_DEPTH_VER36; nIndex ++ )
	{
		if ( !tDiscussParam.m_atMtMember[nIndex].IsNull() )
		{
			TMtVer36 tMtMem36;
			memset( &tMtMem36, 0, sizeof(tMtMem36) );
			MtVer40ToVer36(tMtMem36, tDiscussParam.m_atMtMember[nIndex] );
			tDiscussParamVer36.m_atMtMember[nIndex] = tMtMem36;
		}
	}
	return;
}

inline void CStructConvert::DiscussParamVer40V4R4B2ToVer40Daily( TMixParamV4R5 &tMixParam, TDiscussParamV4R4B2 &tDiscussParam )
{
    tMixParam.SetMode(tDiscussParam.m_byMixMode);
    return;
}

inline void CStructConvert::DiscussParamVer40DailyToVer40V4R4B2( TDiscussParamV4R4B2 &tDiscussParam, TMixParamV4R5 &tMixParam )
{
    // FIXME：这里对V4R4B2以前的MCU的兼容性不好，级联的情况下会导致混音状态刷不出来
    tDiscussParam.m_byMixMode = tMixParam.GetMode();
	
	// [11/20/2010 xliang] add member conversion
	u8 byNum = min(MAXNUM_MIXER_DEPTH, tMixParam.GetMemberNum());
	tDiscussParam.m_byMemberNum = byNum;
	for( u8 byIdx = 0; byIdx < byNum; byIdx ++)
	{
		tDiscussParam.m_atMtMember[byIdx].SetMt(LOCAL_MCUID, tMixParam.GetMemberByIdx(byIdx));
	}
    return;
}

inline void CStructConvert::ConfStatusV4R5ToV4R6( TConfStatus &tConfStatus, TConfStatusV4R5 &tConfStatusV4R5 )
{
    memcpy(&tConfStatus, &tConfStatusV4R5, sizeof(tConfStatus));
	
	memcpy(&tConfStatus.m_tMixParam, &tConfStatusV4R5.m_tMixParam, sizeof(TMixParam));
	
	//调整TVMPParam
	TVMPParamV4R5 tVMPParamV4R5, tVMPTwParamV4R5;
	tVMPParamV4R5	= tConfStatusV4R5.GetVmpParam();
	tVMPTwParamV4R5 = tConfStatusV4R5.GetVmpTwParam();
	
	TVMPParam	tVMPParam, tVMPTwParam;
	memset( &tVMPParam,0,sizeof(tVMPParam) );
	memset( &tVMPTwParam,0,sizeof(tVMPTwParam) );
	VMPParamV4R5ToV4R6(tVMPParam, tVMPParamV4R5);
	VMPParamV4R5ToV4R6(tVMPTwParam, tVMPTwParamV4R5);
	
	tConfStatus.SetVmpParam(tVMPParam);
	tConfStatus.SetVmpTwParam(tVMPTwParam);
	
	//补上prs
	tConfStatus.SetPrsing(tConfStatusV4R5.IsPrsing());
	
    return;
}

inline void CStructConvert::ConfStatusV4R6ToV4R5( TConfStatusV4R5 &tConfStatusV4R5, TConfStatus &tConfStatus )
{
    memset(&tConfStatusV4R5, 0, sizeof(tConfStatusV4R5));
    memcpy( &tConfStatusV4R5,&tConfStatus, sizeof(tConfStatusV4R5));
	memcpy(&tConfStatusV4R5.m_tMixParam, &tConfStatus.m_tMixParam, sizeof(TMixParam));
    
    tConfStatusV4R5.SetConfMode( tConfStatus.GetConfMode() );

	//调整TVMPParam (16个最大成员)
	TVMPParam	tVMPParam, tVMPTwParam;
	tVMPParam	= tConfStatus.GetVmpParam();
	tVMPTwParam = tConfStatus.GetVmpTwParam();
	
	TVMPParamV4R5 tVMPParamV4R5, tVMPTwParamV4R5;
	memset( &tVMPParamV4R5,0,sizeof(tVMPParamV4R5) );
	memset( &tVMPTwParamV4R5,0,sizeof(tVMPTwParamV4R5) );
	VMPParamV4R6ToV4R5( tVMPParamV4R5, tVMPParam);
	VMPParamV4R6ToV4R5( tVMPTwParamV4R5, tVMPTwParam );
	
	tConfStatusV4R5.SetVmpParam(tVMPParamV4R5);
	tConfStatusV4R5.SetVmpTwParam(tVMPTwParamV4R5);
	
	//补上prs
	tConfStatusV4R5.SetPrsing(tConfStatus.IsPrsing());

    return;
}

inline void CStructConvert::VMPParamV4R5ToV4R6(TVMPParam &tVMPParam, TVMPParamV4R5 &tVMPParamV4R5)
{
	tVMPParam.SetVMPAuto( tVMPParamV4R5.IsVMPAuto() );
	tVMPParam.SetVMPBrdst( tVMPParamV4R5.IsVMPBrdst() );
	tVMPParam.SetVMPStyle( tVMPParamV4R5.GetVMPStyle() );
	tVMPParam.SetVMPSchemeId( tVMPParamV4R5.GetMaxMemberNum() );
	tVMPParam.SetVMPMode( tVMPParamV4R5.GetVMPMode() );
	tVMPParam.SetIsRimEnabled( tVMPParamV4R5.GetIsRimEnabled() );
	tVMPParam.SetVMPBatchPoll( 0 );
	for( s32 nIndex = 0; nIndex < MAXNUM_MPUSVMP_MEMBER; nIndex ++ )
	{
		TVMPMember *ptVMPMemberV4R5 = tVMPParamV4R5.GetVmpMember(nIndex);
		if ( NULL != ptVMPMemberV4R5 ) 
		{
			TVMPMember tVmpMember;
			memset( &tVmpMember, 0, sizeof(tVmpMember) );
			tVmpMember = *ptVMPMemberV4R5;
			tVMPParam.SetVmpMember( nIndex, tVmpMember );
		}
	}
	// 此段逻辑MAXNUM_VMP_MEMBER为20，MAXNUM_MPUSVMP_MEMBER也为20，进不了
	/*for( u8 byIdx = MAXNUM_VMP_MEMBER; byIdx < MAXNUM_MPUSVMP_MEMBER; byIdx ++)
	{
		TVMPMember tVmpMember;
		memset( &tVmpMember, 0, sizeof(tVmpMember) );
		tVMPParam.SetVmpMember( byIdx, tVmpMember );
	}*/
	return;
}

inline void CStructConvert::VMPParamV4R6ToV4R5(TVMPParamV4R5 &tVMPParamV4R5,TVMPParam &tVMPParam)
{
	tVMPParamV4R5.SetVMPAuto( tVMPParam.IsVMPAuto() );
	tVMPParamV4R5.SetVMPBrdst( tVMPParam.IsVMPBrdst() );
	tVMPParamV4R5.SetVMPStyle( tVMPParam.GetVMPStyle() );
	tVMPParamV4R5.SetVMPSchemeId( tVMPParam.GetMaxMemberNum() );
	tVMPParamV4R5.SetVMPMode( tVMPParam.GetVMPMode() );
	tVMPParamV4R5.SetIsRimEnabled( tVMPParam.GetIsRimEnabled() );
	
	for( s32 nIndex = 0; nIndex < MAXNUM_VMP_MEMBER; nIndex ++ )
	{
		TVMPMember *ptVMPMember = tVMPParam.GetVmpMember(nIndex);
		if ( NULL != ptVMPMember ) 
		{
			TVMPMember tVmpMemberV4R5;
			memset( &tVmpMemberV4R5, 0, sizeof(tVmpMemberV4R5) );
			tVmpMemberV4R5 = *ptVMPMember;
			tVMPParamV4R5.SetVmpMember( nIndex, tVmpMemberV4R5 );
		}
	}
}

inline u8 CStructConvert::ActiveMediaTypeVer36ToVer40( u8 byActiveMediaType36 )
{
	switch( byActiveMediaType36 )
	{
	case ACTIVE_TYPE_PCMA_VER36:		return ACTIVE_TYPE_PCMA;
	case ACTIVE_TYPE_PCMU_VER36:		return ACTIVE_TYPE_PCMU;
	case ACTIVE_TYPE_G721_VER36:		return ACTIVE_TYPE_G721;
	case ACTIVE_TYPE_G722_VER36:		return ACTIVE_TYPE_G722;
	case ACTIVE_TYPE_G7231_VER36:		return ACTIVE_TYPE_G7231;
	case ACTIVE_TYPE_G728_VER36:		return ACTIVE_TYPE_G728;
	case ACTIVE_TYPE_G729_VER36:		return ACTIVE_TYPE_G729;
	case ACTIVE_TYPE_G7221_VER36:		return ACTIVE_TYPE_G7221;			
	case ACTIVE_TYPE_H261_VER36:		return ACTIVE_TYPE_H261;		
	case ACTIVE_TYPE_H262_VER36:		return ACTIVE_TYPE_H262;
	case ACTIVE_TYPE_H263_VER36:		return ACTIVE_TYPE_H263;
	default:							return MEDIA_TYPE_NULL;
	}
}


inline u8 CStructConvert::ActiveMediaTypeVer40ToVer36( u8 byActiveMediaType40 )
{
	switch( byActiveMediaType40 )
	{
	case ACTIVE_TYPE_PCMA:			return ACTIVE_TYPE_PCMA_VER36;
	case ACTIVE_TYPE_PCMU:			return ACTIVE_TYPE_PCMU_VER36;
	case ACTIVE_TYPE_G721:			return ACTIVE_TYPE_G721_VER36;
	case ACTIVE_TYPE_G722:			return ACTIVE_TYPE_G722_VER36;
	case ACTIVE_TYPE_G7231:			return ACTIVE_TYPE_G7231_VER36;
	case ACTIVE_TYPE_G728:			return ACTIVE_TYPE_G728_VER36;
	case ACTIVE_TYPE_G729:			return ACTIVE_TYPE_G729_VER36;
	case ACTIVE_TYPE_G7221:			return ACTIVE_TYPE_G7221_VER36;			
	case ACTIVE_TYPE_H261:			return ACTIVE_TYPE_H261_VER36;		
	case ACTIVE_TYPE_H262:			return ACTIVE_TYPE_H262_VER36;
	case ACTIVE_TYPE_H263:			return ACTIVE_TYPE_H263_VER36;
	default:						return MEDIA_TYPE_NULL_VER36;
	}
}

inline u8 CStructConvert::EqpMainTypeVer36ToVer40( u8 byEqpMainType36 )
{
	switch( byEqpMainType36 )
	{
	case TYPE_MCU_VER36:			return TYPE_MCU;
	case TYPE_MCUPERI_VER36:		return TYPE_MCUPERI; 
	case TYPE_MT_VER36:				return TYPE_MT;
	default:						return NULL;
	}
}

inline u8 CStructConvert::EqpMainTypeVer40ToVer36( u8 byEqpMainType40 )
{
	switch( byEqpMainType40 )
	{
	case TYPE_MCU:					return TYPE_MCU_VER36;
	case TYPE_MCUPERI:				return TYPE_MCUPERI_VER36; 
	case TYPE_MT:					return TYPE_MT_VER36;
	default:						return NULL;
	}
}

inline u8 CStructConvert::MediaModeVer36ToVer40( u8 byMediaMode36 )
{
	switch( byMediaMode36)
	{
	case MODE_VIDEO_VER36:				return MODE_VIDEO;				
	case MODE_AUDIO_VER36:				return MODE_AUDIO;
	case MODE_BOTH_VER36:				return MODE_BOTH;
	case MODE_DATA_VER36:				return MODE_DATA;
	case MODE_SECVIDEO_VER36:			return MODE_SECVIDEO;
	case MODE_NONE_VER36:				return MODE_NONE;
	default:							return MODE_NONE;
	}
}

inline u8 CStructConvert::MediaModeVer40ToVer36( u8 byMediaMode40 )
{
	switch( byMediaMode40)
	{
	case MODE_VIDEO:				return MODE_VIDEO_VER36;				
	case MODE_AUDIO:				return MODE_AUDIO_VER36;
	case MODE_BOTH:					return MODE_BOTH_VER36;
	case MODE_DATA:					return MODE_DATA_VER36;
	case MODE_SECVIDEO:				return MODE_SECVIDEO_VER36;
	case MODE_NONE:					return MODE_NONE_VER36;
	default:						return MODE_NONE_VER36;
	}
}

inline u8 CStructConvert::VideoDStreamTypeVer36ToVer40( u8 byDStreamType36 )
{
	switch( byDStreamType36 )
	{
	case VIDEO_DSTREAM_H263PLUS_VER36:		return VIDEO_DSTREAM_H263PLUS;
	case VIDEO_DSTREAM_MAIN_VER36:			return VIDEO_DSTREAM_MAIN;
	case VIDEO_DSTREAM_H263PLUS_H239_VER36:	return VIDEO_DSTREAM_H263PLUS_H239;
	case VIDEO_DSTREAM_H263_H239_VER36:		return VIDEO_DSTREAM_H263_H239;
	case VIDEO_DSTREAM_H264_H239_VER36:		return VIDEO_DSTREAM_H264_H239;
	default:								return VIDEO_DSTREAM_MAIN;
	}
}

inline u8 CStructConvert::VideoDStreamTypeVer40ToVer36( u8 byDStreamType40 )
{
	switch( byDStreamType40 )
	{
	case VIDEO_DSTREAM_H263PLUS:		return VIDEO_DSTREAM_H263PLUS_VER36;
	case VIDEO_DSTREAM_MAIN:			return VIDEO_DSTREAM_MAIN_VER36;
	case VIDEO_DSTREAM_H263PLUS_H239:	return VIDEO_DSTREAM_H263PLUS_H239_VER36;
	case VIDEO_DSTREAM_H263_H239:		return VIDEO_DSTREAM_H263_H239_VER36;
	case VIDEO_DSTREAM_H264_H239:		return VIDEO_DSTREAM_H264_H239_VER36;
	default:							return VIDEO_DSTREAM_MAIN_VER36;
	}
}

inline u8 CStructConvert::ConfOpenModeVer36ToVer40( u8 byConfOpenMode36 )
{
	switch( byConfOpenMode36 )
	{
	case CONF_OPENMODE_CLOSED_VER36:	return CONF_OPENMODE_CLOSED;
	case CONF_OPENMODE_NEEDPWD_VER36:	return CONF_OPENMODE_NEEDPWD;
	case CONF_OPENMODE_OPEN_VER36:		return CONF_OPENMODE_OPEN;
	default:							return CONF_OPENMODE_CLOSED;
	}
}

inline u8 CStructConvert::ConfOpenModeVer40ToVer36( u8 byConfOpenMode40 )
{
	switch( byConfOpenMode40 )
	{
	case CONF_OPENMODE_CLOSED:		return CONF_OPENMODE_CLOSED_VER36;
	case CONF_OPENMODE_NEEDPWD:		return CONF_OPENMODE_NEEDPWD_VER36;
	case CONF_OPENMODE_OPEN:		return CONF_OPENMODE_OPEN_VER36;
	default:						return CONF_OPENMODE_CLOSED_VER36;
	}
}

inline u8 CStructConvert::ConfEncryptModeVer36ToVer40( u8 byConfEncryptMode36 )
{	
	switch( byConfEncryptMode36 )
	{
	case CONF_ENCRYPTMODE_AES_VER36:	return CONF_ENCRYPTMODE_AES;
	case CONF_ENCRYPTMODE_DES_VER36:	return CONF_ENCRYPTMODE_DES;
	case CONF_ENCRYPTMODE_NONE_VER36:	return CONF_ENCRYPTMODE_NONE;
	default:							return CONF_ENCRYPTMODE_NONE;
	}
}

inline u8 CStructConvert::ConfEncryptModeVer40ToVer36( u8 byConfEncryptMode40 )
{
	switch( byConfEncryptMode40 )
	{
	case CONF_ENCRYPTMODE_AES:		return CONF_ENCRYPTMODE_AES_VER36;
	case CONF_ENCRYPTMODE_DES:		return CONF_ENCRYPTMODE_DES_VER36;
	case CONF_ENCRYPTMODE_NONE:		return CONF_ENCRYPTMODE_NONE_VER36;
	default:						return CONF_ENCRYPTMODE_NONE_VER36;
	}
}

inline u8 CStructConvert::ConfUniformModeVer36ToVer40( u8 byConfUniformMode36 )
{
	switch( byConfUniformMode36 )
	{
	case CONF_UNIFORMMODE_NONE_VER36:	return CONF_UNIFORMMODE_NONE;
	case CONF_UNIFORMMODE_VALID_VER36:	return CONF_UNIFORMMODE_VALID;
	default:							return CONF_UNIFORMMODE_NONE;
	}
}

inline u8 CStructConvert::ConfUniformModeVer40ToVer36( u8 byConfUniformMode40 )
{
	switch( byConfUniformMode40 )
	{
	case CONF_UNIFORMMODE_NONE:		return CONF_UNIFORMMODE_NONE_VER36;
	case CONF_UNIFORMMODE_VALID:	return CONF_UNIFORMMODE_VALID_VER36;
	default:						return CONF_UNIFORMMODE_NONE_VER36;
	}
}

inline u8 CStructConvert::SwitchChnnlUniformModeVer36ToVer40( u8 bySwitchChnnlUniformMode36 )
{
	switch( bySwitchChnnlUniformMode36 )
	{
	case SWITCHCHANNEL_UNIFORMMODE_NONE_VER36:	return SWITCHCHANNEL_UNIFORMMODE_NONE;
	case SWITCHCHANNEL_UNIFORMMODE_VALID_VER36:	return SWITCHCHANNEL_UNIFORMMODE_VALID;
	default:									return SWITCHCHANNEL_UNIFORMMODE_NONE;
	}
}

inline u8 CStructConvert::SwitchChnnlUniformModeVer40ToVer36( u8 bySwitchChnnlUniformMode40 )
{
	switch( bySwitchChnnlUniformMode40 )
	{
	case SWITCHCHANNEL_UNIFORMMODE_NONE:	return SWITCHCHANNEL_UNIFORMMODE_NONE_VER36;
	case SWITCHCHANNEL_UNIFORMMODE_VALID:	return SWITCHCHANNEL_UNIFORMMODE_VALID_VER36;
	default:								return SWITCHCHANNEL_UNIFORMMODE_NONE_VER36;
	}
}

inline u8 CStructConvert::ConfDataModeVer36ToVer40( u8 byConfDataMode36 )
{
	switch( byConfDataMode36 )
	{
	case CONF_DATAMODE_VAONLY_VER36:	return CONF_DATAMODE_VAONLY;
	case CONF_DATAMODE_VAANDDATA_VER36:	return CONF_DATAMODE_VAANDDATA;
	case CONF_DATAMODE_DATAONLY_VER36:	return CONF_DATAMODE_DATAONLY;
	default:							return CONF_DATAMODE_VAONLY;
	}
}

inline u8 CStructConvert::ConfDataModeVer40ToVer36( u8 byConfDataMode40 )
{
	switch( byConfDataMode40 )
	{
	case CONF_DATAMODE_VAONLY:		return CONF_DATAMODE_VAONLY_VER36;
	case CONF_DATAMODE_VAANDDATA:	return CONF_DATAMODE_VAANDDATA_VER36;
	case CONF_DATAMODE_DATAONLY:	return CONF_DATAMODE_DATAONLY_VER36;
	default:						return CONF_DATAMODE_VAONLY_VER36;
	}
}

inline u8 CStructConvert::ConfReleaseModeVer36ToVer40( u8 byConfReleaseMode36 )
{
	switch( byConfReleaseMode36 )
	{
	case CONF_RELEASEMODE_NOMT_VER36:	return CONF_RELEASEMODE_NOMT;
	case CONF_RELEASEMODE_NONE_VER36:	return CONF_RELEASEMODE_NONE;
	default:							return CONF_RELEASEMODE_NONE;
	}
}

inline u8 CStructConvert::ConfReleaseModeVer40ToVer36( u8 byConfReleaseMode40 )
{
	switch( byConfReleaseMode40 )
	{
	case CONF_RELEASEMODE_NOMT:		return CONF_RELEASEMODE_NOMT_VER36;
	case CONF_RELEASEMODE_NONE:		return CONF_RELEASEMODE_NONE_VER36;
	default:						return CONF_RELEASEMODE_NONE_VER36;
	}
}

inline u8 CStructConvert::ConfTakeModeVer36ToVer40( u8 byConfTakeMode36 )
{	
	switch( byConfTakeMode36 )
	{
	case CONF_TAKEMODE_SCHEDULED_VER36:	return CONF_TAKEMODE_SCHEDULED;
	case CONF_TAKEMODE_ONGOING_VER36:	return CONF_TAKEMODE_ONGOING;
	case CONF_TAKEMODE_TEMPLATE_VER36:	return CONF_TAKEMODE_TEMPLATE;
	default:							return CONF_TAKEMODE_SCHEDULED;
	}
}

inline u8 CStructConvert::ConfTakeModeVer40ToVer36( u8 byConfTakeMode40 )
{
	switch( byConfTakeMode40 )
	{
	case CONF_TAKEMODE_SCHEDULED:	return CONF_TAKEMODE_SCHEDULED_VER36;
	case CONF_TAKEMODE_ONGOING:		return CONF_TAKEMODE_ONGOING_VER36;
	case CONF_TAKEMODE_TEMPLATE:	return CONF_TAKEMODE_TEMPLATE_VER36;
	default:						return CONF_TAKEMODE_SCHEDULED_VER36;
	}
}

inline u8 CStructConvert::ConfLockModeVer36ToVer40( u8 byConfLockMode36 )
{
	switch( byConfLockMode36 )
	{
	case CONF_LOCKMODE_NEEDPWD_VER36:		return CONF_LOCKMODE_NEEDPWD;
	case CONF_LOCKMODE_LOCK_VER36:			return CONF_LOCKMODE_LOCK;
	case CONF_LOCKMODE_NONE_VER36:			return CONF_LOCKMODE_NONE;
	default:								return CONF_LOCKMODE_NONE;
	}
}

inline u8 CStructConvert::ConfLockModeVer40ToVer36( u8 byConfLockMode40 )
{
	switch( byConfLockMode40 )
	{
	case CONF_LOCKMODE_NEEDPWD:			return CONF_LOCKMODE_NEEDPWD_VER36;
	case CONF_LOCKMODE_LOCK:			return CONF_LOCKMODE_LOCK_VER36;
	case CONF_LOCKMODE_NONE:			return CONF_LOCKMODE_NONE_VER36;
	default:							return CONF_LOCKMODE_NONE_VER36;
	}
}

inline u8 CStructConvert::ConfSpeakerSrcVer36ToVer40( u8 byConfSpeakerSrc36 )
{
	switch( byConfSpeakerSrc36 )
	{
	case CONF_SPEAKERSRC_SELF_VER36:	return CONF_SPEAKERSRC_SELF;
	case CONF_SPEAKERSRC_CHAIR_VER36:	return CONF_SPEAKERSRC_CHAIR;
	case CONF_SPEAKERSRC_LAST_VER36:	return CONF_SPEAKERSRC_LAST;
	default:							return CONF_SPEAKERSRC_SELF;
	}
}

inline u8 CStructConvert::ConfSpeakerSrcVer40ToVer36( u8 byConfSpeakerSrc40 )
{
	switch( byConfSpeakerSrc40 )
	{
	case CONF_SPEAKERSRC_SELF:		return CONF_SPEAKERSRC_SELF_VER36;
	case CONF_SPEAKERSRC_CHAIR:		return CONF_SPEAKERSRC_CHAIR_VER36;
	case CONF_SPEAKERSRC_LAST:		return CONF_SPEAKERSRC_LAST_VER36;
	default:						return CONF_SPEAKERSRC_SELF_VER36;
	}
}

inline u8 CStructConvert::ConfVmpModeVer36ToVer40( u8 byConfVmpMode36 )
{
	switch( byConfVmpMode36 )
	{
	case CONF_VMPMODE_NONE_VER36:		return CONF_VMPMODE_NONE;
	case CONF_VMPMODE_CTRL_VER36:		return CONF_VMPMODE_CTRL;
	case CONF_VMPMODE_AUTO_VER36:		return CONF_VMPMODE_AUTO;
	default:							return CONF_VMPMODE_NONE;
	}
}

inline u8 CStructConvert::ConfVmpModeVer40ToVer36( u8 byConfVmpMode40 )
{
	switch( byConfVmpMode40 )
	{
	case CONF_VMPMODE_NONE:			return CONF_VMPMODE_NONE_VER36;
	case CONF_VMPMODE_CTRL:			return CONF_VMPMODE_CTRL_VER36;
	case CONF_VMPMODE_AUTO:			return CONF_VMPMODE_AUTO_VER36;
	default:						return CONF_VMPMODE_NONE_VER36;
	}
}

inline u8 CStructConvert::ConfRecModeVer36ToVer40( u8 byConfRecMode36 )
{
	switch( byConfRecMode36 )
	{
	case CONF_RECMODE_REC_VER36:		return CONF_RECMODE_REC;
	case CONF_RECMODE_PAUSE_VER36:		return CONF_RECMODE_PAUSE;
	case CONF_RECMODE_NONE_VER36:		return CONF_RECMODE_NONE;
	default:							return CONF_RECMODE_NONE;
	}
}

inline u8 CStructConvert::ConfRecModeVer40ToVer36( u8 byConfRecMode40 )
{
	switch( byConfRecMode40 )
	{
	case CONF_RECMODE_REC:			return CONF_RECMODE_REC_VER36;
	case CONF_RECMODE_PAUSE:		return CONF_RECMODE_PAUSE_VER36;
	case CONF_RECMODE_NONE:			return CONF_RECMODE_NONE_VER36;
	default:						return CONF_RECMODE_NONE_VER36;
	}
}

inline u8 CStructConvert::ConfPlayModeVer36ToVer40( u8 byConfPlayMode36 )
{
	switch( byConfPlayMode36 )
	{
	case CONF_PLAYMODE_PLAY_VER36:		return CONF_PLAYMODE_PLAY;
	case CONF_PLAYMODE_PAUSE_VER36:		return CONF_PLAYMODE_PAUSE;
	case CONF_PLAYMODE_FF_VER36:		return CONF_PLAYMODE_FF;
	case CONF_PLAYMODE_FB_VER36:		return CONF_PLAYMODE_FB;
	case CONF_PLAYMODE_NONE_VER36:		return CONF_PLAYMODE_NONE;
	default:							return CONF_PLAYMODE_NONE;
	}
}

inline u8 CStructConvert::ConfPlayModeVer40ToVer36( u8 byConfPlayMode40 )
{
	switch( byConfPlayMode40 )
	{
	case CONF_PLAYMODE_PLAY:		return CONF_PLAYMODE_PLAY_VER36;
	case CONF_PLAYMODE_PAUSE:		return CONF_PLAYMODE_PAUSE_VER36;
	case CONF_PLAYMODE_FF:			return CONF_PLAYMODE_FF_VER36;
	case CONF_PLAYMODE_FB:			return CONF_PLAYMODE_FB_VER36;
	case CONF_PLAYMODE_NONE:		return CONF_PLAYMODE_NONE_VER36;
	default:						return CONF_PLAYMODE_NONE_VER36;
	}
}

inline u8 CStructConvert::ConfPollModeVer36ToVer40( u8 byConfPollMode36 )
{	
	switch( byConfPollMode36 )
	{
	case CONF_POLLMODE_VIDEO_VER36:		return CONF_POLLMODE_VIDEO;
	case CONF_POLLMODE_SPEAKER_VER36:	return CONF_POLLMODE_SPEAKER;
	case CONF_POLLMODE_NONE_VER36:		return CONF_POLLMODE_NONE;
	default:							return CONF_POLLMODE_NONE;
	}
}

inline u8 CStructConvert::ConfPollModeVer40ToVer36( u8 byConfPollMode40 )
{
	switch( byConfPollMode40 )
	{
	case CONF_POLLMODE_VIDEO:		return CONF_POLLMODE_VIDEO_VER36;
	case CONF_POLLMODE_SPEAKER:		return CONF_POLLMODE_SPEAKER_VER36;
	case CONF_POLLMODE_NONE:		return CONF_POLLMODE_NONE_VER36;
	default:						return CONF_POLLMODE_NONE_VER36;
	}
}

inline u8 CStructConvert::VmpMemberTypeVer36ToVer40( u8 byVmpMemberType36 )
{
	switch( byVmpMemberType36 )
	{
	case VMP_MEMBERTYPE_MCSSPEC_VER36:		return VMP_MEMBERTYPE_MCSSPEC;
	case VMP_MEMBERTYPE_SPEAKER_VER36:		return VMP_MEMBERTYPE_SPEAKER;
	case VMP_MEMBERTYPE_CHAIRMAN_VER36:		return VMP_MEMBERTYPE_CHAIRMAN;
	case VMP_MEMBERTYPE_POLL_VER36:			return VMP_MEMBERTYPE_POLL;
	case VMP_MEMBERTYPE_VAC_VER36:			return VMP_MEMBERTYPE_VAC;
	default:								return VMP_MEMBERTYPE_MCSSPEC;
	}
}

inline u8 CStructConvert::VmpMemberTypeVer40ToVer36( u8 byVmpMemberType40 )
{
	switch( byVmpMemberType40 )
	{
	case VMP_MEMBERTYPE_MCSSPEC:		return VMP_MEMBERTYPE_MCSSPEC_VER36;
	case VMP_MEMBERTYPE_SPEAKER:		return VMP_MEMBERTYPE_SPEAKER_VER36;
	case VMP_MEMBERTYPE_CHAIRMAN:		return VMP_MEMBERTYPE_CHAIRMAN_VER36;
	case VMP_MEMBERTYPE_POLL:			return VMP_MEMBERTYPE_POLL_VER36;
	case VMP_MEMBERTYPE_VAC:			return VMP_MEMBERTYPE_VAC_VER36;
	default:							return VMP_MEMBERTYPE_MCSSPEC_VER36;
	}
}

inline u8 CStructConvert::VmpStyleTypeVer36ToVer40( u8 byVmpStyleType36 )
{
	switch( byVmpStyleType36 )
	{
	case VMP_STYLE_DYNAMIC_VER36:		return VMP_STYLE_DYNAMIC;
	case VMP_STYLE_ONE_VER36:			return VMP_STYLE_ONE;
	case VMP_STYLE_VTWO_VER36:			return VMP_STYLE_VTWO;
	case VMP_STYLE_HTWO_VER36:			return VMP_STYLE_HTWO;
	case VMP_STYLE_THREE_VER36:			return VMP_STYLE_THREE;
	case VMP_STYLE_FOUR_VER36:			return VMP_STYLE_FOUR;
	case VMP_STYLE_SIX_VER36:			return VMP_STYLE_SIX;
	case VMP_STYLE_EIGHT_VER36:			return VMP_STYLE_EIGHT;
	case VMP_STYLE_NINE_VER36:			return VMP_STYLE_NINE;
	case VMP_STYLE_TEN_VER36:			return VMP_STYLE_TEN;
	case VMP_STYLE_THIRTEEN_VER36:		return VMP_STYLE_THIRTEEN;
	case VMP_STYLE_SIXTEEN_VER36:		return VMP_STYLE_SIXTEEN;
	case VMP_STYLE_SPECFOUR_VER36:		return VMP_STYLE_SPECFOUR;
	case VMP_STYLE_SEVEN_VER36:			return VMP_STYLE_SEVEN;
	default:							return VMP_STYLE_DYNAMIC;
	}
}

inline u8 CStructConvert::VmpStyleTypeVer40ToVer36( u8 byVmpStyleType40 )
{
	switch( byVmpStyleType40 )
	{
	case VMP_STYLE_DYNAMIC:			return VMP_STYLE_DYNAMIC_VER36;
	case VMP_STYLE_ONE:				return VMP_STYLE_ONE_VER36;
	case VMP_STYLE_VTWO:			return VMP_STYLE_VTWO_VER36;
	case VMP_STYLE_HTWO:			return VMP_STYLE_HTWO_VER36;
	case VMP_STYLE_THREE:			return VMP_STYLE_THREE_VER36;
	case VMP_STYLE_FOUR:			return VMP_STYLE_FOUR_VER36;
	case VMP_STYLE_SIX:				return VMP_STYLE_SIX_VER36;
	case VMP_STYLE_EIGHT:			return VMP_STYLE_EIGHT_VER36;
	case VMP_STYLE_NINE:			return VMP_STYLE_NINE_VER36;
	case VMP_STYLE_TEN:				return VMP_STYLE_TEN_VER36;
	case VMP_STYLE_THIRTEEN:		return VMP_STYLE_THIRTEEN_VER36;
	case VMP_STYLE_SIXTEEN:			return VMP_STYLE_SIXTEEN_VER36;
	case VMP_STYLE_SPECFOUR:		return VMP_STYLE_SPECFOUR_VER36;
	case VMP_STYLE_SEVEN:			return VMP_STYLE_SEVEN_VER36;
	default:						return VMP_STYLE_DYNAMIC_VER36;
	}
}

inline void CStructConvert::ConvertTConfInfoIn2Out( TConfInfo &tConfInfo, LPCSTR Version, void *ptConfInfo, u16 &wLength )
{
	if ( 0 == strcmp(Version, "36") )					//4.6 -> 3.6
	{
		TConfInfoVer36  tConfInfoVer36;
		TConfInfoV4R4B2 tConfInfoV4R4B2;
		ConfInfoV4R6ToV4R4( tConfInfoV4R4B2, tConfInfo );
		ConfInfoVer40V4R4B2ToVer36( tConfInfoVer36, tConfInfoV4R4B2 );

        *(TConfInfoVer36 *) ptConfInfo = tConfInfoVer36;
		wLength = sizeof( TConfInfoVer36 );
	}
	else if ( (0 == strcmp(Version, "4.0"))				//4.6 -> 4.0   
		   || (0 == strcmp(Version, "7")) 
		   || (0 == strcmp(Version, "5")) 
		   || (0 == strcmp(Version, "4.4")) )
	{
		TConfInfoV4R4B2 tConfInfoV4R4B2;
		ConfInfoV4R6ToV4R4( tConfInfoV4R4B2, tConfInfo );
        
		*(TConfInfoV4R4B2 *) ptConfInfo = tConfInfoV4R4B2;
		wLength = sizeof( TConfInfoV4R4B2 );
	}

	else if ( (0 == strcmp(Version, "4.5"))             //4.6 -> 4.5           
		   || (0 == strcmp(Version, "8")) )
	{
		TConfInfoV4R5 tConfInfoV4R5;
		ConfInfoV4R6ToV4R5( tConfInfoV4R5, tConfInfo );
		
		*(TConfInfoV4R5 *) ptConfInfo = tConfInfoV4R5;
		wLength = sizeof( TConfInfoV4R5 );
	}
	else                                    //4.6 -> 4.6
	{ 
		*(TConfInfo *) ptConfInfo = tConfInfo;
		wLength = sizeof( TConfInfo );
	}
}

inline void CStructConvert::ConvertTConfInfoOut2In( LPCSTR Version, void *ptConfInfo, u16 wLength, TConfInfo &tConfInfo )
{
	if ( (0 == strcmp(Version, "36")) && (wLength >= sizeof(TConfInfoVer36)) )             // 3.6 -> 4.6
	{
		TConfInfoVer36  tConfInfoVer36 = *(TConfInfoVer36 *)ptConfInfo;
		ConfInfoV36ToV4R6( tConfInfo, tConfInfoVer36 );
	} 
	else if ( ((0 == strcmp(Version, "4.0"))         // 4.0 -> 4.6
		    || (0 == strcmp(Version, "7")) 
		    || (0 == strcmp(Version, "5")) 
		    || (0 == strcmp(Version, "4.4"))) && (wLength >= sizeof(TConfInfoV4R4B2)) )
	{
		TConfInfoV4R4B2 tConfInfoV4R4B2 = *(TConfInfoV4R4B2 *)ptConfInfo;
		ConfInfoV4R4ToV4R6( tConfInfo, tConfInfoV4R4B2 );
	}
	else if ( ((0 == strcmp(Version, "4.5"))         // 4.5 -> 4.6           
		    || (0 == strcmp(Version, "8"))) && (wLength >= sizeof(TConfInfoV4R5)) )
	{
		TConfInfoV4R5 tConfInfoV4R5 = *(TConfInfoV4R5 *)ptConfInfo;
		ConfInfoV4R5ToV4R6( tConfInfo, tConfInfoV4R5 );
	}
	else                               // 4.6 -> 4.6
	{
		tConfInfo = *(TConfInfo *)ptConfInfo;
	}
}

#ifdef WIN32
#pragma pack( pop )
#endif

#endif  // _CONFINFOCONVERT_H_
