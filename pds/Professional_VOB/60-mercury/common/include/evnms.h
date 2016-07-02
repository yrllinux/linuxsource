/*****************************************************************************
   模块名      : 
   文件名      : evnms.h
   相关文件    : 
   文件实现功能: 
   作者        : 
   版本        : V4.6  Copyright(C) 2009-2012 KDC, All rights reserved.
-----------------------------------------------------------------------------
   修改记录:
   日  期       版本        修改人          修改内容
   2004/10/25   0.9         zhangsh         创建
   2009/08/28   4.6          张宝卿         从卫星会议移植过来    
******************************************************************************/
#ifndef _EV_NMS_H_
#define _EV_NMS_H_

#include "osp.h"
#include "eventid.h"
//MCU向网管登记预约会议信息,消息体为TSchConfInfoHeader + 各终端IP
OSPEVENT( MCU_NMS_SCHEDULE_CONF_REQ,				EV_SATMCUNMS_BGN + 1 );
//网管回应TSchConfNotif + 冲突终端IP
OSPEVENT( NMS_MCU_SCHEDULE_CONF_NOTIF,				EV_SATMCUNMS_BGN + 2 );

//终端入会MCU给网管,TMtOperateToNms
OSPEVENT( MCU_NMS_MTJIONCONF_REQ,				    EV_SATMCUNMS_BGN + 3 );
//网管给MCU的关于终端入会回应，TMtOperateNotif
OSPEVENT( NMS_MCU_MTJIONCONF_NOTIF,				    EV_SATMCUNMS_BGN + 4 );

//终端退会MCU给网管,TMtOperateToNms
OSPEVENT( MCU_NMS_MTQUITCONF_REQ,				    EV_SATMCUNMS_BGN + 5 );
//网管关于退会的MCU回应,TMtOperateNotif
OSPEVENT( NMS_MCU_MTQUITCONF_NOTIF,				    EV_SATMCUNMS_BGN + 6 );

//会议开始时MCU向网管申请频率,TMcuApplyFreq
OSPEVENT( MCU_NMS_APPLYCONFFREQ_REQ,				EV_SATMCUNMS_BGN + 7 );
//网管给MCU申请的回应,TFreqGetMsg
OSPEVENT( NMS_MCU_APPLYCONFFREQ_NOTIF,				EV_SATMCUNMS_BGN + 8 );

//会议结束时释放频率,TMcuReleaseFreq
OSPEVENT( MCU_NMS_RELEASECONFFREQ_REQ,				EV_SATMCUNMS_BGN + 9 );
//网管给MCU释放的回应,TFreqReleaseMsg
OSPEVENT( NMS_MCU_RELEASECONFFREQ_NOTIF,			EV_SATMCUNMS_BGN + 10 );

//更改会议开始时间/延长会议时间,TConfInfoNotif
OSPEVENT( MCU_NMS_RESCHEDCONF_REQ,				    EV_SATMCUNMS_BGN + 11);
//回应,TSchConfNotif
OSPEVENT( NMS_MCU_RESCHEDCONF_NOTIF,				EV_SATMCUNMS_BGN + 12 );


#endif /* _EV_NMS_H_ */
