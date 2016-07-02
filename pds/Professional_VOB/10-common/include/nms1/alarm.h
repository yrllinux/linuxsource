#ifndef NMS_ALARM_H_
#define NMS_ALARM_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "nmsmacro.h"

#define ALARM_VALUE_MAX_LEN         (s32)16
//告警
class CAlarm : public CObject
{
public:
    u32	    m_dwDeviceID;           // 设备编号
    u32	    m_dwAlarmCode;          // 告警码
    TCHAR   m_szAlarmValue[ALARM_VALUE_MAX_LEN];    //告警值
    TCHAR   m_szGenTime[TIME_LEN];  // 告警产生时间
    TCHAR   m_szResTime[TIME_LEN];  // 告警恢复时间
    bool    m_bIsChecked;
    u8      m_byAlarmType;          // 新产生的告警/告警恢复
    u8      m_byObjectTypeId;       // 告警对象类型Id

public:
    CAlarm() : m_dwDeviceID(0), m_dwAlarmCode(0), m_bIsChecked(false),
               m_byAlarmType(0), m_byObjectTypeId(0)
	{
        ZeroMemory( m_szAlarmValue, sizeof (m_szAlarmValue) );
        ZeroMemory( m_szGenTime,    sizeof (m_szGenTime) );
        ZeroMemory( m_szResTime,    sizeof (m_szResTime) );
	};

    CAlarm(const CAlarm &cAlarm) : m_dwDeviceID(cAlarm.m_dwDeviceID),
                                   m_dwAlarmCode(cAlarm.m_dwAlarmCode),
                                   m_bIsChecked(cAlarm.m_bIsChecked),
                                   m_byAlarmType(cAlarm.m_byAlarmType),
                                   m_byObjectTypeId(cAlarm.m_byObjectTypeId)
    {
		_tcsncpy( m_szAlarmValue, cAlarm.m_szAlarmValue,
                  sizeof (m_szAlarmValue) - 1 );
        _tcsncpy( m_szGenTime, cAlarm.m_szGenTime, sizeof (m_szGenTime) - 1 );
        m_szAlarmValue[sizeof (m_szAlarmValue) - 1] = 0;
        m_szGenTime[sizeof (m_szGenTime) - 1] = 0;
		//  原有代码里没有给恢复时间赋值
    };

public:
	CAlarm & operator= (const CAlarm &cAlarm)
	{
        if ( this == &cAlarm )  //  wanghao 2005/11/03 防止自赋值
        {
            return (*this);
        }

		m_dwDeviceID        = cAlarm.m_dwDeviceID;
		m_dwAlarmCode       = cAlarm.m_dwAlarmCode;
		_tcsncpy( m_szAlarmValue, cAlarm.m_szAlarmValue,
                  sizeof (m_szAlarmValue) - 1 );
        _tcsncpy( m_szGenTime, cAlarm.m_szGenTime, sizeof (m_szGenTime) - 1 );
        m_szAlarmValue[sizeof (m_szAlarmValue) - 1] = 0;
        m_szGenTime[sizeof (m_szGenTime) - 1] = 0;
		//  原有代码里没有给恢复时间赋值
		m_bIsChecked        = cAlarm.m_bIsChecked;
        m_byAlarmType       = cAlarm.m_byAlarmType;
		m_byObjectTypeId    = cAlarm.m_byObjectTypeId;

		return (*this);
	}

	void Dump() const
	{
		OspLog( APPLICATION,
                "AlarmType = %d, EquipmentType = %d, DeviceID = %d, AlarmValue = %s, AlarmCode = %d, alarmTime = %s ",
                m_byAlarmType, m_byObjectTypeId, m_dwDeviceID, m_szAlarmValue,
                m_dwAlarmCode, m_szGenTime );
	}

    void Dump1() const
    {
		OspPrintf( TRUE, FALSE,
                   "AlarmType = %d, EquipmentType = %d, DeviceID = %d, AlarmValue = %s, AlarmCode = %d, alarmTime = %s ",
                   m_byAlarmType, m_byObjectTypeId, m_dwDeviceID,
                   m_szAlarmValue, m_dwAlarmCode, m_szGenTime );
    }

    /*=============================================================================
    函 数 名:Print
    功    能:内容打印
    参    数:无
    注    意:无
    返 回 值:无
    -------------------------------------------------------------------------------
    修改纪录:
    日      期  版本    修改人  修改内容
    2006/03/30  4.0     王昊    创建
    =============================================================================*/
    void Print(void) const
    {
        ::OspPrintf( TRUE, FALSE, "AlarmType = %d, ObjType = %d, DeviceID = %d, \
Value = %s\nAlarmCode = %d, GenTime = %s, ResTime = %s\n", m_byAlarmType,
                     m_byObjectTypeId, m_dwDeviceID, m_szAlarmValue,
                     m_dwAlarmCode, m_szGenTime, m_szResTime );
    }
};

typedef CTypedPtrList<CObList, CAlarm *> CAlarmList;//Alarm列表

#endif