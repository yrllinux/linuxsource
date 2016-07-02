#ifndef _LICENSEKEYNAME_H_
#define _LICENSEKEYNAME_H_

// 必须的KEY值 //////////////////////////////////////////////////////////
#define KEY_LICENSE_VERSION		(s8*)"KEY_LICENSE_VERSION"		// 许可证版本
#define KEY_LICENSE_TYPE		(s8*)"KEY_LICENSE_TYPE"			// 许可证类型
/////////////////////////////////////////////////////////////////////////

// 普通KEY值 ////////////////////////////////////////////////////////////
//MCU
#define KEY_MCU_DEVICE_ID		(s8*)"KEY_MCU_DEVICE_ID"		// MCU设备ID
#define KEY_MCU_TYPE			(s8*)"KEY_MCU_TYPE"			// MCU类型
#define KEY_MCU_ACCESS_NUM		(s8*)"KEY_MCU_ACCESS_NUM"		// MCU终端接入数量
#define KEY_MCU_EXP_DATE		(s8*)"KEY_MCU_EXP_DATE"			// MCU失效时间
#define KEY_MCU_VCS_ACCESS_NUM  (s8*)"KEY_MCU_VCS_ACCESS_NUM"   // MCU允许接入的VCS数
#define KEY_MCU_ACCESS_HD_MT_NUM (s8*)"KEY_MCU_ACCESS_HD_MT_NUM"// MCU高清终端接入数
#define KEY_MCU_ACCESS_PCMT_NUM (s8*)"KEY_MCU_ACCESS_PCMT_NUM"	// MCU PCMT接入数
#define	KEY_MCU_ACCESS_AUDIO_NUM (s8*)"KEY_MCU_ACCESS_AUDIO_NUM" // MCU语音终端接入数

//MT
#define KEY_RUN_KDMMT			(s8*)"KEY_RUN_KDMMT"			// 是否运行终端监控
#define KEY_MT_MODAL			(s8*)"KEY_MT_MODAL"			// 终端类型
#define KEY_MT_ACCESS_NUM		(s8*)"KEY_MT_ACCESS_NUM"		// 终端MC接入数量		

//T120
#define KEY_T120_ACCESS_NUM		(s8*)"KEY_T120_ACCESS_NUM"		// T120接入数量
#define KEY_T120_DEVICE_ID		(s8*)"KEY_T120_DEVICE_ID"		// T120设备ID
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#define LICENSE_TYPE			(s8*)"KDVMCU"				// License类型
#define LICENSE_VERSION			(s8*)"4.0R4"				// License版本
//////////////////////////////////////////////////////////////////////////

//[liu lijiu][2010/10/28]增加8000G版本号
//////////////////////////////////////////////////////////////////////////
#define LICENSE_TYPE_8KE			(s8*)"KDV8000G"				// 8000G License类型
#define LICENSE_VERSION_8KE			(s8*)"V4.6"				    // 8000G License版本
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#define LICENSE_TYPE_HDI		(s8*)"KDVHDI"				// License类型
#define LICENSE_VERSION_HDI		(s8*)"V4.5"				// License版本
#define KEY_HDI_ACCESS_NUM		(s8*)"KEY_HDI_ACCESS_NUM"	// HDI MT接入数量
#define KEY_HDI_DEVICE_ID		(s8*)"KEY_HDI_DEVICE_ID"	//HDI 设备序列号 
//////////////////////////////////////////////////////////////////////////

#endif
