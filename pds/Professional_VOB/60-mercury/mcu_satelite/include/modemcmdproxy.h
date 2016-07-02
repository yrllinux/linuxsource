#ifndef _MODEM_CMD_PROXY_H
#define _MODEM_CMD_PROXY_H

#define MCMD_OK                 0               //成功
#define MCMD_FAIL               -1              //失败
#define MCMD_UNSURE             -2              //正在执行中

#if defined(_MSC_VER)       // for Microsoft c++
#define API extern "C"  __declspec(dllexport)
#else                       // for gcc
#define API extern "C"
#endif

#define MOD_CLKSRC_RCV					3								//发送：从时钟源
#define MOD_CLKSRC_INTER				0								//发送：主时钟源
#define DEM_CLKSRC_RCV					0								//接收：从时钟源
#define DEM_CLKSRC_INTER				1

enum TModemType
{
    MODEM_TYPE_70M_70M,              
    MODEM_TYPE_70M_LBAND,            
    MODEM_TYPE_LBAND_70M,            
    MODEM_TYPE_LBAND_LBAND,          
};

class ModemCmdProxy
{
public:
    ModemCmdProxy(u32 ip = 0, u16 port = 0);

    int SetDemBitRate(u32 rate, u8 trynum = 0);                //设置接收码率
    int CheckDemBitRate(u32 &rate);
	int QueryDemBitRate(u8 trynum = 0);
    int SetDemClkSrc(u32 src, u8 trynum = 0);                  //设置接收时钟源
    int CheckDemClkSrc(u32 &src);
	int QueryDemClkSrc(u8 trynum = 0);
    int SetDemFreq(u32 freq, u8 trynum = 0);                   //设置接收频点
    int CheckDemFreq(u32 &freq);
	int QueryDemFreq(u8 trynum = 0);
    int SetDemBrFr(u32 rate, u32 freq, u8 trynum = 0);         //设置接收码率和频点
    int CheckDemBrFr(u32 &rate, u32 &freq);

    int SetModBitRate(u32 rate, u8 trynum = 0);                //设置发送码率
    int CheckModBitRate(u32 &rate);
	int QueryModBitRate(u8 trynum = 0);
    int SetModClkSrc(u32 src, u8 trynum = 0);                  //设置发送时钟源
    int CheckModClkSrc(u32 &src);
	int QueryModClkSrc(u8 trynum = 0);
    int SetModFreq(u32 freq, u8 trynum = 0);                   //设置发送频点
    int CheckModFreq(u32 &freq);
	int QueryModFreq(u8 trynum = 0);
    int SetModOutput(BOOL32 enable, BOOL32 reset = FALSE, u8 trynum = 0); //设置发送是否开始
    int QueryModOutput(u8 trynum = 0);                         //查询发送是否开始
    int CheckModOutput(BOOL32 &enable);
    int SetModBrFr(u32 rate, u32 freq, u8 trynum = 0);			   //设置发送码率和频点
    int CheckModBrFr(u32 &rate, u32 &freq);

    void SetModemIp(u32 ip, u16 port); 	                       //设置IP和Port
    void SetModemType(u8 type);

private:
    ModemCmdProxy(ModemCmdProxy&) {};
    bool isvalid();                                            //检查所有参数是否满足
    u32 ip;
    u16 port;
    u8  txtype;                                                // 发送类型：1 - 70M; 2 - LBAND
    u8  rxtype;
    u32 txdefault;                                             // 默认频率
};


API int  MdcomInit		(void);								   //启动模块
API void MdcomTerm		(void);								   //停止模块
API void MdcomReport	(void);								   //状态报告
API void MdcomMsgLog	(void);							       //消息跟踪
API int  MdcomCommand	(const char* cmd, const char* ip, u16 port, u32 value); 
API int  MdcomCheck		(const char* cmd, const char* ip, u16 port);


#endif