#ifndef __DATA_SWITCH_H__
#define __DATA_SWITCH_H__

#include "osp.h"

#define DSVERSION       "Dataswitch 36.10.06.06.051209" 

/* DataSwitch 句柄 */
#define DSID    u32

/*DataSwitch 返回值*/
#define DSOK    1
#define DSERROR 0

#define INVALID_DSID  0xffffffff   /*无效的句柄值*/
#define DSERR_INVALIDID    INVALID_DSID          /*无效的标识，该值在dsCreate创建失败时返回*/

typedef struct tagTDSSpyMsg
{
	unsigned long  dwSrcIP;    /*网络序*/
	unsigned long  dwDstIP;    /*网络序*/
	unsigned short wDstPort;  /*网络序*/
	unsigned short wSrcPort;  /*网络序*/
	char data[16];
}TDSSpyMsg;

typedef u32 (*FilterFunc)(u32 dwRecvIP, u16 wRecvPort, u32 dwSrcIP, u16 wSrcPort, u8* pData, u32 uLen);


#define SENDMEM_MAX_MODLEN        (u8)32        // 发送时允许的最大修改长度
    
/**
 * 数据包转发目标：指明具体的IP, port对
 * DS所控制的多个IP，对于数据包的转发应该不产生影响。也就是说，如果
 * 有两个IP都可以将一个数据包正确的地转发出去，则使用哪一个IP是没有
 * 关系的。所以，转发输出接口IP是不需要的。
 */
typedef struct tagNetSndMember
{
	unsigned long   dwIP;        /* 转发目的IP地址   */
	unsigned short  wPort;       /* 转发目的端口号   */
	long            lIdx;        /* 接口索引 */
	unsigned long   errNum;      /* 出错次数*/
	long            errNo;       /* 错误号 */
    void *          lpuser;      /* user info */
    /* 为实现发送时修改数据而增加的部分 */
    unsigned short  wOffset;        /* 修改部分的起始位置; 目前未使用 */
    unsigned short  wLen;		/* 修改部分的长度，必须为4的倍数；为零表示不修改 */
    char            pNewData[SENDMEM_MAX_MODLEN];/* 要修改的数据 */
    void *          pAppData;       /* 用户自定义的回调函数的参数 */
}TNetSndMember;

typedef void (*SendCallback)(u32 dwRecvIP, u16 wRecvPort,
                             u32 dwSrcIP,  u16 wSrcPort,
                             TNetSndMember *ptSends,            // 转发目标列表
                             u16* pwSendNum,                    // 转发目标个数 
                             u8* pUdpData, u32 dwUdpLen);

/**********************************************************
 * 函数: dsSetCapacity                                    *
 * 功能: 容量设置(只能在创建交换模块之前调用)             *
 * 输入: dwMaxRcvGrp        - 最大接收组数                *
 *       dwMaxSndMmbPerRcvGrp  - 每一接收组最大发送组数   *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/
API u32 dsSetCapacity( u32 dwMaxRcvGrp = 256 , u32 dwMaxSndMmbPerRcvGrp = 128 );

/**********************************************************
 * 函数: dsCreate                                         *
 * 功能: 创建交换模块                                     *
 * 输入: num     - 接口IP数组个数                         *
 *       dwIP[]  - 接口IP数组                             *
 * 输出: 无                                               *
 * 返回: 成功返回不等于INVALID_DSID的值                   *
 *       否则返回 INVALID_DSID                            *
 **********************************************************/
API DSID dsCreate( u32 num ,u32 dwIP[]);

/**********************************************************
 * 函数: dsDestroy                                        *
 * 功能: 销毁交换模块                                     *
 * 输入: dsID    - 创建交换模块时的句柄                   *
 * 输出: 无                                               *
 * 返回: 无                                               *
 **********************************************************/
API void dsDestroy( DSID dsId );
 
/*********************************************************
 * 函数: dsRegSpy                                         *
 * 功能: 注册监视实例                                     *
 * 输入: dsID     - 创建交换模块时的句柄                  *
 *       dwInstId - 实例ID                                *
 *       wEvent   - 通知实例的消息                        *
 *       wUDPDataLen - 监听的UDP包数据长度(须小于16)      *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 *                                                        *
 * 注：该函数只能有一个使用者调用                         *
 **********************************************************/
API u32  dsRegSpy( DSID dsId , u32 dwInstId , u16 wEvent,u16 wUDPDataLen);
 
 
 /*********************************************************
 * 函数: dsUnRegSpy                                       *
 * 功能: 清除嗅听注册                                     *
 * 输入: dsID     - 创建交换模块时的句柄                  *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 *                                                        *
 * 注：该函数只能有一个使用者调用                         *
 **********************************************************/
API u32  dsUnRegSpy( DSID dsId);


 /*********************************************************
 * 函数: dsAddSpy                                         *
 * 功能: 增加一路嗅听                                     *
 * 输入: dsId        - 创建交换模块时的句柄               *
 *       dwLocalIP   - 本地接收IP                         *
 *       wLocalPort  - 本地接收端口号                     *
 *       dwLocalIfIP - 本地接收接口IP                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/ 
API u32  dsAddSpy( DSID dsId ,u32 dwLocalIP ,u16 wLocalPort ,u32 dwIfIP);

 /*********************************************************
 * 函数: dsRemvoeSpy                                      *
 * 功能: 清除一路嗅听                                     *
 * 输入: dsId        - 创建交换模块时的句柄               *
 *       dwLocalIP   - 本地接收IP                         *
 *       wLocalPort  - 本地接收端口号                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/ 
API u32  dsRemvoeSpy( DSID dsId , u32 dwLocalIP ,u16 wLocalPort );
 
     
/**********************************************************
 * 函数: dsAddDump                                        *
 * 功能: 增加垃圾节点                                     *
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwLocalIp   - 本地接收IP                         *
 *       wLocalPort  - 本地接收端口号                     *
 *       dwLocalIfIP - 本地接收接口IP                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/   
API u32 dsAddDump( DSID dsId ,u32 dwLocalIP ,u16 wLocalPort ,u32 dwLocalIfIP);

/**********************************************************
 * 函数: dsRemoveDump                                     *
 * 功能: 删除垃圾节点                                     *
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwLocalIP   - 本地接收IP                         *
 *       wLocalPort  - 本地接收端口号                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/   
API u32 dsRemoveDump( DSID dsId ,u32 dwLocalIP ,u16 wLocalPort);

/**********************************************************
 * 函数: dsGetRecvPktCount                                *
 * 功能: 查询接收总包数                                   *
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwLocalIP   - 本地接收IP                         *
 *       wLocalPort  - 本地接收端口号                     *
 *       dwSrcIP     - 源IP                               *
 *       wSrcPort    - 源端口号                           *
 *       dwRecvPktCount  - 接收总包数                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/    
API u32 dsGetRecvPktCount( DSID dsId , u32 dwLocalIP , u16 wLocalPort ,
						  u32 dwSrcIP , u16 wSrcPort , u32& dwRecvPktCount );

/**********************************************************
 * 函数: dsGetSendPktCount                                *
 * 功能: 查询发送总包数                                   *
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwLocalIP   - 本地接收IP                         *
 *       wLocalPort  - 本地接收端口号                     *
 *       dwSrcIP     - 源IP                               *
 *       wSrcPort    - 源端口号                           *
 *       dwDstIP     - 转发目的IP地址                     *
 *       wDstPort    - 转发目的端口号                     *
 *       dwSendPktCount  - 发送总包数                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/    
API u32 dsGetSendPktCount( DSID dsId , u32 dwLocalIP , u16 wLocalPort ,
						  u32 dwSrcIP , u16 wSrcPort ,
						  u32 dwDstIP, u16 wDstPort, u32& dwSendPktCount );

/**********************************************************
 * 函数: dsGetRecvBytesCount	                          *
 * 功能: 查询接收总字节数                                 *
 * 输入:												  *
 * 输出: 无                                               *
 * 返回: 返回即时接收字节数								  *
 **********************************************************/
API u32 dsGetRecvBytesCount( );

/**********************************************************
 * 函数: dsGetSendBytesCount	                          *
 * 功能: 查询发送总字节数                                 *
 * 输入:												  *
 * 输出: 无                                               *
 * 返回: 返回即时字节数									  *
 **********************************************************/
API u32 dsGetSendBytesCount( );

/**********************************************************
 * 函数: dsAdd                                            *
 * 功能: 增加交换节点                                     *
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwLocalIP   - 本地接收IP                         *
 *       wLocalPort  - 本地接收端口号                     *
 *       dwLocalIfIP - 本地接收接口IP                     *
 *       dwDstIP     - 转发目的IP地址                     *
 *       wDstPort    - 转发目的端口号                     *
 *   	 dwDstIfIP   - 转发目的接口IP                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/    
API u32 dsAdd(DSID dsId ,
               u32  dwLocalIP ,
               u16  wLocalPort,
               u32  dwLocalIfIP,
               u32  dwDstIP,
               u16  wDstPort,
    		   u32  dwDstOutIfIP);


/**********************************************************
 * 函数: dsRemove                                         *
 * 功能: 删除交换节点                                     *
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwDstIP     - 转发目的IP地址                     *
 *       wDstPort    - 转发目的端口号                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/    
API u32 dsRemove(DSID dsId,
                  u32  dwDstIP,
                  u16  wDstPort);

/**********************************************************
 * 函数: dsRemoveAll                                      *
 * 功能: 删除所有交换                                     *
 * 输入: dsID        - 创建交换模块时的句柄               *
 * 输出: 无                                               *
 * 返回: 无                                               *
 **********************************************************/    
API u32 dsRemoveAll( DSID dsId );

/**********************************************************
 * 函数: dsAddManyToOne                                   *
 * 功能: 增加交支持多点交换到一点的交换                   *
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwLocalP    - 本地接收IP                         *
 *       wLocalPort  - 本地接收端口号                     *
 *       dwLocalIfIP - 本地接收接口IP                     *
 *       dwDstIIP    - 转发目的IP地址                     *
 *       wDstPort    - 转发目的端口号                     *
 *   	 dwDstOutIfIP- 转发目的接口IP                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/   
API u32 dsAddManyToOne(DSID dsId ,
                        u32  dwLocalIP,
                        u16  wLocalPort,
        	            u32  dwLocalIfIP,
                        u32  dwDstIP,
                        u16  wDstPort,
    		            u32  dwDstOutIfIP);
/**********************************************************
 * 函数: dsRemoveAllManyToOne                             *
 * 功能: 删除所有交换到指定目的节点的交换                 *
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwDstIP     - 转发目的IP地址                     *
 *       wDstPort    - 转发目的端口号                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/        
API u32 dsRemoveAllManyToOne(DSID dsId ,
        	                 u32  dwDstIP,
    	                     u16  wDstPort);

/**********************************************************
 * 函数: dsRemoveManyToOne                                *
 * 功能: 删除指定的交换节点                               *
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwLocalIP   - 本地接收IP                         *
 *       wLocalPort  - 本地接收端口号                     *
 *       dwDstIP     - 转发目的IP地址                     *
 *       wDstPort    - 转发目的端口号                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/   
API u32 dsRemoveManyToOne(DSID dsId ,
    	                  u32  dwLocalIP,
                          u16  wLocalPort,
                          u32  dwDstIP,
                          u16  wDstPort  );

/**********************************************************
 * 函数: dsAddSrcSwitch                                   *
 * 功能: 增加根据UDP包的源IP、Port交换的点                *
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwLocalIp   - 本地接收IP                         *
 *       wLocalPort  - 本地接收端口号                     *
 *       dwLocalIfIP - 本地接收接口IP                     *
 *       dwSrcIP     - 源IP                               *
 *       wSrcPort    - 源端口号                           *
 *       dwDstIP     - 转发目的IP地址                     *
 *       wDstPort    - 转发目的端口号                     *
 *   	 dwDstOutIfIP- 转发目的接口IP                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 *                                                        *
 * 注: 该方式为支持多点到一点交换                         *
 *     该函数将dwLocalIP@wLocalPort 收到的源IP/PORT为     *
 * dwSrcIP@wSrcPort的UDP数据包转发到dwDstIP@wDstPort      *
 **********************************************************/        
API u32 dsAddSrcSwitch(DSID dsId ,
                       u32  dwLocalIP,
                       u16  wLocalPort,
        	           u32  dwLoalIfIP,
                       u32  dwSrcIP ,
                       u16  wSrcPort,
                       u32  dwDstIP,
                       u16  wDstPort,
    		           u32  dwDstOutIfIP);

/**********************************************************
 * 函数: dsRemoveAllSrcSwitch                             *
 * 功能: 删除所有根据源IP、Port交换到目的节点的交换       *
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwLocalIp   - 本地接收IP                         *
 *       wLocalPort  - 本地接收端口号                     *
 *       dwSrcIP - 转发目的IP地址                         *
 *       wSrcPort    - 转发目的端口号                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/            
API u32 dsRemoveAllSrcSwitch(DSID dsId,
                              u32  dwLocalIP,
                              u16  wLocalPort,
                              u32  dwSrcIP,
                              u16  wSrcPort);

/**********************************************************
 * 函数: dsRemoveSrcSwitch                                *
 * 功能: 删除指定的根据源IP、Port交换到目的节点的交换     *
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwSrcIP     - 源IP                               *
 *       wSrcPort    - 源端口号                           *
 *       dwDstIP - 转发目的IP地址                         *
 *       wDstPort    - 转发目的端口号                     *
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/            
API u32 dsRemoveSrcSwitch(DSID dsId,
                          u32  dwLocalIP,
                          u16  wLocalPort,
      	                  u32  dwSrcIP,
    	                  u16  wSrcPort,
                          u32  dwDstIP,
                          u16  wDstPort);


/**********************************************************
 * 函数: dsSpecifyFwdSrc                                  *
 * 功能: 为指定接收地址设置转发数据包所填充的源地址       *
 * 输入:
 * @param dsId          - DSID
 * @param OrigIP        - 原始IP
 * @param OrigPort      - 原始Port
 * @param MappedIP      - 映射IP
 * @param MappedPort    - 映射Port
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/
API u32 dsSpecifyFwdSrc(DSID dsId, u32 OrigIP, u16 OrigPort, u32 MappedIP, u16 MappedPort);

/**********************************************************
 * 函数: dsResetFwdSrc                                    *
 * 功能: 恢复指定地址转发数据包的源地址
 * 输入:
 * @param dsId          - DSID
 * @param OrigIP        - 原始IP
 * @param OrigPort      - 原始Port
 * 输出: 无                                               *
 * 返回: 成功返回DSOK 否则DSERROR                         *
 **********************************************************/
API u32 dsResetFwdSrc(DSID dsId, u32 OrigIP, u16 OrigPort);


 /**********************************************************
 * 函数: dsSetSSRCChange                                  *
 * 功能: 设置对码流中SSRC作改动                           *
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwIP     - IP                                    *
 *       wPort    - 端口号                                *
 * 输出: 无                                               *
 * 返回: 无                                               *
 **********************************************************/    
 API u32 dsSetSSRCChange( DSID dsId, u32  dwIP, u16  wPort );

/**********************************************************
 * 函数: dsCancelSSRCChange                                  *
 * 功能: 取消对码流中SSRC作改动                           *
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwIP     - IP                                    *
 *       wPort    - 端口号                                *
 * 输出: 无                                               *
 * 返回: 无                                               *
 **********************************************************/    
 API u32 dsCancelSSRCChange( DSID dsId, u32  dwIP, u16  wPort );

 /**********************************************************
 * 函数: dsSetFilterFunc                                  *
 * 功能: 设置过滤函数                                     *
 *       取消时，设置函数指针为NULL即可
 * 输入: dsID        - 创建交换模块时的句柄               *
 *       dwIP     - IP                                    *
 *       wPort    - 端口号                                *
 *       pfFilter - 函数指针
 * 输出: 无                                               *
 * 返回: 无                                               *
 **********************************************************/    
API u32 dsSetFilterFunc( DSID dsId, u32  dwIP, u16  wPort , FilterFunc pfFilter);

 /**********************************************************
 * 函数: dsSetSendCallback                                *
 * 功能: 设置发送回调函数                                 *
 *       取消时，设置函数指针为NULL即可
 * 输入: dsID           - 创建交换模块时的句柄            *
 *       dwLocalIp      - 本地接收IP                      *
 *       wLocalPort     - 本地接收端口号                  *
 *       dwSrcIP        - 转发目的IP地址                  *
 *       wSrcPort       - 转发目的端口号                  *
 *       pfCallback     - 回调函数
 * 输出: 无                                               *
 * 返回: 无                                               *
 **********************************************************/    
API u32 dsSetSendCallback( DSID dsId, 
                u32 dwLocalIP, 
                u16 wLocalPort,
                u32 dwSrcIP,
                u16 wSrcPort,
                SendCallback pfCallback);

 /**********************************************************
 * 函数: dsSetAppDataForSend                              *
 * 功能: 为发送目标设置一个自定义的指针                   *
 *       取消时，设置函数指针为NULL即可
 * 输入: dsID           - 创建交换模块时的句柄            *
 *       dwLocalIp      - 本地接收IP                      *
 *       wLocalPort     - 本地接收端口号                  *
 *       dwSrcIP        - 源IP地址                  *
 *       wSrcPort       - 源端口号                  *
 *       dwDstIP        - 转发目的IP地址                  *
 *       wDstPort       - 转发目的端口号                  *
 *       pAppData       - 自定义指针
 * 输出: 无                                               *
 * 返回: 
 *     DSOK:成功 DSERROR:失败                             *
 **********************************************************/    
API u32 dsSetAppDataForSend( DSID dsId, 
                u32 dwLocalIP, 
                u16 wLocalPort,
                u32 dwSrcIP,
                u16 wSrcPort,
                u32 dwDstIP,
                u16 wDstPort,
                void * pAppData);

 ////////////////////////////////////////
API void dsDbg();
API void dsver(void);
API void dshelp(void);

#endif/*!__DATA_SWITCH_H__*/
