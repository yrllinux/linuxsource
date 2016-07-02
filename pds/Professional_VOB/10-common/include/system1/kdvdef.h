/*****************************************************************************

   模块名      : KDV define 

   文件名      : kdvdef.h

   相关文件    : 

   文件实现功能: KDV宏定义

   作者        : 魏治兵

   版本        : V3.0  Copyright(C) 2001-2002 KDC, All rights reserved.

-----------------------------------------------------------------------------

   修改记录:

   日  期      版本        修改人      修改内容

   2004/02/17  3.0         魏治兵        创建

******************************************************************************/

#ifndef _KDV_DEFINE_H_

#define _KDV_DEFINE_H_



#ifdef __cplusplus

extern "C" {

#endif /* __cplusplus */





/*媒体类型定义*/

#define   MEDIA_TYPE_NULL                 (u8)255  /*媒体类型为空*/
	
/*音频*/

#define	  MEDIA_TYPE_MP3	              (u8)96  /*mp3 mode 0-4*/

#define   MEDIA_TYPE_G7221C	              (u8)98  /*G722.1.C Siren14*/

#define   MEDIA_TYPE_G719	              (u8)99  /*G719 non-standard of polycom serial 22*/

#define   MEDIA_TYPE_PCMA		          (u8)8   /*G.711 Alaw  mode 5*/

#define   MEDIA_TYPE_PCMU		          (u8)0   /*G.711 ulaw  mode 6*/

#define   MEDIA_TYPE_G721		          (u8)2   /*G.721*/

#define   MEDIA_TYPE_G722		          (u8)9   /*G.722*/

#define	  MEDIA_TYPE_G7231		          (u8)4   /*G.7231*/

#define   MEDIA_TYPE_ADPCM                (u8)5   /*DVI4 ADPCM*/

#define	  MEDIA_TYPE_G728		          (u8)15  /*G.728*/

#define	  MEDIA_TYPE_G729		          (u8)18  /*G.729*/
#define   MEDIA_TYPE_G7221                (u8)13  /*G.7221*/

#define   MEDIA_TYPE_AACLC                (u8)102 /*AAC LC*/
#define   MEDIA_TYPE_AACLD                (u8)103 /*AAC LD*/

/*视频*/

#define   MEDIA_TYPE_MP4	              (u8)97  /*MPEG-4*/

#define   MEDIA_TYPE_H261	              (u8)31  /*H.261*/

#define   MEDIA_TYPE_H262	              (u8)33  /*H.262 (MPEG-2)*/

#define   MEDIA_TYPE_H263	              (u8)34  /*H.263*/

#define   MEDIA_TYPE_H263PLUS             (u8)101 /*H.263+*/

#define   MEDIA_TYPE_H264	              (u8)106 /*H.264*/

#define   MEDIA_TYPE_H264_ForHuawei       (u8)105 /*H.264*/

#define	  MEDIA_TYPE_FEC					(u8)107 /* fec custom define */

/*数据*/
#define   MEDIA_TYPE_H224	              (u8)100  /*H.224 Payload 暂定100*/
#define   MEDIA_TYPE_T120                 (u8)200  /*T.120媒体类型*/
#define   MEDIA_TYPE_H239                 (u8)239  /*H.239数据类型 */
#define   MEDIA_TYPE_MMCU                 (u8)120  /*级联数据通道 */
	

/*kdv约定的本地发送时使用的活动媒体类型 */
#define   ACTIVE_TYPE_PCMA		          (u8)110   /*G.711 Alaw  mode 5*/
#define   ACTIVE_TYPE_PCMU		          (u8)111   /*G.711 ulaw  mode 6*/
#define   ACTIVE_TYPE_G721		          (u8)112   /*G.721*/
#define   ACTIVE_TYPE_G722		          (u8)113   /*G.722*/
#define	  ACTIVE_TYPE_G7231		          (u8)114   /*G.7231*/
#define	  ACTIVE_TYPE_G728		          (u8)115   /*G.728*/
#define	  ACTIVE_TYPE_G729		          (u8)116   /*G.729*/
#define   ACTIVE_TYPE_G7221               (u8)117   /*G.7221*/
#define   ACTIVE_TYPE_H261	              (u8)118   /*H.261*/
#define   ACTIVE_TYPE_H262	              (u8)119   /*H.262 (MPEG-2)*/
#define   ACTIVE_TYPE_H263	              (u8)120   /*H.263*/
#define   ACTIVE_TYPE_G7221C			  (u8)121	/*G7221c*/
#define   ACTIVE_TYPE_ADPCM				  (u8)122	/*ADPCM*/	
#define   ACTIVE_TYPE_G719                (u8)123   /*G.719*/
	



/*APP ID*/

#define AID_KDV_BASE                      (u16)0 



/*网管客户端内部应用号（1-10）*/

#define AID_NMC_BGN      AID_KDV_BASE + 1

#define AID_NMC_END      AID_NMC_BGN + 9

	

/*会议控制台内部应用号（11-20）*/

#define AID_MCS_BGN      AID_NMC_END + 1

#define AID_MCS_END      AID_MCS_BGN + 9	



/*终端控制台内部应用号（21-30）*/

#define AID_MTC_BGN      AID_MCS_END + 1

#define AID_MTC_END      AID_MTC_BGN + 9

	

/*网管服务器内部应用号（31-40）*/

#define AID_SVR_BGN      AID_MTC_END + 1

#define AID_SVR_END      AID_SVR_BGN + 9



/*代理内部应用号（41-50）*/

#define AID_AGT_BGN      AID_SVR_END + 1

#define AID_AGT_END      AID_AGT_BGN + 9

	

/*MCU内部应用号（51-100）*/

#define AID_MCU_BGN      AID_AGT_END + 1

#define AID_MCU_END       AID_MCU_BGN + 49

	

/*MT内部应用号（101-150）*/

#define AID_MT_BGN       AID_MCU_END + 1

#define AID_MT_END       AID_MT_BGN + 49

	

/*RECORDER内部应用号（151-160）*/

#define AID_REC_BGN      AID_MT_END + 1

#define AID_REC_END      AID_REC_BGN + 9

	

/*混音器内部应用号（161-170）*/

#define AID_MIXER_BGN    AID_REC_END + 1

#define AID_MIXER_END    AID_MIXER_BGN + 9

	

/*电视墙内部应用号（171-181）*/

#define AID_TVWALL_BGN   AID_MIXER_END + 1

#define AID_TVWALL_END   AID_TVWALL_BGN + 9



/*T.120服务器内部应用号（181-200）*/

#define AID_DCS_BGN      AID_TVWALL_END + 1

#define AID_DCS_END      AID_DCS_BGN + 19



/*T.120控制台内部应用号（201-210）*/

#define AID_DCC_BGN      AID_DCS_END + 1

#define AID_DCC_END      AID_DCC_BGN + 9



/*码率适配服务器内部应用号（211-220）*/

#define AID_BAS_BGN      AID_DCC_END + 1

#define AID_BAS_END      AID_BAS_BGN + 9



/*GK服务器内部应用号(221 - 230)*/

#define AID_GKS_BGN      AID_BAS_END+1

#define AID_GKS_END      AID_GKS_BGN + 9



/*GK控制台内部应用号(231 - 235)*/

#define AID_GKC_BGN      AID_GKS_END+1

#define AID_GKC_END      AID_GKC_BGN + 4



/*用户管理内部应用号(236 - 240)*/

#define AID_UM_BGN      AID_GKC_END+1

#define AID_UM_END      AID_UM_BGN + 4



/*地址簿内部应用号(241 - 250)*/

#define AID_ADDRBOOK_BGN    AID_UM_END+1

#define AID_ADDRBOOK_END    AID_ADDRBOOK_BGN + 9

/*数据会议终端内部应用号(251 - 260)*/

#define AID_DCMT_BGN    AID_ADDRBOOK_END+1

#define AID_DCMT_END    AID_DCMT_BGN + 9

/*mdsc hdsc watchdog 模块(261-265) hualiang add*/
#define AID_DSC_BGN    AID_DCMT_END + 1 
#define AID_DSC_END    AID_DSC_BGN + 4

/* radius 计费 模块(266-275) guozhongjun add*/
#define  AID_RS_BGN    AID_DSC_END + 1
#define  AID_RS_END    AID_RS_BGN + 9

/* 文件升级服务器 模块(276 - 283) wanghao 2007/1/4 */
#define AID_SUS_BGN             AID_RS_END + 1
#define AID_SUS_END             AID_SUS_BGN + 7

/* 文件升级客户端 模块(284 - 285) wanghao 2007/1/4 */
#define AID_SUC_BGN             AID_SUS_END + 1
#define AID_SUC_END             AID_SUC_BGN + 1

/* RTSP server 模块(286 - 290) xsl add */
#define AID_RTSP_BGN			AID_SUC_END +1
#define AID_RTSP_END			AID_RTSP_BGN +5


/* Traversal Server 模块(291 - 320) */
#define AID_TS_BGN				AID_RTSP_END + 1
#define AID_TS_END				AID_TS_BGN + 29


/*2004/07/15 不同类型音频码率定义*/

#define AUDIO_BITRATE_G711A 64
#define AUDIO_BITRATE_G711U 64
#define AUDIO_BITRATE_G7231 6
#define AUDIO_BITRATE_MP3   48
#define AUDIO_BITRATE_G728  16
#define AUDIO_BITRATE_G729  8/*暂不支持*/
#define AUDIO_BITRATE_G722  64
#define AUDIO_BITRATE_G7221C   24 /* guzh [2008/09/03] 该值是不准确的，需要根据具体类型确定 */
#define AUDIO_BITRATE_G719  24  //lukunpeng [2010/04/15]
#define AUDIO_BITRATE_AAC   96  /* guzh [01/18/2008] */


/*视频会议产品各模块的监听端口*/

const u16 PORT_NMS				= 60000;

const u16 PORT_NMC				= 60000;

const u16 PORT_MCU				= 60000;

const u16 PORT_MCS				= 60000;

const u16 PORT_MT				= 60000;

const u16 PORT_MTC				= 60000;

const u16 PORT_TVWALL			= 60000;

const u16 PORT_DIGITALMIXER	    = 60000;

const u16 PORT_RECORDER      	= 60000;

const u16 PORT_DCS				= 61000;

const u16 PORT_GK          		= 60000;

const u16 PORT_GKC         		= 60000; 

const u16 PORT_RS               = 61000;

/*视频会议产品各产品类型定义*/
const u16 MT_TYPE_KDV7620_B    = 0x042D;
const u16 MT_TYPE_KDV7620_A    = 0x042E;

const u16 MT_TYPE_KDV7820_A    = 0x0431;
const u16 MT_TYPE_KDV7920_A    = 0x0432;

const u16 MT_TYPE_KDV7820_B    = 0x043B;
const u16 MT_TYPE_KDV7920_B    = 0x043C;
	
//产品pid
#define BRD_PID_KDM200_HDU				0x043A
#define BRD_PID_KDM200_HDU_L		    0x0444
#define BRD_PID_KDM200_HDU_D			0x0445 
#define BRD_PID_KDV_EAPU		        0x0443
#define BRD_PID_KDV_MPU					0x0436
#define BRD_PID_KDV_MAU					0x0439	
#define BRD_PID_KDV8000A_MPC2		    0x043D
#define BRD_PID_KDV8000A_CRI2			0x043E
#define BRD_PID_KDV8000A_DRI2   		0x043F
#define BRD_PID_KDV8000A_IS21   		0x0440
#define BRD_PID_KDV8000A_IS22  			0x0441 
#define KDV8000E						0x0442
//4.7新版卡  [2/2/2012 chendaiwei]
#define BRD_PID_KDV8000A_HDU2			0x0450
#define BRD_PID_KDV8000A_MPU2			0x044F
#define BRD_PID_KDV8000A_APU2			0x0451
#define BRD_PID_KDV8000A_HDU2_L			0x0460
#define BRD_PID_KDV8000A_HDU2_S			0x0464

#ifdef __cplusplus

}

#endif  /* __cplusplus */



#endif /* _KDV_def_H_ */



/* end of file kdvdef.h */
























