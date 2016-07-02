#ifndef _MTSUBDAVINCI_H_
#define _MTSUBDAVINCI_H_

#include "osp.h"

#define  EV_SubDavinciBrdInitCmd         100
#define  EV_CfgSubDavinciEthernetCmd     101   //设置从davinci的主ip
#define  EV_CfgSubDavinciSecEthernetCmd  102   //设置从davinci的第二个ip
#define  EV_DelSubDavinciSecEthernetCmd  103   //删除从davinci的第二个ip
#define  EV_CfgSavePxySrvCfgCmd          104   //保存Pxy服务器配置文件
#define  EV_RunGkAndPxyCmd               105   //启用Gk和Pxy的应用

#define  EV_SubDavinciIpConflictInitCmd  106   //启动从davinci的ip冲突检测
#define  EV_SubDavinciIpConflictInd      107   //从davinci ip冲突检测指示

#define  EV_SubDavinciLoopTestCmd        108   //用于测试主从消息回环
#define  EV_SubDavinciLoopTestInd        109   //用于测试主从消息回环

#define  EV_SubDavinciAddDefaultGWCmd    110   //增加从davinci默认网关
#define  EV_SubDavinciDelDefaultGWCmd    111   //删除从davinci默认网关

#define  MAX_MULTINETSEGMENTPXY_NUM      5     //多网段代理数目

/************************************************************************/
/* 内置代理，多网段接入的配置                                           */
/************************************************************************/
typedef struct tagTSubPxyIPCfg
{
    BOOL bUsed;
    u32  dwIP;         //IP地址
    u32  dwMask;       //子网掩码
    u32  dwGateway;    //网关地址
}TSubPxyIPCfg, *PTSubPxyIPCfg;

API BOOL mtSubDavinciEventCallBack(u32 dwEventId, void* pContent, u32 dwContentLen);

//Proxy server
class CSubPxySrvCfgEx
{
private:    
    //[StarInfo]
    u8        m_byStartPxy;      //是否启动pxy
    u8        m_byStartMp;       //是否启动mp
    //[LocalInfo]
    u32       m_dwLocalIpAddr;   //server地址
    u32       m_dwLocalRasPort;  //server H323的信令端口
    u32       m_dwLocalH245Port; 
    u32       m_dwLocalOspPort;  
    //clinet和server之间的码流端口
    u32       m_dwLocalRtpPort;       
    u32       m_dwLocalRtcpPort;      
    u32       m_dwLocalMediaBasePort; 
    //[GKInfo]
    u32       m_dwGKIpAddr;      //GK地址
    u32       m_dwGKRasPort;     //GK端口
    //[DefaultGW]
    u32       m_dwGateWay;       //网关地址
    TSubPxyIPCfg m_atPxyIPCfg[MAX_MULTINETSEGMENTPXY_NUM]; //多网段接入配置
public:
    BOOL   m_bInit;
    static CSubPxySrvCfgEx* GetInstance();
    
protected:
    CSubPxySrvCfgEx()
    {
        m_bInit = FALSE;
        m_byStartPxy = 1;
        m_byStartMp  = 1;
        m_dwLocalIpAddr = inet_addr("127.0.0.1");
        m_dwLocalRasPort  = 1819;
        m_dwLocalH245Port = 1820;
        m_dwLocalOspPort  = 2776;
        m_dwLocalRtpPort  = 2776;
        m_dwLocalRtcpPort = 2777;
        m_dwLocalMediaBasePort = 48000;
        m_dwGKIpAddr = inet_addr("0.0.0.0");
        m_dwGKRasPort = 1719;
        memset( &m_atPxyIPCfg[0], 0, sizeof(m_atPxyIPCfg) );
    }
    BOOL srvCfgPathExist(const s8* path)
    {
		BOOL bRet = TRUE;
#ifdef _LINUX_
        s8  achCurPath[256] ={0};
        s8* pStr = getcwd(achCurPath,sizeof(achCurPath));
        if(NULL == pStr) return FALSE;
        bRet= (chdir(path) >= 0);
        chdir(achCurPath);
#endif
        return bRet;
    }
    
    BOOL srvCfgMakeDir(const s8* path )
    {
        int nRet = 0;
        
#ifdef WIN32
        nRet = CreateDirectory( path, 0 );
#endif
#ifdef _VXWORKS_
        nRet = mkdir( path  );
#endif
#ifdef _LINUX_
        if(srvCfgPathExist(path)) return FALSE;
        
        nRet = mkdir(path,  S_IRUSR//文件所有者具可读取权限
            |S_IWUSR//文件所有者具可写入权限
            |S_IXUSR//文件所有者具可执行权限
            |S_IRGRP// 用户组具可读取权限
            |S_IWGRP//用户组具可写入权限
            |S_IXGRP//用户组具可执行权限
            |S_IROTH//其他用户具可读取权限
            |S_IWOTH//其他用户具可写入权限
            |S_IXOTH);//他用户具可执行权限
#endif
        if(nRet < 0)
            printf("%s created failled,error no. = %d\n",path, nRet);
        else
            printf("%s created success\n",path);        
        return TRUE;
    }

public:
    BOOL ReadFromFile();
    BOOL SaveToFile();	
    BOOL SetPxySrvInfo(u32 dwLocalIp, u32 dwGkIp, u32 dwGateway);
    //多网段接入
	BOOL SetPxyIPCfg(TSubPxyIPCfg atPxyIPCfg_old[MAX_MULTINETSEGMENTPXY_NUM], TSubPxyIPCfg atPxyIPCfg_new[MAX_MULTINETSEGMENTPXY_NUM]);
	//设置代理的本地ip, 也是需要配置网关的
	BOOL SetPxyLocalIPCfg(TSubPxyIPCfg tPxy_old, TSubPxyIPCfg tPxy_new );
	//GK的ip的配置
	BOOL SetPxyGKCfg( u32 dwGKIP);
	void CommonSaveToFile();
	void PxyLocalIPSave();
	void PxyIPArraySave(); 
};

#endif





















