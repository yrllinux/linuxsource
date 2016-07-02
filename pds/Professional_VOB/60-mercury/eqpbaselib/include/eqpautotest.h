/*****************************************************************************
   模块名      : eqpbase
   文件名      : eqpautotest.h
   相关文件    : 
   文件实现功能: 外设共用生产测试
   作者        :  周嘉麟
   版本        :  1.0
   日期		   :  2012/02/14
-----------------------------------------------------------------------------
******************************************************************************/

#ifndef _EQP_AUTOTEST_H_
#define _EQP_AUTOTEST_H_

#define VERSION_LEN             32
#define MAX_SERIAL_LENGTH	    32
#define MAX_MAC_LENGTH		    32
#define MAX_VERSION_LENGTH	    32
class CDeviceInfo
{
private:
    s8          achSerial[MAX_SERIAL_LENGTH];           //序列号
    s8          achMac[MAX_MAC_LENGTH];                 //mac地址
    s8          achSoftVersion[MAX_VERSION_LENGTH];     //软件版本
    s8          achHardVersion[MAX_VERSION_LENGTH];     //硬件版本
    u32         dwIp;                                   //设备ip
    u32         dwSubMask;                              //子网掩码
public:
    u32 getSubMask()
    {
        return dwSubMask;
    }
	
    void setSubMask(u32 dwMask)
    {
        dwSubMask = dwMask;
    }
	
    void setSerial(s8 * buf)
    {
        if(buf == NULL) return;
        memset(achSerial,0,MAX_SERIAL_LENGTH);
        int length = (strlen(buf) >= MAX_SERIAL_LENGTH -1 ? MAX_SERIAL_LENGTH - 1: strlen(buf));
        memcpy(achSerial,buf,length);
        achSerial[length] = '\0';
    }    
    s8* getSerial()
    {
        return achSerial;
    }
    
    void setIp(u32 IP)
    {
        dwIp = IP;
    }
    
    u32 getIp()
    {
        return dwIp;
    }
	
    s8* getMac()
    {
        return achMac;
    } 
    void setMac(s8* pchbuf)
    {
        if(pchbuf == NULL) return;
        memset(achMac,0,MAX_VERSION_LENGTH);
        int length = (strlen(pchbuf) >= MAX_MAC_LENGTH -1 ? MAX_MAC_LENGTH - 1: strlen(pchbuf));
        memcpy(achMac, pchbuf, length);
    }
	
    void setSoftVersion(s8* buf)
    {
        if(buf == NULL) return;
        memset(achSoftVersion, 0, MAX_VERSION_LENGTH);
        int length = (strlen(buf) >= MAX_VERSION_LENGTH -1 ? MAX_VERSION_LENGTH - 1 : strlen(buf));
        memcpy(achSoftVersion, buf, length);
        achSoftVersion[length] = '\0';
    }
    s8* getSoftVersion()
    {
        return achSoftVersion;
    }
    void setHardVersion(s8 * buf)
    {
        if(buf == NULL) return;
        memset(achHardVersion, 0, MAX_VERSION_LENGTH);
        int length = (strlen(buf) >= MAX_VERSION_LENGTH -1 ? MAX_VERSION_LENGTH - 1 : strlen(buf));
        memcpy(achHardVersion, buf, length);
        achHardVersion[length] = '\0';
    }    
    s8* getHardVersion()
    {
        return achHardVersion;
    }
};
#endif