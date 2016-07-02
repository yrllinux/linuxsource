#ifndef _MTLOGCTRL_H_
#define _MTLOGCTRL_H_

#include "osp.h"
#include "kdvdef.h"
#include "kdvtype.h"

// Log Interface

#define MDL_ALL            0       // ALL module
#define MDL_UI             1       // MT UI module
#define MDL_SERVICE        2       // MT Service module
#define MDL_H323SERVICE    3       // MT H323Service module
#define MDL_MP             4       // MT MP module
#define MDL_MC             5       // MT MC module
#define MDL_DEVICE         6       // MT Device module
#define MDL_AGENT          7       // MT Agent module
#define MDL_SYSAPI         8       // MT Sysapi module
#define MDL_KDVGUI         9       // MT KDVGUI module
#define MDL_CFG            10       // MT Config module




#define MT_LOG_TYPE_NONE   0
#define MT_LOG_TYPE_INFO   0x1
#define MT_LOG_TYPE_WARN   0x2
#define MT_LOG_TYPE_ERR    0x4
#define MT_LOG_TYPE_DEBUG  0x8
#define MT_LOG_TYPE_ALL   0xff


#define MT_LOG_TO_NULL       0x00
#define MT_LOG_TO_STDOUT     0x01
#define MT_LOG_TO_OSP_TELNET 0x02
#define MT_LOG_TO_FILE       0x04
#define MT_LOG_TO_ALL        0xff



extern void MtLogInit();

extern void MtLogInfo(int module, const s8 *fmt, ...);
extern void MtLogWarn(int module, const s8 *fmt, ...);
extern void MtLogErr(int module, const s8 *fmt, ...);
extern void MtLogDebug(int module, const s8 *fmt, ...);


extern void MtLogPrint(int module, int type, const s8 *fmt, va_list arg, const s8 * pTitle);

extern void MtLogTypeFilter(u32 module, u8 typeFilter);
extern void MtLogPrintFuncFilter(u8 filter);

#endif
