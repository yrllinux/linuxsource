/*****************************************************************************
   模块名      : mtadp
   文件名      : mtadputils.cpp
   相关文件    : mtadputils.h, evmcumt.h
   文件实现功能: 处理一些数据结构的转换
   作者        : Tan Guang
   版本        : V1.0  Copyright(C) 2003-2004 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期		版本        修改人      走读人		修改内容
   2003/11/11	1.0         Tan Guang				创建
   2005/12/30	4.0			张宝卿					兼容3.6终端
******************************************************************************/

#include "osp.h"
#include "cm.h"
#include "stkutils.h"
#include "mcustruct.h"
#include "mcuver.h"
#include "mtadp.h"
#include "evmcumt.h"
#include "evmcu.h"
//#include "h323adapter.h"
#include "mtadputils.h"

/*=============================================================================
  函 数 名： PackMtInfoListIntoBuf
  功    能： 将终端信息列表以紧凑方式填入缓冲区。如果是H323ID或E164
			 别名，则填入以TMt + type(u8) + length(u16) + StrBuf(u8*)
			 为单元的列表；如果是传输地址形式的别名，则填入以TMt+
			 type(u8)+TTransportAddr为单元的列表
  算法实现： 
  全局变量： 
  参    数： const TMt	atMt[],	源TMt数组
			 const TMtAlias atMtAlias[], 源TMtAlias数组
			 u8 byArraySize, 上面两个数组的元素个数
			 u8 *pbyBuf,	 目的缓冲区首地址
			 u32 dwBufLen)	 预分配的目的缓冲区长度
			 u8 *pbyBuf,	 填充的缓冲区
  返 回 值： (u32)实际填充的缓冲区字节数
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
u32 CMtAdpUtils::PackMtInfoListIntoBuf( const TMt      atMt[], 
									    const TMtAlias atMtAlias[], 
									    u8  byArraySize, 
									    u8* pbyBuf, 
									    u32 dwBufLen )	
{
	u8 *pbyTmp = pbyBuf;
	u8 *pbyMemLimit = pbyBuf + dwBufLen;

	for( s32 nArrySize = 0; nArrySize < byArraySize; nArrySize ++ )
	{
		if(!TRY_MEM_COPY(pbyTmp, (void*)&atMt[nArrySize], sizeof(TMt), pbyMemLimit))
        {
			return (pbyTmp - pbyBuf);
        }
		pbyTmp += sizeof(TMt);

		u8 byType = atMtAlias[nArrySize].m_AliasType;		
		if( !TRY_MEM_COPY(pbyTmp, &byType, sizeof(byType), pbyMemLimit) )
        {
			return (pbyTmp - pbyBuf);
        }
		pbyTmp += sizeof(byType);			

		if( (u8)mtAliasTypeE164 == byType || (u8)mtAliasTypeH323ID == byType || (u8)mtAliasTypeH320ID == byType || (u8)mtAliasTypeH320Alias == byType)
		{
			u16 wAliasLen = htons(strlen(atMtAlias[nArrySize].m_achAlias));
			if( !TRY_MEM_COPY( pbyTmp, &wAliasLen, sizeof(wAliasLen), pbyMemLimit) )
            {
				return ( pbyTmp - pbyBuf );
            }
			pbyTmp += sizeof(wAliasLen);

			if( !TRY_MEM_COPY(pbyTmp, (void*)atMtAlias[nArrySize].m_achAlias, wAliasLen, pbyMemLimit) )
            {
				return (pbyTmp - pbyBuf);
            }
			pbyTmp += wAliasLen;
		}
		else if( (u8)mtAliasTypeTransportAddress == byType )
		{
			if( !TRY_MEM_COPY( pbyTmp, (void*)&( atMtAlias[nArrySize].m_tTransportAddr ), sizeof(TTransportAddr), pbyMemLimit) )
            {
                return (pbyTmp - pbyBuf);
            }
			pbyTmp += sizeof(TTransportAddr);
		}
		else
		{
			StaticLog( "error mt alias detected in PackMtInfoListIntoBuf(): %d!\n", byType );
		}
	}
	
	return ( pbyTmp - pbyBuf );
}


/*=============================================================================
  函 数 名： UnpackMtInfoListFromBuf
  功    能： 从缓冲区中读出TMt和TMtAlias数组
  算法实现： 
  全局变量： 
  参    数： pbyBuf - 经PackMtInfoListIntoBuf函数填充的缓冲区
		  ： dwBufLen - pbyBuf缓冲区长度
		  ： atMt[] - 目的TMt数组
          ： atMtAlias[] - 目的TMtAlias数组
	      ： u8 byArraySize - 上面两个数组的元素个数
  返 回 值： (u8)实际填充的TMt或TMtAlias结构的个数
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
u8 CMtAdpUtils::UnpackMtInfoListFromBuf(const u8	*pbyBuf,	
										u32			dwBufLen,	
										TMt			atMt[],		
										TMtAlias	atMtAlias[],	
										u8			byArraySize)
{
	const u8 *pbyTmp = pbyBuf;	
	const u8 *pbyLimit = pbyBuf + dwBufLen;
	u8 byIdx = 0;
	while( pbyTmp <= pbyLimit )
	{
		if( pbyTmp + sizeof(TMt) > pbyLimit ) 
			break;

		atMt[byIdx] = *(TMt*)pbyTmp;

		if( atMt[byIdx].GetMtId() == 0 || atMt[byIdx].GetMtId() >= 193 )
		{
			StaticLog( "error mt id detected!\n");
		}		
		pbyTmp += sizeof(TMt);

		if( pbyTmp + sizeof(u8) > pbyLimit ) 
        {
            break;		
        }

		u8 byType = *(u8*)pbyTmp;

		atMtAlias[byIdx].m_AliasType = /*(mtAliasType)*/byType;
		pbyTmp += sizeof(u8);

		if( (u8)mtAliasTypeE164 == byType || (u8)mtAliasTypeH323ID == byType || (u8)mtAliasTypeH320ID == byType || (u8)mtAliasTypeH320Alias == byType )
		{

			if( pbyTmp + sizeof(u16) > pbyLimit ) 
            {
                break;
            }
			u16 aliasLen = ntohs(*(u16*)pbyTmp);
			pbyTmp += sizeof(u16);

			if(pbyTmp + aliasLen > pbyLimit) 
            {
				break;
            }
			memcpy( atMtAlias[byIdx].m_achAlias, pbyTmp, aliasLen );
			if( aliasLen < sizeof(atMtAlias[byIdx].m_achAlias) )
            {
				atMtAlias[byIdx].m_achAlias[aliasLen] = '\0';
            }
			pbyTmp += aliasLen;
		}
		else if( (u8)mtAliasTypeTransportAddress == byType )
		{
			if( pbyTmp + sizeof(TTransportAddr) > pbyLimit ) 
            {
				break;
            }
			atMtAlias[byIdx].m_tTransportAddr = *(TTransportAddr*)pbyTmp;
			pbyTmp += sizeof(TTransportAddr);
		}	
		else
		{
			StaticLog( "error mt alias detected in UnpackMtInfoListFromBuf(): %d!\n", byType);
		}

		byIdx++;
		if( byIdx >= byArraySize ) 
        {
			break;
        }
	}

	return byIdx;
}


/*=============================================================================
  函 数 名： TConfList2FacilityInfo
  功    能： TConfList数组转化成FacilityInfo结构
  算法实现： 
  全局变量： 
  参    数： const u8		*pbyBuf, 
			 s32 bufLen, 
			 TFACILITYINFO	&tFacilityInfo
  返 回 值： s8   
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
 2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
void CMtAdpUtils::TConfList2FacilityInfo(const u8		*pbyBuf, 
										 /*s32*/u16			 wBufLen, 
										 TFACILITYINFO	&tFacilityInfo )
{
	if( NULL == pbyBuf )
		return;

	u16 wConfs = wBufLen / sizeof(TConfNameInfo);
	tFacilityInfo.m_tConferences.m_byNum = (u8)wConfs;
	
    tFacilityInfo.m_byReason = (u8)confListChoice;

	for( u16 nConfNum = 0; nConfNum < wConfs; nConfNum++ )
	{	
		TConfNameInfo *pConf = (TConfNameInfo*)(pbyBuf + nConfNum * sizeof(TConfNameInfo));
		pConf->m_cConfId.GetConfId((u8*)tFacilityInfo.m_tConferences.m_atConfInfo[nConfNum].m_abyConfId, MAXLEN_CONFID);
		tFacilityInfo.m_tConferences.m_atConfInfo[nConfNum].m_tConfAlias.m_emType = cat_alias;
		tFacilityInfo.m_tConferences.m_atConfInfo[nConfNum].m_tConfAlias.m_tAlias.m_emType = type_h323ID;
		strncpy( tFacilityInfo.m_tConferences.m_atConfInfo[nConfNum].m_tConfAlias.m_tAlias.m_achAlias, 
			     pConf->achConfName, MAXLEN_CONFNAME - 1 );
		tFacilityInfo.m_tConferences.m_atConfInfo[nConfNum].m_tConfAlias.m_tAlias.m_achAlias[MAXLEN_CONFNAME - 1] = '\0';
	}

	return;
}


/*=============================================================================
  函 数 名： TFacilityInfo2TConfList
  功    能： FacilityInfo结构转化成TConfList数组
  算法实现： 
  全局变量： 
  参    数： const u8		*pbyBuf, 
			 s32 bufLen, 
			 TFACILITYINFO	&tFacilityInfo
  返 回 值： (s32)实际填充的字节数
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
 2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
// s32 CMtAdpUtils::TFacilityInfo2TConfList(const TFACILITYINFO tFacilityInfo, 
// 										 u8					*pbybuf, 
// 										 s32				 nBufLen)
// {
// //	if( NULL == pbybuf )
// //		return 0;
// 
// 	s32 nRealLen = 0;
// 	s32 nConfs = min( nBufLen / sizeof(TConfNameInfo), tFacilityInfo.m_tConferences.m_byNum );
// 	TConfNameInfo tConfList;
// 	memset( &tConfList, 0, sizeof(tConfList) );
// 	memcpy( &tConfList, pbybuf, sizeof(tConfList) );
// 	
// 	u8 *pbyTmp = pbybuf;
// 
// 	for( s32 nConfNum = 0; nConfNum < nConfs; nConfNum++ )
// 	{
// 		tConfList.m_cConfId.SetConfId( (u8*)tFacilityInfo.m_tConferences.m_atConfInfo[nConfNum].m_abyConfId, MAXLEN_CONFID );
// 
// 		strncpy( tConfList.achConfName, 
// 			     tFacilityInfo.m_tConferences.m_atConfInfo[nConfNum].m_tConfAlias.m_tAlias.m_achAlias,
// 			     sizeof(tConfList.achConfName) - 1 );	
// 
// 		tConfList.achConfName[sizeof(tConfList.achConfName) - 1] = '\0';
// 		
// 		pbyTmp += sizeof(TConfNameInfo);
// 		memset( &tConfList, 0, sizeof(tConfList) );
// 		memcpy( &tConfList, pbyTmp, sizeof(tConfList) );
// 		nRealLen += sizeof(TConfNameInfo);
// 	}
// 
// 	return nRealLen;
// }


/*=============================================================================
  函 数 名： PayloadTypeIn2Out
  功    能： 内部媒体类型转化为外部媒体类型 
  算法实现： 
  全局变量： 
  参    数： u8 payloadType
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang              创建
  2005/11/11    4.0			万春雷                增加相应媒体类型
=============================================================================*/
u8 CMtAdpUtils::PayloadTypeIn2Out( u8 byPayloadType )
{
	switch( byPayloadType ) 
	{
	//音频
	case MEDIA_TYPE_PCMA:			return (u8)a_g711a;		
	case MEDIA_TYPE_PCMU:			return (u8)a_g711u;		
	case MEDIA_TYPE_G722:			return (u8)a_g722;
	case MEDIA_TYPE_G7231:			return (u8)a_g7231;
	case MEDIA_TYPE_G728:			return (u8)a_g728;
	case MEDIA_TYPE_G729:			return (u8)a_g729;
	case MEDIA_TYPE_MP3:			return (u8)a_mp3;
	case MEDIA_TYPE_G7221C:			return (u8)a_g7221;
    case MEDIA_TYPE_AACLC:          return (u8)a_mpegaaclc;
    case MEDIA_TYPE_AACLD:          return (u8)a_mpegaacld;
	case MEDIA_TYPE_G719:			return (u8)a_g719;
	//视频
	case MEDIA_TYPE_H261:			return (u8)v_h261;
	case MEDIA_TYPE_H262:			return (u8)v_h262;
	case MEDIA_TYPE_H263:			return (u8)v_h263;
	case MEDIA_TYPE_H263PLUS:		return (u8)v_h263plus;
	case MEDIA_TYPE_H264:			return (u8)v_h264;
	case MEDIA_TYPE_MP4:			return (u8)v_mpeg4;
	//数据
	case MEDIA_TYPE_H224:			return (u8)d_h224;
	case MEDIA_TYPE_T120:			return (u8)d_t120;
	case MEDIA_TYPE_MMCU:			return (u8)t_mmcu;
	//类型空
	case MEDIA_TYPE_NULL:			return (u8)t_none;		
	default:						return (u8)t_none;
	}
}


/*=============================================================================
  函 数 名： PayloadTypeOut2In
  功    能： 外部媒体类型转化为内部媒体类型
  算法实现： 
  全局变量： 
  参    数： u8 payLoadType
             u8 &payload
             u8 &mediaType
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang              创建
  2005/11/11    4.0			万春雷                增加相应媒体类型
=============================================================================*/
void CMtAdpUtils::PayloadTypeOut2In( u8 byPayLoadType, u8 &byPayload, u8 &byMediaType )
{
	switch( byPayLoadType ) 
	{
	//音频格式
	case a_g711a:	 byPayload = MEDIA_TYPE_PCMA;    byMediaType = MODE_AUDIO; break;
	case a_g711u:	 byPayload = MEDIA_TYPE_PCMU;    byMediaType = MODE_AUDIO; break;
	case a_g722:	 byPayload = MEDIA_TYPE_G722;    byMediaType = MODE_AUDIO; break;
	case a_g7231:	 byPayload = MEDIA_TYPE_G7231;   byMediaType = MODE_AUDIO; break;
	case a_g728:	 byPayload = MEDIA_TYPE_G728;	 byMediaType = MODE_AUDIO; break;
	case a_g729:	 byPayload = MEDIA_TYPE_G729;    byMediaType = MODE_AUDIO; break;
	case a_mp3:	     byPayload = MEDIA_TYPE_MP3;	 byMediaType = MODE_AUDIO; break;
	case a_g7221:	 byPayload = MEDIA_TYPE_G7221C;	 byMediaType = MODE_AUDIO; break;
    case a_mpegaaclc: byPayload = MEDIA_TYPE_AACLC;	 byMediaType = MODE_AUDIO; break;
    case a_mpegaacld: byPayload = MEDIA_TYPE_AACLD;	 byMediaType = MODE_AUDIO; break;
	case a_g719:	byPayload = MEDIA_TYPE_G719;	 byMediaType = MODE_AUDIO; break;
	//视频格式
	case v_h261:	 byPayload = MEDIA_TYPE_H261;    byMediaType = MODE_VIDEO; break;
	case v_h262:	 byPayload = MEDIA_TYPE_H262;    byMediaType = MODE_VIDEO; break;
	case v_h263:	 byPayload = MEDIA_TYPE_H263;	 byMediaType = MODE_VIDEO; break;
	case v_h263plus: byPayload = MEDIA_TYPE_H263PLUS;byMediaType = MODE_VIDEO; break;
	case v_h264:	 byPayload = MEDIA_TYPE_H264;	 byMediaType = MODE_VIDEO; break;
	case v_mpeg4:	 byPayload = MEDIA_TYPE_MP4;     byMediaType = MODE_VIDEO; break;
	//数据格式
	case d_t120:	 byPayload = MEDIA_TYPE_T120;	 byMediaType = MODE_DATA;  break;
	case d_h224:	 byPayload = MEDIA_TYPE_H224;	 byMediaType = MODE_DATA;  break;
	case t_mmcu:     byPayload = MEDIA_TYPE_MMCU;    byMediaType = MODE_DATA;  break;

	default:		 byPayload = MEDIA_TYPE_NULL;    byMediaType = MODE_NONE;  break;
	}
	return;
}


/*=============================================================================
  函 数 名： PayloadTypeOut2In
  功    能： 外部媒体类型转化为内部媒体类型
  算法实现： 
  全局变量： 
  参    数： u8 payLoadType
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
u8 CMtAdpUtils::PayloadTypeOut2In( u8 byPayLoadType )
{
	u8 byOutPayloadType = byPayLoadType;
	u8 byInPayloadType  = MEDIA_TYPE_NULL;
	u8 byInMediaType    = MODE_NONE;

	PayloadTypeOut2In( byOutPayloadType, byInPayloadType, byInMediaType);
	
	return byInPayloadType;
}

/*=============================================================================
  函 数 名： CameraCommandOut2In
  功    能： 外部摄像机命令转换为内部命令
  算法实现： 
  全局变量： 
  参    数： TFeccStruct &tFeccStruct
             u16 &wEvent
             u8 &byParam
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
void CMtAdpUtils::CameraCommandOut2In( TFeccStruct &tFeccStruct, u16 &wEvent, u8 &byParam )
{
	static u16 wMessage = 0;
	static u8  byAction = 0;

	switch(tFeccStruct.m_byAction)
	{
	case actionStart:
	case actionStop:
		{
			if(tFeccStruct.m_byAction == (u8)actionStart)
			{
				wMessage = MT_MCU_MTCAMERA_CTRL_CMD; 
			}
			else
			{
				wMessage = MT_MCU_MTCAMERA_CTRL_STOP; 
			}
			switch(tFeccStruct.arguments.m_byRequest)
			{
			case requestTiltUp:		byAction = CAMERA_CTRL_UP;			break;
			case requestTiltDown:	byAction = CAMERA_CTRL_DOWN;		break;
			case requestPanLeft:	byAction = CAMERA_CTRL_LEFT;		break;
			case requestPanRight:	byAction = CAMERA_CTRL_RIGHT;		break;
			case requestFocusIn:	byAction = CAMERA_CTRL_FOCUSIN;		break;
			case requestFocusOut:	byAction = CAMERA_CTRL_FOCUSOUT;	break;
			case requestZoomIn:		byAction = CAMERA_CTRL_ZOOMIN;		break;
			case requestZoomOut:	byAction = CAMERA_CTRL_ZOOMOUT;		break;
			case requestBrightnessUp:	byAction = CAMERA_CTRL_BRIGHTUP;	break;
			case requestBrightnessDown: byAction = CAMERA_CTRL_BRIGHTDOWN;	break;
			default:	
				break;
			}
		}				
		break;

	case actionContinue:
		break;
		
	case actionVideoSourceSwitched:
        wMessage = MT_MCU_VIDEOSOURCESWITCHED_CMD;        
		byAction = tFeccStruct.arguments.m_bySourceNo;
        break;
		
	case actionSelectVideoSource:
		wMessage = MT_MCU_SELECTVIDEOSOURCE_CMD;
		byAction = tFeccStruct.arguments.m_bySourceNo;
		break;

	case actionStoreAsPreset:
		wMessage = MT_MCU_MTCAMERA_SAVETOPOS_CMD;
		byAction = tFeccStruct.arguments.m_byPresetNo;
		break;

	case actionActivatePreset:
		wMessage = MT_MCU_MTCAMERA_MOVETOPOS_CMD;
		byAction = tFeccStruct.arguments.m_byPresetNo;
		break;	

	case actionAutoFocus:
		wMessage = MT_MCU_MTCAMERA_CTRL_CMD; 
		byAction = CAMERA_CTRL_AUTOFOCUS;
		break;

	default:
		break;
	}
	wEvent  = wMessage;
	byParam = byAction;

	return;
}

 
/*=============================================================================
  函 数 名： CameraCommandIn2Out
  功    能： 内部摄像机命令转换为外部命令
  算法实现： 
  全局变量： 
  参    数： const CServMsg &cMsg
             TFeccStruct    &tFeccStruct
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
void CMtAdpUtils::CameraCommandIn2Out( const CServMsg &cMsg, TFeccStruct &tFeccStruct)
{
 	if( cMsg.GetMsgBodyLen() < sizeof(TMt) + 1 ) 
		return;
	
//	TMt tMt = *(TMt*)cMsg.GetMsgBody();
	u8  byFeccParam = *(u8*)( cMsg.GetMsgBody() + sizeof(TMt) );	

	switch( cMsg.GetEventId() ) 
	{
	case MCU_MT_MTCAMERA_CTRL_CMD:
	case MCU_MT_MTCAMERA_CTRL_STOP:
		{
			if( cMsg.GetEventId() == MCU_MT_MTCAMERA_CTRL_CMD )
			{
				tFeccStruct.m_byAction = (u8)actionStart;
				tFeccStruct.m_byTimeout = 0x08;
			}
			else
			{
				tFeccStruct.m_byAction = (u8)actionStop;
			}
			switch(byFeccParam) 
			{
			case CAMERA_CTRL_UP: 
				tFeccStruct.arguments.m_byRequest = (u8)requestTiltUp; 				
				break;
				
			case CAMERA_CTRL_DOWN: 
				tFeccStruct.arguments.m_byRequest = (u8)requestTiltDown; 
				break;

			case CAMERA_CTRL_LEFT:
				tFeccStruct.arguments.m_byRequest = (u8)requestPanLeft; 
				break;

			case CAMERA_CTRL_RIGHT:
				tFeccStruct.arguments.m_byRequest = (u8)requestPanRight; 
				break;

			case  CAMERA_CTRL_UPLEFT:
				tFeccStruct.arguments.m_byRequest = (u8)requestPanLeft | (u8)requestTiltUp; 
				break;

			case  CAMERA_CTRL_UPRIGHT:
				tFeccStruct.arguments.m_byRequest = (u8)requestPanRight | (u8)requestTiltUp; 
				break;

			case CAMERA_CTRL_DOWNLEFT:
				tFeccStruct.arguments.m_byRequest = (u8)requestPanLeft | (u8)requestTiltDown; 
				break;

			case CAMERA_CTRL_DOWNRIGHT:
				tFeccStruct.arguments.m_byRequest = (u8)requestPanRight | (u8)requestTiltDown; 
				break;

			case CAMERA_CTRL_ZOOMIN:
				tFeccStruct.arguments.m_byRequest = (u8)requestZoomIn; 
				break;

			case CAMERA_CTRL_ZOOMOUT:
				tFeccStruct.arguments.m_byRequest = (u8)requestZoomOut; 
				break;

			case CAMERA_CTRL_FOCUSIN:
				tFeccStruct.arguments.m_byRequest = (u8)requestFocusIn; 
				break;

			case CAMERA_CTRL_FOCUSOUT:
				tFeccStruct.arguments.m_byRequest = (u8)requestFocusOut; 
				break;

			case CAMERA_CTRL_BRIGHTUP:				
				tFeccStruct.arguments.m_byRequest = (u8)requestBrightnessUp;
				break;

			case CAMERA_CTRL_BRIGHTDOWN:				
				tFeccStruct.arguments.m_byRequest = (u8)requestBrightnessDown;
				break;

			case CAMERA_CTRL_AUTOFOCUS:
				tFeccStruct.m_byAction = (u8)actionAutoFocus;
				break;
				
			default:
				break;
			}
		}
		break;
	case MCU_MT_MTCAMERA_RCENABLE_CMD:		
		break;

	case MCU_MT_MTCAMERA_SAVETOPOS_CMD:
		tFeccStruct.m_byAction = (u8)actionStoreAsPreset;
		tFeccStruct.arguments.m_byPresetNo = byFeccParam;
		break;

	case MCU_MT_MTCAMERA_MOVETOPOS_CMD:
		tFeccStruct.m_byAction = (u8)actionActivatePreset;
		tFeccStruct.arguments.m_byPresetNo = byFeccParam;
		break;
		
	case MCU_MT_VIDEOSOURCESWITCHED_CMD:
		tFeccStruct.m_byAction = (u8)actionVideoSourceSwitched;
		tFeccStruct.arguments.m_bySourceNo = byFeccParam;
		break;

	case MCU_MT_SETMTVIDSRC_CMD:
		tFeccStruct.m_byAction = (u8)actionSelectVideoSource;
		tFeccStruct.arguments.m_bySourceNo = byFeccParam;
		break;

	default:
		tFeccStruct.m_byAction = (u8)actionInvalid;
		break;
	}	
	return;
}


/*=============================================================================
  函 数 名： TMtStatusOut2In
  功    能： 外部TTERSTATUS结构转换为内部TMtStatus结构
  算法实现： 
  全局变量： 
  参    数： const _TTERSTATUS &tTerStatus
  返 回 值： TMtStatus 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
TMtStatus CMtAdpUtils::TMtStatusOut2In(_TTERSTATUS &tTerStatus)
{
	TMtStatus tMtStatus;
	memset( &tMtStatus, 0, sizeof(tMtStatus) );
	tMtStatus.SetDecoderMute( tTerStatus.IsQuiet() );
    tMtStatus.SetCaptureMute( tTerStatus.IsMute() );
	tMtStatus.SetHasMatrix( tTerStatus.GetMatrixStatus() );
	//tMtStatus.SetSelPolling( tTerStatus.GetPollStat() != poll_status_none );
    tMtStatus.SetVideoLose( tTerStatus.IsVideoLose() );
	tMtStatus.SetDecoderVolume( tTerStatus.GetOutputVolume() );
	tMtStatus.SetCaptureVolume( tTerStatus.GetInputVolume() );
	tMtStatus.SetCurVideo( tTerStatus.GetVideoSrc() );
	tMtStatus.SetMtBoardType( tTerStatus.GetBoardType());
    tMtStatus.SetExVideoSrcNum(tTerStatus.GetExVidSrcNum());

	return tMtStatus;
}


/*=============================================================================
  函 数 名： TMtStatusIn2Out
  功    能： 
  算法实现： 
  全局变量： 
  参    数： const TMtStatus &tMtStatus
  返 回 值： _TTERSTATUS 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
_TTERSTATUS CMtAdpUtils::TMtStatusIn2Out( TMtStatus &tMtStatus )
{
	_TTERSTATUS tTerStatus;
	memset( &tTerStatus, 0, sizeof(tTerStatus) );
	tTerStatus.SetQuiet(tMtStatus.IsDecoderMute());
	tTerStatus.SetMute(tMtStatus.IsCaptureMute());
	tTerStatus.SetRemoteCtrlEnalbe(tMtStatus.IsEnableFECC());
	
	if(tMtStatus.IsHasMatrix())	
	{
		tTerStatus.SetMatrixStatus(1);	
	}
	tTerStatus.SetInputVolume(tMtStatus.GetCaptureVolume());
	tTerStatus.SetOutputVolume(tMtStatus.GetDecoderVolume());

    tTerStatus.SetVideoLose(tMtStatus.IsVideoLose());
    /*
	if(tMtStatus.IsSelPolling())
	{
        tTerStatus.SetPollStat(poll_status_normal);
	}
    else
	{
        tTerStatus.SetPollStat(poll_status_none);
	}
    */
	tTerStatus.SetBoardType(tMtStatus.GetMtBoardType());
    tTerStatus.SetExVidSrcNum(tMtStatus.GetExVideoSrcNum());
		
	return tTerStatus;
}

/*=============================================================================
  函 数 名：  TMtBitrateOut2In
  功    能： 
  算法实现： 
  全局变量： 
  参    数： _TTERBITRATE &tTerBitrate
  返 回 值： TMtBitrate  
=============================================================================*/
TMtBitrate   CMtAdpUtils::TMtBitrateOut2In( _TTERBITRATE &tTerBitrate )
{
    u16 wFstBitrate, wSecBitrate;
    TMtBitrate tMtBitrate;

    tTerBitrate.GetSendAverageBitRate(wFstBitrate, wSecBitrate);
    tMtBitrate.SetSendBitRate(wFstBitrate);
    tMtBitrate.SetH239SendBitRate(wSecBitrate);

    tTerBitrate.GetRecvAverageBitRate(wFstBitrate, wSecBitrate);
    tMtBitrate.SetRecvBitRate(wFstBitrate);
    tMtBitrate.SetH239RecvBitRate(wSecBitrate);

    return tMtBitrate;
}

/*=============================================================================
  函 数 名： CapabilitiesSetIn2Out
  功    能： 内部能力集结构转换为外部结构能力集结构 
  算法实现： 
  全局变量： 
  参    数： TCapSupport &tCapSupport,
			 TCapSet     *ptCapSet,
			 u8           byVendorId
             TCapSupportEx &tCapEx: 增量补充第二双流能力
			 u8 byAudioTrackNum	 音频声道数
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang              创建
  2004/12/30                xsl                   加密模式设置加密算法
  2005/11/11    4.0         wancl                 代码优化
  2005/12/21	4.0			张宝卿				  增加帧率设置
  2007/12/01    4.0R5       顾振华                H.264处理
  2008/04/15    4.0R5       顾振华                H.264处理
  2008/11/15    4.0R5       张宝卿                增量补充第二双流能力
  2012/03/16	4.7			陈代伟                增加声道数参数
=============================================================================*/
void CMtAdpUtils::CapabilitiesSetIn2Out(const TCapSupport &tCapSupport,
                                        const TCapSupportEx &tCapEx,
										TCapSet     *ptCapSet,
										u8           byVendorId,
										TAudioTypeDesc *ptAudioTypeDesc,
										u8 byAudioCapNum,
										const TVideoStreamCap &tDsStreamCapex
										)
{
	TVideoCap   tVideoCap;				//协议栈视频能力
    TH264VideoCap tH264Cap;             //协议栈H.264视频能力
//	TDStreamCap tDStreamCap;			//协议栈双流能力
    TDStreamCap tDStreamCap2;           //第二双流能力

	TSimCapSet  tSimuCap1, tSimuCap2;
	tSimuCap1   = tCapSupport.GetMainSimCapSet();
	tSimuCap2   = tCapSupport.GetSecondSimCapSet();
//	tDStreamCap = tCapSupport.GetDStreamCapSet();

	s32 nPos = 0;
	s32 nChoicePos = 0;
	u8  byPayload  = 0;
    //u8  byPayload2 = 0;
	u8  byCapIndex = 0;
    u8  byFstH264SubId = 0, bySecH264SubId = 0;
	
	//设置第一路音频描述条目以及能力集
	nPos = 0;
	nChoicePos = 0;
	//新增代码    从接口中取出会议的音频能力，填写到ptCapSet中，以便之后通知远端本地能力
	if (byAudioCapNum > 0)
	{
		for (u8 byAudioCapIdx = 0;byAudioCapIdx < byAudioCapNum;byAudioCapIdx++,ptAudioTypeDesc++)
		{
			if (ptAudioTypeDesc == NULL)
			{
				StaticLog( "[CapabilitiesSetIn2Out]ptAudioTypeDesc == NULL,So Break!\n");
				break;
			}
			byPayload  = PayloadTypeIn2Out(ptAudioTypeDesc->GetAudioMediaType());
			if( (u8)t_none != byPayload )
			{			
				if( (u8)a_g7231 == byPayload )
				{
					TG7231Cap tG7231Cap;
					tG7231Cap.SetCapDirection( cmCapReceiveAndTransmit );
					ptCapSet->SetG7231Cap( tG7231Cap );
				}
				else
				{	
					if( (u8)a_g7221 == byPayload && MT_MANU_NETMEETING == byVendorId )
					{  
						StaticLog( "netmeeting limit test_main!\n");				
					}
					else if ( (u8)a_mpegaaclc == byPayload || (u8)a_mpegaacld == byPayload )
					{
						TAACCap tAACCap;
						tAACCap.SetCapDirection(cmCapReceiveAndTransmit);
						tAACCap.SetChnl(GetAACChnnNum(ptAudioTypeDesc->GetAudioTrackNum()));
						tAACCap.SetSampleFreq(tagAACCap::emFs32000);
						ptCapSet->SetAACCap(tAACCap, byPayload);
					}
					else
					{
						TAudioCap tAudioCap;
						tAudioCap.SetCapDirection( cmCapReceiveAndTransmit );
						tAudioCap.SetPackTime(30);
						ptCapSet->SetAudioCap( tAudioCap, byPayload );				
					}
				}
				ptCapSet->SetCapDesc(byCapIndex, nPos, nChoicePos++, byPayload);
			}
		}
		nPos++;
	}
    //第一路视频(主辅视频同)
	nChoicePos = 0;
	byPayload  = PayloadTypeIn2Out(tSimuCap1.GetVideoMediaType());
    u8 byPayload2 = PayloadTypeIn2Out(tSimuCap2.GetVideoMediaType());
	if( (u8)t_none != byPayload )
	{		
        // guzh [04/15/2008]
        if ((u8)v_h264 == byPayload)
        {
            tH264Cap.Clear();
            tH264Cap.SetCapDirection( cmCapReceiveAndTransmit );
            tH264Cap.SetBitRate( tSimuCap1.GetVideoCap().GetMaxBitRate() );
            tH264Cap.SetResolution( ResIn2Out(tSimuCap1.GetVideoCap().GetResolution()),
                                    tSimuCap1.GetUserDefFrameRate() );

			//TBD 这里应该永远不会调用，主辅不应同时为H264 [3/30/2012 chendaiwei]
            if (  (u8)v_h264 == byPayload2 &&
                ( tSimuCap1.GetVideoResolution() != tSimuCap2.GetVideoResolution() ||
				tSimuCap1.GetUserDefFrameRate() != tSimuCap2.GetUserDefFrameRate() )  )
            {
                tH264Cap.SetResolution( ResIn2Out(tSimuCap2.GetVideoCap().GetResolution()),
					tSimuCap2.GetUserDefFrameRate() );
				
				//zjj0111219 辅格式不可能有HP能力
				tH264Cap.SetSupportProfile( (u8)TH264VideoCap::emProfileBaseline );				
            }

			if( tSimuCap1.GetVideoCap().IsSupportHP() )
			{
				tH264Cap.SetSupportProfile( (u8)TH264VideoCap::emProfileHigh);
				
				TH264VideoCap tH264BpCap;
				memcpy(&tH264BpCap,&tH264Cap,sizeof(TH264VideoCap));

				byFstH264SubId = ptCapSet->SetH264VideoCap(tH264Cap);
				ptCapSet->SetCapDesc(byCapIndex, nPos++, nChoicePos++, byPayload, byFstH264SubId);

				//BP HP分开填能力，兼容外厂商终端 [3/30/2012 chendaiwei]
				u8 byH264BpSubId = 0;
				tH264BpCap.SetSupportProfile((u8)TH264VideoCap::emProfileBaseline);
				byH264BpSubId = ptCapSet->SetH264VideoCap(tH264BpCap);
				ptCapSet->SetCapDesc(byCapIndex, nPos-1, nChoicePos++, byPayload, byH264BpSubId);
			}
			else
			{
				tH264Cap.SetSupportProfile( (u8)TH264VideoCap::emProfileBaseline);
				byFstH264SubId = ptCapSet->SetH264VideoCap(tH264Cap);
				ptCapSet->SetCapDesc(byCapIndex, nPos++, nChoicePos++, byPayload, byFstH264SubId);
			}
        }
        else
        {
            tVideoCap.Clear();
            tVideoCap.SetCapDirection( cmCapReceiveAndTransmit );
            tVideoCap.SetBitRate( tSimuCap1.GetVideoCap().GetMaxBitRate() );
            tVideoCap.SetResolution( (emResolution)ResIn2Out(tSimuCap1.GetVideoCap().GetResolution()), 
                                     FrameRateIn2Out(tSimuCap1.GetVideoFrameRate()) );

            if (  byPayload == byPayload2 &&
                  tSimuCap1.GetVideoResolution() != tSimuCap2.GetVideoResolution() ) 
            {
                tVideoCap.SetResolution( (emResolution)ResIn2Out(tSimuCap2.GetVideoCap().GetResolution()), 
                                         FrameRateIn2Out(tSimuCap2.GetVideoFrameRate()) );
            }

            ptCapSet->SetVideoCap( tVideoCap, byPayload );
            ptCapSet->SetCapDesc(byCapIndex, nPos++, nChoicePos++, byPayload);
        }        
	}

    // 第一路视频(辅格式)
	if( (u8)t_none != byPayload2 && 
		byPayload != byPayload2 )
	{
        // guzh [04/15/2008]
        if ((u8)v_h264 == byPayload2)// xliang [12/31/2008] 
        {
            tH264Cap.Clear();
            tH264Cap.SetCapDirection( cmCapReceiveAndTransmit );
            tH264Cap.SetBitRate( tSimuCap2.GetVideoCap().GetMaxBitRate() );
            tH264Cap.SetResolution( ResIn2Out(tSimuCap2.GetVideoCap().GetResolution()),
                                    tSimuCap2.GetUserDefFrameRate() );
			
			//zjj20111227 辅格式不可能有HP能力
			tH264Cap.SetSupportProfile( (u8)TH264VideoCap::emProfileBaseline );			

            byFstH264SubId = ptCapSet->SetH264VideoCap( tH264Cap );
            ptCapSet->SetCapDesc(byCapIndex, nPos-1, nChoicePos++, byPayload2, byFstH264SubId);
        }
        else
        {
            tVideoCap.Clear();
            tVideoCap.SetCapDirection( cmCapReceiveAndTransmit );
            tVideoCap.SetBitRate( tSimuCap2.GetVideoCap().GetMaxBitRate() );
            tVideoCap.SetResolution( (emResolution)ResIn2Out(tSimuCap2.GetVideoCap().GetResolution()), 
                                     FrameRateIn2Out(tSimuCap2.GetVideoFrameRate()) );

            ptCapSet->SetVideoCap( tVideoCap, byPayload2 );
            ptCapSet->SetCapDesc(byCapIndex, nPos-1, nChoicePos++, byPayload2);
        }

	}

	//对于RADVISION-MCU等厂商：通常需要在(第一路支持H263并且第二路支持H263+双流时), 同时也需在第一路同时支持H263+的可选项
	//（dont send h263+ cap to netmeeting）
	if( MT_MANU_NETMEETING != byVendorId && 
		tCapSupport.IsSupportMediaType(MEDIA_TYPE_H263) && 
		MEDIA_TYPE_H263PLUS == tCapSupport.GetDStreamMediaType() &&
		!tCapSupport.IsDStreamSupportH239() )
	{	
		ptCapSet->SetCapDesc(byCapIndex, nPos-1, nChoicePos++, (u16)v_h263plus);
	}
    
	//设置第二路视频描述条目以及能力集
    //双流支持, 目前支持三种类型：
    //与主视频格式一致、H263＋、H239(H263/H263+/H264)，缺省值支持与主视频格式一致
    BOOL32 IsDSEvMain = FALSE;
    if ( tCapSupport.GetDStreamType() == VIDEO_DSTREAM_MAIN || tCapSupport.GetDStreamType() == VIDEO_DSTREAM_MAIN_H239 )
    {
        IsDSEvMain = TRUE;
    }

	if( MT_MANU_NETMEETING != byVendorId )
	{
		u8 byPayloadDV = PayloadTypeIn2Out(tCapSupport.GetDStreamMediaType());
		
		if( (u8)t_none != byPayloadDV )
		{
            // guzh [2008/04/15] H.264 能力集设置到协议栈时，每次都用一个新的设置。而其他视频则用同一个    
            if ( (u8)v_h264 == byPayloadDV )
            {
                tH264Cap.Clear();
                tH264Cap.SetCapDirection( cmCapReceiveAndTransmit );
                tH264Cap.SetBitRate( tCapSupport.GetDStreamMaxBitRate() );
                tH264Cap.SetResolution( ResIn2Out(tCapSupport.GetDStreamResolution()), 
                                        tCapSupport.GetDStreamCapSet().GetUserDefFrameRate() ); 

				s32 nTempPos = 0;
				TH264VideoCap tH264BpCap;
				tH264BpCap.Clear();
				memcpy(&tH264BpCap,&tH264Cap,sizeof(TH264VideoCap));

				if( tCapSupport.GetDStreamCapSet().IsSupportHP())
				{
					tH264Cap.SetSupportProfile( (u8)TH264VideoCap::emProfileHigh);
					bySecH264SubId = ptCapSet->SetH264VideoCap( tH264Cap );

					//[5/13/2011 zhushengze]兼容以前同主视频不带239的情况，带239和不带239的双流都填入能力集              
					if( tCapSupport.IsDStreamSupportH239() )
					{
						ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, GetH239Type(byPayloadDV), bySecH264SubId);
						
						//同主视频增加不带239双流
						//取消对同主视频不带H239的兼容[4/13/2012 chendaiwei]
// 						if (IsDSEvMain)
// 						{
// 							ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, byPayloadDV, bySecH264SubId);
// 						}
					}
					else
					{
						ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, byPayloadDV, bySecH264SubId);
					}
				}

				//非同主视频,且支持HP能力，应填入BP路能力（目前H264同主视频只有8000H支持） [3/30/2012 chendaiwei]
				if(  !tCapSupport.GetDStreamCapSet().IsSupportHP()
				    || (tCapSupport.GetDStreamCapSet().IsSupportHP() && !IsDSEvMain) )
				{
					tH264BpCap.SetSupportProfile( (u8)TH264VideoCap::emProfileBaseline);
					bySecH264SubId = ptCapSet->SetH264VideoCap( tH264BpCap );
					
					//[5/13/2011 zhushengze]兼容以前同主视频不带239的情况，带239和不带239的双流都填入能力集              
					if( tCapSupport.IsDStreamSupportH239() )
					{
						ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, GetH239Type(byPayloadDV), bySecH264SubId);
						
						//同主视频增加不带239双流
						//取消对同主视频不带H239的兼容[4/13/2012 chendaiwei]
// 						if (IsDSEvMain)
// 						{
// 							ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, byPayloadDV, bySecH264SubId);
// 						}
					}
					else
					{
						ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, byPayloadDV, bySecH264SubId);
					}
				}

				//双流uxGA60勾选1080/30，单独组织1080能力通知对端[5/28/2013 chendaiwei]
				if( tDsStreamCapex.GetMediaType() == MEDIA_TYPE_H264 )
				{
					u8 bySecH264SubIdEx = 0;
					TH264VideoCap tH264DsCapEx;             //协议栈H.264视频能力
					tH264DsCapEx.Clear();
					tH264DsCapEx.SetCapDirection( cmCapReceiveAndTransmit );
					tH264DsCapEx.SetBitRate( tCapSupport.GetDStreamMaxBitRate() );
					tH264DsCapEx.SetResolution( ResIn2Out(tDsStreamCapex.GetResolution()), 
                                        tDsStreamCapex.GetUserDefFrameRate() ); 
					
					//非同主视频,且支持HP能力,填入HP能力（目前H264同主视频只有8000H支持） [3/30/2012 chendaiwei]
					if(tCapSupport.GetDStreamCapSet().IsSupportHP() && !IsDSEvMain)
					{
						tH264DsCapEx.SetSupportProfile( (u8)TH264VideoCap::emProfileHigh);
						bySecH264SubIdEx = ptCapSet->SetH264VideoCap( tH264DsCapEx );          
						if( tCapSupport.IsDStreamSupportH239() )
						{
							ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, GetH239Type(byPayloadDV), bySecH264SubIdEx);
						}
						else
						{
							ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, byPayloadDV, bySecH264SubIdEx);
						}
					}

					//始终填入BP能力[5/28/2013 chendaiwei]
					tH264DsCapEx.SetSupportProfile( (u8)TH264VideoCap::emProfileBaseline);
					bySecH264SubIdEx = ptCapSet->SetH264VideoCap( tH264DsCapEx );          
					if( tCapSupport.IsDStreamSupportH239() )
					{
						ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, GetH239Type(byPayloadDV), bySecH264SubIdEx);
					}
					else
					{
						ptCapSet->SetCapDesc(byCapIndex, nPos, nTempPos++, byPayloadDV, bySecH264SubIdEx);
					}
				}
				
				nPos++;
            }
            else 
            {
                if ( byPayload != byPayloadDV &&
                     byPayload2 != byPayloadDV )
                {
                    tVideoCap.Clear();
                    tVideoCap.SetCapDirection( cmCapReceiveAndTransmit );
                    tVideoCap.SetBitRate( tCapSupport.GetDStreamMaxBitRate() );
                    
                    tVideoCap.SetResolution( (emResolution)ResIn2Out(tCapSupport.GetDStreamResolution()), 
                                             FrameRateIn2Out(tCapSupport.GetDStreamFrameRate()) );
                    ptCapSet->SetVideoCap( tVideoCap, byPayloadDV );
                }

                //[5/13/2011 zhushengze]兼容以前同主视频不带239的情况，带239和不带239的双流都填入能力集
                if( tCapSupport.IsDStreamSupportH239() )
                {
                    ptCapSet->SetCapDesc(byCapIndex, nPos, 0, GetH239Type(byPayloadDV));

					//同主视频增加不带239双流
					//取消对同主视频不带H239的兼容[4/13/2012 chendaiwei]
//                     if (IsDSEvMain)
//                     {
//                         ptCapSet->SetCapDesc(byCapIndex, nPos, 1, byPayloadDV, bySecH264SubId);
//                     }
                    nPos++;
                    
                }
                else
                {
                    ptCapSet->SetCapDesc(byCapIndex, nPos++, 0, byPayloadDV);
			    }

                //sony 在支持 H263－H239 同时，还应支持 H263+－H239 可选
                if( MT_MANU_SONY == byVendorId && 
                    (u8)v_h263 == byPayloadDV && 
                    tCapSupport.IsDStreamSupportH239() )
                {
                    ptCapSet->SetCapDesc(byCapIndex, nPos-1, 1, GetH239Type( (u16)v_h263plus ));
                    
                    if( (u8)v_h263plus != byPayload && 
                        (u8)v_h263plus != byPayload2 )
                    {
                        tVideoCap.Clear();
                        tVideoCap.SetCapDirection( cmCapReceiveAndTransmit );
                        tVideoCap.SetBitRate( tCapSupport.GetDStreamMaxBitRate() );
                        
                        tVideoCap.SetResolution( (emResolution)ResIn2Out(tCapSupport.GetDStreamResolution()), 
                                                 FrameRateIn2Out(tCapSupport.GetDStreamFrameRate()) );
                        ptCapSet->SetVideoCap( tVideoCap, (u16)v_h263plus );
                    }
                    
                    //设置 role==live(2)，以适应sony动态双流
                    ptCapSet->SetSupportH239( 2 );
			    }
            }
		}
        
        //zbq[11/15/2008] 第二双流能力处理
        tDStreamCap2.SetSupportH239(tCapSupport.IsDStreamSupportH239());
        tDStreamCap2.SetMediaType(tCapEx.GetSecDSType());
        tDStreamCap2.SetMaxBitRate(tCapEx.GetSecDSBitRate());
        tDStreamCap2.SetResolution(tCapEx.GetSecDSRes());
        
        if (MEDIA_TYPE_H264 == tCapEx.GetSecDSType())
        {
            tDStreamCap2.SetUserDefFrameRate(tCapEx.GetSecDSFrmRate());
        }
        else
        {
            tDStreamCap2.SetFrameRate(tCapEx.GetSecDSFrmRate());
        }

        byPayloadDV = PayloadTypeIn2Out(tDStreamCap2.GetMediaType());
        
        if( (u8)t_none != byPayloadDV )
        {
            // guzh [2008/04/15] H.264 能力集设置到协议栈时，每次都用一个新的设置。而其他视频则用同一个    
            if ( (u8)v_h264 == byPayloadDV )
            {
                tH264Cap.Clear();
                tH264Cap.SetCapDirection( cmCapReceiveAndTransmit );
                tH264Cap.SetBitRate( tDStreamCap2.GetMaxBitRate() );
                tH264Cap.SetResolution( ResIn2Out(tDStreamCap2.GetResolution()), 
                    tDStreamCap2.GetUserDefFrameRate() );

				////zjj20111227 第二双流不可能有HP能力
				tH264Cap.SetSupportProfile( (u8)TH264VideoCap::emProfileBaseline );				

                bySecH264SubId = ptCapSet->SetH264VideoCap( tH264Cap );
                
                if( tDStreamCap2.IsSupportH239() )
                {
                    ptCapSet->SetCapDesc(byCapIndex, nPos++, 0, GetH239Type(byPayloadDV), bySecH264SubId);
                }
                else
                {
                    ptCapSet->SetCapDesc(byCapIndex, nPos++, 0, byPayloadDV, bySecH264SubId);
                }
            }
            else 
            {
                if ( byPayload != byPayloadDV &&
                    byPayload2 != byPayloadDV )
                {
                    tVideoCap.Clear();
                    tVideoCap.SetCapDirection( cmCapReceiveAndTransmit );
                    tVideoCap.SetBitRate( tDStreamCap2.GetMaxBitRate() );
                    
                    tVideoCap.SetResolution( (emResolution)ResIn2Out(tDStreamCap2.GetResolution()), 
                        FrameRateIn2Out(tDStreamCap2.GetFrameRate()) );
                    ptCapSet->SetVideoCap( tVideoCap, byPayloadDV );
                }
                
                if( tDStreamCap2.IsSupportH239()  )
                {
                    ptCapSet->SetCapDesc(byCapIndex, nPos++, 0, GetH239Type(byPayloadDV));
                }
                else
                {
                    ptCapSet->SetCapDesc(byCapIndex, nPos++, 0, byPayloadDV);
                }
            }
        }
	}
	
    if(tCapSupport.IsSupportH224())
    {
        ptCapSet->SetCapDesc(byCapIndex, nPos++, 0, (u16)d_h224);		
    }
    if(tCapSupport.IsSupportT120())
    {
        ptCapSet->SetCapDesc(byCapIndex, nPos++, 0, (u16)d_t120);		
    }
    if(tCapSupport.IsSupportMMcu())
    {
        ptCapSet->SetCapDesc(byCapIndex, nPos++, 0, (u16)t_mmcu);		
	}

	//设置音视频码流加密模式下的相应加密算法
	u8 byEncryptType = (u8)emEncryptTypeNone;
	u8 byEncrypMode  = tCapSupport.GetEncryptMode();
	byEncryptType |= EncryptTypeIn2Out( byEncrypMode );
	ptCapSet->SetEncryptBits( byEncryptType );

	return;
}


/*=============================================================================
  函 数 名： VideoCapCommon
  功    能： 提取共同视频能力，默认common就是local
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值：  
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2008/04/15    4.0R5       顾振华                H.264共同能力判断
=============================================================================*/
// void CMtAdpUtils::VideoCapCommon( const TSimCapSet &tLocalCap,  TCapSet *ptRemoteCapSet, 
//                                   TSimCapSet &tCommonCap, 
//                                   u8 byVendor, u8 byEncryptMode)
// {
//     u8 byPayload = PayloadTypeIn2Out(tLocalCap.GetVideoMediaType());
//     u16 wFStreamComType = GetComType(byPayload, byEncryptMode);
// 
//     u16 wBitrate = 0;       
//     if ( byPayload == v_h264 )
//     {
//         // Convert to tStackCap
//         TH264VideoCap tStackCap;
//         VideoCap264In2Out(tLocalCap.GetVideoCap(), tStackCap);
//         
//         if ( !ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1/*, &tStackCap*/) )
//         {
//             //FIXME: print ctrl
//             OspPrintf(TRUE, FALSE, "[Capset::Common] Remote H.264 IsExistsimultaneous failed, NOT Support H.264.\n");
//             tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
//             return;            
//         }        
// 
//         if ( !ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1, &tStackCap) )
//         {
// // [pengjie 2010/6/19] 按大小正确的取能力
// //          TH264VideoCap* ptRemotePriCap = ptRemoteCapSet->GetH264VideoCap(0);
// 
//             TVideoStreamCap tRemoteCap;
// 			MaxVideoCap264Out2In(ptRemoteCapSet, tLocalCap.GetVideoCap(), 
// 				wFStreamComType, tRemoteCap);
// //          VideoCap264Out2In(*ptRemotePriCap, tLocalCap.GetVideoCap(), tRemoteCap);
//             tCommonCap.SetVideoCap(tRemoteCap);
//         }
// 
// 
//         //guzh [02/21/2008] 额外的高清标清终端对通限制(仅对KedaMT处理, KedaMCU及其他厂商均不考虑)
//         //guzh 2008/08/07 BAS HD 放开限制
//         /*
//         if ( byVendor == MT_MANU_KDC )
//         {
//             // 参考 mtadp.cpp GetCapVideoParam()
//             // 3. CIF 的会议，认为高清的终端没有H.264 CIF能力
//             u8 byVcRes = tLocalCap.GetVideoResolution();
//             if ( byVcRes == VIDEO_FORMAT_AUTO )
//             {
//                 byVcRes = VIDEO_FORMAT_CIF;
//             }
//             if (byVcRes == VIDEO_FORMAT_CIF &&
//                 tStackCap.IsSupport(emHD720, 25))
//             {
//                 //FIXME: print ctrl
//                 OspPrintf(TRUE, FALSE, "[Capset::Common] CIF Conf but MT is HD-cap, NOT Support.\n");
//                 tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
//                 return;
//             }
//         }
//         */
//         
//         // guzh [2/27/2007] 对速率进行保存
//         if (tStackCap.GetBitRate() != 0)
//         {
//             if (tLocalCap.GetVideoMaxBitRate() != 0)
//             {
//                 wBitrate = min(tStackCap.GetBitRate(), tLocalCap.GetVideoMaxBitRate());
//             }
//             else
//             {
//                 wBitrate = tStackCap.GetBitRate();
//             }
//         }
//     }
//     else
//     {            
//         if( !ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1) )
//         {
//             tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
//             return;
//         }        
// 
//         TVideoCap* ptVideoCap = NULL;
//         ptVideoCap = ptRemoteCapSet->GetVideoCap(byPayload);           
//         if( NULL == ptVideoCap )
//         {
//             tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
//             return;
//         }
//         u8 byVcRes = tLocalCap.GetVideoCap().GetResolution();
//         u8 byMPI = ptVideoCap->GetResolution( (emResolution)ResIn2Out(byVcRes) );
//         if( 0 == byMPI )
//         {
//             tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
//             return;
//         }
//         
//         // guzh [2/27/2007] 对速率进行保存
//         if (ptVideoCap->GetBitRate() != 0)
//         {
//             if (tLocalCap.GetVideoMaxBitRate() != 0)
//             {
//                 wBitrate = min(ptVideoCap->GetBitRate(), tLocalCap.GetVideoMaxBitRate());
//             }
//             else
//             {
//                 wBitrate = ptVideoCap->GetBitRate();
//             }
//         }
//     }										
//     
//     if ( wBitrate != 0 )
//     {
//         tCommonCap.SetVideoMaxBitRate(wBitrate);
//     }
// 
//     return;
// }


/*=============================================================================
  函 数 名： VideoDCapCommon
  功    能： 判断是否存在共同双流能力(first video+dual video)
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： BOOL 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2008/04/15    4.0R5       顾振华                H.264共同能力判断
=============================================================================*/
// BOOL CMtAdpUtils::VideoDCapCommon(const TDStreamCap tDStreamCap,
//                                   BOOL bH339, u8 byEncryptMode,
//                                   const TVideoStreamCap tFstVideoCap,
//                                   TCapSet *ptRemoteCapSet)
// {
//     u16    awDStreamComType[2] = {0}; 
//     TH264VideoCap atH264Cap[2];        
//     u8 byArraySubId = 0;
//     BOOL bSupport  = FALSE;
//     
//     u8 byPayloadD = PayloadTypeIn2Out(tDStreamCap.GetMediaType());
//     awDStreamComType[1] = GetComType(byPayloadD, byEncryptMode);
//     if(bH339)
//     {
//         awDStreamComType[1] = GetH239Type(awDStreamComType[1]);
//     }
//     
//     u8 byPayload = PayloadTypeIn2Out(tFstVideoCap.GetMediaType());
//     if( t_none != byPayload )
//     {
//         awDStreamComType[0] = GetComType(byPayload, byEncryptMode);
//         
//         //zbq[12/27/2008] 区分同时能力依据 提取双流能力
// 
//         //[1]. h264的双流以h264作同时能力依据，走h264的共同能力模式
//         if (v_h264 == byPayloadD)
//         {
//             // 逐个添加 TH264VideoCap
//             if (v_h264 == byPayload)
//             {
//                 VideoCap264In2Out(tFstVideoCap, atH264Cap[byArraySubId]);
//                 byArraySubId++;
//             }
//             
//             if (v_h264 == byPayloadD)
//             {
//                 VideoCap264In2Out(tDStreamCap, atH264Cap[byArraySubId]);
//                 byArraySubId++;
//             }
//             
//             bSupport = ptRemoteCapSet->IsExistsimultaneous(awDStreamComType, 2, atH264Cap);
//         }
//         //[2].其他双流走普通的共同提取模式
//         else
//         {
//             bSupport  = ptRemoteCapSet->IsExistsimultaneous(awDStreamComType, 2);
//         }
//     }
//     else  // 如果第一路无共同能力，直接判断双流共同能力集, zgc, 2008-06-27
//     {
//         if (v_h264 == byPayloadD)
//         {
//             VideoCap264In2Out(tDStreamCap, atH264Cap[byArraySubId]);
//             byArraySubId++;
//         }
// 
//         u16 wDStreamComType = awDStreamComType[1];
//         bSupport = ptRemoteCapSet->IsExistsimultaneous(&wDStreamComType, 1, atH264Cap);
//     }
//     
//     return bSupport;
// }

/*=============================================================================
  函 数 名： CapSupportCommon
  功    能： 提取共同能力集结构
  算法实现： 
  全局变量： 
  参    数： TCapSupport &tLocalCap       [IN] 本地能力集
             TCapSet     *ptRemoteCapSet  [IN] 远端能力集
             TCapSupport &tCommonCap      [OUT]共同能力集 
			 const TVideoStreamCap *ptMSCapEx   [IN]主流扩展能力集勾选数组
			 u8 byMSCapExNum			  [IN]主流扩展能力集勾选数目
			 cosnt TVideoStreamCap *ptDSCapEx   [IN]双流扩展能力集勾选数组
			 u8 byDSCapEXNum			  [IN]双流扩展能力集勾选数目

  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2004/12/30                xsl                   创建(考虑加密模式下同时能力集判断)
  2005/11/11    4.0         wancl                 代码优化
  2005/12/27	4.0			张宝卿				  双流增加分辨率的判断
  2008/04/15    4.0R5       顾振华                H.264共同能力判断
  2008/11/15    4.0R5       张宝卿                按第二双流能力取共同能力尝试 支持
  2011/12/08    4.0R7       陈代伟                增加扩展能力集勾选支持
=============================================================================*/
void CMtAdpUtils::CapSupportCommon(const TCapSupport &tLocalCap, 
                                   const TCapSupportEx &tCapEx,
								   TCapSet     *ptRemoteCapSet, 
								   TMultiCapSupport &tCommonCap,
								   const TVideoStreamCap *ptMSCapEx,
								   u8 byMSCapExNum,
								   const TVideoStreamCap *ptDSCapEx,
								   u8 byDSCapEXNum,
								   TAudioTypeDesc *ptLocalAudioTypeDesc,
								   u8 byAudioCapNum
								   )
{
//	TSimCapSet tSimuCap1, tSimuCap2, tSimuCapOut;
	u16    wFStreamComType;       //第一路
	BOOL32 bSupport= FALSE;
	u8     byEncryptMode = EncryptTypeIn2Out(tLocalCap.GetEncryptMode());

	wFStreamComType = (u16)t_mmcu;
	bSupport = (tLocalCap.IsSupportMMcu() && ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType,1));
	tCommonCap.SetSupportMMcu(bSupport);

	wFStreamComType = GetComType((u8)d_t120, byEncryptMode);
	bSupport = (tLocalCap.IsSupportT120() && ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType,1));
	tCommonCap.SetSupportT120(bSupport);
	
	wFStreamComType = GetComType((u8)d_h224, byEncryptMode);
	bSupport = (tLocalCap.IsSupportH224() && ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType,1));
	tCommonCap.SetSupportH224(bSupport);
	
//	tSimuCap1 = tLocalCap.GetMainSimCapSet();
//	tSimuCap2 = tLocalCap.GetSecondSimCapSet();

    BOOL32 bMatchSuccess = FALSE;
    bMatchSuccess = MatchCommonCap(tLocalCap,
                                        tCapEx,
                                        ptRemoteCapSet,
                                        tCommonCap,
                                        /*byVendor,*/
                                        byEncryptMode,
										ptMSCapEx,byMSCapExNum,ptDSCapEx,byDSCapEXNum,
										ptLocalAudioTypeDesc,byAudioCapNum);
    if (!bMatchSuccess)
    {
        StaticLog( "[CapSupportCommon]match common cap failed!\n");
    }
	
// 	//获取第一路音视频能力集交集
//     tSimuCapOut = tSimuCap1;
//     VideoCapCommon(tSimuCap1, ptRemoteCapSet, tSimuCapOut, byVendor, byEncryptMode);
// 	
// 	// 主音频
// 	wFStreamComType = GetComType(PayloadTypeIn2Out(tSimuCap1.GetAudioMediaType()), byEncryptMode);
// 	if( !ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1) )
// 	{
// 		tSimuCapOut.SetAudioMediaType(MEDIA_TYPE_NULL);
// 	}
// 
// 	tCommonCap.SetMainSimCapSet(tSimuCapOut);
// 	    
// 	if( !tSimuCap2.IsNull() )
// 	{
//         tSimuCapOut = tSimuCap2;
//         VideoCapCommon(tSimuCap2, ptRemoteCapSet, tSimuCapOut, byVendor, byEncryptMode);
// 
//         // 辅音频
// 		wFStreamComType = GetComType(PayloadTypeIn2Out(tSimuCap2.GetAudioMediaType()), byEncryptMode);
// 		if( !ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1) )
// 		{
// 			tSimuCapOut.SetAudioMediaType(MEDIA_TYPE_NULL);
// 		}
// 
// 		tCommonCap.SetSecondSimCapSet(tSimuCapOut);
// 	}
// 
// 	//获取第二路视频能力集交集
// 
//     BOOL32 bSecDSCap = FALSE;
// 
// 	if( MEDIA_TYPE_NULL != tLocalCap.GetDStreamMediaType() )
// 	{
//         BOOL bH239 = tLocalCap.IsDStreamSupportH239();
// 
//         //[1].先取第一路主视频能力的交集作为 双流 同时能力的依据
//         bSupport = VideoDCapCommon(tLocalCap.GetDStreamCapSet(),
//                                    bH239, byEncryptMode,
//                                    tCommonCap.GetMainSimCapSet().GetVideoCap(),
//                                    ptRemoteCapSet);
// 
// 		
// 		//[2].不支持，则尝试取第一路辅视频能力的交集作为 双流 同时能力的依据
// 		if( !bSupport && MEDIA_TYPE_NULL != tCommonCap.GetSecondSimCapSet().GetVideoMediaType() )
// 		{
//             bSupport = VideoDCapCommon(tLocalCap.GetDStreamCapSet(),
//                                        bH239, byEncryptMode,
//                                        tCommonCap.GetSecondSimCapSet().GetVideoCap(),
//                                        ptRemoteCapSet);
// 		}
// 
//         //zbq[11/15/2008] 再不支持，尝试按 第二双流能力 取共同能力
//         if (!bSupport &&
//             MEDIA_TYPE_NULL != tCapEx.GetSecDSType())
//         {
//             TDStreamCap tDStreamCap2;
//             tDStreamCap2.SetSupportH239(bH239);
//             tDStreamCap2.SetMediaType(tCapEx.GetSecDSType());
//             tDStreamCap2.SetMaxBitRate(tCapEx.GetSecDSBitRate());
//             tDStreamCap2.SetResolution(tCapEx.GetSecDSRes());
//             
//             if (MEDIA_TYPE_H264 == tCapEx.GetSecDSType())
//             {
//                 tDStreamCap2.SetUserDefFrameRate(tCapEx.GetSecDSFrmRate());
//             }
//             else
//             {
//                 tDStreamCap2.SetFrameRate(tCapEx.GetSecDSFrmRate());
//             }
// 
//             //[3].仍然先取第一路主视频能力的交集作为 第二双流 同时能力的依据
//             bSupport = VideoDCapCommon(tDStreamCap2,
//                                        bH239, byEncryptMode,
//                                        tCommonCap.GetMainSimCapSet().GetVideoCap(),
//                                        ptRemoteCapSet);
//             
//             
//             //[4].还不支持，则尝试取第一路辅视频能力的交集作为 第二双流 同时能力的依据
//             if( !bSupport && MEDIA_TYPE_NULL != tCommonCap.GetSecondSimCapSet().GetVideoMediaType() )
//             {
//                 bSupport = VideoDCapCommon(tDStreamCap2,
//                                            bH239, byEncryptMode,
//                                            tCommonCap.GetSecondSimCapSet().GetVideoCap(),
//                                            ptRemoteCapSet);
// 		    }
// 
//             if (bSupport)
//             {
//                 bSecDSCap = TRUE;
//             }
//         }
// 
// 		if( bSupport )
// 		{
//             if (!bSecDSCap)
//             {
//                 tCommonCap.SetDStreamMediaType(tLocalCap.GetDStreamMediaType());
//                 tCommonCap.SetDStreamSupportH239(bH239);
//                 tCommonCap.SetDStreamMaxBitRate(tLocalCap.GetDStreamMaxBitRate());
//                 tCommonCap.SetDStreamResolution(tLocalCap.GetDStreamResolution());
//                 if (tLocalCap.GetDStreamMediaType() == MEDIA_TYPE_H264)
//                 {
//                     tCommonCap.SetDStreamUsrDefFPS(tLocalCap.GetDStreamUsrDefFPS());
//                 }
//                 else
//                 {
//                     tCommonCap.SetDStreamFrameRate(tLocalCap.GetDStreamFrameRate());
// 			    }	
//             }
//             else
//             {
//                 tCommonCap.SetDStreamMediaType(tCapEx.GetSecDSType());
//                 tCommonCap.SetDStreamSupportH239(bH239);
//                 tCommonCap.SetDStreamMaxBitRate(tCapEx.GetSecDSBitRate());
//                 tCommonCap.SetDStreamResolution(tCapEx.GetSecDSRes());
//                 tCommonCap.SetDStreamFrameRate(tCapEx.GetSecDSFrmRate());
//             }
// 		}
//         //[5].双流适配功能加入，对H264能力集的限制放开, zgc, 2008-08-25
//         else if ( ptRemoteCapSet->GetH264Num() > 0 &&
//                   tLocalCap.GetDStreamMediaType() == MEDIA_TYPE_H264 )
//         {
//             tCommonCap.SetDStreamMediaType(MEDIA_TYPE_H264);
//             tCommonCap.SetDStreamSupportH239(bH239);
//             tCommonCap.SetDStreamMaxBitRate(tLocalCap.GetDStreamMaxBitRate());
// 
// 			//zbq[09/02/2008] 帧率作尝试，分辨率取对端
// 			u8 byDSFPS = 0;
// 			u8 byDSRes = 0;
// /*			u8 byH264Num = ptRemoteCapSet->GetH264Num();
// 			for(u8 bySubId = 0; bySubId < byH264Num; bySubId ++)
// 			{
//                 byDSRes = 0;
// 
// 				TVideoStreamCap tCommonDSCap;
// 				
// 				TDStreamCap tLocalDSCap = tLocalCap.GetDStreamCapSet();
// 				TVideoStreamCap tLocalDSVidCap = (TVideoStreamCap)tLocalDSCap;
// 				TH264VideoCap *ptDSVidCap = ptRemoteCapSet->GetH264VideoCap(bySubId);
// 
//                 //是否同主视频
//                 BOOL32 bDSEqMV = FALSE;
// 
//                 if (tLocalCap.GetMainVideoType() == tLocalCap.GetDStreamMediaType() &&
//                     tLocalCap.GetMainVideoResolution() == tLocalCap.GetDStreamResolution())
//                 {
//                     bDSEqMV = TRUE;
//                 }
// 
//                 //Presentation/Live：取当前Local作尝试
//                 if (MEDIA_TYPE_H264 == tLocalDSCap.GetMediaType())
//                 {
//                     byDSFPS = tLocalDSCap.GetUserDefFrameRate();
//                 }
//                 else
//                 {
//                     byDSFPS = tLocalDSCap.GetFrameRate();
//                 }
// 
// 				VideoCap264Out2In(*ptDSVidCap,
// 							       tLocalDSVidCap,
// 								   tCommonDSCap, TRUE, byDSFPS, bDSEqMV);
//                 
//                 byDSRes = tCommonDSCap.GetResolution();
//                 
//                 if (byDSRes > tLocalCap.GetDStreamResolution())
//                 {
//                     //对端宏块积比common大，直接取宏块描述结果
//                 }
//                 else
//                 {
//                     //对端宏块积比较小，作如下处理：按5fps取分辨率
//                     byDSFPS = 5;
// 
//                     VideoCap264Out2In(*ptDSVidCap,
//                                        tLocalDSVidCap,
//                                        tCommonDSCap, TRUE, byDSFPS, bDSEqMV);
//                     byDSRes = tCommonDSCap.GetResolution();
//                 }
//                 //byDSRes = byDSRes > tCommonDSCap.GetResolution() ? byDSRes : tCommonDSCap.GetResolution();
// 
// 				//特殊处理，避开SVGA，尝试3帧的XGA
// 				if (VIDEO_FORMAT_SVGA == byDSRes)
// 				{
// 					if (ptDSVidCap->IsSupport(em1024768, 3))
// 					{
// 						byDSRes = ResOut2In(em1024768);
// 						byDSFPS = 3;
// 					}
// 				}
// 			}
// */
// 			//[2010.12.10 zhushz] 取得最大双流能力				
// 			BOOL32 bDSEqMV = FALSE;
// 
// 			//判断是否同主视频
//             if (tLocalCap.GetMainVideoType() == tLocalCap.GetDStreamMediaType() &&
//                 tLocalCap.GetMainVideoResolution() == tLocalCap.GetDStreamResolution())
//             {
//                 bDSEqMV = TRUE;
//             }
// 				
// 
// 			TDStreamCap tLocalDSCap = tLocalCap.GetDStreamCapSet();			
// 			TVideoStreamCap tMainStreamCap = tCommonCap.GetMainSimCapSet().GetVideoCap();
// 			TVideoStreamCap  tCommonDSCap;
// 			//取得最大双流能力
// 			MaxDSCap264Out2In(ptRemoteCapSet, tLocalDSCap, tMainStreamCap, byEncryptMode, tCommonDSCap, bDSEqMV);
// 			byDSFPS=tCommonDSCap.GetUserDefFrameRate();
// 			byDSRes=tCommonDSCap.GetResolution();
// 			//end
// 			tCommonCap.SetDStreamResolution(byDSRes);
// 			tCommonCap.SetDStreamUsrDefFPS(byDSFPS);
//         }
// 		else
// 		{
// 			//zjj 20090827 是否支持泰山德终端无双流能力时作弊加入双流能力
// 			if( MT_MANU_TAIDE == byVendor && g_cMtAdpApp.m_bSupportTaideDsCap )
// 			{
// 				if (!bSecDSCap)
// 				{
// 					tCommonCap.SetDStreamMediaType(tLocalCap.GetDStreamMediaType());
// 					tCommonCap.SetDStreamSupportH239(bH239);
// 					tCommonCap.SetDStreamMaxBitRate(tLocalCap.GetDStreamMaxBitRate());
// 					tCommonCap.SetDStreamResolution(tLocalCap.GetDStreamResolution());
// 					if (tLocalCap.GetDStreamMediaType() == MEDIA_TYPE_H264)
// 					{
// 						tCommonCap.SetDStreamUsrDefFPS(tLocalCap.GetDStreamUsrDefFPS());
// 					}
// 					else
// 					{
// 						tCommonCap.SetDStreamFrameRate(tLocalCap.GetDStreamFrameRate());
// 					}	
// 				}
// 				else
// 				{
// 					tCommonCap.SetDStreamMediaType(tCapEx.GetSecDSType());
// 					tCommonCap.SetDStreamSupportH239(bH239);
// 					tCommonCap.SetDStreamMaxBitRate(tCapEx.GetSecDSBitRate());
// 					tCommonCap.SetDStreamResolution(tCapEx.GetSecDSRes());
// 					tCommonCap.SetDStreamFrameRate(tCapEx.GetSecDSFrmRate());
// 				}
// 			}
// 		}
// 	}

	//获取码流加密模式
	tCommonCap.SetEncryptMode(tLocalCap.GetEncryptMode());
	
	return;
}

/*=============================================================================
函 数 名： CapabilityExSetIn2Out
功    能： 
算法实现： 
全局变量： 
参    数：  TCapSupportEx &tCapSupportEx
           TCapSet *ptCapSet
返 回 值： 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/9/27   4.0			周广程                  创建
=============================================================================*/
void CMtAdpUtils::CapabilityExSetIn2Out( TCapSupportEx &tCapSupportEx, TCapSet *ptCapSet )
{
	// FECType
	emFECType emType;
	CMtAdpUtils::FECTypeIn2Out( tCapSupportEx.GetVideoFECType(), emType );
	ptCapSet->SetFECType( (u8)emType );

	return;
}


/*=============================================================================
函 数 名：  FECTypeIn2Out
功    能： 
算法实现： 
全局变量： 
参    数：  u8 byFECType
			emFECType &FECType
返 回 值： static void 
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/9/30   4.0			周广程                  创建
=============================================================================*/
void  CMtAdpUtils::FECTypeIn2Out( u8 byFECType, emFECType &FECType )
{
	switch( byFECType )
	{
	case FECTYPE_NONE:
		FECType = emFECNone ;
		break;
	case FECTYPE_RAID5:
		FECType = emFECRaid5 ;
		break;
	case FECTYPE_RAID6:
		FECType = emFECRaid6 ;
		break;
	default:
		StaticLog( "[CapabilityExSetIn2Out] FECType.%d error!\n", byFECType );
		FECType = emFECNone ;
		break;
	}
}

/*=============================================================================
函 数 名： FECTypeOut2In
功    能： 
算法实现： 
全局变量： 
参    数：  emFECType FECType
           u8 &byFECType
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/9/30   4.0			周广程                  创建
=============================================================================*/
void  CMtAdpUtils::FECTypeOut2In( emFECType FECType, u8 &byFECType )
{
	switch( FECType )
	{
	case emFECNone:
		byFECType = FECTYPE_NONE;
		break;
	case emFECRaid5:
		byFECType = FECTYPE_RAID5;
		break;
	case emFECRaid6:
		byFECType = FECTYPE_RAID6;
		break;
	default:
		StaticLog( "[FECTypeOut2In] FECType.%d error!\n", FECType );
		byFECType = FECTYPE_NONE;
		break;
	}
}

/*=============================================================================
函 数 名： FECTypeCommon
功    能： 
算法实现： 
全局变量： 
参    数：  emFECType LocalFECType
           u8 byRemoteFECType
           emFECType &CommonFECType
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2007/10/8   4.0			周广程                  创建
=============================================================================*/
void  CMtAdpUtils::FECTypeCommon( emFECType LocalFECType, u8 byRemoteFECType, emFECType &CommonFECType )
{
	CommonFECType = (emFECType)( byRemoteFECType & (u8)LocalFECType );
	return;
}

/*=============================================================================
函 数 名： VideoCap264In2Out
功    能： H.264 协议栈能力结构构造
算法实现： 
全局变量： 
参    数：
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/04/15  4.0R5	    顾振华                  创建
=============================================================================*/
void CMtAdpUtils::VideoCap264In2Out(const TVideoStreamCap tInCap, TH264VideoCap &tOutCap)
{
    if ( tInCap.GetMediaType() != MEDIA_TYPE_H264 )
    {
        return;
    }

    u8 byVcRes = tInCap.GetResolution();
    if ( byVcRes == VIDEO_FORMAT_AUTO )
    {
        byVcRes = VIDEO_FORMAT_CIF;
    }
    s32 nStackRes = CMtAdpUtils::ResIn2Out(byVcRes);
    u8 byFps = tInCap.GetUserDefFrameRate();
    tOutCap.SetResolution(nStackRes, byFps);

    if (tInCap.GetMaxBitRate() > 0)
    {
        tOutCap.SetBitRate(tInCap.GetMaxBitRate());
    }

	// 设置支持HP [12/8/2011 chendaiwei]
	if(tInCap.IsSupportHP())
	{
		tOutCap.SetSupportProfile((u8)TH264VideoCap::emProfileHigh);
	}
	else
	{
		tOutCap.SetSupportProfile( (u8)TH264VideoCap::emProfileBaseline);	
	}
}

/*=============================================================================
函 数 名： MaxVideoCap264Out2In
功    能： H.264 协议栈能力结构构造(取出最大最合适的非239的264能力)
算法实现： 遍历对端所有264的能力，取最大的最合适的非239的264能力
全局变量： 
参    数： TCapSet *ptRemoteCapSet 对端能力
           TSimCapSet &tLocalCap 本地能力
		   u16 wFStreamComType 媒体载荷
		   TVideoStreamCap  &tMaxRemoteCap 共同能力
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20100519    4.6         彭杰                  create
20100902    4.6         彭杰                  修改（取出的为最大最合适的非239的264能力）
=============================================================================*/
void CMtAdpUtils::MaxVideoCap264Out2In(TCapSet *ptRemoteCapSet, 
									   const TVideoStreamCap &tLocalVideoCap,
									   u16 wFStreamComType,
									   TVideoStreamCap  &tMaxRemoteCap)
{
	u8 abyRes[] = { VIDEO_FORMAT_HD1080 , VIDEO_FORMAT_HD720 , VIDEO_FORMAT_4CIF , VIDEO_FORMAT_CIF };
	u8 byMaxResNum = sizeof(abyRes)/sizeof(u8);
	
	u8 byResIdx;
	TVideoStreamCap tLocalVideoStreamCap = tLocalVideoCap;
	for ( byResIdx = 0; byResIdx < byMaxResNum; byResIdx++ )
	{
		// 找到本地能力分辨率所处的位置
		if( tLocalVideoStreamCap.GetResolution() == abyRes[byResIdx] )
		{
			break;
		}
	}
	
	// 从当前分辨率逐步往下比，取到最大支持的分辨率
	for ( u8 byCurResIdx = byResIdx; byCurResIdx < byMaxResNum; byCurResIdx++ )
	{
		if( abyRes[byCurResIdx] <= VIDEO_FORMAT_4CIF && tLocalVideoStreamCap.GetUserDefFrameRate() > 30 )
		{
			tLocalVideoStreamCap.SetUserDefFrameRate(30);
		}
		tLocalVideoStreamCap.SetResolution( abyRes[byCurResIdx] );
		TH264VideoCap tStackCap;
		tStackCap.Clear();
		VideoCap264In2Out(tLocalVideoStreamCap, tStackCap);
        
		if ( ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1, &tStackCap) )
		{
			// 取到最大共同能力
			tMaxRemoteCap = tLocalVideoStreamCap;
			break;
		}
		else // 没有取到
		{
			// [pengjie 2010/9/8] 由于目前还没有帧率适配，所以下面降帧率比较部分先不开放
			// 			tStackCap.SetMaxMBPS( tStackCap.GetMaxFS() );
			// 			if ( ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1, &tStackCap ) )
			// 			{
			// 				u8 byFps = tStackCap.GetResolution( CMtAdpUtils::ResIn2Out(tLocalVideoStreamCap.GetResolution()) );
			// 				tLocalVideoStreamCap.SetUserDefFrameRate(byFps);
			// 				tMaxRemoteCap = tLocalVideoStreamCap;
			// 				break;
            // 			}
			
			// 对于cif，帧率模糊处理，如果当前能力不匹配，且帧率又在大于25小于等于30之间，则将本地帧率降为25再匹配一次
			if( VIDEO_FORMAT_CIF == abyRes[byCurResIdx] )
			{
				u8 byLocalFps = tLocalVideoStreamCap.GetUserDefFrameRate();
				if( byLocalFps > 25 && byLocalFps <= 30 )
				{
					tLocalVideoStreamCap.SetUserDefFrameRate( 25 );
					tStackCap.Clear();
					VideoCap264In2Out(tLocalVideoStreamCap, tStackCap);
					
					if ( ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1, &tStackCap ) )
					{
						tMaxRemoteCap = tLocalVideoStreamCap;
						break;
					}
				}
			}
		}
	}
	
	return;
}
/*=============================================================================
函 数 名： MatchCommonCap
功    能： 匹配共同能力集
算法实现：1.第一路主格式
          2.第一路辅格式
          3.双流
            3.1主格式+双流(主格式适配)
            3.2主格式+第二双流(主格式适配)
            3.3辅格式+双流
            3.4辅格式+第二双流
全局变量： 
参    数： const TCapSupport &tLocalCap [IN]本地能力集
           const TCapSupportEx &tCapEx  [IN]本地第二双流
           TCapSet *ptRemoteCapSet      [IN]对端能力集
           TCapSupport &tCommonCap      [OUT]共同能力集    
           u8 byEncryptMode             [IN]加密模式
           u8 byAdpFps                  [IN]适配帧率
		   const TVideoStreamCap *ptMSCapEx   [IN]主流扩展能力集勾选数组
		   u8 byMSCapExNum					  [IN]主流扩展能力集勾选数目
		   cosnt TVideoStreamCap *ptDSCapEx   [IN]双流扩展能力集勾选数组
		   u8 byDSCapEXNum					  [IN]双流扩展能力集勾选数目

返 回 值： BOOL32:第一路是否匹配成功  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20110408    4.6         朱胜泽                 create
=============================================================================*/
BOOL32 CMtAdpUtils::MatchCommonCap( const TCapSupport &tLocalCap,
                                         const TCapSupportEx &tCapEx,
								         TCapSet *ptRemoteCapSet,
									     TMultiCapSupport &tCommonCap,
									     u8 byEncryptMode,
										 const TVideoStreamCap *ptMSCapEx,
										 u8 byMSCapExNum,			 
										 const TVideoStreamCap *ptDSCapEx,
										 u8 byDSCapEXNum,
										 const TAudioTypeDesc *ptLocalAudioTypeDesc,
										 u8 byAudioCapNum
										 )
{
		
    TSimCapSet tSimuCap1 = tLocalCap.GetMainSimCapSet();
	TSimCapSet tSimuCap2 = tLocalCap.GetSecondSimCapSet();
    TDStreamCap tSimuDSCap = tLocalCap.GetDStreamCapSet();

	//[1].取第一路主格式共同能力
    BOOL32 bMainMatchSuccess = FALSE;
	TSimCapSet tMainSimCapSet = tSimuCap1; 
	TSimCapSet tMainExtraSimCapSet;


	//音频能力集匹配代码
	BOOL32 bAudioMatchSuccess = FALSE;
	bAudioMatchSuccess = MatchAudioCap(byEncryptMode,byAudioCapNum,
								ptLocalAudioTypeDesc,ptRemoteCapSet,tCommonCap);
	if (bAudioMatchSuccess)
    {
		TAudioStreamCap tAudioStreamCap;
		tAudioStreamCap.SetMediaType( tCommonCap.GetMainAudioType() );
		tCommonCap.SetExtraAudioCap( tAudioStreamCap,0 ); 
        StaticLog( "[MatchCommonCap]MatchAudioCap (%d,%d) successfully!\n",
			tCommonCap.GetMainAudioType(),tCommonCap.GetMainAudioTrackNum());
    }
	else
	{
		StaticLog( "[MatchCommonCap]MatchAudioCap failed!\n");
	}
	tMainSimCapSet.SetAudioMediaType(tCommonCap.GetMainAudioType());

	//第一路主格式视频共同能力集
    u16 wMainVideoComType = GetComType(	PayloadTypeIn2Out(tMainSimCapSet.GetVideoMediaType()),
										byEncryptMode);
	if (ptRemoteCapSet->IsExistsimultaneous(&wMainVideoComType, 1))
	{//远端有本地主流视频能力类型的视频能力，取共同能力
	
        //第一路主格式能力集匹配
		bMainMatchSuccess = MatchVideoCap( tSimuCap1,
										   tSimuDSCap,
									       ptRemoteCapSet,
									       tMainSimCapSet,
									       byEncryptMode,
                                           tMainExtraSimCapSet,
										   ptMSCapEx,byMSCapExNum);		

	}
	else
	{
		StaticLog( "[MatchVideoCommonCap]Remote no main type as local main!\n");
	}

	if(tMainExtraSimCapSet.GetVideoMediaType()!=MEDIA_TYPE_NULL)
	{
		tCommonCap.SetExtraVideoCap(tMainExtraSimCapSet.GetVideoCap(),0);
	}
    
    if (bMainMatchSuccess)
    {
        //save第一路主格式共同能力集
        tCommonCap.SetMainSimCapSet(tMainSimCapSet);
		
		
    }
    else
    {
        //主格式视频无共同能力
        TVideoStreamCap tNullVideoCap;
        tNullVideoCap.Clear();
        tMainSimCapSet.SetVideoCap(tNullVideoCap);

        tCommonCap.SetMainSimCapSet(tMainSimCapSet);
        StaticLog( "[MatchVideoCommonCap] No Common Main Video Cap!\n");
    }
	

	//[2].取第一路辅格式共同能力集
    BOOL32 bSecondMatchSuccess = FALSE;
    TSimCapSet tSecondSimCapSet = tSimuCap2;
    tMainExtraSimCapSet.Clear();

    if(!tSecondSimCapSet.IsNull())
    {
        //第一路辅格式音频
        /*u16 wSecondAudioComType = GetComType(PayloadTypeIn2Out(tSecondSimCapSet.GetAudioMediaType()),
                                             byEncryptMode);
        if (!ptRemoteCapSet->IsExistsimultaneous(&wSecondAudioComType, 1))
        {
            tSecondSimCapSet.SetAudioMediaType(MEDIA_TYPE_NULL);
        }*/

        //第一路辅格式视频
        u16 wSecondVideoComType = GetComType(PayloadTypeIn2Out(tSecondSimCapSet.GetVideoMediaType()),
                                                               byEncryptMode);
        if (ptRemoteCapSet->IsExistsimultaneous(&wSecondVideoComType, 1))
        {
            //第一路辅格式能力集匹配
            bSecondMatchSuccess = MatchVideoCap(tSimuCap2,
												tSimuDSCap,
                                                ptRemoteCapSet,
                                                tSecondSimCapSet,
                                                byEncryptMode,
                                                tMainExtraSimCapSet,
												ptMSCapEx,byMSCapExNum);
        }
        else
        {
            StaticLog( "[MatchVideoCommonCap]Remote no Media type as Local Second!\n");
        }  
        
        if (bSecondMatchSuccess)
        {
            //save第一路辅格式共同能力集
            tCommonCap.SetSecondSimCapSet(tSecondSimCapSet);

			if(tMainExtraSimCapSet.GetVideoMediaType() != MEDIA_TYPE_NULL)
			{
				tCommonCap.SetExtraVideoCap(tMainExtraSimCapSet.GetVideoCap(),0);
			}
			tSecondSimCapSet.SetAudioMediaType(tCommonCap.GetMainAudioType());
			if (tCommonCap.GetMainVideoType() == MEDIA_TYPE_NULL)
			{
				tCommonCap.SetMainSimCapSet(tSecondSimCapSet);
			}
        }
        else
        {
            StaticLog( "[MatchVideoCommonCap] No Common Second Cap!\n");
        }
    }

	//[3].取双流视频共同能力
    BOOL32 bDSMatchSuccess = FALSE;
    BOOL32 bH239 = tLocalCap.IsDStreamSupportH239();  

    //第一共同双流
    TDStreamCap tCommonDSCap;
    tCommonDSCap.Reset();

    //第二共同双流
    TDStreamCap tSecComDStreamCap;
    tSecComDStreamCap.Reset();
    
    TVideoStreamCap tMainVideoCap = tCommonCap.GetMainSimCapSet().GetVideoCap();

	if ( MEDIA_TYPE_NULL != tLocalCap.GetDStreamMediaType())
	{//第一路主格式匹配成功，匹配双流
                
        //第一路主格式+双流
		bDSMatchSuccess = MatchDSVideoCap(tMainVideoCap,
                                          tSimuDSCap,
                                          ptRemoteCapSet,
                                          tCommonDSCap,
                                          tSecComDStreamCap,
                                          byEncryptMode,
										  ptDSCapEx,
										  byDSCapEXNum);

        if (bDSMatchSuccess)
        {
            //保存双流共同能力集
            tCommonCap.SetDStreamCapSet(tCommonDSCap);
			//保存第二共同双流
            if (MEDIA_TYPE_NULL != tSecComDStreamCap.GetMediaType())
            {
                tCommonCap.SetSecComDStreamCapSet(tSecComDStreamCap);
            }
        }
    }
    else
    {
        StaticLog( "[MatchVideoCommonCap]local cap no Double Stream!");
    }

    if ( MEDIA_TYPE_NULL != tCapEx.GetSecDSType())
    {//第一路主格式+第一双流不成功，用第一路主格式+第二双流

        TDStreamCap tDStreamCap2;
        tDStreamCap2.SetSupportH239(bH239);
        tDStreamCap2.SetMediaType(tCapEx.GetSecDSType());
        tDStreamCap2.SetMaxBitRate(tCapEx.GetSecDSBitRate());
        tDStreamCap2.SetResolution(tCapEx.GetSecDSRes());
        
        if (MEDIA_TYPE_H264 == tCapEx.GetSecDSType())
        {
            tDStreamCap2.SetUserDefFrameRate(tCapEx.GetSecDSFrmRate());
        }
        else
        {
            tDStreamCap2.SetFrameRate(tCapEx.GetSecDSFrmRate());
        }

		tCommonDSCap.Reset();
		tSecComDStreamCap.Reset();

        //第一路主格式+第二双流
        bDSMatchSuccess = MatchDSVideoCap( tMainVideoCap,
                                           tDStreamCap2,
                                           ptRemoteCapSet,
                                           tCommonDSCap,
                                           tSecComDStreamCap,
                                           byEncryptMode,
										   ptDSCapEx,
										   byDSCapEXNum);

        if (bDSMatchSuccess)
        {            
            
            //保存双流共同能力集
            //如果双双流能力比当前第一或第二双流分辨率大||分辨率相等的情况下帧率大于第一或第二双流
            //那么用双双流替换第一或第二双流，替换的前提是分辨率相同类型（同为静态或同为动态）
            if ( MEDIA_TYPE_NULL == tCommonCap.GetDStreamMediaType() ||
                 ( IsResIsSameType(tCommonCap.GetDStreamResolution(), tCommonDSCap.GetResolution()) &&
                   ( tCommonCap.GetDStreamResolution() < tCommonDSCap.GetResolution())))
            {
                tCommonCap.SetDStreamCapSet(tCommonDSCap);
            }
            else
            {
                TDStreamCap tSecComDStreamSet = tCommonCap.GetSecComDStreamCapSet();
                if ( MEDIA_TYPE_NULL == tSecComDStreamSet.GetMediaType() ||
                     ( IsResIsSameType( tSecComDStreamSet.GetResolution(), tCommonDSCap.GetResolution()) &&
                       ( tSecComDStreamSet.GetResolution() < tCommonDSCap.GetResolution())))
                {
                    tCommonCap.SetSecComDStreamCapSet(tCommonDSCap);
                }
            }
        }
    }


    TDStreamCap tComDStream = tCommonCap.GetDStreamCapSet();
    TDStreamCap tSecComDStream = tCommonCap.GetSecComDStreamCapSet();
	//同为H264,优选分辨率，帧率乘积较大能力[9/5/2012 chendaiwei]
	if( tComDStream.GetMediaType() == MEDIA_TYPE_H264 &&
		tSecComDStream.GetMediaType() == MEDIA_TYPE_H264 &&
		tComDStream.IsH264CapLower(tSecComDStream.GetResolution(),tSecComDStream.GetUserDefFrameRate()))
	{
        tCommonCap.SetDStreamCapSet(tSecComDStream);
        tCommonCap.SetSecComDStreamCapSet(tComDStream);
	}
    //优先分辨率高的双流
	else if( tSecComDStream.GetMediaType() != MEDIA_TYPE_NULL
		&& tComDStream.IsResLowerInProduct(tComDStream.GetResolution(), tSecComDStream.GetResolution()))
	{
        tCommonCap.SetDStreamCapSet(tSecComDStream);
        tCommonCap.SetSecComDStreamCapSet(tComDStream);
    }

	return bMainMatchSuccess | bSecondMatchSuccess;
}

/*=============================================================================
函 数 名： MatchVideoCap
功    能： 匹配第一路视频能力集
算法实现：
全局变量： 
参    数： const TSimCapSet &tLocalCap  [IN]本地第一路能力集
		   const TDStreamCap &tLocalDStreamCap [IN]本地双流能力集
           TCapSet *ptRemoteCapSet      [IN]对端能力集
           TSimCapSet &tCommonCap       [OUT]第一路共同能力集    
           u8 byEncryptMode             [IN]加密模式
           TSimCapSet &tExtraCommonCap        [OUT]第二组第一路共同能力集（不依赖于勾选）   
		   const TVideoStreamCap *ptMSCapEx   [IN]主流扩展能力集勾选数组
		   u8 byMSCapExNum					  [IN]主流扩展能力集勾选数目
		   cosnt TVideoStreamCap *ptDSCapEx   [IN]双流扩展能力集勾选数组
		   u8 byDSCapEXNum					  [IN]双流扩展能力集勾选数目

返 回 值： BOOL32:是否匹配成功  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20110408    4.6         朱胜泽                 create
20111216    4.7         陈代伟					modify
=============================================================================*/
BOOL32 CMtAdpUtils::MatchVideoCap(  const TSimCapSet &tLocalCap, 
									const TDStreamCap &tLocalDStreamCap,
								    TCapSet *ptRemoteCapSet, 
								    TSimCapSet &tCommonCap,
								    u8 byEncryptMode,
                                    TSimCapSet &tExtraCommonCap,
									const TVideoStreamCap *ptMSCapEx,
									u8 byMSCapExNum)
{
	u16 awStreamComType[2] = {0};
	u8 byPayload = PayloadTypeIn2Out(tLocalCap.GetVideoMediaType());
    u16 wFStreamComType = GetComType(byPayload, byEncryptMode);
	awStreamComType[0] = wFStreamComType;
	
    TH264VideoCap atH264Cap[2];	
    u8 byDStreamPayload = PayloadTypeIn2Out(tLocalDStreamCap.GetMediaType());
    awStreamComType[1] = GetComType(byDStreamPayload, byEncryptMode);
	
	// 标识主流是否有共同能力集 [12/8/2011 chendaiwei]
    BOOL32 bMatchSuccess = FALSE;

    u16 wBitrate = 0;
    u16 wLocalVideoMaxBitRate = tLocalCap.GetVideoMaxBitRate();

    if ( (u8)v_h264 == byPayload )
    {
		//1.以会议主分辨率，采取逐级降分辨率的方式比共同能力集，保存在第二组共同能力集中，主要供VCS单方调度使用[12/16/2011 chendaiwei]
		// Convert to tStackCap
		TH264VideoCap tStackCap;
		TSimCapSet tLocalSimCapSet = tLocalCap;

		u8 abyRes[] = { VIDEO_FORMAT_HD1080, VIDEO_FORMAT_HD720, VIDEO_FORMAT_4CIF, VIDEO_FORMAT_CIF };
		u8 byResNum = sizeof(abyRes)/sizeof(u8);

		//本地视频能力分辨率
		u8 byLocalResPos;
		for (byLocalResPos = 0; byLocalResPos < byResNum; byLocalResPos++)
		{
			if (tLocalSimCapSet.GetVideoCap().GetResolution() == abyRes[byLocalResPos])
			{
				break;
			}
			else
			{
				if ( byLocalResPos == byResNum - 1 )
				{
					LogPrint(LOG_LVL_KEYSTATUS, MID_MCULIB_MTADP, "[MatchVideoCap]not find Res of h264 video cap of local main cap !");
					return FALSE;
				}
				else
				{
					continue;
				}				
			}
		}

		//分辨率适配
		for (u8 byResIdx = byLocalResPos; byResIdx < byResNum; byResIdx++)
		{
			//4cif以下分辨率最大帧率为25帧
			if( VIDEO_FORMAT_4CIF >= abyRes[byResIdx] &&
				25 < tLocalSimCapSet.GetUserDefFrameRate() )			
			{	
				tLocalSimCapSet.SetUserDefFrameRate(25);								
			}
			
			tLocalSimCapSet.SetVideoResolution(abyRes[byResIdx]);
			
			TVideoStreamCap tLocalVideoCap = tLocalSimCapSet.GetVideoCap();			
			tStackCap.Clear();
			VideoCap264In2Out(tLocalVideoCap, tStackCap);
			
			if (ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType, 1, &tStackCap))
			{//匹配成功，设置共同能力集
				
				tExtraCommonCap.SetVideoCap(tLocalVideoCap);
				break;
			} 
			
			//匹配失败,切换HP/BP进行匹配				
			if( tLocalSimCapSet.GetVideoProfileType() == emHpAttrb )
			{
				tLocalVideoCap.SetH264ProfileAttrb(emBpAttrb);
				tStackCap.Clear();
				VideoCap264In2Out(tLocalVideoCap, tStackCap);

				if (ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType, 1, &tStackCap))
				{
					tExtraCommonCap.SetVideoCap(tLocalVideoCap);
					break;
				}
			}

			if( VIDEO_FORMAT_4CIF != tLocalSimCapSet.GetVideoResolution() &&
					VIDEO_FORMAT_CIF != tLocalSimCapSet.GetVideoResolution() &&
					tLocalVideoCap.GetUserDefFrameRate() > 30 )
			{
				if( tLocalSimCapSet.GetVideoProfileType() == emHpAttrb )
				{
					tLocalVideoCap.SetH264ProfileAttrb(emHpAttrb);
				}
				else
				{
					tLocalVideoCap.SetH264ProfileAttrb(emBpAttrb);
				}
				tLocalVideoCap.SetUserDefFrameRate( tLocalVideoCap.GetUserDefFrameRate() / 2 ) ;
				tStackCap.Clear();
				VideoCap264In2Out(tLocalVideoCap, tStackCap);

				if (ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType, 1, &tStackCap))
				{
					tExtraCommonCap.SetVideoCap(tLocalVideoCap);
					break;
				}

				if( tLocalVideoCap.GetH264ProfileAttrb() == emHpAttrb )
				{
					tLocalVideoCap.SetH264ProfileAttrb(emBpAttrb);					
					tStackCap.Clear();
					VideoCap264In2Out(tLocalVideoCap, tStackCap);

					if (ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType, 1, &tStackCap))
					{
						tExtraCommonCap.SetVideoCap(tLocalVideoCap);
						break;
					}
				}
			}			

			if(byResIdx == byResNum - 1)
			{
				tExtraCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);

				break;
			} 
			
		}

		// guzh [2/27/2007] 对速率进行保存
        //[20110729 zhushz]pclnt err:666
        u16 wStackCapBitRate =  tStackCap.GetBitRate();
        
        if (wStackCapBitRate != 0)
        {
            if (wLocalVideoMaxBitRate != 0)
            {
                wBitrate = min(wStackCapBitRate, wLocalVideoMaxBitRate);
            }
            else
            {
                wBitrate = tStackCap.GetBitRate();
            }
        }
		tExtraCommonCap.SetVideoMaxBitRate(wBitrate);
		
		//2.采用界面勾选扩展能力集直接比能力集[12/16/2011 chendaiwei]
        // Convert to tStackCap
		TVideoStreamCap tLocalMainVideoCap = tLocalCap.GetVideoCap();
		tStackCap.Clear();
        VideoCap264In2Out(tLocalMainVideoCap, tStackCap);

		//标识是否有双流最小共同能力H263+/XGA/5FPS或H264/XGA/5FPS  [12/8/2011 chendaiwei]
		BOOL32 bExistDSMinCap = FALSE;

		//  [12/8/2011 chendaiwei]主能力集匹配成功，设置共同能力集；协商是否有双流最小共同能力集
		if (ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType, 1, &tStackCap))
		{	
			tCommonCap.SetVideoCap(tLocalMainVideoCap);
			bMatchSuccess = TRUE;

			// 协商双流最小共同能力集 [12/8/2011 chendaiwei]
			//1)用H263+/XGA/5FPS尝试
			u8 byH263PlusPayload = PayloadTypeIn2Out(MEDIA_TYPE_H263PLUS);
			u16 wH263ComType = GetComType(byH263PlusPayload, byEncryptMode);
			if (tLocalDStreamCap.IsSupportH239() && 
				ptRemoteCapSet->IsSupportH239())
			{
				wH263ComType = GetH239Type(wH263ComType);
			}

			if (ptRemoteCapSet->IsExistsimultaneous(&wH263ComType, 1))
			{
				bExistDSMinCap = TRUE;
			}

			//2)用H264/XGA/5FPS尝试
			TVideoStreamCap tMinH264DSCap;
			tMinH264DSCap.Clear();
			tMinH264DSCap.SetMediaType(MEDIA_TYPE_H264);
			tMinH264DSCap.SetResolution(VIDEO_FORMAT_XGA);
			tMinH264DSCap.SetUserDefFrameRate(5);
			tMinH264DSCap.SetMaxBitRate(tLocalDStreamCap.GetMaxBitRate());

			atH264Cap[0].Clear();
			VideoCap264In2Out(tLocalMainVideoCap, atH264Cap[0]);
			atH264Cap[1].Clear();
			VideoCap264In2Out(tMinH264DSCap, atH264Cap[1]);

			if (ptRemoteCapSet->IsExistsimultaneous(awStreamComType, 2, atH264Cap))
			{
				bExistDSMinCap = TRUE;
			}
		}

		// 1.主流无共同能力集2.主流有共同能力集的情况下，不满足双流共同能力集最低要求 [12/8/2011 chendaiwei]
		if(!bMatchSuccess || !bExistDSMinCap ) 
		{
			// 依次遍历主流扩展能力集数组[12/8/2011 chendaiwei]
			u8 byIdx = 0;
			for(; byIdx < byMSCapExNum; byIdx++)
			{
				TVideoStreamCap tMSCapEx = ptMSCapEx[byIdx];

				if(tMSCapEx.GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}

				tStackCap.Clear();
				VideoCap264In2Out(tMSCapEx, tStackCap);
				
				//  [12/8/2011 chendaiwei]扩展能力集匹配成功，设置共同能力集
				if (ptRemoteCapSet->IsExistsimultaneous(&wFStreamComType, 1, &tStackCap))
				{
					if(!bMatchSuccess)
					{
						tCommonCap.SetVideoCap(tMSCapEx);
						bMatchSuccess = TRUE;
					}

					// 协商双流最小共同能力集 [12/8/2011 chendaiwei]
					//1)用H263+/XGA/5FPS尝试
					u8 byH263PlusPayload = PayloadTypeIn2Out(MEDIA_TYPE_H263PLUS);
					u16 wH263ComType = GetComType(byH263PlusPayload, byEncryptMode);
					if (tLocalDStreamCap.IsSupportH239() && 
						ptRemoteCapSet->IsSupportH239())
					{
						wH263ComType = GetH239Type(wH263ComType);
					}
					
					if (ptRemoteCapSet->IsExistsimultaneous(&wH263ComType, 1))
					{
						bExistDSMinCap = TRUE;
					}
					
					//用H264/XGA/5FPS尝试
					TVideoStreamCap tMinH264DSCap;
					tMinH264DSCap.Clear();
					tMinH264DSCap.SetMediaType(MEDIA_TYPE_H264);
					tMinH264DSCap.SetResolution(VIDEO_FORMAT_XGA);
					tMinH264DSCap.SetUserDefFrameRate(5);
					tMinH264DSCap.SetMaxBitRate(tLocalDStreamCap.GetMaxBitRate());
					
					atH264Cap[0].Clear();
					VideoCap264In2Out(tMSCapEx, atH264Cap[0]);
					atH264Cap[1].Clear();
					VideoCap264In2Out(tMinH264DSCap, atH264Cap[1]);
					
					if (ptRemoteCapSet->IsExistsimultaneous(awStreamComType, 2, atH264Cap))
					{
						bExistDSMinCap = TRUE;
					}
					
					// 主流有共同能力集，双流有最小共同能力集，成功返回 [12/8/2011 chendaiwei]
					if(bMatchSuccess && bExistDSMinCap)
					{
						tCommonCap.SetVideoCap(tMSCapEx);
						break;
					}
				}
			}

			if(!bMatchSuccess)
			{
				tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
			}
		}

        // guzh [2/27/2007] 对速率进行保存
        //[20110729 zhushz]pclnt err:666
        wStackCapBitRate =  tStackCap.GetBitRate();
        
        if (wStackCapBitRate != 0)
        {
            if (wLocalVideoMaxBitRate != 0)
            {
                wBitrate = min(wStackCapBitRate, wLocalVideoMaxBitRate);
            }
            else
            {
                wBitrate = tStackCap.GetBitRate();
            }
        }
    }
    else
    { 
        if( ptRemoteCapSet->IsExistsimultaneous( &wFStreamComType, 1) )
        {
            bMatchSuccess = TRUE;
        } 
        else
        {
            tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
            bMatchSuccess = FALSE;
        }

        TVideoCap* ptVideoCap = NULL;
        ptVideoCap = ptRemoteCapSet->GetVideoCap(byPayload);  
        
        u16 wVideoBitRate = 0;
        u8 byMPI = 0;
        u8 byVcRes = tLocalCap.GetVideoCap().GetResolution();
        if( NULL == ptVideoCap )
        {
            tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
            bMatchSuccess =  FALSE;
        }
        else
        {
            byMPI = ptVideoCap->GetResolution( (emResolution)ResIn2Out(byVcRes) );
            wVideoBitRate = ptVideoCap->GetBitRate();
        }
        
        if( 0 == byMPI )
        {
            tCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
            bMatchSuccess = FALSE;
        }
        
        // guzh [2/27/2007] 对速率进行保存
        if (wVideoBitRate != 0)
        {
            if (wLocalVideoMaxBitRate != 0)
            {
                wBitrate = min(wVideoBitRate, wLocalVideoMaxBitRate);
            }
            else
            {
                wBitrate = wVideoBitRate;
            }
        }

		tExtraCommonCap.SetVideoMediaType(MEDIA_TYPE_NULL);
    }										
    
    if ( wBitrate != 0 )
    {
        tCommonCap.SetVideoMaxBitRate(wBitrate);
    }

    return bMatchSuccess;
}

/*=============================================================================
函 数 名： MatchDSVideoCap
功    能： 匹配双流共同能力集
算法实现：
全局变量： 
参    数： const TVideoStreamCap &tLocalMainVideoCap  [IN]本地第一路能力集
           const TDStreamCap &tLocalDStreamCap        [IN]本地双流能力集
           TCapSet *ptRemoteCapSet                    [IN]对端能力集
           TVideoStreamCap  &tCommonDSCap             [OUT]第一路共同能力集    
           u8 byEncryptMode                           [IN]加密模式
           BOOL32 bH239                               [IN]是否打239标签
           u8 byAdpFps                                [IN]适配帧率

返 回 值： BOOL32:是否匹配成功  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20110408    4.6         朱胜泽                 create
=============================================================================*/
BOOL32 CMtAdpUtils:: MatchDSVideoCap(const TVideoStreamCap &tLocalMainVideoCap,
                                     const TDStreamCap &tLocalDStreamCap,
                                     TCapSet * ptRemoteCapSet,
                                     /*TVideoStreamCap*/TDStreamCap  &tCommonDSCap,
                                     TDStreamCap &tSecComDStreamCap,
                                     u8 byEncryptMode,
									 const TVideoStreamCap *ptDSCapEx,
									 u8 byDSCapEXNum)
{
    BOOL32 bMatchSuccess = FALSE;

    u16    awStreamComType[2] = {0}; 
    TH264VideoCap atH264Cap[2];

    u8 byFStreamPayload = PayloadTypeIn2Out(tLocalMainVideoCap.GetMediaType());
    awStreamComType[0] = GetComType(byFStreamPayload, byEncryptMode);

    u8 byDStreamPayload = PayloadTypeIn2Out(tLocalDStreamCap.GetMediaType());
    awStreamComType[1] = GetComType(byDStreamPayload, byEncryptMode);

    //对端无239标签，本端也不打239
    if (tLocalDStreamCap.IsSupportH239() && 
        ptRemoteCapSet->IsSupportH239())
    {
        awStreamComType[1] = GetH239Type(awStreamComType[1]);
    }

	

	//1)主双流H264
	if ((u8)v_h264 == byFStreamPayload && (u8)v_h264 == byDStreamPayload)
	{
		TVideoStreamCap tDSVideoCap = (TVideoStreamCap)tLocalDStreamCap;   
		atH264Cap[1].Clear();
		VideoCap264In2Out(tDSVideoCap, atH264Cap[1]);
		atH264Cap[0].Clear();
		VideoCap264In2Out(tLocalMainVideoCap, atH264Cap[0]);

		//双流主能力集比共同能力集
		if (ptRemoteCapSet->IsExistsimultaneous(awStreamComType, 2, atH264Cap))
		{
			//TODO 为什么不直接用比出来的双流能力集，而是要重新设置帧率[12/8/2011 chendaiwei]
			//取出匹配出的帧率设置到双流共同能力集中
			/*u8 byRemoteFps = atH264Cap[1].GetResolution(CMtAdpUtils::ResIn2Out(tDSVideoCap.GetResolution()));
			u8 byLocalFps = tDSVideoCap.GetUserDefFrameRate();
			u8 byFps = (byLocalFps < byRemoteFps) ? byLocalFps : byRemoteFps;			
			tDSVideoCap.SetUserDefFrameRate(byFps);
			
			//设置码率
			tDSVideoCap.SetMaxBitRate(tLocalDStreamCap.GetMaxBitRate());*/
			
			//设置共同双流
			memcpy(&tCommonDSCap, &tDSVideoCap, sizeof(TVideoStreamCap));
			tCommonDSCap.SetSupportH239(ptRemoteCapSet->IsSupportH239());
			
			bMatchSuccess = TRUE;
         }
		else
		{
			// 依次遍历双流扩展能力集数组[12/8/2011 chendaiwei]
			u8 byIdx = 0;
			for(; byIdx < byDSCapEXNum; byIdx++)
			{
				TVideoStreamCap tDSCapEx = ptDSCapEx[byIdx];
				
				if(tDSCapEx.GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}
				
				atH264Cap[1].Clear();
				VideoCap264In2Out(tDSCapEx, atH264Cap[1]);
				atH264Cap[0].Clear();
				VideoCap264In2Out(tLocalMainVideoCap, atH264Cap[0]);

				if (ptRemoteCapSet->IsExistsimultaneous(awStreamComType, 2, atH264Cap))
				{
					//TODO 为什么不直接用比出来的双流能力集，而是要重新设置帧率[12/8/2011 chendaiwei]
					//取出匹配出的帧率设置到双流共同能力集中
					/*u8 byRemoteFps = atH264Cap[1].GetResolution(CMtAdpUtils::ResIn2Out(tDSCapEx.GetResolution()));
					u8 byLocalFps = tDSCapEx.GetUserDefFrameRate();
					u8 byFps = (byLocalFps < byRemoteFps) ? byLocalFps : byRemoteFps;			
					tDSCapEx.SetUserDefFrameRate(byFps);*/					
					
					//设置共同双流
					memcpy(&tCommonDSCap, &tDSCapEx, sizeof(TVideoStreamCap));
					tCommonDSCap.SetSupportH239(ptRemoteCapSet->IsSupportH239());
					
					bMatchSuccess = TRUE;
					break;
				}
			}
			
			if(byIdx == byDSCapEXNum || ptDSCapEx[byIdx].GetMediaType() == MEDIA_TYPE_NULL)
			{
				tCommonDSCap.SetMediaType(MEDIA_TYPE_NULL);
				bMatchSuccess = FALSE;
			}
		}
	}
	//2)双流H264
	else if((u8)v_h264 != byFStreamPayload && (u8)v_h264 == byDStreamPayload)
	{
		TVideoStreamCap tDSVideoCap = (TVideoStreamCap)tLocalDStreamCap;   
		atH264Cap[1].Clear();
		VideoCap264In2Out(tDSVideoCap, atH264Cap[1]);
		
		//双流主能力集
		if (ptRemoteCapSet->IsExistsimultaneous(&awStreamComType[1], 1, &atH264Cap[1]))
		{
			//TODO 为什么不直接用比出来的双流能力集，而是要重新设置帧率[12/8/2011 chendaiwei]
			//取出匹配出的帧率设置到双流共同能力集中
			/*u8 byRemoteFps = atH264Cap[1].GetResolution(CMtAdpUtils::ResIn2Out(tDSVideoCap.GetResolution()));
			u8 byLocalFps = tDSVideoCap.GetUserDefFrameRate();
			u8 byFps = (byLocalFps < byRemoteFps) ? byLocalFps : byRemoteFps;			
			tDSVideoCap.SetUserDefFrameRate(byFps);
			
			//设置码率
			tDSVideoCap.SetMaxBitRate(tLocalDStreamCap.GetMaxBitRate());*/
			
			//设置共同双流
			memcpy(&tCommonDSCap, &tDSVideoCap, sizeof(TVideoStreamCap));
			tCommonDSCap.SetSupportH239(ptRemoteCapSet->IsSupportH239());
			
			bMatchSuccess = TRUE;
        }
		//双流扩展能力集
		else
		{
			// 依次遍历双流扩展能力集数组[12/8/2011 chendaiwei]
			u8 byIdx = 0;
			for(; byIdx < byDSCapEXNum; byIdx++)
			{
				TVideoStreamCap tDSCapEx = ptDSCapEx[byIdx];
				
				if(tDSCapEx.GetMediaType() == MEDIA_TYPE_NULL)
				{
					break;
				}
				
				atH264Cap[1].Clear();
				VideoCap264In2Out(tDSCapEx, atH264Cap[1]);

				if (ptRemoteCapSet->IsExistsimultaneous(&awStreamComType[1], 1, &atH264Cap[1]))
				{
					//TODO 为什么不直接用比出来的双流能力集，而是要重新设置帧率[12/8/2011 chendaiwei]
					//取出匹配出的帧率设置到双流共同能力集中
					/*u8 byRemoteFps = atH264Cap[1].GetResolution(CMtAdpUtils::ResIn2Out(tDSCapEx.GetResolution()));
					u8 byLocalFps = tDSCapEx.GetUserDefFrameRate();
					u8 byFps = (byLocalFps < byRemoteFps) ? byLocalFps : byRemoteFps;			
					tDSCapEx.SetUserDefFrameRate(byFps);	*/		
					
					
					//设置共同双流
					memcpy(&tCommonDSCap, &tDSCapEx, sizeof(TVideoStreamCap));
					tCommonDSCap.SetSupportH239(ptRemoteCapSet->IsSupportH239());
					
					bMatchSuccess = TRUE;
					break;
				}
			}
			
			if(byIdx == byDSCapEXNum || ptDSCapEx[byIdx].GetMediaType() == MEDIA_TYPE_NULL)
			{
				tCommonDSCap.SetMediaType(MEDIA_TYPE_NULL);
				bMatchSuccess = FALSE;
			}
		}
	}
	//3)双流非H264
	else if((u8)v_h264 != byDStreamPayload)
	{
        if (ptRemoteCapSet->IsExistsimultaneous(&awStreamComType[1], 1))
        {
            tCommonDSCap.SetMediaType(tLocalDStreamCap.GetMediaType());
            tCommonDSCap.SetResolution(tLocalDStreamCap.GetResolution());
            tCommonDSCap.SetFrameRate(tLocalDStreamCap.GetFrameRate());
            tCommonDSCap.SetMaxBitRate(tLocalDStreamCap.GetMaxBitRate());
            tCommonDSCap.SetSupportH239(ptRemoteCapSet->IsSupportH239());
            bMatchSuccess = TRUE;             
        }
	}

	//计算不同类型的双流第二共同能力集
    //第一共同双流与第二共同双流：一个为静态则另一个为动态
    if (bMatchSuccess)
    {
        if ((u8)v_h264 == byDStreamPayload)
        {
			TVideoStreamCap tDSFirstCap =  (TVideoStreamCap)tCommonDSCap;

			if ((u8)v_h264 == byFStreamPayload)
			{
				//TVideoStreamCap tLocalDSVideoCap = (TVideoStreamCap)tLocalDStreamCap; 
				// 依次遍历双流扩展能力集数组[12/8/2011 chendaiwei]
				u8 byIdx = 0;
				for(; byIdx < byDSCapEXNum; byIdx++)
				{
					TVideoStreamCap tDSCapEx = ptDSCapEx[byIdx];

					if(IsResIsSameType(tDSCapEx.GetResolution(),tDSFirstCap.GetResolution()))
					{
						continue;
					}
					
					if(tDSCapEx.GetMediaType() == MEDIA_TYPE_NULL)
					{
						break;
					}
					
					atH264Cap[1].Clear();
					VideoCap264In2Out(tDSCapEx, atH264Cap[1]);
					atH264Cap[0].Clear();
					VideoCap264In2Out(tLocalMainVideoCap, atH264Cap[0]);
					
					if (ptRemoteCapSet->IsExistsimultaneous(awStreamComType, 2, atH264Cap))
					{
						//TODO 为什么不直接用比出来的双流能力集，而是要重新设置帧率[12/8/2011 chendaiwei]
						//取出匹配出的帧率设置到双流共同能力集中
						/*u8 byRemoteFps = atH264Cap[1].GetResolution(CMtAdpUtils::ResIn2Out(tDSCapEx.GetResolution()));
						u8 byLocalFps = tDSCapEx.GetUserDefFrameRate();
						u8 byFps = (byLocalFps < byRemoteFps) ? byLocalFps : byRemoteFps;			
						tDSCapEx.SetUserDefFrameRate(byFps);*/						
						
						//第二共同双流能力集设置
						memcpy(&tSecComDStreamCap, &tDSCapEx, sizeof(TVideoStreamCap));
                        tSecComDStreamCap.SetSupportH239(ptRemoteCapSet->IsSupportH239());
						
						break;
					}
				}
			}
			else
			{	
				//TVideoStreamCap tLocalDSVideoCap = (TVideoStreamCap)tLocalDStreamCap; 
				// 依次遍历双流扩展能力集数组[12/8/2011 chendaiwei]
				u8 byIdx = 0;
				for(; byIdx < byDSCapEXNum; byIdx++)
				{
					TVideoStreamCap tDSCapEx = ptDSCapEx[byIdx];

					if(IsResIsSameType(tDSCapEx.GetResolution(),tDSFirstCap.GetResolution()))
					{
						continue;
					}
					
					if(tDSCapEx.GetMediaType() == MEDIA_TYPE_NULL)
					{
						break;
					}
					
					atH264Cap[1].Clear();
					VideoCap264In2Out(tDSCapEx, atH264Cap[1]);
					
					if (ptRemoteCapSet->IsExistsimultaneous(&awStreamComType[1], 1, &atH264Cap[1]))
					{
						//TODO 为什么不直接用比出来的双流能力集，而是要重新设置帧率[12/8/2011 chendaiwei]
						//取出匹配出的帧率设置到双流共同能力集中
						/*u8 byRemoteFps = atH264Cap[1].GetResolution(CMtAdpUtils::ResIn2Out(tDSCapEx.GetResolution()));
						u8 byLocalFps = tDSCapEx.GetUserDefFrameRate();
						u8 byFps = (byLocalFps < byRemoteFps) ? byLocalFps : byRemoteFps;			
						tDSCapEx.SetUserDefFrameRate(byFps);*/						
						
						//第二共同双流能力集设置
						memcpy(&tSecComDStreamCap, &tDSCapEx, sizeof(TVideoStreamCap));
                        tSecComDStreamCap.SetSupportH239(ptRemoteCapSet->IsSupportH239());
						
						break;
					}
				}
			}
		}
	}

    return bMatchSuccess;
}

/*=============================================================================
函 数 名： ReduceOneLevelMainCap
功    能： 将第一路能力降一个等级
算法实现：
全局变量： 
参    数： TVideoStreamCap &tViedoCap   [IN/OUT]第一路能力
           u8 byAdpFps                  [IN]适配帧率
           u8 byOldFps                  [IN]原始帧率

返 回 值： BOOL32:是否匹配成功  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20110408    4.6         朱胜泽                 create
=============================================================================*/
BOOL32 CMtAdpUtils::ReduceOneLevelMainCap(TVideoStreamCap &tViedoCap, u8 byAdpFps, u8 byOldFps)
{
   
    if (MEDIA_TYPE_NULL == tViedoCap.GetMediaType())
    {
        return FALSE;
    }
    
    u8 abyRes[] = { VIDEO_FORMAT_HD1080, VIDEO_FORMAT_HD720, VIDEO_FORMAT_4CIF, VIDEO_FORMAT_CIF };
    u8 byResNum = sizeof(abyRes)/sizeof(u8);

    u8 byFps;
    if(MEDIA_TYPE_H264 == tViedoCap.GetMediaType())
    {
        byFps = tViedoCap.GetUserDefFrameRate();
    }
    else
    {
        byFps = tViedoCap.GetFrameRate();
    }


    if (byFps > byAdpFps && 0 != byAdpFps)
    {
        if(tViedoCap.GetMediaType() == MEDIA_TYPE_H264)
        {
            tViedoCap.SetUserDefFrameRate(byAdpFps);
        }
        else
        {
            tViedoCap.SetFrameRate(byAdpFps);
        }

        return TRUE;
    }
    else
    {//已经降过帧率，将帧率复原

        if(tViedoCap.GetMediaType() == MEDIA_TYPE_H264)
        {
            tViedoCap.SetUserDefFrameRate(byOldFps);
        }
        else
        {
            tViedoCap.SetFrameRate(byOldFps);
        }
        
    }

    u8 byResPos;
    for(byResPos = 0; byResPos < byResNum; byResPos++)
    {
        if (tViedoCap.GetResolution() == abyRes[byResPos])
        {
            break;
        } 
        else
        {
            if (byResNum - 1 == byResPos)
            {
                return FALSE;             
            }
            else
            {
                continue;
            }
        }
    }

    if ( byResPos >= byResNum - 1/*VIDEO_FORMAT_CIF == abyRes[byResPos]*/ )
    {
        return FALSE;
    }
    else
    {
        tViedoCap.SetResolution(abyRes[byResPos + 1]);
        if (abyRes[byResPos + 1] <= VIDEO_FORMAT_4CIF &&
            byFps >25)
        {
            if(tViedoCap.GetMediaType() == MEDIA_TYPE_H264)
            {
                tViedoCap.SetUserDefFrameRate(25);
            }
            else
            {
                tViedoCap.SetFrameRate(25);
            }
        }
        return TRUE;
    }       
}
/*=============================================================================
函 数 名： MaxDSCap264Out2In
功    能： H.264 协议栈能力结构构造,取得最大双流能力
算法实现： 
全局变量： 
参    数：TCapSet *ptRemoteCapSet:远端能力[IN]
		   const TDStreamCap &tLocalDStreamCap:本端双流能力[IN] 
		   const TVideoStreamCap &tMainStreamCap:主流共同能力[IN]
		   u8 byEncryptMode:机密类型[IN] 
		   TVideoStreamCap  &tCommonDSCap:共同双流能力[OUT]
		   BOOL32 bDSEqMV:是否同主视频[IN] 
返 回 值： BOOL32:是否取得最大共同双流能力  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2010-12-10	1.0			朱胜泽		彭杰		创建	
=============================================================================*/
// BOOL32 CMtAdpUtils::MaxDSCap264Out2In(TCapSet *ptRemoteCapSet, 
// 									  const TDStreamCap &tLocalDStreamCap, 
// 									  const TVideoStreamCap &tMainStreamCap,
// 									  u8 byEncryptMode, 
// 									  TVideoStreamCap  &tCommonDSCap,
// 									  BOOL32 bDSEqMV)
// {
// 
// 	BOOL32 bSuppor = FALSE;	
// 	
// 	//部分分辨率不能对应到外部类型
// 	u8 abyRes[] = {VIDEO_FORMAT_HD1080, VIDEO_FORMAT_UXGA, /*VIDEO_FORMAT_16CIF,*/ VIDEO_FORMAT_SXGA, 
// 		           VIDEO_FORMAT_HD720, VIDEO_FORMAT_XGA, VIDEO_FORMAT_W4CIF, VIDEO_FORMAT_SVGA, 
// 				   VIDEO_FORMAT_4CIF, VIDEO_FORMAT_4SIF, VIDEO_FORMAT_VGA, VIDEO_FORMAT_2CIF, 
// 				   /*VIDEO_FORMAT_2SIF,*/ VIDEO_FORMAT_CIF, VIDEO_FORMAT_SIF, /*VIDEO_FORMAT_QCIF,*/ 
// 				   /*VIDEO_FORMAT_SQCIF,*/ VIDEO_FORMAT_AUTO};
// 	
// 	u8 abyEqMVRes[] = {VIDEO_FORMAT_HD1080, VIDEO_FORMAT_HD720, VIDEO_FORMAT_4CIF, VIDEO_FORMAT_CIF};
// 
// 	//分辨率个数
// 	u8 byMaxResNum;
// 	if (bDSEqMV)
// 	{
// 		byMaxResNum = sizeof(abyEqMVRes)/sizeof(u8);
// 	}
// 	else
// 	{
// 		byMaxResNum = sizeof(abyRes)/sizeof(u8);
// 	}
// 
// 	u16 awStreamComType[2] = {0,0};
// 	
// 	//获取共同能力集中第一流能力
// 	u8 byPayLoadM = PayloadTypeIn2Out(tMainStreamCap.GetMediaType());
// 	awStreamComType[0] = GetComType(byPayLoadM, byEncryptMode);
// 	
// 	//获取本地双流能力
// 	TVideoStreamCap tDSVideoCap = (TVideoStreamCap)tLocalDStreamCap;
// 	u8 byPayloadD = PayloadTypeIn2Out(tDSVideoCap.GetMediaType());		
// 	awStreamComType[1] = GetComType(byPayloadD, byEncryptMode);
// 
// 	TH264VideoCap tLocal264Cap[2];
// 	tLocal264Cap[0].Clear();
// 	
// 	if ((u8)v_h264 == byPayLoadM)
// 	{			
// 		VideoCap264In2Out(tMainStreamCap, tLocal264Cap[0]);			
// 	}	
// 
// 	//非同主格式下给双流打239标签
// 	if(!bDSEqMV)  	
//   	{
// 		awStreamComType[1] = GetH239Type(awStreamComType[1]);
//  	}
// 	
// 	for (u8 byLoop = 0; byLoop < byMaxResNum; byLoop++)
// 	{
// 		//设置分辨率
// 		if (bDSEqMV)
// 		{
// 			tDSVideoCap.SetResolution( abyEqMVRes[byLoop] );
// 		}
// 		else
// 		{
// 			tDSVideoCap.SetResolution( abyRes[byLoop] );
// 		}
// 
// 		tLocal264Cap[1].Clear();
// 		VideoCap264In2Out(tDSVideoCap, tLocal264Cap[1]);
// 				
// 		if ((u8)v_h264 == byPayLoadM)
// 		{//第一路也为264，那么比两路			
// 			if(ptRemoteCapSet->IsExistsimultaneous(awStreamComType, 2, tLocal264Cap))
// 			{
// 				u8 byRemoteFps = tLocal264Cap[1].GetResolution(CMtAdpUtils::ResIn2Out(tDSVideoCap.GetResolution()));
// 				u8 byLocalFps = tLocalDStreamCap.GetUserDefFrameRate();
// 				u8 byFps = (byLocalFps < byRemoteFps) ? byLocalFps : byRemoteFps;			
// 	 			tDSVideoCap.SetUserDefFrameRate(byFps);
// 				tCommonDSCap = tDSVideoCap;	
// 				bSuppor = TRUE;
// 				break;
// 			}
// 			
// 			//非同主视频下降一级帧率再进行比较
// 			if(!bDSEqMV)
// 			{
// 				tLocal264Cap[1].SetMaxMBPS( tLocal264Cap[1].GetMaxFS() );
// 				if ( ptRemoteCapSet->IsExistsimultaneous( awStreamComType, 2, tLocal264Cap ) )
// 				{
// 					u8 byFps = tLocal264Cap[1].GetResolution( CMtAdpUtils::ResIn2Out(tDSVideoCap.GetResolution()) );
// 					tDSVideoCap.SetUserDefFrameRate(byFps);
// 					tCommonDSCap = tDSVideoCap;
// 					bSuppor = TRUE;
// 					break;
// 				}
// 			}
// 
// 			//特殊处理，避开SVGA，尝试3帧的XGA
// 			if( tDSVideoCap.GetResolution() == VIDEO_FORMAT_SVGA )
// 			{
// 				TVideoStreamCap tTempVideoCap = tDSVideoCap;
// 				tTempVideoCap.SetResolution( VIDEO_FORMAT_XGA );
// 				tTempVideoCap.SetUserDefFrameRate( 3 );
// 				tLocal264Cap[1].Clear();
// 				VideoCap264In2Out(tTempVideoCap, tLocal264Cap[1]);
// 				if(ptRemoteCapSet->IsExistsimultaneous(awStreamComType, 2, tLocal264Cap))
// 				{
// 					tCommonDSCap = tTempVideoCap;
// 					bSuppor = TRUE;
// 					break;
// 				}
// 			}
// 		}
// 		else
// 		{//第一路不为264，那么比双流
// 			if(ptRemoteCapSet->IsExistsimultaneous(&awStreamComType[1], 1, &tLocal264Cap[1]))
// 			{				
// 				u8 byRemoteFps = tLocal264Cap[1].GetResolution(CMtAdpUtils::ResIn2Out(tDSVideoCap.GetResolution()));
// 				u8 byLocalFps = tLocalDStreamCap.GetUserDefFrameRate();
// 				u8 byFps = (byLocalFps < byRemoteFps) ? byLocalFps : byRemoteFps;
// 	 			tDSVideoCap.SetUserDefFrameRate(byFps);
// 				tCommonDSCap = tDSVideoCap;			
// 				bSuppor = TRUE;
// 				break;
// 			}
// 
// 			//非同主视频下降一级分辨率再进行比较
// 			if(!bDSEqMV)
// 			{
// 				tLocal264Cap[1].SetMaxMBPS( tLocal264Cap[1].GetMaxFS() );
// 				if ( ptRemoteCapSet->IsExistsimultaneous( &awStreamComType[1], 1, &tLocal264Cap[1]))
// 				{
// 					u8 byFps = tLocal264Cap[1].GetResolution( CMtAdpUtils::ResIn2Out(tDSVideoCap.GetResolution()) );
// 					tDSVideoCap.SetUserDefFrameRate(byFps);
// 					tCommonDSCap = tDSVideoCap;
// 					bSuppor = TRUE;
// 					break;
// 				}
// 			}
// 
// 			//特殊处理，避开SVGA，尝试3帧的XGA
// 			if( tDSVideoCap.GetResolution() == VIDEO_FORMAT_SVGA )
// 			{
// 				TVideoStreamCap tTempVideoCap = tDSVideoCap;
// 				tTempVideoCap.SetResolution( VIDEO_FORMAT_XGA );
// 				tTempVideoCap.SetUserDefFrameRate( 3 );
// 				tLocal264Cap[1].Clear();
// 				VideoCap264In2Out(tTempVideoCap, tLocal264Cap[1]);
// 				if(ptRemoteCapSet->IsExistsimultaneous(&awStreamComType[1], 1, &tLocal264Cap[1]))
// 				{
// 					tCommonDSCap = tTempVideoCap;
// 					bSuppor = TRUE;
// 					break;
// 				}
// 			}
// 		}
// 		
// 	}			
// 	return bSuppor;
// }

/*=============================================================================
函 数 名： VideoCap264Out2In
功    能： H.264 协议栈能力结构构造
算法实现： 分辨率优先
全局变量： 
参    数： BOOL32 bDual：是否取双流类分辨率
		   u8 byDSFPS：按多大的帧率取分辨率
           BOOL32 bDSEqMV: 是否为同主视频的双流
返 回 值： void  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
2008/08/12  4.0R5	    顾振华                  创建
2008/09/03	4.5			张宝卿					增加对双流类型的获取处理
2008/04/29	4.5			张宝卿					区分双流类型处理
=============================================================================*/
void CMtAdpUtils::VideoCap264Out2In(const TH264VideoCap     tRemoteCap, 
                                    const TVideoStreamCap  &tLocalCap,
                                    TVideoStreamCap        &tInCap,
                                    BOOL32                  bDual,
                                    u8                      byDSFPS,
                                    BOOL32                  bDSEqMV)
{
    tInCap.SetMediaType(MEDIA_TYPE_H264);

	if (!bDual)
	{
		u8 byLocalFrameRate = tLocalCap.GetUserDefFrameRate();
        u8 byLocalCIF4CIFFrmRate = byLocalFrameRate > 25 ? 25 : byLocalFrameRate;
		if ( tRemoteCap.IsSupport( (u8)emHD1080, byLocalFrameRate ) )
		{
			tInCap.SetResolution( VIDEO_FORMAT_HD1080 );
			tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emHD1080) );
		}
		else if ( tRemoteCap.IsSupport( (u8)emHD720, byLocalFrameRate ) )
		{
			tInCap.SetResolution( VIDEO_FORMAT_HD720 );
			tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emHD720) );
		}
		else if ( tRemoteCap.IsSupport( (u8)emCIF4, byLocalCIF4CIFFrmRate ) )
		{
			tInCap.SetResolution( VIDEO_FORMAT_4CIF);
			tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emCIF4) );
		} 
		else if ( tRemoteCap.IsSupport( (u8)emCIF, byLocalCIF4CIFFrmRate ) )
		{
			tInCap.SetResolution( VIDEO_FORMAT_CIF );
			tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emCIF) );
		} 
		else
		{
			tInCap.SetMediaType(MEDIA_TYPE_NULL);
		}
	}
	else
	{
		//zbq[09/03/2008] FIXME:双流能力的获取暂时走5帧能力作判断，可能会有出入...(稍后给配置文件？)
		//zbq[09/17/2008] 双流能力获取帧率按传入参数走，传入为0，走默认5帧

		if (byDSFPS == 0)
		{
			byDSFPS = 5;
		}

        if (!bDSEqMV)
        {
            if ( tRemoteCap.IsSupport( (u8)emUXGA, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_UXGA );
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emUXGA) );
            }
            else if ( tRemoteCap.IsSupport( (u8)emSXGA, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_SXGA );
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emSXGA) );
            }
            else if ( tRemoteCap.IsSupport( (u8)em1024768, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_XGA );
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)em1024768) );
            }
            else if ( tRemoteCap.IsSupport( (u8)em800600, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_SVGA );
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)em800600) );
            }
            else if ( tRemoteCap.IsSupport( (u8)em640480, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_VGA );
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)em640480) );
            }
            else
            {
                tInCap.SetMediaType(MEDIA_TYPE_NULL);
            }
        }
        else
        {
			if ( tRemoteCap.IsSupport( (u8)emHD1080, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_HD1080);
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emHD1080) );
            } 
            else if ( tRemoteCap.IsSupport( (u8)emHD720, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_HD720);
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emHD720) );
            } 
            else if ( tRemoteCap.IsSupport( (u8)emCIF4, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_4CIF );
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emCIF4) );
            }
            else if ( tRemoteCap.IsSupport( (u8)emCIF, byDSFPS ) )
            {
                tInCap.SetResolution( VIDEO_FORMAT_CIF );
                tInCap.SetUserDefFrameRate( tRemoteCap.GetResolution((u8)emCIF) );
            }
            else
            {
                tInCap.SetMediaType(MEDIA_TYPE_NULL);
            }
        }
	}

    return;
}


/*=============================================================================
  函 数 名： GetVideoBitrate 
  功    能： 从能力集获取视频码率 
  算法实现： 
  全局变量： 
  参    数： TCapSet *ptCapSet
  u8 byMediaType
  返 回 值： u16 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2005/8/25    3.6			许世林              创建
2008/04/15  4.0R5	    顾振华                 不支持H.264
=============================================================================*/
/*lint -save -esym(429, ptCapSet)*/
u16 CMtAdpUtils::GetVideoBitrate( TCapSet *ptCapSet, u8 byMediaType )
{
    u16 wBitrate = 0;

    if( NULL != ptCapSet )
    {
        switch(byMediaType)
        {
        case MEDIA_TYPE_H261:
            wBitrate = ptCapSet->GetVideoCap((u16)v_h261)->GetBitRate();
            break;
			
        case MEDIA_TYPE_H262:
            wBitrate = ptCapSet->GetVideoCap((u16)v_h262)->GetBitRate();
            break;
			
        case MEDIA_TYPE_H263:
            wBitrate = ptCapSet->GetVideoCap((u16)v_h263)->GetBitRate();
            break;
			
        case MEDIA_TYPE_H263PLUS:
            wBitrate = ptCapSet->GetVideoCap((u16)v_h263plus)->GetBitRate();
            break;

        case MEDIA_TYPE_H264:
            StaticLog( "[GetVideoBitrate] Get H.264 Video Bitrate failed!\n");
            //guzh FIXME: 暂时这么处理
            wBitrate = ptCapSet->GetH264VideoCap(0)->GetBitRate();
            break;
			
        case MEDIA_TYPE_MP4:
            wBitrate = ptCapSet->GetVideoCap((u16)v_mpeg4)->GetBitRate();
            break;

        default:
            break;
        }
    }
	
    return wBitrate;
}
/* lint -restore*/
/*=============================================================================
  函 数 名： GetComType
  功    能： 得到媒体类型
  算法实现： 
  全局变量： 
  参    数： u8 byMediaType
             u8 byEncrypt
  返 回 值： u16 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/1/14                 xsl                   创建
=============================================================================*/
u16 CMtAdpUtils::GetComType( u8 byMediaType, u8 byEncrypt )
{
	if( emEncryptTypeNone != (emEncryptType)byEncrypt )
	{
		return GetEType(byMediaType, byEncrypt);
	}
	else
	{
		return (u16)byMediaType;
	}
}

/*=============================================================================
  函 数 名： EncryptTypeIn2Out
  功    能： 加密模式: mcu定义类型转换为协议栈定义类型
  算法实现： 
  全局变量： 
  参    数： u8 byEncryptMode
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2004/12/30				xsl                     创建
=============================================================================*/
u8 CMtAdpUtils::EncryptTypeIn2Out( u8 byEncryptMode )
{
	switch(byEncryptMode)
	{
	case CONF_ENCRYPTMODE_DES:		return (u8)emEncryptTypeDES;
	case CONF_ENCRYPTMODE_AES:		return (u8)emEncryptTypeAES;
	default:						return (u8)emEncryptTypeNone; //param invalid from mcu	
	}
}

/*=============================================================================
  函 数 名： EncryptTypeOut2In
  功    能： 加密模式：协议栈类型转换为mcu类型
  算法实现： 
  全局变量： 
  参    数： emEncryptType emType
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2004/12/30				xsl                     创建
=============================================================================*/
u8 CMtAdpUtils::EncryptTypeOut2In(emEncryptType emType)
{
	switch(emType)
	{
	case emEncryptTypeDES:			return CONF_ENCRYPTMODE_DES;
	case emEncryptTypeAES:			return CONF_ENCRYPTMODE_AES;
	case emEncryptTypeNone:		
	default:						return CONF_ENCRYPTMODE_NONE;
	}
}

/*=============================================================================
  函 数 名： AliasTypeIn2Out
  功    能： 内部别名类型转换为外部的别名类型
  算法实现： 
  全局变量： 
  参    数： u8 mtAliasType
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
/*
u8 CMtAdpUtils::AliasTypeIn2Out( u8 byMtAliasType )
{
	switch( byMtAliasType ) 
	{
	case mtAliasTypeE164:			return type_e164;
	case mtAliasTypeH323ID:			return type_h323ID; 	
	default:						return type_others;
	}
}*/

/*=============================================================================
  函 数 名： AliasTypeOut2In
  功    能： 外部别名类型转换为内部的别名类型
  算法实现： 
  全局变量： 
  参    数： AliasType type
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
/*
u8 CMtAdpUtils::AliasTypeOut2In( AliasType type )
{
	switch( type) 
	{
	case type_e164:	  return mtAliasTypeE164;
	case type_h323ID: return mtAliasTypeH323ID;	
	default:	      return mtAliasTypeOthers;
	}
}*/

/*=============================================================================
  函 数 名： MtAliasIn2Out
  功    能： 内部别名转换为外部别名结构
  算法实现： 
  全局变量： 
  参    数：  TMtAlias &tAlias
             cmAlias &tCmAlias
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
void CMtAdpUtils::MtAliasIn2Out( TMtAlias &tAlias, cmAlias &tCmAlias )
{
	/*
     *	AliasAddress ::= CHOICE
		{
			e164 IA5String (SIZE (1..128)) (FROM (“0123456789No. *,")),
			h323-ID BMPString (SIZE (1..256)), -- Basic ISO/IEC 10646-1 (Unicode)
			...,
			url-ID IA5String (SIZE(1..512)), -- URL style address
			transportID TransportAddress,
			email-ID IA5String (SIZE(1..512)), -- rfc822-compliant email address
			partyNumber PartyNumber
		}
	 */
	switch( tAlias.m_AliasType ) 
	{
	case mtAliasTypeE164:             tCmAlias.type = cmAliasTypeE164;             break;
	case mtAliasTypeH323ID:           tCmAlias.type = cmAliasTypeH323ID;           break;
	case mtAliasTypeTransportAddress: tCmAlias.type = cmAliasTypeTransportAddress; break;
	default:                          tCmAlias.type = cmAliasTypeH323ID;           break;	
	}

	if( (u8)mtAliasTypeTransportAddress == tAlias.m_AliasType )
	{
		tCmAlias.transport.type = cmTransportTypeIP;
		tCmAlias.transport.distribution = cmDistributionUnicast;
#ifdef WIN32
		tCmAlias.transport.ip = htonl(tAlias.m_tTransportAddr.GetIpAddr());
#else
		tCmAlias.transport.addr.v4.ip = htonl(tAlias.m_tTransportAddr.GetIpAddr());
#endif
		tCmAlias.transport.port = tAlias.m_tTransportAddr.GetPort();

		tCmAlias.string = NULL;
		tCmAlias.length = 0;
	}
	else if( (u8)mtAliasTypeE164 == tAlias.m_AliasType ) //IA5String
	{
		tAlias.m_achAlias[sizeof(tAlias.m_achAlias) - 1] = 0;

		if( tCmAlias.string )
		{
			strncpy( tCmAlias.string, tAlias.m_achAlias, 128 );

			//truncate illegal s8  
			for( /*s32*/u32 nAlias = 0; nAlias < sizeof(tAlias.m_achAlias) && tAlias.m_achAlias[nAlias]; nAlias ++ )
			{
				if( strchr("#*,0123456789", tAlias.m_achAlias[nAlias]))
				{
					tCmAlias.length ++;
				}
			}
		}				
	}
	else
	{	
		tAlias.m_achAlias[sizeof(tAlias.m_achAlias) - 1] = 0;
		if( tCmAlias.string )
		{			
#ifdef WIN32
            //[4/9/2013 liaokang] 编码方式支持
            u16 wLen = 0;
            if( emenCoding_Utf8 == g_cMtAdpApp.GetMcuEncoding() )
            {
                wLen = (u16)utlUTF82Uni( tAlias.m_achAlias, (BYTE*)tCmAlias.string );
            } 
            else if( emenCoding_GBK == g_cMtAdpApp.GetMcuEncoding() )
            {
                wLen = (u16)utlChr2Bmp( tAlias.m_achAlias, (BYTE*)tCmAlias.string );
            }
#else
            //[4/9/2013 liaokang] 编码方式支持
            u16 wLen = 0;
            if( emenCoding_Utf8 == g_cMtAdpApp.GetMcuEncoding() )
            {
                wLen = (u16)utlUTF82Uni( tAlias.m_achAlias, (RvUint8*)tCmAlias.string );
            } 
            else if( emenCoding_GBK == g_cMtAdpApp.GetMcuEncoding() )
            {
                wLen = (u16)utlChr2Bmp( tAlias.m_achAlias, (RvUint8*)tCmAlias.string );
            }
#endif
			tCmAlias.length = wLen;
		}		
	}

	return;
}


/*=============================================================================
  函 数 名： MtAliasOut2In
  功    能： 外部别名转换为内部别名结构 
  算法实现： 
  全局变量： 
  参    数： const cmAlias &tCmAlias
             TMtAlias &tAlias
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
void CMtAdpUtils::MtAliasOut2In( const cmAlias &tCmAlias, TMtAlias &tAlias )
{
	switch( tCmAlias.type ) 
	{
	case cmAliasTypeE164:      
		tAlias.m_AliasType = (u8)mtAliasTypeE164; 		
		break;
		
	case cmAliasTypeH323ID:     
		{
			s8 achBuf[128] = { 0 };
			memcpy( achBuf, tCmAlias.string, min(sizeof(achBuf), tCmAlias.length));
#ifdef WIN32
#ifdef _UTF8
            //[4/9/2013 liaokang] 编码方式支持
            if( emenCoding_Utf8 == g_cMtAdpApp.GetMcuEncoding() )
            {
                utlUni2UTF8((u8*)achBuf, sizeof(achBuf), tCmAlias.string);
            } 
            else if( emenCoding_GBK == g_cMtAdpApp.GetMcuEncoding() )
            {
                utlBmp2Chr(tCmAlias.string, (u8*)achBuf, sizeof(achBuf));
            }
#else
			utlBmp2Chr(tCmAlias.string, (u8*)achBuf, sizeof(achBuf));
#endif
#else
#ifdef _UTF8
            //[4/9/2013 liaokang] 编码方式支持
            if( emenCoding_Utf8 == g_cMtAdpApp.GetMcuEncoding() )
            {
                utlUni2UTF8((RvUint16*)achBuf, sizeof(achBuf), tCmAlias.string);
            } 
            else if( emenCoding_GBK == g_cMtAdpApp.GetMcuEncoding() )
            {
                utlBmp2Chr(tCmAlias.string, (u8*)achBuf, sizeof(achBuf));
            }
#else
            utlBmp2Chr(tCmAlias.string, (u8*)achBuf, sizeof(achBuf));
#endif
#endif
			tAlias.m_AliasType = (u8)mtAliasTypeH323ID;	
		}			
		break;

	case cmAliasTypeTransportAddress: 
		tAlias.m_AliasType = (u8)mtAliasTypeTransportAddress; 
		break;

	default: 
		break;	
	}
	
	if( tCmAlias.type == cmAliasTypeTransportAddress )
	{		
#ifdef WIN32
		tAlias.m_tTransportAddr.SetIpAddr( ntohl(tCmAlias.transport.ip) );
#else
		tAlias.m_tTransportAddr.SetIpAddr( ntohl(tCmAlias.transport.addr.v4.ip) );
#endif
		tAlias.m_tTransportAddr.SetPort( tCmAlias.transport.port );
	}

	if( tCmAlias.string )
	{
		strncpy(tAlias.m_achAlias, tCmAlias.string, min(sizeof(tAlias.m_achAlias), tCmAlias.length));
	}
	return;		
}

/*=============================================================================
  函 数 名： PszGetChannelName
  功    能： 获取信道名 
  算法实现： 
  全局变量： 
  参    数： u8 byChannelType
             s8  * pszBuf
             u8 byBufLen 
  返 回 值： s8  * 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
s8* CMtAdpUtils::PszGetChannelName( u8 byChannelType, s8* pszBuf, u8 byBufLen )
{
	if( NULL == pszBuf )
	{
		return NULL;
	}
	switch(byChannelType) 
	{
	//音频
	case MEDIA_TYPE_PCMA:	  strncpy( pszBuf, CHANNAME_G711A,    byBufLen );	break;	
	case MEDIA_TYPE_PCMU:	  strncpy( pszBuf, CHANNAME_G711U,    byBufLen );	break;
	case MEDIA_TYPE_G722:	  strncpy( pszBuf, CHANNAME_G722,     byBufLen );	break;
	case MEDIA_TYPE_G7231:	  strncpy( pszBuf, CHANNAME_G7231,    byBufLen );	break;
	case MEDIA_TYPE_G728:	  strncpy( pszBuf, CHANNAME_G728,     byBufLen );	break;
	case MEDIA_TYPE_G729:	  strncpy( pszBuf, CHANNAME_G729,     byBufLen );	break;
	case MEDIA_TYPE_MP3:	  strncpy( pszBuf, CHANNAME_MP3,      byBufLen );	break;
	case MEDIA_TYPE_G7221C:	  strncpy( pszBuf, CHANNAME_G7221C,   byBufLen );	break;
    case MEDIA_TYPE_AACLC:	  strncpy( pszBuf, CHANNAME_AACLC,    byBufLen );	break;
    case MEDIA_TYPE_AACLD:	  strncpy( pszBuf, CHANNAME_AACLD,    byBufLen );	break;
    case MEDIA_TYPE_G719:	  strncpy( pszBuf, CHANNAME_G719,	  byBufLen );	break;
	//视频
	case MEDIA_TYPE_H261:	  strncpy( pszBuf, CHANNAME_H261,     byBufLen );	break;
	case MEDIA_TYPE_H262:	  strncpy( pszBuf, CHANNAME_H262,     byBufLen );	break;
	case MEDIA_TYPE_H263:	  strncpy( pszBuf, CHANNAME_H263,     byBufLen );	break;
	case MEDIA_TYPE_H263PLUS: strncpy( pszBuf, CHANNAME_H263PLUS, byBufLen );	break;
	case MEDIA_TYPE_H264:	  strncpy( pszBuf, CHANNAME_H264,     byBufLen );	break;
	case MEDIA_TYPE_MP4:	  strncpy( pszBuf, CHANNAME_MPEG4,    byBufLen );	break;
	case MEDIA_TYPE_H239:	  strncpy( pszBuf, CHANNAME_H239,     byBufLen );	break;
	//数据
	case MEDIA_TYPE_H224:	  strncpy( pszBuf, CHANNAME_H224,     byBufLen );	break;
	case MEDIA_TYPE_T120:	  strncpy( pszBuf, CHANNAME_T120,     byBufLen );	break;
	case MEDIA_TYPE_MMCU:	  strncpy( pszBuf, CHANNAME_MMCU,     byBufLen );   break;
	
	default:                  strncpy( pszBuf, "UnknownType",     byBufLen );   break;
	}
	pszBuf[byBufLen - 1] = 0;
	
	return  pszBuf;
}


/*=============================================================================
  函 数 名： GetVendorId
  功    能： 厂商内部转换为外部结构
  算法实现： 
  全局变量： 
  参    数： u8 byVendorId
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
u8 CMtAdpUtils::GetVendorId(u8 byVendorId)
{	
	switch(byVendorId) 
	{
	case vendor_MICROSOFT_NETMEETING:   return	MT_MANU_NETMEETING;
	case vendor_KEDA:			return	MT_MANU_KDC;
	case vendor_HUAWEI:         return	MT_MANU_HUAWEI;
	case vendor_POLYCOM:        return	MT_MANU_POLYCOM;
	case vendor_POLYCOM_MCU:    return	MT_MANU_POLYCOM;  
	case vendor_TANDBERG:       return	MT_MANU_TAIDE;
	case vendor_RADVISION:      return	MT_MANU_RADVISION;
	case vendor_VCON:           return	MT_MANU_VCON;
	case vendor_SONY:           return	MT_MANU_SONY;
	case vendor_VTEL:           return	MT_MANU_VTEL;        
    case vendor_ZTE:            return  MT_MANU_ZTE;
    case vendor_ZTE_MCU:        return  MT_MANU_ZTEMCU;    
    case vendor_CODIAN:         return  MT_MANU_CODIAN;
	case vendor_AETHRA:			return  MT_MANU_AETHRA;
	case vendor_CHAORAN:        return  MT_MANU_CHAORAN;
	default:                    return	MT_MANU_OTHER;
	}
}

/*=============================================================================
  函 数 名： ResIn2Out
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byInRes
  返 回 值： emResolution 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang              创建
  2005/11/11    4.0			万春雷                增加相应分辨率
  2007/11/30    4.0R5		顾振华                增加相应分辨率
=============================================================================*/
s32 CMtAdpUtils::ResIn2Out(u8 byInRes)
{
	switch(byInRes)
	{
	case VIDEO_FORMAT_SQCIF:	return (s32)emSQCIF;
	case VIDEO_FORMAT_QCIF:		return (s32)emQCIF;
	case VIDEO_FORMAT_CIF:		return (s32)emCIF;
	case VIDEO_FORMAT_2CIF:		return (s32)emCIF2;
	case VIDEO_FORMAT_4CIF:		return (s32)emCIF4;
	case VIDEO_FORMAT_16CIF:	return (s32)emCIF16;
	case VIDEO_FORMAT_AUTO:		return (s32)emAUTO;
		
	case VIDEO_FORMAT_SIF:		return (s32)em352240;
	case VIDEO_FORMAT_4SIF:		return (s32)em704480;
	case VIDEO_FORMAT_VGA:		return (s32)em640480;
	case VIDEO_FORMAT_SVGA:		return (s32)em800600;
	case VIDEO_FORMAT_XGA:		return (s32)em1024768;

    case VIDEO_FORMAT_W4CIF:    return (s32)emW4CIF;
    case VIDEO_FORMAT_HD720:    return (s32)emHD720;
    case VIDEO_FORMAT_SXGA:     return (s32)emSXGA;
    case VIDEO_FORMAT_UXGA:     return (s32)emUXGA;
    case VIDEO_FORMAT_HD1080:  return (s32)emHD1080;

	default:					return (s32)emCIF;
	}
}

/*=============================================================================
  函 数 名： FrameRateIn2Out
  功    能： 帧率转换
  算法实现： 
  全局变量： 
  参    数： u8 byFrameRate
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/27    4.0			张宝卿                  创建
  2007/12/01    4.0R5       顾振华                  注意不支持帧率自定义
=============================================================================*/
u8 CMtAdpUtils::FrameRateIn2Out( u8 byFrameRate )
{
	switch( byFrameRate )
	{
	case VIDEO_FPS_2997_1:		return	1;						
	case VIDEO_FPS_25:			return	1;
	case VIDEO_FPS_2997_2:		return	2;
	case VIDEO_FPS_2997_3:		return	3;
	case VIDEO_FPS_2997_4:		return	4;
	case VIDEO_FPS_2997_5:		return	5;
	case VIDEO_FPS_2997_6:		return	6;
	case VIDEO_FPS_2997_7P5:    return  7;
    case VIDEO_FPS_2997_10:     return  10;
    case VIDEO_FPS_2997_15:     return  15;
	case VIDEO_FPS_2997_30:		return	30;
	default:					return  1;
	}
//    return 1;
}

u8 CMtAdpUtils::FrameRateOut2In( u8 byFrameRate )
{
	switch( byFrameRate )
	{
		case 1:		return	VIDEO_FPS_2997_1;		
		case 2:		return	VIDEO_FPS_2997_2;
		case 3:		return	VIDEO_FPS_2997_3;
		case 4:		return	VIDEO_FPS_2997_4;
		case 5:		return	VIDEO_FPS_2997_5;
		case 6:		return	VIDEO_FPS_2997_6;
		case 7:		return  VIDEO_FPS_2997_7P5;
		case 10:    return  VIDEO_FPS_2997_10;
		case 15:    return  VIDEO_FPS_2997_15;
		case 30:	return	VIDEO_FPS_2997_30;
		default:	return  byFrameRate;
	}
//    return 1;
}

/*=============================================================================
  函 数 名： ResOut2In
  功    能： 
  算法实现： 
  全局变量： 
  参    数： emResolution emRes
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang              创建
  2005/11/11    4.0			万春雷                增加相应分辨率
  2007/11/30    4.0R5		顾振华                增加相应分辨率
=============================================================================*/
u8 CMtAdpUtils::ResOut2In(s32 emRes)
{
	switch(emRes)
	{
	case emSQCIF:   return VIDEO_FORMAT_SQCIF;
	case emQCIF:    return VIDEO_FORMAT_QCIF;
	case emCIF:     return VIDEO_FORMAT_CIF;
	case emCIF2:    return VIDEO_FORMAT_2CIF;
	case emCIF4:    return VIDEO_FORMAT_4CIF;
	case emCIF16:   return VIDEO_FORMAT_16CIF;
	case emAUTO:    return VIDEO_FORMAT_AUTO;

	case em352240:  return VIDEO_FORMAT_SIF;
	case em704480:  return VIDEO_FORMAT_4SIF;
	case em640480:  return VIDEO_FORMAT_VGA;
	case em800600:  return VIDEO_FORMAT_SVGA;
	case em1024768: return VIDEO_FORMAT_XGA;

    case emW4CIF:   return VIDEO_FORMAT_W4CIF;
    case emHD720:   return VIDEO_FORMAT_HD720;
    case emSXGA:    return VIDEO_FORMAT_SXGA;
    case emUXGA:    return VIDEO_FORMAT_UXGA;
    case emHD1080:  return VIDEO_FORMAT_HD1080;

	default:        return VIDEO_FORMAT_CIF;
	}
}


/*=============================================================================
  函 数 名： TTERSTATUSVer36ToVer40
  功    能： 终端结构转换
  算法实现： 
  全局变量： 
  参    数：  _TTERSTATUS &tTerStatus
             _TTERSTATUS_VER36 &tTerStatus
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/04    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::TERSTATUSVer36ToVer40( _TTERSTATUS &tTerStatus, _TTERSTATUS_VER36 &tTerStatus36 )
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
*/

/*=============================================================================
  函 数 名： TTERSTATUSVer40ToVer36
  功    能： 终端结构转换
  算法实现： 
  全局变量： 
  参    数：  _TTERSTATUS_VER36 &tTerStatus
             _TTERSTATUS &tTerStatus
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/04    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::TERSTATUSVer40ToVer36( _TTERSTATUS_VER36 &tTerStatus36, _TTERSTATUS &tTerStatus )
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
*/
/*=============================================================================
  函 数 名： ConfInfoVer36ToVer40V4R6
  功    能： 会议信息结构由3.6转到4.6
  算法实现： 3.6->4.0->4.5->4.6
  全局变量： 
  参    数：TConfInfo36 &tConfInfo36: 3.6 会议信息结构
  返 回 值：TConfInfo   &tConfInfo:   4.0 会议信息结构
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/12/07    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfInfoV36ToV4R6( TConfInfo &tConfInfo, TConfInfoVer36 &tConfInfoVer36 )
{
    TConfInfoV4R4B2 tConfInfoV4R4B2;
    ConfInfoVer36ToVer40V4R4B2(tConfInfoV4R4B2, tConfInfoVer36);	//3.6->4.0

	ConfInfoV4R4ToV4R6(tConfInfo, tConfInfoV4R4B2);					//4.0->4.6

// 	TConfInfoV4R5	tConfInfoV4R5;
//     ConfInfoVer40V4R4B2ToVer40V4R5(tConfInfoV4R5, tConfInfoV4R4B2);
// 	
// 	ConfInfoV4R5ToV4R6(tConfInfo, tConfInfoV4R5); 
    return;
}
*/
/*=============================================================================
  函 数 名： ConfInfoVer40DailyToVer36
  功    能： 会议信息结构由4.0转到3.6
  算法实现： 
  全局变量： 
  参    数：TConfInfo36 &tConfInfo36: 3.6 会议信息结构
  返 回 值：TConfInfo   &tConfInfo:   4.0 会议信息结构
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/12/07    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfInfoV4R6ToV36( TConfInfoVer36 &tConfInfoVer36, TConfInfo &tConfInfo )
{
    TConfInfoV4R4B2 tConfInfoV4R4B2;
	ConfInfoV4R6ToV4R4(tConfInfoV4R4B2, tConfInfo); //4.6->4.0
    ConfInfoVer40V4R4B2ToVer36(tConfInfoVer36, tConfInfoV4R4B2);//4.0->3.6
    return;
}
*/
/*=============================================================================
  函 数 名： ConfInfoV4R6ToV4R4
  功    能： 会议信息结构由4.6转到4.0
  算法实现： 4.6->4.5->4.0
  全局变量： 
  参    数：TConfInfo36 &tConfInfo36: 4.6 会议信息结构
  返 回 值：TConfInfo   &tConfInfo:   4.0 会议信息结构
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/12/07    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfInfoV4R6ToV4R4( TConfInfoV4R4B2 &tConfInfoV4R4B2, TConfInfo &tConfInfo )
{
   
	TConfInfoV4R5	tConfInfoV4R5;
	ConfInfoV4R6ToV4R5(tConfInfoV4R5, tConfInfo);	//4.6->4.5
    ConfInfoVer40V4R5ToVer40V4R4B2(tConfInfoV4R4B2, tConfInfoV4R5);//4.5->4.0
    return;
}
*/
/*=============================================================================
  函 数 名： ConfInfoV4R6ToV4R4
  功    能： 会议信息结构由4.0转到4.6
  算法实现： 4.0->4.5->4.6
  全局变量： 
  参    数：TConfInfo36 &tConfInfo36: 4.6 会议信息结构
  返 回 值：TConfInfo   &tConfInfo:   4.0 会议信息结构
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/12/07    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfInfoV4R4ToV4R6( TConfInfo &tConfInfo, TConfInfoV4R4B2 &tConfInfoV4R4B2)
{
   
	TConfInfoV4R5	tConfInfoV4R5;
	ConfInfoVer40V4R4B2ToVer40V4R5(tConfInfoV4R5, tConfInfoV4R4B2);	//4.0->4.5
    ConfInfoV4R5ToV4R6(tConfInfo, tConfInfoV4R5);				//4.5->4.6
    return;
}
*/
/*=============================================================================
  函 数 名： ConfInfoVer36ToVer40V4R4B2
  功    能： 会议信息结构由3.6转到4.0V4R4B2
  算法实现： 
  全局变量： 
  参    数：TConfInfo36   &tConfInfo36: 3.6 会议信息结构
  返 回 值：TConfInfoV4R4B2 &tConfInfo: 4.0 会议信息结构
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/31    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfInfoVer36ToVer40V4R4B2( TConfInfoV4R4B2 &tConfInfo, TConfInfoVer36 &tConfInfo36 )
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
*/
/*=============================================================================
  函 数 名： ConfInfoVer40ToVer36
  功    能： 会议信息结构由4.0V4R4B2转到3.6
  算法实现： 
  全局变量： 
  参    数： TConfInfoV4R4B2 &tConfInfo : 4.0 会议信息结构
  返 回 值： TConfInfoVer36 &tConfInfo36: 3.6 会议信息结构 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/31    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfInfoVer40V4R4B2ToVer36( TConfInfoVer36 &tConfInfo36, TConfInfoV4R4B2 &tConfInfo )
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
*/
/*=============================================================================
  函 数 名： ConfInfoVer40V4R5ToVer40V4R4B2
  功    能： 4.5->4.0
  算法实现： 
  全局变量： 
  参    数： TConfInfo &tConfInfo :4.0 会议信息结构
  返 回 值： void
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/11/06    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfInfoVer40V4R5ToVer40V4R4B2( TConfInfoV4R4B2 &tConfInfoV4R4B2, TConfInfoV4R5 &tConfInfo )
{
    memcpy( &tConfInfoV4R4B2, &tConfInfo, sizeof(TConfInfoV4R4B2));

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
*/
/*=============================================================================
  函 数 名： ConfInfoVer40V4R4B2ToVer40V4R5
  功    能： 
  算法实现： 
  全局变量： 
  参    数： TConfInfo &tConfInfo :4.0 会议信息结构
  返 回 值： void
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/11/06    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfInfoVer40V4R4B2ToVer40V4R5( TConfInfoV4R5 &tConfInfo, TConfInfoV4R4B2 &tConfInfoV4R4B2 )
{
    memcpy( &tConfInfo, &tConfInfoV4R4B2, sizeof(TConfInfoV4R5));
    
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
*/
/*==============================================================================
函数名    :  VMPParamV4R6ToV4R5
功能      :  
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-2-24					
==============================================================================*/
/*void CMtAdpUtils::VMPParamV4R6ToV4R5(TVMPParamV4R5 &tVMPParamV4R5,TVMPParam &tVMPParam)
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
*/
/*==============================================================================
函数名    :  VMPParamV4R5ToV4R6
功能      :  
算法实现  :  
参数说明  :  
返回值说明:  
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2009-2-24					
==============================================================================*/
/*void CMtAdpUtils::VMPParamV4R5ToV4R6(TVMPParam &tVMPParam, TVMPParamV4R5 &tVMPParamV4R5)
{
	tVMPParam.SetVMPAuto( tVMPParamV4R5.IsVMPAuto() );
	tVMPParam.SetVMPBrdst( tVMPParamV4R5.IsVMPBrdst() );
	tVMPParam.SetVMPStyle( tVMPParamV4R5.GetVMPStyle() );
	tVMPParam.SetVMPSchemeId( tVMPParamV4R5.GetMaxMemberNum() );
	tVMPParam.SetVMPMode( tVMPParamV4R5.GetVMPMode() );
	tVMPParam.SetIsRimEnabled( tVMPParamV4R5.GetIsRimEnabled() );
	tVMPParam.SetVMPBatchPoll( 0 );
	for( s32 nIndex = 0; nIndex < MAXNUM_VMP_MEMBER; nIndex ++ )
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
	for( u8 byIdx = MAXNUM_VMP_MEMBER; byIdx < MAXNUM_MPUSVMP_MEMBER; byIdx ++)
	{
		TVMPMember tVmpMember;
		memset( &tVmpMember, 0, sizeof(tVmpMember) );
		tVMPParam.SetVmpMember( byIdx, tVmpMember );
	}
	return;
}
*/
/*=============================================================================
函 数 名： ConfStatusV4R5ToV4R6
功    能： 会议状态V4.5 转到 之后
算法实现： 
全局变量： 
参    数： TConfStatus &tConfStatus: 4.5会议状态结构
返 回 值： void
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容

=============================================================================*/
/*void CMtAdpUtils::ConfStatusV4R5ToV4R6( TConfStatus &tConfStatus, TConfStatusV4R5 &tConfStatusV4R5 )
{
    memcpy(&tConfStatus, &tConfStatusV4R5, sizeof(tConfStatus));

	memcpy(&tConfStatus.m_tMixParam, &tConfStatusV4R5.m_tMixParam, sizeof(TMixParam));
	
	//调整TVMPParam
	TVMPParamV4R5 tVMPParamV4R5, tVMPTwParamV4R5;
	tVMPParamV4R5	= tConfStatusV4R5.GetVmpParam();
	tVMPTwParamV4R5 = tConfStatusV4R5.GetVmpTwParam();
	
	TVMPParam	tVMPParam, tVMPTwParam;
	VMPParamV4R5ToV4R6(tVMPParam, tVMPParamV4R5);
	VMPParamV4R5ToV4R6(tVMPTwParam, tVMPTwParamV4R5);
	
	tConfStatus.SetVmpParam(tVMPParam);
	tConfStatus.SetVmpTwParam(tVMPTwParam);

	//补上prs
	tConfStatus.SetPrsing(tConfStatusV4R5.IsPrsing());

    return;
}
*/
/*=============================================================================
函 数 名： ConfStatusV4R6ToV4R5
功    能： 会议状态V4.6->V4.5, 修改时注意结构中子结构的区别
算法实现： 
全局变量： 
参    数： TConfStatus &tConfStatus: 4.5会议状态结构
返 回 值： void
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容

=============================================================================*/
/*void CMtAdpUtils::ConfStatusV4R6ToV4R5( TConfStatusV4R5 &tConfStatusV4R5, TConfStatus &tConfStatus )
{
    memcpy( &tConfStatusV4R5,&tConfStatus, sizeof(tConfStatusV4R5));
	memcpy(&tConfStatusV4R5.m_tMixParam, &tConfStatus.m_tMixParam, sizeof(TMixParam));
	
	//调整TVMPParam (16个最大成员)
	TVMPParam	tVMPParam, tVMPTwParam;
	tVMPParam	= tConfStatus.GetVmpParam();
	tVMPTwParam = tConfStatus.GetVmpTwParam();
	
	TVMPParamV4R5 tVMPParamV4R5, tVMPTwParamV4R5;
	VMPParamV4R6ToV4R5( tVMPParamV4R5, tVMPParam);
	VMPParamV4R6ToV4R5( tVMPTwParamV4R5, tVMPTwParam );
	
	tConfStatusV4R5.SetVmpParam(tVMPParamV4R5);
	tConfStatusV4R5.SetVmpTwParam(tVMPTwParamV4R5);

	//补上prs
	tConfStatusV4R5.SetPrsing(tConfStatus.IsPrsing());

    return;
}
*/
/*=============================================================================
函 数 名： ConfInfoV4R5ToV4R6
功    能： 
算法实现： 
全局变量： 
参    数： TConfInfo &tConfInfo :4.6 会议信息结构
返 回 值： void
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容

=============================================================================*/
/*void CMtAdpUtils::ConfInfoV4R5ToV4R6( TConfInfo &tConfInfo, TConfInfoV4R5 &tConfInfoV4R5 )
{
    memcpy( &tConfInfo, &tConfInfoV4R5, sizeof(TConfInfo));
    
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
*/
/*=============================================================================
函 数 名： ConfInfoV4R6ToV4R5
功    能： ConfInfo 结构转换 4.6 -> 4.5 (区别在于4.6画面合成成员最大个数为20)
算法实现： 
全局变量： 
参    数： TConfInfo &tConfInfo :4.6 会议信息结构
返 回 值： void
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容

=============================================================================*/
/*void CMtAdpUtils::ConfInfoV4R6ToV4R5( TConfInfoV4R5 &tConfInfoV4R5, TConfInfo &tConfInfo )
{
    memcpy( &tConfInfoV4R5, &tConfInfo, sizeof(tConfInfoV4R5));
    
    //会议状态
	TConfStatus tConfStatus;
    memset( &tConfStatus, 0, sizeof(tConfStatus) ); 
	tConfStatus = tConfInfo.GetStatus();

    TConfStatusV4R5 tConfStatusV4R5;
    memset( &tConfStatusV4R5, 0, sizeof(tConfStatusV4R5) );
    ConfStatusV4R6ToV4R5( tConfStatusV4R5, tConfStatus );
	
    tConfInfoV4R5.SetStatus( tConfStatusV4R5 );
    return;
}
*/
/*=============================================================================
  函 数 名： CapSupportVer36ToVer40
  功    能： 能力集结构由3.6转到4.0
  算法实现： 
  全局变量： 
  参    数： TCapSupport36 &tCapSupport36: 3.6 能力集结构
  返 回 值： TCapSupport: 4.0能力集结构 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/31    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::CapSupportVer36ToVer40( TCapSupport &tCapSupport, TCapSupportVer36 &tCapSupportVer36 )
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

*/
/*=============================================================================
  函 数 名： CapSupportVer40ToVer36
  功    能： 能力集结构由4.0转到3.6
  算法实现： 
  全局变量： 
  参    数： TCapSupport &tCapSupport: 4.0 能力集结构
  返 回 值： TCapSupport36: 3.6能力集结构 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/31    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::CapSupportVer40ToVer36( TCapSupportVer36 &tCapSupportVer36, TCapSupport &tCapSupport )
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
*/
/*=============================================================================
  函 数 名： ConfAttribVer36ToVer40
  功    能： 会议属性由3.6向4.0转换
  算法实现： 
  全局变量： 
  参    数： TConfAttrb36 &tConfAttrb36: 3.6的会议属性
  返 回 值： TConfAttrb: 4.0的会议属性 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/31    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfAttribVer36ToVer40( TConfAttrb &tConfAttrb, TConfAttrbVer36 &tConfAttrbVer36 )
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
*/
/*=============================================================================
  函 数 名： ConfAttribVer40ToVer36
  功    能： 会议属性由4.0向3.6的转换
  算法实现： 
  全局变量： 
  参    数： TConfAttrb &tConfAttrb: 4.0会议属性
  返 回 值： TConfAttrb36: 3.6会议属性 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/31    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfAttribVer40ToVer36( TConfAttrbVer36 &tConfAttrbVer36, TConfAttrb &tConfAttrb )
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
*/
/*=============================================================================
  函 数 名： MtAliasVer36ToVer40
  功    能： 终端别名3.6向4.0的转换
  算法实现： 
  全局变量： 
  参    数： TMtAlias36 &tMtAlias36: 3.6终端别名
  返 回 值： TMtAlias: 4.0终端别名 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::MtAliasVer36ToVer40( TMtAlias &tMtAlias, TMtAliasVer36 &tMtAliasVer36 )
{
	strncpy( tMtAlias.m_achAlias, tMtAliasVer36.m_achAlias, sizeof(tMtAliasVer36.m_achAlias) );
	tMtAlias.m_AliasType				 = tMtAliasVer36.m_AliasType;
	tMtAlias.m_tTransportAddr.m_dwIpAddr = tMtAliasVer36.m_tTransportAddr.m_dwIpAddr;
	tMtAlias.m_tTransportAddr.m_wPort    = tMtAliasVer36.m_tTransportAddr.m_wPort;

	return;
}
*/
/*=============================================================================
  函 数 名： MtAliasVer40ToVer36
  功    能： 终端别名4.0向3.6的转换
  算法实现： 
  全局变量： 
  参    数： TMtAlias &tMtAlias　　:4.0终端别名
  返 回 值： TMtAlias36: 3.6终端别名 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::MtAliasVer40ToVer36( TMtAliasVer36 &tMtAliasVer36, TMtAlias &tMtAlias )
{
	strncpy( tMtAliasVer36.m_achAlias, tMtAlias.m_achAlias, sizeof(tMtAlias.m_achAlias) );
	tMtAliasVer36.m_AliasType				  = tMtAlias.m_AliasType;
	tMtAliasVer36.m_tTransportAddr.m_dwIpAddr = tMtAlias.m_tTransportAddr.m_dwIpAddr;
	tMtAliasVer36.m_tTransportAddr.m_wPort    = tMtAlias.m_tTransportAddr.m_wPort;

	return;
}
*/
/*=============================================================================
  函 数 名： MediaEncryptVer36ToVer40
  功    能： 媒体类型密钥3.6转向4.0
  算法实现： 
  全局变量： 
  参    数：TMediaEncrypt36 &tEncryptKey36: 3.6媒体类型密钥
  返 回 值：TMediaEncrypt: 4.0媒体类型密钥 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/31    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::MediaEncryptVer36ToVer40( TMediaEncrypt &tMediaKey, TMediaEncryptVer36 &tEncryptKeyVer36 )
{
	s32 nKeyLen = 0;
	u8  abyKey[MAXLEN_KEY] = {0};
	tEncryptKeyVer36.GetEncryptKey( abyKey, &nKeyLen );
	tMediaKey.SetEncryptKey( abyKey, nKeyLen );
	tMediaKey.SetEncryptMode( tEncryptKeyVer36.GetEncryptMode() );

	return;
}
*/
/*=============================================================================
  函 数 名： MediaEncryptVer40ToVer36
  功    能： 媒体类型密钥4.0转向3.6
  算法实现： 
  全局变量： 
  参    数： TMediaEncrypt &tEncryptKey: 4.0媒体类型密钥
  返 回 值： TMediaEncrypt36: 3.6媒体类型密钥  
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/31    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::MediaEncryptVer40ToVer36( TMediaEncryptVer36 &tEncryptKeyVer36, TMediaEncrypt &tEncryptKey )
{
	s32 nKeyLen = 0;
	u8  abyKey[MAXLEN_KEY] = {0};
	tEncryptKey.GetEncryptKey( abyKey, &nKeyLen );	
	tEncryptKeyVer36.SetEncryptKey( abyKey, nKeyLen );
	tEncryptKeyVer36.SetEncryptMode( tEncryptKey.GetEncryptMode() );
	return;
}
*/
/*=============================================================================
  函 数 名： ConfStatusVer36ToVer40V4R4B2
  功    能： 会议状态3.6转到4.0
  算法实现： 
  全局变量： 
  参    数： TConfStatus36 &tConfStatus36: 3.6会议状态结构
  返 回 值： TConfStatus: 4.0会议状态结构
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfStatusVer36ToVer40V4R4B2( TConfStatusV4R4B2 &tConfStatus, TConfStatusVer36 &tConfStatusVer36 )
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
*/
/*=============================================================================
  函 数 名： ConfStatusVer40V4R4B2ToVer36
  功    能： 会议状态4.0转到3.6
  算法实现： 
  全局变量： 
  参    数： TConfStatus &tConfStatus: 4.0会议状态结构
  返 回 值： TConfStatus36: 3.6会议状态结构
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfStatusVer40V4R4B2ToVer36( TConfStatusVer36 &tConfStatusVer36, TConfStatusV4R4B2 &tConfStatus )
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
*/
/*=============================================================================
  函 数 名： ConfStatusVer40V4R4B2ToVer40Daily
  功    能： 会议状态4.0V4R4B2 转到 之后
  算法实现： 
  全局变量： 
  参    数： TConfStatus &tConfStatus: 4.0会议状态结构
  返 回 值： void
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/11/06    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfStatusVer40V4R4B2ToVer40V4R5( TConfStatusV4R5 &tConfStatus, TConfStatusV4R4B2 &tConfStatusV4R4B2 )
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
*/
/*=============================================================================
  函 数 名： ConfStatusVer40V4R4B2ToVer36
  功    能： 会议状态 v4.5->4.0
  算法实现： 
  全局变量： 
  参    数： TConfStatus &tConfStatus: 4.0会议状态结构
  返 回 值： void
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/11/06    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfStatusV4R5ToV4R4B2( TConfStatusV4R4B2 &tConfStatusV4R4B2, TConfStatusV4R5 &tConfStatus )
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
*/
/*=============================================================================
  函 数 名： ConfIdVer36ToVer40
  功    能： 
  算法实现： 
  全局变量： 
  参    数：  CConfId &cConfId
             CConfIdVer36 &cConfIdVer36
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/1/6    4.0			张宝卿                  创建
=============================================================================*/
/* void CMtAdpUtils::ConfIdVer36ToVer40( CConfId &cConfId, CConfIdVer36 &cConfIdVer36 )
{
	u8 abyConfId[MAXLEN_CONFID];
	u8 byLen = 0;
	cConfIdVer36.GetConfId( abyConfId, sizeof(abyConfId) );
	cConfId.SetConfId( abyConfId, sizeof(abyConfId) );
} */

/*=============================================================================
  函 数 名： ConfIdVer40ToVer36
  功    能： 
  算法实现： 
  全局变量： 
  参    数：  CConfIdVer36 &cConfIdVer36
             CConfId &cConfId
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/1/6    4.0			张宝卿                  创建
=============================================================================*/
/* void CMtAdpUtils::ConfIdVer40ToVer36( CConfIdVer36 &cConfIdVer36, CConfId &cConfId )
{
	u8 abyConfId[MAXLEN_CONFID];
	u8 byLen = 0;
	cConfId.GetConfId( abyConfId, sizeof(abyConfId) );
	cConfIdVer36.SetConfId( abyConfId, sizeof(abyConfId) );
}
*/
/*=============================================================================
  函 数 名： KdvTimeVer36ToVer40
  功    能： 
  算法实现： 
  全局变量： 
  参    数：  TKdvTime &tKdvTime
             TKdvTimeVer36 &tKdvTimeVer36
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/1/6    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::KdvTimeVer36ToVer40( TKdvTime &tKdvTime, TKdvTimeVer36 &tKdvTimeVer36 )
{
	tKdvTime.SetDay( tKdvTimeVer36.GetDay() );
	tKdvTime.SetHour( tKdvTimeVer36.GetHour() );
	tKdvTime.SetMinute( tKdvTimeVer36.GetMinute() );
	tKdvTime.SetMonth( tKdvTimeVer36.GetMonth() );
	tKdvTime.SetSecond( tKdvTimeVer36.GetSecond() );
	tKdvTime.SetYear( tKdvTimeVer36.GetYear() );
}
*/
/*=============================================================================
  函 数 名： KdvTimeVer40ToVer36
  功    能： 
  算法实现： 
  全局变量： 
  参    数：  TKdvTimeVer36 &tKdvTimeVer36
             TKdvTime &tKdvTime
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/1/6    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::KdvTimeVer40ToVer36( TKdvTimeVer36 &tKdvTimeVer36, TKdvTime &tKdvTime )
{
	tKdvTimeVer36.SetDay( tKdvTime.GetDay() );
	tKdvTimeVer36.SetHour( tKdvTime.GetHour() );
	tKdvTimeVer36.SetMinute( tKdvTime.GetMinute() );
	tKdvTimeVer36.SetMonth( tKdvTime.GetMonth() );
	tKdvTimeVer36.SetSecond( tKdvTime.GetSecond() );
	tKdvTimeVer36.SetYear( tKdvTime.GetYear() );
}
*/
/*=============================================================================
  函 数 名： MtVer36ToVer40
  功    能： 终端结构3.6到4.0
  算法实现： 
  全局变量： 
  参    数： TMt36 &tMt36 : 3.6终端结构
  返 回 值： TMt: 4.0终端结构
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::MtVer36ToVer40( TMt &tMt, TMtVer36 &tMtVer36 )
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
*/
/*=============================================================================
  函 数 名： MtVer40ToVer36
  功    能： 终端结构4.0到3.6
  算法实现： 
  全局变量： 
  参    数： TMt &tMt :4.0终端结构
  返 回 值： TMt36: 3.6终端结构
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::MtVer40ToVer36( TMtVer36 &tMtVer36, TMt &tMt )
{
	tMtVer36.SetType( tMt.GetType() );

	if ( TYPE_MCU == tMt.GetType() )
	{
		tMtVer36.SetMcu( tMt.GetMcuId() );
		tMtVer36.SetMcuId( tMt.GetMcuId() );
		tMtVer36.SetMcuType( tMt.GetMcuType() );
	}
	if ( TYPE_MCUPERI == tMt.GetType() )
	{
		tMtVer36.SetMcuEqp( tMt.GetMcuId(), tMt.GetEqpId(), tMt.GetEqpType() );
		tMtVer36.SetEqpId( tMt.GetEqpId() );
		tMtVer36.SetEqpType( tMt.GetEqpType() );
	}
	if ( TYPE_MT == tMt.GetType() )
	{
		tMtVer36.SetMt( tMt.GetMcuId(), tMt.GetMtId(), tMt.GetDriId(), tMt.GetConfIdx() );
		tMtVer36.SetMtId( tMt.GetMtId() );
		tMtVer36.SetMtType( tMt.GetMtType() );
	}
	tMtVer36.SetDriId( tMt.GetDriId() );
	tMtVer36.SetConfIdx( tMt.GetConfIdx() );
	
	return;
}
*/

/*=============================================================================
  函 数 名： ConfModeVer36ToVer40V4R4B2
  功    能： 会议模式3.6到4.0V4R4B2
  算法实现： 
  全局变量： 
  参    数： TConfMode36 &tConfMode36: 3.6会议模式
  返 回 值： TConfMode: 4.0会议模式
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfModeVer36ToVer40V4R4B2( TConfModeV4R4B2 &tConfMode, TConfModeVer36 &tConfModeVer36 )
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
*/
/*=============================================================================
  函 数 名： ConfModeVer40ToVer36
  功    能： 会议模式4.0V4R4B2到3.6
  算法实现： 
  全局变量： 
  参    数： TConfMode &tConfMode:4.0会议模式
  返 回 值： TConfMode36 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfModeVer40V4R4B2ToVer36( TConfModeVer36 &tConfModeVer36, TConfModeV4R4B2 &tConfMode )
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
*/
/*=============================================================================
  函 数 名： ConfModeVer40R4V4B2ToVer40Daily
  功    能： 会议模式4.0V4R4B2到4.0每日
  算法实现： 
  全局变量： 
  参    数： TConfMode &tConfMode:4.0会议模式
  返 回 值： TConfMode36 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/11/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfModeVer40V4R4B2ToVer40Daily( TConfMode &tConfMode, TConfModeV4R4B2 &tConfModeR4V4B2 )
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
*/
/*=============================================================================
  函 数 名： ConfModeVer40DailyToVer40R4V4B2
  功    能： 会议模式4.0每日到4.0V4R4B2
  算法实现： 
  全局变量： 
  参    数： TConfMode &tConfMode:4.0会议模式
  返 回 值： TConfMode36 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/11/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::ConfModeVer40DailyToVer40V4R4B2( TConfModeV4R4B2 &tConfModeR4V4B2, TConfMode &tConfMode )
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
*/
/*=============================================================================
  函 数 名： RecProgVer36ToVer40
  功    能： 录放象进度3.6到4.0
  算法实现： 
  全局变量： 
  参    数： TRecProg36 &tRecProg36: 3.6的录放象进度
  返 回 值： TRecProg: 4.0的录放象进度
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::RecProgVer36ToVer40( TRecProg &tRecProg, TRecProgVer36 &tRecProgVer36 )
{
	tRecProg.SetCurProg( tRecProgVer36.GetCurProg() );
	tRecProg.SetTotalTime( tRecProgVer36.GetTotalTime() );
	return;
}
*/
/*=============================================================================
  函 数 名： RecProgVer40ToVer36
  功    能： 录放象进度4.0到3.6
  算法实现： 
  全局变量： 
  参    数： TRecProg &tRegProc: 4.0的录放象进度
  返 回 值： TRecProg36: 3.6的录放象进度
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::RecProgVer40ToVer36( TRecProgVer36 &tRecProgVer36, TRecProg &tRecProg )
{
	tRecProgVer36.SetCurProg( tRecProg.GetCurProg() );
	tRecProgVer36.SetTotalTime( tRecProg.GetTotalTime() );
	return;
}
*/
/*=============================================================================
  函 数 名： PollInfoVer36ToVer40
  功    能： 轮询信息3.6到4.0
  算法实现： 
  全局变量： 
  参    数： TPollInfo36 &tPollInfo36: 3.6轮询信息
  返 回 值： TPollInfo: 4.0 轮询信息
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::PollInfoVer36ToVer40( TPollInfo &tPollInfo, TPollInfoVer36 &tPollInfoVer36 )
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
*/
/*=============================================================================
  函 数 名： PollInfoVer40ToVer36
  功    能： 轮询信息4.0到3.6
  算法实现： 
  全局变量： 
  参    数： TPollInfo &tPollInfo: 4.0轮询信息
  返 回 值： TPollInfo36: 3.6轮询信息
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::PollInfoVer40ToVer36( TPollInfoVer36 &tPollInfoVer36, TPollInfo &tPollInfo )
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
*/
/*=============================================================================
  函 数 名： MtPollParamVer36ToVer40
  功    能： 终端轮询参数结构3.6到4.0
  算法实现： 
  全局变量： 
  参    数： TMtPollParam36 &tMtPollParam36: 3.6终端轮询参数结构
  返 回 值： TMtPollParam: 4.0 终端轮询参数结构
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::MtPollParamVer36ToVer40( TMtPollParam &tMtPollParam, TMtPollParamVer36 &tMtPollParamVer36 )
{
	//3.6不指定轮询次数, 4.0默认填0, 表示无限轮询
	tMtPollParam.SetPollNum( 0 );
	tMtPollParam.SetKeepTime( tMtPollParamVer36.GetKeepTime() );

	return;
}
*/
/*=============================================================================
  函 数 名： MtPollParamVer40ToVer36
  功    能： 终端轮询参数结构4.0到3.6
  算法实现： 
  全局变量： 
  参    数： TMtPollParam &tMtPollParam: 4.0终端轮询参数结构
  返 回 值： TMtPollParam36: 3.6终端轮询参数结构
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::MtPollParamVer40ToVer36( TMtPollParamVer36 &tMtPollParamVer36, TMtPollParam &tMtPollParam )
{
	tMtPollParamVer36.SetKeepTime( tMtPollParam.GetKeepTime() );
	return;
}
*/
/*=============================================================================
  函 数 名： DiscussParamVer36ToVer40V4R4B2
  功    能： 讨论参数结构3.6到4.0V4R4B2
  算法实现： 
  全局变量： 
  参    数： TDiscussParam36 &tDiscussParam36: 3.6讨论参数结构
  返 回 值： TDiscussParam: 4.0讨论参数结构
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::DiscussParamVer36ToVer40V4R4B2( TDiscussParamV4R4B2 &tDiscussParam, TDiscussParamVer36 &tDiscussParamVer36 )
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
*/
/*=============================================================================
  函 数 名： DiscussParamVer40DailyToVer36R4V4B2
  功    能： 讨论参数结构4.0每日到4.0V4R4B2
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/11/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::DiscussParamVer40DailyToVer40V4R4B2( TDiscussParamV4R4B2 &tDiscussParam, TMixParamV4R5 &tMixParam )
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
*/
/*=============================================================================
  函 数 名： DiscussParamVer40V4R4B2ToVer40Daily
  功    能： 
  算法实现： 
  全局变量： 
  参    数： 
  返 回 值： void
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/11/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::DiscussParamVer40V4R4B2ToVer40Daily( TMixParamV4R5 &tMixParam, TDiscussParamV4R4B2 &tDiscussParam )
{
    tMixParam.SetMode(tDiscussParam.m_byMixMode);
    return;
}
*/
/*=============================================================================
  函 数 名： DiscussParamVer40ToVer36
  功    能： 讨论参数结构4.0到3.6
  算法实现： 
  全局变量： 
  参    数： TDiscussParam &tDiscussParam: 4.0讨论参数结构
  返 回 值： TDiscussParam36: 3.6讨论参数结构 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::DiscussParamVer40V4R4B2ToVer36(TDiscussParamVer36 &tDiscussParamVer36, TDiscussParamV4R4B2 &tDiscussParam )
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
*/
/*=============================================================================
  函 数 名： VMPParamVer36ToVer40
  功    能： 画面合成参数结构3.6到4.0
  算法实现： 
  全局变量： 
  参    数： TVMPParam36 &tVMPParam36: 3.6画面合成参数结构
			 TVMPMember36 &tVMPParam36:3.6画面合成成员结构
  返 回 值： TVMPParam: 4.0画面合成参数结构
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::VMPParamVer36ToVer40( TVMPParamV4R4B2 &tVMPParam, TVMPParamVer36 &tVMPParamVer36 )
{
	tVMPParam.SetVMPAuto( tVMPParamVer36.IsVMPAuto() );
	tVMPParam.SetVMPBrdst( tVMPParamVer36.IsVMPBrdst() );
	tVMPParam.SetVMPStyle( tVMPParamVer36.GetVMPStyle() );
	tVMPParam.SetVMPSchemeId( tVMPParamVer36.GetMaxMemberNum() );

	for( s32 nIndex = 0; nIndex < MAXNUM_VMP_MEMBER; nIndex ++ )
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
*/
/*=============================================================================
  函 数 名： VMPParamVer40ToVer36
  功    能： 画面合成参数结构4.0到3.6
  算法实现： 
  全局变量： 
  参    数： TVMPParam &tVMPParam: 4.0画面合成参数结构
			 TVMPMember &tVMPParam:4.06画面合成成员结构
  返 回 值： TVMPParam36: 3.6画面合成参数结构 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::VMPParamVer40ToVer36( TVMPParamVer36 &tVMPParamVer36, TVMPParamV4R4B2 &tVMPParam )
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
*/
/*=============================================================================
  函 数 名： VMPMemberVer36ToVer40
  功    能： 
  算法实现： 
  全局变量： 
  参    数： TVMPMember36 &tVMPMember36
  返 回 值： TVMPMember 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::VMPMemberVer36ToVer40( TVMPMember &tVMPMember, TVMPMemberVer36 &tVMPMemberVer36 )
{
	//3.6中不指定画面合成成员状态, 4.0中默认为观众
	tVMPMember.SetMemStatus( MT_STATUS_AUDIENCE ); 	
	tVMPMember.SetMemberType( tVMPMemberVer36.GetMemberType() );
	return;
}
*/
/*=============================================================================
  函 数 名： VMPMemberVer40ToVer36
  功    能： 
  算法实现： 
  全局变量： 
  参    数： TVMPMember &tVMPMember
  返 回 值： TVMPMember36 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/02    4.0			张宝卿                  创建
=============================================================================*/
/*void CMtAdpUtils::VMPMemberVer40ToVer36( TVMPMemberVer36 &tVMPMemberVer36, TVMPMember &tVMPMember )
{
	tVMPMemberVer36.SetMemberType( tVMPMember.GetMemberType() );
	return;
}
*/

/*=============================================================================
  函 数 名： MediaTypeVer36ToVer40
  功    能： 媒体类型适配
  算法实现： 
  全局变量： 
  参    数： u8 byMediaType36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::MediaTypeVer36ToVer40( u8 byMediaType36 )
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
*/
/*=============================================================================
  函 数 名： MediaTypeVer40ToVer36
  功    能： 媒体类型适配
  算法实现： 
  全局变量： 
  参    数： u8 byMediaType40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::MediaTypeVer40ToVer36( u8 byMediaType40 )
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
*/
/*=============================================================================
  函 数 名： VideoFormatVer36ToVer40
  功    能： 视频分辨率适配
  算法实现： 
  全局变量： 
  参    数： u8 byVideoFormat36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::VideoFormatVer36ToVer40( u8 byVideoFormat36 )
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
*/
/*=============================================================================
  函 数 名： VideoFormatVer40ToVer36
  功    能： 视频分辨率适配
  算法实现： 4.0新增分辨率默认转为3.6的CIF分辨率
  全局变量： 
  参    数： u8 byDStreamType40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::VideoFormatVer40ToVer36( u8 byVideoFormat40 )
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
*/
/*=============================================================================
  函 数 名： ConfCallModeVer36ToVer40
  功    能： 呼叫终端方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfCallMode36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfCallModeVer36ToVer40( u8 byConfCallMode36 )
{
	switch( byConfCallMode36 )
	{
	case CONF_CALLMODE_NONE_VER36:		return CONF_CALLMODE_NONE;
	case CONF_CALLMODE_ONCE_VER36:		return CONF_CALLMODE_TIMER;
	case CONF_CALLMODE_TIMER_VER36:		return CONF_CALLMODE_TIMER;
	default:							return CONF_CALLMODE_TIMER;
	}
}
*/
/*=============================================================================
  函 数 名： ConfCallModeVer40ToVer36
  功    能： 呼叫终端方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfCallMode40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfCallModeVer40ToVer36( u8 byConfCallMode40 )
{
	switch( byConfCallMode40 )
	{
	case CONF_CALLMODE_NONE:		return CONF_CALLMODE_NONE_VER36;
	case CONF_CALLMODE_TIMER:		return CONF_CALLMODE_TIMER_VER36;
	default:						return CONF_CALLMODE_TIMER_VER36;
	}
}
*/
/*=============================================================================
  函 数 名： ConfBasModeVer36ToVer40
  功    能： 码率适配方式定义
  算法实现： 
  全局变量： 
  参    数： u8 byConfBasMode36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfBasModeVer36ToVer40( u8 byConfBasMode36 )
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
*/
/*=============================================================================
  函 数 名： ConfBasModeVer40ToVer36
  功    能： 码率适配方式定义
  算法实现： 
  全局变量： 
  参    数： u8 byConfBasMode40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfBasModeVer40ToVer36( u8 byConfBasMode40 )
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
*/
/*=============================================================================
  函 数 名： GetGKErrCode
  功    能： 将GK的错误码转成MCU的错误码定义
  算法实现： 
  全局变量： 
  参    数： u16 wErrCode
  返 回 值： u16 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
u16 CMtAdpUtils::GetGKErrCode( u16 wErrCode )
{
    u16 wErrRet = 0;
    switch( wErrCode ) 
    {
    case RADIUS_INTER_ACCT_NOT_EXIST:   wErrRet = (u16)ERR_MCU_GK_USRNAME;   break;
    case RADIUS_INTER_INVALID_PWD:      wErrRet = (u16)ERR_MCU_GK_USRPWD;    break;
    case RADIUS_INTER_CONF_NOT_EXIST:   wErrRet = (u16)ERR_MCU_GK_CONF;      break;
    case RADIUS_INTER_DB_FAIL:          wErrRet = (u16)ERR_MCU_GK_DB;        break;
    case RADIUS_INTER_TIME_OUT:         wErrRet = (u16)ERR_MCU_GK_STARTCHARGE_TIMEOUT;   break;
    case RADIUS_INTER_GK_NOT_SUPPORT_ACCT:  wErrRet = (u16)ERR_MCU_GK_WITHOUT_RADIUS;    break;
    case RADIUS_INTER_GK_OP_FAILED:     wErrRet = (u16)ERR_MCU_GK_OP_RADIUS_FAILED;      break;
    case RADIUS_INTER_ACCT_IN_USE:      wErrRet = (u16)ERR_MCU_GK_ACCT_IN_USE;   break;
    case RADIUS_INTER__OTHER_ERRPR:     wErrRet = (u16)ERR_MCU_GK_UNKNOW;        break;
    case RADIUS_INTER_SESSION_ID_NULL:  wErrRet = (u16)ERR_MCU_GK_SSNID_NULL;    break;
    case RADIUS_INTER_GK_INSTANCE_FULL: wErrRet = (u16)ERR_MCU_GK_INST_FULL;     break;
    default:                            wErrRet = (u16)ERR_MCU_GK_UNKNOW;        break;
    }
    return wErrRet;
}

/*=============================================================================
  函 数 名： DisconnectReasonStack2MtAdp
  功    能： 将呼叫终端失败的错误码转成 Mtadp 对应错误码定义
  算法实现： 
  全局变量： 
  参    数： CallDisconnectReason emReason
  返 回 值： emDisconnectReason 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/05/15    4.0			张宝卿                  创建
=============================================================================*/
emDisconnectReason CMtAdpUtils::DisconnectReasonStack2MtAdp( CallDisconnectReason emReason )
{
    emDisconnectReason emReasonMtAdp;
    switch( emReason )
    {
    case reason_busy:       emReasonMtAdp = emDisconnectReasonBusy;     break;
    case reason_normal:     emReasonMtAdp = emDisconnectReasonNormal;   break;
    case reason_rejected:   emReasonMtAdp = emDisconnectReasonRejected; break;
    case reason_unreachable:emReasonMtAdp = emDisconnectReasonUnreachable;  break;
    case reason_local:      emReasonMtAdp = emDisconnectReasonLocal;    break;
    case reason_unknown:    emReasonMtAdp = emDisconnectReasonUnknown;  break;
    default:                emReasonMtAdp = emDisconnectReasonUnknown;  break;
    }
    return emReasonMtAdp;
}

/*=============================================================================
  函 数 名： DisconnectReasonMtAdp2Stack
  功    能： 将呼叫终端失败的错误码转成 协议栈 对应错误码定义
  算法实现： 
  全局变量： 
  参    数： emDisconnectReason emReason
  返 回 值： CallDisconnectReason 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/05/16    4.0			张宝卿                  创建
=============================================================================*/
CallDisconnectReason CMtAdpUtils::DisconnectReasonMtAdp2Stack( emDisconnectReason emReason )
{
    CallDisconnectReason emReasonStack;
    switch( emReason )
    {
    case emDisconnectReasonBusy:            emReasonStack = reason_busy;        break;
    case emDisconnectReasonNormal:          emReasonStack = reason_normal;      break;
    case emDisconnectReasonRejected:        emReasonStack = reason_rejected;    break;
    case emDisconnectReasonUnreachable:     emReasonStack = reason_unreachable; break;
    case emDisconnectReasonLocal:           emReasonStack = reason_local;       break;
	case emDisconnectReasonRemoteReconnect: emReasonStack = reason_reconnect;   break;
	case emDisconnectReasonRemoteConfHolding:emReasonStack = reason_confholding;break;
	case emDisconnectReasonRemoteHasCascaded:emReasonStack = reason_hascascaded;break;

    case emDisconnectReasonDRQ:
    case emDisconnectReasonRtd:
    case emDisconnectReasonUnmatched:
    case emDisconnectReasonUnknown:
    default:                                emReasonStack = reason_unknown;     break;
    }
    return emReasonStack;
}

/*=============================================================================
  函 数 名： DisconnectReasonMtAdp2Vc
  功    能： 将呼叫终端失败的错误码转成MCU对应错误码定义
  算法实现： 
  全局变量： 
  参    数： u8 byReason
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/05/14    4.0			张宝卿                  创建
=============================================================================*/
u8 CMtAdpUtils::DisconnectReasonMtAdp2Vc( emDisconnectReason emReason )
{
    u8 byReasonVc = 0;
    switch( emReason )
    {
    case emDisconnectReasonDRQ:         byReasonVc = MTLEFT_REASON_DRQ; break;
    case emDisconnectReasonRtd:         byReasonVc = MTLEFT_REASON_RTD; break;
    case emDisconnectReasonBusy:        byReasonVc = MTLEFT_REASON_BUSY; break;
    case emDisconnectReasonNormal:      byReasonVc = MTLEFT_REASON_NORMAL; break;
    case emDisconnectReasonRejected:    byReasonVc = MTLEFT_REASON_REJECTED; break;
    case emDisconnectReasonUnreachable: byReasonVc = MTLEFT_REASON_UNREACHABLE;  break;
    case emDisconnectReasonLocal:       byReasonVc = MTLEFT_REASON_LOCAL;  break;
    case emDisconnectReasonUnmatched:   byReasonVc = MTLEFT_REASON_UNMATCHTYPE; break;
    case emDisconnectReasonUnknown:     byReasonVc = MTLEFT_REASON_UNKNOW; break;
	case emDisconnectReasonBysyExt:     byReasonVc = MTLEFT_REASON_BUSYEXT; break;
	case emDisconnectReasonRemoteReconnect:   byReasonVc = MTLEFT_REASON_REMOTERECONNECT; break;
// [pengjie 2010/6/2] 添加两个原因 协议栈->mcu
	case emDisconnectReasonRemoteConfHolding: byReasonVc = MTLEFT_REASON_REMOTECONFHOLDING; break;
	case emDisconnectReasonRemoteHasCascaded: byReasonVc = MTLEFT_REASON_REMOTEHASCASCADED; break;
// End
    default:                            byReasonVc = MTLEFT_REASON_UNKNOW; break;
    }
    return byReasonVc;
}

/*=============================================================================
  函 数 名： Ipdw2Str
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u32 dwIp
             s8* pszStr
  返 回 值： void 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
void CMtAdpUtils::Ipdw2Str( u32 dwIp, s8* pszStr )
{
	u8 *pbyPointer = (u8 *)&dwIp;
	sprintf( pszStr,"%d.%d.%d.%d",pbyPointer[0],pbyPointer[1],pbyPointer[2],pbyPointer[3]);
}


/*=============================================================================
  函 数 名： GetAliasType
  功    能： 
  算法实现： 
  全局变量： 
  参    数： s8  * pszStr
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
u8 CMtAdpUtils::GetAliasType( s8* pszStr )
{
	if(IsIpStr(pszStr))
	{	
		//ip
		return (u8)mtAliasTypeTransportAddress;
	}
	else if(IsIncludeAllChar(pszStr,"0123456789*,#"))
	{
		//e164
		return (u8)mtAliasTypeE164;
	}
	else
	{
		//h323
		return (u8)mtAliasTypeH323ID;
	}

//	return mtAliasTypeOthers;
}

/*=============================================================================
  函 数 名： IsIncludeAllChar
  功    能： 
  算法实现： 
  全局变量： 
  参    数： s8  * pszSrc
             s8  * pszDst
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
BOOL32 CMtAdpUtils::IsIncludeAllChar(s8* pszSrc, s8* pszDst)
{
	s8* pszSrcTmp = pszSrc;
	s8* pszDstTmp = pszDst;
	while(*pszSrcTmp)
	{
		pszDstTmp = pszDst;
		while(*pszDstTmp)
		{
			if(*pszSrcTmp == *pszDstTmp) 
				break;
//			*pszDstTmp++;
            pszDstTmp++;
		}
		if(*pszDstTmp == 0)
		{
			return FALSE;
		}
		pszSrcTmp ++;
	}
	return TRUE;
}

/*=============================================================================
  函 数 名： IsIpStr
  功    能： 
  算法实现： 
  全局变量： 
  参    数： s8* pszStr
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
BOOL32 CMtAdpUtils::IsIpStr(s8* pszStr)
{
    s32 anValue[4]={ 0 };
	s32 anLen[4] = { 0 };

	s32 nDot = 0;
	s8* pszTmp = pszStr;
	s32 nPos;
	for( nPos = 0; *pszTmp && nPos < 16; nPos ++, pszTmp ++ )
	{
		if( *pszTmp == '.' )
		{
			nDot ++;
			//excude 1256.1.1.1.1
			if( nDot > 3 )
			{
				return FALSE;
			}
			continue;			
		}
		//excude a.1.1.1
		if( *pszTmp<'0'|| *pszTmp>'9' )
		{
			return FALSE;
		}

		anValue[nDot] = anValue[nDot]*10 + (*pszTmp-'0');
		anLen[nDot] ++;
	}

	//excude 1256.1.1.1234444
	if( nPos >= 16 )
	{
		return FALSE;
	}

	//excude 0.1.1.1
	if( anValue[0] == 0 )
	{
		return FALSE;
	}

	for( s32 j = 0; j < 4; j ++ )
	{
		//excude 1256.1.1.1
		if( anLen[j] == 0 || anLen[j] > 3 )
		{
			return FALSE;
		}
		//excude 256.1.1.1
		if( anValue[j] > 255 )
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*=============================================================================
  函 数 名： MediaModeOut2In
  功    能： 
  算法实现： 
  全局变量： 
  参    数： emMediaType mediatype
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
u8 CMtAdpUtils::MediaModeOut2In(emMediaType mediatype)
{
	switch(mediatype)
	{
	case emMediaTypeVideo: return MODE_VIDEO; //break;
	case emMediaTypeAudio: return MODE_AUDIO; //break;
	case emMediaTypeData:  return MODE_DATA;  //break;
	default:               return MODE_NONE;  //break;
	}
}
/*=============================================================================
函 数 名： GetResFromStr
功    能： 
算法实现： 
全局变量： 
参    数： s8* pszRes
返 回 值： u8 
-----------------------------------------------------------------------------
修改记录：
日  期		版本		修改人		走读人    修改内容
2008/10/30    4.5			张宝卿                  创建
=============================================================================*/
u8 CMtAdpUtils::GetResFromStr(s8* pszRes)
{
    if (0 == strcmp(pszRes, "VGA"))
    {
        return 11;
    }
    else if (0 == strcmp(pszRes, "SVGA"))
    {
        return 12;
    }
    else if (0 == strcmp(pszRes, "XGA"))
    {
        return 13;
    }
    else if (0 == strcmp(pszRes, "SXGA"))
    {
        return 33;
    }
    else if (0 == strcmp(pszRes, "UXGA"))
    {
        return 34;
    }
    else if (0 == strcmp(pszRes, "SQCIF"))
    {
        return 1;
    }
    else if (0 == strcmp(pszRes, "QCIF"))
    {
        return 2;
    }
    else if (0 == strcmp(pszRes, "CIF"))
    {
        return 3;
    }
    else if (0 == strcmp(pszRes, "2CIF"))
    {
        return 4;
    }
    else if (0 == strcmp(pszRes, "4CIF"))
    {
        return 5;
    }
    else if (0 == strcmp(pszRes, "16CIF"))
    {
        return 6;
    }
    else if (0 == strcmp(pszRes, "SIF"))
    {
        return 8;
    }
    else if (0 == strcmp(pszRes, "2SIF"))
    {
        return 9;
    }
    else if (0 == strcmp(pszRes, "4SIF"))
    {
        return 10;
    }
    else if (0 == strcmp(pszRes, "W4CIF"))
    {
        return 31;
    }
    else if (0 == strcmp(pszRes, "720P"))
    {
        return 32;
    }
	
    else if (0 == strcmp(pszRes, "1080P"))
    {
        return 35;
    }
	
    return 0;
}

/*=============================================================================
  函 数 名： MediaModeIn2Out
  功    能： 
  算法实现： 
  全局变量： 
  参    数： u8 byMediaMode
  返 回 值： emMediaType 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
emMediaType CMtAdpUtils::MediaModeIn2Out( u8 byMediaMode)
{
	switch(byMediaMode)
	{
	case MODE_VIDEO: return emMediaTypeVideo; //break;
	case MODE_AUDIO: return emMediaTypeAudio; //break;
	case MODE_DATA:  return emMediaTypeData;  //break;
	default:         return emMediaTypeNone;  //break;
	}
}

/*=============================================================================
  函 数 名： PszGetRASReasonName
  功    能： 获得RAS错误类型名
  算法实现： 
  全局变量： 
  参    数： s32    nReason
             s8   * pszBuf
             u8     byBufLen 
  返 回 值： 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2003/11/11    1.0			TanGuang                  创建
=============================================================================*/
s8* CMtAdpUtils::PszGetRASReasonName( s32 nReason, s8*  pszBuf, u8 byBufLen )
{
	if( ! pszBuf || byBufLen  == 0 )
	{
		return NULL;
	}
	s8 achBuf[128];

#define REASON_CASE(x) \
	case x: \
		strcpy( achBuf, #x); \
		strncpy( pszBuf, achBuf + strlen("cmRASReason"), byBufLen  ); \
	break;

	switch( nReason )
	{
    REASON_CASE( cmRASReasonResourceUnavailable )             /* GRJ, RRJ, ARJ, LRJ - gatekeeper resources exhausted */
    REASON_CASE( cmRASReasonInsufficientResources )           /* BRJ */
    REASON_CASE( cmRASReasonInvalidRevision )                 /* GRJ, RRJ, BRJ */
    REASON_CASE( cmRASReasonInvalidCallSignalAddress )        /* RRJ */
    REASON_CASE( cmRASReasonInvalidRASAddress )               /* RRJ - supplied address is invalid */
    REASON_CASE( cmRASReasonInvalidTerminalType )             /* RRJ */
    REASON_CASE( cmRASReasonInvalidPermission )               /* ARJ - permission has expired */
															  /* BRJ - true permission violation */
															  /* LRJ - exclusion by administrator or feature */
    REASON_CASE( cmRASReasonInvalidConferenceID )             /* BRJ - possible revision */
    REASON_CASE( cmRASReasonInvalidEndpointID )               /* ARJ */
    REASON_CASE( cmRASReasonCallerNotRegistered )             /* ARJ */
    REASON_CASE( cmRASReasonCalledPartyNotRegistered )        /* ARJ - can't translate address */
    REASON_CASE( cmRASReasonDiscoveryRequired )               /* RRJ - registration permission has aged */
    REASON_CASE( cmRASReasonDuplicateAlias )                  /* RRJ - alias registered to another endpoint */
    REASON_CASE( cmRASReasonTransportNotSupported )           /* RRJ - one or more of the transports */
    REASON_CASE( cmRASReasonCallInProgress )                  /* URJ */
    REASON_CASE( cmRASReasonRouteCallToGatekeeper )           /* ARJ */
    REASON_CASE( cmRASReasonRequestToDropOther )              /* DRJ - can't request drop for others */
    REASON_CASE( cmRASReasonNotRegistered )                   /* DRJ, LRJ, INAK - not registered with gatekeeper */
    REASON_CASE( cmRASReasonUndefined )                       /* GRJ, RRJ, URJ, ARJ, BRJ, LRJ, INAK */
    REASON_CASE( cmRASReasonTerminalExcluded )                /* GRJ - permission failure, not a resource failure */
    REASON_CASE( cmRASReasonNotBound )                        /* BRJ - discovery permission has aged */
    REASON_CASE( cmRASReasonNotCurrentlyRegistered )          /* URJ */
    REASON_CASE( cmRASReasonRequestDenied )                   /* ARJ - no bandwidrg available */
                                                              /* LRJ - cannot find location */
    REASON_CASE( cmRASReasonLocationNotFound )                /* LRJ - cannot find location */
    REASON_CASE( cmRASReasonSecurityDenial )                  /* GRJ, RRJ, URJ, ARJ, BRJ, LRJ, DRJ, INAK */
    REASON_CASE( cmRASReasonTransportQOSNotSupported )        /* RRJ */
    REASON_CASE( cmRASResourceUnavailable )                   /* Same as cmRASReasonResourceUnavailable */
    REASON_CASE( cmRASReasonInvalidAlias )                    /* RRJ - alias not consistent with gatekeeper rules */
    REASON_CASE( cmRASReasonPermissionDenied )                /* URJ - requesting user not allowed to unregister specified user */
    REASON_CASE( cmRASReasonQOSControlNotSupported )          /* ARJ */
    REASON_CASE( cmRASReasonIncompleteAddress )               /* ARJ, LRJ */
    REASON_CASE( cmRASReasonFullRegistrationRequired )        /* RRJ - registration permission has expired */
    REASON_CASE( cmRASReasonRouteCallToSCN )                  /* ARJ, LRJ */
    REASON_CASE( cmRASReasonAliasesInconsistent )             /* ARJ, LRJ - multiple aliases in request identify distinct people */
    REASON_CASE( cmRASReasonAdditiveRegistrationNotSupported )/* RRJ */
    REASON_CASE( cmRASReasonInvalidTerminalAliases )          /* RRJ */
    REASON_CASE( cmRASReasonExceedsCallCapacity )             /* ARJ - destination does not have the capacity for this call */
    REASON_CASE( cmRASReasonCollectDestination )              /* ARJ */
    REASON_CASE( cmRASReasonCollectPIN )                      /* ARJ */
    REASON_CASE( cmRASReasonGenericData )                     /* GRJ, RRJ, ARJ, LRJ */
    REASON_CASE( cmRASReasonNeededFeatureNotSupported )       /* GRJ, RRJ, ARJ, LRJ */
    REASON_CASE( cmRASReasonUnknownMessageResponse )          /* XRS message was received for the request */
    REASON_CASE( cmRASReasonHopCountExceeded )                /* LRJ */
	case -1: strncpy(  pszBuf, "Timeout", byBufLen  ); break;
	default : * pszBuf = 0;
	}
#undef REASON_CASE

	return  pszBuf;
}


/*=============================================================================
  函 数 名： IsSrcFpsAcptable
  功    能： 源端发送的帧率是否可以为目的端接收
  算法实现： 源端发送的帧率小于等于或模糊小于等于目的端接收能力 即认为可以接收
  全局变量： 
  参    数： u8 bySrcFPS
             u8 byDstFPS
  返 回 值： BOOL32
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2007/12/21    4.5			张宝卿                  创建
=============================================================================*/
BOOL32 CMtAdpUtils::IsSrcFpsAcptable(u8 bySrcFPS, u8 byDstFPS)
{
    //直接支持
    if ( bySrcFPS <= byDstFPS )
    {
        return TRUE;
    }
    //敏感点模糊判断
    if ( byDstFPS >= 58 && byDstFPS <= 60 )
    {
        if ( bySrcFPS >= 58 && bySrcFPS <= 60 )
        {
            return TRUE;
        }
    }
    else if ( byDstFPS >= 25 && byDstFPS <= 30 )
    {
        if ( bySrcFPS >= 24 && bySrcFPS <= 30 )
        {
            return TRUE;
        }
    }
    else if ( byDstFPS >= 14 && byDstFPS <= 16 )
    {
        if ( bySrcFPS >= 14 && bySrcFPS <= 16 )
        {
            return TRUE;
        }
    }
    else if ( byDstFPS >= 9 && byDstFPS <= 11 )
    {
        if ( bySrcFPS >= 9 && bySrcFPS <= 11 )
        {
            return TRUE;
        }
    }
    else if ( byDstFPS >= 4 && byDstFPS <= 6 )
    {
        if ( bySrcFPS >= 4 && bySrcFPS <= 6 )
        {
            return TRUE;
        }
    }

    return FALSE;
}

/*=============================================================================
  函 数 名： TRY_MEM_COPY
  功    能： 内存拷贝处理
  算法实现： 
  全局变量： 
  参    数：  
  返 回 值： BOOL32 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2005/12/31    4.0			张宝卿                  创建
=============================================================================*/
BOOL32 TRY_MEM_COPY( void *pDst, void *pSrc, u32 dwLen, void *pLimit )
{
	if( (u32)pDst + dwLen > (u32)pLimit ) 
	{
		OspLog( LOGLVL_DEBUG1, "insufficient memory to store the data.\n"); 
		return FALSE; 
	} 
	memcpy( pDst, pSrc, dwLen );	
	return TRUE;
}




/******************************************************************************
	说明: 
	1. 以下是还未启用的类型适配函数
	2. 以下函数在3.6版本或(和)4.0版本作出涉及到以下类型的调整时使用
	3. 如果是3.6版本作出调整, 将mastruct.h和3.6的mcustruct.h同步, 然后根据调整
	   内容修改以下相应的适配函数, 并启用, 在相关的地方进行适配
	4. 如果是4.0版本作出调整, 直接根据调整内容修改以下相应的适配函数, 并启用
	5. 如果两个版本均作出了调整, 则按3, 4修改, 并启用
******************************************************************************/
 
/*=============================================================================
  函 数 名： ActiveMediaTypeVer36ToVer40
  功    能： kdv约定的本地发送时使用的活动媒体类型适配
  算法实现： 
  全局变量： 
  参    数： u8 byActiveMediaType36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ActiveMediaTypeVer36ToVer40( u8 byActiveMediaType36 )
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
*/
/*=============================================================================
  函 数 名： ActiveMediaTypeVer40ToVer36
  功    能： kdv约定的本地发送时使用的活动媒体类型适配
  算法实现： 
  全局变量： 
  参    数： u8 byActiveMediaType40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ActiveMediaTypeVer40ToVer36( u8 byActiveMediaType40 )
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
*/
/*=============================================================================
  函 数 名： EqpMainTypeVer36ToVer40
  功    能： 设备主类适配
  算法实现： 
  全局变量： 
  参    数： u8 byEqpMainType36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::EqpMainTypeVer36ToVer40( u8 byEqpMainType36 )
{
	switch( byEqpMainType36 )
	{
	case TYPE_MCU_VER36:			return TYPE_MCU;
	case TYPE_MCUPERI_VER36:		return TYPE_MCUPERI; 
	case TYPE_MT_VER36:				return TYPE_MT;
	default:						return NULL;
	}
}
*/
/*=============================================================================
  函 数 名： EqpMainTypeVer40ToVer36
  功    能： 设备主类适配
  算法实现： 
  全局变量： 
  参    数： u8 byEqpMainType40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::EqpMainTypeVer40ToVer36( u8 byEqpMainType40 )
{
	switch( byEqpMainType40 )
	{
	case TYPE_MCU:					return TYPE_MCU_VER36;
	case TYPE_MCUPERI:				return TYPE_MCUPERI_VER36; 
	case TYPE_MT:					return TYPE_MT_VER36;
	default:						return NULL;
	}
}
*/
/*=============================================================================
  函 数 名： MediaModeVer36ToVer40
  功    能： 传送图像声音参数模式适配
  算法实现： 
  全局变量： 
  参    数： u8 byMediaType36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::MediaModeVer36ToVer40( u8 byMediaMode36 )
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
*/
/*=============================================================================
  函 数 名： MediaModeVer40ToVer36
  功    能： 传送图像声音参数模式适配
  算法实现： 
  全局变量： 
  参    数： u8 byMediaType40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::MediaModeVer40ToVer36( u8 byMediaMode40 )
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
*/
/*=============================================================================
  函 数 名： VideoDStreamTypeVer36ToVer40
  功    能： 视频双流类型适配
  算法实现： 
  全局变量： 
  参    数： u8 byDStreamType36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::VideoDStreamTypeVer36ToVer40( u8 byDStreamType36 )
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
*/
/*=============================================================================
  函 数 名： VideoDStreamTypeVer40ToVer36
  功    能： 视频双流类型适配
  算法实现： 
  全局变量： 
  参    数： u8 byDStreamType40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::VideoDStreamTypeVer40ToVer36( u8 byDStreamType40 )
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
*/
/*=============================================================================
  函 数 名： ConfOpenModeVer36ToVer40
  功    能： 会议开放方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfOpenMode36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfOpenModeVer36ToVer40( u8 byConfOpenMode36 )
{
	switch( byConfOpenMode36 )
	{
	case CONF_OPENMODE_CLOSED_VER36:	return CONF_OPENMODE_CLOSED;
	case CONF_OPENMODE_NEEDPWD_VER36:	return CONF_OPENMODE_NEEDPWD;
	case CONF_OPENMODE_OPEN_VER36:		return CONF_OPENMODE_OPEN;
	default:							return CONF_OPENMODE_CLOSED;
	}
}
*/
/*=============================================================================
  函 数 名： ConfOpenModeVer40ToVer36
  功    能： 会议开放方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfOpenMode40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfOpenModeVer40ToVer36( u8 byConfOpenMode40 )
{
	switch( byConfOpenMode40 )
	{
	case CONF_OPENMODE_CLOSED:		return CONF_OPENMODE_CLOSED_VER36;
	case CONF_OPENMODE_NEEDPWD:		return CONF_OPENMODE_NEEDPWD_VER36;
	case CONF_OPENMODE_OPEN:		return CONF_OPENMODE_OPEN_VER36;
	default:						return CONF_OPENMODE_CLOSED_VER36;
	}
}
*/
/*=============================================================================
  函 数 名： ConfEncryptModeVer36ToVer40
  功    能： 会议加密方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfEncryptMode36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfEncryptModeVer36ToVer40( u8 byConfEncryptMode36 )
{	
	switch( byConfEncryptMode36 )
	{
	case CONF_ENCRYPTMODE_AES_VER36:	return CONF_ENCRYPTMODE_AES;
	case CONF_ENCRYPTMODE_DES_VER36:	return CONF_ENCRYPTMODE_DES;
	case CONF_ENCRYPTMODE_NONE_VER36:	return CONF_ENCRYPTMODE_NONE;
	default:							return CONF_ENCRYPTMODE_NONE;
	}
}
*/
/*=============================================================================
  函 数 名： ConfEncryptModeVer40ToVer36
  功    能： 会议加密方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfEncryptMode40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfEncryptModeVer40ToVer36( u8 byConfEncryptMode40 )
{
	switch( byConfEncryptMode40 )
	{
	case CONF_ENCRYPTMODE_AES:		return CONF_ENCRYPTMODE_AES_VER36;
	case CONF_ENCRYPTMODE_DES:		return CONF_ENCRYPTMODE_DES_VER36;
	case CONF_ENCRYPTMODE_NONE:		return CONF_ENCRYPTMODE_NONE_VER36;
	default:						return CONF_ENCRYPTMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  函 数 名： ConfUniformModeVer36ToVer40
  功    能： 是否支持归一化整理方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfUniformMode36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfUniformModeVer36ToVer40( u8 byConfUniformMode36 )
{
	switch( byConfUniformMode36 )
	{
	case CONF_UNIFORMMODE_NONE_VER36:	return CONF_UNIFORMMODE_NONE;
	case CONF_UNIFORMMODE_VALID_VER36:	return CONF_UNIFORMMODE_VALID;
	default:							return CONF_UNIFORMMODE_NONE;
	}
}
*/
/*=============================================================================
  函 数 名： ConfUniformModeVer40ToVer36
  功    能： 是否支持归一化整理方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfUniformMode40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfUniformModeVer40ToVer36( u8 byConfUniformMode40 )
{
	switch( byConfUniformMode40 )
	{
	case CONF_UNIFORMMODE_NONE:		return CONF_UNIFORMMODE_NONE_VER36;
	case CONF_UNIFORMMODE_VALID:	return CONF_UNIFORMMODE_VALID_VER36;
	default:						return CONF_UNIFORMMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  函 数 名： SwitchChnnlUniformModeVer36ToVer40
  功    能： 交换通道是否支持归一化整理方式适配
  算法实现： 
  全局变量： 
  参    数： u8 bySwitchChnnlUniformMode36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::SwitchChnnlUniformModeVer36ToVer40( u8 bySwitchChnnlUniformMode36 )
{
	switch( bySwitchChnnlUniformMode36 )
	{
	case SWITCHCHANNEL_UNIFORMMODE_NONE_VER36:	return SWITCHCHANNEL_UNIFORMMODE_NONE;
	case SWITCHCHANNEL_UNIFORMMODE_VALID_VER36:	return SWITCHCHANNEL_UNIFORMMODE_VALID;
	default:									return SWITCHCHANNEL_UNIFORMMODE_NONE;
	}
}
*/
/*=============================================================================
  函 数 名： SwitchChnnlUniformModeVer40ToVer36
  功    能： 交换通道是否支持归一化整理方式适配
  算法实现： 
  全局变量： 
  参    数： u8 bySwitchChnnlUniformMode40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::SwitchChnnlUniformModeVer40ToVer36( u8 bySwitchChnnlUniformMode40 )
{
	switch( bySwitchChnnlUniformMode40 )
	{
	case SWITCHCHANNEL_UNIFORMMODE_NONE:	return SWITCHCHANNEL_UNIFORMMODE_NONE_VER36;
	case SWITCHCHANNEL_UNIFORMMODE_VALID:	return SWITCHCHANNEL_UNIFORMMODE_VALID_VER36;
	default:								return SWITCHCHANNEL_UNIFORMMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  函 数 名： ConfDataModeVer36ToVer40
  功    能： 数据会议方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfDataMode36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfDataModeVer36ToVer40( u8 byConfDataMode36 )
{
	switch( byConfDataMode36 )
	{
	case CONF_DATAMODE_VAONLY_VER36:	return CONF_DATAMODE_VAONLY;
	case CONF_DATAMODE_VAANDDATA_VER36:	return CONF_DATAMODE_VAANDDATA;
	case CONF_DATAMODE_DATAONLY_VER36:	return CONF_DATAMODE_DATAONLY;
	default:							return CONF_DATAMODE_VAONLY;
	}
}
*/
/*=============================================================================
  函 数 名： ConfDataModeVer40ToVer36
  功    能： 数据会议方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfDataMode40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfDataModeVer40ToVer36( u8 byConfDataMode40 )
{
	switch( byConfDataMode40 )
	{
	case CONF_DATAMODE_VAONLY:		return CONF_DATAMODE_VAONLY_VER36;
	case CONF_DATAMODE_VAANDDATA:	return CONF_DATAMODE_VAANDDATA_VER36;
	case CONF_DATAMODE_DATAONLY:	return CONF_DATAMODE_DATAONLY_VER36;
	default:						return CONF_DATAMODE_VAONLY_VER36;
	}
}
*/
/*=============================================================================
  函 数 名： ConfReleaseModeVer36ToVer40
  功    能： 会议结束方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfReleaseMode36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfReleaseModeVer36ToVer40( u8 byConfReleaseMode36 )
{
	switch( byConfReleaseMode36 )
	{
	case CONF_RELEASEMODE_NOMT_VER36:	return CONF_RELEASEMODE_NOMT;
	case CONF_RELEASEMODE_NONE_VER36:	return CONF_RELEASEMODE_NONE;
	default:							return CONF_RELEASEMODE_NONE;
	}
}
*/
/*=============================================================================
  函 数 名： ConfReleaseModeVer40ToVer36
  功    能： 会议结束方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfReleaseMode40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfReleaseModeVer40ToVer36( u8 byConfReleaseMode40 )
{
	switch( byConfReleaseMode40 )
	{
	case CONF_RELEASEMODE_NOMT:		return CONF_RELEASEMODE_NOMT_VER36;
	case CONF_RELEASEMODE_NONE:		return CONF_RELEASEMODE_NONE_VER36;
	default:						return CONF_RELEASEMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  函 数 名： ConfTakeModeVer36ToVer40
  功    能： 会议进行方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfTakeMode36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfTakeModeVer36ToVer40( u8 byConfTakeMode36 )
{	
	switch( byConfTakeMode36 )
	{
	case CONF_TAKEMODE_SCHEDULED_VER36:	return CONF_TAKEMODE_SCHEDULED;
	case CONF_TAKEMODE_ONGOING_VER36:	return CONF_TAKEMODE_ONGOING;
	case CONF_TAKEMODE_TEMPLATE_VER36:	return CONF_TAKEMODE_TEMPLATE;
	default:							return CONF_TAKEMODE_SCHEDULED;
	}
}
*/
/*=============================================================================
  函 数 名： ConfTakeModeVer40ToVer36
  功    能： 会议进行方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfTakeMode40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfTakeModeVer40ToVer36( u8 byConfTakeMode40 )
{
	switch( byConfTakeMode40 )
	{
	case CONF_TAKEMODE_SCHEDULED:	return CONF_TAKEMODE_SCHEDULED_VER36;
	case CONF_TAKEMODE_ONGOING:		return CONF_TAKEMODE_ONGOING_VER36;
	case CONF_TAKEMODE_TEMPLATE:	return CONF_TAKEMODE_TEMPLATE_VER36;
	default:						return CONF_TAKEMODE_SCHEDULED_VER36;
	}
}
*/
/*=============================================================================
  函 数 名： ConfSpeakerSrcVer36ToVer40
  功    能： 发言人的源适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfSpeakerSrc36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfSpeakerSrcVer36ToVer40( u8 byConfSpeakerSrc36 )
{
	switch( byConfSpeakerSrc36 )
	{
	case CONF_SPEAKERSRC_SELF_VER36:	return CONF_SPEAKERSRC_SELF;
	case CONF_SPEAKERSRC_CHAIR_VER36:	return CONF_SPEAKERSRC_CHAIR;
	case CONF_SPEAKERSRC_LAST_VER36:	return CONF_SPEAKERSRC_LAST;
	default:							return CONF_SPEAKERSRC_SELF;
	}
}
*/
/*=============================================================================
  函 数 名： ConfSpeakerSrcVer40ToVer36
  功    能： 发言人的源适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfSpeakerSrc40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfSpeakerSrcVer40ToVer36( u8 byConfSpeakerSrc40 )
{
	switch( byConfSpeakerSrc40 )
	{
	case CONF_SPEAKERSRC_SELF:		return CONF_SPEAKERSRC_SELF_VER36;
	case CONF_SPEAKERSRC_CHAIR:		return CONF_SPEAKERSRC_CHAIR_VER36;
	case CONF_SPEAKERSRC_LAST:		return CONF_SPEAKERSRC_LAST_VER36;
	default:						return CONF_SPEAKERSRC_SELF_VER36;
	}
}
*/
/*=============================================================================
  函 数 名： ConfVmpModeVer36ToVer40
  功    能： 画面合成方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfVmpMode36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfVmpModeVer36ToVer40( u8 byConfVmpMode36 )
{
	switch( byConfVmpMode36 )
	{
	case CONF_VMPMODE_NONE_VER36:		return CONF_VMPMODE_NONE;
	case CONF_VMPMODE_CTRL_VER36:		return CONF_VMPMODE_CTRL;
	case CONF_VMPMODE_AUTO_VER36:		return CONF_VMPMODE_AUTO;
	default:							return CONF_VMPMODE_NONE;
	}
}
*/
/*=============================================================================
  函 数 名： ConfVmpModeVer40ToVer36
  功    能： 画面合成方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfVmpMode40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfVmpModeVer40ToVer36( u8 byConfVmpMode40 )
{
	switch( byConfVmpMode40 )
	{
	case CONF_VMPMODE_NONE:			return CONF_VMPMODE_NONE_VER36;
	case CONF_VMPMODE_CTRL:			return CONF_VMPMODE_CTRL_VER36;
	case CONF_VMPMODE_AUTO:			return CONF_VMPMODE_AUTO_VER36;
	default:						return CONF_VMPMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  函 数 名： ConfRecModeVer36ToVer40
  功    能： 会议录像方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfRecMode36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfRecModeVer36ToVer40( u8 byConfRecMode36 )
{
	switch( byConfRecMode36 )
	{
	case CONF_RECMODE_REC_VER36:		return CONF_RECMODE_REC;
	case CONF_RECMODE_PAUSE_VER36:		return CONF_RECMODE_PAUSE;
	case CONF_RECMODE_NONE_VER36:		return CONF_RECMODE_NONE;
	default:							return CONF_RECMODE_NONE;
	}
}
*/
/*=============================================================================
  函 数 名： ConfRecModeVer40ToVer36
  功    能： 会议录像方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfRecMode40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfRecModeVer40ToVer36( u8 byConfRecMode40 )
{
	switch( byConfRecMode40 )
	{
	case CONF_RECMODE_REC:			return CONF_RECMODE_REC_VER36;
	case CONF_RECMODE_PAUSE:		return CONF_RECMODE_PAUSE_VER36;
	case CONF_RECMODE_NONE:			return CONF_RECMODE_NONE_VER36;
	default:						return CONF_RECMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  函 数 名： ConfPlayModeVer36ToVer40
  功    能： 会议放像方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfPlayMode36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfPlayModeVer36ToVer40( u8 byConfPlayMode36 )
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
*/
/*=============================================================================
  函 数 名： ConfPlayModeVer40ToVer36
  功    能： 会议放像方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfPlayMode40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfPlayModeVer40ToVer36( u8 byConfPlayMode40 )
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
*/
/*=============================================================================
  函 数 名： ConfPollModeVer36ToVer40
  功    能： 会议轮询方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfPollMode36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfPollModeVer36ToVer40( u8 byConfPollMode36 )
{	
	switch( byConfPollMode36 )
	{
	case CONF_POLLMODE_VIDEO_VER36:		return CONF_POLLMODE_VIDEO;
	case CONF_POLLMODE_SPEAKER_VER36:	return CONF_POLLMODE_SPEAKER;
	case CONF_POLLMODE_NONE_VER36:		return CONF_POLLMODE_NONE;
	default:							return CONF_POLLMODE_NONE;
	}
}
*/
/*=============================================================================
  函 数 名： ConfPlayModeVer40ToVer36
  功    能： 会议轮询方式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfPlayMode40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfPollModeVer40ToVer36( u8 byConfPollMode40 )
{
	switch( byConfPollMode40 )
	{
	case CONF_POLLMODE_VIDEO:		return CONF_POLLMODE_VIDEO_VER36;
	case CONF_POLLMODE_SPEAKER:		return CONF_POLLMODE_SPEAKER_VER36;
	case CONF_POLLMODE_NONE:		return CONF_POLLMODE_NONE_VER36;
	default:						return CONF_POLLMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  函 数 名： ConfLockModeVer36ToVer40
  功    能： 会议锁定模式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfLockMode36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfLockModeVer36ToVer40( u8 byConfLockMode36 )
{
	switch( byConfLockMode36 )
	{
	case CONF_LOCKMODE_NEEDPWD_VER36:		return CONF_LOCKMODE_NEEDPWD;
	case CONF_LOCKMODE_LOCK_VER36:			return CONF_LOCKMODE_LOCK;
	case CONF_LOCKMODE_NONE_VER36:			return CONF_LOCKMODE_NONE;
	default:								return CONF_LOCKMODE_NONE;
	}
}
*/
/*=============================================================================
  函 数 名： ConfLockModeVer40ToVer36
  功    能： 会议锁定模式适配
  算法实现： 
  全局变量： 
  参    数： u8 byConfLockMode40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::ConfLockModeVer40ToVer36( u8 byConfLockMode40 )
{
	switch( byConfLockMode40 )
	{
	case CONF_LOCKMODE_NEEDPWD:			return CONF_LOCKMODE_NEEDPWD_VER36;
	case CONF_LOCKMODE_LOCK:			return CONF_LOCKMODE_LOCK_VER36;
	case CONF_LOCKMODE_NONE:			return CONF_LOCKMODE_NONE_VER36;
	default:							return CONF_LOCKMODE_NONE_VER36;
	}
}
*/
/*=============================================================================
  函 数 名： VmpMemberTypeVer36ToVer40
  功    能： 画面合成成员类型适配
  算法实现： 
  全局变量： 
  参    数： u8 byVmpMemberType36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::VmpMemberTypeVer36ToVer40( u8 byVmpMemberType36 )
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
*/
/*=============================================================================
  函 数 名： VmpMemberTypeVer40ToVer36
  功    能： 画面合成成员类型适配
  算法实现： 
  全局变量： 
  参    数： u8 byVmpMemberType40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::VmpMemberTypeVer40ToVer36( u8 byVmpMemberType40 )
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
*/
/*=============================================================================
  函 数 名： VmpStyleTypeVer36ToVer40
  功    能： 画面合成风格定义
  算法实现： 
  全局变量： 
  参    数： u8 byVmpStyleType36
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::VmpStyleTypeVer36ToVer40( u8 byVmpStyleType36 )
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
*/
/*=============================================================================
  函 数 名： VmpStyleTypeVer40ToVer36
  功    能： 画面合成风格定义
  算法实现： 
  全局变量： 
  参    数： u8 byVmpStyleType40
  返 回 值： u8 
  -----------------------------------------------------------------------------
  修改记录：
  日  期		版本		修改人		走读人    修改内容
  2006/01/03    4.0			张宝卿                  创建
=============================================================================*/
/*u8 CMtAdpUtils::VmpStyleTypeVer40ToVer36( u8 byVmpStyleType40 )
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
}*/

/*==============================================================================
函数名    :  GetResProduct
功能      :  获取对应分辨率高宽之乘积
算法实现  :  
参数说明  :  u8 byRes [in]分辨率格式，诸如4cif，720p等
返回值说明:  u32	  [out]分辨率高宽之乘积
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2008-10-22   薛亮											创建
==============================================================================*/
u32 CMtAdpUtils::GetResProduct(u8 byRes)
{
    u16 wWidth = 0;
    u16 wHeight = 0;
    
    switch(byRes)
    {
    case VIDEO_FORMAT_SQCIF_112x96:
        {
			wWidth = 112;
			wHeight = 96;
			break;
        }
    case VIDEO_FORMAT_SQCIF_96x80:
		{
			wWidth = 96;
			wHeight = 80;
			break;
        }
    case VIDEO_FORMAT_SQCIF:
        {
			wWidth = 128;
			wHeight = 96;
			break;
        }
    case VIDEO_FORMAT_QCIF:
        {
			wWidth = 176;
			wHeight = 144;
			break;
        }
    case VIDEO_FORMAT_CIF:
        {
			wWidth = 352;
			wHeight = 288;
			break;
        }
    case VIDEO_FORMAT_2CIF:
        {
			wWidth = 352;
			wHeight = 576;
			break;
        }
    case VIDEO_FORMAT_4CIF: //目前按p制： 704*576
		{
			wWidth = 704;
			wHeight = 576;
			break;
        }
    case VIDEO_FORMAT_16CIF:
        {
			wWidth = 1048;
			wHeight = 1152;
			break;
        }
    case VIDEO_FORMAT_AUTO:
        //不解析
        break;
        
    case VIDEO_FORMAT_SIF:
        {
			wWidth = 352;
			wHeight = 240;
			break;
        }
    case VIDEO_FORMAT_2SIF:
        {
			wWidth = 352;
			wHeight = 480;
			break;
        }
    case VIDEO_FORMAT_4SIF:
        {
			wWidth = 704;
			wHeight = 480;
			break;
        }
    case VIDEO_FORMAT_VGA:
        {
			wWidth = 640;
			wHeight = 480;
			break;
        }
    case VIDEO_FORMAT_SVGA:
        {
			wWidth = 800;
			wHeight = 600;
			break;
        }
    case VIDEO_FORMAT_XGA:
        {
			wWidth = 1024;
			wHeight = 768;
			break;
		}
    case VIDEO_FORMAT_W4CIF:
        {
			wWidth = 1024;
			wHeight = 576;
			break;
		}
    case VIDEO_FORMAT_HD720:
        {
			wWidth = 1280;
			wHeight = 720;
			break;
		}
    case VIDEO_FORMAT_SXGA:
        {
			wWidth = 1280;
			wHeight = 1024;
			break;
		}
    case VIDEO_FORMAT_UXGA:
        {
			wWidth = 1600;
			wHeight = 1200;
			break;
		}
    case VIDEO_FORMAT_HD1080:
        {
			wWidth = 1920;
			wHeight = 1088;
			break;
		}
    default:
        break;
    }

    if ( 0 == wHeight || 0 == wWidth )
    {
        StaticLog( "[GetResProduct] res.%d, ignore it\n", byRes );
    }

	return (wWidth * wHeight);
   
}


/*==============================================================================
函数名    :  ResCmp
功能      :  
算法实现  :  
参数说明  :  u8 byRes1	[in] 
u8 byRes2	[in]	 
返回值说明:  s8, byRes1 < byRes2,则返回-1； byRes1 = byRes2则返回0；byRes1>byRes2 则返回1;
如果比较对象有VIDEO_FORMAT_AUTO的，返回2表明没有比较意义
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2008-10-22
==============================================================================*/

s8 CMtAdpUtils::ResCmp(u8 byRes1, u8 byRes2)
{
	if( VIDEO_FORMAT_AUTO == byRes1 || VIDEO_FORMAT_AUTO == byRes2)
	{
		return 2;
	}
	
	u32 dwResProduct1 = GetResProduct(byRes1);
	u32 dwResProduct2 = GetResProduct(byRes2);
	
	if( dwResProduct1 <  dwResProduct2 )
	{
		return -1;
	}
	else if( dwResProduct1 >  dwResProduct2 )
	{
		return 1;
	}
	else 
	{
		return 0;
	}
	
}

/*==============================================================================
函数名    :  McuToMcuMtStatusVerR4V5ToVer40
功能      :  mcu级联会议终端状态结构转换
算法实现  :  
参数说明  : TMcuToMcuMtStatus &tMcuToMcuMtStatus 现在的终端状态 
			TMcuToMcuMtStatusBeforeV4R5 tMcuToMcuMtStatusBeforeV4R5 V4R5以前版本的级联终端状态
返回值说明:  u32	  [out]分辨率高宽之乘积
-------------------------------------------------------------------------------
修改记录  :  
日  期       版本          修改人          走读人          修改记录
2010-04-26   周晶晶											创建
==============================================================================*/
void CMtAdpUtils::McuToMcuMtStatusVerR4V5ToVer40( TMcuToMcuMtStatus &tMcuToMcuMtStatus,
											 TMcuToMcuMtStatusBeforeV4R5 &tMcuToMcuMtStatusBeforeV4R5 
											 )
{
	tMcuToMcuMtStatusBeforeV4R5.SetIsEnableFECC( tMcuToMcuMtStatus.IsEnableFECC() );
	tMcuToMcuMtStatusBeforeV4R5.SetIsMixing( tMcuToMcuMtStatus.IsMixing() );
	tMcuToMcuMtStatusBeforeV4R5.SetIsVideoLose( tMcuToMcuMtStatus.IsVideoLose() );
	tMcuToMcuMtStatusBeforeV4R5.SetCurVideo( tMcuToMcuMtStatus.GetCurVideo() );
	tMcuToMcuMtStatusBeforeV4R5.SetCurAudio( tMcuToMcuMtStatus.GetCurAudio() );
	tMcuToMcuMtStatusBeforeV4R5.SetMtBoardType( tMcuToMcuMtStatus.GetMtBoardType() );
	tMcuToMcuMtStatusBeforeV4R5.SetPartId( tMcuToMcuMtStatus.GetPartId() );
	tMcuToMcuMtStatusBeforeV4R5.SetIsAutoCallMode( tMcuToMcuMtStatus.IsAutoCallMode() );
	tMcuToMcuMtStatusBeforeV4R5.SetSendVideo( tMcuToMcuMtStatus.IsSendVideo() );
	tMcuToMcuMtStatusBeforeV4R5.SetSendAudio( tMcuToMcuMtStatus.IsSendAudio() );
	tMcuToMcuMtStatusBeforeV4R5.SetRecvVideo( tMcuToMcuMtStatus.IsRecvVideo() );
	tMcuToMcuMtStatusBeforeV4R5.SetRecvAudio( tMcuToMcuMtStatus.IsRecvAudio() );

}

/*=============================================================================
    函 数 名： IsIPTypeFromString
    功    能： 从待判别的字串判别是否为ip字串
    算法实现： 
    全局变量： 
    参    数： s8* pszAliasString 待判别的字串
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/25   3.6			万春雷                  创建
=============================================================================*/
BOOL32 CMtAdpUtils::IsIPTypeFromString(s8* pszAliasString)
{
	s32 anValue[4] = {0};
	s32 anLen[4] = {0};
	s32 nDot   = 0;
	s8 *pszTmp = pszAliasString;
	s32 nPos   = 0;
	for(nPos=0; *pszTmp&&nPos<16; nPos++,pszTmp++)
	{
		if( '.' == *pszTmp )
		{
			nDot++;
			//excude 1256.1.1.1.1
			if(nDot > 3)
			{
				return FALSE;
			}
			continue;			
		}
		//excude a.1.1.1
		if( *pszTmp<'0'|| *pszTmp>'9' )
		{
			return FALSE;
		}
		
		anValue[nDot] = anValue[nDot]*10 + (*pszTmp-'0');
		anLen[nDot]++;
	}
	
	//excude 1256.1.1.1234444
	if( nPos >=16 )
	{
		return FALSE;
	}
	
	//excude 0.1.1.1
	if( 0 == anValue[0] )
	{
		return FALSE;
	}
	
	for(nPos=0; nPos<4; nPos++)
	{
		//excude 1256.1.1.1
		if( (0 == anLen[nPos]) || (anLen[nPos] > 3) )
		{
			return FALSE;
		}
		//excude 256.1.1.1
		if(anValue[nPos] > 255)
		{
			return FALSE;
		}
	}

	return TRUE;
}

/*=============================================================================
    函 数 名： IsIPTypeFromString
    功    能： 从待判别的字串判别是否为E164字串
    算法实现： 
    全局变量： 
    参    数： s8* pszAliasString 待判别的字串
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/25   3.6			万春雷                  创建
=============================================================================*/
BOOL32 CMtAdpUtils::IsE164TypeFromString(s8 *pszSrc, s8* pszDst)
{
	s8 *pszSrcTmp = pszSrc;
	s8 *pszDstTmp = pszDst;
	while(*pszSrcTmp)
	{
		pszDstTmp = pszDst;
		while(*pszDstTmp)
		{
			if(*pszSrcTmp == *pszDstTmp) 
			{
				break;
			}
//			*pszDstTmp++;
            pszDstTmp++;
		}
		if( 0 == *pszDstTmp )
		{
			return FALSE;
		}
		pszSrcTmp++;
	}

	return TRUE;
}

/*=============================================================================
    函 数 名： GetMtAliasTypeFromString
    功    能： 从待判别的字串判别终端类型
    算法实现： 
    全局变量： 
    参    数： s8* pszAliasString 待判别的字串
    返 回 值： u8 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    2005/7/25   3.6			万春雷                  创建
=============================================================================*/
u8 CMtAdpUtils::GetMtAliasTypeFromString(s8* pszAliasString)
{
	if(IsIPTypeFromString(pszAliasString))
	{	
		//ip
		return (u8)mtAliasTypeTransportAddress;
	}
	else if(IsE164TypeFromString(pszAliasString,"0123456789*,#"))
	{
		//e164
		return (u8)mtAliasTypeE164;
	}
	else
	{
		//h323
		return (u8)mtAliasTypeH323ID;
	}
	
//	return mtAliasTypeOthers;
}

/*=============================================================================
    函 数 名： ConverTMt2TTERLABEL
    功    能： 将TMT结构转换成TTERLABEL结构
    算法实现： 
    全局变量： 
    参    数： IN : const TMt &tMt 需要转换的TMT信息
	           OUT: TTERLABEL &tMtLabel 转换后的TTERLABEL信息
			   IN : BOOL32 bForcLocal 是否强制将McuNo设置为local
			        g_cMtAdpApp.m_wMcuNetId即为local(192)，由于mcu和终端交互不会报
					非本级的终端信息，所以这里的bForcLocal应该为TRUE
    返 回 值： BOOL32 TURE 转换成功， FALSE 失败 
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    20110527    4.6			彭杰                  create
=============================================================================*/
BOOL32 CMtAdpUtils::ConverTMt2TTERLABEL( const TMt &tMt, TTERLABEL &tMtLabel, BOOL32 bForcLocal /*= TRUE*/ )
{
	if( bForcLocal == TRUE && tMt.GetMcuId() != 0 )
	{
		return tMtLabel.SetTerminalLabel( (u8)g_cMtAdpApp.m_wMcuNetId, tMt.GetMtId() );
	}
	else
	{
		return tMtLabel.SetTerminalLabel( (u8)tMt.GetMcuId(), tMt.GetMtId() );
	}
}

/*=============================================================================
    函 数 名： ConverTTERLABEL2TMt
    功    能： 将TTERLABEL结构转换成TMT结构
    算法实现： 
    全局变量： 
    参    数： IN : TTERLABEL &tMtLabel 需要转换的TTERLABEL信息
	           OUT: TMt &tMt   转换后的TMt信息
			   IN : u8 byConfIdx 会议Idx
			   IN : BOOL32 bForcLocal 是否强制将McuId设置为localIdx(19200)
			        由于终端和mcu交互不会报非本级的终端信息，所以
					这里的bForcLocal应该为TRUE
    返 回 值： void
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    20110527    4.6			彭杰                  create
=============================================================================*/
void CMtAdpUtils::ConverTTERLABEL2TMt( /*const*/ TTERLABEL &tMtLabel, TMt &tMt, u8 byConfIdx,  BOOL32 bForcLocal /*= TRUE*/ )
{
	if( bForcLocal == TRUE )
	{
		tMt.SetMcuId( LOCAL_MCUIDX );
	}
	else
	{
		tMt.SetMcuId( tMtLabel.GetMcuNo() );
	}

	tMt.SetMtId( tMtLabel.GetTerNo() );
	tMt.SetConfIdx( byConfIdx );
}
/*=============================================================================
    函 数 名： IsDynamicRes
    功    能： 是否动态分辨率
    算法实现： 
    全局变量： 
    参    数： const u8 byRes 分辨率
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    20110618    4.6			朱胜泽                  create
=============================================================================*/
BOOL32 CMtAdpUtils::IsDynamicRes( const u8 byRes )
{
    BOOL32 bDynamicRes = FALSE;
    
    switch(byRes)
    {
    case VIDEO_FORMAT_CIF:
    case VIDEO_FORMAT_2CIF:
    case VIDEO_FORMAT_4CIF:
    case VIDEO_FORMAT_16CIF:
        
    case VIDEO_FORMAT_SIF:
    case VIDEO_FORMAT_2SIF:
    case VIDEO_FORMAT_4SIF:
        
    case VIDEO_FORMAT_W4CIF:
    case VIDEO_FORMAT_HD720:
    case VIDEO_FORMAT_HD1080:
        //Auto不解析
    case VIDEO_FORMAT_AUTO:
        bDynamicRes = TRUE;
        break;
        //DS不考虑SQCIF
    case VIDEO_FORMAT_SQCIF_112x96:
    case VIDEO_FORMAT_SQCIF_96x80:
        break;
    default:
        break;
    }

	return bDynamicRes;    
}
/*=============================================================================
    函 数 名： IsStaticRes
    功    能： 是否静态分辨率
    算法实现： 
    全局变量： 
    参    数： const u8 byRes 分辨率
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    20110618    4.6			朱胜泽                  create
=============================================================================*/
BOOL32 CMtAdpUtils::IsStaticRes( const u8 byRes )
{
    BOOL32 bStaticRes = FALSE;
    
    switch(byRes)
    {
    case VIDEO_FORMAT_VGA:
    case VIDEO_FORMAT_SVGA:
    case VIDEO_FORMAT_XGA:
    case VIDEO_FORMAT_SXGA:
    case VIDEO_FORMAT_UXGA:
        bStaticRes = TRUE;
        break;
    default:
        break;
    }

	return bStaticRes;    
}
/*=============================================================================
    函 数 名： IsStaticRes
    功    能： 是否同为静态或动态分辨率（双流）
    算法实现： 
    全局变量： 
    参    数： const u8 bySrcRes 分辨率
               const u8 byDstRes
    返 回 值： BOOL32
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    20110804    4.6			朱胜泽                  create
=============================================================================*/
BOOL32 CMtAdpUtils::IsResIsSameType( const u8 bySrcRes, const u8 byDstRes)
{
    BOOL32 bMatched = FALSE;
    if (IsStaticRes(bySrcRes) && IsStaticRes(byDstRes))
    {
        bMatched = TRUE;
    }
    else if (IsDynamicRes(bySrcRes) && IsDynamicRes(byDstRes))
    {
        bMatched = TRUE;
    }
    return bMatched;
}
/*=============================================================================
    函 数 名： AdjustDStreamFps
    功    能： 调整双流帧率
    算法实现： 
    全局变量： 
    参    数： u8 byFps 帧率
    返 回 值： u8 调整后帧率
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    20110628    4.6			朱胜泽                  create
=============================================================================*/
u8 CMtAdpUtils::AdjustDStreamFps( u8 byFps)
{

    //h246双流帧率：1-5/10/15/20/30
    u8 byDstFps = 0;
    if( byFps <= 5)
    {
        byDstFps = byFps;
    }
    else if( byFps <= 20)
    {
        byDstFps = (byFps/5)*5;
    }
    else if( byFps < 30)
    {
        byDstFps = 20;
    }
    else/* if( byFps >= 30 )*/
    {
        byDstFps = 30;
    }

    return byDstFps;
} 

/*=============================================================================
    函 数 名： GetAACChnnNum
    功    能： 从Mcu传来的通道数转换成协议通信的通道数
    算法实现： 
    全局变量： 
    参    数： u8 byFps 帧率
    返 回 值： u8 调整后帧率
-----------------------------------------------------------------------------
    修改记录：
    日  期		版本		修改人		走读人    修改内容
    20110628    4.6			朱胜泽                  create
=============================================================================*/
TAACCap::emAACChnlCfg CMtAdpUtils::GetAACChnnNum(u8 byAudioTrackNum)
{
	TAACCap::emAACChnlCfg emAChnnl = TAACCap::emChnlCust;

	switch ( byAudioTrackNum )
	{
	case 1:
		emAChnnl = TAACCap::emChnl1;
		break;
	case 2:
		emAChnnl = TAACCap::emChnl2;
		break;
	case 3:
		emAChnnl = TAACCap::emChnl3;
		break;
	case 4:
		emAChnnl = TAACCap::emChnl4;
		break;
	case 5:
		emAChnnl = TAACCap::emChnl5;
		break;
	case 6:
		emAChnnl = TAACCap::emChnl5dot1;
		break;
	case 7:
		emAChnnl = TAACCap::emChnl7dot1;
		break;
	default:
		StaticLog("[GetAACChnnNum] unexpected audio track num %d!\n",byAudioTrackNum);
		break;
	}

	return emAChnnl;
}
/*=============================================================================
函 数 名： MatchAudioCap
功    能： 匹配音频共同能力集
算法实现：
全局变量： 
参    数：  u8 byEncryMode,                       --会议加密模式
			u8 byAudioCapNum,                     --音频能力个数
			TAudioTypeDesc *ptLocalAudioTypeDesc, --音频能力列表
			TCapSet *ptRemoteCapSet,              --远端能力集
			TMultiCapSupport &tCommonCap          --共同能力集
				
				  
返 回 值： BOOL32:音频是否匹配成功  
----------------------------------------------------------------------
修改记录    ：
日  期		版本		修改人		走读人    修改内容
20121221    4.7         田志勇                 create
=============================================================================*/
BOOL32 CMtAdpUtils::MatchAudioCap(   u8 byEncryMode,
									 u8 byAudioCapNum,
								     const TAudioTypeDesc *ptLocalAudioTypeDesc,
									 TCapSet *ptRemoteCapSet,
									 TMultiCapSupport &tCommonCap)
{
	//音频类型信息与原来一样，还是存放在能力集结构体中的主音频能力中的音频类型字段中，
	//声道数则放在原来的副音频能力中音频类型字段中，该字段原来并未使用过
	tCommonCap.SetMainAudioType(MEDIA_TYPE_NULL);
	u8  byAudioCapType = MEDIA_TYPE_NULL;
	u16 wAudioComType = 0;
	u8  byAudioTrackNum = 0;
	TAudioTypeDesc tAudioTypeDesc;
	TAACCap::emAACChnlCfg eLocalChnnl = TAACCap::emChnlCust;
	TAACCap::emAACChnlCfg eRemoteChnnl = TAACCap::emChnlCust;
	for (u8 byAudioCapIdx = 0;byAudioCapIdx < byAudioCapNum;byAudioCapIdx++,ptLocalAudioTypeDesc++)
	{
		if (ptLocalAudioTypeDesc == NULL)
		{
			StaticLog( "[MatchAudioCap]ptLocalAudioTypeDesc == NULL!So Return False!\n");
			return FALSE;
		}
		tAudioTypeDesc = *ptLocalAudioTypeDesc;
		byAudioCapType = tAudioTypeDesc.GetAudioMediaType();
		byAudioTrackNum = tAudioTypeDesc.GetAudioTrackNum();
		wAudioComType = CMtAdpUtils::GetComType( CMtAdpUtils::PayloadTypeIn2Out(byAudioCapType),byEncryMode);
		if (ptRemoteCapSet->IsExistsimultaneous(&wAudioComType, 1))
		{
			if (byAudioCapType == MEDIA_TYPE_AACLC ||
				byAudioCapType == MEDIA_TYPE_AACLD)
			{
				u16 wUnEncryPayload = CMtAdpUtils::PayloadTypeIn2Out(byAudioCapType);
				eLocalChnnl  = CMtAdpUtils::GetAACChnnNum(byAudioTrackNum);
				
				if(ptRemoteCapSet->GetAACCap(wUnEncryPayload) !=NULL)
				{
					eRemoteChnnl = ptRemoteCapSet->GetAACCap(wUnEncryPayload)->GetChnl();
				}
				else
				{
					StaticLog( "[MatchAudioCap]wUnEncryPayload.%d,GetAACCap return NULL,error!\n",wUnEncryPayload);

					continue;
				}

				if ( eLocalChnnl == TAACCap::emChnl2 && (0x02 & eRemoteChnnl))//支持AACL双声道
				{
					tCommonCap.SetMainAudioType(byAudioCapType);
					tCommonCap.SetMainAudioTrackNum(byAudioTrackNum);
				}
				else if ( eLocalChnnl == TAACCap::emChnl1 && (0x01 & eRemoteChnnl) )//支持AACL单声道
				{
					tCommonCap.SetMainAudioType(byAudioCapType);
					tCommonCap.SetMainAudioTrackNum(byAudioTrackNum);
				}
				else
				{
					StaticLog( "[MatchAudioCap]byAudioCapIdx(%d) Audiotype(%d) AudioComtype(%d) LocalChnnlNum:%d RemoteChnnlNum:%d!,So Break;\n",
						byAudioCapIdx,byAudioCapType,wAudioComType,eLocalChnnl,eRemoteChnnl);
					continue;
				}
			} 
			else
			{
				tCommonCap.SetMainAudioType(byAudioCapType);
				tCommonCap.SetMainAudioTrackNum(1);//其他格式默认为单声道
			}
			if (tCommonCap.GetMainAudioType() != MEDIA_TYPE_NULL)
			{
				return TRUE;//找到，则直接返回
			}
		}
	}
	return FALSE;
}
// End File