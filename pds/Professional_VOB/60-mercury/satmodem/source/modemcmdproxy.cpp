
#include "modeminterface.h"
#include "modemservice.h"
#include "modemcmdproxy.h"

//#include "osp.h"

#define FREQ_70M                (u32)70   * 1000 * 1000
#define FREQ_LBAND              (u32)1273 * 1000 * 1000
#define FREQ_70M_MIN            (u32)50   * 1000 * 1000
#define FREQ_70M_MAX            (u32)90   * 1000 * 1000
#define FREQ_LBAND_MIN          (u32)950  * 1000 * 1000
#define FREQ_LBAND_MAX          (u32)1750 * 1000 * 1000


/* LBAND和70M之间的频率转换 */
static u32 ConvertFreq(u32 freq)
{
	/* 在70M之间，则转换为LBAND */
	if ((freq >= FREQ_70M_MIN) && (freq <= FREQ_70M_MAX))
	{
		freq = FREQ_LBAND + freq - FREQ_70M;
	}
	/* 在LBAND之间，则转换为70M */
	else if ((freq >= FREQ_LBAND_MIN) && (freq <= FREQ_LBAND_MAX))
	{
		u32 diff = FREQ_70M - FREQ_70M_MIN;
		/* 如果频率超出70M的频率范围，则分别置最高或者最低 */
		if (freq > (FREQ_LBAND + diff))
		{
			freq = FREQ_70M_MAX;
		} 
		else if (freq < (FREQ_LBAND - diff))
		{
			freq = FREQ_70M_MIN;
		} else {
			freq = FREQ_70M + freq - FREQ_LBAND;
		}
	} else
	{
		/* 非法频率 */
		freq = 0;
	}
	return freq;
}

ModemCmdProxy::ModemCmdProxy(u32 nip, u16 nport)
{
    m_dwIp = nip;
    m_wPort = nport;
    m_byTxType = 0;
    m_byRxType = 0;
    m_dwTxDefault = 0;
}

int ModemCmdProxy::SetDemBitRate(u32 rate, u8 trynum )
{
    int ret;
    MdcomCmd cmd;
    if (!isvalid()) 
	{
		return MCMD_FAIL;
	}

    MdcomOutput(LVL_INFO, "[SetDemBitRate] DEM: Bitrate(ip: %s\tport: %d)\n", strOfIp(m_dwIp), m_wPort);
	MdcomOutput(LVL_INFO, "[SetDemBitRate] Value: %d\tTrynum: %d\n", rate, trynum);

    cmd.cmd = MDCOM_SET_DEM_BITRATE;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = rate;
    ret = CommandModem(&cmd, trynum);
    if (ret == 0)
	{
		//OspPrintf(TRUE, FALSE, "[SetDemBitRate] CommandModem succeed!\n");
        return MCMD_OK;
	}
    else
	{
		//OspPrintf(TRUE, FALSE, "[SetDemBitRate] CommandModem failed!\n");
        return MCMD_FAIL;
	}
}

int ModemCmdProxy::SetDemBrFr(u32 rate, u32 freq, u8 trynum )
{
    int ret;
    MdcomCmd cmd;

    if (!isvalid() || (freq < FREQ_70M_MIN) || (freq > FREQ_70M_MAX))
	{
        return MCMD_FAIL;
	}

    /* 转换频率 */
    if (m_byRxType == 2)
	{
        freq = ConvertFreq(freq);
	}

    MdcomOutput(LVL_INFO, "[SetDemBrFr] DEM: Bitrate and freq(ip: %s\tport: %d)\n", strOfIp(m_dwIp), m_wPort);
	MdcomOutput(LVL_INFO, "[SetDemBrFr] Bitrate: %d\tFreq: %d\tTrynum: %d\n", rate, freq, trynum);

    /* 先发送设置接收频点的命令 */
    cmd.cmd = MDCOM_SET_DEM_FREQ;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = freq;
    ret = CommandModem(&cmd, trynum);
    if (ret != 0)
	{
        return MCMD_FAIL;
	}
	else
	{
		//OspPrintf(TRUE, FALSE, "[SetDemBrFr] set DEM freq succeed!\n");
	}
    /* 再发送设置接收码率的 */
    cmd.cmd = MDCOM_SET_DEM_BITRATE;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = rate;
    ret = CommandModem(&cmd, trynum);

    if (ret == 0)
	{
		//OspPrintf(TRUE, FALSE, "[SetDemBrFr] set DEM bitrate succeed!\n");
        return MCMD_OK;
	}
    else
	{
		//OspPrintf(TRUE, FALSE, "[SetDemBrFr] set DEM bitrate failed, ret.%d!\n", ret);
        return MCMD_FAIL;
	}
}

int ModemCmdProxy::SetDemClkSrc(u32 src, u8 trynum )
{
    int ret;
    MdcomCmd cmd;

	//struct in_addr addr;
	//addr.s_addr = m_dwIp;
    
    if (!isvalid()) return MCMD_FAIL;

    MdcomOutput(LVL_INFO, "[SetDemClkSrc] DEM: Clock source(ip: %s\tport: %d)\n", strOfIp(m_dwIp), m_wPort);
	MdcomOutput(LVL_INFO, "[SetDemClkSrc] Value: %d\tTrynum: %d\n", src, trynum);

    cmd.cmd = MDCOM_SET_DEM_CLKSRC;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = src;
    ret = CommandModem(&cmd, trynum);

    if (ret == 0)
	{
		//OspPrintf(TRUE, FALSE, "[SetDemClkSrc] succeed!\n");
        return MCMD_OK;
	}
    else
	{
		//OspPrintf(TRUE, FALSE, "[SetDemClkSrc] failed, ret.%d!\n");
        return MCMD_FAIL;
	}
}

int ModemCmdProxy::SetDemFreq(u32 freq, u8 trynum )
{
    int ret;
    MdcomCmd cmd;

    if (!isvalid() || (freq < FREQ_70M_MIN) || (freq > FREQ_70M_MAX))
	{
		//OspPrintf(TRUE, FALSE, "[SetDemFreq] failed due to isvalid.%d\n", isvalid());
        return MCMD_FAIL;
	}

    /* 转换频率 */
    if (m_byRxType == 2)
	{
        freq = ConvertFreq(freq);
	}

    MdcomOutput(LVL_INFO, "[SetDemFreq] DEM: Frequency(ip: %s\tport: %d)\n", strOfIp(m_dwIp), m_wPort);
	MdcomOutput(LVL_INFO, "[SetDemFreq] Value: %d\tTrynum: %d\n", freq, trynum);

    cmd.cmd = MDCOM_SET_DEM_FREQ;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = freq;
    ret = CommandModem(&cmd, trynum);

    if (ret == 0)
	{
		//OspPrintf(TRUE, FALSE, "[SetDemFreq] succeed\n");
        return MCMD_OK;
	}
    else
	{
		//OspPrintf(TRUE, FALSE, "[SetDemFreq] failed, ret.%d\n", ret);
        return MCMD_FAIL;
	}
}

int ModemCmdProxy::SetModBitRate(u32 rate, u8 trynum )
{
    int ret;
    MdcomCmd cmd;
	//struct in_addr addr;
	
	//addr.s_addr = m_dwIp;
    if (!isvalid())
	{
		//OspPrintf(TRUE, FALSE, "[SetModBitRate] failed due to isvalid.%d!\n", isvalid());
		return MCMD_FAIL;
	}

    MdcomOutput(LVL_INFO, "[SetModBitRate] Mod: Bitrate(ip: %s\tport: %d)\n", strOfIp(m_dwIp), m_wPort);
	MdcomOutput(LVL_INFO, "[SetModBitRate] Value: %d\tTrynum: %d\n", rate, trynum);

    cmd.cmd = MDCOM_SET_MOD_BITRATE;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = rate;
    ret = CommandModem(&cmd, trynum);

    if (ret == 0)
	{
		//OspPrintf(TRUE, FALSE, "[SetModBitRate] succeed!\n");
        return MCMD_OK;
	}
    else
	{
		//OspPrintf(TRUE, FALSE, "[SetModBitRate] failed, ret.%d!\n", ret);
        return MCMD_FAIL;
	}
}

int ModemCmdProxy::SetModBrFr(u32 rate, u32 freq, u8 trynum )
{
    int ret;
    MdcomCmd cmd;
	//struct in_addr addr;
	
	//addr.s_addr = m_dwIp;
    if (!isvalid() || (freq < FREQ_70M_MIN) || (freq > FREQ_70M_MAX)) 
	{
		//OspPrintf(TRUE, FALSE, "[SetModBrFr] failed due to isvalid.%d, freq.%d!\n", isvalid(), freq);
        return MCMD_FAIL;
	}
    
	/* 转换频率 */
    if (m_byTxType == 2)
	{
        freq = ConvertFreq(freq);
	}
    MdcomOutput(LVL_INFO, "[SetModBrFr] Mod: Bitrate and freq(ip: %s\tport: %d)\n", strOfIp(m_dwIp), m_wPort);
	MdcomOutput(LVL_INFO, "[SetModBrFr] Bitrate: %d\tFreq: %d\tTrynum: %d\n", rate, freq, trynum);

    /* 先发送设置发送频点的命令 */
    cmd.cmd = MDCOM_SET_MOD_FREQ;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = freq;
    ret = CommandModem(&cmd, trynum);
    if (ret != 0)
	{
		//OspPrintf(TRUE, FALSE, "[SetModeBrFr] set send Freq failed, ret.%d\n", ret);
        return MCMD_FAIL;
	}
	else
	{
		//OspPrintf(TRUE, FALSE, "[SetModeBrFr] set send Freq succeed!\n");
	}

    /* 再发送设置发送码率的 */
    cmd.cmd = MDCOM_SET_MOD_BITRATE;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = rate;
    ret = CommandModem(&cmd, trynum);

    if (ret == 0)
	{
		//OspPrintf(TRUE, FALSE, "[SetModeBrFr] set send br succeed!\n");
        return MCMD_OK;
	}
    else
	{
		//OspPrintf(TRUE, FALSE, "[SetModeBrFr] set send Freq failed, ret.%d\n", ret);
        return MCMD_FAIL;
	}
}

int ModemCmdProxy::SetModClkSrc(u32 src, u8 trynum )
{
    int ret;
    MdcomCmd cmd;

    if (!isvalid())
	{
		return MCMD_FAIL;
	}

    MdcomOutput(LVL_INFO, "[SetModClkSrc] Mod: Clock source(ip: %s\tport: %d)\n", strOfIp(m_dwIp), m_wPort);
	MdcomOutput(LVL_INFO, "[SetModClkSrc] Value: %d\tTrynum: %d\n", src, trynum);

    cmd.cmd = MDCOM_SET_MOD_CLKSRC;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = src;
    ret = CommandModem(&cmd, trynum);

    if (ret == 0)
	{
		//OspPrintf(TRUE, FALSE, "[SetModClkSrc] CommandModem succeed!\n");
        return MCMD_OK;
	}
    else
	{
// 		OspPrintf(TRUE, FALSE, "[SetModClkSrc] CommandModem failed, cmd<ip.0x%x, port.%d, cmd.%d, val.%d>, ret.%d \n",
// 			cmd.ip,
// 			cmd.port,
// 			cmd.cmd,
// 			cmd.val,
// 			ret);
        return MCMD_FAIL;
	}
}

int ModemCmdProxy::SetModFreq(u32 freq, u8 trynum )
{
    int ret;
    MdcomCmd cmd;

    if (!isvalid() || (freq < FREQ_70M_MIN) || (freq > FREQ_70M_MAX)) 
	{
        return MCMD_FAIL;
	}

    /* 转换频率 */
    if (m_byTxType == 2)
	{
        freq = ConvertFreq(freq);
	}

    MdcomOutput(LVL_INFO, "[SetModFreq] Mod: Frequency(ip: %s\tport: %d)\n", strOfIp(m_dwIp), m_wPort);
	MdcomOutput(LVL_INFO, "[SetModFreq] Value: %d\tTrynum: %d\n", freq, trynum);

    cmd.cmd = MDCOM_SET_MOD_FREQ;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = freq;
    ret = CommandModem(&cmd, trynum);

    if (ret == 0)
	{
        return MCMD_OK;
	}
    else
	{
        return MCMD_FAIL;
	}
}

int ModemCmdProxy::SetModOutput(BOOL32 enable, BOOL32 reset, u8 trynum )
{
    int ret;
    MdcomCmd cmd;

    if (!isvalid()) return MCMD_FAIL;

    MdcomOutput(LVL_INFO, "[SetModOutput] Mod: Output(ip: %s\tport: %d)\n", strOfIp(m_dwIp), m_wPort);
	MdcomOutput(LVL_INFO, "[SetModOutput] Value: %d\tTrynum: %d\n", enable, trynum);

    if (reset)
    {
        cmd.cmd = MDCOM_SET_MOD_FREQ;
        cmd.ip = m_dwIp;
        cmd.port = m_wPort;
        cmd.val = m_dwTxDefault;
        ret = CommandModem(&cmd, trynum);
		if (ret !=0 )
		{
			return MCMD_FAIL;
		}
    }

    cmd.cmd = MDCOM_SET_MOD_OUTPUT;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;

    if (enable)
	{
        cmd.val = 0x01;
	}
    else
	{
        cmd.val = 0x00;
	}
    ret = CommandModem(&cmd, trynum);  

    if (ret == 0)
	{
        return MCMD_OK;
	}
    else
	{
        return MCMD_FAIL;
	}
}

int ModemCmdProxy::QueryDemBitRate(u8 trynum )
{
    int ret;
    MdcomCmd cmd;

    if (!isvalid()) return MCMD_FAIL;

    MdcomOutput(LVL_INFO, "[QueryDemBitRate] Query Mod: Output(ip: %s\tport: %d)\n", strOfIp(m_dwIp), m_wPort);

    cmd.cmd = MDCOM_QUERY_DEM_BITRATE;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CommandModem(&cmd, trynum);

    if (ret == 0)
        return MCMD_OK;
    else
        return MCMD_FAIL;
}

int ModemCmdProxy::QueryDemClkSrc(u8 trynum )
{
    int ret;
    MdcomCmd cmd;

    if (!isvalid()) return MCMD_FAIL;

    MdcomOutput(LVL_INFO, "[QueryDemClkSrc] Query Mod: Output(ip: %s\tport: %d)\n", strOfIp(m_dwIp), m_wPort);

    cmd.cmd = MDCOM_QUERY_DEM_CLKSRC;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CommandModem(&cmd, trynum);

    if (ret == 0)
        return MCMD_OK;
    else
        return MCMD_FAIL;
}

int ModemCmdProxy::QueryDemFreq(u8 trynum )
{
    int ret;
    MdcomCmd cmd;

    if (!isvalid()) return MCMD_FAIL;

    MdcomOutput(LVL_INFO, "[QueryDemFreq] Query Mod: Output(ip: %s\tport: %d)\n", strOfIp(m_dwIp), m_wPort);

    cmd.cmd = MDCOM_QUERY_DEM_FREQ;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CommandModem(&cmd, trynum);

    if (ret == 0)
        return MCMD_OK;
    else
        return MCMD_FAIL;
}

int ModemCmdProxy::QueryModBitRate(u8 trynum )
{
    int ret;
    MdcomCmd cmd;
    if (!isvalid()) return MCMD_FAIL;

    MdcomOutput(LVL_INFO, "[QueryModBitRate] Query Mod: Output(ip: %s\tport: %d)\n", strOfIp(m_dwIp), m_wPort);

    cmd.cmd = MDCOM_QUERY_MOD_BITRATE;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CommandModem(&cmd, trynum);

    if (ret == 0)
        return MCMD_OK;
    else
        return MCMD_FAIL;
}

int ModemCmdProxy::QueryModClkSrc(u8 trynum )
{
    int ret;
    MdcomCmd cmd;

    if (!isvalid()) return MCMD_FAIL;

    MdcomOutput( LVL_INFO, "[QueryModClkSrc] Query Mod: Output(ip: %s\tport: %d)\n", strOfIp(m_dwIp), m_wPort);

    cmd.cmd = MDCOM_QUERY_MOD_CLKSRC;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CommandModem(&cmd, trynum);

    if (ret == 0)
        return MCMD_OK;
    else
        return MCMD_FAIL;
}

int ModemCmdProxy::QueryModFreq(u8 trynum )
{
    int ret;
    MdcomCmd cmd;
    
	if (!isvalid()) return MCMD_FAIL;

    MdcomOutput(LVL_INFO, "[QueryModFreq] Query Mod: Output(ip: %s\tport: %d)\n", strOfIp(m_dwIp), m_wPort);

    cmd.cmd = MDCOM_QUERY_MOD_FREQ;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CommandModem(&cmd, trynum);

    if (ret == 0)
        return MCMD_OK;
    else
        return MCMD_FAIL;
}

int ModemCmdProxy::QueryModOutput(u8 trynum )
{
    int ret;
    MdcomCmd cmd;

    if (!isvalid()) return MCMD_FAIL;

    MdcomOutput(LVL_INFO, "[QueryModOutput] Query Mod: Output(ip: %s\tport: %d)\n", strOfIp(m_dwIp), m_wPort);

    cmd.cmd = MDCOM_QUERY_MOD_OUTPUT;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CommandModem(&cmd, trynum);

    if (ret == 0)
        return MCMD_OK;
    else
        return MCMD_FAIL;
}

int ModemCmdProxy::CheckDemBitRate(u32 &rate)
{
    int ret;
    MdcomCmd cmd;
    if (!isvalid()) return MCMD_FAIL;

    cmd.cmd = MDCOM_SET_DEM_BITRATE;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CheckCmdStatus(&cmd);

    rate = cmd.val;
    return ret;
}

int ModemCmdProxy::CheckDemBrFr(u32 &rate, u32 &freq)
{
    int ret;
    MdcomCmd cmd;
    if (!isvalid()) return MCMD_FAIL;

    cmd.cmd = MDCOM_SET_DEM_BITRATE;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CheckCmdStatus(&cmd);
    rate = cmd.val;
    if (ret != MCMD_OK)
        return ret;

    cmd.cmd = MDCOM_SET_DEM_FREQ;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CheckCmdStatus(&cmd);
    freq = cmd.val;

    /* 转换频率 */
    if ((freq != 0) && (m_byRxType == 2))
		freq = ConvertFreq(freq);
    
    return ret;
}

int ModemCmdProxy::CheckDemClkSrc(u32 &src)
{
    int ret;
    MdcomCmd cmd;
    if (!isvalid()) return MCMD_FAIL;

    cmd.cmd = MDCOM_SET_DEM_CLKSRC;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CheckCmdStatus(&cmd);

    src = cmd.val;
    return ret;
}

int ModemCmdProxy::CheckDemFreq(u32 &freq)
{
    int ret;
    MdcomCmd cmd;
    if (!isvalid()) return MCMD_FAIL;

    cmd.cmd = MDCOM_SET_DEM_FREQ;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CheckCmdStatus(&cmd);
    freq = cmd.val;

    /* 转换频率 */
    if ((freq != 0) && (m_byRxType == 2))
	    freq = ConvertFreq(freq);
    return ret;
}

int ModemCmdProxy::CheckModBitRate(u32 &rate)
{
    int ret;
    MdcomCmd cmd;
    if (!isvalid()) return MCMD_FAIL;

    cmd.cmd = MDCOM_SET_MOD_BITRATE;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CheckCmdStatus(&cmd);

    rate = cmd.val;
    return ret;
}

int ModemCmdProxy::CheckModBrFr(u32 &rate, u32 &freq)
{
    int ret;
    MdcomCmd cmd;
    if (!isvalid()) return MCMD_FAIL;


    cmd.cmd = MDCOM_SET_MOD_BITRATE;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CheckCmdStatus(&cmd);
    rate = cmd.val;
    if (ret != MCMD_OK)
        return ret;

    cmd.cmd = MDCOM_SET_MOD_FREQ;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CheckCmdStatus(&cmd);
    freq = cmd.val;

    /* 转换频率 */
    if ((freq != 0) && (m_byTxType == 2))
		freq = ConvertFreq(freq);
    
    return ret;
}

int ModemCmdProxy::CheckModClkSrc(u32 &src)
{
    int ret;
    MdcomCmd cmd;
    if (!isvalid())
	{
		return MCMD_FAIL;
	}
    cmd.cmd = MDCOM_SET_MOD_CLKSRC;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CheckCmdStatus(&cmd);

    src = cmd.val;
    return ret;
}

int ModemCmdProxy::CheckModFreq(u32 &freq)
{
    int ret;
    MdcomCmd cmd;
    if (!isvalid()) return MCMD_FAIL;

    cmd.cmd = MDCOM_SET_MOD_FREQ;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CheckCmdStatus(&cmd);
    freq = cmd.val;

    /* 转换频率 */
    if ((freq != 0) && (m_byTxType == 2))
		freq = ConvertFreq(freq);
	return ret;
}

int ModemCmdProxy::CheckModOutput(BOOL32 &enable)
{
    int ret;
    MdcomCmd cmd;
    if (!isvalid()) return MCMD_FAIL;

    cmd.cmd = MDCOM_SET_MOD_OUTPUT;
    cmd.ip = m_dwIp;
    cmd.port = m_wPort;
    cmd.val = 0;
    ret = CheckCmdStatus(&cmd);
    if (cmd.val)
	{
        enable = TRUE;
	}
    else
	{
        enable = FALSE;
	}
    return ret;
}

void ModemCmdProxy::SetModemIp(u32 nip, u16 nport)
{
    m_dwIp = nip;
    m_wPort = nport;
}

void ModemCmdProxy::SetModemType(u8 type)
{
    switch(type)
    {
    case MODEM_TYPE_70M_70M:
        m_byTxType = 1;
        m_byRxType = 1;
        m_dwTxDefault = FREQ_70M_MIN;
        break;
    case MODEM_TYPE_70M_LBAND:
        m_byTxType = 1;
        m_byRxType = 2;
        m_dwTxDefault = FREQ_70M_MIN;
        break;
    case MODEM_TYPE_LBAND_70M:
        m_byTxType = 2;
        m_byRxType = 1;
        m_dwTxDefault = FREQ_LBAND_MIN;
        break;
    case MODEM_TYPE_LBAND_LBAND:
        m_byTxType = 2;
        m_byRxType = 2;
        m_dwTxDefault = FREQ_LBAND_MIN;
        break;
    default:
        m_byTxType = 0;
        m_byRxType = 0;
        m_dwTxDefault = 0;
    }
}

bool ModemCmdProxy::isvalid()
{

	bool bRet = ((m_dwIp != 0) && (m_wPort != 0) && (m_dwTxDefault != 0) &&
            (m_byTxType != 0) && (m_byRxType != 0));

	if (!bRet)
	{
	}
	return bRet;
}

s8 * strOfIp( u32 dwIP )
{
    static char strIP[17];  
    u8 *p = (u8 *)&dwIP;
    sprintf(strIP,"%d.%d.%d.%d%c",p[0],p[1],p[2],p[3],0);
    return strIP;
}


s8 * strOfCmd( u32 dwCmd )
{
	static s8 * pchCmdName[] = {"mod_freq",
								"mod_output",
								"mod_bitrate", 
								"mod_clksrc",
								"dem_freq",
								"dem_bitrate",
								"dem_clksrc",
								"unknow"};

	if (dwCmd > MDCOM_SET_MAX)
	{
		dwCmd = MDCOM_SET_MAX;
	}

	return pchCmdName[dwCmd];
}