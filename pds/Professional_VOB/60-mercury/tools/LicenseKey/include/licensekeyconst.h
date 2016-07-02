#ifndef _LICENSEKEYNAME_H_
#define _LICENSEKEYNAME_H_

// 必须的KEY值 //////////////////////////////////////////////////////////
#define KEY_LICENSE_VERSION		(s8*)"KEY_LICENSE_VERSION"		// 许可证版本
#define KEY_LICENSE_TYPE		(s8*)"KEY_LICENSE_TYPE"			// 许可证类型
/////////////////////////////////////////////////////////////////////////

// 普通KEY值 ////////////////////////////////////////////////////////////
//MCU
#define KEY_MCU_DEVICE_ID		(s8*)"KEY_MCU_DEVICE_ID"		// MCU设备ID
#define KEY_MCU_TYPE			(s8*)"KEY_MCU_TYPE"			    // MCU类型
#define KEY_MCU_ACCESS_NUM		(s8*)"KEY_MCU_ACCESS_NUM"		// MCU终端接入数量
#define KEY_MCU_EXP_DATE		(s8*)"KEY_MCU_EXP_DATE"			// MCU失效时间

//MT
#define KEY_MT_DEVICE_ID		(s8*)"KEY_MT_DEVICE_ID"		    // MT设备ID
#define KEY_RUN_KDMMT			(s8*)"KEY_RUN_KDMMT"			// 是否运行终端监控
#define KEY_MT_MODAL			(s8*)"KEY_MT_MODAL"			    // 终端类型
#define KEY_MT_ACCESS_NUM		(s8*)"KEY_MT_ACCESS_NUM"		// 终端MC接入数量
#define KEY_MT_RUNPROXY         (s8*)"KEY_MT_RUNPROXY"  		// 启用内置防火墙代理
#define KEY_MT_RUNGK            (s8*)"KEY_MT_RUNGK"      		// 启用内置GK

//T120
#define KEY_T120_ACCESS_NUM		(s8*)"KEY_T120_ACCESS_NUM"		// T120接入数量
#define KEY_T120_DEVICE_ID		(s8*)"KEY_T120_DEVICE_ID"		// T120设备ID
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#define LICENSE_TYPE			(s8*)"KDVMCU"				    // License类型
#define LICENSE_VERSION			(s8*)"4.0R4"				    // License版本
//////////////////////////////////////////////////////////////////////////


#define STRING_YES              (s8*)"YES"
#define STRING_NO               (s8*)"NO"
#endif